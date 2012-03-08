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

// Ostatne hlavickove subory
#include "detect_square.hpp"

int thresh = 50, N = 2; // karlphillip: decreased N to 2, was 11.
const char* wndname = "Square Detection Demo";

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0 )
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// returns sequence of squares detected on the image.
// the sequence is stored in the specified memory storage
void squareDetectObjects(const cv::Mat& image, std::vector<std::vector<cv::Point>>& squares)
{
    squares.clear();

    cv::Mat pyr, gray0(image.size(), CV_8U), gray; // inicializacia premennych
    //cv::Mat image2 = image.clone(); // naklonovanie vstupneho obrazu
    //CvMat imageCvMat = image2; // konverzia z cv::Mat do CvMat
    CvMat imageCvMat = image;
    IplConvKernel *se21 = cvCreateStructuringElementEx(3, 3, 0, 0, CV_SHAPE_RECT, NULL);
    cvDilate(&imageCvMat, &imageCvMat, se21, 1);
    cvReleaseStructuringElement(&se21);
    
    // karlphillip: dilate the image so this technique can detect the white square,
    cv::Mat out(&imageCvMat);
    //Mat out(image);
    //dilate(out, out, Mat(), Point(-1,-1)); // original, ale nefunguje - preto som presiel na cvDilate
    // then blur it so that the ocean/sea become one big segment to avoid detecting them as 2 big squares.
    cv::medianBlur(out, out, 7);

    // down-scale and upscale the image to filter out the noise
    cv::pyrDown(out, pyr, cv::Size(out.cols/2, out.rows/2));
    cv::pyrUp(pyr, out, out.size());
    std::vector<std::vector<cv::Point>> contours;

    // find squares in every color plane of the image
    for(int c = 0; c < 3; c++)
    {
        int ch[] = {c, 0};
        mixChannels(&out, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for(int l = 0; l < N; l++)
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if(l == 0)
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cv::Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                cv::dilate(gray, gray, cv::Mat(), cv::Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                //     tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            std::vector<cv::Point> approx;

            // test each contour
            for(size_t i = 0; i < contours.size(); i++)
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if ((approx.size() == 4) && (fabs(cv::contourArea(cv::Mat(approx))) > 1000) && (cv::isContourConvex(cv::Mat(approx))))
                {
                    double maxCosine = 0;

                    for(int j = 2; j < 5; j++)
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if(maxCosine < 0.3) { squares.push_back(approx); }
                }
            }
        }
    }
}


// the function draws all the squares in the image
void squareDrawObjects(cv::Mat& image, const std::vector<std::vector<cv::Point>>& squares)
{
    for(size_t i = 0; i < squares.size(); i++)
    {
        const cv::Point* p = &squares[i][0];
        int n = (int)squares[i].size();
        cv::polylines(image, &p, &n, 1, true, cv::Scalar(0,255,0), 3, CV_AA);
    }

    cv::imshow(wndname, image);
}

