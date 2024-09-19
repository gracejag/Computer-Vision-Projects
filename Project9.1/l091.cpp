#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/videoio.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core.hpp>

// GRACE JAGGA
using namespace std;
using namespace cv;

void part1(){
    VideoCapture video("withChessBoard.MOV");
    int f = VideoWriter::fourcc('M', 'J', 'P', 'G');
    Size frame_size(static_cast<int>(video.get(CAP_PROP_FRAME_WIDTH)), static_cast<int>(video.get(CAP_PROP_FRAME_HEIGHT))); //width and height
    VideoWriter finalVid("vr.avi", f, static_cast<int>(video.get(CAP_PROP_FPS)), frame_size);

    
    Mat frame; //each frame in the video
    Mat gray; //gray scale for each
    
    Mat last;
    vector<Point2f> lastPoint; // x y
    
    vector<Point3f> basicSquare = {{-1, 1, 0}, {1, 1, 0}, {1, -1, 0}, {-1, -1, 0}};
    //where four points are
    vector<vector<Point2f>> points2d; //x y
    vector<vector<Point3f>> points3d; //x y z
    vector<vector<Point2f>> final2d; //just x y

    //find checkerboard points
    int frameCurr=0;
    bool frameval=true;
    while(frameval) {
        video >> frame; //store frame from video into Mat frame
        if(frame.empty()) { //amount of frames
            frameval=false;
            break; //vid done
        }
        //for this frame, stores corners of checkerboard in cornersFound vector
        vector<Point2f> cornersFound;
        bool corner = findChessboardCorners(frame, Size(7, 7), cornersFound);
        
        //per frame, no corner
        if(!corner) {
            vector<uchar> status; //1 or 0
            vector<float> error;
            Mat gOld, gCurr;
            cvtColor(last, gOld, COLOR_BGR2GRAY);
            cvtColor(frame, gCurr, COLOR_BGR2GRAY);
            
            if (!lastPoint.empty()) {
                //opticalflow method to see if corner is not chosen
                calcOpticalFlowPyrLK(gOld, gCurr, lastPoint, cornersFound, status, error);
                
                size_t i, k;
                for (i = k = 0; i < status.size(); i++) {
                    if (status[i]) {
                        //get rid of bad ones
                        lastPoint[k] = lastPoint[i];
                        cornersFound[k++] = cornersFound[i];
                    }
                }
                lastPoint.resize(k);
                cornersFound.resize(k);
            }
        }
        frameCurr++;
        
        //exact locations of center of checkboard
        int topLeft=16;
        int bottomRight=32;
        //for THIS FRAME
        vector<Point2f> cornersForSquare = {cornersFound[topLeft], cornersFound[topLeft+2], cornersFound[bottomRight], cornersFound[bottomRight-2]};
        
        points2d.push_back(cornersForSquare);
        points3d.push_back(basicSquare);
        final2d.push_back(cornersForSquare);
        
        //use in next while loop
        lastPoint = cornersFound;
        frame.copyTo(last);
    }
    
    Mat cameraMatrix = Mat::eye(3, 3, CV_64F);
    Mat distCoeffs = Mat::zeros(4, 1, CV_64F);
    Mat R,T;

    //calibrate camera method using 2d points and 3d
    calibrateCamera(points3d, points2d, frame_size, cameraMatrix, distCoeffs, R, T);
    
    vector<Point3f> cubePoints = {
            {-1, 1, 0}, {1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
            {-1, 1, 2}, {1, 1, 2}, {1, -1, 2}, {-1, -1, 2}};

    VideoCapture video1("withChessBoard.MOV");
    int frame_count = 0;

    //for all the frames
    while(video1.read(frame)) {
        vector<Point2f> squareCorners;
        //solvePnp method gives projection of the point
        solvePnP(basicSquare, final2d[frame_count], cameraMatrix, distCoeffs, R, T);

        Scalar pink = Scalar(246,108,214);
        projectPoints(cubePoints, R, T, cameraMatrix, distCoeffs, squareCorners);
        //for drawing
        for (int i = 0; i < 4; ++i) {
            int val = (i+1)%4;
            line(frame, squareCorners[i], squareCorners[val], pink, 5);
            line(frame, squareCorners[i + 4], squareCorners[val + 4], pink, 5);
            line(frame, squareCorners[i], squareCorners[i + 4], pink, 5);
        }
        finalVid << frame;
        frame_count++;
    }

}

int main(int argc, char** argv) {
   part1();
   return 0;
}


