# Summary

This is an application that takes an input image and identifies human faces within the image. The application relies on a training dataset of faces and non-faces, and uses this data to compute Haar-like features as described by Viola and Jones in [this paper](http://research.microsoft.com/en-us/um/people/viola/Pubs/Detect/violaJones_CVPR2001.pdf). After these features have been computed, the application uses AdaBoost to compute a classifier which is then applied to the test image to detect faces.

This project was written in C++, using the [Qt](http://qt.nokia.com/products/) framework. [Larry Zitnick](http://research.microsoft.com/en-us/people/larryz/) at Microsoft Research wrote the majority of the UI code and the high-level architecture, and I wrote the majority of the machine learning and detection code.

# Credits

This application was programmed by [Nat Guy](http://www.natguy.net).