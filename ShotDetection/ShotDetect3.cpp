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

inline vector<float> calcColorHist(Mat &img){
    int channels = img.channels();
    int nRows = img.rows;
    int nCols = img.cols * channels;

    if(img.isContinuous()) // matrix without gaps?
        nCols *= nRows; // number of bytes (including all channels)

    vector<float> hist(256);
    // initialize with zeros
    for(int i = 0; i < 256; ++i)
        hist[i] = 0.0;

    uchar b,g,r;
    uchar *p = img.ptr<uchar>(0);
    for(int j = 0, q = 0; j < nCols; ++q){
        b = (p[j++] & 0xC0); // two msb
        g = (p[j++] & 0xE0) >> 2; // three msb
        r = (p[j++] & 0xE0) >> 5;
        hist[b|g|r]++;
    }
    /*for(int i = 0; i < hist.size(); i++){
        cout << hist[i] << " ";
    }
    cout << endl;*/

    // normalization of results
    float sum = nCols / channels;
    for(int i = 0; i < hist.size(); i++)
        hist[i] = hist[i] / sum;

    /*for(int i = 0; i < hist.size(); i++){
        cout << hist[i] << " ";
    }
    cout << endl;*/
    return hist;
}

inline Mat getHog(Mat &img){
    Mat grad, img_grey, img_clone = img.clone();
    int ddepth = CV_16S;
    int ksize = 1;
    int scale = 1;
    int delta = 0;

    GaussianBlur(img_clone, img_clone, Size(3, 3), 0, 0, BORDER_DEFAULT);

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
    //imshow("bla", grad);
    //waitKey(0);
    /*Mat img_clone = img.clone();
    img_clone.convertTo(img_clone, CV_32F, 1/255.0);

    imshow('sdfs',img_clone);
    waitKey(0);
    vector<float> hist(256,0.0);

    // Calculate gradients gx, gy
    Mat gx, gy;
    Mat abs_gx, abs_gy;
    Sobel(img_clone, gx, CV_32F, 1, 0, 1);
    Sobel(img_clone, gy, CV_32F, 0, 1, 1);

    // converting back to CV_8U
    convertScaleAbs(gx, abs_gx);
    convertScaleAbs(gy, abs_gy);


    int channels = abs_gx.channels();
    int nRows = abs_gx.rows;
    int nCols = abs_gx.cols * channels;

    if(abs_gx.isContinuous()) // matrix without gaps?
        nCols *= nRows; // number of bytes (including all channels)


    uchar x,y;
    uchar *p = abs_gx.ptr<uchar>(0);
    uchar *q = abs_gy.ptr<uchar>(0);
    for(int j = 0; j < nCols; ++j){
        x = (p[j] & 0xf0);      // four msb
        y = (q[j] & 0xf0) >> 4; // four msb
        hist[x|y]++;
    }
   */
    /*
    uchar x,y;
    for(int j=0;j<abs_gx.rows;j++)
    {
        for (int i=0;i<abs_gx.cols;i++)
        {
                //hist[abs_gx.at<uchar>(j,i)]++;
                //hist[256+abs_gy.at<uchar>(j,i)]++;
            uchar x1 = abs_gx.at<uchar>(j,i);
            x = abs_gx.at<uchar>(j,i) & 0xf0;
            y = (abs_gy.at<uchar>(j,i) & 0xf0) >> 4;
            hist[x|y]++;
        }
    }*/

    // normalize
    /*float sum = abs_gx.cols * abs_gx.rows;
    for(int i = 0; i < hist.size(); i++)
        hist[i] = hist[i] / sum;

    return hist;*/
   // vector<float> bla;
   // return bla;
    return grad;
}

inline float intersection(vector<float> &h1, vector<float> &h2){
    float intersect = 0;

    for(int i = 0; i < h1.size(); ++i)
        intersect += min(h1[i],h2[i]);

    return intersect;
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
    vector<float> lastColorHist, currentColorHist;
    double frameNumber = 0;
    float colorDelta, hogDelta, similarity = 0;
    float tdSum = 0;

    Mat lastHog, currentHog;
    Mat diff;

    cap.read(frame);
    lastColorHist = calcColorHist(frame);
    lastHog = getHog(frame);
    double currentEdgeDiff, lastEdgeDiff = 0;

    while(cap.read(frame)){
        //currentColorHist = calcColorHist(frame);
        currentHog = getHog(frame);

        //similarity = intersection(lastColorHist, currentColorHist);
        //colorDelta = 1 - similarity;

        //similarity = intersection(lastHog, currentHog);

        absdiff(lastHog, currentHog, diff);
        currentEdgeDiff = cv::sum(diff).val[0];

        hogDelta = abs(lastEdgeDiff - currentEdgeDiff)/lastEdgeDiff;

        /*if(colorDelta > td || hogDelta > td){
            // gradual buildup of change -> detect fading
            tdSum += colorDelta - td;
        }else{
            tdSum = 0;
        }*/

        //if(colorDelta > th || tdSum > th || hogDelta > th){
        if(hogDelta > th){
            frameNumber = cap.get(CV_CAP_PROP_POS_FRAMES);
            //imshow(to_string(frameNumber),frame);
            cout <<  argv[3]  << ": detected shot border at frame: " << frameNumber << "\n";
            imwrite(outputPath + "@" + to_string((int)frameNumber) + ".png", frame);
            cout << "lastedgediff: " << lastEdgeDiff << "\t currentEdgeDiff: " << currentEdgeDiff << "\nhogDelta: " << hogDelta << "\n";

            tdSum = 0;
        }
        //lastColorHist = currentColorHist;
        lastEdgeDiff = currentEdgeDiff;

    }
    return 0;
}