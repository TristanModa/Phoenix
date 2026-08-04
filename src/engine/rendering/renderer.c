#include "renderer.h"

#include <dcimgui.h>
#include <backends/dcimgui_impl_sdlgpu3.h>
#include <stdlib.h>

#include "colors.h"
#include "engine/core/logger.h"
#include "engine/collections/arrayList.h"
#include "engine/core/resources.h"

static struct {
    SDL_GPUDevice* gpuDevice;
    SDL_Window* windowHandle;

    // TODO: Wrap in anon struct?
    SDL_GPUGraphicsPipeline* debugLinePipeline;
    SDL_GPUTransferBuffer* debugLineTransferBuffer;
    SDL_GPUBuffer* debugLineBuffer;
} rendererState;

typedef struct debugLine {
    float x1, y1, x2, y2;
    float r, g, b, a;
} DebugLine;

static SDL_GPUShader* createShader(ResourceID resourceID, SDL_GPUShaderStage stage);

static void uploadDebugLineRenderData(SDL_GPUCommandBuffer* commandBuffer);
static void uploadImGuiRenderData(SDL_GPUCommandBuffer* commandBuffer);

static void renderDebugLines(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass);
static void renderImGui(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass);

void Renderer_create(SDL_Window* windowHandle) {
    Logger_info("Creating renderer...");
    Logger_pushIndent();

    // Initialize renderer state
    rendererState = (typeof(rendererState)){
        .windowHandle = windowHandle
    };

    // Create the GPU device
#ifdef DEBUG
    constexpr bool GPU_DEBUG = true;
#else
    constexpr bool GPU_DEBUG = false;
#endif
    rendererState.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, GPU_DEBUG, nullptr);
    if (!rendererState.gpuDevice) {
        Logger_fatal("Failed to create GPU device: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Claim the window for the GPU device
    if (!SDL_ClaimWindowForGPUDevice(rendererState.gpuDevice, rendererState.windowHandle)) {
        Logger_fatal("Failed to claim window for GPU device: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create shaders
    SDL_GPUShader* debugLineVS = createShader(SHADERS__DEBUG_LINE_VERT, SDL_GPU_SHADERSTAGE_VERTEX);
    SDL_GPUShader* debugLineFS = createShader(SHADERS__DEBUG_LINE_FRAG, SDL_GPU_SHADERSTAGE_FRAGMENT);
    if (!debugLineVS || !debugLineFS) {
        Logger_fatal("Failed to create one or more shaders: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Create the debug line pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .target_info = {
            .num_color_targets = 1,
            .color_target_descriptions = (SDL_GPUColorTargetDescription[]) {{
                .format = SDL_GetGPUSwapchainTextureFormat(rendererState.gpuDevice, rendererState.windowHandle),
            }},
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .vertex_shader = debugLineVS,
        .fragment_shader = debugLineFS,
    };
    rendererState.debugLinePipeline = SDL_CreateGPUGraphicsPipeline(rendererState.gpuDevice, &pipelineCreateInfo);
    if (!rendererState.debugLinePipeline) {
        Logger_fatal("Failed to create debug line pipeline: SDL error: %s", SDL_GetError());
    }

    // Create the debug line vertex buffer
    rendererState.debugLineBuffer = SDL_CreateGPUBuffer(
        rendererState.gpuDevice,
        &(SDL_GPUBufferCreateInfo){
            .usage = SDL_GPU_BUFFERUSAGE_GRAPHICS_STORAGE_READ,
            .size = sizeof(DebugLine) * DEBUG_LINE_BUFFER_SIZE
        });

    // Release shaders
    SDL_ReleaseGPUShader(rendererState.gpuDevice, debugLineVS);
    SDL_ReleaseGPUShader(rendererState.gpuDevice, debugLineFS);

    Logger_popIndent();
}

void Renderer_destroy() {
    SDL_WaitForGPUIdle(rendererState.gpuDevice);
    SDL_ReleaseGPUGraphicsPipeline(rendererState.gpuDevice, rendererState.debugLinePipeline);
    SDL_DestroyGPUDevice(rendererState.gpuDevice);
}

void Renderer_render() {
    // Acquire the command buffer
    SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(rendererState.gpuDevice);
    if (!commandBuffer) {
        Logger_fatal("Failed to acquire GPU command buffer: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Acquire the swapchain texture
    SDL_GPUTexture* swapchainTexture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
        commandBuffer, rendererState.windowHandle, &swapchainTexture, nullptr, nullptr)) {
        Logger_fatal("Failed to acquire swapchain texture: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Render to the swapchain texture if it is not null
    if (swapchainTexture != nullptr) {
        // Upload render data
        uploadDebugLineRenderData(commandBuffer);
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
        renderDebugLines(commandBuffer, renderPass);
        renderImGui(commandBuffer, renderPass);

        // End the render pass
        SDL_EndGPURenderPass(renderPass);
    }

    // Submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
}

SDL_GPUDevice* Renderer_getGPUDevice() {
    return rendererState.gpuDevice;
}

SDL_GPUShader* createShader(ResourceID resourceID, SDL_GPUShaderStage stage) {
    size_t shaderCodeSize;
    void* shaderCode = Resources_get(resourceID, &shaderCodeSize);
    SDL_GPUShaderCreateInfo createInfo = {
        .code_size = shaderCodeSize,
        .code = shaderCode,
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = stage,
    };
    return SDL_CreateGPUShader(rendererState.gpuDevice, &createInfo);
}

void uploadDebugLineRenderData(SDL_GPUCommandBuffer* commandBuffer) {
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(commandBuffer);

    DebugLine* dataPtr = SDL_MapGPUTransferBuffer(rendererState.gpuDevice, , true);
    for (size_t i = 0; i < DEBUG_LINE_BUFFER_SIZE; i++) {
        dataPtr[i] = (DebugLine){
            .x1 = (float)SDL_rand(1000) / 500.0f - 0.5f,
            .y1 = (float)SDL_rand(1000) / 500.0f - 0.5f,
            .x2 = (float)SDL_rand(1000) / 500.0f - 0.5f,
            .y2 = (float)SDL_rand(1000) / 500.0f - 0.5f,
            .r = (float)SDL_rand(255) / 255.0f,
            .g = (float)SDL_rand(255) / 255.0f,
            .b = (float)SDL_rand(255) / 255.0f,
            .a = 1.0f,
        };
    }

    SDL_EndGPUCopyPass(copyPass);
}

void uploadImGuiRenderData(SDL_GPUCommandBuffer* commandBuffer) {
    ImDrawData* imDrawData = ImGui_GetDrawData();
    cImGui_ImplSDLGPU3_PrepareDrawData(imDrawData, commandBuffer);
}

void renderDebugLines(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass) {
    // Render debug lines
    SDL_BindGPUGraphicsPipeline(renderPass, rendererState.debugLinePipeline);
    SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);
}

void renderImGui(SDL_GPUCommandBuffer* commandBuffer, SDL_GPURenderPass* renderPass) {
    ImDrawData* imDrawData = ImGui_GetDrawData();
    cImGui_ImplSDLGPU3_RenderDrawData(imDrawData, commandBuffer, renderPass);
}