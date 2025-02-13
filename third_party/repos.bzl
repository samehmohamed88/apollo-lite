load("//third_party/cuda:workspace.bzl", cuda = "repo")
load("//third_party/eigen3:workspace.bzl", eigen3 = "repo")
load("//third_party/geographic_info:workspace.bzl", geographic_info = "repo")
load("//third_party/geographiclib:workspace.bzl", geographiclib = "repo")
load("//third_party/lz4:workspace.bzl", lz4 = "repo")
load("//third_party/mcap:workspace.bzl", mcap = "repo")
load("//third_party/opencv:workspace.bzl", opencv = "repo")
load("//third_party/ros2/robot_localization:workspace.bzl", robot_localization = "repo")
load("//third_party/ros2/ros2_angles:workspace.bzl", ros2_angles = "repo")
load("//third_party/ros2/rules_ros2:workspace.bzl", rules_ros2 = "repo")
load("//third_party/ros2/zed_ros2/zed_ros2_interfaces:workspace.bzl", zed_ros2_interfaces = "repo")
load("//third_party/ros2/zed_ros2/zed_ros2_wrapper:workspace.bzl", zed_ros2_wrapper = "repo")
load("//third_party/sdl2:workspace.bzl", sdl2 = "repo")
load("//third_party/yaml-cpp:workspace.bzl", yaml_cpp = "repo")
load("//third_party/zed:workspace.bzl", zed = "repo")
load("//third_party/zstd:workspace.bzl", zstd = "repo")

#load("//third_party/carla:workspace.bzl", carla = "reo")
#load("//third_party/libtorch:workspace.bzl", libtorch = "repo")
#load("//third_party/cudnn:workspace.bzl", cudnn8 = "repo")
#load("//third_party/tensorrt:workspace.bzl", tensorrt = "repo")
#load("//third_party/rules_ros2:workspace.bzl", rules_ros2 = "repo")

#load("//third_party/ros2_vision_opencv:workspace.bzl", ros2_vision_opencv = "repo")

#load("//third_party/sfml:workspace.bzl", sfml = "repo")

def initialize_third_party():
    opencv()
    eigen3()
    yaml_cpp()
    zed()

    #    libtorch()
    #    cudnn8()
    #    tensorrt()
    #    yaml_cpp()
    #    geographic_info()

    #    zed_ros2_interfaces()
    #    robot_localization()

    #    zed()
    #
    #    ros2_vision_opencv()
    #    geographiclib()

    #    carla()
    #    sfml()
