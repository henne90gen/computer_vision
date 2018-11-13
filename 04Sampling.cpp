#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <iostream>

/**
 * 1)
 *      Filter (b) and (c) are separable
 *
 * 2)
 *      This optimization is specific to the given sobel filter
 *      |a b c|
 *      |d e f|
 *      |g h i|
 *
 *      (c + f + f + i) - (a + d + d + g)
 *
 * 3.1)
 *      sinc cuts out higher frequencies
 *      filtering with sinc filter causes sine wave patterns to appear
 *      disadvantage: dropoff is much slower than with a Gaussian filter
 *      a) discrete values from the sinc-function
 *      b) sampling the sinc-function on a floating point grid (between points)
 *
 * 3.4)
 *      remove all frequencies that are higher than we can sample
 *      a) sinc-function
 *      b) Gaussian approximation of sinc-function
 *
 * 3.5)
 *      downsampling -> fewer number of pixels, fewer samples
 */

cv::Mat sobel(cv::Mat src) {
    cv::Mat verticalMask = cv::Mat::zeros(3, 1, CV_8SC1);
    cv::Mat horizontalMask = cv::Mat::zeros(1, 3, CV_8SC1);
    verticalMask.at<char>(0, 0) = 1;
    verticalMask.at<char>(1, 0) = 2;
    verticalMask.at<char>(2, 0) = 1;
    horizontalMask.at<char>(0, 0) = -1;
    horizontalMask.at<char>(0, 1) = 0;
    horizontalMask.at<char>(0, 2) = 1;
    cv::Mat dst = src.clone();
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            // multiplication is the whole point of this filter!
            // How should it be possible to apply the filter without multiplying?
            // dst.at<char>(y, x) = src.at<char>()
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
