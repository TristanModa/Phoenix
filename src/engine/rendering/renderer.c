#include "renderer.h"

#include <stdlib.h>

#include "../core/logger.h"
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
    rendererState.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!rendererState.gpuDevice) {
        Logger_fatal("Failed to create GPU device: SDL error: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    Logger_popIndent();
}

void Renderer_destroy() {
    SDL_DestroyGPUDevice(rendererState.gpuDevice);
}

void Renderer_render() {

}