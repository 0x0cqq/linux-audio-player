## 交叉编译 ffmpeg

### Host 上操作

1. 下载源代码：`git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg`  

2. 进入源代码文件夹：`cd ffmpeg`，文件形如：

    ```
    .
    ├── CONTRIBUTING.md
    ├── COPYING.GPLv2
    ├── COPYING.GPLv3
    ├── COPYING.LGPLv2.1
    ├── COPYING.LGPLv3
    ├── CREDITS
    ├── Changelog
    ├── INSTALL.md
    ├── LICENSE.md
    ├── MAINTAINERS
    ├── Makefile
    ├── README.md
    ├── RELEASE
    ├── compat
    ├── configure
    ├── doc
    ├── ffbuild
    ├── fftools
    ├── libavcodec
    ├── libavdevice
    ├── libavfilter
    ├── libavformat
    ├── libavutil
    ├── libpostproc
    ├── libswresample
    ├── libswscale
    ├── presets
    ├── tests
    └── tools

    15 directories, 14 files
    ```

3. 切换代码到某个 release 版本：`git checkout n6.0`

4. 配置编译环境: `source /opt/st/myir/3.1-snapshot/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi` 

5. 观察一下 C 的编译命令：`echo $CC`，结果形如：

    ```
    bkrc@ubuntu:~$ echo $CC
    arm-ostl-linux-gnueabi-gcc -mthumb -mfpu=neon-vfpv4 -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi
    ```

    这里有很多编译选项，之后需要搬到 `configure` 文件的配置中

6. 进行 configure:
    
    先设置环境变量：
    
    ```bash
    export SYSROOT="/opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi" 
    ```
    
    构建：
    
    ```bash
    ./configure --enable-cross-compile --cross-prefix=arm-ostl-linux-gnueabi- --arch=armv7a --sysroot=$SYSROOT --cpu=cortex-a7 --enable-thumb --target-os=linux --extra-cflags="-mfpu=neon-vfpv4 -mfloat-abi=hard" --prefix=$SYSROOT/opt/ffmpeg
    ```
    * `--enable-cross-compile`: 启用交叉编译
    * `--cross-prefix=arm-ostl-linux-gnueabi-`：编译器的前缀
    * `--arch=armv7a`：CPU 指令集
    * `--sysroot=$SYSROOT`: SYSROOT 设置好 target 的根文件夹在 host 上的位置
    * `--cpu=cortex-a7`: CPU 架构
    * `--enable-thumb` 启用 thumb，上面写了抄下来
    * `--target-os=linux`：target 的操作系统
    * `--extra-cflags="-mfpu=neon-vfpv4 -mfloat-abi=hard"`：其他的 C 编译选项，抄下来
    * `--prefix=$SYSROOT/opt/ffmpeg`: 安装目标位置（但这么写会出问题，因为可用存储太小了会出问题，可以改成 `/mnt/usb/ffmpeg` 之类的）

    * 注：理论上这里应该 disable 一些无用的属性来减少产生的库文件的大小，否则放入嵌入式系统内可能相当困难，最终的库文件大概有一百到二百兆。具体可以观察 `./configure --help` 打印出的说明。（但事实上，加钱插个 U 盘就行）

7. 构建：`make`，需要很长的时间

8. 安装：`make install`
    * 如果安装到有保护的位置（例如 `/opt`, `/usr/local`），需要先切换到 root 用户：`sudo su`
    * 然后再次加载编译环境：`source /opt/st/myir/3.1-snapshot/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi`
    * 然后再 `make install`

9. 安装完成，可以在 `/opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi/mnt/usb` 下找到文件夹名为 `ffmpeg`

### Target 上操作

1. 打开嵌入式系统，连接好网络

2. 插上一个 U 盘， U 盘文件格式为 FAT32

3. `fdisk` 查看 U 盘名称，一般是 `/dev/sda1` 之类，通过大小和类型确认。

4. 挂载：`mount -t vfat /dev/sda1 /mnt/usb`

5. 将编译好的文件，即 Host 机 `/opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi/mnt/usb/ffmpeg` 下的内容拷贝到 Target 机 `/mnt/usb` 下：
    ```bash
    scp -r /opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi/mnt/usb/ffmpeg root@xxx.xxx.xxx.xxx:/mnt/usb/ffmpeg
    ```

6. 运行安装好的 ffmpeg：`/mnt/usb/ffmpeg/bin/ffmpeg`

    ```
    root@myir:/mnt/usb/ffmpeg# /mnt/usb/ffmpeg/bin/ffmpeg
    ffmpeg version N-110790-ge8e4863325 Copyright (c) 2000-2023 the FFmpeg developers
    built with gcc 9.3.0 (GCC)
    configuration: --enable-cross-compile --cross-prefix=arm-ostl-linux-gnueabi- --arch=armv7a --sysroot=/opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi --cpu=cortex-a7 --enable-thumb --target-os=linux --extra-cflags='-mfpu=neon-vfpv4 -mfloat-abi=hard' --prefix=/opt/st/myir/3.1-snapshot/sysroots/cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi/opt/ffmpeg
    libavutil      58.  9.100 / 58.  9.100
    libavcodec     60. 14.101 / 60. 14.101
    libavformat    60.  5.100 / 60.  5.100
    libavdevice    60.  2.100 / 60.  2.100
    libavfilter     9.  8.101 /  9.  8.101
    libswscale      7.  2.100 /  7.  2.100
    libswresample   4. 11.100 /  4. 11.100
    Hyper fast Audio and Video encoder
    usage: ffmpeg [options] [[infile options] -i infile]... {[outfile options] outfile}...

    Use -h to get full help or, even better, run 'man ffmpeg'
    ```

    header 文件，library 文件都在 `/mnt/usb/ffmpeg` 的对应文件夹内。只需在编译时通过编译选项，提醒编译器搜索该处的路径即可。