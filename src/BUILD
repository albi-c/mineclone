cc_binary(
    name = "mineclone",
    srcs = ["main.cpp"],
    deps = [
        ":game",
        "//lib:libs"
    ]
)

cc_library(
    name = "game",
    srcs = glob(["**/*.cpp"], exclude = ["main.cpp"]),
    hdrs = glob(["**/*.hpp"]),
    includes = ["../lib", "../lib/imgui", "../src"],
    copts = ["-I."]
)

# cc_binary(
#     name = "mineclone",
#     srcs = glob(["**/*.cpp"]),
#     includes = [".", "lib", "src", "lib/imgui"],
#     copts = ["-I.", "-Isrc", "-I./src"],
#     deps = [
#         "//lib:libs"
#     ]
# )
