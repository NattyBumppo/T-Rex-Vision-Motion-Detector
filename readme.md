# Summary

This is a simple application that takes a video stream (e.g., from a webcam) as input and outputs any pixels that have changed significantly over the past several frames, resulting in a sort of "T-Rex vision"-style motion detector (as inspired by Jurassic Park).

Image data is stored in a ring buffer, and when the standard deviation for historical intensity values for a pixel exceeds the draw threshold, that pixel is assumed to have changed significantly and is drawn. The ring buffer size and draw threshold are adjustable.

This project was written in C++, using the [OpenCV](http://opencv.willowgarage.com/wiki/) library. This project requires the OpenCV library to run.

# Credits

This application was programmed by [Nat Guy](http://www.natguy.net).