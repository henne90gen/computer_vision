#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <cmath>
#include <functional>
#include <iostream>
#include <queue>

int main(int argc, char **argv) {
    const char *filename = "../lines1.png";
    cv::Mat original = cv::imread(filename);

    cv::cvtColor(original, original, cv::COLOR_BGR2GRAY);

    cv::Mat gradient_hor;
    cv::Mat gradient_ver;
    cv::Sobel(original, gradient_hor, CV_32F, 1, 0, 1);
    cv::Sobel(original, gradient_ver, CV_32F, 0, 1, 1);
    gradient_hor = (gradient_hor + 255) / 2;
    gradient_ver = (gradient_ver + 255) / 2;
    gradient_hor.convertTo(gradient_hor, CV_8U);
    gradient_ver.convertTo(gradient_ver, CV_8U);

    cv::Mat gradient_angles = gradient_hor.clone();

    cv::Mat histogram = cv::Mat::zeros(1, 255, CV_32S);

    for (int row = 0; row < gradient_ver.rows; row++) {
        uchar *p_ver = gradient_ver.ptr(row);
        uchar *p_hor = gradient_hor.ptr(row);
        uchar *p_ang = gradient_angles.ptr(row);
        for (int col = 0; col < gradient_ver.cols; col++) {
            double angle = atan2(*p_ver, *p_hor) / M_PI * 180.0;
            uchar angle_uchar = (uchar)angle;
            *p_ang = angle_uchar;
            histogram.at<int>(angle_uchar) += 1;
            p_ver++;
            p_hor++;
            p_ang++;
        }
    }

    std::cout << histogram << std::endl;
    std::cout << histogram.at<int>(90) << std::endl;

    imshow("original", original);
    imshow("gradient-hor", gradient_hor);
    imshow("gradient-ver", gradient_ver);
    imshow("gradient-angles", gradient_angles);

    while (true) {
        if (cv::waitKey(0) == 27)
            break;
    }

    return 0;
}
