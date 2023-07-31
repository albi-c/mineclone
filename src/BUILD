cc_binary(
    name = "mineclone",
    srcs = ["main.cpp"],
    data = ["//res:resources"],
    linkopts = [
        "-lGL", "-pthread", "-lX11"
    ],
    deps = [
        ":game"
    ]
)

cc_library(
    name = "game",
    srcs = glob(["**/*.cpp"], exclude = ["main.cpp"]),
    hdrs = glob(["**/*.hpp", "**/*.inl"]),
    includes = ["../lib", "../lib/imgui", "../src"],
    copts = ["-I."],
    deps = [
        "//lib:libs",
        "//libs:static_libs"
    ]
)
