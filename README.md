# Simple C++ video player using FFmpeg 3 and SDL 2.0 

Supports video with audio.
## Windows setup
1 - Install FFMpeg here https://ffmpeg.zeranoe.com/builds/win64/dev/ and add it to your path
2 - Install SDL here https://www.libsdl.org/download-2.0.php and add it to your path
3 - (Optional) Install g++ and make from MinGW https://sourceforge.net/projects/mingw/

1 - Install FFmpeg version >= 3.2.4
        <br />
        ```
        sudo apt update && sudo apt install ffmpeg x264 x265 libavdevice-dev
        ```

2 - Install SDL >= 2.0<br />
        ```
        sudo apt-get install libsdl2-dev
        ```

3 - Install:<br />
        ```
        make
        ```

4 - Run<br />
    ```
    ./player <video_address>
    ```