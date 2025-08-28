#pragma once

#include "CellSpace.hpp"
#include <hephaestus/hephaestus.hpp>
#include <hephaestus/extensions/hephExtensionDebug.hpp>
#include <hephaestus/extensions/hephExtensionScreenRendering.hpp>
#include <hephaestus/memory/hephMemoryAllocator.hpp>
#include <memory>
#include <array>

#include "guiInfo.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <glm/glm.hpp>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define	FRAMERATE	(1.0 / 60.0)


struct  HephGuiWindowLayout {
  void  setMain(GuiWindowInfo info) {
    main = info;
    updateLayout();
  }
  void  updateLayout() {
  }

  GuiWindowInfo main;

  bool          showNodeEditor = true;
};

class	  HephGui {
	public:
		HephGui();

		HephResult	create();
    void  			run();
    void  			destroy();

    void  callbackWindowSize(GLFWwindow* window, int width, int height);
    void  callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    void  callbackCursor(GLFWwindow* window, double x_pos, double y_pos);
    void  callbackMouseButton(GLFWwindow* window, int button, int action, int mod);
    void  callbackScroll(GLFWwindow* window, double xoffset, double yoffset);
    void  callbackPathDrop(GLFWwindow* window, int count, const char** paths);

	private:
    void          updateLayout();
		HephResult		hephaestusSetup();
    HephResult		createHephSwapchain();
    HephResult		createRenderPass();
    HephResult		setupImGui();
    void  				setupCallbackForWindow(GLFWwindow *window);

    HephResult		render();
    HephResult		renderGui();
		
    void					drawTopBar();
    void					drawCAInfo();

		GLFWwindow*		m_mainWindow;
    int						m_width, m_height;
		HephInstance	m_hephInstance;
		HephDevice		m_device;
		bool					m_quit = false;
		double				m_framerate = FRAMERATE;
		
    double        m_time=0;
    double        m_prevTime=0;
		bool					m_showTopBar = false;
		GuiWindowInfo	m_windowInfoTopBar;

    HephGuiWindowLayout         m_winLayout;

    //Vulkan Stuff
		HephSwapchain									m_swapchain;
    VkSurfaceKHR      						m_surface;
		VkSurfaceFormatKHR						m_surfaceFormat;
    VkRenderPass                  m_renderPass = VK_NULL_HANDLE;
    HephCommandPool               m_commandPool;
		HephMemoryAllocator						m_allocator;
    uint32_t                      m_imageCount = 0;
    uint32_t                      m_imageCurrent = 0;
    std::vector<VkFence>          m_fences;
    std::vector<VkCommandBuffer>  m_commandBuffers;

    CellSpace                     m_cellspace;

    //GUI
    VkDescriptorPool      				m_imGuiDescPool = VK_NULL_HANDLE;
};
