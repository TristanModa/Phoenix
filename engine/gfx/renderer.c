#include "renderer.h"

#include <stdlib.h>

static RendererState rendererState;

void Renderer_create() {
	Logger_info("Creating renderer subsystem...");

	// Create the GPU device
	rendererState.gpuDevice = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
	if (!rendererState.gpuDevice) {
		Logger_fatal("Failed to create GPU device: SDL Error: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Claim the window for the GPU device
	if (!SDL_ClaimWindowForGPUDevice(rendererState.gpuDevice, Window_getHandle())) {
		Logger_fatal("Failed to claim window for GPU device: SDL Error: %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}

}

void Renderer_destroy() {
	Logger_info("Destroying renderer subsystem...");

	// Release associated resources
	SDL_ReleaseWindowFromGPUDevice(rendererState.gpuDevice, Window_getHandle());
	SDL_DestroyGPUDevice(rendererState.gpuDevice);
}

void Renderer_render() {

}