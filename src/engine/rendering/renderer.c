#include "renderer.h"

#include <dcimgui.h>
#include <backends/dcimgui_impl_sdlgpu3.h>
#include <stdlib.h>

#include "colors.h"
#include "engine/core/logger.h"
#include "engine/core/resources.h"
#include "engine/core/vtime.h"

typedef struct debugLine {
    float x1, y1, x2, y2;
    Color color;
} DebugLine;

typedef struct debugLineVertex {
    float x, y;
    float r, g, b, a;
} DebugLineVertex;

static struct {
    SDL_GPUDevice* gpuDevice;
    SDL_Window* windowHandle;

    struct {
        size_t lineCount;
        DebugLine lines[MAX_DEBUG_LINE_COUNT];

        SDL_GPUGraphicsPipeline* pipeline;
        SDL_GPUTransferBuffer* transferBuffer;
        SDL_GPUBuffer* gpuBuffer;
    } debugLineRenderer;
} renderState;

static SDL_GPUShader* createShader(ResourceID resourceID, SDL_GPUShaderStage stage);

static void createDebugLinesRenderer();
static void destroyDebugLinesRenderer();
static void uploadDebugLineData(SDL_GPUCommandBuffer* commandBuffer);
static void renderDebugLines(SDL_GPURenderPass* renderPass);

static void uploadImGuiRenderData(SDL_GPUCommandBuffer* commandBuffer);
static void renderImGui(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass);

void Renderer_create(SDL_Window* windowHandle) {
    Logger_info("Creating renderer...");
    Logger_pushIndent();

    // Initialize renderer state
    renderState = (typeof(renderState)){
        .windowHandle = windowHandle
    };

    // Create the GPU device
#ifdef DEBUG
    constexpr bool GPU_DEBUG = true;
#else
    constexpr bool GPU_DEBUG = false;
#endif
    renderState.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, GPU_DEBUG, nullptr);
    if (!renderState.gpuDevice) {
        Logger_fatal("Failed to create GPU device: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Claim the window for the GPU device
    if (!SDL_ClaimWindowForGPUDevice(renderState.gpuDevice, renderState.windowHandle)) {
        Logger_fatal("Failed to claim window for GPU device: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the debug lines renderer
    createDebugLinesRenderer();

    Logger_popIndent();
}

void Renderer_destroy() {
    SDL_WaitForGPUIdle(renderState.gpuDevice);
    destroyDebugLinesRenderer();
    SDL_DestroyGPUDevice(renderState.gpuDevice);
}

void Renderer_render() {
    // Acquire the command buffer
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(renderState.gpuDevice);
    if (!commandBuffer) {
        Logger_fatal("Failed to acquire GPU command buffer: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Acquire the swapchain texture
    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
        commandBuffer, renderState.windowHandle, &swapchainTexture, nullptr, nullptr)) {
        Logger_fatal("Failed to acquire swapchain texture: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Render to the swapchain texture if it is not null
    if (swapchainTexture != nullptr) {
        // Upload render data
        uploadDebugLineData(commandBuffer);
        uploadImGuiRenderData(commandBuffer);

        // Create the render pass
        const SDL_GPUColorTargetInfo colorTargetInfo = {
            .texture = swapchainTexture,
            .clear_color = COLOR_BLACK,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);

        // Render
        renderDebugLines(renderPass);
        renderImGui(commandBuffer, renderPass);

        // End the render pass
        SDL_EndGPURenderPass(renderPass);
    }

    // Submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
}

SDL_GPUDevice* Renderer_getGPUDevice() {
    return renderState.gpuDevice;
}

void Renderer_drawDebugLine(float x1, float y1, float x2, float y2, Color color) {
    // Return if the lines array is full
    if (renderState.debugLineRenderer.lineCount >= MAX_DEBUG_LINE_COUNT) {
        Logger_warning("Could not draw debug line: maximum debug line count of %zu reached", MAX_DEBUG_LINE_COUNT);
        return;
    }

    // Add the line to the lines array
    renderState.debugLineRenderer.lines[renderState.debugLineRenderer.lineCount] = (DebugLine){
        .x1 = x1,
        .y1 = y1,
        .x2 = x2,
        .y2 = y2,
        .color = color
    };
    renderState.debugLineRenderer.lineCount++;
}

SDL_GPUShader* createShader(ResourceID resourceID, SDL_GPUShaderStage stage) {
    size_t shaderCodeSize;
    void* shaderCode = Resources_get(resourceID, &shaderCodeSize);
    const SDL_GPUShaderCreateInfo createInfo = {
        .code_size = shaderCodeSize,
        .code = shaderCode,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = stage,
    };
    return SDL_CreateGPUShader(renderState.gpuDevice, &createInfo);
}

void createDebugLinesRenderer() {
    // Create shaders
    SDL_GPUShader* vertexShader = createShader(SHADERS__DEBUG_LINE_VERT, SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* fragmentShader = createShader(SHADERS__DEBUG_LINE_FRAG, SDL_GPU_SHADERSTAGE_FRAGMENT);
    if (!vertexShader || !fragmentShader) {
        Logger_fatal("Failed to create one or more shaders: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the debug line pipeline
    const SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]) {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderState.gpuDevice, renderState.windowHandle),
            }},
        },
        .vertex_input_state = {
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = (SDL_GPUVertexBufferDescription[]) {{
                .slot = 0,
                .pitch = sizeof(DebugLineVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_attributes = 2,
            .vertex_attributes = (SDL_GPUVertexAttribute[]) {
                {
                    .location = 0,
                    .buffer_slot = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                    .offset = offsetof(DebugLineVertex, x),
                },
                {
                    .location = 1,
                    .buffer_slot = 0,
                    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                    .offset = offsetof(DebugLineVertex, r),
                }
            },
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_LINELIST,
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
    };
    renderState.debugLineRenderer.pipeline = SDL_CreateGPUGraphicsPipeline(renderState.gpuDevice, &pipelineCreateInfo);
    if (!renderState.debugLineRenderer.pipeline) {
        Logger_fatal("Failed to create debug line pipeline: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Release shaders
    SDL_ReleaseGPUShader(renderState.gpuDevice, vertexShader);
    SDL_ReleaseGPUShader(renderState.gpuDevice, fragmentShader);

    // Create the debug line transfer buffer
    renderState.debugLineRenderer.transferBuffer = SDL_CreateGPUTransferBuffer(
        renderState.gpuDevice,
        &(SDL_GPUTransferBufferCreateInfo){
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = sizeof(DebugLineVertex) * MAX_DEBUG_LINE_COUNT * 2
        });
    if (!renderState.debugLineRenderer.transferBuffer) {
        Logger_fatal("Failed to create debug line transfer buffer: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the debug line vertex buffer
    renderState.debugLineRenderer.gpuBuffer = SDL_CreateGPUBuffer(
        renderState.gpuDevice,
        &(SDL_GPUBufferCreateInfo){
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = sizeof(DebugLineVertex) * MAX_DEBUG_LINE_COUNT * 2
        });
    if (!renderState.debugLineRenderer.gpuBuffer) {
        Logger_fatal("Failed to create debug line GPU buffer: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

void destroyDebugLinesRenderer() {
    SDL_ReleaseGPUBuffer(renderState.gpuDevice, renderState.debugLineRenderer.gpuBuffer);
    SDL_ReleaseGPUTransferBuffer(renderState.gpuDevice, renderState.debugLineRenderer.transferBuffer);
    SDL_ReleaseGPUGraphicsPipeline(renderState.gpuDevice, renderState.debugLineRenderer.pipeline);
}

void uploadDebugLineData(SDL_GPUCommandBuffer* commandBuffer) {
    // Upload the debug line data to the transfer buffer
    DebugLineVertex* dataPtr = SDL_MapGPUTransferBuffer(
        renderState.gpuDevice,
        renderState.debugLineRenderer.transferBuffer,
        true);
    for (size_t i = 0; i < renderState.debugLineRenderer.lineCount; i++) {
        const DebugLine* line = &renderState.debugLineRenderer.lines[i];
        dataPtr[i * 2] = (DebugLineVertex){
            .x = line->x1,
            .y = line->y1,
            .r = line->color.r,
            .g = line->color.g,
            .b = line->color.b,
            .a = line->color.a,
        };
        dataPtr[i * 2 + 1] = (DebugLineVertex){
            .x = line->x2,
            .y = line->y2,
            .r = line->color.r,
            .g = line->color.g,
            .b = line->color.b,
            .a = line->color.a,
        };
    }
    SDL_UnmapGPUTransferBuffer(renderState.gpuDevice, renderState.debugLineRenderer.transferBuffer);

    // Copy the transfer buffer to the GPU buffer
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);
    SDL_UploadToGPUBuffer(
        copyPass,
        &(SDL_GPUTransferBufferLocation) {
            .transfer_buffer = renderState.debugLineRenderer.transferBuffer,
            .offset = 0
        },
        &(SDL_GPUBufferRegion) {
            .buffer = renderState.debugLineRenderer.gpuBuffer,
            .offset = 0,
            .size = sizeof(DebugLineVertex) * renderState.debugLineRenderer.lineCount * 2
        },
        true);
    SDL_EndGPUCopyPass(copyPass);
}

void renderDebugLines(SDL_GPURenderPass* renderPass) {
    // Draw lines
    SDL_BindGPUGraphicsPipeline(renderPass, renderState.debugLineRenderer.pipeline);
    const SDL_GPUBufferBinding binding = {
        .buffer = renderState.debugLineRenderer.gpuBuffer,
        .offset = 0
    };
    SDL_BindGPUVertexBuffers(renderPass, 0, &binding, 1);
    SDL_DrawGPUPrimitives(renderPass, renderState.debugLineRenderer.lineCount * 2, 1, 0, 0);

    // Clear the line buffer
    renderState.debugLineRenderer.lineCount = 0;
}

void uploadImGuiRenderData(SDL_GPUCommandBuffer* commandBuffer) {
    ImDrawData* imDrawData = ImGui_GetDrawData();
    cImGui_ImplSDLGPU3_PrepareDrawData(imDrawData, commandBuffer);
}

void renderImGui(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass) {
    ImDrawData* imDrawData = ImGui_GetDrawData();
    cImGui_ImplSDLGPU3_RenderDrawData(imDrawData, commandBuffer, renderPass);
}