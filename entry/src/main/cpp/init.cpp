
#include "napi/native_api.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <cstdint>
#include <hilog/log.h>
#include <native_window/external_window.h>
#include <optional>
#include <rawfile/raw_file_manager.h>
#include <set>
#include <string>
#include <vulkan/vulkan.h>

class HelloVK;

#define GOLOGE(X)                                                                                                      \
    do {                                                                                                               \
        OH_LOG_ERROR(LOG_APP, X);                                                                                      \
    } while (0)

struct ANativeWindowDeleter final {
    void operator()(OHNativeWindow *window) { OH_NativeWindow_DestroyNativeWindow(window); }
};

struct QueueFamilyIndices final {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails final {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class HelloVK {
public:
    void run() { initVulkan(); }
    bool m_initialized = false;
private:
    VkInstance instance;
    VkInstance m_instance = VK_NULL_HANDLE;
    NativeResourceManager *m_aAssetMgr = nullptr;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    NativeResourceManager *m_assetManager = nullptr;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures m_physicalDeviceFeatures;
    std::vector<char const *> const m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                          VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME};
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkExtent2D m_displaySizeIdentity{};
    uint32_t m_resolution = 720;
    std::unique_ptr<OHNativeWindow, ANativeWindowDeleter> m_window;
    VkPhysicalDeviceProperties m_physicalDeviceProperties;

    void initVulkan() {
        createInstance();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDeviceAndQueue();
        EstablishDisplaySizeIdentity();
        
        m_initialized = true;
    }

    void createInstance() {
        instance = VK_NULL_HANDLE;

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "vulkanExample";
        appInfo.pEngineName = "vulkanExample";
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pNext = NULL;
        instanceCreateInfo.pApplicationInfo = &appInfo;

        std::vector<const char *> instanceExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_OHOS_SURFACE_EXTENSION_NAME // Surface扩展
        };
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

        vkCreateInstance(&instanceCreateInfo, nullptr, &instance);

        /*        uint32_t extensionCount = 0;
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
                std::vector<VkExtensionProperties> extensions(extensionCount);
                vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());*/
    }
    
    void Reset(OHNativeWindow *newWindow, NativeResourceManager *newManager) {
        m_window.reset(newWindow);
        m_assetManager = newManager;
        m_aAssetMgr = newManager;
        
        if (m_initialized) {
            CreateSurface();
        }
    }

    void CreateSurface() {
        if (!m_window) {
            OH_LOG_ERROR(LOG_APP, "tujiaming m_window is null");
            return;
        }

        const VkSurfaceCreateInfoOHOS create_info{.sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS,
                                                  .pNext = nullptr,
                                                  .flags = 0,
                                                  .window = m_window.get()};

        vkCreateSurfaceOHOS(m_instance, &create_info, nullptr, &m_surface);
    }

    bool CheckDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

        for (const auto &extension : availableExtensions) {
            requiredExtensions.erase(std::string(extension.extensionName));
        }

        return requiredExtensions.empty();
    }
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount,
                                                      details.presentModes.data());
        }
        return details;
    }
    bool IsDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = FindQueueFamilies(device);
        bool extensionsSupported = CheckDeviceExtensionSupport(device);
        bool swapChainAdequate = false;
        if (extensionsSupported) {
            SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
            swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        return indices.isComplete() && extensionsSupported && swapChainAdequate;
    }

    void PickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

        if (deviceCount <= 0) {
            OH_LOG_ERROR(LOG_APP, "tujiaming failed to find GPUs with Vulkan support!");
            return;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        for (const auto &device : devices) {
            if (IsDeviceSuitable(device)) {
                m_physicalDevice = device;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE) {
            GOLOGE("failed to find a suitable GPU!");
            return;
        }

        vkGetPhysicalDeviceFeatures(m_physicalDevice, &m_physicalDeviceFeatures);
        vkGetPhysicalDeviceProperties(m_physicalDevice, &m_physicalDeviceProperties);
    }
// END DEVICE SUITABILITY

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            ++i;
        }
        return indices;
    }

    void CreateLogicalDeviceAndQueue() {
        QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        float queuePriority = 1.0F;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkDeviceCreateInfo createInfo{};
        VkPhysicalDeviceFeatures deviceFeatures{};
        VkPhysicalDeviceFloat16Int8FeaturesKHR float16Int8Features{};
        float16Int8Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FLOAT16_INT8_FEATURES_KHR;
        float16Int8Features.pNext = nullptr;
        float16Int8Features.shaderFloat16 = VK_TRUE;
        float16Int8Features.shaderInt8 = VK_FALSE;
        createInfo.pNext = &float16Int8Features;

        deviceFeatures.samplerAnisotropy = m_physicalDeviceFeatures.samplerAnisotropy;
        deviceFeatures.shaderInt16 = VK_TRUE;
        deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;

        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

        vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);

        vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
    }

    void EstablishDisplaySizeIdentity() {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &capabilities);

        float ratio;
        if (capabilities.currentExtent.width < capabilities.currentExtent.height) {
            ratio = static_cast<float>(capabilities.currentExtent.width) / m_resolution;
        } else {
            ratio = static_cast<float>(capabilities.currentExtent.height) / m_resolution;
        }
        capabilities.currentExtent.width = static_cast<uint32_t>(capabilities.currentExtent.width / ratio);
        capabilities.currentExtent.height = static_cast<uint32_t>(capabilities.currentExtent.height / ratio);

        m_displaySizeIdentity = capabilities.currentExtent;
    }
};

static napi_value test(napi_env env, napi_callback_info info) {
    HelloVK vulkanBackend_;
    vulkanBackend_.run();

    return nullptr;
}
