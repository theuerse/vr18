#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include <string>

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

inline vector<float> calcHOGHist(Mat &img){

    Mat img_clone;
    img.copyTo(img_clone);
    img_clone.convertTo(img_clone, CV_32F, 1/255.0);

    vector<float> hist(256*2,0.0);

    // Calculate gradients gx, gy
    Mat gx, gy;
    Mat abs_gx, abs_gy;
    Sobel(img_clone, gx, CV_32F, 1, 0, 1);
    Sobel(img_clone, gy, CV_32F, 0, 1, 1);

    // converting back to CV_8U
    convertScaleAbs(gx, abs_gx);
    convertScaleAbs(gy, abs_gy);

    for(int j=0;j<abs_gx.rows;j++)
    {
        for (int i=0;i<abs_gx.cols;i++)
        {
                hist[abs_gx.at<uchar>(j,i)]++;
                hist[256+abs_gy.at<uchar>(j,i)]++;
        }
    }

    // normalize
    float sum = abs_gx.cols * abs_gx.rows * 2;
    for(int i = 0; i < hist.size(); i++)
        hist[i] = hist[i] / sum;

    return hist;
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
    vector<float> lastColorHist, lastHOGHist, currentColorHist, currentHOGHist;
    double frameNumber = 0;
    float colorDelta, hogDelta, similarity = 0;
    float tdSum = 0;

    cap.read(frame);
    lastColorHist = calcColorHist(frame);
    lastHOGHist = calcHOGHist(frame);
    while(cap.read(frame)){
        currentColorHist = calcColorHist(frame);
        currentHOGHist = calcHOGHist(frame);

        similarity = intersection(lastColorHist, currentColorHist);
        colorDelta = 1 - similarity;

        similarity = intersection(lastHOGHist, currentHOGHist);
        hogDelta = 1 - similarity;

        if(colorDelta > td || hogDelta > td){
            // gradual buildup of change -> detect fading
            tdSum += colorDelta - td;
        }else{
            tdSum = 0;
        }

        //if(colorDelta > th || tdSum > th || hogDelta > th){
        if(hogDelta > th){
            frameNumber = cap.get(CV_CAP_PROP_POS_FRAMES);
            //imshow(to_string(frameNumber),frame);
            cout <<  argv[3]  << ": detected shot border at frame: " << frameNumber << "\n";
            imwrite(outputPath + "@" + to_string((int)frameNumber) + ".png", frame);

            tdSum = 0;
        }
        lastColorHist = currentColorHist;

    }
    return 0;
}