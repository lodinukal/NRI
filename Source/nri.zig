const std = @import("std");

pub const Device = struct {
    internal_device: *RawDevice,
    core_interface: CoreInterface,
    swap_chain_interface: SwapChainInterface,
    streamer_interface: StreamerInterface,
    helper_interface: HelperInterface,

    pub fn init(desc: DeviceCreationDesc) !Device {
        var rawtemp: ?*RawDevice = null;
        try check(nriCreateDevice(&desc, &rawtemp));
        errdefer nriDestroyDevice(rawtemp);

        const raw = rawtemp orelse unreachable;

        var self: Device = .{
            .internal_device = raw,
            .core_interface = undefined,
            .swap_chain_interface = undefined,
            .streamer_interface = undefined,
            .helper_interface = undefined,
        };

        if (nriGetInterface(raw, "NriCoreInterface", @sizeOf(CoreInterface), &self.core_interface) != .success) {
            return error.QueryFailed;
        }

        if (nriGetInterface(raw, "NriSwapChainInterface", @sizeOf(SwapChainInterface), &self.swap_chain_interface) != .success) {
            return error.QueryFailed;
        }

        if (nriGetInterface(raw, "NriStreamerInterface", @sizeOf(StreamerInterface), &self.streamer_interface) != .success) {
            return error.QueryFailed;
        }

        if (nriGetInterface(raw, "NriHelperInterface", @sizeOf(HelperInterface), &self.helper_interface) != .success) {
            return error.QueryFailed;
        }

        return self;
    }

    pub fn deinit(self: Device) void {
        nriDestroyDevice(self.internal_device);
    }

    pub inline fn getDeviceDesc(self: Device) *const DeviceDesc {
        return self.core_interface.GetDeviceDesc(self.internal_device);
    }

    pub inline fn getFormatSupport(self: Device, format: Format) FormatSupportFlags {
        return self.core_interface.GetFormatSupport(self.internal_device, format);
    }

    pub inline fn getQuerySize(self: Device, query_pool: *const QueryPool) u32 {
        return self.core_interface.GetQuerySize(self.internal_device, query_pool);
    }

    pub inline fn getBufferMemoryDesc(self: Device, desc: BufferDesc, memory_location: MemoryLocation) MemoryDesc {
        var out_desc: MemoryDesc = undefined;
        self.core_interface.GetBufferMemoryDesc(self.internal_device, &desc, memory_location, &out_desc);
        return out_desc;
    }

    pub inline fn getTextureMemoryDesc(self: Device, desc: TextureDesc, memory_location: MemoryLocation) MemoryDesc {
        var out_desc: MemoryDesc = undefined;
        self.core_interface.GetTextureMemoryDesc(self.internal_device, &desc, memory_location, &out_desc);
        return out_desc;
    }

    pub inline fn getCommandQueue(self: Device, queue_type: CommandQueueType) !*CommandQueue {
        var temp_queue: ?*CommandQueue = null;
        try check(self.core_interface.GetCommandQueue(self.internal_device, queue_type, &temp_queue));
        return temp_queue orelse unreachable;
    }

    pub inline fn queueSubmit(self: Device, command_queue: *CommandQueue, desc: QueueSubmitDesc) void {
        self.core_interface.QueueSubmit(command_queue, &desc);
    }

    pub inline fn setCommandQueueDebugName(self: Device, command_queue: *CommandQueue, name: [:0]const u8) void {
        self.core_interface.SetCommandQueueDebugName(command_queue, name.ptr);
    }

    /// Populate resources with data (not for streaming!)
    pub inline fn uploadData(
        self: Device,
        command_queue: *CommandQueue,
        texture_upload_descs: []const TextureUploadDesc,
        buffer_upload_descs: []const BufferUploadDesc,
    ) !void {
        try check(self.helper_interface.UploadData(
            command_queue,
            texture_upload_descs.ptr,
            @intCast(texture_upload_descs.len),
            buffer_upload_descs.ptr,
            @intCast(buffer_upload_descs.len),
        ));
    }

    pub inline fn waitForIdle(self: Device, command_queue: *CommandQueue) !void {
        try check(self.helper_interface.WaitForIdle(command_queue));
    }

    pub inline fn createCommandAllocator(self: Device, command_queue: *const CommandQueue) !*CommandAllocator {
        var temp_allocator: ?*CommandAllocator = null;
        try check(self.core_interface.CreateCommandAllocator(command_queue, &temp_allocator));
        return temp_allocator orelse unreachable;
    }

    pub inline fn destroyCommandAllocator(self: Device, command_allocator: *CommandAllocator) void {
        self.core_interface.DestroyCommandAllocator(command_allocator);
    }

    pub inline fn reset(self: Device, command_allocator: *CommandAllocator) void {
        self.core_interface.ResetCommandAllocator(command_allocator);
    }

    pub inline fn setCommandAllocatorDebugName(self: Device, command_allocator: *CommandAllocator, name: [:0]const u8) void {
        self.core_interface.SetCommandAllocatorDebugName(command_allocator, name.ptr);
    }

    pub inline fn createCommandBuffer(self: Device, allocator: *CommandAllocator) !*CommandBuffer {
        var temp_buffer: ?*CommandBuffer = null;
        try check(self.core_interface.CreateCommandBuffer(allocator, &temp_buffer));
        return temp_buffer orelse unreachable;
    }

    pub inline fn destroyCommandBuffer(self: Device, command_buffer: *CommandBuffer) void {
        self.core_interface.DestroyCommandBuffer(command_buffer);
    }

    pub inline fn begin(self: Device, command_buffer: *CommandBuffer, descriptor_pool: ?*const DescriptorPool) !void {
        try check(self.core_interface.BeginCommandBuffer(command_buffer, descriptor_pool));
    }

    pub inline fn setDescriptorPool(self: Device, command_buffer: *CommandBuffer, descriptor_pool: *const DescriptorPool) void {
        self.core_interface.CmdSetDescriptorPool(command_buffer, descriptor_pool);
    }

    pub inline fn setDescriptorSet(
        self: Device,
        command_buffer: *CommandBuffer,
        index: u32,
        descriptor_set: *const DescriptorSet,
        dynamic_constant_buffer_offsets: []const u32,
    ) void {
        self.core_interface.CmdSetDescriptorSet(command_buffer, index, descriptor_set, dynamic_constant_buffer_offsets.ptr);
    }

    pub inline fn setPipelineLayout(self: Device, command_buffer: *CommandBuffer, pipeline_layout: *const PipelineLayout) void {
        self.core_interface.CmdSetPipelineLayout(command_buffer, pipeline_layout);
    }

    pub inline fn setPipeline(self: Device, command_buffer: *CommandBuffer, pipeline: *const Pipeline) void {
        self.core_interface.CmdSetPipeline(command_buffer, pipeline);
    }

    pub inline fn setRootConstants(self: Device, command_buffer: *CommandBuffer, push_constant_index: u32, data: []const u8) void {
        self.core_interface.CmdSetRootConstants(command_buffer, push_constant_index, @ptrCast(data.ptr), @intCast(data.len));
    }

    pub inline fn setRootDescriptor(self: Device, command_buffer: *CommandBuffer, root_index: u32, descriptor: *const Descriptor) void {
        self.core_interface.CmdSetRootDescriptor(command_buffer, root_index, descriptor);
    }

    pub inline fn barrier(self: Device, command_buffer: *CommandBuffer, desc: BarrierGroupDesc) void {
        self.core_interface.CmdBarrier(command_buffer, &desc);
    }

    pub inline fn setIndexBuffer(
        self: Device,
        command_buffer: *CommandBuffer,
        buffer: *Buffer,
        offset: u64,
        index_kind: IndexKind,
    ) void {
        self.core_interface.CmdSetIndexBuffer(command_buffer, buffer, offset, index_kind);
    }

    pub inline fn setVertexBuffers(
        self: Device,
        command_buffer: *CommandBuffer,
        base_slot: u32,
        buffers: []const *const Buffer,
        offsets: []const u64,
    ) void {
        self.core_interface.CmdSetVertexBuffers(command_buffer, base_slot, @intCast(buffers.len), buffers.ptr, offsets.ptr);
    }

    pub inline fn setViewport(self: Device, command_buffer: *CommandBuffer, viewports: []const Viewport) void {
        self.core_interface.CmdSetViewports(command_buffer, viewports.ptr, @intCast(viewports.len));
    }

    pub inline fn setScissors(self: Device, command_buffer: *CommandBuffer, rects: []const Rect) void {
        self.core_interface.CmdSetScissors(command_buffer, rects.ptr, @intCast(rects.len));
    }

    pub inline fn setStencilReference(self: Device, command_buffer: *CommandBuffer, front_ref: u8, back_ref: u8) void {
        self.core_interface.CmdSetStencilReference(command_buffer, front_ref, back_ref);
    }

    pub inline fn setDepthBounds(self: Device, command_buffer: *CommandBuffer, bounds_min: f32, bounds_max: f32) void {
        self.core_interface.CmdSetDepthBounds(command_buffer, bounds_min, bounds_max);
    }

    pub inline fn setBlendConstants(self: Device, command_buffer: *CommandBuffer, color: Color32f) void {
        self.core_interface.CmdSetBlendConstants(command_buffer, color);
    }

    pub inline fn setSampleLocations(
        self: Device,
        command_buffer: *CommandBuffer,
        locations: []const SamplePosition,
        sample_num: sample,
    ) void {
        self.core_interface.CmdSetSampleLocations(command_buffer, locations.ptr, @intCast(locations.len), sample_num);
    }

    pub inline fn setShadingRate(self: Device, command_buffer: *CommandBuffer, desc: ShadingRateDesc) void {
        self.core_interface.CmdSetShadingRate(command_buffer, &desc);
    }

    pub inline fn setDepthBias(self: Device, command_buffer: *CommandBuffer, desc: DepthBiasDesc) void {
        self.core_interface.CmdSetDepthBias(command_buffer, &desc);
    }

    pub inline fn beginRendering(self: Device, command_buffer: *CommandBuffer, attachments: AttachmentsDesc) void {
        self.core_interface.CmdBeginRendering(command_buffer, &attachments);
    }

    pub inline fn clearAttachments(
        self: Device,
        command_buffer: *CommandBuffer,
        clear_descs: []const ClearDesc,
        rects: []const Rect,
    ) void {
        self.core_interface.CmdClearAttachments(command_buffer, clear_descs.ptr, @intCast(clear_descs.len), rects.ptr, @intCast(rects.len));
    }

    pub inline fn draw(self: Device, command_buffer: *CommandBuffer, desc: DrawDesc) void {
        self.core_interface.CmdDraw(command_buffer, &desc);
    }

    pub inline fn drawIndexed(self: Device, command_buffer: *CommandBuffer, desc: DrawIndexedDesc) void {
        self.core_interface.CmdDrawIndexed(command_buffer, &desc);
    }

    pub inline fn drawIndirect(
        self: Device,
        command_buffer: *CommandBuffer,
        buffer: *const Buffer,
        offset: u64,
        draw_num: u32,
        stride: u32,
        count_buffer: *const Buffer,
        count_buffer_offset: u64,
    ) void {
        self.core_interface.CmdDrawIndirect(command_buffer, buffer, offset, draw_num, stride, count_buffer, count_buffer_offset);
    }

    pub inline fn drawIndexedIndirect(
        self: Device,
        command_buffer: *CommandBuffer,
        buffer: *const Buffer,
        offset: u64,
        draw_num: u32,
        stride: u32,
        count_buffer: *const Buffer,
        count_buffer_offset: u64,
    ) void {
        self.core_interface.CmdDrawIndexedIndirect(command_buffer, buffer, offset, draw_num, stride, count_buffer, count_buffer_offset);
    }

    pub inline fn endRendering(self: Device, command_buffer: *CommandBuffer) void {
        self.core_interface.CmdEndRendering(command_buffer);
    }

    pub inline fn dispatch(self: Device, command_buffer: *CommandBuffer, desc: DispatchDesc) void {
        self.core_interface.CmdDispatch(command_buffer, &desc);
    }

    pub inline fn dispatchIndirect(self: Device, command_buffer: *CommandBuffer, buffer: *const Buffer, offset: u64) void {
        self.core_interface.CmdDispatchIndirect(command_buffer, buffer, offset);
    }

    pub inline fn copyBuffer(
        self: Device,
        command_buffer: *CommandBuffer,
        dst_buffer: *Buffer,
        dst_offset: u64,
        src_buffer: *Buffer,
        src_offset: u64,
        size: u64,
    ) void {
        self.core_interface.CmdCopyBuffer(command_buffer, dst_buffer, dst_offset, src_buffer, src_offset, size);
    }

    pub inline fn copyTexture(
        self: Device,
        command_buffer: *CommandBuffer,
        dst_texture: *Texture,
        dst_region_desc: TextureRegionDesc,
        src_texture: *Texture,
        src_region_desc: TextureRegionDesc,
    ) void {
        self.core_interface.CmdCopyTexture(command_buffer, dst_texture, &dst_region_desc, src_texture, &src_region_desc);
    }

    pub inline fn resolveTexture(
        self: Device,
        command_buffer: *CommandBuffer,
        dst_texture: *Texture,
        dst_region_desc: TextureRegionDesc,
        src_texture: *const Texture,
        src_region_desc: TextureRegionDesc,
    ) void {
        self.core_interface.CmdResolveTexture(command_buffer, dst_texture, &dst_region_desc, src_texture, &src_region_desc);
    }

    pub inline fn uploadBufferToTexture(
        self: Device,
        command_buffer: *CommandBuffer,
        dst_texture: *Texture,
        dst_region_desc: TextureRegionDesc,
        src_buffer: *Buffer,
        src_data_layout_desc: TextureDataLayoutDesc,
    ) void {
        self.core_interface.CmdUploadBufferToTexture(command_buffer, dst_texture, &dst_region_desc, src_buffer, &src_data_layout_desc);
    }

    pub inline fn readbackTextureToBuffer(
        self: Device,
        command_buffer: *CommandBuffer,
        dst_buffer: *Buffer,
        dst_data_layout_desc: TextureDataLayoutDesc,
        src_texture: *Texture,
        src_region_desc: TextureRegionDesc,
    ) void {
        self.core_interface.CmdReadbackTextureToBuffer(command_buffer, dst_buffer, &dst_data_layout_desc, src_texture, &src_region_desc);
    }

    pub inline fn clearStorageBuffer(self: Device, command_buffer: *CommandBuffer, clear_desc: ClearStorageBufferDesc) void {
        self.core_interface.CmdClearStorageBuffer(command_buffer, &clear_desc);
    }

    pub inline fn clearStorageTexture(self: Device, command_buffer: *CommandBuffer, clear_desc: ClearStorageTextureDesc) void {
        self.core_interface.CmdClearStorageTexture(command_buffer, &clear_desc);
    }

    pub inline fn resetQueries(self: Device, command_buffer: *CommandBuffer, query_pool: *const QueryPool, offset: u32, num: u32) void {
        self.core_interface.CmdResetQueries(command_buffer, query_pool, offset, num);
    }

    pub inline fn beginQuery(self: Device, command_buffer: *CommandBuffer, query_pool: *const QueryPool, offset: u32) void {
        self.core_interface.CmdBeginQuery(command_buffer, query_pool, offset);
    }

    pub inline fn endQuery(self: Device, command_buffer: *CommandBuffer, query_pool: *const QueryPool, offset: u32) void {
        self.core_interface.CmdEndQuery(command_buffer, query_pool, offset);
    }

    pub inline fn copyQueries(
        self: Device,
        command_buffer: *CommandBuffer,
        query_pool: *const QueryPool,
        offset: u32,
        num: u32,
        dst_buffer: *Buffer,
        dst_offset: u64,
    ) void {
        self.core_interface.CmdCopyQueries(command_buffer, query_pool, offset, num, dst_buffer, dst_offset);
    }

    pub inline fn beginAnnotation(self: Device, command_buffer: *CommandBuffer, name: []const u8) void {
        self.core_interface.CmdBeginAnnotation(command_buffer, name.ptr);
    }

    pub inline fn endAnnotation(self: Device, command_buffer: *CommandBuffer) void {
        self.core_interface.CmdEndAnnotation(command_buffer);
    }

    pub inline fn end(self: Device, command_buffer: *CommandBuffer) !void {
        try check(self.core_interface.EndCommandBuffer(command_buffer));
    }

    pub inline fn setCommandBufferDebugName(self: Device, command_buffer: *CommandBuffer, name: [:0]const u8) void {
        self.core_interface.SetCommandBufferDebugName(command_buffer, name.ptr);
    }

    pub inline fn createDescriptorPool(self: Device, desc: DescriptorPoolDesc) !*DescriptorPool {
        var temp_pool: ?*DescriptorPool = null;
        try check(self.core_interface.CreateDescriptorPool(self.internal_device, &desc, &temp_pool));
        return temp_pool orelse unreachable;
    }

    pub inline fn destroyDescriptorPool(self: Device, descriptor_pool: *DescriptorPool) void {
        self.core_interface.DestroyDescriptorPool(descriptor_pool);
    }

    pub inline fn updateRanges(
        device: Device,
        descriptor_pool: *DescriptorPool,
        base_range: u32,
        descs: []const DescriptorRangeUpdateDesc,
    ) void {
        device.core_interface.UpdateDescriptorRanges(descriptor_pool, base_range, @intCast(descs.len), descs.ptr);
    }

    pub inline fn updateDynamicConstantBuffers(
        self: Device,
        descriptor_pool: *DescriptorPool,
        base_buffer: u32,
        descriptors: []const *Descriptor,
    ) void {
        self.core_interface.UpdateDynamicConstantBuffers(descriptor_pool, base_buffer, @intCast(descriptors.len), descriptors.ptr);
    }

    pub inline fn copyDescriptorSet(self: Device, descriptor_pool: *DescriptorPool, desc: DescriptorSetCopyDesc) void {
        self.core_interface.CopyDescriptorSet(descriptor_pool, &desc);
    }

    pub inline fn setDescriptorPoolDebugName(self: Device, descriptor_pool: *DescriptorPool, name: [:0]const u8) void {
        self.core_interface.SetDescriptorPoolDebugName(descriptor_pool, name.ptr);
    }

    pub inline fn allocateDescriptorSets(
        self: Device,
        descriptor_pool: *DescriptorPool,
        allocator: std.mem.Allocator,
        pipeline_layout: *const PipelineLayout,
        set_index: u32,
        instance_num: u32,
        variable_descriptor_num: u32,
    ) ![]*DescriptorSet {
        const temp_sets: []*DescriptorSet = try allocator.alloc(*DescriptorSet, instance_num);
        errdefer allocator.free(temp_sets);
        try check(self.core_interface.AllocateDescriptorSets(descriptor_pool, pipeline_layout, set_index, temp_sets.ptr, instance_num, variable_descriptor_num));
        return temp_sets;
    }

    pub inline fn resetDescriptorPool(self: Device, descriptor_pool: *DescriptorPool) void {
        self.core_interface.ResetDescriptorPool(descriptor_pool);
    }

    pub inline fn createBuffer(self: Device, desc: BufferDesc) !*Buffer {
        var temp_buffer: ?*Buffer = null;
        try check(self.core_interface.CreateBuffer(self.internal_device, &desc, &temp_buffer));
        return temp_buffer orelse unreachable;
    }

    pub inline fn destroyBuffer(self: Device, buffer: *Buffer) void {
        self.core_interface.DestroyBuffer(buffer);
    }

    pub inline fn getDesc(self: Device, buffer: *Buffer) *const BufferDesc {
        return self.core_interface.GetBufferDesc(buffer);
    }

    pub inline fn map(self: Device, buffer: *Buffer, offset: u64, size: u64) ![]u8 {
        const ptr = self.core_interface.MapBuffer(buffer, offset, size) orelse return &.{};
        const as_u8: [*]u8 = @ptrCast(ptr);
        return as_u8[0..size];
    }

    pub inline fn unmap(self: Device, buffer: *Buffer) void {
        self.core_interface.UnmapBuffer(buffer);
    }

    pub inline fn setBufferDebugName(self: Device, buffer: *Buffer, name: [:0]const u8) void {
        self.core_interface.SetBufferDebugName(buffer, name.ptr);
    }

    pub inline fn createTexture(self: Device, desc: TextureDesc) !*Texture {
        var temp_texture: ?*Texture = null;
        try check(self.core_interface.CreateTexture(self.internal_device, &desc, &temp_texture));
        return temp_texture orelse unreachable;
    }

    pub inline fn destroyTexture(self: Device, texture: *Texture) void {
        self.core_interface.DestroyTexture(texture);
    }

    pub inline fn getTextureDesc(self: Device, texture: *Texture) *const TextureDesc {
        return self.core_interface.GetTextureDesc(texture);
    }

    pub inline fn setTextureDebugName(self: Device, texture: *Texture, name: [:0]const u8) void {
        self.core_interface.SetTextureDebugName(texture, name.ptr);
    }

    pub inline fn createBufferView(self: Device, desc: BufferViewDesc) !*Descriptor {
        var temp_desc: ?*Descriptor = null;
        try check(self.core_interface.CreateBufferView(&desc, &temp_desc));
        return temp_desc orelse unreachable;
    }

    pub inline fn createTexture1DView(self: Device, desc: Texture1DViewDesc) !*Descriptor {
        var temp_desc: ?*Descriptor = null;
        try check(self.core_interface.CreateTexture1DView(&desc, &temp_desc));
        return temp_desc orelse unreachable;
    }

    pub inline fn createTexture2DView(self: Device, desc: Texture2DViewDesc) !*Descriptor {
        var temp_desc: ?*Descriptor = null;
        try check(self.core_interface.CreateTexture2DView(&desc, &temp_desc));
        return temp_desc orelse unreachable;
    }

    pub inline fn createTexture3DView(self: Device, desc: Texture3DViewDesc) !*Descriptor {
        var temp_desc: ?*Descriptor = null;
        try check(self.core_interface.CreateTexture3DView(&desc, &temp_desc));
        return temp_desc orelse unreachable;
    }

    pub inline fn createSampler(self: Device, desc: SamplerDesc) !*Descriptor {
        var temp_desc: ?*Descriptor = null;
        try check(self.core_interface.CreateSampler(self.internal_device, &desc, &temp_desc));
        return temp_desc orelse unreachable;
    }

    pub inline fn destroyDescriptor(self: Device, descriptor: *Descriptor) void {
        self.core_interface.DestroyDescriptor(descriptor);
    }

    pub inline fn setDescriptorDebugName(self: Device, descriptor: *Descriptor, name: [:0]const u8) void {
        self.core_interface.SetDescriptorDebugName(descriptor, name.ptr);
    }

    pub inline fn createPipelineLayout(self: Device, desc: PipelineLayoutDesc) !*PipelineLayout {
        var temp_layout: ?*PipelineLayout = null;
        try check(self.core_interface.CreatePipelineLayout(self.internal_device, &desc, &temp_layout));
        return temp_layout orelse unreachable;
    }

    pub inline fn destroyPipelineLayout(self: Device, pipeline_layout: *PipelineLayout) void {
        self.core_interface.DestroyPipelineLayout(pipeline_layout);
    }

    pub inline fn createGraphicsPipeline(self: Device, desc: GraphicsPipelineDesc) !*Pipeline {
        var temp_pipeline: ?*Pipeline = null;
        try check(self.core_interface.CreateGraphicsPipeline(self.internal_device, &desc, &temp_pipeline));
        return temp_pipeline orelse unreachable;
    }

    pub inline fn createComputePipeline(self: Device, desc: ComputePipelineDesc) !*Pipeline {
        var temp_pipeline: ?*Pipeline = null;
        try check(self.core_interface.CreateComputePipeline(self.internal_device, &desc, &temp_pipeline));
        return temp_pipeline orelse unreachable;
    }

    pub inline fn destroyPipeline(self: Device, pipeline: *Pipeline) void {
        self.core_interface.DestroyPipeline(pipeline);
    }

    pub inline fn setPipelineDebugName(self: Device, pipeline: *Pipeline, name: [:0]const u8) void {
        self.core_interface.SetPipelineDebugName(pipeline, name.ptr);
    }

    pub inline fn createQueryPool(self: Device, desc: QueryPoolDesc) !*QueryPool {
        var temp_pool: ?*QueryPool = null;
        try check(self.core_interface.CreateQueryPool(self.internal_device, &desc, &temp_pool));
        return temp_pool orelse unreachable;
    }

    pub inline fn destroyQueryPool(self: Device, query_pool: *QueryPool) void {
        self.core_interface.DestroyQueryPool(query_pool);
    }

    pub inline fn createFence(self: Device, value: u64) !*Fence {
        var temp_fence: ?*Fence = null;
        try check(self.core_interface.CreateFence(self.internal_device, value, &temp_fence));
        return temp_fence orelse unreachable;
    }

    pub inline fn destroyFence(self: Device, fence: *Fence) void {
        self.core_interface.DestroyFence(fence);
    }

    pub inline fn wait(self: Device, fence: *Fence, value: u64) void {
        self.core_interface.Wait(fence, value);
    }

    pub inline fn getValue(self: Device, fence: *Fence) u64 {
        return self.core_interface.GetFenceValue(fence);
    }

    pub inline fn setFenceDebugName(self: Device, fence: *Fence, name: [:0]const u8) void {
        self.core_interface.SetFenceDebugName(fence, name.ptr);
    }

    pub inline fn allocateMemory(self: Device, desc: AllocateMemoryDesc) !*Memory {
        var temp_memory: ?*Memory = null;
        try check(self.core_interface.AllocateMemory(self.internal_device, &desc, &temp_memory));
        return temp_memory orelse unreachable;
    }

    pub inline fn setMemoryDebugName(self: Device, memory: *Memory, name: [:0]const u8) void {
        self.core_interface.SetMemoryDebugName(memory, name.ptr);
    }

    pub inline fn bindBufferMemory(self: Device, bindings: []const BufferMemoryBindingDesc) !void {
        check(self.core_interface.BindBufferMemory(self.internal_device, bindings.ptr, @intCast(bindings.len)));
    }

    pub inline fn bindTextureMemory(self: Device, bindings: []const TextureMemoryBindingDesc) !void {
        check(self.core_interface.BindTextureMemory(self.internal_device, bindings.ptr, @intCast(bindings.len)));
    }

    pub inline fn freeMemory(self: Device, memory: *Memory) void {
        self.core_interface.FreeMemory(memory);
    }

    pub inline fn createSwapChain(self: Device, desc: SwapChainDesc) !*SwapChain {
        var temp_swap_chain: ?*SwapChain = null;
        try check(self.swap_chain_interface.CreateSwapChain(self.internal_device, &desc, &temp_swap_chain));
        return temp_swap_chain orelse unreachable;
    }

    pub inline fn destroySwapChain(self: Device, swap_chain: *SwapChain) void {
        self.swap_chain_interface.DestroySwapChain(swap_chain);
    }

    pub inline fn setSwapChainDebugName(self: Device, swap_chain: *SwapChain, name: []const u8) void {
        self.swap_chain_interface.SetSwapChainDebugName(swap_chain, name.ptr);
    }

    pub inline fn getSwapChainTextures(self: Device, swap_chain: *SwapChain) []const *Texture {
        var texture_count: u32 = 0;
        const ptr = self.swap_chain_interface.GetSwapChainTextures(swap_chain, &texture_count);
        return ptr[0..texture_count];
    }

    pub inline fn acquireNextSwapChainTexture(self: Device, swap_chain: *SwapChain) u32 {
        return self.swap_chain_interface.AcquireNextSwapChainTexture(swap_chain);
    }

    pub inline fn waitForPresent(self: Device, swap_chain: *SwapChain) !void {
        try check(self.swap_chain_interface.WaitForPresent(swap_chain));
    }

    pub inline fn queuePresent(self: Device, swap_chain: *SwapChain) !void {
        try check(self.swap_chain_interface.QueuePresent(swap_chain));
    }

    pub inline fn getDisplayDesc(self: Device, swap_chain: *SwapChain) DisplayDesc {
        var out_desc: DisplayDesc = undefined;
        try check(self.swap_chain_interface.GetDisplayDesc(swap_chain, &out_desc));
        return out_desc;
    }

    pub inline fn setDebugName(self: Device, name: [:0]const u8) void {
        self.core_interface.SetDeviceDebugName(self.internal_device, name.ptr);
    }

    pub inline fn calculateAllocationNumber(self: Device, desc: ResourceGroupDesc) u32 {
        return self.helper_interface.CalculateAllocationNumber(self.internal_device, &desc);
    }

    pub inline fn allocateAndBindMemory(self: Device, desc: ResourceGroupDesc, out: []*Memory) !void {
        try check(self.helper_interface.AllocateAndBindMemory(self.internal_device, &desc, out.ptr));
    }

    pub inline fn queryVideoMemoryInfo(self: Device, memory_location: MemoryLocation) !VideoMemoryInfo {
        var out_info: VideoMemoryInfo = undefined;
        try check(self.helper_interface.QueryVideoMemoryInfo(self.internal_device, memory_location, &out_info));
        return out_info;
    }

    pub inline fn createStreamer(self: Device, desc: *const StreamerDesc) !*Streamer {
        var temp_streamer: ?*Streamer = null;
        try check(self.streamer_interface.CreateStreamer(self.internal_device, desc, &temp_streamer));
        return temp_streamer orelse unreachable;
    }

    pub inline fn destroyStreamer(self: Device, streamer: *Streamer) void {
        self.streamer_interface.DestroyStreamer(streamer);
    }

    pub inline fn getStreamerConstantBuffer(self: Device, streamer: *Streamer) !*Buffer {
        var temp_buffer: ?*Buffer = null;
        try check(self.streamer_interface.GetStreamerConstantBuffer(streamer, &temp_buffer));
        return temp_buffer orelse unreachable;
    }

    pub inline fn getStreamerDynamicBuffer(self: Device, streamer: *Streamer) !*Buffer {
        var temp_buffer: ?*Buffer = null;
        try check(self.streamer_interface.GetStreamerDynamicBuffer(streamer, &temp_buffer));
        return temp_buffer orelse unreachable;
    }

    pub inline fn addStreamerBufferUpdateRequest(
        self: Device,
        streamer: *Streamer,
        desc: *const BufferUpdateRequestDesc,
    ) u64 {
        return self.streamer_interface.AddStreamerBufferUpdateRequest(streamer, desc);
    }

    pub inline fn addStreamerTextureUpdateRequest(
        self: Device,
        streamer: *Streamer,
        desc: *const TextureUpdateRequestDesc,
    ) u64 {
        return self.streamer_interface.AddStreamerTextureUpdateRequest(streamer, desc);
    }

    pub inline fn updateStreamerConstantBuffer(
        self: Device,
        streamer: *Streamer,
        data: ?*const u8,
        size: u32,
    ) u32 {
        return self.streamer_interface.UpdateStreamerConstantBuffer(streamer, data, size);
    }

    pub inline fn copyStreamerUpdateRequests(self: Device, streamer: *Streamer) !void {
        try check(self.streamer_interface.CopyStreamerUpdateRequests(streamer));
    }

    pub inline fn uploadStreamerUpdateRequests(self: Device, command_buffer: *CommandBuffer, streamer: *Streamer) void {
        self.streamer_interface.CmdUploadStreamerUpdateRequests(command_buffer, streamer);
    }
};

pub const Fence = opaque {};
pub const Memory = opaque {};
pub const Buffer = opaque {};
pub const RawDevice = opaque {};
pub const Texture = opaque {};
pub const Pipeline = opaque {};
pub const QueryPool = opaque {};
pub const Descriptor = opaque {};
pub const CommandQueue = opaque {};
pub const CommandBuffer = opaque {};
pub const DescriptorSet = opaque {};
pub const DescriptorPool = opaque {};
pub const PipelineLayout = opaque {};
pub const CommandAllocator = opaque {};

pub const mip = u8;
pub const sample = u8;
pub const memory_kind = u32;
pub const dimension = u16;
pub const PARTIALLY_BOUND: bool = true;
pub const DESCRIPTOR_ARRAY: bool = true;
pub const VARIABLE_DESCRIPTOR_NUM: bool = true;
pub const ALL_SAMPLES: u32 = 0;
pub const ONE_VIEWPORT: u32 = 0;
pub const WHOLE_SIZE: dimension = 0;
pub const REMAINING_MIPS: mip = 0;
pub const REMAINING_LAYERS: dimension = 0;
pub const Result = enum(u8) {
    success = 0,
    failure = 1,
    invalid_argument = 2,
    out_of_memory = 3,
    unsupported = 4,
    device_lost = 5,
    out_of_date = 6,
    max_num = 7,
    _,

    pub inline fn asErr(self: Result) ?ResultError {
        if (self == .success) {
            return null;
        }
        switch (self) {
            .failure => return error.Failure,
            .invalid_argument => return error.InvalidArgument,
            .out_of_memory => return error.OutOfMemory,
            .unsupported => return error.Unsupported,
            .device_lost => return error.DeviceLost,
            .out_of_date => return error.OutOfDate,
            else => |x| {
                std.debug.print("Unknown result: {}\n", .{x});
                return error.Failure;
            },
        }
    }
};
pub const ResultError = error{
    Failure,
    InvalidArgument,
    OutOfMemory,
    Unsupported,
    DeviceLost,
    OutOfDate,
};
pub inline fn check(r: Result) !void {
    if (r != .success) {
        return r.asErr() orelse unreachable;
    }
}

pub const Format = enum(u8) {
    unknown = 0,
    r8_unorm = 1,
    r8_snorm = 2,
    r8_uint = 3,
    r8_sint = 4,
    rg8_unorm = 5,
    rg8_snorm = 6,
    rg8_uint = 7,
    rg8_sint = 8,
    bgra8_unorm = 9,
    bgra8_srgb = 10,
    rgba8_unorm = 11,
    rgba8_srgb = 12,
    rgba8_snorm = 13,
    rgba8_uint = 14,
    rgba8_sint = 15,
    r16_unorm = 16,
    r16_snorm = 17,
    r16_uint = 18,
    r16_sint = 19,
    r16_sfloat = 20,
    rg16_unorm = 21,
    rg16_snorm = 22,
    rg16_uint = 23,
    rg16_sint = 24,
    rg16_sfloat = 25,
    rgba16_unorm = 26,
    rgba16_snorm = 27,
    rgba16_uint = 28,
    rgba16_sint = 29,
    rgba16_sfloat = 30,
    r32_uint = 31,
    r32_sint = 32,
    r32_sfloat = 33,
    rg32_uint = 34,
    rg32_sint = 35,
    rg32_sfloat = 36,
    rgb32_uint = 37,
    rgb32_sint = 38,
    rgb32_sfloat = 39,
    rgba32_uint = 40,
    rgba32_sint = 41,
    rgba32_sfloat = 42,
    b5_g6_r5_unorm = 43,
    b5_g5_r5_a1_unorm = 44,
    b4_g4_r4_a4_unorm = 45,
    r10_g10_b10_a2_unorm = 46,
    r10_g10_b10_a2_uint = 47,
    r11_g11_b10_ufloat = 48,
    r9_g9_b9_e5_ufloat = 49,
    bc1_rgba_unorm = 50,
    bc1_rgba_srgb = 51,
    bc2_rgba_unorm = 52,
    bc2_rgba_srgb = 53,
    bc3_rgba_unorm = 54,
    bc3_rgba_srgb = 55,
    bc4_r_unorm = 56,
    bc4_r_snorm = 57,
    bc5_rg_unorm = 58,
    bc5_rg_snorm = 59,
    bc6h_rgb_ufloat = 60,
    bc6h_rgb_sfloat = 61,
    bc7_rgba_unorm = 62,
    bc7_rgba_srgb = 63,
    d16_unorm = 64,
    d24_unorm_s8_uint = 65,
    d32_sfloat = 66,
    d32_sfloat_s8_uint_x24 = 67,
    r24_unorm_x8 = 68,
    x24_g8_uint = 69,
    r32_sfloat_x8_x24 = 70,
    x32_g8_uint_x24 = 71,
    max_num = 72,
};
pub const PlaneFlags = packed struct(u8) {
    color: bool = false,
    depth: bool = false,
    stencil: bool = false,
    _: u5 = 0,

    pub inline fn bits(self: PlaneFlags) u8 {
        return @bitCast(@intFromEnum(self));
    }
};
pub const FormatSupportFlags = packed struct(u16) {
    unsupported: bool = false,
    texture: bool = false,
    storage_texture: bool = false,
    color_attachment: bool = false,
    depth_stencil_attachment: bool = false,
    blend: bool = false,
    storage_texture_atomics: bool = false,
    buffer: bool = false,
    storage_buffer: bool = false,
    vertex_buffer: bool = false,
    storage_buffer_atomics: bool = false,
    _: u5 = 0,

    pub inline fn bits(self: FormatSupportFlags) u16 {
        return @bitCast(@intFromEnum(self));
    }
};
pub const StageFlags = packed struct(u32) {
    pub const all = .{};
    pub const none = .{
        .index_input = true,
        .vertex_shader = true,
        .tess_control_shader = true,
        .tess_evaluation_shader = true,
        .geometry_shader = true,
        .mesh_control_shader = true,
        .mesh_evaluation_shader = true,
        .fragment_shader = true,
        .depth_stencil_attachment = true,
        .color_attachment = true,
        .compute_shader = true,
        .raygen_shader = true,
        .miss_shader = true,
        .intersection_shader = true,
        .closest_hit_shader = true,
        .any_hit_shader = true,
        .callable_shader = true,
        .copy = true,
        .clear_storage = true,
        .acceleration_structure = true,
        .indirect = true,
    };

    index_input: bool = false,
    vertex_shader: bool = false,
    tess_control_shader: bool = false,
    tess_evaluation_shader: bool = false,
    geometry_shader: bool = false,
    mesh_control_shader: bool = false,
    mesh_evaluation_shader: bool = false,
    fragment_shader: bool = false,
    depth_stencil_attachment: bool = false,
    color_attachment: bool = false,
    compute_shader: bool = false,
    raygen_shader: bool = false,
    miss_shader: bool = false,
    intersection_shader: bool = false,
    closest_hit_shader: bool = false,
    any_hit_shader: bool = false,
    callable_shader: bool = false,
    copy: bool = false,
    clear_storage: bool = false,
    acceleration_structure: bool = false,
    indirect: bool = false,
    _: u11 = 0,

    pub const tessellation_shaders = .{
        .tess_control_shader = true,
        .tess_evaluation_shader = true,
    };

    pub const mesh_shaders = .{
        .mesh_control_shader = true,
        .mesh_evaluation_shader = true,
    };

    pub const graphics_shaders = .{
        .vertex_shader = true,
        .tessellation_shaders = true,
        .geometry_shader = true,
        .mesh_shaders = true,
        .fragment_shader = true,
    };

    pub const ray_tracing_shaders = .{
        .raygen_shader = true,
        .miss_shader = true,
        .intersection_shader = true,
        .closest_hit_shader = true,
        .any_hit_shader = true,
        .callable_shader = true,
    };

    pub const draw = .{
        .index_input = true,
        .graphics_shaders = true,
        .depth_stencil_attachment = true,
        .color_attachment = true,
    };
};
pub const Rect = extern struct {
    x: i16 = 0,
    y: i16 = 0,
    width: dimension = 0,
    height: dimension = 0,
};
pub const Viewport = extern struct {
    x: f32 = 0,
    y: f32 = 0,
    width: f32 = 0,
    height: f32 = 0,
    depth_range_min: f32 = 0,
    depth_range_max: f32 = 0,
};
pub const Color32f = extern struct {
    x: f32 = 0,
    y: f32 = 0,
    z: f32 = 0,
    w: f32 = 0,
};
pub const Color32ui = extern struct {
    x: u32 = 0,
    y: u32 = 0,
    z: u32 = 0,
    w: u32 = 0,
};
pub const Color32i = extern struct {
    x: i32 = 0,
    y: i32 = 0,
    z: i32 = 0,
    w: i32 = 0,
};
pub const DepthStencil = extern struct {
    depth: f32 = 0,
    stencil: u8 = 0,
};
pub const Color = extern union {
    f: Color32f,
    ui: Color32ui,
    i: Color32i,

    pub const black: Color = .{
        .f = .{ .x = 0.0, .y = 0.0, .z = 0.0, .w = 1.0 },
    };
};
pub const SamplePosition = extern struct {
    x: i8 = 0,
    y: i8 = 0,
};
pub const CommandQueueType = enum(u8) {
    graphics = 0,
    compute = 1,
    copy = 2,
    high_priority_copy = 3,
};
pub const MemoryLocation = enum(u8) {
    device = 0,
    device_upload = 1,
    host_upload = 2,
    host_readback = 3,
};
pub const TextureKind = enum(u8) {
    texture1D = 0,
    texture2D = 1,
    texture3D = 2,
};
pub const Texture1DViewKind = enum(u8) {
    shader_resource_1D = 0,
    shader_resource_1D_array = 1,
    shader_resource_storage_1D = 2,
    shader_resource_storage_1D_array = 3,
    color_attachment = 4,
    depth_stencil_attachment = 5,
    depth_readonly_stencil_attachment = 6,
    depth_attachment_stencil_readonly = 7,
    depth_stencil_readonly = 8,
};
pub const Texture2DViewKind = enum(u8) {
    shader_resource_2D = 0,
    shader_resource_2D_array = 1,
    shader_resource_cube = 2,
    shader_resource_cube_array = 3,
    shader_resource_storage_2D = 4,
    shader_resource_storage_2D_array = 5,
    color_attachment = 6,
    depth_stencil_attachment = 7,
    depth_readonly_stencil_attachment = 8,
    depth_attachment_stencil_readonly = 9,
    depth_stencil_readonly = 10,
    shading_rate_attachment = 11,
};
pub const Texture3DViewKind = enum(u8) {
    shader_resource_3D = 0,
    shader_resource_storage_3D = 1,
    color_attachment = 2,
};
pub const BufferViewKind = enum(u8) {
    shader_resource = 0,
    shader_resource_storage = 1,
    constant = 2,
};
pub const DescriptorKind = enum(u8) {
    sampler = 0,
    constant_buffer = 1,
    texture = 2,
    storage_texture = 3,
    buffer = 4,
    storage_buffer = 5,
    structured_buffer = 6,
    storage_structured_buffer = 7,
    acceleration_structure = 8,
};
pub const TextureUsageFlags = packed struct(u8) {
    shader_resource: bool = false,
    shader_resource_storage: bool = false,
    color_attachment: bool = false,
    depth_stencil_attachment: bool = false,
    shading_rate_attachment: bool = false,
    _: u3 = 0,

    pub inline fn bits(self: TextureUsageFlags) u8 {
        return @bitCast(@intFromEnum(self));
    }
};
pub const BufferUsageFlags = packed struct(u8) {
    shader_resource: bool = false,
    shader_resource_storage: bool = false,
    vertex_buffer: bool = false,
    index_buffer: bool = false,
    constant_buffer: bool = false,
    argument_buffer: bool = false,
    ray_tracing_buffer: bool = false,
    acceleration_structure_build_read: bool = false,

    pub inline fn bits(self: BufferUsageFlags) u8 {
        return @bitCast(@intFromEnum(self));
    }
};
pub const TextureDesc = extern struct {
    type: TextureKind = .texture2D,
    usageMask: TextureUsageFlags = .{},
    format: Format = .unknown,
    width: dimension = 0,
    height: dimension = 0,
    depth: dimension = 0,
    mipNum: mip = 0,
    layerNum: dimension = 0,
    sampleNum: sample = 0,
};
pub const BufferDesc = extern struct {
    size: u64 = 0,
    structure_stride: u32 = 0,
    usage_mask: BufferUsageFlags = .{},
};
pub const Texture1DViewDesc = extern struct {
    texture: ?*const Texture = null,
    view_type: Texture1DViewKind = .color_attachment,
    format: Format = .unknown,
    mip_offset: mip = 0,
    mip_num: mip = 0,
    layer_offset: dimension = 0,
    layer_num: dimension = 0,
};
pub const Texture2DViewDesc = extern struct {
    texture: ?*const Texture = null,
    view_type: Texture2DViewKind = .color_attachment,
    format: Format = .unknown,
    mip_offset: mip = 0,
    mip_num: mip = 0,
    layer_offset: dimension = 0,
    layer_num: dimension = 0,
};
pub const Texture3DViewDesc = extern struct {
    texture: ?*const Texture = null,
    view_type: Texture3DViewKind = .color_attachment,
    format: Format = .unknown,
    mip_offset: mip = 0,
    mip_num: mip = 0,
    slice_offset: dimension = 0,
    slice_num: dimension = 0,
};
pub const BufferViewDesc = extern struct {
    buffer: ?*const Buffer = null,
    viewType: BufferViewKind = .shader_resource,
    format: Format = .unknown,
    offset: u64 = 0,
    size: u64 = 0,
};
pub const DescriptorPoolDesc = extern struct {
    descriptorSetMaxNum: u32 = 0,
    samplerMaxNum: u32 = 0,
    constantBufferMaxNum: u32 = 0,
    dynamicConstantBufferMaxNum: u32 = 0,
    textureMaxNum: u32 = 0,
    storageTextureMaxNum: u32 = 0,
    bufferMaxNum: u32 = 0,
    storageBufferMaxNum: u32 = 0,
    structuredBufferMaxNum: u32 = 0,
    storageStructuredBufferMaxNum: u32 = 0,
    accelerationStructureMaxNum: u32 = 0,
};
pub const PushConstantDesc = extern struct {
    registerIndex: u32 = 0,
    size: u32 = 0,
    shaderStages: StageFlags = .{},
};
pub const DescriptorRangeDesc = extern struct {
    baseRegisterIndex: u32 = 0,
    descriptorNum: u32 = 0,
    descriptorType: DescriptorKind = .buffer,
    shaderStages: StageFlags = .{},
    isDescriptorNumVariable: bool = false,
    isArray: bool = false,
};
pub const DynamicConstantBufferDesc = extern struct {
    registerIndex: u32 = 0,
    shaderStages: StageFlags = .{},
};
pub const DescriptorSetDesc = extern struct {
    registerSpace: u32 = 0,
    ranges: [*c]const DescriptorRangeDesc = null,
    rangeNum: u32 = 0,
    dynamicConstantBuffers: [*c]const DynamicConstantBufferDesc = null,
    dynamicConstantBufferNum: u32 = 0,
    partiallyBound: bool = false,
};
pub const PipelineLayoutDesc = extern struct {
    descriptor_sets: ?[*]const DescriptorSetDesc = null,
    push_constants: ?[*]const PushConstantDesc = null,
    descriptor_set_num: u32 = 0,
    push_constant_num: u32 = 0,
    shader_stages: StageFlags = .{},
    ignore_global_spirv_offsets: bool = false,
    enable_d3d12_draw_parameters_emulation: bool = false,

    pub fn init(desc: struct {
        descriptor_sets: []const DescriptorSetDesc = &.{},
        push_constants: []const PushConstantDesc = &.{},
        shader_stages: StageFlags = .{},
        ignore_global_spirv_offsets: bool = false,
        enable_d3d12_draw_parameters_emulation: bool = false,
    }) PipelineLayoutDesc {
        return .{
            .descriptor_sets = desc.descriptor_sets.ptr,
            .push_constants = desc.push_constants.ptr,
            .descriptor_set_num = @intCast(desc.descriptor_sets.len),
            .push_constant_num = @intCast(desc.push_constants.len),
            .shader_stages = desc.shader_stages,
            .ignore_global_spirv_offsets = desc.ignore_global_spirv_offsets,
            .enable_d3d12_draw_parameters_emulation = desc.enable_d3d12_draw_parameters_emulation,
        };
    }
};
pub const VertexStreamStepRate = enum(u8) {
    per_vertex = 0,
    per_instance = 1,
};
pub const IndexKind = enum(u8) {
    uint16 = 0,
    uint32 = 1,
};
pub const PrimitiveRestart = enum(u8) {
    disabled = 0,
    indices_uint16 = 1,
    indices_uint32 = 2,
};
pub const Topology = enum(u8) {
    point_list = 0,
    line_list = 1,
    line_strip = 2,
    triangle_list = 3,
    triangle_strip = 4,
    line_list_with_adjacency = 5,
    line_strip_with_adjacency = 6,
    triangle_list_with_adjacency = 7,
    triangle_strip_with_adjacency = 8,
    patch_list = 9,
};
pub const InputAssemblyDesc = extern struct {
    topology: Topology = .point_list,
    tess_control_point_num: u8 = 0,
    primitive_restart: PrimitiveRestart = .disabled,
};
pub const VertexAttributeD3D = extern struct {
    name: [*c]const u8 = null,
    index: u32 = 0,

    pub fn init(name: []const u8, index: u32) VertexAttributeD3D {
        return .{
            .name = name.ptr,
            .index = index,
        };
    }
};
pub const VertexAttributeVK = extern struct {
    location: u32 = 0,
};
pub const VertexAttributeDesc = extern struct {
    d3d: VertexAttributeD3D = .{},
    vk: VertexAttributeVK = .{},
    offset: u32 = 0,
    format: Format = .unknown,
    stream_index: u16 = 0,
};
pub const VertexStreamDesc = extern struct {
    stride: u16 = 0,
    binding_slot: u16 = 0,
    step_rate: VertexStreamStepRate = .per_vertex,

    pub fn init(
        stride: u16,
        binding_slot: u16,
        step_rate: VertexStreamStepRate,
    ) VertexStreamDesc {
        return .{
            .stride = stride,
            .binding_slot = binding_slot,
            .step_rate = step_rate,
        };
    }
};
pub const VertexInputDesc = extern struct {
    attributes: ?[*]const VertexAttributeDesc = null,
    streams: ?[*]const VertexStreamDesc = null,
    attribute_num: u8 = 0,
    stream_num: u8 = 0,

    pub fn init(attributes: []const VertexAttributeDesc, streams: []const VertexStreamDesc) VertexInputDesc {
        return .{
            .attributes = attributes.ptr,
            .streams = streams.ptr,
            .attribute_num = @intCast(attributes.len),
            .stream_num = @intCast(streams.len),
        };
    }
};
pub const FillMode = enum(u8) {
    solid = 0,
    wireframe = 1,
};
pub const CullMode = enum(u8) {
    none = 0,
    front = 1,
    back = 2,
};
pub const ShadingRate = enum(u8) {
    fragment_size_1x1 = 0,
    fragment_size_1x2 = 1,
    fragment_size_2x1 = 2,
    fragment_size_2x2 = 3,
    fragment_size_2x4 = 4,
    fragment_size_4x2 = 5,
    fragment_size_4x4 = 6,
};
pub const ShadingRateCombiner = enum(u8) {
    replace = 0,
    keep = 1,
    min = 2,
    max = 3,
    sum = 4,
};
pub const DepthBiasDesc = extern struct {
    constant: f32 = 0,
    clamp: f32 = 0,
    slope: f32 = 0,
};
pub const RasterizationDesc = extern struct {
    viewport_num: u32 = 0,
    depth_bias: DepthBiasDesc = .{},
    fill_mode: FillMode = .solid,
    cull_mode: CullMode = .none,
    front_counter_clockwise: bool = false,
    depth_clamp: bool = false,
    line_smoothing: bool = false,
    conservative_raster: bool = false,
    shading_rate: bool = false,
};
pub const MultisampleDesc = extern struct {
    sampleMask: u32 = 0,
    sampleNum: sample = 0,
    alphaToCoverage: bool = false,
    programmableSampleLocations: bool = false,
};
pub const ShadingRateDesc = extern struct {
    shadingRate: ShadingRate = .fragment_size_1x1,
    primitiveCombiner: ShadingRateCombiner = .replace,
    attachmentCombiner: ShadingRateCombiner = .replace,
};
pub const LogicFunc = enum(u8) {
    none = 0,
    clear = 1,
    @"and" = 2,
    and_reverse = 3,
    copy = 4,
    and_inverted = 5,
    xor = 6,
    @"or" = 7,
    nor = 8,
    equivalent = 9,
    invert = 10,
    or_reverse = 11,
    copy_inverted = 12,
    or_inverted = 13,
    nand = 14,
    set = 15,
};
pub const CompareFunc = enum(u8) {
    none = 0,
    always = 1,
    never = 2,
    equal = 3,
    not_equal = 4,
    less = 5,
    less_equal = 6,
    greater = 7,
    greater_equal = 8,
};
pub const StencilFunc = enum(u8) {
    keep = 0,
    zero = 1,
    replace = 2,
    increment_and_clamp = 3,
    decrement_and_clamp = 4,
    invert = 5,
    increment_and_wrap = 6,
    decrement_and_wrap = 7,
};
pub const BlendFactor = enum(u8) {
    zero = 0,
    one = 1,
    src_color = 2,
    one_minus_src_color = 3,
    dst_color = 4,
    one_minus_dst_color = 5,
    src_alpha = 6,
    one_minus_src_alpha = 7,
    dst_alpha = 8,
    one_minus_dst_alpha = 9,
    constant_color = 10,
    one_minus_constant_color = 11,
    constant_alpha = 12,
    one_minus_constant_alpha = 13,
    src_alpha_saturate = 14,
    src1_color = 15,
    one_minus_src1_color = 16,
    src1_alpha = 17,
    one_minus_src1_alpha = 18,
};
pub const BlendFunc = enum(u8) {
    add = 0,
    subtract = 1,
    reverse_subtract = 2,
    min = 3,
    max = 4,
};
pub const ColorWriteFlags = packed struct(u8) {
    r: bool = false,
    g: bool = false,
    b: bool = false,
    a: bool = false,
    _: u4 = 0,

    pub inline fn bits(self: ColorWriteFlags) u8 {
        return @bitCast(@intFromEnum(self));
    }

    pub const rgba = .{
        .r = true,
        .g = true,
        .b = true,
        .a = true,
    };
    pub const rgb = .{
        .r = true,
        .g = true,
        .b = true,
    };
};
pub const ClearValue = extern union {
    depthStencil: DepthStencil,
    color: Color,
};
pub const ClearDesc = extern struct {
    value: ClearValue = .{ .color = .{ .f = .{ .x = 0.0, .y = 0.0, .z = 0.0, .w = 0.0 } } },
    planes: PlaneFlags = .{},
    color_attachment_index: u32 = 0,
};
pub const StencilDesc = extern struct {
    compareFunc: CompareFunc = .none,
    fail: StencilFunc = .keep,
    pass: StencilFunc = .keep,
    depthFail: StencilFunc = .keep,
    writeMask: u8 = 0,
    compareMask: u8 = 0,
};
pub const BlendingDesc = extern struct {
    src_factor: BlendFactor = .zero,
    dst_factor: BlendFactor = .zero,
    func: BlendFunc = .add,
};
pub const ColorAttachmentDesc = extern struct {
    format: Format = .unknown,
    color_blend: BlendingDesc = .{},
    alpha_blend: BlendingDesc = .{},
    color_write_mask: ColorWriteFlags = .{},
    blend_enabled: bool = false,
};
pub const DepthAttachmentDesc = extern struct {
    compareFunc: CompareFunc = .none,
    write: bool = false,
    boundsTest: bool = false,
};
pub const StencilAttachmentDesc = extern struct {
    front: StencilDesc = .{},
    back: StencilDesc = .{},
};
pub const OutputMergerDesc = extern struct {
    color: ?*const ColorAttachmentDesc = null,
    depth: DepthAttachmentDesc = .{},
    stencil: StencilAttachmentDesc = .{},
    depth_stencil_format: Format = .unknown,
    color_logic_func: LogicFunc = .none,
    color_num: u32 = 0,
};
pub const AttachmentsDesc = extern struct {
    depth_stencil: ?*const Descriptor = null,
    shading_rate: ?*const Descriptor = null,
    colors: ?[*]const ?*const Descriptor = null,
    color_num: u32 = 0,
};
pub const Filter = enum(u8) {
    nearest = 0,
    linear = 1,
};
pub const FilterExt = enum(u8) {
    none = 0,
    min = 1,
    max = 2,
};
pub const AddressMode = enum(u8) {
    repeat = 0,
    mirrored_repeat = 1,
    clamp_to_edge = 2,
    clamp_to_border = 3,
    mirror_clamp_to_edge = 4,
};
pub const AddressModes = extern struct {
    u: AddressMode = .repeat,
    v: AddressMode = .repeat,
    w: AddressMode = .repeat,
};
pub const Filters = extern struct {
    min: Filter = .nearest,
    mag: Filter = .nearest,
    mip: Filter = .nearest,
    ext: FilterExt = .none,
};
pub const SamplerDesc = extern struct {
    filters: Filters = .{},
    anisotropy: u8 = 0,
    mipBias: f32 = 0,
    mipMin: f32 = 0,
    mipMax: f32 = 0,
    addressModes: AddressModes = .{},
    compareFunc: CompareFunc = .none,
    borderColor: Color = .black,
    isInteger: bool = false,
};
pub const ShaderDesc = extern struct {
    stage: StageFlags = .{},
    bytecode: ?*const anyopaque = null,
    size: u64 = 0,
    entry_point_name: [*:0]const u8 = "main",

    pub fn init(
        desc: struct {
            stage: StageFlags,
            bytecode: []const u8,
            entry_point_name: [:0]const u8 = "main",
            override_size: u64 = 0, // override
        },
    ) ShaderDesc {
        return .{
            .stage = desc.stage,
            .bytecode = desc.bytecode.ptr,
            .size = if (desc.override_size > 0) desc.override_size else @intCast(desc.bytecode.len),
            .entry_point_name = desc.entry_point_name.ptr,
        };
    }
};
pub const GraphicsPipelineDesc = extern struct {
    pipeline_layout: ?*const PipelineLayout = null,
    vertex_input: ?*const VertexInputDesc = null,
    input_assembly: InputAssemblyDesc = .{},
    rasterization: RasterizationDesc = .{},
    multisample: ?*const MultisampleDesc = null,
    output_merger: OutputMergerDesc = .{},
    shaders: ?[*]const ShaderDesc = null,
    shader_num: u32 = 0,

    pub fn init(desc: struct {
        pipeline_layout: ?*const PipelineLayout = null,
        vertex_input: ?*const VertexInputDesc = null,
        input_assembly: InputAssemblyDesc = .{},
        rasterization: RasterizationDesc = .{},
        multisample: ?*const MultisampleDesc = null,
        output_merger: OutputMergerDesc = .{},
        shaders: []const ShaderDesc = &.{},
    }) GraphicsPipelineDesc {
        return .{
            .pipeline_layout = desc.pipeline_layout,
            .vertex_input = desc.vertex_input,
            .input_assembly = desc.input_assembly,
            .rasterization = desc.rasterization,
            .multisample = desc.multisample,
            .output_merger = desc.output_merger,
            .shaders = desc.shaders.ptr,
            .shader_num = @intCast(desc.shaders.len),
        };
    }
};
pub const ComputePipelineDesc = extern struct {
    pipelineLayout: ?*const PipelineLayout = null,
    shader: ShaderDesc = .{},
};
pub const Layout = enum(u8) {
    unknown = 0,
    color_attachment = 1,
    depth_stencil_attachment = 2,
    depth_stencil_readonly = 3,
    shader_resource = 4,
    shader_resource_storage = 5,
    copy_source = 6,
    copy_destination = 7,
    present = 8,
    shading_rate_attachment = 9,
};
pub const AccessFlags = packed struct(u16) {
    index_buffer: bool = false,
    vertex_buffer: bool = false,
    constant_buffer: bool = false,
    shader_resource: bool = false,
    shader_resource_storage: bool = false,
    argument_buffer: bool = false,
    color_attachment: bool = false,
    depth_stencil_attachment_write: bool = false,
    depth_stencil_attachment_read: bool = false,
    copy_source: bool = false,
    copy_destination: bool = false,
    acceleration_structure_read: bool = false,
    acceleration_structure_write: bool = false,
    shading_rate_attachment: bool = false,
    _: u2 = 0,

    pub inline fn bits(self: AccessFlags) u16 {
        return @bitCast(@intFromEnum(self));
    }
};
pub const AccessStage = extern struct {
    access: AccessFlags = .{},
    stages: StageFlags = .{},
};
pub const AccessLayoutStage = extern struct {
    access: AccessFlags = .{},
    layout: Layout = .unknown,
    stages: StageFlags = .{},
};
pub const GlobalBarrierDesc = extern struct {
    before: AccessStage = .{},
    after: AccessStage = .{},
};
pub const BufferBarrierDesc = extern struct {
    buffer: ?*Buffer = null,
    before: AccessStage = .{},
    after: AccessStage = .{},
};
pub const TextureBarrierDesc = extern struct {
    texture: ?*Texture = null,
    before: AccessLayoutStage = .{},
    after: AccessLayoutStage = .{},
    mip_offset: mip = 0,
    mip_num: mip = 0,
    layer_offset: dimension = 0,
    layer_num: dimension = 0,
    planes: PlaneFlags = .{},
};
pub const BarrierGroupDesc = extern struct {
    globals: ?[*]const GlobalBarrierDesc = null,
    buffers: ?[*]const BufferBarrierDesc = null,
    textures: ?[*]const TextureBarrierDesc = null,
    global_num: u16 = 0,
    buffer_num: u16 = 0,
    texture_num: u16 = 0,
};
pub const TextureRegionDesc = extern struct {
    x: u16 = 0,
    y: u16 = 0,
    z: u16 = 0,
    width: dimension = 0,
    height: dimension = 0,
    depth: dimension = 0,
    mipOffset: mip = 0,
    layerOffset: dimension = 0,
};
pub const TextureDataLayoutDesc = extern struct {
    offset: u64 = 0,
    rowPitch: u32 = 0,
    slicePitch: u32 = 0,
};
pub const FenceSubmitDesc = extern struct {
    fence: ?*Fence = null,
    value: u64 = 0,
    stages: StageFlags = .{},
};
pub const QueueSubmitDesc = extern struct {
    wait_fences: ?[*]const FenceSubmitDesc = null,
    wait_fence_num: u32 = 0,
    command_buffers: ?[*]const *const CommandBuffer = null,
    command_buffer_num: u32 = 0,
    signal_fences: ?[*]const FenceSubmitDesc = null,
    signal_fence_num: u32 = 0,
};
pub const MemoryDesc = extern struct {
    size: u64 = 0,
    alignment: u32 = 0,
    type: memory_kind = 0,
    mustBeDedicated: bool = false,
};
pub const AllocateMemoryDesc = extern struct {
    size: u64 = 0,
    type: memory_kind = 0,
    priority: f32 = 0,
};
pub const BufferMemoryBindingDesc = extern struct {
    memory: ?*Memory = null,
    buffer: ?*Buffer = null,
    offset: u64 = 0,
};
pub const TextureMemoryBindingDesc = extern struct {
    memory: ?*Memory = null,
    texture: ?*Texture = null,
    offset: u64 = 0,
};
pub const ClearStorageBufferDesc = extern struct {
    storageBuffer: ?*const Descriptor = null,
    value: u32 = 0,
    setIndexInPipelineLayout: u32 = 0,
    rangeIndex: u32 = 0,
    offsetInRange: u32 = 0,
};
pub const ClearStorageTextureDesc = extern struct {
    storageTexture: ?*const Descriptor = null,
    value: ClearValue = .{ .color = .{ .f = .{ .x = 0.0, .y = 0.0, .z = 0.0, .w = 0.0 } } },
    setIndexInPipelineLayout: u32 = 0,
    rangeIndex: u32 = 0,
    offsetInRange: u32 = 0,
};
pub const DescriptorRangeUpdateDesc = extern struct {
    descriptors: [*c]const ?*const Descriptor = null,
    descriptorNum: u32 = 0,
    offsetInRange: u32 = 0,
};
pub const DescriptorSetCopyDesc = extern struct {
    srcDescriptorSet: ?*const DescriptorSet = null,
    baseSrcRange: u32 = 0,
    baseDstRange: u32 = 0,
    rangeNum: u32 = 0,
    baseSrcDynamicConstantBuffer: u32 = 0,
    baseDstDynamicConstantBuffer: u32 = 0,
    dynamicConstantBufferNum: u32 = 0,
};
pub const DrawDesc = extern struct {
    vertexNum: u32 = 0,
    instanceNum: u32 = 0,
    baseVertex: u32 = 0,
    baseInstance: u32 = 0,
};
pub const DrawIndexedDesc = extern struct {
    index_num: u32 = 0,
    instance_num: u32 = 0,
    base_index: u32 = 0,
    base_vertex: i32 = 0,
    base_instance: u32 = 0,
};
pub const DispatchDesc = extern struct {
    x: u32 = 0,
    y: u32 = 0,
    z: u32 = 0,
};
pub const DrawBaseDesc = extern struct {
    shaderEmulatedBaseVertex: u32 = 0,
    shaderEmulatedBaseInstance: u32 = 0,
    vertexNum: u32 = 0,
    instanceNum: u32 = 0,
    baseVertex: u32 = 0,
    baseInstance: u32 = 0,
};
pub const DrawIndexedBaseDesc = extern struct {
    shaderEmulatedBaseVertex: i32 = 0,
    shaderEmulatedBaseInstance: u32 = 0,
    indexNum: u32 = 0,
    instanceNum: u32 = 0,
    baseIndex: u32 = 0,
    baseVertex: i32 = 0,
    baseInstance: u32 = 0,
};
pub const QueryType = enum(u8) {
    timestamp = 0,
    timestamp_copy_queue = 1,
    occlusion = 2,
    pipeline_statistics = 3,
    acceleration_structure_compacted_size = 4,
};
pub const QueryPoolDesc = extern struct {
    queryType: QueryType = .timestamp,
    capacity: u32 = 0,
};
pub const PipelineStatisticsDesc = extern struct {
    inputVertexNum: u64 = 0,
    inputPrimitiveNum: u64 = 0,
    vertexShaderInvocationNum: u64 = 0,
    geometryShaderInvocationNum: u64 = 0,
    geometryShaderPrimitiveNum: u64 = 0,
    rasterizerInPrimitiveNum: u64 = 0,
    rasterizerOutPrimitiveNum: u64 = 0,
    fragmentShaderInvocationNum: u64 = 0,
    tessControlShaderInvocationNum: u64 = 0,
    tessEvaluationShaderInvocationNum: u64 = 0,
    computeShaderInvocationNum: u64 = 0,
    meshControlShaderInvocationNum: u64 = 0,
    meshEvaluationShaderInvocationNum: u64 = 0,
    meshEvaluationShaderPrimitiveNum: u64 = 0,
};
pub const GraphicsAPI = enum(u8) {
    d3d11 = 0,
    d3d12 = 1,
    vk = 2,
};
pub const Vendor = enum(u8) {
    unknown = 0,
    nvidia = 1,
    amd = 2,
    intel = 3,
};
pub const AdapterDesc = extern struct {
    name: [256]u8 = undefined,
    luid: u64 = 0,
    videoMemorySize: u64 = 0,
    systemMemorySize: u64 = 0,
    deviceId: u32 = 0,
    vendor: Vendor = .unknown,
};

pub const DeviceDesc = extern struct {
    adapter_desc: AdapterDesc = .{},
    graphics_api: GraphicsAPI,
    version_major: u16 = 0,
    version_minor: u16 = 0,

    viewport_max_num: u32 = 0,
    viewport_sub_pixel_bits: u32 = 0,
    viewport_bounds_range: [2]i32 = undefined,

    attachment_max_dim: dimension,
    attachment_layer_max_num: dimension,
    color_attachment_max_num: dimension,

    color_sample_max_num: sample,
    depth_sample_max_num: sample,
    stencil_sample_max_num: sample,
    zero_attachments_sample_max_num: sample,
    texture_color_sample_max_num: sample,
    texture_integer_sample_max_num: sample,
    texture_depth_sample_max_num: sample,
    texture_stencil_sample_max_num: sample,
    storage_texture_sample_max_num: sample,

    texture_1D_max_dim: dimension,
    texture_2D_max_dim: dimension,
    texture_3D_max_dim: dimension,
    texture_array_layer_max_num: dimension,
    texel_buffer_max_dim: u32,

    device_upload_heap_size: u64,
    memory_allocation_max_num: u32,
    sampler_allocation_max_num: u32,
    constant_buffer_max_range: u32,
    storage_buffer_max_range: u32,
    buffer_texture_granularity: u32,
    buffer_max_size: u64,
    push_constants_max_size: u32,

    upload_buffer_texture_row_alignment: u32,
    upload_buffer_texture_slice_alignment: u32,
    typed_buffer_offset_alignment: u32,
    constant_buffer_offset_alignment: u32,
    storage_buffer_offset_alignment: u32,
    ray_tracing_shader_table_alignment: u32,
    ray_tracing_scratch_alignment: u32,

    bound_descriptor_set_max_num: u32,
    per_stage_descriptor_sampler_max_num: u32,
    per_stage_descriptor_constant_buffer_max_num: u32,
    per_stage_descriptor_storage_buffer_max_num: u32,
    per_stage_descriptor_texture_max_num: u32,
    per_stage_descriptor_storage_texture_max_num: u32,
    per_stage_resource_max_num: u32,

    descriptor_set_sampler_max_num: u32,
    descriptor_set_constant_buffer_max_num: u32,
    descriptor_set_storage_buffer_max_num: u32,
    descriptor_set_texture_max_num: u32,
    descriptor_set_storage_texture_max_num: u32,

    vertex_shader_attribute_max_num: u32,
    vertex_shader_stream_max_num: u32,
    vertex_shader_output_component_max_num: u32,

    tess_control_shader_generation_max_level: f32,
    tess_control_shader_patch_point_max_num: u32,
    tess_control_shader_per_vertex_input_component_max_num: u32,
    tess_control_shader_per_vertex_output_component_max_num: u32,
    tess_control_shader_per_patch_output_component_max_num: u32,
    tess_control_shader_total_output_component_max_num: u32,
    tess_evaluation_shader_input_component_max_num: u32,
    tess_evaluation_shader_output_component_max_num: u32,

    geometry_shader_invocation_max_num: u32,
    geometry_shader_input_component_max_num: u32,
    geometry_shader_output_component_max_num: u32,
    geometry_shader_output_vertex_max_num: u32,
    geometry_shader_total_output_component_max_num: u32,

    fragment_shader_input_component_max_num: u32,
    fragment_shader_output_attachment_max_num: u32,
    fragment_shader_dual_source_attachment_max_num: u32,
    fragment_shader_combined_output_resource_max_num: u32,

    compute_shader_shared_memory_max_size: u32,
    compute_shader_work_group_max_num: [3]u32,
    compute_shader_work_group_invocation_max_num: u32,
    compute_shader_work_group_max_dim: [3]u32,

    ray_tracing_shader_group_identifier_size: u32,
    ray_tracing_shader_table_max_stride: u32,
    ray_tracing_shader_recursion_max_depth: u32,
    ray_tracing_geometry_object_max_num: u32,

    mesh_control_shared_memory_max_size: u32,
    mesh_control_work_group_invocation_max_num: u32,
    mesh_control_payload_max_size: u32,
    mesh_evaluation_output_vertices_max_num: u32,
    mesh_evaluation_output_primitive_max_num: u32,
    mesh_evaluation_output_component_max_num: u32,
    mesh_evaluation_shared_memory_max_size: u32,
    mesh_evaluation_work_group_invocation_max_num: u32,

    timestamp_frequency_hz: u64,
    sub_pixel_precision_bits: u32,
    sub_texel_precision_bits: u32,
    mipmap_precision_bits: u32,
    draw_indirect_max_num: u32,
    sampler_lod_bias_min: f32,
    sampler_lod_bias_max: f32,
    sampler_anisotropy_max: f32,
    texel_offset_min: i32,
    texel_offset_max: u32,
    texel_gather_offset_min: i32,
    texel_gather_offset_max: u32,
    clip_distance_max_num: u32,
    cull_distance_max_num: u32,
    combined_clip_and_cull_distance_max_num: u32,
    conservative_raster_tier: u8,
    programmable_sample_locations_tier: u8,
    shading_rate_attachment_tile_size: u8,

    is_compute_queue_supported: bool,
    is_copy_queue_supported: bool,
    is_texture_filter_min_max_supported: bool,
    is_logic_op_supported: bool,
    is_depth_bounds_test_supported: bool,
    is_draw_indirect_count_supported: bool,
    is_independent_front_and_back_stencil_reference_and_masks_supported: bool,
    is_line_smoothing_supported: bool,
    is_copy_queue_timestamp_supported: bool,
    is_dispatch_rays_indirect_supported: bool,
    is_draw_mesh_tasks_indirect_supported: bool,
    is_mesh_shader_pipeline_stats_supported: bool,
    is_enchanced_barrier_supported: bool,
    is_memory_tier2_supported: bool,
    is_pipeline_shading_rate_supported: bool,
    is_primitive_shading_rate_supported: bool,
    is_attachment_shading_rate_supported: bool,
    is_dynamic_depth_bias_supported: bool,

    is_shader_native_i16_supported: bool,
    is_shader_native_f16_supported: bool,
    is_shader_native_i32_supported: bool,
    is_shader_native_f32_supported: bool,
    is_shader_native_i64_supported: bool,
    is_shader_native_f64_supported: bool,

    is_shader_atomics_i16_supported: bool,
    is_shader_atomics_f16_supported: bool,
    is_shader_atomics_i32_supported: bool,
    is_shader_atomics_f32_supported: bool,
    is_shader_atomics_i64_supported: bool,
    is_shader_atomics_f64_supported: bool,

    is_draw_parameters_emulation_enabled: bool,

    is_swap_chain_supported: bool,
    is_ray_tracing_supported: bool,
    is_mesh_shader_supported: bool,
    is_low_latency_supported: bool,
};

extern fn nriGetInterface(device: *const RawDevice, interfaceName: [*c]const u8, interfaceSize: usize, interfacePtr: ?*anyopaque) Result;
pub const CoreInterface = extern struct {
    GetDeviceDesc: *const fn (*const RawDevice) callconv(.C) *const DeviceDesc,
    GetBufferDesc: *const fn (*const Buffer) callconv(.C) *const BufferDesc,
    GetTextureDesc: *const fn (*const Texture) callconv(.C) *const TextureDesc,
    GetFormatSupport: *const fn (*const RawDevice, Format) callconv(.C) FormatSupportFlags,
    GetQuerySize: *const fn (*const QueryPool) callconv(.C) u32,
    GetBufferMemoryDesc: *const fn (*const RawDevice, *const BufferDesc, MemoryLocation, *MemoryDesc) callconv(.C) void,
    GetTextureMemoryDesc: *const fn (*const RawDevice, *const TextureDesc, MemoryLocation, *MemoryDesc) callconv(.C) void,
    GetCommandQueue: *const fn (*RawDevice, CommandQueueType, *?*CommandQueue) callconv(.C) Result,
    CreateCommandAllocator: *const fn (*const CommandQueue, *?*CommandAllocator) callconv(.C) Result,
    CreateCommandBuffer: *const fn (*CommandAllocator, *?*CommandBuffer) callconv(.C) Result,
    CreateDescriptorPool: *const fn (*RawDevice, *const DescriptorPoolDesc, *?*DescriptorPool) callconv(.C) Result,
    CreateBuffer: *const fn (*RawDevice, *const BufferDesc, *?*Buffer) callconv(.C) Result,
    CreateTexture: *const fn (*RawDevice, *const TextureDesc, *?*Texture) callconv(.C) Result,
    CreateBufferView: *const fn (*const BufferViewDesc, *?*Descriptor) callconv(.C) Result,
    CreateTexture1DView: *const fn (*const Texture1DViewDesc, *?*Descriptor) callconv(.C) Result,
    CreateTexture2DView: *const fn (*const Texture2DViewDesc, *?*Descriptor) callconv(.C) Result,
    CreateTexture3DView: *const fn (*const Texture3DViewDesc, *?*Descriptor) callconv(.C) Result,
    CreateSampler: *const fn (*RawDevice, *const SamplerDesc, *?*Descriptor) callconv(.C) Result,
    CreatePipelineLayout: *const fn (*RawDevice, *const PipelineLayoutDesc, *?*PipelineLayout) callconv(.C) Result,
    CreateGraphicsPipeline: *const fn (*RawDevice, *const GraphicsPipelineDesc, *?*Pipeline) callconv(.C) Result,
    CreateComputePipeline: *const fn (*RawDevice, *const ComputePipelineDesc, *?*Pipeline) callconv(.C) Result,
    CreateQueryPool: *const fn (*RawDevice, *const QueryPoolDesc, *?*QueryPool) callconv(.C) Result,
    CreateFence: *const fn (*RawDevice, u64, *?*Fence) callconv(.C) Result,
    DestroyCommandAllocator: *const fn (*CommandAllocator) callconv(.C) void,
    DestroyCommandBuffer: *const fn (*CommandBuffer) callconv(.C) void,
    DestroyDescriptorPool: *const fn (*DescriptorPool) callconv(.C) void,
    DestroyBuffer: *const fn (*Buffer) callconv(.C) void,
    DestroyTexture: *const fn (*Texture) callconv(.C) void,
    DestroyDescriptor: *const fn (*Descriptor) callconv(.C) void,
    DestroyPipelineLayout: *const fn (*PipelineLayout) callconv(.C) void,
    DestroyPipeline: *const fn (*Pipeline) callconv(.C) void,
    DestroyQueryPool: *const fn (*QueryPool) callconv(.C) void,
    DestroyFence: *const fn (*Fence) callconv(.C) void,
    AllocateMemory: *const fn (*RawDevice, *const AllocateMemoryDesc, *?*Memory) callconv(.C) Result,
    BindBufferMemory: *const fn (*RawDevice, [*]const BufferMemoryBindingDesc, u32) callconv(.C) Result,
    BindTextureMemory: *const fn (*RawDevice, [*]const TextureMemoryBindingDesc, u32) callconv(.C) Result,
    FreeMemory: *const fn (*Memory) callconv(.C) void,
    BeginCommandBuffer: *const fn (*CommandBuffer, ?*const DescriptorPool) callconv(.C) Result,
    CmdSetDescriptorPool: *const fn (*CommandBuffer, *const DescriptorPool) callconv(.C) void,
    CmdSetPipelineLayout: *const fn (*CommandBuffer, *const PipelineLayout) callconv(.C) void,
    CmdSetDescriptorSet: *const fn (*CommandBuffer, u32, *const DescriptorSet, [*c]const u32) callconv(.C) void,
    CmdSetRootConstants: *const fn (*CommandBuffer, u32, *const anyopaque, u32) callconv(.C) void,
    CmdSetRootDescriptor: *const fn (*CommandBuffer, u32, *const Descriptor) callconv(.C) void,
    CmdSetPipeline: *const fn (*CommandBuffer, *const Pipeline) callconv(.C) void,
    CmdBarrier: *const fn (*CommandBuffer, *const BarrierGroupDesc) callconv(.C) void,
    CmdSetIndexBuffer: *const fn (*CommandBuffer, *const Buffer, u64, IndexKind) callconv(.C) void,
    CmdSetVertexBuffers: *const fn (*CommandBuffer, u32, u32, [*]const *const Buffer, [*]const u64) callconv(.C) void,
    CmdSetViewports: *const fn (*CommandBuffer, [*]const Viewport, u32) callconv(.C) void,
    CmdSetScissors: *const fn (*CommandBuffer, [*]const Rect, u32) callconv(.C) void,
    CmdSetStencilReference: *const fn (*CommandBuffer, u8, u8) callconv(.C) void,
    CmdSetDepthBounds: *const fn (*CommandBuffer, f32, f32) callconv(.C) void,
    CmdSetBlendConstants: *const fn (*CommandBuffer, *const Color32f) callconv(.C) void,
    CmdSetSampleLocations: *const fn (*CommandBuffer, [*]const SamplePosition, sample, sample) callconv(.C) void,
    CmdSetShadingRate: *const fn (*CommandBuffer, *const ShadingRateDesc) callconv(.C) void,
    CmdSetDepthBias: *const fn (*CommandBuffer, *const DepthBiasDesc) callconv(.C) void,
    CmdBeginRendering: *const fn (*CommandBuffer, *const AttachmentsDesc) callconv(.C) void,
    CmdClearAttachments: *const fn (*CommandBuffer, [*]const ClearDesc, u32, [*]const Rect, u32) callconv(.C) void,
    CmdDraw: *const fn (*CommandBuffer, *const DrawDesc) callconv(.C) void,
    CmdDrawIndexed: *const fn (*CommandBuffer, *const DrawIndexedDesc) callconv(.C) void,
    CmdDrawIndirect: *const fn (*CommandBuffer, *const Buffer, u64, u32, u32, *const Buffer, u64) callconv(.C) void,
    CmdDrawIndexedIndirect: *const fn (*CommandBuffer, *const Buffer, u64, u32, u32, *const Buffer, u64) callconv(.C) void,
    CmdEndRendering: *const fn (*CommandBuffer) callconv(.C) void,
    CmdDispatch: *const fn (*CommandBuffer, *const DispatchDesc) callconv(.C) void,
    CmdDispatchIndirect: *const fn (*CommandBuffer, *const Buffer, u64) callconv(.C) void,
    CmdCopyBuffer: *const fn (*CommandBuffer, *Buffer, u64, *const Buffer, u64, u64) callconv(.C) void,
    CmdCopyTexture: *const fn (*CommandBuffer, *Texture, *const TextureRegionDesc, *const Texture, *const TextureRegionDesc) callconv(.C) void,
    CmdResolveTexture: *const fn (*CommandBuffer, *Texture, *const TextureRegionDesc, *const Texture, *const TextureRegionDesc) callconv(.C) void,
    CmdUploadBufferToTexture: *const fn (*CommandBuffer, *Texture, *const TextureRegionDesc, *const Buffer, *const TextureDataLayoutDesc) callconv(.C) void,
    CmdReadbackTextureToBuffer: *const fn (*CommandBuffer, *Buffer, *const TextureDataLayoutDesc, *const Texture, *const TextureRegionDesc) callconv(.C) void,
    CmdClearStorageBuffer: *const fn (*CommandBuffer, *const ClearStorageBufferDesc) callconv(.C) void,
    CmdClearStorageTexture: *const fn (*CommandBuffer, *const ClearStorageTextureDesc) callconv(.C) void,
    CmdResetQueries: *const fn (*CommandBuffer, *const QueryPool, u32, u32) callconv(.C) void,
    CmdBeginQuery: *const fn (*CommandBuffer, *const QueryPool, u32) callconv(.C) void,
    CmdEndQuery: *const fn (*CommandBuffer, *const QueryPool, u32) callconv(.C) void,
    CmdCopyQueries: *const fn (*CommandBuffer, *const QueryPool, u32, u32, *Buffer, u64) callconv(.C) void,
    CmdBeginAnnotation: *const fn (*CommandBuffer, [*]const u8) callconv(.C) void,
    CmdEndAnnotation: *const fn (*CommandBuffer) callconv(.C) void,
    EndCommandBuffer: *const fn (*CommandBuffer) callconv(.C) Result,
    QueueSubmit: *const fn (*CommandQueue, *const QueueSubmitDesc) callconv(.C) void,
    Wait: *const fn (*Fence, u64) callconv(.C) void,
    GetFenceValue: *const fn (*Fence) callconv(.C) u64,
    UpdateDescriptorRanges: *const fn (*DescriptorSet, u32, u32, [*]const DescriptorRangeUpdateDesc) callconv(.C) void,
    UpdateDynamicConstantBuffers: *const fn (*DescriptorSet, u32, u32, [*]const *const Descriptor) callconv(.C) void,
    CopyDescriptorSet: *const fn (*DescriptorSet, *const DescriptorSetCopyDesc) callconv(.C) void,
    AllocateDescriptorSets: *const fn (*DescriptorPool, *const PipelineLayout, u32, [*]*DescriptorSet, u32, u32) callconv(.C) Result,
    ResetDescriptorPool: *const fn (*DescriptorPool) callconv(.C) void,
    ResetCommandAllocator: *const fn (*CommandAllocator) callconv(.C) void,
    MapBuffer: *const fn (*Buffer, u64, u64) callconv(.C) ?*anyopaque,
    UnmapBuffer: *const fn (*Buffer) callconv(.C) void,
    SetDeviceDebugName: *const fn (*RawDevice, [*:0]const u8) callconv(.C) void,
    SetFenceDebugName: *const fn (*Fence, [*:0]const u8) callconv(.C) void,
    SetDescriptorDebugName: *const fn (*Descriptor, [*:0]const u8) callconv(.C) void,
    SetPipelineDebugName: *const fn (*Pipeline, [*:0]const u8) callconv(.C) void,
    SetCommandBufferDebugName: *const fn (*CommandBuffer, [*:0]const u8) callconv(.C) void,
    SetBufferDebugName: *const fn (*Buffer, [*:0]const u8) callconv(.C) void,
    SetTextureDebugName: *const fn (*Texture, [*:0]const u8) callconv(.C) void,
    SetCommandQueueDebugName: *const fn (*CommandQueue, [*:0]const u8) callconv(.C) void,
    SetCommandAllocatorDebugName: *const fn (*CommandAllocator, [*:0]const u8) callconv(.C) void,
    SetDescriptorPoolDebugName: *const fn (*DescriptorPool, [*:0]const u8) callconv(.C) void,
    SetPipelineLayoutDebugName: *const fn (*PipelineLayout, [*:0]const u8) callconv(.C) void,
    SetQueryPoolDebugName: *const fn (*QueryPool, [*:0]const u8) callconv(.C) void,
    SetDescriptorSetDebugName: *const fn (*DescriptorSet, [*:0]const u8) callconv(.C) void,
    SetMemoryDebugName: *const fn (*Memory, [*:0]const u8) callconv(.C) void,
    GetDeviceNativeObject: *const fn (*const RawDevice) callconv(.C) ?*anyopaque,
    GetCommandBufferNativeObject: *const fn (*const CommandBuffer) callconv(.C) ?*anyopaque,
    GetBufferNativeObject: *const fn (*const Buffer) callconv(.C) u64,
    GetTextureNativeObject: *const fn (*const Texture) callconv(.C) u64,
    GetDescriptorNativeObject: *const fn (*const Descriptor) callconv(.C) u64,
};
pub fn nriGetSupportedDepthFormat(arg_coreInterface: [*c]const CoreInterface, arg_device: ?*const RawDevice, arg_minBits: u32, arg_stencil: bool) callconv(.C) Format {
    var coreInterface = arg_coreInterface;
    _ = &coreInterface;
    var device = arg_device;
    _ = &device;
    var minBits = arg_minBits;
    _ = &minBits;
    var stencil = arg_stencil;
    _ = &stencil;
    if ((minBits <= 16) and !stencil) {
        if (coreInterface.*.GetFormatSupport.?(device, .d16_unorm).depth_stencil_attachment) return .d16_unorm;
    }
    if (minBits <= 24) {
        if (coreInterface.*.GetFormatSupport.?(device, .d24_unorm_s8_uint).depth_stencil_attachment) return .d24_unorm_s8_uint;
    }
    if ((minBits <= 32) and !stencil) {
        if (coreInterface.*.GetFormatSupport.?(device, .d32_sfloat).depth_stencil_attachment) return .d32_sfloat;
    }
    if (coreInterface.*.GetFormatSupport.?(device, .d32_sfloat_s8_uint_x24).depth_stencil_attachment) return .d32_sfloat_s8_uint_x24;
    return .unknown;
}

// device creatione ext

pub const Message = enum(u8) {
    info = 0,
    warning = 1,
    err = 2,
};
pub const AllocationCallbacks = extern struct {
    Allocate: ?*const fn (?*anyopaque, usize, usize) callconv(.C) ?*anyopaque = null,
    Reallocate: ?*const fn (?*anyopaque, ?*anyopaque, usize, usize) callconv(.C) ?*anyopaque = null,
    Free: ?*const fn (?*anyopaque, ?*anyopaque) callconv(.C) void = null,
    userArg: ?*anyopaque = null,
};
pub const CallbackInterface = extern struct {
    MessageCallback: ?*const fn (Message, [*c]const u8, u32, [*c]const u8, ?*anyopaque) callconv(.C) void = null,
    AbortExecution: ?*const fn (?*anyopaque) callconv(.C) void = null,
    userArg: ?*anyopaque = null,
};
pub const SPIRVBindingOffsets = extern struct {
    samplerOffset: u32 = 0,
    textureOffset: u32 = 0,
    constantBufferOffset: u32 = 0,
    storageTextureAndBufferOffset: u32 = 0,
};
pub const VKExtensions = extern struct {
    instanceExtensions: [*c]const [*c]const u8 = null,
    instanceExtensionNum: u32 = 0,
    deviceExtensions: [*c]const [*c]const u8 = null,
    deviceExtensionNum: u32 = 0,
};
pub const DeviceCreationDesc = extern struct {
    adapter_desc: ?*const AdapterDesc = null,
    callback_interface: CallbackInterface = .{},
    allocation_callbacks: AllocationCallbacks = .{},
    spirv_binding_offsets: SPIRVBindingOffsets = .{},
    vk_extensions: VKExtensions = .{},
    graphics_api: GraphicsAPI = .vk,
    shader_ext_register: u32 = 0,
    shader_ext_space: u32 = 0,
    enable_validation: bool = false,
    enable_graphics_api_validation: bool = false,
    enable_d3d12_draw_parameters_emulation: bool = false,
    enable_d3d11_command_buffer_emulation: bool = false,
    disable_vk_ray_tracing: bool = true,
    disable3rd_party_allocation_callbacks: bool = true,
};
pub const enumerateAdapters = nriEnumerateAdapters;
extern fn nriEnumerateAdapters(adapterDescs: [*]AdapterDesc, adapterDescNum: *u32) Result;
pub const createDevice = nriCreateDevice;
extern fn nriCreateDevice(deviceCreationDesc: *const DeviceCreationDesc, device: *?*RawDevice) Result;
pub const destroyDevice = nriDestroyDevice;
extern fn nriDestroyDevice(device: ?*RawDevice) void;
pub const reportLiveObjects = nriReportLiveObjects;
extern fn nriReportLiveObjects() void;

// swap chain ext

pub const SwapChain = opaque {};
pub const OUT_OF_DATE: u32 = @as(u32, @bitCast(-@as(c_int, 1)));
pub const SwapChainFormat = enum(u8) {
    bt709_g10_16bit = 0,
    bt709_g22_8bit = 1,
    bt709_g22_10bit = 2,
    bt2020_g2084_10bit = 3,
};
pub const SwapChainDesc = extern struct {
    window: ?*anyopaque = null,
    command_queue: *const CommandQueue,
    width: dimension = 0,
    height: dimension = 0,
    texture_num: u8 = 2,
    format: SwapChainFormat = .bt709_g22_8bit,
    vertical_sync_interval: u8 = 0,
    queued_frame_num: u8 = 0,
    waitable: bool = true,
    allow_low_latency: bool = true,
};
pub const ChromaticityCoords = extern struct {
    x: f32 = 0,
    y: f32 = 0,
};
pub const DisplayDesc = extern struct {
    redPrimary: ChromaticityCoords = .{},
    greenPrimary: ChromaticityCoords = .{},
    bluePrimary: ChromaticityCoords = .{},
    whitePoint: ChromaticityCoords = .{},
    minLuminance: f32 = 0,
    maxLuminance: f32 = 0,
    maxFullFrameLuminance: f32 = 0,
    sdrLuminance: f32 = 0,
    isHDR: bool = false,
};
pub const SwapChainInterface = extern struct {
    CreateSwapChain: *const fn (*RawDevice, *const SwapChainDesc, *?*SwapChain) callconv(.C) Result,
    DestroySwapChain: *const fn (*SwapChain) callconv(.C) void,
    SetSwapChainDebugName: *const fn (*SwapChain, [*]const u8) callconv(.C) void,
    GetSwapChainTextures: *const fn (*const SwapChain, *u32) callconv(.C) [*]const *Texture,
    AcquireNextSwapChainTexture: *const fn (*SwapChain) callconv(.C) u32,
    WaitForPresent: *const fn (*SwapChain) callconv(.C) Result,
    QueuePresent: *const fn (*SwapChain) callconv(.C) Result,
    GetDisplayDesc: *const fn (*SwapChain, *DisplayDesc) callconv(.C) Result,
};

// streamer ext
pub const Streamer = opaque {};

pub const StreamerDesc = extern struct {
    constant_buffer_memory_location: MemoryLocation = .device,
    constant_buffer_size: u64 = 0,

    dynamic_buffer_memory_location: MemoryLocation = .host_upload,
    dynamic_buffer_usage_flags: BufferUsageFlags = .{},
    frame_in_flight_num: u32 = 0,
};

pub const BufferUpdateRequestDesc = extern struct {
    data: ?*const anyopaque = null,
    dataSize: u64 = 0,
    dstBuffer: ?*const Buffer = null,
    dstBufferOffset: u64 = 0,
};

pub const TextureUpdateRequestDesc = extern struct {
    data: ?*const anyopaque = null,
    dataRowPitch: u32 = 0,
    dataSlicePitch: u32 = 0,
    dstTexture: ?*const Texture = null,
    dstRegionDesc: TextureRegionDesc = .{},
};

pub const StreamerInterface = extern struct {
    CreateStreamer: *const fn (*RawDevice, *const StreamerDesc, *?*Streamer) callconv(.C) Result,
    DestroyStreamer: *const fn (*Streamer) callconv(.C) void,
    GetStreamerConstantBuffer: *const fn (*Streamer) callconv(.C) ?*Buffer,
    GetStreamerDynamicBuffer: *const fn (*Streamer) callconv(.C) ?*Buffer,
    AddStreamerBufferUpdateRequest: *const fn (*Streamer, *const BufferUpdateRequestDesc) callconv(.C) u64,
    AddStreamerTextureUpdateRequest: *const fn (*Streamer, *const TextureUpdateRequestDesc) callconv(.C) u64,
    UpdateStreamerConstantBuffer: *const fn (*Streamer, ?*const anyopaque, u32) callconv(.C) u32,
    CopyStreamerUpdateRequests: *const fn (*Streamer) callconv(.C) Result,
    CmdUploadStreamerUpdateRequests: *const fn (*CommandBuffer, *Streamer) callconv(.C) void,
};

// helper

pub const VideoMemoryInfo = extern struct {
    budgetSize: u64 = 0,
    usageSize: u64 = 0,
};

pub const TextureSubresourceUploadDesc = extern struct {
    slices: ?*const anyopaque = null,
    sliceNum: u32 = 0,
    rowPitch: u32 = 0,
    slicePitch: u32 = 0,
};

pub const TextureUploadDesc = extern struct {
    subresources: ?*const TextureSubresourceUploadDesc = null,
    texture: *const Texture,
    after: AccessLayoutStage = .{},
    planes: PlaneFlags = .{},
};

pub const BufferUploadDesc = extern struct {
    data: ?[*]const u8 = null,
    data_size: u64 = 0,
    buffer: *const Buffer,
    buffer_offset: u64 = 0,
    after: AccessStage = .{},

    pub fn init(desc: struct {
        data: []const u8,
        buffer: *const Buffer,
        buffer_offset: u64 = 0,
        after: AccessStage,
    }) BufferUploadDesc {
        return .{
            .data = desc.data.ptr,
            .data_size = desc.data.len,
            .buffer = desc.buffer,
            .buffer_offset = desc.buffer_offset,
            .after = desc.after,
        };
    }
};

pub const ResourceGroupDesc = extern struct {
    memory_location: MemoryLocation = .device,
    textures: ?[*]const *const Texture = null,
    texture_num: u32 = 0,
    buffers: ?[*]const *const Buffer = null,
    buffer_num: u32 = 0,
    preferred_memory_size: u64 = 0, // default goes to 256 Mb
};

pub const FormatProps = packed struct {
    name: [*:0]const u8,
    format: Format,
    redBits: u8,
    greenBits: u8,
    blueBits: u8,
    alphaBits: u8,
    stride: u6,
    blockWidth: u4,
    blockHeight: u4,
    isBgr: bool,
    isCompressed: bool,
    isDepth: bool,
    isExpShared: bool,
    isFloat: bool,
    isPacked: bool,
    isInteger: bool,
    isNorm: bool,
    isSigned: bool,
    isSrgb: bool,
    isStencil: bool,
    _: u7,
};

pub const HelperInterface = extern struct {
    CalculateAllocationNumber: *const fn (*RawDevice, *const ResourceGroupDesc) callconv(.C) u32,
    AllocateAndBindMemory: *const fn (*RawDevice, *const ResourceGroupDesc, [*]*Memory) callconv(.C) Result,
    UploadData: *const fn (*CommandQueue, [*]const TextureUploadDesc, u32, [*]const BufferUploadDesc, u32) callconv(.C) Result,
    WaitForIdle: *const fn (*CommandQueue) callconv(.C) Result,
    QueryVideoMemoryInfo: *const fn (*RawDevice, MemoryLocation, *VideoMemoryInfo) callconv(.C) Result,
};
