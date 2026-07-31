#include "renderer.h"

#include <dcimgui.h>
#include <backends/dcimgui_impl_sdlgpu3.h>
#include <stdlib.h>

#include "colors.h"
#include "engine/core/logger.h"
#include "engine/collections/arrayList.h"

static struct {
    SDL_GPUDevice* gpuDevice;
    SDL_Window* windowHandle;
} rendererState;

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

    Logger_popIndent();
}

void Renderer_destroy() {
    SDL_WaitForGPUIdle(rendererState.gpuDevice);
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
        // Prepare the ImGui draw data
        ImDrawData* imDrawData = ImGui_GetDrawData();
        cImGui_ImplSDLGPU3_PrepareDrawData(imDrawData, commandBuffer);

        // Create the render pass
        SDL_GPUColorTargetInfo colorTargetInfo = {
            .texture = swapchainTexture,
            .clear_color = COLOR_BLACK,
            .load_op = SDL_GPU_LOADOP_CLEAR,
            .store_op = SDL_GPU_STOREOP_STORE
        };
        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);

        // Render the ImGui draw data
        cImGui_ImplSDLGPU3_RenderDrawData(imDrawData, commandBuffer, renderPass);

        // End the render pass
        SDL_EndGPURenderPass(renderPass);
    }

    // Submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
}

SDL_GPUDevice* Renderer_getGPUDevice() {
    return rendererState.gpuDevice;
}