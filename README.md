# Simple C++ video player using FFmpeg and SDL 2.0 

1 - Install OpenCV<br />
        Tutorial for Ubuntu 16.04: https://www.learnopencv.com/install-opencv3-on-ubuntu/

2 - Install FFmpeg version >= 3.2.4<br />
        ```ruby
        sudo add-apt-repository ppa:jonathonf/ffmpeg-3
        sudo apt update && sudo apt install ffmpeg libav-tools x264 x265 libavdevice-dev
        ```

3 - Install SDL >= 2.0<br />
        ```ruby
        sudo apt-get install libsdl2-dev libsdl2-2.0
        ```

4 - Install:<br />
        ```ruby
        make
        ```

5 - Run<br />
    ```ruby
    ./player video_address
    ```