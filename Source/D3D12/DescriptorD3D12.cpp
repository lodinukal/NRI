// © 2021 NVIDIA Corporation

#include "SharedD3D12.h"

#include "AccelerationStructureD3D12.h"
#include "BufferD3D12.h"
#include "DescriptorD3D12.h"
#include "TextureD3D12.h"

using namespace nri;

static inline DXGI_FORMAT GetShaderFormatForDepth(DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_D16_UNORM:
            return DXGI_FORMAT_R16_UNORM;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        case DXGI_FORMAT_D32_FLOAT:
            return DXGI_FORMAT_R32_FLOAT;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        default:
            return format;
    }
}

Result DescriptorD3D12::Create(const Texture1DViewDesc& textureViewDesc) {
    const TextureD3D12& texture = (TextureD3D12&)*textureViewDesc.texture;
    DXGI_FORMAT format = GetDxgiFormat(textureViewDesc.format).typed;

    const TextureDesc& textureDesc = texture.GetDesc();
    Mip_t remainingMips = textureViewDesc.mipNum == REMAINING_MIPS ? (textureDesc.mipNum - textureViewDesc.mipOffset) : textureViewDesc.mipNum;
    Dim_t remainingLayers = textureViewDesc.layerNum == REMAINING_LAYERS ? (textureDesc.layerNum - textureViewDesc.layerOffset) : textureViewDesc.layerNum;

    switch (textureViewDesc.viewType) {
        case Texture1DViewType::SHADER_RESOURCE_1D: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Texture1D.MostDetailedMip = textureViewDesc.mipOffset;
            desc.Texture1D.MipLevels = remainingMips;
            desc.Texture1D.ResourceMinLODClamp = 0;

            return CreateShaderResourceView(texture, desc);
        }
        case Texture1DViewType::SHADER_RESOURCE_1D_ARRAY: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1DARRAY;
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Texture1DArray.MostDetailedMip = textureViewDesc.mipOffset;
            desc.Texture1DArray.MipLevels = remainingMips;
            desc.Texture1DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture1DArray.ArraySize = remainingLayers;
            desc.Texture1DArray.ResourceMinLODClamp = 0;

            return CreateShaderResourceView(texture, desc);
        }
        case Texture1DViewType::SHADER_RESOURCE_STORAGE_1D: {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
            desc.Texture1D.MipSlice = textureViewDesc.mipOffset;

            return CreateUnorderedAccessView(texture, desc, textureViewDesc.format);
        }
        case Texture1DViewType::SHADER_RESOURCE_STORAGE_1D_ARRAY: {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipSlice = textureViewDesc.mipOffset;
            desc.Texture1DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture1DArray.ArraySize = remainingLayers;

            return CreateUnorderedAccessView(texture, desc, textureViewDesc.format);
        }
        case Texture1DViewType::COLOR_ATTACHMENT: {
            D3D12_RENDER_TARGET_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipSlice = textureViewDesc.mipOffset;
            desc.Texture1DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture1DArray.ArraySize = remainingLayers;

            return CreateRenderTargetView(texture, desc);
        }
        case Texture1DViewType::DEPTH_STENCIL_ATTACHMENT: {
            D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1DARRAY;
            desc.Texture1DArray.MipSlice = textureViewDesc.mipOffset;
            desc.Texture1DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture1DArray.ArraySize = remainingLayers;

            if (textureViewDesc.flags & ResourceViewBits::READONLY_DEPTH)
                desc.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
            if (textureViewDesc.flags & ResourceViewBits::READONLY_STENCIL)
                desc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;

            return CreateDepthStencilView(texture, desc);
        }
    }

    return Result::FAILURE;
}

Result DescriptorD3D12::Create(const Texture2DViewDesc& textureViewDesc) {
    const TextureD3D12& texture = (TextureD3D12&)*textureViewDesc.texture;
    DXGI_FORMAT format = GetDxgiFormat(textureViewDesc.format).typed;

    const TextureDesc& textureDesc = texture.GetDesc();
    Mip_t remainingMips = textureViewDesc.mipNum == REMAINING_MIPS ? (textureDesc.mipNum - textureViewDesc.mipOffset) : textureViewDesc.mipNum;
    Dim_t remainingLayers = textureViewDesc.layerNum == REMAINING_LAYERS ? (textureDesc.layerNum - textureViewDesc.layerOffset) : textureViewDesc.layerNum;

    switch (textureViewDesc.viewType) {
        case Texture2DViewType::SHADER_RESOURCE_2D: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = GetShaderFormatForDepth(format);
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            if (textureDesc.sampleNum > 1) {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMS;
            } else {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MostDetailedMip = textureViewDesc.mipOffset;
                desc.Texture2D.MipLevels = remainingMips;
                desc.Texture2D.PlaneSlice = 0;
                desc.Texture2D.ResourceMinLODClamp = 0;
            }

            return CreateShaderResourceView(texture, desc);
        }
        case Texture2DViewType::SHADER_RESOURCE_2D_ARRAY: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = GetShaderFormatForDepth(format);
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            if (textureDesc.sampleNum > 1) {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DMSARRAY;
                desc.Texture2DMSArray.FirstArraySlice = textureViewDesc.layerOffset;
                desc.Texture2DMSArray.ArraySize = remainingLayers;
            } else {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
                desc.Texture2DArray.MostDetailedMip = textureViewDesc.mipOffset;
                desc.Texture2DArray.MipLevels = remainingMips;
                desc.Texture2DArray.FirstArraySlice = textureViewDesc.layerOffset;
                desc.Texture2DArray.ArraySize = remainingLayers;
                desc.Texture2D.PlaneSlice = 0;
                desc.Texture2DArray.ResourceMinLODClamp = 0;
            }

            return CreateShaderResourceView(texture, desc);
        }
        case Texture2DViewType::SHADER_RESOURCE_CUBE: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = GetShaderFormatForDepth(format);
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
            desc.TextureCube.MostDetailedMip = textureViewDesc.mipOffset;
            desc.TextureCube.MipLevels = remainingMips;

            return CreateShaderResourceView(texture, desc);
        }
        case Texture2DViewType::SHADER_RESOURCE_CUBE_ARRAY: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = GetShaderFormatForDepth(format);
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBEARRAY;
            desc.TextureCubeArray.MostDetailedMip = textureViewDesc.mipOffset;
            desc.TextureCubeArray.MipLevels = remainingMips;
            desc.TextureCubeArray.First2DArrayFace = textureViewDesc.layerOffset;
            desc.TextureCubeArray.NumCubes = remainingLayers / 6;

            return CreateShaderResourceView(texture, desc);
        }
        case Texture2DViewType::SHADER_RESOURCE_STORAGE_2D: {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            desc.Texture2D.MipSlice = textureViewDesc.mipOffset;
            desc.Texture2D.PlaneSlice = 0;

            return CreateUnorderedAccessView(texture, desc, textureViewDesc.format);
        }
        case Texture2DViewType::SHADER_RESOURCE_STORAGE_2D_ARRAY: {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipSlice = textureViewDesc.mipOffset;
            desc.Texture2DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture2DArray.ArraySize = remainingLayers;
            desc.Texture2DArray.PlaneSlice = 0;

            return CreateUnorderedAccessView(texture, desc, textureViewDesc.format);
        }
        case Texture2DViewType::COLOR_ATTACHMENT: {
            D3D12_RENDER_TARGET_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipSlice = textureViewDesc.mipOffset;
            desc.Texture2DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture2DArray.ArraySize = remainingLayers;
            desc.Texture2DArray.PlaneSlice = 0;

            return CreateRenderTargetView(texture, desc);
        }
        case Texture2DViewType::DEPTH_STENCIL_ATTACHMENT: {
            D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
            desc.Texture2DArray.MipSlice = textureViewDesc.mipOffset;
            desc.Texture2DArray.FirstArraySlice = textureViewDesc.layerOffset;
            desc.Texture2DArray.ArraySize = remainingLayers;

            if (textureViewDesc.flags & ResourceViewBits::READONLY_DEPTH)
                desc.Flags |= D3D12_DSV_FLAG_READ_ONLY_DEPTH;
            if (textureViewDesc.flags & ResourceViewBits::READONLY_STENCIL)
                desc.Flags |= D3D12_DSV_FLAG_READ_ONLY_STENCIL;

            return CreateDepthStencilView(texture, desc);
        }
        case Texture2DViewType::SHADING_RATE_ATTACHMENT: {
            m_Resource = texture; // a resource view is not needed
            return Result::SUCCESS;
        }
    }

    return Result::FAILURE;
}

Result DescriptorD3D12::Create(const Texture3DViewDesc& textureViewDesc) {
    const TextureD3D12& texture = (TextureD3D12&)*textureViewDesc.texture;
    DXGI_FORMAT format = GetDxgiFormat(textureViewDesc.format).typed;

    const TextureDesc& textureDesc = texture.GetDesc();
    Mip_t remainingMips = textureViewDesc.mipNum == REMAINING_MIPS ? (textureDesc.mipNum - textureViewDesc.mipOffset) : textureViewDesc.mipNum;

    switch (textureViewDesc.viewType) {
        case Texture3DViewType::SHADER_RESOURCE_3D: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Texture3D.MostDetailedMip = textureViewDesc.mipOffset;
            desc.Texture3D.MipLevels = remainingMips;
            desc.Texture3D.ResourceMinLODClamp = 0;

            return CreateShaderResourceView(texture, desc);
        }
        case Texture3DViewType::SHADER_RESOURCE_STORAGE_3D: {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipSlice = textureViewDesc.mipOffset;
            desc.Texture3D.FirstWSlice = textureViewDesc.sliceOffset;
            desc.Texture3D.WSize = textureViewDesc.sliceNum;

            return CreateUnorderedAccessView(texture, desc, textureViewDesc.format);
        }
        case Texture3DViewType::COLOR_ATTACHMENT: {
            D3D12_RENDER_TARGET_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
            desc.Texture3D.MipSlice = textureViewDesc.mipOffset;
            desc.Texture3D.FirstWSlice = textureViewDesc.sliceOffset;
            desc.Texture3D.WSize = textureViewDesc.sliceNum;

            return CreateRenderTargetView(texture, desc);
        }
    }

    return Result::FAILURE;
}

Result DescriptorD3D12::Create(const BufferViewDesc& bufferViewDesc) {
    const BufferD3D12& buffer = *((BufferD3D12*)bufferViewDesc.buffer);
    const BufferDesc& bufferDesc = buffer.GetDesc();
    uint64_t size = bufferViewDesc.size == WHOLE_SIZE ? bufferDesc.size : bufferViewDesc.size;

    DXGI_FORMAT format = GetDxgiFormat(bufferViewDesc.format).typed;
    const FormatProps& formatProps = GetFormatProps(bufferViewDesc.format);
    uint32_t elementSize = bufferDesc.structureStride ? bufferDesc.structureStride : formatProps.stride;
    uint64_t elementOffset = (uint32_t)(bufferViewDesc.offset / elementSize);
    uint32_t elementNum = (uint32_t)(size / elementSize);

    switch (bufferViewDesc.viewType) {
        case BufferViewType::CONSTANT: {
            D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
            desc.BufferLocation = buffer.GetPointerGPU() + bufferViewDesc.offset;
            desc.SizeInBytes = (uint32_t)size;

            return CreateConstantBufferView(desc);
        }
        case BufferViewType::SHADER_RESOURCE: {
            D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            desc.Buffer.FirstElement = elementOffset;
            desc.Buffer.NumElements = elementNum;
            desc.Buffer.StructureByteStride = bufferDesc.structureStride;
            desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

            return CreateShaderResourceView(buffer, desc);
        }
        case BufferViewType::SHADER_RESOURCE_STORAGE: {
            D3D12_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = format;
            desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = elementOffset;
            desc.Buffer.NumElements = elementNum;
            desc.Buffer.StructureByteStride = bufferDesc.structureStride;
            desc.Buffer.CounterOffsetInBytes = 0;
            desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

            return CreateUnorderedAccessView(buffer, desc, bufferViewDesc.format);
        }
    }

    return Result::FAILURE;
}

Result DescriptorD3D12::Create(const AccelerationStructure& accelerationStructure) {
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.RaytracingAccelerationStructure.Location = ((AccelerationStructureD3D12&)accelerationStructure).GetHandle();

    return CreateShaderResourceView(nullptr, desc);
}

Result DescriptorD3D12::Create(const SamplerDesc& samplerDesc) {
    bool useAnisotropy = samplerDesc.anisotropy > 1 ? true : false;
    bool useComparison = samplerDesc.compareFunc != CompareFunc::NONE;

    D3D12_SAMPLER_DESC desc = {};
    desc.Filter = useAnisotropy
        ? GetFilterAnisotropic(samplerDesc.filters.ext, useComparison)
        : GetFilterIsotropic(samplerDesc.filters.mip, samplerDesc.filters.mag, samplerDesc.filters.min, samplerDesc.filters.ext, useComparison);
    desc.AddressU = GetAddressMode(samplerDesc.addressModes.u);
    desc.AddressV = GetAddressMode(samplerDesc.addressModes.v);
    desc.AddressW = GetAddressMode(samplerDesc.addressModes.w);
    desc.MipLODBias = samplerDesc.mipBias;
    desc.MaxAnisotropy = samplerDesc.anisotropy;
    desc.ComparisonFunc = GetComparisonFunc(samplerDesc.compareFunc);
    desc.MinLOD = samplerDesc.mipMin;
    desc.MaxLOD = samplerDesc.mipMax;

    if (samplerDesc.borderColor == BorderColor::FLOAT_OPAQUE_BLACK || samplerDesc.borderColor == BorderColor::INT_OPAQUE_BLACK)
        desc.BorderColor[3] = 1.0f;
    else if (samplerDesc.borderColor == BorderColor::FLOAT_OPAQUE_WHITE || samplerDesc.borderColor == BorderColor::INT_OPAQUE_WHITE) {
        desc.BorderColor[0] = 1.0f;
        desc.BorderColor[1] = 1.0f;
        desc.BorderColor[2] = 1.0f;
        desc.BorderColor[3] = 1.0f;
    }

    m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;

    Result result = m_Device.GetDescriptorHandle(m_HeapType, m_Handle);
    if (result == Result::SUCCESS) {
        m_DescriptorPointerCPU = m_Device.GetDescriptorPointerCPU(m_Handle);
        m_Device->CreateSampler(&desc, {m_DescriptorPointerCPU});
    }

    return result;
}

Result DescriptorD3D12::CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC& desc) {
    m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    Result result = m_Device.GetDescriptorHandle(m_HeapType, m_Handle);
    if (result == Result::SUCCESS) {
        m_DescriptorPointerCPU = m_Device.GetDescriptorPointerCPU(m_Handle);
        m_Device->CreateConstantBufferView(&desc, {m_DescriptorPointerCPU});
        m_BufferLocation = desc.BufferLocation;
    }

    return result;
}

Result DescriptorD3D12::CreateShaderResourceView(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& desc) {
    m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    Result result = m_Device.GetDescriptorHandle(m_HeapType, m_Handle);
    if (result == Result::SUCCESS) {
        m_DescriptorPointerCPU = m_Device.GetDescriptorPointerCPU(m_Handle);
        m_Device->CreateShaderResourceView(resource, &desc, {m_DescriptorPointerCPU});
        m_Resource = resource;
    }

    return result;
}

Result DescriptorD3D12::CreateUnorderedAccessView(ID3D12Resource* resource, const D3D12_UNORDERED_ACCESS_VIEW_DESC& desc, Format format) {
    m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

    Result result = m_Device.GetDescriptorHandle(m_HeapType, m_Handle);
    if (result == Result::SUCCESS) {
        m_DescriptorPointerCPU = m_Device.GetDescriptorPointerCPU(m_Handle);
        m_Device->CreateUnorderedAccessView(resource, nullptr, &desc, {m_DescriptorPointerCPU});
        m_Resource = resource;
        m_IsIntegerFormat = GetFormatProps(format).isInteger;
    }

    return result;
}

Result DescriptorD3D12::CreateRenderTargetView(ID3D12Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC& desc) {
    m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    Result result = m_Device.GetDescriptorHandle(m_HeapType, m_Handle);
    if (result == Result::SUCCESS) {
        m_DescriptorPointerCPU = m_Device.GetDescriptorPointerCPU(m_Handle);
        m_Device->CreateRenderTargetView(resource, &desc, {m_DescriptorPointerCPU});
        m_Resource = resource;
    }

    return result;
}

Result DescriptorD3D12::CreateDepthStencilView(ID3D12Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC& desc) {
    m_HeapType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

    Result result = m_Device.GetDescriptorHandle(m_HeapType, m_Handle);
    if (result == Result::SUCCESS) {
        m_DescriptorPointerCPU = m_Device.GetDescriptorPointerCPU(m_Handle);
        m_Device->CreateDepthStencilView(resource, &desc, {m_DescriptorPointerCPU});
        m_Resource = resource;
    }

    return result;
}

#include "DescriptorD3D12.hpp"
