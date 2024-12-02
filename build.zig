const std = @import("std");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const option_static_library = b.option(bool, "static_library", "Build as a static library") orelse true;
    const option_enable_none_support = b.option(bool, "enable_none_support", "Enable NONE backend") orelse true;
    const option_enable_d3d11_support = b.option(bool, "enable_d3d11_support", "Enable D3D11 backend") orelse true;
    const option_enable_d3d12_support = b.option(bool, "enable_d3d12_support", "Enable D3D12 backend") orelse true;
    const option_enable_vk_support = b.option(bool, "enable_vk_support", "Enable VULKAN backend") orelse true;

    const option_enable_xlib_support = b.option(
        bool,
        "enable_xlib_support",
        "vulkan: Enable 'xlib' support",
    ) orelse true;
    const option_enable_wayland_support = b.option(
        bool,
        "enable_wayland_support",
        "vulkan: Enable 'wayland' support",
    ) orelse true;

    // const option_enable_agility_sdk_support = b.option(
    //     bool,
    //     "enable_agility_sdk_support",
    //     "d3d12: Enable Agility SDK support to unlock access to recent D3D12 features",
    // ) orelse false;
    // const option_agility_sdk_path = b.option(
    //     std.Build.LazyPath,
    //     "agility_sdk_path",
    //     "d3d12: Path to a directory containing Agility SDK (contents of '.nupkg/build/native/')",
    // ) orelse null;
    // const option_agility_sdk_version = b.option(
    //     u32,
    //     "agility_sdk_version",
    //     "d3d12: Agility SDK version",
    // ) orelse 614;
    //NRI_AGILITY_SDK_DIR will be a zig temp dir

    // const option_enable_external_libraries = b.option(
    //     bool,
    //     "enable_external_libraries",
    //     "Enable vendor specific extension libraries (NVAPI and AMD AGS)",
    // ) orelse true;

    const header = b.build_root.handle.openFile("Include/NRI.h", .{}) catch unreachable;
    defer header.close();

    const header_contents = try header.readToEndAlloc(b.allocator, std.math.maxInt(u32));

    const version_major = blk: {
        const index_of_major = std.mem.indexOf(u8, header_contents, "NRI_VERSION_MAJOR") orelse unreachable;
        var version_major_it = std.mem.tokenizeAny(u8, header_contents[index_of_major..], " \n");
        // Skip "NRI_VERSION_MAJOR"
        _ = version_major_it.next();
        const tok = version_major_it.next().?;
        break :blk std.fmt.parseInt(u32, tok, 10) catch
            std.debug.panic("Failed to parse version major: '{s}'", .{tok});
    };

    const version_minor = blk: {
        const index_of_minor = std.mem.indexOf(u8, header_contents, "NRI_VERSION_MINOR") orelse unreachable;
        var version_minor_it = std.mem.tokenizeAny(u8, header_contents[index_of_minor..], " \n");
        // Skip "NRI_VERSION_MINOR"
        _ = version_minor_it.next();
        const tok = version_minor_it.next().?;
        break :blk std.fmt.parseInt(u32, tok, 10) catch
            std.debug.panic("Failed to parse version minor: '{s}'", .{tok});
    };

    std.debug.print("NRI version: {}.{}\n", .{ version_major, version_minor });

    var cflags = std.ArrayList([]const u8).init(b.allocator);
    try cflags.appendSlice(&.{"-Wno-everything"});
    var macros = std.ArrayList(struct { []const u8, []const u8 }).init(b.allocator);
    try macros.append(.{ "WIN32_LEAN_AND_MEAN", "" });
    try macros.append(.{ "NOMINMAX", "" });
    try macros.append(.{ "_CRT_SECURE_NO_WARNINGS", "" });

    const include = b.path("Include");
    const shared_include = b.path("Source/Shared");

    if (option_enable_none_support) {
        try macros.append(.{ "NRI_USE_NONE", "1" });
    }

    if (option_enable_d3d11_support) {
        try macros.append(.{ "NRI_USE_D3D11", "1" });
    }

    if (option_enable_d3d12_support) {
        try macros.append(.{ "NRI_USE_D3D12", "1" });
    }

    if (option_enable_vk_support) {
        try macros.append(.{ "NRI_USE_VK", "1" });

        switch (target.result.os.tag) {
            .windows => {
                try macros.append(.{ "VK_USE_PLATFORM_WIN32_KHR", "1" });
            },
            .macos, .ios => {
                try macros.append(.{ "VK_USE_PLATFORM_METAL_EXT", "1" });
                try macros.append(.{ "VK_ENABLE_BETA_EXTENSIONS", "1" });
            },
            .linux => {
                if (option_enable_xlib_support) {
                    try macros.append(.{ "VK_USE_PLATFORM_XLIB_KHR", "1" });
                }
                if (option_enable_wayland_support) {
                    try macros.append(.{ "VK_USE_PLATFORM_WAYLAND_KHR", "1" });
                }
            },
            else => {},
        }
    }

    if (!option_static_library) {
        switch (target.result.os.tag) {
            .windows => {
                try macros.append(.{ "NRI_API", "extern \"C\" __declspec(dllexport)" });
            },
            else => {
                try macros.append(.{ "NRI_API", "extern \"C\" __attribute__((visibility(\"default\")))" });
            },
        }
    }

    var cpp_flags = std.ArrayList([]const u8).init(b.allocator);
    try cpp_flags.appendSlice(&.{"-std=c++17"});
    try cpp_flags.appendSlice(cflags.items);

    const nri_shared = b.addStaticLibrary(.{
        .target = target,
        .optimize = optimize,
        .name = "NRI_Shared",
        .link_libc = true,
    });
    nri_shared.linkLibC();
    nri_shared.linkLibCpp();
    nri_shared.addCSourceFiles(.{
        .root = b.path("Source/Shared"),
        .flags = cpp_flags.items,
        .files = &.{
            "Shared.cpp",
        },
    });
    nri_shared.addIncludePath(include);
    nri_shared.addIncludePath(shared_include);
    addMacros(nri_shared, macros.items);

    const nri_validation = b.addStaticLibrary(.{
        .target = target,
        .optimize = optimize,
        .name = "NRI_Validation",
        .link_libc = true,
    });
    nri_validation.linkLibC();
    nri_validation.linkLibCpp();
    nri_validation.addCSourceFiles(.{
        .root = b.path("Source/Validation"),
        .flags = cpp_flags.items,
        .files = &.{
            "ImplVal.cpp",
        },
    });
    nri_validation.addIncludePath(include);
    nri_validation.addIncludePath(shared_include);
    addMacros(nri_validation, macros.items);

    const nri_none: ?*std.Build.Step.Compile = if (option_enable_none_support) none: {
        const nri_none = b.addStaticLibrary(.{
            .target = target,
            .optimize = optimize,
            .name = "NRI_NONE",
        });
        nri_none.linkLibC();
        nri_none.linkLibCpp();
        nri_none.addCSourceFiles(.{
            .root = b.path("Source/NONE"),
            .flags = cpp_flags.items,
            .files = &.{
                "implNONE.cpp",
            },
        });
        nri_none.addIncludePath(include);
        nri_none.addIncludePath(shared_include);
        addMacros(nri_none, macros.items);
        break :none nri_none;
    } else null;

    const nri_d3d11: ?*std.Build.Step.Compile = if (option_enable_d3d11_support) d3d11: {
        const nri_d3d11 = b.addStaticLibrary(.{
            .target = target,
            .optimize = optimize,
            .name = "NRI_D3D11",
        });
        nri_d3d11.linkLibC();
        nri_d3d11.linkLibCpp();
        nri_d3d11.addCSourceFiles(.{
            .root = b.path("Source/D3D11"),
            .flags = cpp_flags.items,
            .files = &.{
                "implD3D11.cpp",
            },
        });
        nri_d3d11.addIncludePath(include);
        nri_d3d11.addIncludePath(shared_include);
        nri_d3d11.linkSystemLibrary("dxgi");
        nri_d3d11.linkSystemLibrary("dxguid");
        //D3D11CreateDevice
        nri_d3d11.linkSystemLibrary("d3d11");
        nri_d3d11.linkLibrary(nri_shared);
        addMacros(nri_d3d11, macros.items);

        const direct3d_headers = b.lazyDependency("direct3d_headers", .{}) orelse return;
        nri_d3d11.addIncludePath(direct3d_headers.path("include"));

        break :d3d11 nri_d3d11;
    } else null;

    const nri_d3d12: ?*std.Build.Step.Compile = if (option_enable_d3d12_support) d3d12: {
        const nri_d3d12 = b.addStaticLibrary(.{
            .target = target,
            .optimize = optimize,
            .name = "NRI_D3D12",
        });
        nri_d3d12.linkLibC();
        nri_d3d12.linkLibCpp();
        nri_d3d12.addCSourceFiles(.{
            .root = b.path("Source/D3D12"),
            .flags = cpp_flags.items,
            .files = &.{
                "implD3D12.cpp",
            },
        });
        nri_d3d12.addCSourceFiles(.{
            .root = b.path("External/memalloc"),
            .flags = cpp_flags.items,
            .files = &.{
                "D3D12MemAlloc.cpp",
            },
        });
        nri_d3d12.addIncludePath(b.path("External"));
        nri_d3d12.addIncludePath(b.path("External/memalloc"));
        nri_d3d12.addIncludePath(include);
        nri_d3d12.addIncludePath(shared_include);
        nri_d3d12.linkSystemLibrary("dxgi");
        nri_d3d12.linkSystemLibrary("dxguid");
        //D3D12CreateDevice
        nri_d3d12.linkSystemLibrary("d3d12");
        addMacros(nri_d3d12, macros.items);

        nri_d3d12.linkLibrary(nri_shared);

        const direct3d_headers = b.lazyDependency("direct3d_headers", .{}) orelse return;
        nri_d3d12.addIncludePath(direct3d_headers.path("include"));

        break :d3d12 nri_d3d12;
    } else null;

    const nri_vk: ?*std.Build.Step.Compile = if (option_enable_vk_support) vk: {
        const nri_vk = b.addStaticLibrary(.{
            .target = target,
            .optimize = optimize,
            .name = "NRI_VK",
        });
        nri_vk.linkLibC();
        nri_vk.linkLibCpp();
        nri_vk.addCSourceFiles(.{
            .root = b.path("External/memalloc/"),
            .flags = cflags.items,
            .files = &.{
                "vk_mem_alloc.h",
            },
        });
        nri_vk.addCSourceFiles(.{
            .root = b.path("Source/VK"),
            .flags = cpp_flags.items,
            .files = &.{
                "implVK.cpp",
            },
        });
        nri_vk.addIncludePath(include);
        nri_vk.addIncludePath(shared_include);
        nri_vk.addIncludePath(b.path("External"));
        addMacros(nri_vk, macros.items);

        const vulkan_headers = b.lazyDependency("vulkan_headers", .{}) orelse return;
        nri_vk.addIncludePath(vulkan_headers.path("include"));

        nri_vk.linkLibrary(nri_shared);

        switch (target.result.os.tag) {
            .linux => {
                if (option_enable_xlib_support) {
                    const x11_headers = b.lazyDependency("x11_headers", .{}) orelse return;
                    nri_vk.addIncludePath(x11_headers.path("include"));
                }
                if (option_enable_wayland_support) {
                    const wayland_headers = b.lazyDependency("wayland_headers", .{}) orelse return;
                    nri_vk.addIncludePath(wayland_headers.path("include"));
                }
            },
            else => {},
        }

        break :vk nri_vk;
    } else null;

    const nri = if (option_static_library) b.addStaticLibrary(.{
        .target = target,
        .optimize = optimize,
        .name = "NRI",
        .link_libc = true,
    }) else b.addSharedLibrary(.{
        .target = target,
        .optimize = optimize,
        .name = "NRI",
        .link_libc = true,
    });
    nri.addCSourceFile(.{
        .file = b.path("Source/Creation/Creation.cpp"),
        .flags = cpp_flags.items,
    });
    addMacros(nri, macros.items);
    nri.linkLibrary(nri_shared);
    nri.linkLibrary(nri_validation);
    nri.addIncludePath(include);
    nri.addIncludePath(shared_include);
    if (nri_none) |none| {
        nri.linkLibrary(none);
    }
    if (nri_d3d11) |d3d11| {
        nri.linkLibrary(d3d11);
    }
    if (nri_d3d12) |d3d12| {
        nri.linkLibrary(d3d12);
    }
    if (nri_vk) |vk| {
        nri.linkLibrary(vk);
    }

    const root = b.addModule("root", .{
        .root_source_file = b.path("Source/nri.zig"),
        .target = target,
        .optimize = optimize,
    });
    root.linkLibrary(nri);
}

inline fn addMacros(compile: *std.Build.Step.Compile, macros: []const struct { []const u8, []const u8 }) void {
    for (macros) |macro| {
        compile.root_module.addCMacro(macro.@"0", macro.@"1");
    }
}

// set_property (TARGET ${PROJECT_NAME} PROPERTY FOLDER ${PROJECT_FOLDER})

// set_target_properties (${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
// message ("NRI output path: '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")

// if (WIN32)
//     if (NRI_ENABLE_D3D11_SUPPORT OR NRI_ENABLE_D3D12_SUPPORT)
//         # Function - copy a library to the output folder of a project
//         function (copy_library PROJECT LIBRARY_NAME)
//             add_custom_command (TARGET ${PROJECT} POST_BUILD
//                 COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LIBRARY_NAME} $<TARGET_FILE_DIR:${PROJECT}>
//                 COMMAND_EXPAND_LISTS)
//         endfunction ()

//         # Copy AMD AGS into the output folder
//         find_file (AMD_AGS_DLL NAMES amd_ags_x64.dll PATHS "External/amdags/ags_lib/lib")
//         copy_library (${PROJECT_NAME} ${AMD_AGS_DLL})
//     endif ()
// endif ()
