#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;


int main()
{
Mat img;
img = cv::imread("test.png");
namedWindow("Display window", CV_WINDOW_AUTOSIZE);
imshow("Original Image", img);
waitKey();
return 0;
}
