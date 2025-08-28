#include "CellSpace.hpp"
#include <glm/fwd.hpp>
#include <hephaestus/command/hephCommandPool.hpp>
#include <hephaestus/core/hephResult.hpp>
#include <vulkan/vulkan_core.h>


bool CellSpace::drawCellGrid(){
  if (m_verticesTri.size() || m_indicesTri.size()) {
    vkDeviceWaitIdle(m_device.device);
    m_memoryAllocator.destroyBuffer(m_indexBufferTri);
    m_memoryAllocator.destroyBuffer(m_vertexBufferTri);
  }

  m_verticesTri.clear();
  m_indicesTri.clear();
  
  m_cellgrid.draw(m_verticesTri, m_indicesTri);
  if(m_autoGen)
    m_cellgrid.nextGen();

  size_t  sizeV = m_verticesTri.size() * sizeof(Vertex);
  size_t  sizeI = m_indicesTri.size() * sizeof(uint32_t);

  if(sizeV==0 || sizeI==0)
    return false;

  HephBufferCreateInfo createInfoV = {
    .size = sizeV,
    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  };
  m_memoryAllocator.createBuffer(createInfoV, m_vertexBufferTri);
  m_memoryAllocator.stagingMakeAndCopy(m_vertexBufferTri, m_verticesTri.data(), sizeV, m_commandPool);
  HephBufferCreateInfo createInfoI = {
    .size = sizeI,
    .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  };
  m_memoryAllocator.createBuffer(createInfoI, m_indexBufferTri);
  m_memoryAllocator.stagingMakeAndCopy(m_indexBufferTri, m_indicesTri.data(), sizeI, m_commandPool);

  return true;
}

void CellSpace::destroy(){
  vkDestroyPipeline(m_device.device, m_pipelineTri, nullptr);
  vkDestroyPipeline(m_device.device, m_pipelineLine, nullptr);
  vkDestroyPipelineLayout(m_device.device, m_pipelineLayout, nullptr);
  m_memoryAllocator.destroyBuffer(m_indexBufferTri);
  m_memoryAllocator.destroyBuffer(m_vertexBufferTri);
  m_memoryAllocator.destroyBuffer(m_indexBufferLine);
  m_memoryAllocator.destroyBuffer(m_vertexBufferLine);
  m_commandPool.destroy();
}

HephResult  CellSpace::cmdBufferRecord(VkCommandBuffer& cmdBuffer, VkViewport& viewport){
  VkDeviceSize offsets[] = {0};
  vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLine);
  vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

  vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_vertexBufferLine.buffer, offsets);
  vkCmdBindIndexBuffer(cmdBuffer, m_indexBufferLine.buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmdBuffer, m_indicesLine.size(), 1, 0, 0, 0);

  if(!drawCellGrid())
    return HephResult();
  vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineTri);
  vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

  vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_vertexBufferTri.buffer, offsets);
  vkCmdBindIndexBuffer(cmdBuffer, m_indexBufferTri.buffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(cmdBuffer, m_indicesTri.size(), 1, 0, 0, 0);

  return HephResult();
}

HephResult  CellSpace::setDevice(VkRenderPass& renderPass, HephDevice& device){
  m_device = device;
  m_renderPass = renderPass;

  HEPH_CHECK_RESULT(m_memoryAllocator.create(m_device));

  HephCommandPoolCreateInfo createInfo={
    .queueFamilyIndex = m_device.queues[0].familyIndex
  };

  HEPH_CHECK_RESULT(m_commandPool.create(m_device, createInfo));
  HEPH_CHECK_RESULT(createPipeline());

	return HephResult();
}


HephResult	CellSpace::createPipeline() {
  VkVertexInputBindingDescription       vertexBindingDesc = {
    .binding = 0,
    .stride = sizeof(Vertex),
    .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
  };

  VkVertexInputAttributeDescription     vertexAttrDesc[2] = {
    {
      .location = 0,
      .binding = 0,
      .format = VK_FORMAT_R32G32_SFLOAT,
      .offset = 0,
    },
    {
      .location = 1,
      .binding = 0,
      .format = VK_FORMAT_R32G32B32_SFLOAT,
      .offset = sizeof(glm::vec2),
    }
  };

  VkPipelineVertexInputStateCreateInfo  vertexInputInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
    .vertexBindingDescriptionCount = 1,
    .pVertexBindingDescriptions = &vertexBindingDesc,
    .vertexAttributeDescriptionCount = 2,
    .pVertexAttributeDescriptions = vertexAttrDesc,
  };

  VkPipelineInputAssemblyStateCreateInfo  inputAssemblyInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE,
  };

  VkPipelineRasterizationStateCreateInfo rasterizationInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_NONE,
    .frontFace = VK_FRONT_FACE_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 0.0f, // Optional
    .depthBiasClamp = 0.0f, // Optional
    .depthBiasSlopeFactor = 0.0f, // Optional
    .lineWidth = 1.0f,
  };

  VkPipelineViewportStateCreateInfo   viewportInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .scissorCount = 1,
  };

  VkPipelineMultisampleStateCreateInfo multisampleStateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 1.0f, // Optional
    .pSampleMask = nullptr, // Optional
    .alphaToCoverageEnable = VK_FALSE, // Optional
    .alphaToOneEnable = VK_FALSE, // Optional
  };

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {
    .blendEnable = VK_TRUE,
    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,
    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD,
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };

  VkPipelineColorBlendStateCreateInfo colorBlendInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .logicOpEnable = VK_FALSE,
    .attachmentCount = 1,
    .pAttachments = &colorBlendAttachment,
  };

  HephShaderModuleWrapper	vertShaderModule = {
		.filename = "./spv/shader.vert.spv",
	};
  HephShaderModuleWrapper	fragShaderModule = {
		.filename = "./spv/shader.frag.spv",
	};
	vertShaderModule.load(m_device);
	fragShaderModule.load(m_device);

  VkDynamicState dynamicStates[] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo  dynamicStateInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = 2,
    .pDynamicStates = dynamicStates,
  };

  VkPipelineShaderStageCreateInfo   vertShaderStageInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_VERTEX_BIT,
    .module = vertShaderModule.shaderModule,
    .pName = "main", //TODO: entrypoint
    .pSpecializationInfo = nullptr,
  };
  VkPipelineShaderStageCreateInfo   fragShaderStageInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
    .module = fragShaderModule.shaderModule,
    .pName = "main", //TODO: entrypoint
    .pSpecializationInfo = nullptr,
  };
  VkPipelineShaderStageCreateInfo   shaderStages[] = {
    vertShaderStageInfo,
    fragShaderStageInfo,
  };

	auto	layouts = m_pipelineDescriptor.getLayoutBuffer();
  VkPipelineLayoutCreateInfo 				pipelineLayoutInfo = {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .setLayoutCount = static_cast<uint32_t>(layouts.size()),
    .pSetLayouts = layouts.data(),
    .pushConstantRangeCount = 0, // Optional
    .pPushConstantRanges = nullptr, // Optional
  };

  HEPH_CHECK_RESULT(HephResult(vkCreatePipelineLayout(m_device.device, &pipelineLayoutInfo
        , nullptr, &m_pipelineLayout), "failed to create triangle pipeline layout!"));

  VkGraphicsPipelineCreateInfo  		pipelineInfo = {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .stageCount = 2,
    .pStages = shaderStages,
    .pVertexInputState = &vertexInputInfo,
    .pInputAssemblyState = &inputAssemblyInfo,
    .pViewportState = &viewportInfo,
    .pRasterizationState = &rasterizationInfo,
    .pMultisampleState = &multisampleStateInfo,
    .pDepthStencilState = nullptr, // Optional
    .pColorBlendState = &colorBlendInfo,
    .pDynamicState = &dynamicStateInfo,
    .layout = m_pipelineLayout,
    .renderPass = m_renderPass,
  };

  HEPH_CHECK_RESULT(HephResult(vkCreateGraphicsPipelines(m_device.device, VK_NULL_HANDLE, 1,
        &pipelineInfo, nullptr, &m_pipelineTri), "failed to create graphics pipeline!"));

  inputAssemblyInfo.topology=VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
  rasterizationInfo.polygonMode=VK_POLYGON_MODE_FILL;
  VkGraphicsPipelineCreateInfo    pipelineInfoLine={
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .stageCount = 2,
    .pStages = shaderStages,
    .pVertexInputState = &vertexInputInfo,
    .pInputAssemblyState = &inputAssemblyInfo,
    .pViewportState = &viewportInfo,
    .pRasterizationState = &rasterizationInfo,
    .pMultisampleState = &multisampleStateInfo,
    .pDepthStencilState = nullptr, // Optional
    .pColorBlendState = &colorBlendInfo,
    .pDynamicState = &dynamicStateInfo,
    .layout = m_pipelineLayout,
    .renderPass = m_renderPass,
  };

  HEPH_CHECK_RESULT(HephResult(vkCreateGraphicsPipelines(m_device.device, VK_NULL_HANDLE, 1,
        &pipelineInfo, nullptr, &m_pipelineLine), "failed to create graphics pipeline!"));

  createGridBuffer();

	vertShaderModule.destroy(m_device);
	fragShaderModule.destroy(m_device);
	return (HephResult());
}

void CellSpace::createGridBuffer(){
  int gridW=m_cellgrid.getWidth();
  int gridH=m_cellgrid.getHeight();
  Vertex v={
    .pos=glm::vec2(-1.0,-1.0),
    .color=glm::vec3(0.075,0.075,0.075)
  };

  int nbVertex=0;
  for(int i=0;i<=gridW;i++){
    v.pos.x=(float)i*2/gridW-1.0;
    v.pos.y=-1.0;
    m_verticesLine.push_back(v);
    m_indicesLine.push_back(nbVertex);
    v.pos.x=(float)i*2/gridW-1.0;
    v.pos.y=1.0;
    m_verticesLine.push_back(v);
    m_indicesLine.push_back(nbVertex+1);
    nbVertex+=2;
  }
  for(int i=0;i<=gridH;i++){
    v.pos.y=(float)i*2/gridH-1.0;
    v.pos.x=-1.0;
    m_verticesLine.push_back(v);
    m_indicesLine.push_back(nbVertex);
    v.pos.y=(float)i*2/gridH-1.0;
    v.pos.x=1.0;
    m_verticesLine.push_back(v);
    m_indicesLine.push_back(nbVertex+1);
    nbVertex+=2;
  }

  size_t  sizeV = m_verticesLine.size() * sizeof(Vertex);
  size_t  sizeI = m_indicesLine.size() * sizeof(uint32_t);

  HephBufferCreateInfo createInfoV = {
    .size = sizeV,
    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  };
  m_memoryAllocator.createBuffer(createInfoV, m_vertexBufferLine);
  m_memoryAllocator.stagingMakeAndCopy(m_vertexBufferLine, m_verticesLine.data(), sizeV, m_commandPool);
  HephBufferCreateInfo createInfoI = {
    .size = sizeI,
    .usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    .propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
  };
  m_memoryAllocator.createBuffer(createInfoI, m_indexBufferLine);
  m_memoryAllocator.stagingMakeAndCopy(m_indexBufferLine, m_indicesLine.data(), sizeI, m_commandPool);

}
