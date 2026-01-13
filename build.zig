const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const os_tag = target.result.os.tag;
    const arch = target.result.cpu.arch;
    const abi = target.result.abi;

    // Platform and arch names for output files
    const platform_name: []const u8 = if (abi != .none) b.fmt("{s}-{s}", .{ @tagName(os_tag), @tagName(abi) }) else @tagName(os_tag);
    const arch_name: []const u8 = @tagName(arch);

    const name = b.fmt("exhelper_{s}_{s}", .{ platform_name, arch_name });
    const lib_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .pic = true,
        .omit_frame_pointer = false,
        .unwind_tables = .sync,
        .link_libcpp = true,
    });
    // Build shared library
    const lib = b.addLibrary(.{
        .name = name,
        .linkage = .dynamic,
        .root_module = lib_module,
    });
    lib.bundle_ubsan_rt = false;
    lib.bundle_compiler_rt = false;
    lib.link_gc_sections = true;
    lib.discard_local_symbols = true;

    lib_module.addCSourceFile(.{
        .file = b.path("src/main.c"),
        .flags = &.{"-fno-sanitize=undefined"},
    });
    lib_module.addAssemblyFile(b.path("src/main.S"));
    lib_module.addIncludePath(b.path("src/include"));
    b.installArtifact(lib);

    // Test executable
    const exe_module = b.createModule(.{
        .target = target,
        .optimize = optimize,
        .link_libcpp = true,
    });
    const exe = b.addExecutable(.{
        .name = name,
        .root_module = exe_module,
    });

    exe_module.addCSourceFile(.{
        .file = b.path("src/test.cpp"),
        .flags = &.{ "-fexceptions", "-std=c++17" },
    });
    exe_module.addAssemblyFile(b.path("src/test.S"));
    exe_module.addIncludePath(b.path("src/include"));

    // Link library by path instead of linkLibrary to avoid dependency graph issues
    exe_module.addLibraryPath(b.path("zig-out/lib"));
    exe_module.linkSystemLibrary(name, .{});
    exe.step.dependOn(&lib.step);

    // Run step
    const run_cmd = b.addRunArtifact(exe);
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Build and run the test executable");
    run_step.dependOn(&run_cmd.step);

    // Test step - verify exit code 0 and "OK" in output
    const test_cmd = b.addRunArtifact(exe);
    test_cmd.expectExitCode(0);
    test_cmd.addCheck(.{ .expect_stdout_match = "OK" });

    const test_step = b.step("test", "Run test");
    test_step.dependOn(&test_cmd.step);
}
