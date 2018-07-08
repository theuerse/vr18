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
int cnt = 0;

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

    GaussianBlur(img_clone, img_clone, Size(1, 1), 0, 0, BORDER_DEFAULT);

    // Convert the image to grayscale
    cvtColor(img_clone, img_grey, COLOR_BGR2GRAY);
    Mat grad_x, grad_y;
    Mat abs_grad_x, abs_grad_y;

    equalizeHist( img_grey, img_grey );
    Sobel(img_grey, grad_x, ddepth, 1, 0, ksize, scale, delta, BORDER_DEFAULT);
    Sobel(img_grey, grad_y, ddepth, 0, 1, ksize, scale, delta, BORDER_DEFAULT);
    // converting back to CV_8U
    convertScaleAbs(grad_x, abs_grad_x);
    convertScaleAbs(grad_y, abs_grad_y);
    addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

    // convert image to monochrome
    threshold( grad, grad, 50,255,THRESH_BINARY );

    /*double size = 50;
    Mat element = getStructuringElement( MORPH_ERODE,
                                         Size( 2*size + 1, 2*size+1 ),
                                         Point( size, size ) );
    erode( img_clone, img_clone, element );*/

    //imshow(to_string(cnt++), grad);

    return grad;
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
    bitwise_xor(a,b,a_xor);
    // ebenso B* und eine invertierte Version des Bildes A'
    bitwise_xor(b,a,b_xor);

    /*imshow("a_xor", a_xor);
    imshow("b_xor", b_xor);
    waitKey(0);*/

    //imshow(to_string(cnt++), a);

    vector<Point> black_pixels;

    //In den so entstehenden Bildern A und B wird jeweils die Anzahl aller gefärbten Pixel gezählt
    double eIn, eOut, pIn, pOut;
    cv::findNonZero(a_xor, black_pixels);
    eIn = black_pixels.size();

    cv::findNonZero(b_xor, black_pixels);
    eOut = black_pixels.size();

    double a_sum, b_sum;

    cv::findNonZero(a, black_pixels);
    a_sum = black_pixels.size();

    cv::findNonZero(b, black_pixels);
    b_sum = black_pixels.size();

    if(a_sum == 0 && b_sum == 0) return 0;

    //  eOut wird durch die Anzahl der Kantenpixel im Kantenbild A' geteilt
    pOut = eOut/a_sum;

    // eIn wird durch die Anzahl der Kantenpixel im Kantenbild B' geteilt.
    pIn = eIn/b_sum;

    // Das Ergebnis ist das Kanteneintrittsverhältnis pIn und das Kantenaustrittsverhältnis pOut
    ecr = max(pIn,pOut);

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
    float ecr, ecrSum=0;

    Mat lastEdgeMat, currentEdgeMat;
    Mat diff;

    cap.read(frame);
    lastEdgeMat = frame.clone();

    while(cap.read(frame)){
        //currentEdgeMat = getEdgeMat(frame);
        currentEdgeMat = frame.clone();

        //ecr = getECR(lastEdgeMat, currentEdgeMat);
        ecr = 1 - getMSSIM(lastEdgeMat, currentEdgeMat).val[0];

        if(ecr >= td){
            ecrSum += ecr -td;
        }else{
           ecrSum = 0;
        }

        if(ecr > th){
            frameNumber = cap.get(CV_CAP_PROP_POS_FRAMES);
            //imshow(to_string(frameNumber),frame);
            cout <<  argv[3]  << ": detected shot border at frame: " << frameNumber << "\n";
            imwrite(outputPath + "@" + to_string((int)frameNumber) + ".png", frame);

            ecrSum = 0;
        }
        currentEdgeMat.copyTo(lastEdgeMat);

    }

    waitKey(0);
    return 0;
}