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

typedef uchar Pixel;

cv::Mat sobel(cv::Mat src) {
    /*
     * |a b c|
     * |d e f|
     * |g h i|
     *
     * (c + f + f + i) - (a + d + d + g)
     */
    cv::Mat dst;
    src.convertTo(dst, CV_32SC1);
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            int a = src.at<char>(y + 1, x - 1);
            int c = src.at<char>(y + 1, x + 1);
            int d = src.at<char>(y, x - 1);
            int f = src.at<char>(y, x + 1);
            int g = src.at<char>(y - 1, x - 1);
            int i = src.at<char>(y - 1, x + 1);
            dst.at<int>(y, x) = (c + f + f + i) - (a + d + d + g);
        }
    }
    dst = dst + 1020.0 / 2040.0 * 255.0;
    dst.convertTo(dst, CV_8UC1);
    return dst;
}

void showSobel() {
    const char *filename = "../keyb7.jpg";
    cv::Mat image = cv::imread(filename, 1);

    imshow("Image", image);

    cv::Mat greyImage;
    cv::cvtColor(image, greyImage, cv::COLOR_BGR2GRAY);

    greyImage = sobel(greyImage);
    imshow("Grey Image", greyImage);
}

void generateBar(cv::Mat &dst, int width, int height, int offsetX, int offsetY) {
    dst = cv::Mat::zeros(512, 512, CV_8UC1);
    // set background to value 50
    dst.forEach<Pixel>([](Pixel &pixel, const int *position) -> void { pixel = 50; });
    for (int y = offsetY; y < offsetY + height; y++) {
        for (int x = offsetX; x < offsetX + width; x++) {
            dst.at<uchar>(y, x) = 255;
        }
    }
}

void generateStripes(cv::Mat &dst, int stripeWidth) {
    dst = cv::Mat::zeros(512, 512, CV_8UC1);
    // set background to 50
    dst.forEach<Pixel>([](Pixel &pixel, const int *position) -> void { pixel = 50; });
    for (int y = 0; y < dst.rows; y++) {
        int counter = 0;
        for (int x = 0; x < dst.cols; x++) {
            if (counter % 2 == 0) {
                dst.at<uchar>(y, x) = 255;
            }
            if (x % stripeWidth == 0) {
                counter++;
            }
        }
    }
}

float sinc(float x, float y) {
    float pi_x = M_PI * x;
    float pi_y = M_PI * y;
    float left = sin(pi_x) / pi_x;
    float right = sin(pi_y) / pi_y;
    return left * right;
}

void applySincOnGrid(cv::Mat src, cv::Mat &dst) {
    dst = src.clone();
    int filterSize = 10;
    dst.forEach<Pixel>([dst, filterSize](Pixel &pixel, const int *position) -> void {
        int pixel_x = position[0];
        int pixel_y = position[1];
        float sum = (float)pixel;
        for (int y = pixel_y - filterSize; y < pixel_y + filterSize; y++) {
            if (y < 0 || y >= dst.rows) {
                continue;
            }
            for (int x = pixel_x - filterSize; x < pixel_x + filterSize; x++) {
                if (x < 0 || x >= dst.cols) {
                    continue;
                }
                float s = sinc(x - pixel_x, y - pixel_y);
                if (s != s) {
                    // s is NaN (not a number)
                    continue;
                }
                sum += s * (float)dst.at<uchar>(y, x);
            }
        }
        pixel = (uchar)sum;
    });
}

void applySincBetweenGrid(cv::Mat src, cv::Mat &dst) {
    dst = src.clone();
    dst.forEach<Pixel>([](Pixel &pixel, const int *position) -> void {
        float x = (float)position[0];
        float y = (float)position[1];
        pixel = (uchar)sinc(x, y);
    });
}

int main(int argc, char **argv) {
    // showSobel();
    cv::Mat bar;
    generateBar(bar, 100, 300, 100, 100);
    // generateStripes(bar, 1);

    imshow("Bar", bar);

    cv::Mat sincOnGrid;
    applySincOnGrid(bar, sincOnGrid);
    imshow("sincOnGrid", sincOnGrid);

    // cv::Mat sincBetweenGrid;
    // applySincBetweenGrid(bar, sincBetweenGrid);
    // imshow("sincBetweenGrid", sincBetweenGrid);

    while (true) {
        if (cv::waitKey(0) == 27)
            break;
    }

    return 0;
}
