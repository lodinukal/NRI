// © 2021 NVIDIA Corporation

#pragma once

namespace nri {

constexpr std::array<VkIndexType, (size_t)IndexType::MAX_NUM> INDEX_TYPE_TABLE = {
    VK_INDEX_TYPE_UINT16, // UINT16
    VK_INDEX_TYPE_UINT32, // UINT32
};

constexpr VkIndexType GetIndexType(IndexType indexType) {
    return INDEX_TYPE_TABLE[(size_t)indexType];
}

constexpr std::array<VkImageLayout, (size_t)Layout::MAX_NUM> LAYOUT_TABLE = {
    VK_IMAGE_LAYOUT_UNDEFINED,                                    // UNKNOWN
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                     // COLOR_ATTACHMENT
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,             // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,              // DEPTH_STENCIL_READONLY
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                     // SHADER_RESOURCE
    VK_IMAGE_LAYOUT_GENERAL,                                      // SHADER_RESOURCE_STORAGE
    IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                            // COPY_SOURCE
    IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                            // COPY_DESTINATION
    IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,                            // RESOLVE_SOURCE
    IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,                            // RESOLVE_DESTINATION
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                              // PRESENT
    VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR, // SHADING_RATE_ATTACHMENT
};

constexpr VkImageLayout GetImageLayout(Layout layout) {
    return LAYOUT_TABLE[(size_t)layout];
}

constexpr std::array<VkDescriptorType, (size_t)DescriptorType::MAX_NUM> DESCRIPTOR_TYPES = {
    VK_DESCRIPTOR_TYPE_SAMPLER,                    // SAMPLER
    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,             // CONSTANT_BUFFER
    VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,              // TEXTURE
    VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,              // STORAGE_TEXTURE
    VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,       // BUFFER
    VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,       // STORAGE_BUFFER
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,             // STRUCTURED_BUFFER
    VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,             // STORAGE_STRUCTURED_BUFFER
    VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, // ACCELERATION_STRUCTURE
};

constexpr VkDescriptorType GetDescriptorType(DescriptorType type) {
    return DESCRIPTOR_TYPES[(size_t)type];
}

constexpr std::array<VkPrimitiveTopology, (size_t)Topology::MAX_NUM> TOPOLOGIES = {
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST,                    // POINT_LIST
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,                     // LINE_LIST
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,                    // LINE_STRIP
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                 // TRIANGLE_LIST
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,                // TRIANGLE_STRIP
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,      // LINE_LIST_WITH_ADJACENCY
    VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,     // LINE_STRIP_WITH_ADJACENCY
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,  // TRIANGLE_LIST_WITH_ADJACENCY
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY, // TRIANGLE_STRIP_WITH_ADJACENCY
    VK_PRIMITIVE_TOPOLOGY_PATCH_LIST                     // PATCH_LIST
};

constexpr VkPrimitiveTopology GetTopology(Topology topology) {
    return TOPOLOGIES[(size_t)topology];
}

constexpr std::array<VkCullModeFlags, (size_t)CullMode::MAX_NUM> CULL_MODES = {
    VK_CULL_MODE_NONE,      // NONE
    VK_CULL_MODE_FRONT_BIT, // FRONT
    VK_CULL_MODE_BACK_BIT   // BACK
};

constexpr VkCullModeFlags GetCullModeVk(CullMode cullMode) {
    return CULL_MODES[(size_t)cullMode];
}

constexpr std::array<VkPolygonMode, (size_t)FillMode::MAX_NUM> POLYGON_MODES = {
    VK_POLYGON_MODE_FILL, // SOLID
    VK_POLYGON_MODE_LINE, // WIREFRAME
};

constexpr VkPolygonMode GetPolygonMode(FillMode fillMode) {
    return POLYGON_MODES[(size_t)fillMode];
}

constexpr std::array<VkCompareOp, (size_t)CompareFunc::MAX_NUM> COMPARE_OP = {
    VK_COMPARE_OP_NEVER,            // NONE
    VK_COMPARE_OP_ALWAYS,           // ALWAYS
    VK_COMPARE_OP_NEVER,            // NEVER
    VK_COMPARE_OP_EQUAL,            // EQUAL
    VK_COMPARE_OP_NOT_EQUAL,        // NOT_EQUAL
    VK_COMPARE_OP_LESS,             // LESS
    VK_COMPARE_OP_LESS_OR_EQUAL,    // LESS_EQUAL
    VK_COMPARE_OP_GREATER,          // GREATER
    VK_COMPARE_OP_GREATER_OR_EQUAL, // GREATER_EQUAL
};

constexpr VkCompareOp GetCompareOp(CompareFunc compareFunc) {
    return COMPARE_OP[(size_t)compareFunc];
}

constexpr std::array<VkStencilOp, (size_t)StencilFunc::MAX_NUM> STENCIL_OP = {
    VK_STENCIL_OP_KEEP,                // KEEP,
    VK_STENCIL_OP_ZERO,                // ZERO,
    VK_STENCIL_OP_REPLACE,             // REPLACE,
    VK_STENCIL_OP_INCREMENT_AND_CLAMP, // INCREMENT_AND_CLAMP,
    VK_STENCIL_OP_DECREMENT_AND_CLAMP, // DECREMENT_AND_CLAMP,
    VK_STENCIL_OP_INVERT,              // INVERT,
    VK_STENCIL_OP_INCREMENT_AND_WRAP,  // INCREMENT_AND_WRAP,
    VK_STENCIL_OP_DECREMENT_AND_WRAP   // DECREMENT_AND_WRAP
};

constexpr VkStencilOp GetStencilOpVK(StencilFunc stencilFunc) {
    return STENCIL_OP[(size_t)stencilFunc];
}

constexpr std::array<VkLogicOp, (size_t)LogicFunc::MAX_NUM> LOGIC_OP = {
    VK_LOGIC_OP_MAX_ENUM,      // NONE
    VK_LOGIC_OP_CLEAR,         // CLEAR
    VK_LOGIC_OP_AND,           // AND
    VK_LOGIC_OP_AND_REVERSE,   // AND_REVERSE
    VK_LOGIC_OP_COPY,          // COPY
    VK_LOGIC_OP_AND_INVERTED,  // AND_INVERTED
    VK_LOGIC_OP_XOR,           // XOR
    VK_LOGIC_OP_OR,            // OR
    VK_LOGIC_OP_NOR,           // NOR
    VK_LOGIC_OP_EQUIVALENT,    // EQUIVALENT
    VK_LOGIC_OP_INVERT,        // INVERT
    VK_LOGIC_OP_OR_REVERSE,    // OR_REVERSE
    VK_LOGIC_OP_COPY_INVERTED, // COPY_INVERTED
    VK_LOGIC_OP_OR_INVERTED,   // OR_INVERTED
    VK_LOGIC_OP_NAND,          // NAND
    VK_LOGIC_OP_SET            // SET
};

constexpr VkLogicOp GetLogicOpVK(LogicFunc logicFunc) {
    return LOGIC_OP[(size_t)logicFunc];
}

constexpr std::array<VkBlendFactor, (size_t)BlendFactor::MAX_NUM> BLEND_FACTOR = {
    VK_BLEND_FACTOR_ZERO,                     // ZERO
    VK_BLEND_FACTOR_ONE,                      // ONE
    VK_BLEND_FACTOR_SRC_COLOR,                // SRC_COLOR
    VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,      // ONE_MINUS_SRC_COLOR
    VK_BLEND_FACTOR_DST_COLOR,                // DST_COLOR
    VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,      // ONE_MINUS_DST_COLOR
    VK_BLEND_FACTOR_SRC_ALPHA,                // SRC_ALPHA
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,      // ONE_MINUS_SRC_ALPHA
    VK_BLEND_FACTOR_DST_ALPHA,                // DST_ALPHA
    VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,      // ONE_MINUS_DST_ALPHA
    VK_BLEND_FACTOR_CONSTANT_COLOR,           // CONSTANT_COLOR
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR, // ONE_MINUS_CONSTANT_COLOR
    VK_BLEND_FACTOR_CONSTANT_ALPHA,           // CONSTANT_ALPHA
    VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA, // ONE_MINUS_CONSTANT_ALPHA
    VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,       // SRC_ALPHA_SATURATE
    VK_BLEND_FACTOR_SRC1_COLOR,               // SRC1_COLOR
    VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,     // ONE_MINUS_SRC1_COLOR
    VK_BLEND_FACTOR_SRC1_ALPHA,               // SRC1_ALPHA
    VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA,     // ONE_MINUS_SRC1_ALPHA
};

constexpr VkBlendFactor GetBlendFactor(BlendFactor blendFactor) {
    return BLEND_FACTOR[(size_t)blendFactor];
}

constexpr std::array<VkBlendOp, (size_t)BlendFunc::MAX_NUM> BLEND_OP = {
    VK_BLEND_OP_ADD,              // ADD
    VK_BLEND_OP_SUBTRACT,         // SUBTRACT
    VK_BLEND_OP_REVERSE_SUBTRACT, // REVERSE_SUBTRACT
    VK_BLEND_OP_MIN,              // MIN
    VK_BLEND_OP_MAX               // MAX
};

constexpr VkBlendOp GetBlendOpVK(BlendFunc blendFunc) {
    return BLEND_OP[(size_t)blendFunc];
}

constexpr std::array<VkImageType, (size_t)TextureType::MAX_NUM> IMAGE_TYPES = {
    VK_IMAGE_TYPE_1D, // TEXTURE_1D
    VK_IMAGE_TYPE_2D, // TEXTURE_2D
    VK_IMAGE_TYPE_3D, // TEXTURE_3D
};

constexpr VkImageType GetImageType(TextureType type) {
    return IMAGE_TYPES[(size_t)type];
}

constexpr std::array<VkFilter, (size_t)Filter::MAX_NUM> FILTER = {
    VK_FILTER_NEAREST, // NEAREST
    VK_FILTER_LINEAR,  // LINEAR
};

constexpr VkFilter GetFilter(Filter filter) {
    return FILTER[(size_t)filter];
}

constexpr std::array<VkSamplerMipmapMode, (size_t)Filter::MAX_NUM> SAMPLER_MIPMAP_MODE = {
    VK_SAMPLER_MIPMAP_MODE_NEAREST, // NEAREST
    VK_SAMPLER_MIPMAP_MODE_LINEAR,  // LINEAR
};

constexpr VkSamplerMipmapMode GetSamplerMipmapMode(Filter filter) {
    return SAMPLER_MIPMAP_MODE[(size_t)filter];
}

constexpr VkSamplerAddressMode GetSamplerAddressMode(AddressMode addressMode) {
    return (VkSamplerAddressMode)(VK_SAMPLER_ADDRESS_MODE_REPEAT + (uint32_t)addressMode);
}

constexpr std::array<VkImageViewType, (size_t)Texture1DViewType::MAX_NUM> IMAGE_VIEW_TYPE_1D = {
    VK_IMAGE_VIEW_TYPE_1D,       // SHADER_RESOURCE_1D,
    VK_IMAGE_VIEW_TYPE_1D_ARRAY, // SHADER_RESOURCE_1D_ARRAY,
    VK_IMAGE_VIEW_TYPE_1D,       // SHADER_RESOURCE_STORAGE_1D,
    VK_IMAGE_VIEW_TYPE_1D_ARRAY, // SHADER_RESOURCE_STORAGE_1D_ARRAY,
    VK_IMAGE_VIEW_TYPE_1D,       // COLOR_ATTACHMENT,
    VK_IMAGE_VIEW_TYPE_1D,       // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_VIEW_TYPE_1D,       // DEPTH_READONLY_STENCIL_ATTACHMENT,
    VK_IMAGE_VIEW_TYPE_1D,       // DEPTH_ATTACHMENT_STENCIL_READONLY,
    VK_IMAGE_VIEW_TYPE_1D,       // DEPTH_STENCIL_READONLY,
};

constexpr VkImageViewType GetImageViewType(Texture1DViewType type) {
    return IMAGE_VIEW_TYPE_1D[(size_t)type];
}

constexpr std::array<VkImageViewType, (size_t)Texture2DViewType::MAX_NUM> IMAGE_VIEW_TYPE_2D = {
    VK_IMAGE_VIEW_TYPE_2D,         // SHADER_RESOURCE_2D,
    VK_IMAGE_VIEW_TYPE_2D_ARRAY,   // SHADER_RESOURCE_2D_ARRAY,
    VK_IMAGE_VIEW_TYPE_CUBE,       // SHADER_RESOURCE_CUBE,
    VK_IMAGE_VIEW_TYPE_CUBE_ARRAY, // SHADER_RESOURCE_CUBE_ARRAY,
    VK_IMAGE_VIEW_TYPE_2D,         // SHADER_RESOURCE_STORAGE_2D,
    VK_IMAGE_VIEW_TYPE_2D_ARRAY,   // SHADER_RESOURCE_STORAGE_2D_ARRAY,
    VK_IMAGE_VIEW_TYPE_2D,         // COLOR_ATTACHMENT,
    VK_IMAGE_VIEW_TYPE_2D,         // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_VIEW_TYPE_2D,         // DEPTH_READONLY_STENCIL_ATTACHMENT,
    VK_IMAGE_VIEW_TYPE_2D,         // DEPTH_ATTACHMENT_STENCIL_READONLY,
    VK_IMAGE_VIEW_TYPE_2D,         // DEPTH_STENCIL_READONLY,
    VK_IMAGE_VIEW_TYPE_2D,         // SHADING_RATE_ATTACHMENT
};

constexpr VkImageViewType GetImageViewType(Texture2DViewType type) {
    return IMAGE_VIEW_TYPE_2D[(size_t)type];
}

constexpr std::array<VkImageViewType, (size_t)Texture3DViewType::MAX_NUM> IMAGE_VIEW_TYPE_3D = {
    VK_IMAGE_VIEW_TYPE_3D, // SHADER_RESOURCE_3D,
    VK_IMAGE_VIEW_TYPE_3D, // SHADER_RESOURCE_STORAGE_3D,
    VK_IMAGE_VIEW_TYPE_3D, // COLOR_ATTACHMENT
};

constexpr VkImageViewType GetImageViewType(Texture3DViewType type) {
    return IMAGE_VIEW_TYPE_3D[(size_t)type];
}

constexpr std::array<VkImageUsageFlags, (size_t)Texture1DViewType::MAX_NUM> IMAGE_VIEW_USAGE_1D = {
    VK_IMAGE_USAGE_SAMPLED_BIT,                  // SHADER_RESOURCE_1D,
    VK_IMAGE_USAGE_SAMPLED_BIT,                  // SHADER_RESOURCE_1D_ARRAY,
    VK_IMAGE_USAGE_STORAGE_BIT,                  // SHADER_RESOURCE_STORAGE_1D,
    VK_IMAGE_USAGE_STORAGE_BIT,                  // SHADER_RESOURCE_STORAGE_1D_ARRAY,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,         // COLOR_ATTACHMENT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, // DEPTH_READONLY_STENCIL_ATTACHMENT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, // DEPTH_ATTACHMENT_STENCIL_READONLY,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, // DEPTH_STENCIL_READONLY,
};

constexpr VkImageUsageFlags GetImageViewUsage(Texture1DViewType type) {
    return IMAGE_VIEW_USAGE_1D[(size_t)type];
}

constexpr std::array<VkImageUsageFlags, (size_t)Texture2DViewType::MAX_NUM> IMAGE_VIEW_USAGE_2D = {
    VK_IMAGE_USAGE_SAMPLED_BIT,                              // SHADER_RESOURCE_2D,
    VK_IMAGE_USAGE_SAMPLED_BIT,                              // SHADER_RESOURCE_2D_ARRAY,
    VK_IMAGE_USAGE_SAMPLED_BIT,                              // SHADER_RESOURCE_CUBE,
    VK_IMAGE_USAGE_SAMPLED_BIT,                              // SHADER_RESOURCE_CUBE_ARRAY,
    VK_IMAGE_USAGE_STORAGE_BIT,                              // SHADER_RESOURCE_STORAGE_2D,
    VK_IMAGE_USAGE_STORAGE_BIT,                              // SHADER_RESOURCE_STORAGE_2D_ARRAY,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,                     // COLOR_ATTACHMENT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,             // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,             // DEPTH_READONLY_STENCIL_ATTACHMENT,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,             // DEPTH_ATTACHMENT_STENCIL_READONLY,
    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,             // DEPTH_STENCIL_READONLY,
    VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR, // SHADING_RATE_ATTACHMENT
};

constexpr VkImageUsageFlags GetImageViewUsage(Texture2DViewType type) {
    return IMAGE_VIEW_USAGE_2D[(size_t)type];
}

constexpr std::array<VkImageUsageFlags, (size_t)Texture3DViewType::MAX_NUM> IMAGE_VIEW_USAGE_3D = {
    VK_IMAGE_USAGE_SAMPLED_BIT,          // SHADER_RESOURCE_3D,
    VK_IMAGE_USAGE_STORAGE_BIT,          // SHADER_RESOURCE_STORAGE_3D,
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // COLOR_ATTACHMENT
};

constexpr VkImageUsageFlags GetImageViewUsage(Texture3DViewType type) {
    return IMAGE_VIEW_USAGE_3D[(size_t)type];
}

constexpr std::array<VkImageLayout, (size_t)Texture1DViewType::MAX_NUM> IMAGE_LAYOUT_1D = {
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                   // SHADER_RESOURCE_1D,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                   // SHADER_RESOURCE_1D_ARRAY,
    VK_IMAGE_LAYOUT_GENERAL,                                    // SHADER_RESOURCE_STORAGE_1D,
    VK_IMAGE_LAYOUT_GENERAL,                                    // SHADER_RESOURCE_STORAGE_1D_ARRAY,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                   // COLOR_ATTACHMENT,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,           // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL, // DEPTH_READONLY_STENCIL_ATTACHMENT,
    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL, // DEPTH_ATTACHMENT_STENCIL_READONLY,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,            // DEPTH_STENCIL_READONLY,
};

constexpr VkImageLayout GetImageLayoutForView(Texture1DViewType type) {
    return IMAGE_LAYOUT_1D[(size_t)type];
}

constexpr std::array<VkImageLayout, (size_t)Texture2DViewType::MAX_NUM> IMAGE_LAYOUT_2D = {
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                     // SHADER_RESOURCE_2D,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                     // SHADER_RESOURCE_2D_ARRAY,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                     // SHADER_RESOURCE_CUBE,
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,                     // SHADER_RESOURCE_CUBE_ARRAY,
    VK_IMAGE_LAYOUT_GENERAL,                                      // SHADER_RESOURCE_STORAGE_2D,
    VK_IMAGE_LAYOUT_GENERAL,                                      // SHADER_RESOURCE_STORAGE_2D_ARRAY,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                     // COLOR_ATTACHMENT,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,             // DEPTH_STENCIL_ATTACHMENT
    VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,   // DEPTH_READONLY_STENCIL_ATTACHMENT,
    VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,   // DEPTH_ATTACHMENT_STENCIL_READONLY,
    VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,              // DEPTH_STENCIL_READONLY,
    VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR, // SHADING_RATE_ATTACHMENT
};

constexpr VkImageLayout GetImageLayoutForView(Texture2DViewType type) {
    return IMAGE_LAYOUT_2D[(size_t)type];
}

constexpr std::array<VkImageLayout, (size_t)Texture3DViewType::MAX_NUM> IMAGE_LAYOUT_3D = {
    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, // SHADER_RESOURCE_3D,
    VK_IMAGE_LAYOUT_GENERAL,                  // SHADER_RESOURCE_STORAGE_3D,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // COLOR_ATTACHMENT
};

constexpr VkImageLayout GetImageLayoutForView(Texture3DViewType type) {
    return IMAGE_LAYOUT_3D[(size_t)type];
}

constexpr std::array<VkFragmentShadingRateCombinerOpKHR, (size_t)ShadingRateCombiner::MAX_NUM> SHADING_RATE_COMBINER = {
    VK_FRAGMENT_SHADING_RATE_COMBINER_OP_REPLACE_KHR, // REPLACE,
    VK_FRAGMENT_SHADING_RATE_COMBINER_OP_KEEP_KHR,    // KEEP,
    VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MIN_KHR,     // MIN,
    VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MAX_KHR,     // MAX,
    VK_FRAGMENT_SHADING_RATE_COMBINER_OP_MUL_KHR,     // SUM, // TODO: SUM vs MUL?
};

constexpr VkFragmentShadingRateCombinerOpKHR GetShadingRateVKCombiner(ShadingRateCombiner combiner) {
    return SHADING_RATE_COMBINER[(size_t)combiner];
}

constexpr std::array<TextureType, (size_t)TextureType::MAX_NUM> TEXTURE_TYPE_TABLE = {
    TextureType::TEXTURE_1D, // VK_IMAGE_TYPE_1D
    TextureType::TEXTURE_2D, // VK_IMAGE_TYPE_2D
    TextureType::TEXTURE_3D, // VK_IMAGE_TYPE_3D
};

constexpr TextureType GetTextureType(VkImageType type) {
    if ((size_t)type < TEXTURE_TYPE_TABLE.size())
        return TEXTURE_TYPE_TABLE[(size_t)type];

    return TextureType::MAX_NUM;
}

constexpr VkPipelineStageFlags2 GetPipelineStageFlags(StageBits stageBits) {
    // Check non-mask values first
    if (stageBits == StageBits::ALL)
        return VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

    if (stageBits == StageBits::NONE)
        return VK_PIPELINE_STAGE_2_NONE;

    // Gather bits
    VkPipelineStageFlags2 flags = 0;

    if (stageBits & StageBits::INDEX_INPUT)
        flags |= VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;

    if (stageBits & StageBits::VERTEX_SHADER)
        flags |= VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;

    if (stageBits & StageBits::TESS_CONTROL_SHADER)
        flags |= VK_PIPELINE_STAGE_2_TESSELLATION_CONTROL_SHADER_BIT;

    if (stageBits & StageBits::TESS_EVALUATION_SHADER)
        flags |= VK_PIPELINE_STAGE_2_TESSELLATION_EVALUATION_SHADER_BIT;

    if (stageBits & StageBits::GEOMETRY_SHADER)
        flags |= VK_PIPELINE_STAGE_2_GEOMETRY_SHADER_BIT;

    if (stageBits & StageBits::MESH_CONTROL_SHADER)
        flags |= VK_PIPELINE_STAGE_2_TASK_SHADER_BIT_EXT;

    if (stageBits & StageBits::MESH_EVALUATION_SHADER)
        flags |= VK_PIPELINE_STAGE_2_MESH_SHADER_BIT_EXT;

    if (stageBits & StageBits::FRAGMENT_SHADER)
        flags |= VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

    if (stageBits & StageBits::DEPTH_STENCIL_ATTACHMENT)
        flags |= VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;

    if (stageBits & StageBits::COLOR_ATTACHMENT)
        flags |= VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

    if (stageBits & StageBits::COMPUTE_SHADER)
        flags |= VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT;

    if (stageBits & StageBits::RAY_TRACING_SHADERS)
        flags |= VK_PIPELINE_STAGE_2_RAY_TRACING_SHADER_BIT_KHR;

    if (stageBits & StageBits::INDIRECT)
        flags |= VK_PIPELINE_STAGE_2_DRAW_INDIRECT_BIT;

    if (stageBits & (StageBits::COPY | StageBits::CLEAR_STORAGE | StageBits::RESOLVE))
        flags |= VK_PIPELINE_STAGE_2_TRANSFER_BIT;

    if (stageBits & StageBits::ACCELERATION_STRUCTURE)
        flags |= VK_PIPELINE_STAGE_2_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;

    return flags;
}

constexpr VkShaderStageFlags GetShaderStageFlags(StageBits stage) {
    // Check non-mask values first
    if (stage == StageBits::ALL)
        return VK_SHADER_STAGE_ALL;

    // Gather bits
    VkShaderStageFlags stageFlags = 0;

    if (stage & StageBits::VERTEX_SHADER)
        stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;

    if (stage & StageBits::TESS_CONTROL_SHADER)
        stageFlags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

    if (stage & StageBits::TESS_EVALUATION_SHADER)
        stageFlags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

    if (stage & StageBits::GEOMETRY_SHADER)
        stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;

    if (stage & StageBits::FRAGMENT_SHADER)
        stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;

    if (stage & StageBits::COMPUTE_SHADER)
        stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;

    if (stage & StageBits::RAYGEN_SHADER)
        stageFlags |= VK_SHADER_STAGE_RAYGEN_BIT_KHR;

    if (stage & StageBits::MISS_SHADER)
        stageFlags |= VK_SHADER_STAGE_MISS_BIT_KHR;

    if (stage & StageBits::INTERSECTION_SHADER)
        stageFlags |= VK_SHADER_STAGE_INTERSECTION_BIT_KHR;

    if (stage & StageBits::CLOSEST_HIT_SHADER)
        stageFlags |= VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;

    if (stage & StageBits::ANY_HIT_SHADER)
        stageFlags |= VK_SHADER_STAGE_ANY_HIT_BIT_KHR;

    if (stage & StageBits::CALLABLE_SHADER)
        stageFlags |= VK_SHADER_STAGE_CALLABLE_BIT_KHR;

    if (stage & StageBits::MESH_CONTROL_SHADER)
        stageFlags |= VK_SHADER_STAGE_TASK_BIT_EXT;

    if (stage & StageBits::MESH_EVALUATION_SHADER)
        stageFlags |= VK_SHADER_STAGE_MESH_BIT_EXT;

    return stageFlags;
}

constexpr VkImageAspectFlags GetImageAspectFlags(Format format) {
    switch (format) {
        case Format::D16_UNORM:
        case Format::D32_SFLOAT:
        case Format::R24_UNORM_X8:
        case Format::R32_SFLOAT_X8_X24:
            return VK_IMAGE_ASPECT_DEPTH_BIT;

        case Format::D24_UNORM_S8_UINT:
        case Format::D32_SFLOAT_S8_UINT_X24:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;

        case Format::X32_G8_UINT_X24:
        case Format::X24_G8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;

        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

constexpr bool HasStencil(Format format) {
    switch (format) {
        case nri::Format::D24_UNORM_S8_UINT:
            return true;
        case nri::Format::D32_SFLOAT_S8_UINT_X24:
            return true;
        case nri::Format::X24_G8_UINT:
            return true;
        case nri::Format::X32_G8_UINT_X24:
            return true;
        default:
            return false;
    }
}

constexpr Result GetReturnCode(VkResult result) {
    switch (result) {
        case VK_SUCCESS:
        case VK_EVENT_SET:
        case VK_EVENT_RESET:
            return Result::SUCCESS;

        case VK_ERROR_DEVICE_LOST:
            return Result::DEVICE_LOST;

        case VK_ERROR_SURFACE_LOST_KHR:
        case VK_ERROR_OUT_OF_DATE_KHR:
        case VK_SUBOPTIMAL_KHR:
            return Result::OUT_OF_DATE;

        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
        case VK_ERROR_INCOMPATIBLE_DRIVER:
        case VK_ERROR_FEATURE_NOT_PRESENT:
        case VK_ERROR_EXTENSION_NOT_PRESENT:
        case VK_ERROR_LAYER_NOT_PRESENT:
            return Result::UNSUPPORTED;

        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        case VK_ERROR_VALIDATION_FAILED_EXT:
        case VK_ERROR_NOT_PERMITTED_EXT:
        case VK_ERROR_TOO_MANY_OBJECTS:
        case VK_ERROR_MEMORY_MAP_FAILED:
        case VK_ERROR_INITIALIZATION_FAILED:
        case VK_INCOMPLETE:
        case VK_TIMEOUT:
        case VK_NOT_READY:
            return Result::FAILURE;

        case VK_ERROR_OUT_OF_HOST_MEMORY:
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        case VK_ERROR_OUT_OF_POOL_MEMORY:
        case VK_ERROR_FRAGMENTATION_EXT:
        case VK_ERROR_FRAGMENTED_POOL:
            return Result::OUT_OF_MEMORY;

        default:
            return Result::FAILURE;
    }
}

constexpr VkColorComponentFlags GetColorComponent(ColorWriteBits colorWriteMask) {
    return VkColorComponentFlags(colorWriteMask & ColorWriteBits::RGBA);
}

constexpr VkAccelerationStructureTypeKHR GetAccelerationStructureTypeVk(AccelerationStructureType type) {
    static_assert(VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR == (uint32_t)AccelerationStructureType::TOP_LEVEL, "Enum mismatch");
    static_assert(VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR == (uint32_t)AccelerationStructureType::BOTTOM_LEVEL, "Enum mismatch");

    return (VkAccelerationStructureTypeKHR)type;
}

constexpr VkBuildAccelerationStructureFlagsKHR GetAccelerationStructureBuildFlagsVk(AccelerationStructureBuildBits flags) {
    static_assert(VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_UPDATE_BIT_KHR == (uint32_t)AccelerationStructureBuildBits::ALLOW_UPDATE, "Enum mismatch");
    static_assert(VK_BUILD_ACCELERATION_STRUCTURE_ALLOW_COMPACTION_BIT_KHR == (uint32_t)AccelerationStructureBuildBits::ALLOW_COMPACTION, "Enum mismatch");
    static_assert(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR == (uint32_t)AccelerationStructureBuildBits::PREFER_FAST_TRACE, "Enum mismatch");
    static_assert(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_BUILD_BIT_KHR == (uint32_t)AccelerationStructureBuildBits::PREFER_FAST_BUILD, "Enum mismatch");
    static_assert(VK_BUILD_ACCELERATION_STRUCTURE_LOW_MEMORY_BIT_KHR == (uint32_t)AccelerationStructureBuildBits::MINIMIZE_MEMORY, "Enum mismatch");

    return (VkBuildAccelerationStructureFlagsKHR)flags;
}

constexpr VkGeometryFlagsKHR GetGeometryFlags(BottomLevelGeometryBits geometryFlags) {
    static_assert(VK_GEOMETRY_OPAQUE_BIT_KHR == (uint32_t)BottomLevelGeometryBits::OPAQUE_GEOMETRY, "Enum mismatch");
    static_assert(VK_GEOMETRY_NO_DUPLICATE_ANY_HIT_INVOCATION_BIT_KHR == (uint32_t)BottomLevelGeometryBits::NO_DUPLICATE_ANY_HIT_INVOCATION, "Enum mismatch");

    return (VkGeometryFlagsKHR)geometryFlags;
}

constexpr VkGeometryTypeKHR GetGeometryType(GeometryType geometryType) {
    static_assert(VK_GEOMETRY_TYPE_TRIANGLES_KHR == (uint32_t)GeometryType::TRIANGLES, "Enum mismatch");
    static_assert(VK_GEOMETRY_TYPE_AABBS_KHR == (uint32_t)GeometryType::AABBS, "Enum mismatch");

    return (VkGeometryTypeKHR)geometryType;
}

constexpr VkCopyAccelerationStructureModeKHR GetCopyModeVK(CopyMode copyMode) {
    static_assert(VK_COPY_ACCELERATION_STRUCTURE_MODE_CLONE_KHR == (uint32_t)CopyMode::CLONE, "Enum mismatch");
    static_assert(VK_COPY_ACCELERATION_STRUCTURE_MODE_COMPACT_KHR == (uint32_t)CopyMode::COMPACT, "Enum mismatch");

    return (VkCopyAccelerationStructureModeKHR)copyMode;
}

inline VkFormat GetVkFormat(Format format, bool demoteSrgb = false) {
    if (demoteSrgb) {
        const FormatProps& formatProps = GetFormatProps(format);
        if (formatProps.isSrgb)
            format = (Format)((uint32_t)format - 1);
    }

    return (VkFormat)NRIFormatToVKFormat(format);
}

inline VkExtent2D GetShadingRateVK(ShadingRate shadingRate) {
    switch (shadingRate) {
        case ShadingRate::FRAGMENT_SIZE_1X1:
            return {1, 1};
        case ShadingRate::FRAGMENT_SIZE_1X2:
            return {1, 2};
        case ShadingRate::FRAGMENT_SIZE_2X1:
            return {2, 1};
        case ShadingRate::FRAGMENT_SIZE_2X2:
            return {2, 2};
        case ShadingRate::FRAGMENT_SIZE_2X4:
            return {2, 4};
        case ShadingRate::FRAGMENT_SIZE_4X2:
            return {4, 2};
        case ShadingRate::FRAGMENT_SIZE_4X4:
            return {4, 4};
    }

    return {};
}

void ConvertGeometryObjectSizesVK(VkAccelerationStructureGeometryKHR* destObjects, uint32_t* primitiveNums, const GeometryObject* sourceObjects, uint32_t objectNum);
void ConvertGeometryObjectsVK(VkAccelerationStructureGeometryKHR* destObjects, VkAccelerationStructureBuildRangeInfoKHR* ranges, const GeometryObject* sourceObjects, uint32_t objectNum);

} // namespace nri