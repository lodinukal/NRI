﻿// © 2021 NVIDIA Corporation

template <typename DescComponent, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE subobjectType>
struct alignas(void*) PipelineDescComponent {
    PipelineDescComponent() = default;

    inline void operator=(const DescComponent& d) {
        desc = d;
    }

    D3D12_PIPELINE_STATE_SUBOBJECT_TYPE type = subobjectType;
    DescComponent desc = {};
};

#ifdef NRI_USE_AGILITY_SDK
typedef PipelineDescComponent<D3D12_RASTERIZER_DESC1, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER> PipelineRasterizer;
typedef PipelineDescComponent<D3D12_DEPTH_STENCIL_DESC2, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL2> PipelineDepthStencil;
#else
typedef PipelineDescComponent<D3D12_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER> PipelineRasterizer;
typedef PipelineDescComponent<D3D12_DEPTH_STENCIL_DESC1, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1> PipelineDepthStencil;
#endif

typedef PipelineDescComponent<ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE> PipelineRootSignature;
typedef PipelineDescComponent<D3D12_INPUT_LAYOUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT> PipelineInputLayout;
typedef PipelineDescComponent<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE> PipelineIndexBufferStripCutValue;
typedef PipelineDescComponent<D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY> PipelinePrimitiveTopology;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS> PipelineVertexShader;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS> PipelineHullShader;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS> PipelineDomainShader;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS> PipelineGeometryShader;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_AS> PipelineAmplificationShader;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_MS> PipelineMeshShader;
typedef PipelineDescComponent<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS> PipelinePixelShader;
typedef PipelineDescComponent<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK> PipelineNodeMask;
typedef PipelineDescComponent<DXGI_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC> PipelineSampleDesc;
typedef PipelineDescComponent<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK> PipelineSampleMask;
typedef PipelineDescComponent<D3D12_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND> PipelineBlend;
typedef PipelineDescComponent<DXGI_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT> PipelineDepthStencilFormat;
typedef PipelineDescComponent<D3D12_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS> PipelineRenderTargetFormats;
typedef PipelineDescComponent<D3D12_PIPELINE_STATE_FLAGS, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS> PipelineFlags;

static_assert((uint32_t)PrimitiveRestart::DISABLED == D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED, "Enum mismatch");
static_assert((uint32_t)PrimitiveRestart::INDICES_UINT16 == D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFF, "Enum mismatch");
static_assert((uint32_t)PrimitiveRestart::INDICES_UINT32 == D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF, "Enum mismatch");

static void FillRasterizerState(D3D12_RASTERIZER_DESC& rasterizerDesc, const GraphicsPipelineDesc& graphicsPipelineDesc) {
    const RasterizationDesc& r = graphicsPipelineDesc.rasterization;

    rasterizerDesc.FillMode = GetFillMode(r.fillMode);
    rasterizerDesc.CullMode = GetCullModeD3D12(r.cullMode);
    rasterizerDesc.FrontCounterClockwise = (BOOL)r.frontCounterClockwise;
    rasterizerDesc.DepthBias = (INT)r.depthBias.constant;
    rasterizerDesc.DepthBiasClamp = r.depthBias.clamp;
    rasterizerDesc.SlopeScaledDepthBias = r.depthBias.slope;
    rasterizerDesc.DepthClipEnable = (BOOL)r.depthClamp;
    rasterizerDesc.AntialiasedLineEnable = (BOOL)r.lineSmoothing;
    rasterizerDesc.ConservativeRaster = r.conservativeRaster ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    if (graphicsPipelineDesc.multisample) {
        rasterizerDesc.MultisampleEnable = graphicsPipelineDesc.multisample->sampleNum > 1 ? TRUE : FALSE;
        rasterizerDesc.ForcedSampleCount = graphicsPipelineDesc.multisample->sampleNum > 1 ? graphicsPipelineDesc.multisample->sampleNum : 0;
    }
}

static void FillDepthStencilState(D3D12_DEPTH_STENCIL_DESC* depthStencilDesc, const OutputMergerDesc& om) {
    depthStencilDesc->DepthEnable = om.depth.compareFunc == CompareFunc::NONE ? FALSE : TRUE;
    depthStencilDesc->DepthWriteMask = om.depth.write ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc->DepthFunc = GetComparisonFunc(om.depth.compareFunc);
    depthStencilDesc->StencilEnable = (om.stencil.front.compareFunc == CompareFunc::NONE && om.stencil.back.compareFunc == CompareFunc::NONE) ? FALSE : TRUE;
    depthStencilDesc->StencilReadMask = (UINT8)om.stencil.front.compareMask;
    depthStencilDesc->StencilWriteMask = (UINT8)om.stencil.front.writeMask;
    depthStencilDesc->FrontFace.StencilFailOp = GetStencilOpD3D12(om.stencil.front.fail);
    depthStencilDesc->FrontFace.StencilDepthFailOp = GetStencilOpD3D12(om.stencil.front.depthFail);
    depthStencilDesc->FrontFace.StencilPassOp = GetStencilOpD3D12(om.stencil.front.pass);
    depthStencilDesc->FrontFace.StencilFunc = GetComparisonFunc(om.stencil.front.compareFunc);
    depthStencilDesc->BackFace.StencilFailOp = GetStencilOpD3D12(om.stencil.back.fail);
    depthStencilDesc->BackFace.StencilDepthFailOp = GetStencilOpD3D12(om.stencil.back.depthFail);
    depthStencilDesc->BackFace.StencilPassOp = GetStencilOpD3D12(om.stencil.back.pass);
    depthStencilDesc->BackFace.StencilFunc = GetComparisonFunc(om.stencil.back.compareFunc);
}

#ifdef NRI_USE_AGILITY_SDK
static void FillRasterizerState(D3D12_RASTERIZER_DESC1& rasterizerDesc, const GraphicsPipelineDesc& graphicsPipelineDesc) {
    const RasterizationDesc& r = graphicsPipelineDesc.rasterization;

    rasterizerDesc.FillMode = GetFillMode(r.fillMode);
    rasterizerDesc.CullMode = GetCullModeD3D12(r.cullMode);
    rasterizerDesc.FrontCounterClockwise = (BOOL)r.frontCounterClockwise;
    rasterizerDesc.DepthBias = r.depthBias.constant;
    rasterizerDesc.DepthBiasClamp = r.depthBias.clamp;
    rasterizerDesc.SlopeScaledDepthBias = r.depthBias.slope;
    rasterizerDesc.DepthClipEnable = (BOOL)r.depthClamp;
    rasterizerDesc.AntialiasedLineEnable = (BOOL)r.lineSmoothing;
    rasterizerDesc.ConservativeRaster = r.conservativeRaster ? D3D12_CONSERVATIVE_RASTERIZATION_MODE_ON : D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    if (graphicsPipelineDesc.multisample) {
        rasterizerDesc.MultisampleEnable = graphicsPipelineDesc.multisample->sampleNum > 1 ? TRUE : FALSE;
        rasterizerDesc.ForcedSampleCount = graphicsPipelineDesc.multisample->sampleNum > 1 ? graphicsPipelineDesc.multisample->sampleNum : 0;
    }
}

static void FillDepthStencilState(D3D12_DEPTH_STENCIL_DESC2& depthStencilDesc, const OutputMergerDesc& om) {
    depthStencilDesc.DepthEnable = om.depth.compareFunc == CompareFunc::NONE ? FALSE : TRUE;
    depthStencilDesc.DepthWriteMask = om.depth.write ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = GetComparisonFunc(om.depth.compareFunc);
    depthStencilDesc.StencilEnable = (om.stencil.front.compareFunc == CompareFunc::NONE && om.stencil.back.compareFunc == CompareFunc::NONE) ? FALSE : TRUE;
    depthStencilDesc.FrontFace.StencilFailOp = GetStencilOpD3D12(om.stencil.front.fail);
    depthStencilDesc.FrontFace.StencilDepthFailOp = GetStencilOpD3D12(om.stencil.front.depthFail);
    depthStencilDesc.FrontFace.StencilPassOp = GetStencilOpD3D12(om.stencil.front.pass);
    depthStencilDesc.FrontFace.StencilFunc = GetComparisonFunc(om.stencil.front.compareFunc);
    depthStencilDesc.FrontFace.StencilReadMask = om.stencil.front.compareMask;
    depthStencilDesc.FrontFace.StencilWriteMask = om.stencil.front.writeMask;
    depthStencilDesc.BackFace.StencilFailOp = GetStencilOpD3D12(om.stencil.back.fail);
    depthStencilDesc.BackFace.StencilDepthFailOp = GetStencilOpD3D12(om.stencil.back.depthFail);
    depthStencilDesc.BackFace.StencilPassOp = GetStencilOpD3D12(om.stencil.back.pass);
    depthStencilDesc.BackFace.StencilFunc = GetComparisonFunc(om.stencil.back.compareFunc);
    depthStencilDesc.BackFace.StencilReadMask = om.stencil.back.compareMask;
    depthStencilDesc.BackFace.StencilWriteMask = om.stencil.back.writeMask;
}
#endif

static void FillInputLayout(D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc, const GraphicsPipelineDesc& graphicsPipelineDesc) {
    if (!graphicsPipelineDesc.vertexInput)
        return;

    const VertexInputDesc& vi = *graphicsPipelineDesc.vertexInput;

    inputLayoutDesc.NumElements = vi.attributeNum;

    D3D12_INPUT_ELEMENT_DESC* inputElementsDescs = (D3D12_INPUT_ELEMENT_DESC*)inputLayoutDesc.pInputElementDescs;
    for (uint32_t i = 0; i < vi.attributeNum; i++) {
        const VertexAttributeDesc& vertexAttributeDesc = vi.attributes[i];
        const VertexStreamDesc& vertexStreamDesc = vi.streams[vertexAttributeDesc.streamIndex];
        bool isPerVertexData = vertexStreamDesc.stepRate == VertexStreamStepRate::PER_VERTEX;

        inputElementsDescs[i].SemanticName = vertexAttributeDesc.d3d.semanticName;
        inputElementsDescs[i].SemanticIndex = vertexAttributeDesc.d3d.semanticIndex;
        inputElementsDescs[i].Format = GetDxgiFormat(vertexAttributeDesc.format).typed;
        inputElementsDescs[i].InputSlot = vertexStreamDesc.bindingSlot;
        inputElementsDescs[i].AlignedByteOffset = vertexAttributeDesc.offset;
        inputElementsDescs[i].InputSlotClass = isPerVertexData ? D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA : D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
        inputElementsDescs[i].InstanceDataStepRate = isPerVertexData ? 0 : 1;
    }
}

static void FillShaderBytecode(D3D12_SHADER_BYTECODE& shaderBytecode, const ShaderDesc& shaderDesc) {
    shaderBytecode.pShaderBytecode = shaderDesc.bytecode;
    shaderBytecode.BytecodeLength = (size_t)shaderDesc.size;
}

static void FillBlendState(D3D12_BLEND_DESC& blendDesc, const GraphicsPipelineDesc& graphicsPipelineDesc) {
    const OutputMergerDesc& om = graphicsPipelineDesc.outputMerger;

    blendDesc.AlphaToCoverageEnable = (graphicsPipelineDesc.multisample && graphicsPipelineDesc.multisample->alphaToCoverage) ? TRUE : FALSE;
    blendDesc.IndependentBlendEnable = TRUE;

    for (uint32_t i = 0; i < om.colorNum; i++) {
        const ColorAttachmentDesc& colorAttachmentDesc = om.colors[i];

        blendDesc.RenderTarget[i].RenderTargetWriteMask = GetRenderTargetWriteMask(colorAttachmentDesc.colorWriteMask);
        blendDesc.RenderTarget[i].BlendEnable = colorAttachmentDesc.blendEnabled;

        if (colorAttachmentDesc.blendEnabled) {
            blendDesc.RenderTarget[i].LogicOp = GetLogicOpD3D12(om.logicFunc);
            blendDesc.RenderTarget[i].LogicOpEnable = om.logicFunc != LogicFunc::NONE ? TRUE : FALSE;
            blendDesc.RenderTarget[i].SrcBlend = GetBlend(colorAttachmentDesc.colorBlend.srcFactor);
            blendDesc.RenderTarget[i].DestBlend = GetBlend(colorAttachmentDesc.colorBlend.dstFactor);
            blendDesc.RenderTarget[i].BlendOp = GetBlendOpD3D12(colorAttachmentDesc.colorBlend.func);
            blendDesc.RenderTarget[i].SrcBlendAlpha = GetBlend(colorAttachmentDesc.alphaBlend.srcFactor);
            blendDesc.RenderTarget[i].DestBlendAlpha = GetBlend(colorAttachmentDesc.alphaBlend.dstFactor);
            blendDesc.RenderTarget[i].BlendOpAlpha = GetBlendOpD3D12(colorAttachmentDesc.alphaBlend.func);
        }
    }
}

static uint32_t FillSampleDesc(DXGI_SAMPLE_DESC& sampleDesc, const GraphicsPipelineDesc& graphicsPipelineDesc) {
    if (!graphicsPipelineDesc.multisample) {
        sampleDesc.Count = 1;
        return uint32_t(-1);
    }

    sampleDesc.Count = graphicsPipelineDesc.multisample->sampleNum;
    sampleDesc.Quality = 0;

    return graphicsPipelineDesc.multisample->sampleMask != ALL_SAMPLES ? graphicsPipelineDesc.multisample->sampleMask : uint32_t(-1);
}

Result PipelineD3D12::CreateFromStream(const GraphicsPipelineDesc& graphicsPipelineDesc) {
    CHECK(m_Device.GetVersion() >= 2, "Newer interface needed");

    struct Stream {
        PipelineRootSignature rootSignature;
        PipelinePrimitiveTopology primitiveTopology;
        PipelineInputLayout inputLayout;
        PipelineIndexBufferStripCutValue indexBufferStripCutValue;
        PipelineVertexShader vertexShader;
        PipelineHullShader hullShader;
        PipelineDomainShader domainShader;
        PipelineGeometryShader geometryShader;
        PipelineAmplificationShader amplificationShader;
        PipelineMeshShader meshShader;
        PipelinePixelShader pixelShader;
        PipelineSampleDesc sampleDesc;
        PipelineSampleMask sampleMask;
        PipelineRasterizer rasterizer;
        PipelineBlend blend;
        PipelineRenderTargetFormats renderTargetFormats;
        PipelineDepthStencil depthStencil;
        PipelineDepthStencilFormat depthStencilFormat;
        PipelineNodeMask nodeMask;
        PipelineFlags flags;
    };

    Stream stream = {};
    stream.rootSignature = *m_PipelineLayout;
    stream.nodeMask = NRI_NODE_MASK;

    // Shaders
    for (uint32_t i = 0; i < graphicsPipelineDesc.shaderNum; i++) {
        const ShaderDesc& shader = graphicsPipelineDesc.shaders[i];
        if (shader.stage == StageBits::VERTEX_SHADER)
            FillShaderBytecode(stream.vertexShader.desc, shader);
        else if (shader.stage == StageBits::TESS_CONTROL_SHADER)
            FillShaderBytecode(stream.hullShader.desc, shader);
        else if (shader.stage == StageBits::TESS_EVALUATION_SHADER)
            FillShaderBytecode(stream.domainShader.desc, shader);
        else if (shader.stage == StageBits::GEOMETRY_SHADER)
            FillShaderBytecode(stream.geometryShader.desc, shader);
        else if (shader.stage == StageBits::MESH_CONTROL_SHADER)
            FillShaderBytecode(stream.amplificationShader.desc, shader);
        else if (shader.stage == StageBits::MESH_EVALUATION_SHADER)
            FillShaderBytecode(stream.meshShader.desc, shader);
        else if (shader.stage == StageBits::FRAGMENT_SHADER)
            FillShaderBytecode(stream.pixelShader.desc, shader);
        else
            return Result::INVALID_ARGUMENT;
    }

    // Vertex input
    uint32_t attributeNum = graphicsPipelineDesc.vertexInput ? graphicsPipelineDesc.vertexInput->attributeNum : 0;
    Scratch<D3D12_INPUT_ELEMENT_DESC> scratch = AllocateScratch(m_Device, D3D12_INPUT_ELEMENT_DESC, attributeNum);
    stream.inputLayout.desc.pInputElementDescs = scratch;
    if (graphicsPipelineDesc.vertexInput) {
        const VertexInputDesc& vi = *graphicsPipelineDesc.vertexInput;

        FillInputLayout(stream.inputLayout.desc, graphicsPipelineDesc);

        uint32_t maxBindingSlot = 0;
        for (uint32_t i = 0; i < vi.streamNum; i++) {
            const VertexStreamDesc& vertexStream = vi.streams[i];
            if (vertexStream.bindingSlot > maxBindingSlot)
                maxBindingSlot = vertexStream.bindingSlot;
        }
        m_InputAssemplyStrides.resize(maxBindingSlot + 1);

        for (uint32_t i = 0; i < graphicsPipelineDesc.vertexInput->streamNum; i++) {
            const VertexStreamDesc& vertexStream = vi.streams[i];
            m_InputAssemplyStrides[vertexStream.bindingSlot] = vertexStream.stride;
        }
    }

    // Input assembly
    m_PrimitiveTopology = ::GetPrimitiveTopology(graphicsPipelineDesc.inputAssembly.topology, graphicsPipelineDesc.inputAssembly.tessControlPointNum);
    stream.primitiveTopology = GetPrimitiveTopologyType(graphicsPipelineDesc.inputAssembly.topology);
    stream.indexBufferStripCutValue = (D3D12_INDEX_BUFFER_STRIP_CUT_VALUE)graphicsPipelineDesc.inputAssembly.primitiveRestart;

    // Multisample
    stream.sampleMask.desc = FillSampleDesc(stream.sampleDesc.desc, graphicsPipelineDesc);

    // Rasterizer
    FillRasterizerState(stream.rasterizer.desc, graphicsPipelineDesc);
#ifdef NRI_USE_AGILITY_SDK
    if (IsDepthBiasEnabled(graphicsPipelineDesc.rasterization.depthBias))
        stream.flags = D3D12_PIPELINE_STATE_FLAG_DYNAMIC_DEPTH_BIAS;
#endif

        // Depth stencil
#ifdef NRI_USE_AGILITY_SDK
    FillDepthStencilState(stream.depthStencil.desc, graphicsPipelineDesc.outputMerger);
#else
    FillDepthStencilState((D3D12_DEPTH_STENCIL_DESC*)&stream.depthStencil.desc, graphicsPipelineDesc.outputMerger);
#endif
    stream.depthStencil.desc.DepthBoundsTestEnable = graphicsPipelineDesc.outputMerger.depth.boundsTest ? 1 : 0;
    stream.depthStencilFormat = GetDxgiFormat(graphicsPipelineDesc.outputMerger.depthStencilFormat).typed;

    // Output merger
    FillBlendState(stream.blend.desc, graphicsPipelineDesc);

    stream.renderTargetFormats.desc.NumRenderTargets = graphicsPipelineDesc.outputMerger.colorNum;
    for (uint32_t i = 0; i < graphicsPipelineDesc.outputMerger.colorNum; i++)
        stream.renderTargetFormats.desc.RTFormats[i] = GetDxgiFormat(graphicsPipelineDesc.outputMerger.colors[i].format).typed;

    // Create
    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {};
    pipelineStateStreamDesc.pPipelineStateSubobjectStream = &stream;
    pipelineStateStreamDesc.SizeInBytes = sizeof(stream);

    HRESULT hr = m_Device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState));
    RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device2::CreatePipelineState()");

    return Result::SUCCESS;
}

Result PipelineD3D12::Create(const GraphicsPipelineDesc& graphicsPipelineDesc) {
    m_PipelineLayout = (const PipelineLayoutD3D12*)graphicsPipelineDesc.pipelineLayout;
    m_IsGraphicsPipeline = true;

    if (m_Device.GetVersion() >= 2)
        return CreateFromStream(graphicsPipelineDesc);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipleineStateDesc = {};
    graphicsPipleineStateDesc.NodeMask = NRI_NODE_MASK;
    graphicsPipleineStateDesc.pRootSignature = *m_PipelineLayout;

    // Shaders
    for (uint32_t i = 0; i < graphicsPipelineDesc.shaderNum; i++) {
        const ShaderDesc& shader = graphicsPipelineDesc.shaders[i];
        if (shader.stage == StageBits::VERTEX_SHADER)
            FillShaderBytecode(graphicsPipleineStateDesc.VS, shader);
        else if (shader.stage == StageBits::TESS_CONTROL_SHADER)
            FillShaderBytecode(graphicsPipleineStateDesc.HS, shader);
        else if (shader.stage == StageBits::TESS_EVALUATION_SHADER)
            FillShaderBytecode(graphicsPipleineStateDesc.DS, shader);
        else if (shader.stage == StageBits::GEOMETRY_SHADER)
            FillShaderBytecode(graphicsPipleineStateDesc.GS, shader);
        else if (shader.stage == StageBits::FRAGMENT_SHADER)
            FillShaderBytecode(graphicsPipleineStateDesc.PS, shader);
        else
            return Result::INVALID_ARGUMENT;
    }

    // Vertex input
    uint32_t attributeNum = graphicsPipelineDesc.vertexInput ? graphicsPipelineDesc.vertexInput->attributeNum : 0;
    Scratch<D3D12_INPUT_ELEMENT_DESC> scratch = AllocateScratch(m_Device, D3D12_INPUT_ELEMENT_DESC, attributeNum);
    graphicsPipleineStateDesc.InputLayout.pInputElementDescs = scratch;
    if (graphicsPipelineDesc.vertexInput) {
        const VertexInputDesc& vi = *graphicsPipelineDesc.vertexInput;

        FillInputLayout(graphicsPipleineStateDesc.InputLayout, graphicsPipelineDesc);

        uint32_t maxBindingSlot = 0;
        for (uint32_t i = 0; i < vi.streamNum; i++) {
            const VertexStreamDesc& vertexStream = vi.streams[i];
            if (vertexStream.bindingSlot > maxBindingSlot)
                maxBindingSlot = vertexStream.bindingSlot;
        }
        m_InputAssemplyStrides.resize(maxBindingSlot + 1);

        for (uint32_t i = 0; i < graphicsPipelineDesc.vertexInput->streamNum; i++) {
            const VertexStreamDesc& vertexStream = vi.streams[i];
            m_InputAssemplyStrides[vertexStream.bindingSlot] = vertexStream.stride;
        }
    }

    // Input assembly
    m_PrimitiveTopology = ::GetPrimitiveTopology(graphicsPipelineDesc.inputAssembly.topology, graphicsPipelineDesc.inputAssembly.tessControlPointNum);
    graphicsPipleineStateDesc.PrimitiveTopologyType = GetPrimitiveTopologyType(graphicsPipelineDesc.inputAssembly.topology);
    graphicsPipleineStateDesc.IBStripCutValue = (D3D12_INDEX_BUFFER_STRIP_CUT_VALUE)graphicsPipelineDesc.inputAssembly.primitiveRestart;

    // Multisample
    graphicsPipleineStateDesc.SampleMask = FillSampleDesc(graphicsPipleineStateDesc.SampleDesc, graphicsPipelineDesc);

    // Rasterizer
    FillRasterizerState(graphicsPipleineStateDesc.RasterizerState, graphicsPipelineDesc);
#ifdef NRI_USE_AGILITY_SDK
    if (IsDepthBiasEnabled(graphicsPipelineDesc.rasterization.depthBias))
        graphicsPipleineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_DYNAMIC_DEPTH_BIAS;
#endif

    // Depth stencil
    FillDepthStencilState(&graphicsPipleineStateDesc.DepthStencilState, graphicsPipelineDesc.outputMerger);
    graphicsPipleineStateDesc.DSVFormat = GetDxgiFormat(graphicsPipelineDesc.outputMerger.depthStencilFormat).typed;

    // Blending
    FillBlendState(graphicsPipleineStateDesc.BlendState, graphicsPipelineDesc);

    graphicsPipleineStateDesc.NumRenderTargets = graphicsPipelineDesc.outputMerger.colorNum;
    for (uint32_t i = 0; i < graphicsPipelineDesc.outputMerger.colorNum; i++)
        graphicsPipleineStateDesc.RTVFormats[i] = GetDxgiFormat(graphicsPipelineDesc.outputMerger.colors[i].format).typed;

    // Create
    HRESULT hr = m_Device->CreateGraphicsPipelineState(&graphicsPipleineStateDesc, IID_PPV_ARGS(&m_PipelineState));
    RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device::CreateGraphicsPipelineState()");

    return Result::SUCCESS;
}

Result PipelineD3D12::Create(const ComputePipelineDesc& computePipelineDesc) {
    m_PipelineLayout = (const PipelineLayoutD3D12*)computePipelineDesc.pipelineLayout;

    D3D12_COMPUTE_PIPELINE_STATE_DESC computePipleineStateDesc = {};
    computePipleineStateDesc.NodeMask = NRI_NODE_MASK;
    computePipleineStateDesc.pRootSignature = *m_PipelineLayout;

    FillShaderBytecode(computePipleineStateDesc.CS, computePipelineDesc.shader);

    HRESULT hr = m_Device->CreateComputePipelineState(&computePipleineStateDesc, IID_PPV_ARGS(&m_PipelineState));
    RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device::CreateComputePipelineState()");

    return Result::SUCCESS;
}

Result PipelineD3D12::Create(const RayTracingPipelineDesc& rayTracingPipelineDesc) {
    if (m_Device.GetVersion() < 5)
        return Result::UNSUPPORTED;

    m_PipelineLayout = (const PipelineLayoutD3D12*)rayTracingPipelineDesc.pipelineLayout;

    ID3D12RootSignature* rootSignature = *m_PipelineLayout;

    uint32_t stateSubobjectNum = 0;
    uint32_t shaderNum = rayTracingPipelineDesc.shaderLibrary ? rayTracingPipelineDesc.shaderLibrary->shaderNum : 0;
    uint32_t stateObjectNum = 1 // pipeline config
        + 1                     // shader config
        + 1                     // node mask
        + shaderNum             // DXIL libraries
        + rayTracingPipelineDesc.shaderGroupDescNum
        + (rootSignature ? 1 : 0);
    Scratch<D3D12_STATE_SUBOBJECT> stateSubobjects = AllocateScratch(m_Device, D3D12_STATE_SUBOBJECT, stateObjectNum);

    D3D12_RAYTRACING_PIPELINE_CONFIG rayTracingPipelineConfig = {};
    {
        rayTracingPipelineConfig.MaxTraceRecursionDepth = rayTracingPipelineDesc.recursionDepthMax;

        stateSubobjects[stateSubobjectNum].Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG;
        stateSubobjects[stateSubobjectNum].pDesc = &rayTracingPipelineConfig;
        stateSubobjectNum++;
    }

    D3D12_RAYTRACING_SHADER_CONFIG rayTracingShaderConfig = {};
    {
        rayTracingShaderConfig.MaxPayloadSizeInBytes = rayTracingPipelineDesc.payloadAttributeSizeMax;
        rayTracingShaderConfig.MaxAttributeSizeInBytes = rayTracingPipelineDesc.intersectionAttributeSizeMax;

        stateSubobjects[stateSubobjectNum].Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG;
        stateSubobjects[stateSubobjectNum].pDesc = &rayTracingShaderConfig;
        stateSubobjectNum++;
    }

    D3D12_NODE_MASK nodeMask = {};
    {
        nodeMask.NodeMask = NRI_NODE_MASK;

        stateSubobjects[stateSubobjectNum].Type = D3D12_STATE_SUBOBJECT_TYPE_NODE_MASK;
        stateSubobjects[stateSubobjectNum].pDesc = &nodeMask;
        stateSubobjectNum++;
    }

    D3D12_GLOBAL_ROOT_SIGNATURE globalRootSignature = {};
    if (rootSignature) {
        globalRootSignature.pGlobalRootSignature = rootSignature;

        stateSubobjects[stateSubobjectNum].Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE;
        stateSubobjects[stateSubobjectNum].pDesc = &globalRootSignature;
        stateSubobjectNum++;
    }

    Scratch<D3D12_DXIL_LIBRARY_DESC> libraryDescs = AllocateScratch(m_Device, D3D12_DXIL_LIBRARY_DESC, rayTracingPipelineDesc.shaderLibrary->shaderNum);
    for (uint32_t i = 0; i < rayTracingPipelineDesc.shaderLibrary->shaderNum; i++) {
        libraryDescs[i].DXILLibrary.pShaderBytecode = rayTracingPipelineDesc.shaderLibrary->shaders[i].bytecode;
        libraryDescs[i].DXILLibrary.BytecodeLength = (size_t)rayTracingPipelineDesc.shaderLibrary->shaders[i].size;
        libraryDescs[i].NumExports = 0;
        libraryDescs[i].pExports = nullptr;

        stateSubobjects[stateSubobjectNum].Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY;
        stateSubobjects[stateSubobjectNum].pDesc = &libraryDescs[i];
        stateSubobjectNum++;
    }

    Vector<std::wstring> wEntryPointNames(rayTracingPipelineDesc.shaderLibrary->shaderNum, m_Device.GetStdAllocator());
    for (uint32_t i = 0; i < rayTracingPipelineDesc.shaderLibrary->shaderNum; i++) {
        const ShaderDesc& shader = rayTracingPipelineDesc.shaderLibrary->shaders[i];
        const size_t entryPointNameLength = shader.entryPointName != nullptr ? strlen(shader.entryPointName) + 1 : 0;
        wEntryPointNames[i].resize(entryPointNameLength);
        ConvertCharToWchar(shader.entryPointName, (wchar_t*)wEntryPointNames[i].data(), entryPointNameLength);
    }

    uint32_t hitGroupNum = 0;
    Scratch<D3D12_HIT_GROUP_DESC> hitGroups = AllocateScratch(m_Device, D3D12_HIT_GROUP_DESC, rayTracingPipelineDesc.shaderGroupDescNum);
    m_ShaderGroupNames.reserve(rayTracingPipelineDesc.shaderGroupDescNum);
    for (uint32_t i = 0; i < rayTracingPipelineDesc.shaderGroupDescNum; i++) {
        bool isHitGroup = true;
        bool hasIntersectionShader = false;
        std::wstring shaderIndentifierName;
        for (uint32_t j = 0; j < GetCountOf(rayTracingPipelineDesc.shaderGroupDescs[i].shaderIndices); j++) {
            const uint32_t& shaderIndex = rayTracingPipelineDesc.shaderGroupDescs[i].shaderIndices[j];
            if (shaderIndex) {
                uint32_t lookupIndex = shaderIndex - 1;
                const ShaderDesc& shader = rayTracingPipelineDesc.shaderLibrary->shaders[lookupIndex];
                const std::wstring& entryPointName = wEntryPointNames[lookupIndex];
                if (shader.stage == StageBits::RAYGEN_SHADER || shader.stage == StageBits::MISS_SHADER || shader.stage == StageBits::CALLABLE_SHADER) {
                    shaderIndentifierName = entryPointName;
                    isHitGroup = false;
                    break;
                }

                switch (shader.stage) {
                    case StageBits::INTERSECTION_SHADER:
                        hitGroups[hitGroupNum].IntersectionShaderImport = entryPointName.c_str();
                        hasIntersectionShader = true;
                        break;
                    case StageBits::CLOSEST_HIT_SHADER:
                        hitGroups[hitGroupNum].ClosestHitShaderImport = entryPointName.c_str();
                        break;
                    case StageBits::ANY_HIT_SHADER:
                        hitGroups[hitGroupNum].AnyHitShaderImport = entryPointName.c_str();
                        break;
                }

                shaderIndentifierName = std::to_wstring(i);
            }
        }

        m_ShaderGroupNames.push_back(shaderIndentifierName);

        if (isHitGroup) {
            hitGroups[hitGroupNum].HitGroupExport = m_ShaderGroupNames[i].c_str();
            hitGroups[hitGroupNum].Type = hasIntersectionShader ? D3D12_HIT_GROUP_TYPE_PROCEDURAL_PRIMITIVE : D3D12_HIT_GROUP_TYPE_TRIANGLES;

            stateSubobjects[stateSubobjectNum].Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP;
            stateSubobjects[stateSubobjectNum].pDesc = &hitGroups[hitGroupNum++];
            stateSubobjectNum++;
        }
    }

    D3D12_STATE_OBJECT_DESC stateObjectDesc = {};
    stateObjectDesc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
    stateObjectDesc.NumSubobjects = stateSubobjectNum;
    stateObjectDesc.pSubobjects = stateSubobjectNum ? &stateSubobjects[0] : nullptr;

    HRESULT hr = m_Device->CreateStateObject(&stateObjectDesc, IID_PPV_ARGS(&m_StateObject));
    RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device5::CreateStateObject()");

    m_StateObject->QueryInterface(&m_StateObjectProperties);

    return Result::SUCCESS;
}

void PipelineD3D12::Bind(ID3D12GraphicsCommandList* graphicsCommandList, D3D12_PRIMITIVE_TOPOLOGY& primitiveTopology) const {
    if (m_StateObject)
        ((ID3D12GraphicsCommandList4*)graphicsCommandList)->SetPipelineState1(m_StateObject);
    else
        graphicsCommandList->SetPipelineState(m_PipelineState);

    if (m_IsGraphicsPipeline) {
        primitiveTopology = m_PrimitiveTopology;
        graphicsCommandList->IASetPrimitiveTopology(m_PrimitiveTopology);
    }
}

NRI_INLINE Result PipelineD3D12::WriteShaderGroupIdentifiers(uint32_t baseShaderGroupIndex, uint32_t shaderGroupNum, void* buffer) const {
    uint8_t* byteBuffer = (uint8_t*)buffer;
    for (uint32_t i = 0; i < shaderGroupNum; i++) {
        memcpy(byteBuffer + i * D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT, m_StateObjectProperties->GetShaderIdentifier(m_ShaderGroupNames[baseShaderGroupIndex + i].c_str()),
            (size_t)m_Device.GetDesc().rayTracingShaderGroupIdentifierSize);
    }

    return Result::SUCCESS;
}
