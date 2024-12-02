// © 2021 NVIDIA Corporation

void nri::GetResourceDesc(D3D12_RESOURCE_DESC* desc, const TextureDesc& textureDesc) {
    uint16_t blockWidth = (uint16_t)GetFormatProps(textureDesc.format).blockWidth;
    const DxgiFormat& dxgiFormat = GetDxgiFormat(textureDesc.format);

    *desc = {};
    desc->Dimension = GetResourceDimension(textureDesc.type);
    desc->Width = Align(textureDesc.width, blockWidth);
    desc->Height = Align(std::max(textureDesc.height, (Dim_t)1), blockWidth);
    desc->DepthOrArraySize = std::max(textureDesc.type == TextureType::TEXTURE_3D ? textureDesc.depth : textureDesc.layerNum, (Dim_t)1);
    desc->MipLevels = std::max(textureDesc.mipNum, (Mip_t)1);
    desc->Format = (textureDesc.usage & nri::TextureUsageBits::SHADING_RATE_ATTACHMENT) ? dxgiFormat.typed : dxgiFormat.typeless;
    desc->SampleDesc.Count = std::max(textureDesc.sampleNum, (Sample_t)1);
    desc->Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    desc->Flags = GetTextureFlags(textureDesc.usage);
}

Result TextureD3D12::Create(const TextureDesc& textureDesc) {
    m_Desc = FixTextureDesc(textureDesc);

    return Result::SUCCESS;
}

Result TextureD3D12::Create(const TextureD3D12Desc& textureDesc) {
    if (textureDesc.desc)
        m_Desc = FixTextureDesc(*textureDesc.desc);
    else if (!GetTextureDesc(textureDesc, m_Desc))
        return Result::INVALID_ARGUMENT;

    m_Texture = (ID3D12ResourceBest*)textureDesc.d3d12Resource;

    return Result::SUCCESS;
}

Result TextureD3D12::BindMemory(const MemoryD3D12* memory, uint64_t offset) {
    // Texture was already created externally
    if (m_Texture)
        return Result::SUCCESS;

    D3D12_CLEAR_VALUE clearValue = {GetDxgiFormat(m_Desc.format).typed};

    const D3D12_HEAP_DESC& heapDesc = memory->GetHeapDesc();
    // STATE_CREATION ERROR #640: CREATERESOURCEANDHEAP_INVALIDHEAPMISCFLAGS
    D3D12_HEAP_FLAGS heapFlagsFixed = heapDesc.Flags & ~(D3D12_HEAP_FLAG_DENY_NON_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_RT_DS_TEXTURES | D3D12_HEAP_FLAG_DENY_BUFFERS);

#ifdef NRI_USE_AGILITY_SDK
    if (m_Device.GetVersion() >= 10) {
        D3D12_RESOURCE_DESC1 desc1 = {};
        GetResourceDesc((D3D12_RESOURCE_DESC*)&desc1, m_Desc);

        const D3D12_BARRIER_LAYOUT initialLayout = D3D12_BARRIER_LAYOUT_COMMON;
        bool isRenderableSurface = desc1.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        uint32_t castableFormatNum = 0;
        DXGI_FORMAT* castableFormats = nullptr; // TODO: add castable formats, see options12.RelaxedFormatCastingSupported

        if (memory->IsDummy()) {
            HRESULT hr = m_Device->CreateCommittedResource3(&heapDesc.Properties, heapFlagsFixed, &desc1, initialLayout, isRenderableSurface ? &clearValue : nullptr, nullptr,
                castableFormatNum, castableFormats, IID_PPV_ARGS(&m_Texture));
            RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device10::CreateCommittedResource3()");
        } else {
            HRESULT hr = m_Device->CreatePlacedResource2(
                *memory, offset, &desc1, initialLayout, isRenderableSurface ? &clearValue : nullptr, castableFormatNum, castableFormats, IID_PPV_ARGS(&m_Texture));
            RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device10::CreatePlacedResource2()");
        }
    } else
#endif
    {
        D3D12_RESOURCE_DESC desc = {};
        GetResourceDesc(&desc, m_Desc);

        const D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
        bool isRenderableSurface = desc.Flags & (D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        if (memory->IsDummy()) {
            HRESULT hr = m_Device->CreateCommittedResource(&heapDesc.Properties, heapFlagsFixed, &desc, initialState, isRenderableSurface ? &clearValue : nullptr, IID_PPV_ARGS(&m_Texture));
            RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device::CreateCommittedResource()");
        } else {
            HRESULT hr = m_Device->CreatePlacedResource(*memory, offset, &desc, initialState, isRenderableSurface ? &clearValue : nullptr, IID_PPV_ARGS(&m_Texture));
            RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device::CreatePlacedResource()");
        }
    }

    // Priority
    D3D12_RESIDENCY_PRIORITY residencyPriority = (D3D12_RESIDENCY_PRIORITY)ConvertPriorityD3D12(memory->GetPriority());
    if (m_Device.GetVersion() >= 1 && residencyPriority != 0) {
        ID3D12Pageable* obj = m_Texture.GetInterface();
        HRESULT hr = m_Device->SetResidencyPriority(1, &obj, &residencyPriority);
        RETURN_ON_BAD_HRESULT(&m_Device, hr, "ID3D12Device1::SetResidencyPriority()");
    }

    return Result::SUCCESS;
}
