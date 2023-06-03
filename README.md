# Linux Audio Player

Linux audio player backend with ALSA library and FFmpeg library.

## Dependency

* GNU GCC/G++
* CMake
* ALSA library
* FFmpeg library(`libavfilter, libavcodec, ...`)

Install dependencies by:

```bash
sudo apt update
sudo apt install gcc g++ cmake
sudo apt install libasound2-dev libavfilter-dev libavcodec-dev libavformat-dev libswresample-dev libavutil-dev
```

Other dependencies 

```bash
sudo apt install alsa-utils ffmpeg
```

Recommended `.vscode/c_cpp_properties.json`:

```json
{
    "configurations": [
        {
            "compilerArgs": [
                "-O3",
                "-Werror"
            ],
            "intelliSenseMode": "linux-gcc-x64",
            "compilerPath": "/usr/bin/gcc",
            "includePath": [
                "${workspaceRoot}/include/**"
            ],
            "cStandard": "c17",
            "cppStandard": "c++11",
            "name": "Linux"
        }
    ],
    "version": 4
}
```

## Build

In project root directory, `bash build.sh`.

> When cross compiling, replace `cmake .. ` with `cmake .. -Dcross=true`.

### Rebuild

Simply remove `build` directory by `rm -rf build`.