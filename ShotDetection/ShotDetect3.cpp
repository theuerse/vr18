#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include <string>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

using namespace std;
using namespace cv;

//https://stackoverflow.com/questions/874134/find-if-string-ends-with-another-string-in-c (user Pavel)
static bool endsWith(const std::string& str, const std::string& suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

inline string getBasename(string path){
    // https://stackoverflow.com/questions/8520560/get-a-file-name-from-a-path (user hmjd)
    // Remove directory if present.
    // Do this before extension removal in case directory has a period character.
    const size_t last_slash_idx = path.find_last_of("\\/");
    if (std::string::npos != last_slash_idx)
    {
        path.erase(0, last_slash_idx + 1);
    }

    // Remove extension if present.
    const size_t period_idx = path.rfind('.');
    if (std::string::npos != period_idx)
    {
        path.erase(period_idx);
    }

    return path;
}



inline Mat getEdgeMat(Mat &img){
    Mat grad, img_grey, img_clone = img.clone();
    int ddepth = CV_16S;
    int ksize = 1;
    int scale = 1;
    int delta = 0;

    //GaussianBlur(img_clone, img_clone, Size(3, 3), 0, 0, BORDER_DEFAULT);

    // Convert the image to grayscale
    cvtColor(img_clone, img_grey, COLOR_BGR2GRAY);
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;
    Sobel(img_grey, grad_x, ddepth, 1, 0, ksize, scale, delta, BORDER_DEFAULT);
    Sobel(img_grey, grad_y, ddepth, 0, 1, ksize, scale, delta, BORDER_DEFAULT);
    // converting back to CV_8U
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    // convert image to monochrome
    grad = grad > 128;

    return grad;
}

inline double getECR(Mat &x, Mat &y){
    https://de.wikipedia.org/wiki/Edge_Change_Ratio
    double ecr = 0;
    // a ... A', b ... B'
    Mat a = x.clone(), b = y.clone();

    Mat a_dil, b_dil; // A*, B*
    dilate(a, a_dil, Mat(), Point(-1, -1), 2, 1, 1);
    dilate(b, b_dil, Mat(), Point(-1, -1), 2, 1, 1);



    Mat a_inv, b_inv;
    bitwise_not(a,a_inv);
    bitwise_not(b,b_inv);

    Mat a_xor, b_xor;
    // A* und eine invertierte Version des Bildes B' werden XOR-verknüpft
    bitwise_xor(a_dil,b_inv,a_xor);
    // ebenso B* und eine invertierte Version des Bildes A'
    bitwise_xor(b_dil,a_inv,b_xor);

    imshow("a_xor", a_xor);
    imshow("b_xor", b_xor);
    waitKey(0);

    //In den so entstehenden Bildern A und B wird jeweils die Anzahl aller gefärbten Pixel gezählt
    double eIn, eOut, pIn, pOut;
    eIn = cv::sum(a_xor)[0];
    eOut = cv::sum(b_xor)[0];

    double a_sum = cv::sum(a)[0];
    double b_sum = cv::sum(b)[0];

    //  eOut wird durch die Anzahl der Kantenpixel im Kantenbild A' geteilt
    pOut = eOut/a_sum;

    // eIn wird durch die Anzahl der Kantenpixel im Kantenbild B' geteilt.
    pIn = eIn/b_sum;

    // Das Ergebnis ist das Kanteneintrittsverhältnis pIn und das Kantenaustrittsverhältnis pOut
    ecr = max(pIn/a_sum,pOut/b_sum);

    return ecr;
}



int main(int argc, char ** argv){
    if(argc < 5){
        cout << "Please specify: Th, Td, video-path and output-directory.\n";
        cout << "./ShotDetect2 0.5 0.05 /home/theuers/exp2/35749.mp4 /home/theuers/exp2/\n";
        exit(0);
    }

    float th = stof(argv[1]);
    float td = stof(argv[2]);

    string outputPath = argv[4];
    if(!endsWith(outputPath,"/")) outputPath += "/";
    outputPath += getBasename(argv[3]);

    Mat frame;
    VideoCapture cap = VideoCapture(argv[3]);
    double frameNumber = 0;
    float ecr;

    Mat lastEdgeMat, currentEdgeMat;
    Mat diff;

    cap.read(frame);
    lastEdgeMat = getEdgeMat(frame);

    while(cap.read(frame)){
        currentEdgeMat = getEdgeMat(frame);

        ecr = getECR(lastEdgeMat, currentEdgeMat);

        if(ecr > th){
            frameNumber = cap.get(CV_CAP_PROP_POS_FRAMES);
            //imshow(to_string(frameNumber),frame);
            cout <<  argv[3]  << ": detected shot border at frame: " << frameNumber << "\n";
            imwrite(outputPath + "@" + to_string((int)frameNumber) + ".png", frame);
        }
        lastEdgeMat = currentEdgeMat;

    }
    return 0;
}