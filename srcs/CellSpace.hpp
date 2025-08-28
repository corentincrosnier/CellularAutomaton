#pragma once
#include "SubSpace.hpp"
#include "CellGrid.hpp"
#include <hephaestus/core/hephInstance.hpp>
#include <hephaestus/core/hephResult.hpp>
#include <hephaestus/hephaestus.hpp>
#include <glm/fwd.hpp>
#include <hephaestus/memory/hephMemoryAllocator.hpp>
#include <vulkan/vulkan_core.h>

class CellSpace{
  public:
    CellSpace(){
      m_cellgrid=CellGrid(160,90);
      //m_cellgrid.loadRLEat(0, 0, "./life_patterns/newgun.rle");
    }

    bool        drawCellGrid();
    HephResult  cmdBufferRecord(VkCommandBuffer& cmdBuffer, VkViewport& viewport);
    HephResult  setDevice(VkRenderPass& renderPass, HephDevice& device);
    CellGrid&   getGrid(){return m_cellgrid;}
    void        destroy();
    void        toggleAutoGen(){m_autoGen=(m_autoGen)?false:true;}
    void        setAutoGen(bool autogen){m_autoGen=autogen;}

  protected:
    
    HephResult	createPipeline();
    HephResult	createPipelineLine();
    void        createGridBuffer();

    SubSpace                  m_root;

    bool                      m_autoGen=false;
    CellGrid                  m_cellgrid;
    std::vector<Vertex>       m_verticesLine;
    std::vector<int>          m_indicesLine;
    std::vector<Vertex>       m_verticesTri;
    std::vector<int>          m_indicesTri;
    HephBufferWrapper         m_vertexBufferTri;
    HephBufferWrapper         m_indexBufferTri;
    HephBufferWrapper         m_vertexBufferLine;
    HephBufferWrapper         m_indexBufferLine;
    HephDevice                m_device;
    HephPipelineDescriptor    m_pipelineDescriptor;
    VkPipelineLayout          m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline                m_pipelineTri = VK_NULL_HANDLE;
    VkPipeline                m_pipelineLine = VK_NULL_HANDLE;
    HephCommandPool           m_commandPool;
    HephMemoryAllocator       m_memoryAllocator;
    VkRenderPass              m_renderPass = VK_NULL_HANDLE;
    VkViewport                m_viewport;
};
