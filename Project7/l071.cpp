#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
// GRACE JAGGA
using namespace std;
using namespace cv;

void getImg(int argc, char* argv[]){
    int penval=83;int nicval= 94;int quartval= 106;
    int dimeval= 78;int silval= 166;int halfval= 133;
    int highThresh=175;int lowThresh=155;
    int smallestval=83;int biggestval=106;
    string origFile="easyCoins.jpg"; string finalFile="imagef.jpg";
    string imageCirclesFile="imageCircles.jpg";
    string imageCoinsFile="imageCoins.jpg";string resultsTxt="results.txt";
    if(argc>1){
        for(int i=0;i<argc;i++){
            if(i!=0){
                char* c = argv[i-1];
                if((string)c=="-f"){ //image name
                    origFile=(string)argv[i];
                }
                if((string)c=="-lt"){ //canny low
                    lowThresh=stoi(string(argv[i]));
                }
                if((string)c=="-ht"){ //canny high
                    highThresh=stoi(string(argv[i]));
                }
                if((string)c=="-ff"){
                    finalFile=(string)argv[i];
                }
                if((string)c=="-fCi"){
                    imageCirclesFile=(string)argv[i];
                }
                if((string)c=="-fCo"){
                    imageCoinsFile=(string)argv[i];
                }
                if((string)c=="-fR"){
                    resultsTxt=(string)argv[i];
                }
                if((string)c=="-penval"){
                    penval=stoi((string)argv[i]);
                    smallestval=penval;
                    quartval=1.27*penval; biggestval=quartval;
                    silval=2*penval; halfval=1.61*penval; dimeval=penval*0.94;
                    //change all the values based on penny val
                }
                if((string)c=="-nicval"){
                    nicval=stoi((string)argv[i]);
                }
                if((string)c=="-quartval"){
                    quartval=stoi((string)argv[i]);
                    biggestval=quartval; //biggestval
                }
                if((string)c=="-dimeval"){
                    dimeval=stoi((string)argv[i]);
                    smallestval = dimeval;
                }
                if((string)c=="-halfval"){
                    halfval=stoi((string)argv[i]);
                    biggestval=halfval;
                }
                if((string)c=="-silval"){
                    silval=stoi((string)argv[i]);
                    //biggestval=silval;
                }
            }
        }
    }
    
    Mat image;
    image = imread( origFile, 1 );
    Mat gray;
    gray = imread( origFile, IMREAD_GRAYSCALE );
    imwrite("./imageg.jpg", gray);
    
    //Applying the Canny Edge filter
    Mat edges;
    const int ratio = 3;
    const int kernel_size = 3;
    blur(gray, edges, Size(3,3));
    Canny(image, edges, lowThresh, highThresh, kernel_size);
    imwrite("./"+finalFile, edges);
    
    Mat circleImg; //make a copy of original image
    circleImg.create( image.size(), image.type() );
    image.copyTo(circleImg);
    vector<Vec3f> circles;
    
    int silvAmt=0;
    vector<Vec3f> silvCoins;
    HoughCircles(gray, silvCoins, HOUGH_GRADIENT, 1,160, 100, 30, silval-10, silval);
    for( size_t i = 0; i < silvCoins.size(); i++ ){
        Vec3i c = silvCoins[i];
        Point center = Point(c[0], c[1]);// circle center
        Scalar s=Scalar(0,0,255);
        circle( circleImg, center, 4, Scalar(0,0,255), 3, LINE_AA);// circle outline
        int radius = c[2];
        circle( circleImg, center, radius, s, 3, LINE_AA);
        circle( circleImg, center, radius+1, s, 3, LINE_AA);
        circle( circleImg, center, radius+2, s, 3, LINE_AA);
    }
    
    
    HoughCircles(gray, circles, HOUGH_GRADIENT, 1,160, 100, 30, smallestval, biggestval);
    for( size_t i = 0; i < circles.size(); i++ ){
        Vec3i c = circles[i];
        bool isCoin=true;;
        Point center = Point(c[0], c[1]);// circle center
        for(size_t r=0;r<silvCoins.size();r++){
            Vec3i ccc = silvCoins[r];
            Point cc = Point(ccc[0], ccc[1]);
            double dist = sqrt(pow(cc.y - center.y, 2) + pow(cc.x - center.x, 2));
            if(dist<silval){ //within the silver coin, 120
                isCoin=false;
            }
        }
        if(isCoin){
            circle( circleImg, center, 4, Scalar(0,0,255), 3, LINE_AA);// circle outline
            int radius = c[2];
            circle( circleImg, center, radius, Scalar(0,0,255), 3, LINE_AA);
            circle( circleImg, center, radius+1, Scalar(0,0,255), 3, LINE_AA);
            circle( circleImg, center, radius+2, Scalar(0,0,255), 3, LINE_AA);
        }
    }
    imwrite("./"+imageCirclesFile, circleImg);

    Mat coinsImg;
    int penAmt=0;int nicAmt=0;int quartAmt=0;
    int dimeAmt=0;int halfAmt=0;
    image.copyTo(coinsImg);
    bool isCoin=true;
    
    for( size_t i = 0; i < silvCoins.size(); i++ ){
        Vec3i c = silvCoins[i];
        Point center = Point(c[0], c[1]);// circle center
        Scalar s = Scalar(227,78,243);
        circle( circleImg, center, 4, Scalar(0,0,255), 3, LINE_AA);// circle outline
        int radius = c[2];
        silvAmt++;
        circle( circleImg, center, radius, s, 3, LINE_AA);
        circle( circleImg, center, radius+1, s, 3, LINE_AA);
        circle( circleImg, center, radius+2, s, 3, LINE_AA);
    }
    for( size_t i = 0; i < circles.size(); i++ ){
        Vec3i c = circles[i];
        Point center = Point(c[0], c[1]);// circle center
        isCoin=true;
        for(size_t r=0;r<silvCoins.size();r++){
            Vec3i ccc = silvCoins[r];
            Point cc = Point(ccc[0], ccc[1]);
            double dist = sqrt(pow(cc.y - center.y, 2) + pow(cc.x - center.x, 2));
            if(dist<silval){ //within the silver coin, 120
                isCoin=false;
            }
        }
        if(isCoin){
            int radius = c[2];
            Scalar s=Scalar(0,0,255); //red default
            if(abs(radius-dimeval)<5){ //dime blue
                s = Scalar(220,71,7); dimeAmt++;
            }
            else if(abs(radius-halfval)<3 && halfAmt<1){ //half dollar green
                s = Scalar(22,175,83); halfAmt++;
            }
            else if(abs(radius-penval)<7){ //penny red
                s = Scalar(0,0,255); penAmt++;
            }
            else if(abs(radius-nicval)<4){ //nickel yellow
                s = Scalar(113,246,246); nicAmt++;
            }
            else if(abs(radius-quartval)<8){ //quarter purple
                s = Scalar(165,38,102); quartAmt++;
            }
            circle( circleImg, center, radius, s, 3, LINE_AA);
            circle( circleImg, center, radius+1, s, 3, LINE_AA);
            circle( circleImg, center, radius+2, s, 3, LINE_AA);
        }
    }
    imwrite("./"+imageCoinsFile, circleImg);
    
    double sum = (penAmt*0.01)+(nicAmt*0.05)+(dimeAmt*0.1)+
    (quartAmt*0.25)+(halfAmt*0.5)+(silvAmt*1);
    sum = (sum*100)+0.5;
    int su = sum;
    sum = su/100.00;
    
    cout << "\nTotal sum: $" << sum;
    cout << "\nAmount of dimes: "<< dimeAmt;
    cout << "\nAmount of pennies: "<< penAmt;
    cout << "\nAmount of nickels: "<< nicAmt;
    cout << "\nAmount of quarters: "<< quartAmt;
    cout << "\nAmount of half dollars: "<< halfAmt;
    cout << "\nAmount of silver dollars : "<< silvAmt;
    cout << "\n";
    
//   ofstream resultsFile("results.txt");
//   resultsFile << "Amount of dimes: "<< dimeAmt;
//   resultsFile << "\nAmount of pennies: "<< penAmt;
//   resultsFile << "\nAmount of nickels: "<< nicAmt;
//   resultsFile << "\nAmount of quarters: "<< quartAmt;
//   resultsFile << "\nAmount of half dollars: "<< halfAmt;
//   resultsFile << "\nAmount of silver dollars : "<< silvAmt;
//   resultsFile << "\nTotal sum: $" << sum;
//   resultsFile.close();

    waitKey(0);
}

int main(int argc, char** argv ){
  getImg(argc, argv);
  return 0;
}
