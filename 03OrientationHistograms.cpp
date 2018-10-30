#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/videoio/videoio.hpp"
#include <functional>
#include <iostream>
#include <queue>

int main(int argc, char **argv) {
    const char *filename = "../left.jpg";
    cv::namedWindow("Fruits!");
    // imshow("Fruits!", mapped_result_img);

    while (true) {
        if (cv::waitKey(0) == 27)
            break;
    }

    return 0;
}
