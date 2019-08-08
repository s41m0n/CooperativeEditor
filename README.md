# CooperativeEditor
* *System and Device Programming project* * - Cooperative multiuser editor.

The aim of this project is to implement, using client-server architecture, a cooperative editor thanks to many user can work on the same file.

## Installation

If you do not want to manually download all dependencies, you can download `conan` which is a very powerful Dependency Manager Python module.

Once installed, create a default conan profile under `~/.conan/profiles/default`:

`╰─ conan profile new default --detect`

Remind to change the value `compiler.libcxx` and `compiler.version` according to your current version. The final configuration should be similar to this one:

```bash
~/.conan/profile/default
------------------------------------------------------------------------------------------------------------------------
[settings]
 os=Linux
 os_build=Linux
 arch=x86_64
 arch_build=x86_64
 compiler=gcc
 compiler.version=9.1
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

Finally, if the command successfully creates the file `conanbuildinfo.cmake` you can proceed compiling the project using CMake.

## Authors

**Simone Magnani** a.k.a. **s41m0n** [Linkedin](1)
**Riccardo Marchi** a.k.a. **richiMarchi** [Linkedin](https://www.linkedin.com/in/riccardo-marchi4/)

## License

This project is licensed under the **Polytechnic University of Turin**
