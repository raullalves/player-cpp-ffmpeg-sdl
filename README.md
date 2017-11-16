# Simple C++ video player using FFmpeg 3 and SDL 2.0 

Supports video with audio.

1 - Install FFmpeg version >= 3.2.4<br />
        ```
        sudo add-apt-repository ppa:jonathonf/ffmpeg-3
        ```
        <br />
        ```
        sudo apt update && sudo apt install ffmpeg libav-tools x264 x265 libavdevice-dev
        ```

2 - Install SDL >= 2.0<br />
        ```
        sudo apt-get install libsdl2-dev libsdl2-2.0
        ```

3 - Install:<br />
        ```
        make
        ```

4 - Run<br />
    ```
    ./player video_address
    ```