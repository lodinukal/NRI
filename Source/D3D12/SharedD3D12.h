// © 2021 NVIDIA Corporation

#pragma once

#include <d3d12.h>

#define D3D_HIGHEST_SHADER_MODEL D3D_SHADER_MODEL_6_7

#include "SharedExternal.h"

typedef size_t DescriptorPointerCPU;
typedef uint64_t DescriptorPointerGPU;
typedef uint16_t HeapIndexType;
typedef uint16_t HeapOffsetType;

struct MemoryTypeInfo {
    uint16_t heapFlags;
    uint8_t heapType;
    bool mustBeDedicated;
};

inline nri::MemoryType Pack(const MemoryTypeInfo& memoryTypeInfo) {
    return *(nri::MemoryType*)&memoryTypeInfo;
}

inline MemoryTypeInfo Unpack(const nri::MemoryType& memoryType) {
    return *(MemoryTypeInfo*)&memoryType;
}

static_assert(sizeof(MemoryTypeInfo) == sizeof(nri::MemoryType), "Must be equal");

namespace nri {
enum DescriptorHeapType : uint32_t {
    RESOURCE = 0,
    SAMPLER,
    MAX_NUM
};

struct DescriptorHandle {
    HeapIndexType heapIndex;
    HeapOffsetType heapOffset;
};

struct DescriptorHeapDesc {
    ComPtr<ID3D12DescriptorHeap> heap;
    DescriptorPointerCPU basePointerCPU = 0;
    DescriptorPointerGPU basePointerGPU = 0;
    uint32_t descriptorSize = 0;
    uint32_t num = 0;
};

void GetResourceDesc(D3D12_RESOURCE_DESC* desc, const BufferDesc& bufferDesc);
void GetResourceDesc(D3D12_RESOURCE_DESC* desc, const TextureDesc& textureDesc);
void ConvertGeometryDescs(D3D12_RAYTRACING_GEOMETRY_DESC* geometryDescs, const GeometryObject* geometryObjects, uint32_t geometryObjectNum);
bool GetTextureDesc(const TextureD3D12Desc& textureD3D12Desc, TextureDesc& textureDesc);
bool GetBufferDesc(const BufferD3D12Desc& bufferD3D12Desc, BufferDesc& bufferDesc);
uint64_t GetMemorySizeD3D12(const MemoryD3D12Desc& memoryD3D12Desc);
D3D12_RESIDENCY_PRIORITY ConvertPriorityD3D12(float priority);

D3D12_HEAP_TYPE GetHeapType(MemoryLocation memoryLocation);
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE GetAccelerationStructureTypeD3D12(AccelerationStructureType accelerationStructureType);
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS GetAccelerationStructureBuildFlagsD3D12(AccelerationStructureBuildBits accelerationStructureBuildFlags);
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE GetCopyModeD3D12(CopyMode copyMode);
D3D12_RESOURCE_FLAGS GetBufferFlags(BufferUsageBits bufferUsage);
D3D12_RESOURCE_FLAGS GetTextureFlags(TextureUsageBits textureUsage);
D3D12_FILTER GetFilterIsotropic(Filter mip, Filter magnification, Filter minification, FilterExt filterExt, bool useComparison);
D3D12_FILTER GetFilterAnisotropic(FilterExt filterExt, bool useComparison);
D3D12_TEXTURE_ADDRESS_MODE GetAddressMode(AddressMode addressMode);
D3D12_COMPARISON_FUNC GetComparisonFunc(CompareFunc compareFunc);
D3D12_COMMAND_LIST_TYPE GetCommandListType(CommandQueueType commandQueueType);
D3D12_DESCRIPTOR_HEAP_TYPE GetDescriptorHeapType(DescriptorType descriptorType);
D3D12_HEAP_FLAGS GetHeapFlags(MemoryType memoryType);
D3D12_PRIMITIVE_TOPOLOGY_TYPE GetPrimitiveTopologyType(Topology topology);
D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(Topology topology, uint8_t tessControlPointNum);
D3D12_FILL_MODE GetFillMode(FillMode fillMode);
D3D12_CULL_MODE GetCullModeD3D12(CullMode cullMode);
D3D12_STENCIL_OP GetStencilOpD3D12(StencilFunc stencilFunc);
UINT8 GetRenderTargetWriteMask(ColorWriteBits colorWriteMask);
D3D12_LOGIC_OP GetLogicOpD3D12(LogicFunc logicFunc);
D3D12_BLEND GetBlend(BlendFactor blendFactor);
D3D12_BLEND_OP GetBlendOpD3D12(BlendFunc blendFunc);
D3D12_SHADER_VISIBILITY GetShaderVisibility(StageBits shaderStage);
D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangesType(DescriptorType descriptorType);
D3D12_RESOURCE_DIMENSION GetResourceDimension(TextureType textureType);
D3D12_SHADING_RATE GetShadingRateD3D12(ShadingRate shadingRate);
D3D12_SHADING_RATE_COMBINER GetShadingRateD3D12Combiner(ShadingRateCombiner shadingRateCombiner);
} // namespace nri

#if NRI_USE_EXT_LIBS
#    include "amdags/ags_lib/inc/amd_ags.h"
#    include "nvapi/nvShaderExtnEnums.h"
#    include "nvapi/nvapi.h"
#endif

namespace d3d12 {
#include "D3DExt.h"
}

// VMA
#if defined(__GNUC__)
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#elif defined(__clang__)
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wunused-parameter"
#else
#    pragma warning(push)
#    pragma warning(disable : 4100) // unreferenced formal parameter
#endif

#define D3D12MA_D3D12_HEADERS_ALREADY_INCLUDED
#include "memalloc/D3D12MemAlloc.h"

#if defined(__GNUC__)
#    pragma GCC diagnostic pop
#elif defined(__clang__)
#    pragma clang diagnostic pop
#elif defined(MSVC)
#    pragma warning(pop)
#endif

#include "DeviceD3D12.h"
