An android rtp player

RtpPlayer demostrates a simple rtp stream player on android.

Codecs
------

currently the player supports the following codecs:

- [x] vp8
- [x] h264(avc)
- [x] opus

Projects
--------

the solution incorporates 8 projects:

- [x] RtpPlayer    
        -- the rtp player
- [x] MediaCodec    
        -- A/V codec wrapper
- [x] libvpx    
        -- the vp8 codec
- [x] libx264   
        -- the h264 encoder
- [x] opus  
        -- the opus codec
- [x] ffmpeg    
        -- the h264 decoder
- [x] pro   
        -- close-sourced platform api wrapper(locks and threads etc.)
- [x] asio  
        -- the asio library(http://think-async.com/)

ScreenShots
-----------

<img src="https://cloud.githubusercontent.com/assets/8287989/21835786/ad5c3e02-d7fa-11e6-97f7-a120e0161a28.png" height="450px" >
