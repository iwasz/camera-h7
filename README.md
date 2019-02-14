Useful tips
===========
* How to convert RGB565 data to a PNG:


	ffmpeg -y -vcodec rawvideo -f rawvideo -pix_fmt rgb565 -s 320x200 -i data.dat -f image2 -vcodec png data1.png

