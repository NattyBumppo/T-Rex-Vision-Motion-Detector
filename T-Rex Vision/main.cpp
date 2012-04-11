/*
 * This is a simple application that takes a video stream (e.g., from a webcam) as input
 * and outputs any pixels that have changed significantly over the past several frames,
 * resulting in a sort of "T-Rex vision"-style motion detector (as inspired by Jurassic Park).
 *
 * Image data is stored in a ring buffer, and when the standard deviation for historical
 * intensity values for a pixel exceeds the draw threshold, that pixel is assumed to have
 * changed significantly and is drawn. The ring buffer size and draw threshold are adjustable.
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

int diffThresholdTrackbarPos;
int prevFramesToStoreTrackbarPos;
int diffThresh = 10;
int prevFramesToStore = 3;
Mat image, grayscale, blurGray, coloredOutput;
std::vector<Mat> ringBuffer;
int ringBufferOldest = 0;

string windowName = "T-Rex Vision | Press Q or Esc to quit";

string diffThresholdTrackbarName = "Diff. Threshold";
string prevFramesToStoreTrackbarName = "Frame Memory";


// Add blurred grayscale data buffer to the current ring buffer
void addToRingBuffer(Mat toAdd)
{
    // cout << "Adding to ring buffer..." << endl;
    
    // If ring buffer is empty, fill up with occurrences of current data buffer
    if (ringBuffer.empty())
    {
        // Note that prevFramesToStore indicates how many historical frames
        // are saved, so a prevFramesToStore of 0 would still indicate a ring buffer
        // size of 1
        for (int i = 0; i < prevFramesToStore + 1; i ++)
        {
            ringBuffer.push_back(toAdd.clone());
        }
        cout << "Ring buffer was empty; added " << prevFramesToStore << " duplicates." << endl;
        cout << "Ring buffer size is now: " << ringBuffer.size() << endl;
    }
    // Otherwise, remove the oldest data object, add the new one where it was,
    //  and shift around so that the previous second-oldest is now the oldest
    else
    {
        // cout << "Adding. Oldest element in ring buffer is #" << ringBufferOldest << endl;
        ringBuffer.at(ringBufferOldest) = toAdd.clone();
        ringBufferOldest++;
        if (ringBufferOldest >= ringBuffer.size())
        {
            ringBufferOldest = 0;
        }
        
        // cout << "Added! Oldest element in ring buffer is #" << ringBufferOldest << endl;
    }
}

// Empty out the ring buffer (used whenever the size of the buffer is changed)
void emptyRingBuffer()
{
    cout << "Emptying ring buffer. Current size is " << ringBuffer.size();
    // cout << "Clearing..." << endl;
    
    // Empty out ringBuffer
    ringBuffer.clear();
    ringBufferOldest = 0;
    
    // cout << "Cleared!" << endl;
    
}

// Draw pixels to the screen whose standard deviation exceeds the difference threshold
void renderRingBuffer()
{
    Mat drawMask = Mat::zeros(480, 640, DataType<unsigned char>::type);
    
    for (int row = 0; row < drawMask.rows; row++)
    {
        for (int col = 0; col < drawMask.cols; col++)
        {  
            // cout << "drawMask dimensions: " << drawMask.size().width << " x " << drawMask.size().height << endl;
            //  cout << "imageNum buffer dimensions: " << ringBuffer.at(imageNum).size().width << " x " << ringBuffer.at(imageNum).size().height << endl;
            // cout << "ImageNum:" << imageNum << "; row: " << row << "; col: " << col << endl;
            
            // Sum up pixel intensity values
            int sum = 0;
            for(int imageNum = 0; imageNum < ringBuffer.size(); imageNum++)
            {
                sum += (ringBuffer.at(imageNum)).at<unsigned char>(row, col);
            }
            
            // Calculate mean value for this pixel
            float mean = (float)sum / (float)ringBuffer.size();
            
            // Find the sum of the squared differences between
            // each pixel value and the mean
            float sumSquaredDifferences = 0;
            for(int imageNum = 0; imageNum < ringBuffer.size(); imageNum++)
            {
                sumSquaredDifferences += ((float)(ringBuffer.at(imageNum)).at<unsigned char>(row, col) - mean) * ((float)(ringBuffer.at(imageNum)).at<unsigned char>(row, col) - mean);
            }
            
            // Calculate standard deviation
            float stdDev = sqrt(sumSquaredDifferences/(ringBuffer.size()));
            
            // Hide pixel unless stdDev is over threshold
            if (stdDev > diffThresh)
            {
                // Let color show through
                (drawMask).at<unsigned char>(row, col) = 255;
            }
            else
            {
                // Draw darkness at this point
                (drawMask).at<unsigned char>(row, col) = 0;
            }                
        }
    }
    
    coloredOutput = Scalar::all(0);
    
    image.copyTo(coloredOutput, drawMask);
    
    imshow(windowName, coloredOutput);
    
}


// Process captured video data
bool process(VideoCapture& capture)
{
    
    int screenshotCount = 0;
    char screenshotFilename[200];
    
    // Loop until a valid image is obtained from the camera
    while(1)
    {
        // cout << "Obtaining image..." << endl;
        capture >> image;
        if (image.empty())
            continue;
        else
            break;
    }
    
    // imshow(windowName, image);
    
    // Create grayscale version of image for intensity comparisons
    cvtColor(image, grayscale, CV_BGR2GRAY);
    
    // Blur grayscale before intensity comparison
    blur(grayscale, blurGray, Size(2,2));
    
    // Add the image data to the current ring buffer
    addToRingBuffer(blurGray);
    
    // Draw the current ring buffer to the screen (only drawing changed pixels)
    renderRingBuffer();
    
    // Handle user input
    char key = (char) waitKey(5); //delay 15 ms
    switch (key)
    {
            // Quit program
        case 'q':
        case 'Q':
        case 27: // Esc key
            return true;
            // Save screenshot
        case ' ':
            sprintf(screenshotFilename, "%.3d.jpg", screenshotCount++);
            imwrite(screenshotFilename, image);
            cout << "Saved " << screenshotFilename << endl;
            break;
            // Decrease difference threshold
        case '1': 
            diffThresholdTrackbarPos = getTrackbarPos(diffThresholdTrackbarName, windowName);
            diffThresholdTrackbarPos--;
            if (diffThresholdTrackbarPos < 0)
            {
                diffThresholdTrackbarPos++;
            }
            setTrackbarPos(diffThresholdTrackbarName, windowName, diffThresholdTrackbarPos);
            diffThresh = diffThresholdTrackbarPos;
            break;
            // Increase difference threshold
        case '2':
            diffThresholdTrackbarPos = getTrackbarPos(diffThresholdTrackbarName, windowName);
            diffThresholdTrackbarPos++;
            if (diffThresholdTrackbarPos > 100)
            {
                diffThresholdTrackbarPos--;
            }
            setTrackbarPos(diffThresholdTrackbarName, windowName, diffThresholdTrackbarPos);
            diffThresh = diffThresholdTrackbarPos;
            break;
            // Decrease ring buffer size
        case '3': 
            prevFramesToStoreTrackbarPos = getTrackbarPos(prevFramesToStoreTrackbarName, windowName);
            prevFramesToStoreTrackbarPos--;
            if (prevFramesToStoreTrackbarPos < 0)
            {
                prevFramesToStoreTrackbarPos++;
            }
            setTrackbarPos(prevFramesToStoreTrackbarName, windowName, prevFramesToStoreTrackbarPos);
            prevFramesToStore = prevFramesToStoreTrackbarPos;
            emptyRingBuffer();
            break;
            // Increase ring buffer size
        case '4':
            prevFramesToStoreTrackbarPos = getTrackbarPos(prevFramesToStoreTrackbarName, windowName);
            prevFramesToStoreTrackbarPos++;
            if (prevFramesToStoreTrackbarPos > 100)
            {
                prevFramesToStoreTrackbarPos--;
            }
            setTrackbarPos(prevFramesToStoreTrackbarName, windowName, prevFramesToStoreTrackbarPos);
            prevFramesToStore = prevFramesToStoreTrackbarPos;
            emptyRingBuffer();
            break;       
        default:
            break;
    }
    return false;
}

// Callback for when the difference trackbar is changed
void onDiffThresholdTrackbar(int, void*)
{
    
    // cout << "onDiffThreshold called" << endl;
    
}

// Callback for when the image buffer size trackbar is changed
void onPrevFramesToStoreTrackbar(int, void*)
{
    emptyRingBuffer();
    cout << "onPrevFramesToStoreTrackbar called" << endl;
}

int main(int ac, char** av)
{
    /* Attempt to open video capture device 0
     * (this should be the default device number for the system
     * camera; e.g., the Macbook built-in camera)
     */
    VideoCapture capture(0); // open video capture device 0
    
    // If the device couldn't be opened, error out and close
    if (!capture.isOpened()) {
        cerr << "Failed to open video device!" << endl;
        return 1;
    }
    
    // Create application window
    namedWindow(windowName, CV_WINDOW_KEEPRATIO); //resizable window;    
    
    // Create trackbar to change difference threshold
    createTrackbar(diffThresholdTrackbarName, windowName, &diffThresh, 100, onDiffThresholdTrackbar);
    
    // Create trackbar to change image buffer size
    createTrackbar(prevFramesToStoreTrackbarName, windowName, &prevFramesToStore, 4, onPrevFramesToStoreTrackbar);
    
    cout << "Press the space bar to save the current frame shown." << endl;
    cout << "Press Q or Esc to quit." << endl;
    
    // Enter main input/output loop
    bool endProgram = false;
    while(!endProgram)
    {
        endProgram = process(capture);
    }
    
    return 0;
}