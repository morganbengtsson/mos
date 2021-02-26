#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <functional>

namespace mos::gfx::vulkan {
class Renderer {
public:
  Renderer(const int width, const int height, std::vector<const char*> extensions,
           const std::function<VkSurfaceKHR(VkInstance)> &surface_creator);
  ~Renderer();

private:
  std::vector<const char *> validation_layers_{"VK_LAYER_KHRONOS_validation"};
  std::vector<const char *> device_extensions_{VK_KHR_SWAPCHAIN_EXTENSION_NAME};
  vk::Instance instance_;

  VkDebugUtilsMessengerEXT debug_messenger_;
  VkSurfaceKHR surface_;

  VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
  vk::Device device_;

  VkQueue graphics_queue_;
  VkQueue present_queue_;

  VkSwapchainKHR swap_chain_;
  std::vector<VkImage> swap_chain_images_;
  VkFormat swap_chain_image_format_;
  VkExtent2D swap_chain_extent_;

};
}
