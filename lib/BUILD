cc_library(
    name = "freetype2",
    deps = [
        "@freetype2//:lib"
    ]
)

cc_library(
    name = "libs",
    srcs = glob(["**/*.cpp", "**/*.c"]),
    hdrs = glob(["**/*.hpp", "**/*.h"]),
    includes = ["./imgui"],
    visibility = ["//src:__pkg__"],
    deps = [
        ":freetype2"
    ]
)
