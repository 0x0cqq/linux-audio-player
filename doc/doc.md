# 基于 Linux ALSA 和 FFmpeg 的音乐解码播放器

## 简介

## Decoder 类

调用 ffmpeg 库，将不同格式的音频文件解码得到 pcm 数据

## Controller 类

负责音乐的播放控制：

* 切换音乐文件
* 快进，快退




## Player 类

调用 ALSA 库，将 Decoder 得到的 pcm 数据播放，并且根据 Controller 调整声音
