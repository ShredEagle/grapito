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
        ("glad/0.1.34"),

        ("aunteater/7702f73a38@adnn/develop"),
        ("graphics/4c1db118e3@adnn/develop"),
        ("math/be21b787d3@adnn/develop"),
        ("websocket/717708d463@adnn/develop"),
    )

    build_requires = ("cmake/3.20.4")

    generators = "cmake_paths", "cmake_find_package", "CMakeToolchain"
    build_policy = "missing"

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
