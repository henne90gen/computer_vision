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
    cv::Mat show_gradient_hor = (gradient_hor + 255.0) / 2.0;
    cv::Mat show_gradient_ver = (gradient_ver + 255.0) / 2.0;
    show_gradient_hor.convertTo(show_gradient_hor, CV_8U);
    show_gradient_ver.convertTo(show_gradient_ver, CV_8U);

    imshow("gradient-hor", show_gradient_hor);
    imshow("gradient-ver", show_gradient_ver);

    cv::Mat gradient_angles = cv::Mat::zeros(gradient_hor.rows, gradient_hor.cols, CV_8U);
    cv::Mat gradient_magnitudes = gradient_hor.clone();
    int num_hist_bins = 360;
    double histogram[360] = {0};
    for (int row = 0; row < gradient_ver.rows; row++) {
        float *p_ver = gradient_ver.ptr<float>(row);
        float *p_hor = gradient_hor.ptr<float>(row);
        uchar *p_ang = gradient_angles.ptr(row);
        float *p_mag = gradient_magnitudes.ptr<float>(row);
        for (int col = 0; col < gradient_ver.cols; col++) {
            double x = (double)*p_hor;
            double y = (double)*p_ver;
            double magnitude = sqrt(x * x + y * y);
            double angle = atan2(-y, x) / M_PI * 180.0;
            if (angle < 0) {
                angle += 360.0;
            }
            if (magnitude > 30) {
                histogram[(int)angle] += magnitude;
            }
            *p_mag = magnitude;
            *p_ang = (uchar)(angle / 360.0 * 255.0);
            p_ver++;
            p_hor++;
            p_ang++;
            p_mag++;
        }
    }

    double x1, x2, x3, y1, y2, y3, prev_x, prev_y;
    bool lookingForNext = false;
    double max = 0.0;
    for (int i = 0; i < 360; i++) {
        if (max < histogram[i]) {
            max = histogram[i];
        }
        if (y2 < histogram[i]) {
            y2 = histogram[i];
            x2 = i;
            y1 = prev_y;
            x1 = prev_x;
            lookingForNext = true;
        }
        if (histogram[i] != 0) {
            prev_y = histogram[i];
            prev_x = i;
            if (lookingForNext && histogram[i] != y2) {
                y3 = histogram[i];
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

    cv::Mat hist = cv::Mat::zeros(360, 360, CV_8UC3);
    for (int x = 0; x < hist.cols; x++) {
        double y = histogram[x];
        y = y / max * 360.0;
        line(hist, cv::Point(x, 360), cv::Point(x, 360.0 - y), CV_RGB(255, 255, 255));
    }
    imshow("hist", hist);

    double a = result.at<float>(0);
    double b = result.at<float>(1);
    double c = result.at<float>(2);

    double mainAngle = (-1.0 * b) / (2.0 * a);
    std::cout << mainAngle << std::endl;

    cv::Point2f original_center(original.cols / 2.0, original.rows / 2.0);
    cv::Mat rot_mat = cv::getRotationMatrix2D(original_center, -1.0 * mainAngle, 1.0);
    cv::Mat rotated;
    cv::warpAffine(original, rotated, rot_mat, original.size());
    imshow("original", original);
    imshow("rotated", rotated);

    cv::Mat show_gradient_angles;
    gradient_magnitudes.convertTo(show_gradient_angles, CV_8U, 0.71);
    imshow("gradient-angles", gradient_angles);

    cv::Mat show_gradient_magnitudes;
    gradient_magnitudes.convertTo(show_gradient_magnitudes, CV_8U, 0.71);
    imshow("gradient-magnitudes", show_gradient_magnitudes);

    while (true) {
        if (cv::waitKey(0) == 27) {
            break;
        }
    }

    return 0;
}
