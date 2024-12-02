// © 2021 NVIDIA Corporation

static inline bool IsConstantColorReferenced(BlendFactor factor) {
    return factor == BlendFactor::CONSTANT_COLOR || factor == BlendFactor::CONSTANT_ALPHA || factor == BlendFactor::ONE_MINUS_CONSTANT_COLOR || factor == BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
}

PipelineVK::~PipelineVK() {
    if (m_OwnsNativeObjects) {
        const auto& vk = m_Device.GetDispatchTable();
        vk.DestroyPipeline(m_Device, m_Handle, m_Device.GetAllocationCallbacks());
    }
}

Result PipelineVK::Create(const GraphicsPipelineDesc& graphicsPipelineDesc) {
    m_BindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Shaders
    Scratch<VkPipelineShaderStageCreateInfo> stages = AllocateScratch(m_Device, VkPipelineShaderStageCreateInfo, graphicsPipelineDesc.shaderNum);
    Scratch<VkShaderModule> modules = AllocateScratch(m_Device, VkShaderModule, graphicsPipelineDesc.shaderNum);

    for (uint32_t i = 0; i < graphicsPipelineDesc.shaderNum; i++) {
        const ShaderDesc& shaderDesc = graphicsPipelineDesc.shaders[i];
        Result res = SetupShaderStage(stages[i], shaderDesc, modules[i]);
        if (res != Result::SUCCESS)
            return res;

        stages[i].pName = shaderDesc.entryPointName ? shaderDesc.entryPointName : "main";
    }

    // Vertex input
    const VertexInputDesc* vi = graphicsPipelineDesc.vertexInput;
    uint32_t attributeNum = vi ? vi->attributeNum : 0u;
    uint32_t streamNum = vi ? vi->streamNum : 0u;

    Scratch<VkVertexInputAttributeDescription> vertexAttributeDescs = AllocateScratch(m_Device, VkVertexInputAttributeDescription, attributeNum);
    Scratch<VkVertexInputBindingDescription> vertexBindingDescs = AllocateScratch(m_Device, VkVertexInputBindingDescription, streamNum);

    VkPipelineVertexInputStateCreateInfo vertexInputState = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescs;
    vertexInputState.pVertexBindingDescriptions = vertexBindingDescs;

    if (vi) {
        vertexInputState.vertexAttributeDescriptionCount = vi->attributeNum;
        vertexInputState.vertexBindingDescriptionCount = vi->streamNum;

        for (uint32_t i = 0; i < vi->attributeNum; i++) {
            const VertexAttributeDesc& attribute = vi->attributes[i];

            VkVertexInputAttributeDescription& vertexAttributeDesc = vertexAttributeDescs[i];
            vertexAttributeDesc = {};
            vertexAttributeDesc.location = attribute.vk.location;
            vertexAttributeDesc.binding = attribute.streamIndex;
            vertexAttributeDesc.format = GetVkFormat(attribute.format);
            vertexAttributeDesc.offset = attribute.offset;
        }

        for (uint32_t i = 0; i < vi->streamNum; i++) {
            const VertexStreamDesc& stream = vi->streams[i];

            VkVertexInputBindingDescription& vertexBindingDesc = vertexBindingDescs[i];
            vertexBindingDesc = {};
            vertexBindingDesc.binding = stream.bindingSlot;
            vertexBindingDesc.stride = stream.stride;
            vertexBindingDesc.inputRate = stream.stepRate == VertexStreamStepRate::PER_VERTEX ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
        }
    }

    // Input assembly
    const InputAssemblyDesc& ia = graphicsPipelineDesc.inputAssembly;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssemblyState.topology = GetTopology(ia.topology);
    inputAssemblyState.primitiveRestartEnable = ia.primitiveRestart != PrimitiveRestart::DISABLED;

    VkPipelineTessellationStateCreateInfo tessellationState = {VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO};
    tessellationState.patchControlPoints = ia.tessControlPointNum;

    // Multisample
    const MultisampleDesc* ms = graphicsPipelineDesc.multisample;

    VkPipelineSampleLocationsStateCreateInfoEXT sampleLocationsState = {VK_STRUCTURE_TYPE_PIPELINE_SAMPLE_LOCATIONS_STATE_CREATE_INFO_EXT};
    sampleLocationsState.sampleLocationsInfo.sType = VK_STRUCTURE_TYPE_SAMPLE_LOCATIONS_INFO_EXT;

    VkPipelineMultisampleStateCreateInfo multisampleState = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisampleState.rasterizationSamples = ms ? (VkSampleCountFlagBits)ms->sampleNum : VK_SAMPLE_COUNT_1_BIT;

    if (graphicsPipelineDesc.multisample) {
        multisampleState.sampleShadingEnable = false;
        multisampleState.minSampleShading = 0.0f;
        multisampleState.pSampleMask = ms->sampleMask != ALL_SAMPLES ? &ms->sampleMask : nullptr;
        multisampleState.alphaToCoverageEnable = ms->alphaToCoverage;
        multisampleState.alphaToOneEnable = false;

        const void** tail = &multisampleState.pNext;
        if (ms->sampleLocations) {
            sampleLocationsState.sampleLocationsEnable = VK_TRUE;

            APPEND_EXT(sampleLocationsState);
        }
    }

    // Rasterization
    const RasterizationDesc& r = graphicsPipelineDesc.rasterization;

    VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.viewportCount = graphicsPipelineDesc.rasterization.viewportNum == ONE_VIEWPORT ? 1 : graphicsPipelineDesc.rasterization.viewportNum;
    viewportState.scissorCount = viewportState.viewportCount;

    VkPipelineRasterizationStateCreateInfo rasterizationState = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterizationState.depthClampEnable = r.depthClamp;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE; // TODO: D3D doesn't have this
    rasterizationState.polygonMode = GetPolygonMode(r.fillMode);
    rasterizationState.cullMode = GetCullModeVk(r.cullMode);
    rasterizationState.frontFace = r.frontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
    rasterizationState.depthBiasEnable = IsDepthBiasEnabled(r.depthBias) ? VK_TRUE : VK_FALSE;
    rasterizationState.depthBiasConstantFactor = r.depthBias.constant;
    rasterizationState.depthBiasClamp = r.depthBias.clamp;
    rasterizationState.depthBiasSlopeFactor = r.depthBias.slope;
    rasterizationState.lineWidth = 1.0f;

    const void** tail = &rasterizationState.pNext;
    VkPipelineRasterizationConservativeStateCreateInfoEXT consetvativeRasterizationState = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT};
    if (r.conservativeRaster) {
        consetvativeRasterizationState.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;
        consetvativeRasterizationState.extraPrimitiveOverestimationSize = 0.0f;

        APPEND_EXT(consetvativeRasterizationState);
    }

    VkPipelineRasterizationLineStateCreateInfoKHR lineState = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_LINE_STATE_CREATE_INFO_KHR};
    if (r.lineSmoothing) {
        lineState.lineRasterizationMode = VK_LINE_RASTERIZATION_MODE_RECTANGULAR_SMOOTH_KHR;
        APPEND_EXT(lineState);
    }

    m_DepthBias = r.depthBias;

    // Depth-stencil
    const DepthAttachmentDesc& da = graphicsPipelineDesc.outputMerger.depth;
    const StencilAttachmentDesc& sa = graphicsPipelineDesc.outputMerger.stencil;

    VkPipelineDepthStencilStateCreateInfo depthStencilState = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencilState.depthTestEnable = da.compareFunc != CompareFunc::NONE;
    depthStencilState.depthWriteEnable = da.write;
    depthStencilState.depthCompareOp = GetCompareOp(da.compareFunc);
    depthStencilState.depthBoundsTestEnable = da.boundsTest;
    depthStencilState.stencilTestEnable = (sa.front.compareFunc == CompareFunc::NONE && sa.back.compareFunc == CompareFunc::NONE) ? VK_FALSE : VK_TRUE;
    depthStencilState.minDepthBounds = 0.0f;
    depthStencilState.maxDepthBounds = 1.0f;

    depthStencilState.front.failOp = GetStencilOpVK(sa.front.fail);
    depthStencilState.front.passOp = GetStencilOpVK(sa.front.pass);
    depthStencilState.front.depthFailOp = GetStencilOpVK(sa.front.depthFail);
    depthStencilState.front.compareOp = GetCompareOp(sa.front.compareFunc);
    depthStencilState.front.compareMask = sa.front.compareMask;
    depthStencilState.front.writeMask = sa.front.writeMask;

    depthStencilState.back.failOp = GetStencilOpVK(sa.back.fail);
    depthStencilState.back.passOp = GetStencilOpVK(sa.back.pass);
    depthStencilState.back.depthFailOp = GetStencilOpVK(sa.back.depthFail);
    depthStencilState.back.compareOp = GetCompareOp(sa.back.compareFunc);
    depthStencilState.back.compareMask = sa.back.compareMask;
    depthStencilState.back.writeMask = sa.back.writeMask;

    // Blending
    const OutputMergerDesc& om = graphicsPipelineDesc.outputMerger;
    Scratch<VkPipelineColorBlendAttachmentState> scratch = AllocateScratch(m_Device, VkPipelineColorBlendAttachmentState, om.colorNum);

    VkPipelineColorBlendStateCreateInfo colorBlendState = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlendState.logicOpEnable = om.logicFunc != LogicFunc::NONE ? VK_TRUE : VK_FALSE;
    colorBlendState.logicOp = GetLogicOpVK(om.logicFunc);
    colorBlendState.attachmentCount = om.colorNum;
    colorBlendState.pAttachments = scratch;

    bool isConstantColorReferenced = false;
    VkPipelineColorBlendAttachmentState* attachments = const_cast<VkPipelineColorBlendAttachmentState*>(colorBlendState.pAttachments);
    for (uint32_t i = 0; i < om.colorNum; i++) {
        const ColorAttachmentDesc& attachmentDesc = om.colors[i];

        attachments[i] = {
            VkBool32(attachmentDesc.blendEnabled),
            GetBlendFactor(attachmentDesc.colorBlend.srcFactor),
            GetBlendFactor(attachmentDesc.colorBlend.dstFactor),
            GetBlendOpVK(attachmentDesc.colorBlend.func),
            GetBlendFactor(attachmentDesc.alphaBlend.srcFactor),
            GetBlendFactor(attachmentDesc.alphaBlend.dstFactor),
            GetBlendOpVK(attachmentDesc.alphaBlend.func),
            GetColorComponent(attachmentDesc.colorWriteMask),
        };

        if (IsConstantColorReferenced(attachmentDesc.colorBlend.srcFactor) || IsConstantColorReferenced(attachmentDesc.colorBlend.dstFactor) || IsConstantColorReferenced(attachmentDesc.alphaBlend.srcFactor) || IsConstantColorReferenced(attachmentDesc.alphaBlend.dstFactor))
            isConstantColorReferenced = true;
    }

    // Formats
    Scratch<VkFormat> colorFormats = AllocateScratch(m_Device, VkFormat, om.colorNum);
    for (uint32_t i = 0; i < om.colorNum; i++)
        colorFormats[i] = GetVkFormat(om.colors[i].format);

    VkPipelineRenderingCreateInfo pipelineRenderingCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};
    // pipelineRenderingCreateInfo.viewMask; // TODO: add multi-view support
    pipelineRenderingCreateInfo.colorAttachmentCount = om.colorNum;
    pipelineRenderingCreateInfo.pColorAttachmentFormats = colorFormats;
    pipelineRenderingCreateInfo.depthAttachmentFormat = GetVkFormat(om.depthStencilFormat);
    pipelineRenderingCreateInfo.stencilAttachmentFormat = HasStencil(om.depthStencilFormat) ? GetVkFormat(om.depthStencilFormat) : VK_FORMAT_UNDEFINED;

    // Dynamic state
    uint32_t dynamicStateNum = 0;
    std::array<VkDynamicState, 16> dynamicStates;
    dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_SCISSOR;
    if (rasterizationState.depthBiasEnable)
        dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_DEPTH_BIAS;
    if (depthStencilState.depthBoundsTestEnable)
        dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_DEPTH_BOUNDS;
    if (depthStencilState.stencilTestEnable)
        dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_STENCIL_REFERENCE;
    if (sampleLocationsState.sampleLocationsEnable)
        dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_SAMPLE_LOCATIONS_EXT;
    if (isConstantColorReferenced)
        dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_BLEND_CONSTANTS;
    if (r.shadingRate)
        dynamicStates[dynamicStateNum++] = VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR;

    VkPipelineDynamicStateCreateInfo dynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamicState.dynamicStateCount = dynamicStateNum;
    dynamicState.pDynamicStates = dynamicStates.data();

    // Create
    VkPipelineCreateFlags flags = 0;
    if (r.shadingRate)
        flags |= VK_PIPELINE_CREATE_RENDERING_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;

    const PipelineLayoutVK& pipelineLayoutVK = *(const PipelineLayoutVK*)graphicsPipelineDesc.pipelineLayout;

    VkGraphicsPipelineCreateInfo info = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        &pipelineRenderingCreateInfo,
        flags,
        graphicsPipelineDesc.shaderNum,
        stages,
        &vertexInputState,
        &inputAssemblyState,
        &tessellationState,
        &viewportState,
        &rasterizationState,
        &multisampleState,
        &depthStencilState,
        &colorBlendState,
        &dynamicState,
        pipelineLayoutVK,
        VK_NULL_HANDLE,
        0,
        VK_NULL_HANDLE,
        -1,
    };

    const auto& vk = m_Device.GetDispatchTable();
    const VkResult vkResult = vk.CreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &info, m_Device.GetAllocationCallbacks(), &m_Handle);
    RETURN_ON_FAILURE(&m_Device, vkResult == VK_SUCCESS, GetReturnCode(vkResult), "vkCreateGraphicsPipelines returned %d", (int32_t)vkResult);

    for (size_t i = 0; i < graphicsPipelineDesc.shaderNum; i++)
        vk.DestroyShaderModule(m_Device, modules[i], m_Device.GetAllocationCallbacks());

    return Result::SUCCESS;
}

Result PipelineVK::Create(const ComputePipelineDesc& computePipelineDesc) {
    m_BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;

    const PipelineLayoutVK& pipelineLayoutVK = *(const PipelineLayoutVK*)computePipelineDesc.pipelineLayout;

    const VkShaderModuleCreateInfo moduleInfo = {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        nullptr,
        (VkShaderModuleCreateFlags)0,
        (size_t)computePipelineDesc.shader.size,
        (const uint32_t*)computePipelineDesc.shader.bytecode,
    };

    VkShaderModule module = VK_NULL_HANDLE;
    const auto& vk = m_Device.GetDispatchTable();
    VkResult result = vk.CreateShaderModule(m_Device, &moduleInfo, m_Device.GetAllocationCallbacks(), &module);
    RETURN_ON_FAILURE(&m_Device, result == VK_SUCCESS, GetReturnCode(result), "vkCreateShaderModule returned %d", (int32_t)result);

    VkPipelineShaderStageCreateInfo stage = {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        (VkPipelineShaderStageCreateFlags)0,
        VK_SHADER_STAGE_COMPUTE_BIT,
        module,
        computePipelineDesc.shader.entryPointName ? computePipelineDesc.shader.entryPointName : "main",
        nullptr,
    };

    const VkComputePipelineCreateInfo info = {
        VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        nullptr,
        (VkPipelineCreateFlags)0,
        stage,
        pipelineLayoutVK,
        VK_NULL_HANDLE,
        -1,
    };

    result = vk.CreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &info, m_Device.GetAllocationCallbacks(), &m_Handle);
    RETURN_ON_FAILURE(&m_Device, result == VK_SUCCESS, GetReturnCode(result), "vkCreateComputePipelines returned %d", (int32_t)result);

    vk.DestroyShaderModule(m_Device, module, m_Device.GetAllocationCallbacks());

    return Result::SUCCESS;
}

Result PipelineVK::Create(const RayTracingPipelineDesc& rayTracingPipelineDesc) {
    m_BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;

    const PipelineLayoutVK& pipelineLayoutVK = *(const PipelineLayoutVK*)rayTracingPipelineDesc.pipelineLayout;

    const uint32_t stageNum = rayTracingPipelineDesc.shaderLibrary->shaderNum;
    Scratch<VkPipelineShaderStageCreateInfo> stages = AllocateScratch(m_Device, VkPipelineShaderStageCreateInfo, stageNum);
    Scratch<VkShaderModule> modules = AllocateScratch(m_Device, VkShaderModule, stageNum);

    for (uint32_t i = 0; i < stageNum; i++) {
        const ShaderDesc& shaderDesc = rayTracingPipelineDesc.shaderLibrary->shaders[i];
        Result result = SetupShaderStage(stages[i], shaderDesc, modules[i]);
        if (result != Result::SUCCESS)
            return result;

        stages[i].pName = shaderDesc.entryPointName ? shaderDesc.entryPointName : "main";
    }

    Scratch<VkRayTracingShaderGroupCreateInfoKHR> groupArray = AllocateScratch(m_Device, VkRayTracingShaderGroupCreateInfoKHR, rayTracingPipelineDesc.shaderGroupDescNum);
    for (uint32_t i = 0; i < rayTracingPipelineDesc.shaderGroupDescNum; i++) {
        const ShaderGroupDesc& srcGroup = rayTracingPipelineDesc.shaderGroupDescs[i];

        VkRayTracingShaderGroupCreateInfoKHR& dstGroup = groupArray[i];
        dstGroup = {VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR};
        dstGroup.generalShader = VK_SHADER_UNUSED_KHR;
        dstGroup.closestHitShader = VK_SHADER_UNUSED_KHR;
        dstGroup.anyHitShader = VK_SHADER_UNUSED_KHR;
        dstGroup.intersectionShader = VK_SHADER_UNUSED_KHR;

        for (uint32_t j = 0; j < GetCountOf(srcGroup.shaderIndices); j++) {
            if (srcGroup.shaderIndices[j]) {
                const uint32_t index = srcGroup.shaderIndices[j] - 1;
                const VkPipelineShaderStageCreateInfo& stageInfo = stages[index];

                switch (stageInfo.stage) {
                    case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
                    case VK_SHADER_STAGE_MISS_BIT_KHR:
                    case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
                        dstGroup.generalShader = index;
                        break;

                    case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
                        dstGroup.anyHitShader = index;
                        break;

                    case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
                        dstGroup.closestHitShader = index;
                        break;

                    case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
                        dstGroup.intersectionShader = index;
                        break;

                    default:
                        // already initialized
                        break;
                }
            }
        }

        if (dstGroup.intersectionShader != VK_SHADER_UNUSED_KHR)
            dstGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
        else if (dstGroup.generalShader != VK_SHADER_UNUSED_KHR)
            dstGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
        else
            dstGroup.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    }

    VkRayTracingPipelineCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR};
    createInfo.stageCount = stageNum;
    createInfo.pStages = stages;
    createInfo.groupCount = rayTracingPipelineDesc.shaderGroupDescNum;
    createInfo.pGroups = groupArray;
    createInfo.maxPipelineRayRecursionDepth = rayTracingPipelineDesc.recursionDepthMax;
    createInfo.layout = pipelineLayoutVK;
    createInfo.basePipelineIndex = -1;

    const auto& vk = m_Device.GetDispatchTable();
    const VkResult vkResult = vk.CreateRayTracingPipelinesKHR(m_Device, VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &createInfo, m_Device.GetAllocationCallbacks(), &m_Handle);
    RETURN_ON_FAILURE(&m_Device, vkResult == VK_SUCCESS, GetReturnCode(vkResult), "vkCreateRayTracingPipelinesKHR returned %d", (int32_t)vkResult);

    for (size_t i = 0; i < stageNum; i++)
        vk.DestroyShaderModule(m_Device, modules[i], m_Device.GetAllocationCallbacks());

    return Result::SUCCESS;
}

Result PipelineVK::Create(VkPipelineBindPoint bindPoint, VKNonDispatchableHandle vkPipeline) {
    if (!vkPipeline)
        return Result::INVALID_ARGUMENT;

    m_OwnsNativeObjects = false;
    m_Handle = (VkPipeline)vkPipeline;
    m_BindPoint = bindPoint;

    return Result::SUCCESS;
}

Result PipelineVK::SetupShaderStage(VkPipelineShaderStageCreateInfo& stage, const ShaderDesc& shaderDesc, VkShaderModule& module) {
    const VkShaderModuleCreateInfo moduleInfo = {
        VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        nullptr,
        (VkShaderModuleCreateFlags)0,
        (size_t)shaderDesc.size,
        (const uint32_t*)shaderDesc.bytecode,
    };

    const auto& vk = m_Device.GetDispatchTable();
    VkResult result = vk.CreateShaderModule(m_Device, &moduleInfo, m_Device.GetAllocationCallbacks(), &module);
    RETURN_ON_FAILURE(&m_Device, result == VK_SUCCESS, GetReturnCode(result), "vkCreateShaderModule returned %d", (int32_t)result);

    stage = {
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        nullptr,
        (VkPipelineShaderStageCreateFlags)0,
        (VkShaderStageFlagBits)GetShaderStageFlags(shaderDesc.stage),
        module,
        nullptr,
        nullptr,
    };

    return Result::SUCCESS;
}

inline void PipelineVK::SetDebugName(const char* name) {
    m_Device.SetDebugNameToTrivialObject(VK_OBJECT_TYPE_PIPELINE, (uint64_t)m_Handle, name);
}

inline Result PipelineVK::WriteShaderGroupIdentifiers(uint32_t baseShaderGroupIndex, uint32_t shaderGroupNum, void* buffer) const {
    const size_t dataSize = (size_t)(shaderGroupNum * m_Device.GetDesc().rayTracingShaderGroupIdentifierSize);

    const auto& vk = m_Device.GetDispatchTable();
    VkResult result = vk.GetRayTracingShaderGroupHandlesKHR(m_Device, m_Handle, baseShaderGroupIndex, shaderGroupNum, dataSize, buffer);
    RETURN_ON_FAILURE(&m_Device, result == VK_SUCCESS, GetReturnCode(result), "vkGetRayTracingShaderGroupHandlesKHR returned %d", (int32_t)result);

    return Result::SUCCESS;
}
