# CooperativeEditor
System and Device programming project - Cooperative multiuser editor.

The aim of this project is to implement, using client-server architecture, a cooperative editor thanks to many user can work on the same file.

## Installation

If you do not want to manually download all dependencies, you can download `conan` which is a very powerful Dependency Manager Python module.

Once installed, create a default conan profile:

`╰─ conan config`

Remind to change the value `compiler.libcxx=libstdc` to:

```bash
~/.conan/profile/default
------------------------------------------------------------------------------------------------------------------------
[settings]
 os=Linux
 os_build=Linux
 arch=x86_64
 arch_build=x86_64
 compiler=gcc
 compiler.version=8
 compiler.libcxx=libstdc++11
 build_type=Release
 [options]
 [build_requires]
 [env]
```

Then add the `bincraft` repository to your conan config:

`╰─ conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan`

Before compiling the project with CMake, install all the dependencied by typing the following command:

`╰─ conan install . -s build_type=Debug --install-folder=cmake-build-debug --build missing`

## Author

**Simone Magnani** * *System and Device Programming project* * [Linkedin](https://www.linkedin.com/in/simone-magnani-564830151/)

## License

This project is licensed under the **Polytechnic University of Turin**
