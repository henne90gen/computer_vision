#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <iostream>

/**
 * Task 1:
 *      Filter (b) is separable
 */

cv::Mat sobel(cv::Mat src) {
    cv::Mat verticalMask = cv::Mat::zeros(3, 1, CV_8SC1);
    cv::Mat horizontalMask = cv::Mat::zeros(1, 3, CV_8SC1);
    verticalMask.at<char>(0, 0) = 1;
    verticalMask.at<char>(1, 0) = 2;
    verticalMask.at<char>(2, 0) = 1;
    horizontalMask.at<char>(0, 0) = -1;
    horizontalMask.at<char>(1, 0) = 0;
    horizontalMask.at<char>(2, 0) = 1;
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
        }
    }
}

int main(int argc, char **argv) {
    const char *filename = "../left.jpg";
    cv::Mat image = cv::imread(filename, 1);

    imshow("Image", image);

    while (true) {
        if (cv::waitKey(0) == 27)
            break;
    }

    return 0;
}
