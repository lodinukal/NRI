// © 2021 NVIDIA Corporation

#include "SharedExternal.h"

using namespace nri;

template <typename T>
constexpr T* DummyObject() {
    return (T*)(size_t)(1);
}

struct DeviceNONE final : public DeviceBase {
    inline DeviceNONE(const CallbackInterface& callbacks, StdAllocator<uint8_t>& stdAllocator)
        : DeviceBase(callbacks, stdAllocator) {
        m_Desc.uploadBufferTextureRowAlignment = 1;
        m_Desc.uploadBufferTextureSliceAlignment = 1;
        m_Desc.typedBufferOffsetAlignment = 1;
        m_Desc.constantBufferOffsetAlignment = 1;
        m_Desc.storageBufferOffsetAlignment = 1;
        m_Desc.rayTracingShaderTableAlignment = 1;
        m_Desc.rayTracingScratchAlignment = 1;

        m_Desc.isSwapChainSupported = true;
        m_Desc.isRayTracingSupported = true;
        m_Desc.isMeshShaderSupported = true;
        m_Desc.isLowLatencySupported = true;

        m_Desc.graphicsAPI = GraphicsAPI::NONE;
        m_Desc.nriVersionMajor = NRI_VERSION_MAJOR;
        m_Desc.nriVersionMinor = NRI_VERSION_MINOR;
    }

    inline ~DeviceNONE() {
    }

    inline const DeviceDesc& GetDesc() const {
        return m_Desc;
    }

    inline void Destruct() {
    }

    Result FillFunctionTable(CoreInterface& table) const;
    Result FillFunctionTable(HelperInterface& table) const;
    Result FillFunctionTable(LowLatencyInterface& table) const;
    Result FillFunctionTable(MeshShaderInterface& table) const;
    Result FillFunctionTable(RayTracingInterface& table) const;
    Result FillFunctionTable(StreamerInterface& table) const;
    Result FillFunctionTable(SwapChainInterface& table) const;
    Result FillFunctionTable(ResourceAllocatorInterface& table) const;

private:
    DeviceDesc m_Desc = {};
};

Result CreateDeviceNONE(const DeviceCreationDesc& deviceCreationDesc, DeviceBase*& device) {
    StdAllocator<uint8_t> allocator(deviceCreationDesc.allocationCallbacks);
    DeviceNONE* impl = Allocate<DeviceNONE>(allocator, deviceCreationDesc.callbackInterface, allocator);

    if (!impl) {
        Destroy(allocator, impl);
        device = DummyObject<DeviceBase>();

        return Result::FAILURE;
    }

    device = (DeviceBase*)impl;

    return Result::SUCCESS;
}

//============================================================================================================================================================================================
#pragma region[  Core  ]

static const DeviceDesc& NRI_CALL GetDeviceDesc(const Device& device) {
    return ((DeviceNONE&)device).GetDesc();
}

static const BufferDesc& NRI_CALL GetBufferDesc(const Buffer&) {
    static const BufferDesc bufferDesc = {};

    return bufferDesc;
}

static const TextureDesc& NRI_CALL GetTextureDesc(const Texture&) {
    static const TextureDesc textureDesc = {};

    return textureDesc;
}

static FormatSupportBits NRI_CALL GetFormatSupport(const Device&, Format) {
    return (FormatSupportBits)(-1);
}

static uint32_t NRI_CALL GetQuerySize(const QueryPool&) {
    return 0;
}

static void NRI_CALL GetBufferMemoryDesc(const Device&, const BufferDesc&, MemoryLocation, MemoryDesc& memoryDesc) {
    memoryDesc = {};
}

static void NRI_CALL GetTextureMemoryDesc(const Device&, const TextureDesc&, MemoryLocation, MemoryDesc& memoryDesc) {
    memoryDesc = {};
}

static Result NRI_CALL GetCommandQueue(Device&, CommandQueueType, CommandQueue*& commandQueue) {
    commandQueue = DummyObject<CommandQueue>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateCommandAllocator(const CommandQueue&, CommandAllocator*& commandAllocator) {
    commandAllocator = DummyObject<CommandAllocator>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateCommandBuffer(CommandAllocator&, CommandBuffer*& commandBuffer) {
    commandBuffer = DummyObject<CommandBuffer>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateDescriptorPool(Device&, const DescriptorPoolDesc&, DescriptorPool*& descriptorPool) {
    descriptorPool = DummyObject<DescriptorPool>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateBuffer(Device&, const BufferDesc&, Buffer*& buffer) {
    buffer = DummyObject<Buffer>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateTexture(Device&, const TextureDesc&, Texture*& texture) {
    texture = DummyObject<Texture>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateBufferView(const BufferViewDesc&, Descriptor*& bufferView) {
    bufferView = DummyObject<Descriptor>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateTexture1DView(const Texture1DViewDesc&, Descriptor*& textureView) {
    textureView = DummyObject<Descriptor>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateTexture2DView(const Texture2DViewDesc&, Descriptor*& textureView) {
    textureView = DummyObject<Descriptor>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateTexture3DView(const Texture3DViewDesc&, Descriptor*& textureView) {
    textureView = DummyObject<Descriptor>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateSampler(Device&, const SamplerDesc&, Descriptor*& sampler) {
    sampler = DummyObject<Descriptor>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreatePipelineLayout(Device&, const PipelineLayoutDesc&, PipelineLayout*& pipelineLayout) {
    pipelineLayout = DummyObject<PipelineLayout>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateGraphicsPipeline(Device&, const GraphicsPipelineDesc&, Pipeline*& pipeline) {
    pipeline = DummyObject<Pipeline>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateComputePipeline(Device&, const ComputePipelineDesc&, Pipeline*& pipeline) {
    pipeline = DummyObject<Pipeline>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateFence(Device&, uint64_t, Fence*& fence) {
    fence = DummyObject<Fence>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateQueryPool(Device&, const QueryPoolDesc&, QueryPool*& queryPool) {
    queryPool = DummyObject<QueryPool>();

    return Result::SUCCESS;
}

static void NRI_CALL DestroyCommandBuffer(CommandBuffer&) {
}

static void NRI_CALL DestroyCommandAllocator(CommandAllocator&) {
}

static void NRI_CALL DestroyDescriptorPool(DescriptorPool&) {
}

static void NRI_CALL DestroyBuffer(Buffer&) {
}

static void NRI_CALL DestroyTexture(Texture&) {
}

static void NRI_CALL DestroyDescriptor(Descriptor&) {
}

static void NRI_CALL DestroyPipelineLayout(PipelineLayout&) {
}

static void NRI_CALL DestroyPipeline(Pipeline&) {
}

static void NRI_CALL DestroyQueryPool(QueryPool&) {
}

static void NRI_CALL DestroyFence(Fence&) {
}

static Result NRI_CALL AllocateMemory(Device&, const AllocateMemoryDesc&, Memory*& memory) {
    memory = DummyObject<Memory>();

    return Result::SUCCESS;
}

static Result NRI_CALL BindBufferMemory(Device&, const BufferMemoryBindingDesc*, uint32_t) {
    return Result::SUCCESS;
}

static Result NRI_CALL BindTextureMemory(Device&, const TextureMemoryBindingDesc*, uint32_t) {
    return Result::SUCCESS;
}

static void NRI_CALL FreeMemory(Memory&) {
}

static Result NRI_CALL BeginCommandBuffer(CommandBuffer&, const DescriptorPool*) {
    return Result::SUCCESS;
}

static void NRI_CALL CmdSetDescriptorPool(CommandBuffer&, const DescriptorPool&) {
}

static void NRI_CALL CmdSetDescriptorSet(CommandBuffer&, uint32_t, const DescriptorSet&, const uint32_t*) {
}

static void NRI_CALL CmdSetPipelineLayout(CommandBuffer&, const PipelineLayout&) {
}

static void NRI_CALL CmdSetPipeline(CommandBuffer&, const Pipeline&) {
}

static void NRI_CALL CmdSetRootConstants(CommandBuffer&, uint32_t, const void*, uint32_t) {
}

static void NRI_CALL CmdSetRootDescriptor(CommandBuffer&, uint32_t, Descriptor&) {
}

static void NRI_CALL CmdBarrier(CommandBuffer&, const BarrierGroupDesc&) {
}

static void NRI_CALL CmdSetIndexBuffer(CommandBuffer&, const Buffer&, uint64_t, IndexType) {
}

static void NRI_CALL CmdSetVertexBuffers(CommandBuffer&, uint32_t, uint32_t, const Buffer* const*, const uint64_t*) {
}

static void NRI_CALL CmdSetViewports(CommandBuffer&, const Viewport*, uint32_t) {
}

static void NRI_CALL CmdSetScissors(CommandBuffer&, const Rect*, uint32_t) {
}

static void NRI_CALL CmdSetStencilReference(CommandBuffer&, uint8_t, uint8_t) {
}

static void NRI_CALL CmdSetDepthBounds(CommandBuffer&, float, float) {
}

static void NRI_CALL CmdSetBlendConstants(CommandBuffer&, const Color32f&) {
}

static void NRI_CALL CmdSetSampleLocations(CommandBuffer&, const SampleLocation*, Sample_t, Sample_t) {
}

static void NRI_CALL CmdSetShadingRate(CommandBuffer&, const ShadingRateDesc&) {
}

static void NRI_CALL CmdSetDepthBias(CommandBuffer&, const DepthBiasDesc&) {
}

static void NRI_CALL CmdBeginRendering(CommandBuffer&, const AttachmentsDesc&) {
}

static void NRI_CALL CmdClearAttachments(CommandBuffer&, const ClearDesc*, uint32_t, const Rect*, uint32_t) {
}

static void NRI_CALL CmdDraw(CommandBuffer&, const DrawDesc&) {
}

static void NRI_CALL CmdDrawIndexed(CommandBuffer&, const DrawIndexedDesc&) {
}

static void NRI_CALL CmdDrawIndirect(CommandBuffer&, const Buffer&, uint64_t, uint32_t, uint32_t, const Buffer*, uint64_t) {
}

static void NRI_CALL CmdDrawIndexedIndirect(CommandBuffer&, const Buffer&, uint64_t, uint32_t, uint32_t, const Buffer*, uint64_t) {
}

static void NRI_CALL CmdEndRendering(CommandBuffer&) {
}

static void NRI_CALL CmdDispatch(CommandBuffer&, const DispatchDesc&) {
}

static void NRI_CALL CmdDispatchIndirect(CommandBuffer&, const Buffer&, uint64_t) {
}

static void NRI_CALL CmdCopyBuffer(CommandBuffer&, Buffer&, uint64_t, const Buffer&, uint64_t, uint64_t) {
}

static void NRI_CALL CmdCopyTexture(CommandBuffer&, Texture&, const TextureRegionDesc*, const Texture&, const TextureRegionDesc*) {
}

static void NRI_CALL CmdUploadBufferToTexture(CommandBuffer&, Texture&, const TextureRegionDesc&, const Buffer&, const TextureDataLayoutDesc&) {
}

static void NRI_CALL CmdReadbackTextureToBuffer(CommandBuffer&, Buffer&, const TextureDataLayoutDesc&, const Texture&, const TextureRegionDesc&) {
}

static void NRI_CALL CmdClearStorageBuffer(CommandBuffer&, const ClearStorageBufferDesc&) {
}

static void NRI_CALL CmdClearStorageTexture(CommandBuffer&, const ClearStorageTextureDesc&) {
}

static void NRI_CALL CmdResetQueries(CommandBuffer&, const QueryPool&, uint32_t, uint32_t) {
}

static void NRI_CALL CmdBeginQuery(CommandBuffer&, const QueryPool&, uint32_t) {
}

static void NRI_CALL CmdEndQuery(CommandBuffer&, const QueryPool&, uint32_t) {
}

static void NRI_CALL CmdCopyQueries(CommandBuffer&, const QueryPool&, uint32_t, uint32_t, Buffer&, uint64_t) {
}

static void NRI_CALL CmdBeginAnnotation(CommandBuffer&, const char*) {
}

static void NRI_CALL CmdEndAnnotation(CommandBuffer&) {
}

static Result NRI_CALL EndCommandBuffer(CommandBuffer&) {
    return Result::SUCCESS;
}

static void NRI_CALL QueueSubmit(CommandQueue&, const QueueSubmitDesc&) {
}

static void NRI_CALL Wait(Fence&, uint64_t) {
}

static uint64_t NRI_CALL GetFenceValue(Fence&) {
    return 0;
}

static void NRI_CALL UpdateDescriptorRanges(DescriptorSet&, uint32_t, uint32_t, const DescriptorRangeUpdateDesc*) {
}

static void NRI_CALL UpdateDynamicConstantBuffers(DescriptorSet&, uint32_t, uint32_t, const Descriptor* const*) {
}

static void NRI_CALL CopyDescriptorSet(DescriptorSet&, const DescriptorSetCopyDesc&) {
}

static Result NRI_CALL AllocateDescriptorSets(DescriptorPool&, const PipelineLayout&, uint32_t, DescriptorSet**, uint32_t, uint32_t) {
    return Result::SUCCESS;
}

static void NRI_CALL ResetDescriptorPool(DescriptorPool&) {
}

static void NRI_CALL ResetCommandAllocator(CommandAllocator&) {
}

static void* NRI_CALL MapBuffer(Buffer&, uint64_t, uint64_t) {
    return nullptr;
}

static void NRI_CALL UnmapBuffer(Buffer&) {
}

static void NRI_CALL SetDeviceDebugName(Device&, const char*) {
}

static void NRI_CALL SetFenceDebugName(Fence&, const char*) {
}

static void NRI_CALL SetDescriptorDebugName(Descriptor&, const char*) {
}

static void NRI_CALL SetPipelineDebugName(Pipeline&, const char*) {
}

static void NRI_CALL SetCommandBufferDebugName(CommandBuffer&, const char*) {
}

static void NRI_CALL SetBufferDebugName(Buffer&, const char*) {
}

static void NRI_CALL SetTextureDebugName(Texture&, const char*) {
}

static void NRI_CALL SetCommandQueueDebugName(CommandQueue&, const char*) {
}

static void NRI_CALL SetCommandAllocatorDebugName(CommandAllocator&, const char*) {
}

static void NRI_CALL SetDescriptorPoolDebugName(DescriptorPool&, const char*) {
}

static void NRI_CALL SetPipelineLayoutDebugName(PipelineLayout&, const char*) {
}

static void NRI_CALL SetQueryPoolDebugName(QueryPool&, const char*) {
}

static void NRI_CALL SetMemoryDebugName(Memory&, const char*) {
}

static void NRI_CALL SetDescriptorSetDebugName(DescriptorSet&, const char*) {
}

static void* NRI_CALL GetDeviceNativeObject(const Device&) {
    return nullptr;
}

static void* NRI_CALL GetCommandBufferNativeObject(const CommandBuffer&) {
    return nullptr;
}

static uint64_t NRI_CALL GetBufferNativeObject(const Buffer&) {
    return 0;
}

static uint64_t NRI_CALL GetDescriptorNativeObject(const Descriptor&) {
    return 0;
}

static uint64_t NRI_CALL GetTextureNativeObject(const Texture&) {
    return 0;
}

Result DeviceNONE::FillFunctionTable(CoreInterface& table) const {
    table.GetDeviceDesc = ::GetDeviceDesc;
    table.GetBufferDesc = ::GetBufferDesc;
    table.GetTextureDesc = ::GetTextureDesc;
    table.GetFormatSupport = ::GetFormatSupport;
    table.GetQuerySize = ::GetQuerySize;
    table.GetBufferMemoryDesc = ::GetBufferMemoryDesc;
    table.GetTextureMemoryDesc = ::GetTextureMemoryDesc;
    table.GetCommandQueue = ::GetCommandQueue;
    table.CreateCommandAllocator = ::CreateCommandAllocator;
    table.CreateCommandBuffer = ::CreateCommandBuffer;
    table.CreateDescriptorPool = ::CreateDescriptorPool;
    table.CreateBuffer = ::CreateBuffer;
    table.CreateTexture = ::CreateTexture;
    table.CreateBufferView = ::CreateBufferView;
    table.CreateTexture1DView = ::CreateTexture1DView;
    table.CreateTexture2DView = ::CreateTexture2DView;
    table.CreateTexture3DView = ::CreateTexture3DView;
    table.CreateSampler = ::CreateSampler;
    table.CreatePipelineLayout = ::CreatePipelineLayout;
    table.CreateGraphicsPipeline = ::CreateGraphicsPipeline;
    table.CreateComputePipeline = ::CreateComputePipeline;
    table.CreateQueryPool = ::CreateQueryPool;
    table.CreateFence = ::CreateFence;
    table.DestroyCommandAllocator = ::DestroyCommandAllocator;
    table.DestroyCommandBuffer = ::DestroyCommandBuffer;
    table.DestroyDescriptorPool = ::DestroyDescriptorPool;
    table.DestroyBuffer = ::DestroyBuffer;
    table.DestroyTexture = ::DestroyTexture;
    table.DestroyDescriptor = ::DestroyDescriptor;
    table.DestroyPipelineLayout = ::DestroyPipelineLayout;
    table.DestroyPipeline = ::DestroyPipeline;
    table.DestroyQueryPool = ::DestroyQueryPool;
    table.DestroyFence = ::DestroyFence;
    table.AllocateMemory = ::AllocateMemory;
    table.BindBufferMemory = ::BindBufferMemory;
    table.BindTextureMemory = ::BindTextureMemory;
    table.FreeMemory = ::FreeMemory;
    table.BeginCommandBuffer = ::BeginCommandBuffer;
    table.CmdSetDescriptorPool = ::CmdSetDescriptorPool;
    table.CmdSetDescriptorSet = ::CmdSetDescriptorSet;
    table.CmdSetPipelineLayout = ::CmdSetPipelineLayout;
    table.CmdSetPipeline = ::CmdSetPipeline;
    table.CmdSetRootConstants = ::CmdSetRootConstants;
    table.CmdSetRootDescriptor = ::CmdSetRootDescriptor;
    table.CmdBarrier = ::CmdBarrier;
    table.CmdSetIndexBuffer = ::CmdSetIndexBuffer;
    table.CmdSetVertexBuffers = ::CmdSetVertexBuffers;
    table.CmdSetViewports = ::CmdSetViewports;
    table.CmdSetScissors = ::CmdSetScissors;
    table.CmdSetStencilReference = ::CmdSetStencilReference;
    table.CmdSetDepthBounds = ::CmdSetDepthBounds;
    table.CmdSetBlendConstants = ::CmdSetBlendConstants;
    table.CmdSetSampleLocations = ::CmdSetSampleLocations;
    table.CmdSetShadingRate = ::CmdSetShadingRate;
    table.CmdSetDepthBias = ::CmdSetDepthBias;
    table.CmdBeginRendering = ::CmdBeginRendering;
    table.CmdClearAttachments = ::CmdClearAttachments;
    table.CmdDraw = ::CmdDraw;
    table.CmdDrawIndexed = ::CmdDrawIndexed;
    table.CmdDrawIndirect = ::CmdDrawIndirect;
    table.CmdDrawIndexedIndirect = ::CmdDrawIndexedIndirect;
    table.CmdEndRendering = ::CmdEndRendering;
    table.CmdDispatch = ::CmdDispatch;
    table.CmdDispatchIndirect = ::CmdDispatchIndirect;
    table.CmdCopyBuffer = ::CmdCopyBuffer;
    table.CmdCopyTexture = ::CmdCopyTexture;
    table.CmdUploadBufferToTexture = ::CmdUploadBufferToTexture;
    table.CmdReadbackTextureToBuffer = ::CmdReadbackTextureToBuffer;
    table.CmdClearStorageBuffer = ::CmdClearStorageBuffer;
    table.CmdClearStorageTexture = ::CmdClearStorageTexture;
    table.CmdResetQueries = ::CmdResetQueries;
    table.CmdBeginQuery = ::CmdBeginQuery;
    table.CmdEndQuery = ::CmdEndQuery;
    table.CmdCopyQueries = ::CmdCopyQueries;
    table.CmdBeginAnnotation = ::CmdBeginAnnotation;
    table.CmdEndAnnotation = ::CmdEndAnnotation;
    table.EndCommandBuffer = ::EndCommandBuffer;
    table.QueueSubmit = ::QueueSubmit;
    table.Wait = ::Wait;
    table.GetFenceValue = ::GetFenceValue;
    table.UpdateDescriptorRanges = ::UpdateDescriptorRanges;
    table.UpdateDynamicConstantBuffers = ::UpdateDynamicConstantBuffers;
    table.CopyDescriptorSet = ::CopyDescriptorSet;
    table.AllocateDescriptorSets = ::AllocateDescriptorSets;
    table.ResetDescriptorPool = ::ResetDescriptorPool;
    table.ResetCommandAllocator = ::ResetCommandAllocator;
    table.MapBuffer = ::MapBuffer;
    table.UnmapBuffer = ::UnmapBuffer;
    table.SetDeviceDebugName = ::SetDeviceDebugName;
    table.SetFenceDebugName = ::SetFenceDebugName;
    table.SetDescriptorDebugName = ::SetDescriptorDebugName;
    table.SetPipelineDebugName = ::SetPipelineDebugName;
    table.SetCommandBufferDebugName = ::SetCommandBufferDebugName;
    table.SetBufferDebugName = ::SetBufferDebugName;
    table.SetTextureDebugName = ::SetTextureDebugName;
    table.SetCommandQueueDebugName = ::SetCommandQueueDebugName;
    table.SetCommandAllocatorDebugName = ::SetCommandAllocatorDebugName;
    table.SetDescriptorPoolDebugName = ::SetDescriptorPoolDebugName;
    table.SetPipelineLayoutDebugName = ::SetPipelineLayoutDebugName;
    table.SetQueryPoolDebugName = ::SetQueryPoolDebugName;
    table.SetDescriptorSetDebugName = ::SetDescriptorSetDebugName;
    table.SetMemoryDebugName = ::SetMemoryDebugName;
    table.GetDeviceNativeObject = ::GetDeviceNativeObject;
    table.GetCommandBufferNativeObject = ::GetCommandBufferNativeObject;
    table.GetBufferNativeObject = ::GetBufferNativeObject;
    table.GetTextureNativeObject = ::GetTextureNativeObject;
    table.GetDescriptorNativeObject = ::GetDescriptorNativeObject;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  Helper  ]

static uint32_t NRI_CALL CalculateAllocationNumber(const Device&, const ResourceGroupDesc&) {
    return 0;
}

static Result NRI_CALL AllocateAndBindMemory(Device&, const ResourceGroupDesc&, Memory**) {
    return Result::SUCCESS;
}

static Result NRI_CALL UploadData(CommandQueue&, const TextureUploadDesc*, uint32_t, const BufferUploadDesc*, uint32_t) {
    return Result::SUCCESS;
}

static Result NRI_CALL WaitForIdle(CommandQueue&) {
    return Result::SUCCESS;
}

static Result NRI_CALL QueryVideoMemoryInfo(const Device&, MemoryLocation, VideoMemoryInfo& videoMemoryInfo) {
    videoMemoryInfo = {};

    return Result::SUCCESS;
}

Result DeviceNONE::FillFunctionTable(HelperInterface& table) const {
    table.CalculateAllocationNumber = ::CalculateAllocationNumber;
    table.AllocateAndBindMemory = ::AllocateAndBindMemory;
    table.UploadData = ::UploadData;
    table.WaitForIdle = ::WaitForIdle;
    table.QueryVideoMemoryInfo = ::QueryVideoMemoryInfo;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  LowLatency  ]

static Result SetLatencySleepMode(SwapChain&, const LatencySleepMode&) {
    return Result::SUCCESS;
}

static Result SetLatencyMarker(SwapChain&, LatencyMarker) {
    return Result::SUCCESS;
}

static Result LatencySleep(SwapChain&) {
    return Result::SUCCESS;
}

static Result GetLatencyReport(const SwapChain&, LatencyReport&) {
    return Result::SUCCESS;
}

static void NRI_CALL QueueSubmitTrackable(CommandQueue&, const QueueSubmitDesc&, const SwapChain&) {
}

Result DeviceNONE::FillFunctionTable(LowLatencyInterface& table) const {
    table.SetLatencySleepMode = ::SetLatencySleepMode;
    table.SetLatencyMarker = ::SetLatencyMarker;
    table.LatencySleep = ::LatencySleep;
    table.GetLatencyReport = ::GetLatencyReport;
    table.QueueSubmitTrackable = ::QueueSubmitTrackable;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  MeshShader  ]

static void NRI_CALL CmdDrawMeshTasks(CommandBuffer&, const DrawMeshTasksDesc&) {
}

static void NRI_CALL CmdDrawMeshTasksIndirect(CommandBuffer&, const Buffer&, uint64_t, uint32_t, uint32_t, const Buffer*, uint64_t) {
}

Result DeviceNONE::FillFunctionTable(MeshShaderInterface& table) const {
    table.CmdDrawMeshTasks = ::CmdDrawMeshTasks;
    table.CmdDrawMeshTasksIndirect = ::CmdDrawMeshTasksIndirect;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  RayTracing  ]

static void NRI_CALL GetAccelerationStructureMemoryDesc(const Device&, const AccelerationStructureDesc&, MemoryLocation, MemoryDesc&) {
}

static uint64_t NRI_CALL GetAccelerationStructureUpdateScratchBufferSize(const AccelerationStructure&) {
    return 0;
}

static uint64_t NRI_CALL GetAccelerationStructureBuildScratchBufferSize(const AccelerationStructure&) {
    return 0;
}

static uint64_t NRI_CALL GetAccelerationStructureHandle(const AccelerationStructure&) {
    return 0;
}

static Result NRI_CALL CreateRayTracingPipeline(Device&, const RayTracingPipelineDesc&, Pipeline*& pipeline) {
    pipeline = DummyObject<Pipeline>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateAccelerationStructure(Device&, const AccelerationStructureDesc&, AccelerationStructure*& accelerationStructure) {
    accelerationStructure = DummyObject<AccelerationStructure>();

    return Result::SUCCESS;
}

static Result NRI_CALL CreateAccelerationStructureDescriptor(const AccelerationStructure&, Descriptor*& descriptor) {
    descriptor = DummyObject<Descriptor>();

    return Result::SUCCESS;
}

static void NRI_CALL DestroyAccelerationStructure(AccelerationStructure&) {
}

static Result NRI_CALL BindAccelerationStructureMemory(Device&, const AccelerationStructureMemoryBindingDesc*, uint32_t) {
    return Result::SUCCESS;
}

static Result NRI_CALL WriteShaderGroupIdentifiers(const Pipeline&, uint32_t, uint32_t, void*) {
    return Result::SUCCESS;
}

static void NRI_CALL CmdBuildTopLevelAccelerationStructure(CommandBuffer&, uint32_t, const Buffer&, uint64_t, AccelerationStructureBuildBits, AccelerationStructure&, Buffer&, uint64_t) {
}

static void NRI_CALL CmdBuildBottomLevelAccelerationStructure(CommandBuffer&, uint32_t, const GeometryObject*, AccelerationStructureBuildBits, AccelerationStructure&, Buffer&, uint64_t) {
}

static void NRI_CALL CmdUpdateTopLevelAccelerationStructure(CommandBuffer&, uint32_t, const Buffer&, uint64_t, AccelerationStructureBuildBits, AccelerationStructure&, const AccelerationStructure&, Buffer&, uint64_t) {
}

static void NRI_CALL CmdUpdateBottomLevelAccelerationStructure(CommandBuffer&, uint32_t, const GeometryObject*, AccelerationStructureBuildBits, AccelerationStructure&, const AccelerationStructure&, Buffer&, uint64_t) {
}

static void NRI_CALL CmdDispatchRays(CommandBuffer&, const DispatchRaysDesc&) {
}

static void NRI_CALL CmdDispatchRaysIndirect(CommandBuffer&, const Buffer&, uint64_t) {
}

static void NRI_CALL CmdCopyAccelerationStructure(CommandBuffer&, AccelerationStructure&, const AccelerationStructure&, CopyMode) {
}

static void NRI_CALL CmdWriteAccelerationStructureSize(CommandBuffer&, const AccelerationStructure* const*, uint32_t, QueryPool&, uint32_t) {
}

static void NRI_CALL SetAccelerationStructureDebugName(AccelerationStructure&, const char*) {
}

static uint64_t NRI_CALL GetAccelerationStructureNativeObject(const AccelerationStructure&) {
    return 0;
}

Result DeviceNONE::FillFunctionTable(RayTracingInterface& table) const {
    table.GetAccelerationStructureMemoryDesc = ::GetAccelerationStructureMemoryDesc;
    table.GetAccelerationStructureUpdateScratchBufferSize = ::GetAccelerationStructureUpdateScratchBufferSize;
    table.GetAccelerationStructureBuildScratchBufferSize = ::GetAccelerationStructureBuildScratchBufferSize;
    table.GetAccelerationStructureHandle = ::GetAccelerationStructureHandle;
    table.CreateRayTracingPipeline = ::CreateRayTracingPipeline;
    table.CreateAccelerationStructure = ::CreateAccelerationStructure;
    table.CreateAccelerationStructureDescriptor = ::CreateAccelerationStructureDescriptor;
    table.DestroyAccelerationStructure = ::DestroyAccelerationStructure;
    table.BindAccelerationStructureMemory = ::BindAccelerationStructureMemory;
    table.WriteShaderGroupIdentifiers = ::WriteShaderGroupIdentifiers;
    table.CmdBuildTopLevelAccelerationStructure = ::CmdBuildTopLevelAccelerationStructure;
    table.CmdBuildBottomLevelAccelerationStructure = ::CmdBuildBottomLevelAccelerationStructure;
    table.CmdUpdateTopLevelAccelerationStructure = ::CmdUpdateTopLevelAccelerationStructure;
    table.CmdUpdateBottomLevelAccelerationStructure = ::CmdUpdateBottomLevelAccelerationStructure;
    table.CmdDispatchRays = ::CmdDispatchRays;
    table.CmdDispatchRaysIndirect = ::CmdDispatchRaysIndirect;
    table.CmdCopyAccelerationStructure = ::CmdCopyAccelerationStructure;
    table.CmdWriteAccelerationStructureSize = ::CmdWriteAccelerationStructureSize;
    table.SetAccelerationStructureDebugName = ::SetAccelerationStructureDebugName;
    table.GetAccelerationStructureNativeObject = ::GetAccelerationStructureNativeObject;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  ResourceAllocator  ]

static Result AllocateBuffer(Device&, const AllocateBufferDesc&, Buffer*& buffer) {
    buffer = DummyObject<Buffer>();

    return Result::SUCCESS;
}

static Result AllocateTexture(Device&, const AllocateTextureDesc&, Texture*& texture) {
    texture = DummyObject<Texture>();

    return Result::SUCCESS;
}

static Result AllocateAccelerationStructure(Device&, const AllocateAccelerationStructureDesc&, AccelerationStructure*& accelerationStructure) {
    accelerationStructure = DummyObject<AccelerationStructure>();

    return Result::SUCCESS;
}

Result DeviceNONE::FillFunctionTable(ResourceAllocatorInterface& table) const {
    table.AllocateBuffer = ::AllocateBuffer;
    table.AllocateTexture = ::AllocateTexture;
    table.AllocateAccelerationStructure = ::AllocateAccelerationStructure;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  Streamer  ]

static Result CreateStreamer(Device&, const StreamerDesc&, Streamer*& streamer) {
    streamer = DummyObject<Streamer>();

    return Result::SUCCESS;
}

static void DestroyStreamer(Streamer&) {
}

static Buffer* GetStreamerConstantBuffer(Streamer&) {
    return nullptr;
}

static uint32_t UpdateStreamerConstantBuffer(Streamer&, const void*, uint32_t) {
    return 0;
}

static uint64_t AddStreamerBufferUpdateRequest(Streamer&, const BufferUpdateRequestDesc&) {
    return 0;
}

static uint64_t AddStreamerTextureUpdateRequest(Streamer&, const TextureUpdateRequestDesc&) {
    return 0;
}

static Result CopyStreamerUpdateRequests(Streamer&) {
    return Result::SUCCESS;
}

static Buffer* GetStreamerDynamicBuffer(Streamer&) {
    return nullptr;
}

static void CmdUploadStreamerUpdateRequests(CommandBuffer&, Streamer&) {
}

Result DeviceNONE::FillFunctionTable(StreamerInterface& table) const {
    table.CreateStreamer = ::CreateStreamer;
    table.DestroyStreamer = ::DestroyStreamer;
    table.GetStreamerConstantBuffer = ::GetStreamerConstantBuffer;
    table.GetStreamerDynamicBuffer = ::GetStreamerDynamicBuffer;
    table.AddStreamerBufferUpdateRequest = ::AddStreamerBufferUpdateRequest;
    table.AddStreamerTextureUpdateRequest = ::AddStreamerTextureUpdateRequest;
    table.UpdateStreamerConstantBuffer = ::UpdateStreamerConstantBuffer;
    table.CopyStreamerUpdateRequests = ::CopyStreamerUpdateRequests;
    table.CmdUploadStreamerUpdateRequests = ::CmdUploadStreamerUpdateRequests;

    return Result::SUCCESS;
}

#pragma endregion

//============================================================================================================================================================================================
#pragma region[  SwapChain  ]

static Result NRI_CALL CreateSwapChain(Device&, const SwapChainDesc&, SwapChain*& swapChain) {
    swapChain = DummyObject<SwapChain>();

    return Result::SUCCESS;
}

static void NRI_CALL DestroySwapChain(SwapChain&) {
}

static void NRI_CALL SetSwapChainDebugName(SwapChain&, const char*) {
}

static Texture* const* NRI_CALL GetSwapChainTextures(const SwapChain&, uint32_t& textureNum) {
    static void* textures[1] = {};
    textureNum = 1;

    return (Texture**)textures;
}

static uint32_t NRI_CALL AcquireNextSwapChainTexture(SwapChain&) {
    return 0;
}

static Result NRI_CALL WaitForPresent(SwapChain&) {
    return Result::SUCCESS;
}

static Result NRI_CALL QueuePresent(SwapChain&) {
    return Result::SUCCESS;
}

static Result NRI_CALL GetDisplayDesc(SwapChain&, DisplayDesc& displayDesc) {
    displayDesc = {};

    return Result::SUCCESS;
}

Result DeviceNONE::FillFunctionTable(SwapChainInterface& table) const {
    table.CreateSwapChain = ::CreateSwapChain;
    table.DestroySwapChain = ::DestroySwapChain;
    table.SetSwapChainDebugName = ::SetSwapChainDebugName;
    table.GetSwapChainTextures = ::GetSwapChainTextures;
    table.AcquireNextSwapChainTexture = ::AcquireNextSwapChainTexture;
    table.WaitForPresent = ::WaitForPresent;
    table.QueuePresent = ::QueuePresent;
    table.GetDisplayDesc = ::GetDisplayDesc;

    return Result::SUCCESS;
}

#pragma endregion