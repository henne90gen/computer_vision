#include <iostream>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <vector>

float getBrightness(cv::Mat &in) {
    cv::Mat hsv;
    cv::cvtColor(in, hsv, cv::COLOR_BGR2HSV);
    cv::Scalar brightness = cv::sum(hsv);
    return (float) brightness.val[2];
}

void adjustBrightness(cv::Mat &in, float bMean) {
    float mean = getBrightness(in) / (in.rows * in.cols);
    in.convertTo(in, -1, 1, bMean - mean);
}

void showHist(const char *windowName, cv::Mat &in) {
    cv::Mat b_hist, g_hist, r_hist;
    std::vector<cv::Mat> bgr_planes;
    split(in, bgr_planes);
    int histSize = 256;
    float range[] = {0, 256}; // the upper boundary is exclusive
    const float *histRange = {range};
    bool uniform = true;
    bool accumulate = false;
    cv::calcHist(&bgr_planes[0], 1, nullptr, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[1], 1, nullptr, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
    cv::calcHist(&bgr_planes[2], 1, nullptr, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double) hist_w / histSize);
    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());
    for (int i = 1; i < histSize; i++) {
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
                 cv::Point(bin_w * (i), hist_h - cvRound(b_hist.at<float>(i))), cv::Scalar(255, 0, 0), 2, 8, 0);
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
                 cv::Point(bin_w * (i), hist_h - cvRound(g_hist.at<float>(i))), cv::Scalar(0, 255, 0), 2, 8, 0);
        cv::line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
                 cv::Point(bin_w * (i), hist_h - cvRound(r_hist.at<float>(i))), cv::Scalar(0, 0, 255), 2, 8, 0);
    }
    cv::imshow(windowName, histImage);
}

int main(int argc, char **argv) {
    cv::Mat left = cv::imread("../left.jpg", cv::ImreadModes::IMREAD_COLOR);
    cv::Mat right = cv::imread("../right.jpg", cv::ImreadModes::IMREAD_COLOR);

    if (!left.data || !right.data) {
        printf("Could not load images\n");
        return 1;
    }
    float bLeft = getBrightness(left);
    float bRight = getBrightness(right);
    float bAll = bLeft + bRight;
    float bMean = bAll / (left.cols * left.rows + right.cols * right.rows);
    adjustBrightness(left, bMean);
    adjustBrightness(right, bMean);

    showHist("Histogram left", left);
    showHist("Histogram right", right);

    cv::Mat in[] = {left, right};
    cv::Mat mixed(left.rows, left.cols, CV_8UC3);
    // left[2] -> mixed[2]
    // right[1] -> mixed[1]
    // right[0] -> mixed[0]
    int from_to[] = {2, 2, 4, 1, 3, 0};
    cv::mixChannels(in, 2, &mixed, 1, from_to, 3);

    cv::Mat result;
    cv::resize(mixed, result, cv::Size(mixed.size().width / 5, mixed.size().height / 5));
    cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display Image", result);
    cv::waitKey(0);
    std::cin.get();
    return 0;
}
