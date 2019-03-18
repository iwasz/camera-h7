Useful tips
===========
* How to convert RGB565 data to a PNG (first command proved to be correct after
countless hours of debugging):


	ffmpeg -y -vcodec rawvideo -f rawvideo -pix_fmt bgr565be -s 320x200 -i data.dat -f image2 -vcodec png data5.png 
	ffmpeg -y -vcodec rawvideo -f rawvideo -pix_fmt rgb565 -s 320x200 -i data.dat -f image2 -vcodec png data1.png

