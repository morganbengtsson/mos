#include <cstring>
#include <algorithm>
#include <tuple>
#include <mos/gfx/vulkan/renderer.hpp>

namespace mos::gfx::vulkan {

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

namespace {


const std::vector<const char*> device_extensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


static VKAPI_ATTR VkBool32 VKAPI_CALL
debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
               VkDebugUtilsMessageTypeFlagsEXT message_type,
               const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
               void *user_data) {
  std::cerr << "validation layer: " << callback_data->pMessage << std::endl;

  return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}


bool check_validation_layer_support(
    const std::vector<const char *> validation_layers) {
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  for (const char *layer_name : validation_layers) {
    bool layerFound = false;

    for (const auto &layerProperties : available_layers) {
      if (strcmp(layer_name, layerProperties.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}

void populate_debug_messenger_create_info(
    VkDebugUtilsMessengerCreateInfoEXT &create_info) {
  create_info = {};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debug_callback;
}

vk::Instance create_instance(std::vector<const char *> extensions,
                           const std::vector<const char *> validation_layers) {
  if (enableValidationLayers &&
      !check_validation_layer_support(validation_layers)) {
    throw std::runtime_error("validation layers requested, but not available!");
  }

  vk::ApplicationInfo app_info{{.pApplicationName = "Hello Triangle",
                                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                .pEngineName = "No Engine",
                                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                .apiVersion = VK_API_VERSION_1_0}};

  vk::InstanceCreateInfo create_info;
  create_info.pApplicationInfo = &app_info;

  if (enableValidationLayers) {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  create_info.enabledExtensionCount = extensions.size();
  create_info.ppEnabledExtensionNames = extensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  if (enableValidationLayers) {
    create_info.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    create_info.ppEnabledLayerNames = validation_layers.data();

    populate_debug_messenger_create_info(debugCreateInfo);
    create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    create_info.enabledLayerCount = 0;

    create_info.pNext = nullptr;
  }

  vk::Instance instance = vk::createInstance(create_info);

  return instance;
}

VkDebugUtilsMessengerEXT create_debug_messenger(vk::Instance &instance) {
  VkDebugUtilsMessengerEXT debug_messenger;

  if (!enableValidationLayers)
    return debug_messenger;

  VkDebugUtilsMessengerCreateInfoEXT create_info;
  populate_debug_messenger_create_info(create_info);

  if (CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr,
                                   &debug_messenger) != VK_SUCCESS) {
    throw std::runtime_error("failed to set up debug messenger!");
  }
  return debug_messenger;
}

void destroy_debug_utils_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const VkSurfaceKHR &surface) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete()) {
      break;
    }

    i++;
  }

  return indices;
}

bool isDeviceSuitable(VkPhysicalDevice device, const VkSurfaceKHR &surface) {
  QueueFamilyIndices indices = findQueueFamilies(device, surface);

  return indices.isComplete();
}

VkPhysicalDevice pick_physical_device(const VkInstance &instance, const VkSurfaceKHR &surface) {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  VkPhysicalDevice physical_device;
  for (const auto &device : devices) {
    if (isDeviceSuitable(device, surface)) {
      physical_device = device;
      break;
    }
  }

  if (physical_device == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
  return physical_device;
}

std::tuple<VkDevice, VkQueue>
create_logical_device(const VkPhysicalDevice &physical_device,
                    const std::vector<const char *> validation_layers, const VkSurfaceKHR &surface) {
  QueueFamilyIndices indices = findQueueFamilies(physical_device, surface);

  VkDeviceQueueCreateInfo queueCreateInfo{};
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
  queueCreateInfo.queueCount = 1;

  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{};

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  createInfo.pQueueCreateInfos = &queueCreateInfo;
  createInfo.queueCreateInfoCount = 1;

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
  createInfo.ppEnabledExtensionNames = device_extensions.data();

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validation_layers.size());
    createInfo.ppEnabledLayerNames = validation_layers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  VkDevice device;
  if (vkCreateDevice(physical_device, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  VkQueue queue;
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &queue);
  return {device, queue};
}

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
  for (const auto& availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const int width, const int height) {
  if (capabilities.currentExtent.width != UINT32_MAX) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}


std::tuple<VkSwapchainKHR, std::vector<VkImage>,VkFormat, VkExtent2D> create_swap_chain(const VkDevice &device, const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface, const int width, const int height) {
  SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);

  VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, width, height);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  VkSwapchainCreateInfoKHR createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;

  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  createInfo.oldSwapchain = VK_NULL_HANDLE;

  VkSwapchainKHR swapChain;
  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
    throw std::runtime_error("failed to create swap chain!");
  }

  std::vector<VkImage> swapChainImages;
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

  VkFormat swapChainImageFormat = surfaceFormat.format;
  VkExtent2D swapChainExtent = extent;

  return {swapChain, swapChainImages, swapChainImageFormat, swapChainExtent};
}


} // namespace

Renderer::Renderer(const int width, const int height, std::vector<const char *> extensions, const std::function<VkSurfaceKHR(VkInstance)> &surface_creator)
    : instance_(create_instance(extensions, validation_layers_)),
      debug_messenger_(create_debug_messenger(instance_)),
      surface_(surface_creator(instance_)),
      physical_device_(pick_physical_device(instance_, surface_)) {
  std::tie(device_, graphics_queue_) = create_logical_device(physical_device_, validation_layers_, surface_);
  std::tie(swap_chain_, swap_chain_images_, swap_chain_image_format_,
           swap_chain_extent_) =
      create_swap_chain(device_, physical_device_, surface_, width, height);
}
Renderer::~Renderer() {
  vkDestroySwapchainKHR(device_, swap_chain_, nullptr);
  vkDestroyDevice(device_, nullptr);

  if (enableValidationLayers) {
    destroy_debug_utils_messenger(instance_, debug_messenger_, nullptr);
  }

  vkDestroySurfaceKHR(instance_, surface_, nullptr);
  vkDestroyInstance(instance_, nullptr);

}

} // namespace mos::gfx::vulkan
