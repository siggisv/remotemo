from conan import ConanFile

class Remotemo(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("sdl/[~2.26]")
        self.requires("sdl_image/[~2]")
        self.requires("libwebp/[~1.3]", override=True)
