#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace cv;

set<double> IframeNumbers;

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

void getIFrames(string videoFilePath){
    string filename = getBasename(videoFilePath) +"_frames.txt";
    string cmd = "ffprobe " + videoFilePath + " -show_frames | grep -E 'pict_type|coded_picture_number' > " + filename;
    system(cmd.c_str());

    std::ifstream infile(filename);
    std::string line;
    string prefix = "coded_picture_number=";
    while (std::getline(infile, line))
    {
        if (boost::starts_with(line, "pict_type=I")){
            if(std::getline(infile, line)){
                if(boost::starts_with(line,prefix)){
                    boost::replace_all(line, prefix, "");
                    IframeNumbers.insert(boost::lexical_cast<double>(line));
                }
            }
        }
    }

    cmd = "rm " + filename;
    system(cmd.c_str());

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

inline float intersection(vector<float> &h1, vector<float> &h2){
    float intersect = 0;

    for(int i = 0; i < h1.size(); ++i)
        intersect += min(h1[i],h2[i]);

    return intersect;
}

Scalar getMSSIM( const Mat& i1, const Mat& i2)
{
    const double C1 = 6.5025, C2 = 58.5225;
    /***************************** INITS **********************************/
    int d     = CV_32F;

    Mat I1, I2;
    i1.convertTo(I1, d);           // cannot calculate on one byte large values
    i2.convertTo(I2, d);

    Mat I2_2   = I2.mul(I2);        // I2^2
    Mat I1_2   = I1.mul(I1);        // I1^2
    Mat I1_I2  = I1.mul(I2);        // I1 * I2

    /*************************** END INITS **********************************/

    Mat mu1, mu2;   // PRELIMINARY COMPUTING
    GaussianBlur(I1, mu1, Size(11, 11), 1.5);
    GaussianBlur(I2, mu2, Size(11, 11), 1.5);

    Mat mu1_2   =   mu1.mul(mu1);
    Mat mu2_2   =   mu2.mul(mu2);
    Mat mu1_mu2 =   mu1.mul(mu2);

    Mat sigma1_2, sigma2_2, sigma12;

    GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
    sigma1_2 -= mu1_2;

    GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
    sigma2_2 -= mu2_2;

    GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
    sigma12 -= mu1_mu2;

    ///////////////////////////////// FORMULA ////////////////////////////////
    Mat t1, t2, t3;

    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1;
    t2 = sigma1_2 + sigma2_2 + C2;
    t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    Mat ssim_map;
    divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

    Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
    return mssim;
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

    IframeNumbers.clear();
    getIFrames(argv[3]);

    Mat frame;
    VideoCapture cap = VideoCapture(argv[3]);
    vector<float> lastHist, currentHist, lastShotHist;
    double frameNumber = 0;
    float delta, similarity = 0;
    float tdSum = 0;

    cap.read(frame);
    Mat lastShot = frame.clone();
    lastHist == calcColorHist(frame);
    lastShotHist = lastHist;
    while(cap.read(frame)){
        frameNumber = cap.get(CV_CAP_PROP_POS_FRAMES);
        currentHist = calcColorHist(frame);

        similarity = intersection(lastHist, currentHist);
        delta = 1 - similarity;

        if(delta > td){
            // gradual buildup of change -> detect fading
            tdSum += delta - td;
        }else{
            tdSum = 0;
        }

        if(delta > th || tdSum > th || IframeNumbers.find(frameNumber) != IframeNumbers.end()){
            if(getMSSIM(lastShot, frame).val[0] < 0.6 && intersection(lastShotHist, currentHist) < 0.6){
                //imshow(to_string(frameNumber),frame);
                cout <<  argv[3]  << ": detected shot border at frame: " << frameNumber << "\n";
                imwrite(outputPath + "@" + to_string((int)frameNumber) + ".png", frame);

                tdSum = 0;
                lastShot = frame.clone();
                lastShotHist = currentHist;
            }
        }
        lastHist = currentHist;

    }
    return 0;
}