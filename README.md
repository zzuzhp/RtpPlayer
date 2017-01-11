a simple android rtp player

RtpPlayer demostrates a simple rtp stream player on android.

Codecs
------

currently the player supports the following codecs:

- [x] vp8
- [x] h264(avc)
- [x] opus

Projects
--------

the solution include 7 projects:

1. RtpPlayer
   this is an AndroidStudio based project, which implements a simple rtp player.
2. MediaCodec
   this is a A/V codec wrapper
3. libvpx
   this is the vp8 codec 
4. libx264
   this is the h264 encoder
5. opus
   this is the opus codec
6. ffmpeg
   this is the h264 decoder
7. pro
   this is the network framework(receives the rtp streams) as well as some platform api wrapper(locks and threads etc.).
   'pro' is not open-sourced, only bin files are available.

ScreenShots
-----------

<img src="https://cloud.githubusercontent.com/assets/8287989/21835786/ad5c3e02-d7fa-11e6-97f7-a120e0161a28.png" height="450px" >
