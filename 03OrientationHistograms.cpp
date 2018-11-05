#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <cmath>
#include <functional>
#include <iostream>
#include <queue>

int main(int argc, char **argv) {
    const char *filename = "../keyb7.jpg";
    // const char *filename = "../bar.png";
    cv::Mat original = cv::imread(filename);

    cv::cvtColor(original, original, cv::COLOR_BGR2GRAY);

    cv::Mat gradient_hor;
    cv::Mat gradient_ver;
    cv::Sobel(original, gradient_hor, CV_32F, 1, 0, 1);
    cv::Sobel(original, gradient_ver, CV_32F, 0, 1, 1);
    gradient_hor = (gradient_hor + 255.0) / 2.0;
    gradient_ver = (gradient_ver + 255.0) / 2.0;
    gradient_hor.convertTo(gradient_hor, CV_8U);
    gradient_ver.convertTo(gradient_ver, CV_8U);

    cv::Mat gradient_angles = gradient_hor.clone();
    cv::Mat gradient_magnitudes = gradient_hor.clone();
    cv::Mat histogram = cv::Mat::zeros(1, 36, CV_32F);
    for (int row = 0; row < gradient_ver.rows; row++) {
        uchar *p_ver = gradient_ver.ptr(row);
        uchar *p_hor = gradient_hor.ptr(row);
        uchar *p_ang = gradient_angles.ptr(row);
        uchar *p_mag = gradient_magnitudes.ptr(row);
        for (int col = 0; col < gradient_ver.cols; col++) {
            double x = (double)*p_hor;
            double y = (double)*p_ver;
            if (x != 0 || y != 0) {
                std::cout << "(" << x << "," << y << ") ";
            }
            double angle = (atan2(y, x) + M_PI) / M_PI * 180.0;
            angle -= 180.0;
            if (angle < 0) {
                angle += 360.0;
            }
            double magnitude = abs(sqrt(y * y + x * x));
            *p_mag = (uchar)magnitude;
            uchar angle_uchar = (uchar)(angle / 360.0 * 255.0);
            *p_ang = angle_uchar;
            histogram.at<float>(angle / 10.0) += magnitude;
            p_ver++;
            p_hor++;
            p_ang++;
        }
        std::cout << std::endl;
    }

    double x1, x2, x3, y1, y2, y3, prev_x, prev_y;
    bool lookingForNext = false;
    double max = 0.0;
    for (int i = 0; i < histogram.cols; i++) {
        if (max < histogram.at<float>(i)) {
            max = histogram.at<float>(i);
        }
        if (y2 < histogram.at<float>(i)) {
            y2 = histogram.at<float>(i);
            x2 = i;
            y1 = prev_y;
            x1 = prev_x;
            lookingForNext = true;
        }
        if (histogram.at<float>(i) != 0) {
            prev_y = histogram.at<float>(i);
            prev_x = i;
            if (lookingForNext && histogram.at<float>(i) != y2) {
                y3 = histogram.at<float>(i);
                x3 = i;
                lookingForNext = false;
            }
        }
    }
    cv::Mat A = cv::Mat::zeros(3, 3, CV_32F);
    A.at<float>(0, 0) = x1 * x1;
    A.at<float>(0, 1) = x1;
    A.at<float>(0, 2) = 1.0;
    A.at<float>(1, 0) = x2 * x2;
    A.at<float>(1, 1) = x2;
    A.at<float>(1, 2) = 1.0;
    A.at<float>(2, 0) = x3 * x3;
    A.at<float>(2, 1) = x3;
    A.at<float>(2, 2) = 1.0;

    cv::Mat B = cv::Mat::zeros(3, 1, CV_32F);
    B.at<float>(0, 0) = y1;
    B.at<float>(1, 0) = y2;
    B.at<float>(2, 0) = y3;

    cv::invert(A, A);
    cv::Mat result = A * B;

    cv::Mat hist = cv::Mat::zeros(255, 255, CV_8UC3);
    for (int x = 0; x < hist.cols; x++) {
        double lookup = (double)x / 255.0 * 36.0;
        double y = histogram.at<float>((int)lookup);
        y = y / max * 255.0;
        line(hist, cv::Point(x, 255), cv::Point(x, 255.0 - y), CV_RGB(255, 255, 255));
    }
    imshow("hist", hist);

    double a = result.at<float>(0);
    double b = result.at<float>(1);
    double c = result.at<float>(2);

    double mainAngle = (-1.0 * b) / (2.0 * a);

    cv::Point2f original_center(original.cols / 2.0, original.rows / 2.0);
    cv::Mat rot_mat = cv::getRotationMatrix2D(original_center, -1.0 * mainAngle, 1.0);
    cv::Mat rotated;
    cv::warpAffine(original, rotated, rot_mat, original.size());
    imshow("original", original);
    imshow("rotated", rotated);
    imshow("gradient-angles", gradient_angles);
    imshow("gradient-magnitudes", gradient_magnitudes);

    while (true) {
        if (cv::waitKey(0) == 27)
            break;
    }

    return 0;
}
