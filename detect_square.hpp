/**
* The "Square Detector" program.
* It loads several images sequentially and tries to find squares in each image
* Zdroj: http://stackoverflow.com/questions/7731742/square-detection-doesnt-find-squares
*/

// Standardne hlavickove subory
#include <iostream>
#include <math.h>
#include <string.h>

// OpenCV hlavickove subory
#include <opencv/cv.h>
#include <opencv/highgui.h>

double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0 );

void squareDetectObjects(const cv::Mat& image, std::vector<std::vector<cv::Point>>& squares);

void squareDrawObjects(cv::Mat& image, const std::vector<std::vector<cv::Point>>& squares);
