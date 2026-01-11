const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const os_tag = target.result.os.tag;
    const arch = target.result.cpu.arch;

    const is_macos = os_tag == .macos;
    const is_linux = os_tag == .linux;

    // Platform and arch names for output files
    const platform_name: []const u8 = if (is_macos) "macos" else if (is_linux) "linux" else "unknown";
    const arch_name: []const u8 = switch (arch) {
        .x86_64 => "x86_64",
        .aarch64 => "arm64",
        else => "unknown",
    };

    const lib_name = b.fmt("exhelper_{s}_{s}", .{ platform_name, arch_name });

    // Build shared library
    const lib = b.addLibrary(.{
        .name = lib_name,
        .linkage = .dynamic,
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .pic = true,
            .omit_frame_pointer = false,
            .unwind_tables = .sync,
        }),
    });
    lib.bundle_ubsan_rt = false;
    lib.bundle_compiler_rt = false;
    lib.link_gc_sections = true;

    lib.addCSourceFile(.{
        .file = b.path("src/main.c"),
        .flags = &.{ "-fvisibility=hidden", "-fno-sanitize=undefined" },
    });
    lib.addAssemblyFile(b.path("src/main.S"));
    lib.addIncludePath(b.path("src/include"));
    lib.linkLibCpp();
    b.installArtifact(lib);

    // Test executable
    const exe = b.addExecutable(.{
        .name = b.fmt("exhelper_test_{s}_{s}", .{ platform_name, arch_name }),
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
        }),
    });

    exe.linkLibCpp();
    exe.addCSourceFile(.{
        .file = b.path("src/test.cpp"),
        .flags = &.{ "-fexceptions", "-std=c++17" },
    });
    exe.addAssemblyFile(b.path("src/test.S"));
    exe.addIncludePath(b.path("src/include"));

    // Link library by path instead of linkLibrary to avoid dependency graph issues
    exe.addLibraryPath(b.path("zig-out/lib"));
    exe.linkSystemLibrary(lib_name);
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
