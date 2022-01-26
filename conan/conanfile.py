from conans import ConanFile, tools
from conan.tools.cmake import CMake

from os import path


class GrapitoConan(ConanFile):
    name = "grapito"
    license = "MIT"
    author = "adnn"
    url = "https://github.com/ShredEagle/grapito"
    description = "Shoot them urp!"
    topics = ("opengl", "2D", "game")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "build_tests": [True, False],
    }
    default_options = {
        "shared": False,
        "build_tests": False,
    }

    requires = (
        ("boost/1.77.0"),
        ("glad/0.1.34"),
        ("imgui/1.84.2"),
        ("spdlog/1.9.2"),
        ("openal/1.21.1"),
        ("vorbis/1.3.7"),

        ("aunteater/9cd7bd9340@adnn/develop"),
        ("graphics/7d5b1c3cea@adnn/develop"),
        ("math/017e7c42bf@adnn/develop"),
        ("websocket/ef5d5bf4d9@adnn/develop"),
        ("sounds/c80889af36@adnn/develop"),
    )


    generators = "cmake_paths", "cmake_find_package_multi", "CMakeToolchain"
    build_policy = "missing"
    # Otherwise, conan removes the imported imgui backends after build()
    # they are still required for the CMake config phase of package()
    keep_imports = True

    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
        "submodule": "recursive",
    }


    def configure(self):
        tools.check_min_cppstd(self, "17")


    def _generate_cmake_configfile(self):
        """ Generates a conanuser_config.cmake file which includes the file generated by """
        """ cmake_paths generator, and forward the remaining options to CMake. """
        with open("conanuser_config.cmake", "w") as config:
            config.write("message(STATUS \"Including user generated conan config.\")\n")
            # avoid path.join, on Windows it outputs '\', which is a string escape sequence.
            config.write("include(\"{}\")\n".format("${CMAKE_CURRENT_LIST_DIR}/conan_paths.cmake"))
            config.write("set({} {})\n".format("BUILD_tests", self.options.build_tests))


    def generate(self):
        self._generate_cmake_configfile()


    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.configure()
        return cmake


    def build(self):
        cmake = self._configure_cmake()
        cmake.build()


    def package(self):
        cmake = self._configure_cmake()
        cmake.install()


    def imports(self):
        # see: https://blog.conan.io/2019/06/26/An-introduction-to-the-Dear-ImGui-library.html
        # the imgui package is designed this way: consumer has to import desired backends.
        self.copy("imgui_impl_glfw.cpp",         src="./res/bindings", dst="imgui_backends")
        self.copy("imgui_impl_opengl3.cpp",      src="./res/bindings", dst="imgui_backends")
        self.copy("imgui_impl_glfw.h",           src="./res/bindings", dst="imgui_backends")
        self.copy("imgui_impl_opengl3.h",        src="./res/bindings", dst="imgui_backends")
        self.copy("imgui_impl_opengl3_loader.h", src="./res/bindings", dst="imgui_backends")

