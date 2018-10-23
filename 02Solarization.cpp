// TU Dresden, Inf, CV1 Ex2, Holger Heidrich
// Aufg 2, non-linear grey value transformation, solarisation
// --
// This program plays with colors simulating solarisation.	
// Clicking two times into the "GreyCurve"-image gives two points.
// Make these points be the extremals of a polynomial of degree three in the range 0..255 x 0..255.\n"
// The "GreyCurve"-image itself has size (512,512). Point (0,0) is upper left corner. \n"
// Use the polynomial y(x) to map values x of the input image to values y of an output image by creating and using a lookup table.\n"
// Saturate to 0 and 255 if the mapped grey / color values exceed these values. \n"
//-----------------------------------------------------------------------------
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <functional>
#include <queue>

//using namespace cv;
//using namespace std;

#define MaxPoints 2
int nb_points = 0;

cv::Mat MapCurveimage512;
cv::Mat image, mapped_result_img;
cv::Point SrcPtInt[MaxPoints];

void on_mouse(int event, int mouseX, int mouseY, int, void *) {
    switch (event) {
        case cv::EVENT_LBUTTONDOWN: {
            // add new point to polygon
            if (nb_points < MaxPoints) {
                SrcPtInt[nb_points].x = mouseX;
                SrcPtInt[nb_points].y = mouseY;
                if (nb_points) {   // second point in SrcPtInt

                    MapCurveimage512 = 0;

                    // read the two extrema points
                    int x1 = SrcPtInt[0].x;
                    int x2 = SrcPtInt[1].x;
                    int y1 = 511 - SrcPtInt[0].y; // "511 -" cause we want "0" in lower left corner
                    int y2 = 511 - SrcPtInt[1].y;
                    double dx = x1 - x2;
                    double dy = y1 - y2;
                    float x0 = (float) (x1 + x2) / 2;
                    float a = (float) (-2.0 * dy / pow(dx, 3.0));
                    float b = (float) (-3.0 / 2.0 * dy / dx);
                    float c = (float) ((y1 + y2) / 2.0 + b * x0);

                    cv::Mat lookupTable(1, 256, CV_8U);
                    for (int x = 0; x < 256; x++) {
                        float newX = (float) x * 2.0f;
                        float base = newX - x0;
                        int y = (int) (a * base * base * base - b * newX + c);
                        y /= 2;
                        lookupTable.data[x] = (uchar) y;
                    }

                    // create the LUT for that polynomial and
                    // draw the polynomial in the MapCurveimage (pixelwise)
                    // your code for the polynomial and color transform (instead of the following line)
                    line(MapCurveimage512, SrcPtInt[0], SrcPtInt[1],
                         CV_RGB(255, 255, 255));    // remove that line example

                    for (int x = 0; x < MapCurveimage512.cols; x++) {
                        int lookupX = (int) ((float) x / (float) MapCurveimage512.cols * 256.0f);
                        int y = lookupTable.data[lookupX] * 2;
                        MapCurveimage512.at<uchar>(512 - y, x) = 255;
                    }

                    // use the lookup table (LUT) to map the input image to the result image
                    // use the same LUT for each color channel (or fantasize)
                    LUT(mapped_result_img, lookupTable, mapped_result_img);

                    // show non-linear mapping curve
                    imshow("GreyCurve", MapCurveimage512);

                    // show the result
                    imshow("result image", mapped_result_img);

                    mapped_result_img = image.clone();
                    nb_points = 0;
                } else nb_points++;
            }
        }
            break;
    }
}

int main(int argc, char **argv) {
    const char *filename = "../left.jpg";
    image = cv::imread(filename, 1);
    mapped_result_img = image.clone();

    cv::namedWindow("GreyCurve");
    cv::namedWindow("Fruits!");
    imshow("Fruits!", mapped_result_img);

    MapCurveimage512.create(512, 512, CV_8U);
    MapCurveimage512 = 0;
    imshow("GreyCurve", MapCurveimage512);

    cv::setMouseCallback("GreyCurve", on_mouse, 0);

    while (true) {
        if (cv::waitKey(0) == 27) break;
    }

    return 0;
}




































































































/*
// determine polynomial coefficients of y(x):=a*(x-x0)�-b*x+c;
int x1 = SrcPtInt[0].x;	int y1 = 511 - SrcPtInt[0].y;
int x2 = SrcPtInt[1].x;	int y2 = 511 - SrcPtInt[1].y;
double        dx = x1 - x2;	double		  dy = y1 - y2;
float x0 = (float)(x1 + x2) / 2;
float a = (float)(-2.0*dy / pow(dx, 3.0));
float b = (float)(-3.0 / 2.0*dy / dx);
float c = (float)((y1 + y2) / 2.0 + b*x0);
*/