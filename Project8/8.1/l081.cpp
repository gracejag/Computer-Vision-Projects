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

Mat myTranslate(Mat m) {
   double xScale= 400; //centered
   double yScale= 300;
   double zScale= 0;
   std::vector<std::vector<double>> t = {{1, 0, 0, xScale}, {0, 1, 0, yScale},{0, 0, 1, zScale},{0, 0, 0, 1}};

   Mat translations = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
   
   return translations * m; //vector multiplication
}

Mat myXRotate(Mat m, double angle) {
    //matrix for rotating against x axis
   std::vector<std::vector<double>> t = {{1, 0, 0, 0},{0, cos(angle), -sin(angle), 0},
                                    {0, sin(angle), cos(angle), 0}, {0, 0, 0, 1}};
   
   Mat rotateX = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
   
   return rotateX * m;
}

void part1() {
    Scalar black = Scalar(0,0,0); Scalar white = Scalar(255,255,255);
    
    //CUBE 8 vertices:
    std::vector<std::vector<double>> points = {{1, 1, 1, 1},{1, 1, -1, 1},{1, -1, 1, 1},
    {-1, 1, 1, 1},{-1, -1, 1, 1},{-1, 1, -1, 1},{1, -1, -1, 1},{-1, -1, -1, 1}};
    
    vector<Mat> cube;
    VideoWriter video;
    video.open("rotation.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 60, Size(800, 600));
    
    for(int i = 0; i < points.size(); i++) {
        //holds vertices in mat
        Mat point = (Mat_<double>(4, 1) << points[i][0], points[i][1], points[i][2],points[i][3]);
        
        //TRANSFORMATION 1: scaling
        double x_scale=80;
        double y_scale=80;
        double z_scale=80;
        std::vector<std::vector<double>> t = {{x_scale, 0, 0, 0},{0, y_scale, 0, 0},
                                                {0, 0, z_scale, 0},{0, 0, 0, 1}};
        
        Mat scale = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
        
        Mat scaled = scale * point;
        cube.push_back(scaled);
    }
    
    int amt = 200;
    double a = 2 * 3.14 / static_cast<double>(amt); //for angle
    
    ofstream results("coordinates.txt");
    
    for(int b = 0; b <= amt; b++) {
        Mat image(600, 800, CV_8UC3, white);
        
        vector<Mat> cubeMove;
        
        double currAng = a*b;
        for(int i = 0; i < cube.size(); i++) {
            //TRANSFORMATION 2: rotate x axis
            Mat firstTrans = myXRotate(cube[i], currAng);
            //TRANSFORMATION 3: translate
            Mat secondTrans = myTranslate(firstTrans);

            cubeMove.push_back(secondTrans); //after transformations applied
            //copy to new vector
            std::vector<double> vec(secondTrans.begin<double>(), secondTrans.end<double>());
            //create circle for point
            Point pr=Point(vec[0], vec[1]);
            circle(image, pr, 3, black, 1);
            
            int r = cube.size()-1; //last one
            //write to coordinates.txt the points
            
            if(b < 4) {
                if(i==3){results << "(" << vec[0] << "," << vec[1] << "," << vec[2] << ")" << endl;}
                else {results << "(" << vec[0] << "," << vec[1] << "," << vec[2] << "), ";}
            }
            if(b== 4) {results.close();}
            
        }
        //ORTHOGRAPHIC PROJECTION - 2D
        vector<Point> cubepoints;
        for(int i = 0; i < cubeMove.size(); i++) {
            //copy onto  vector to store
            std::vector<double> vec(cubeMove[i].begin<double>(), cubeMove[i].end<double>());
            //use first two points only - ignore third
            cubepoints.push_back(Point(vec[0], vec[1]));
        }
        //draw lines to connect vertices
        line(image, cubepoints[0], cubepoints[1], black);
        line(image, cubepoints[0], cubepoints[2], black);
        line(image, cubepoints[0], cubepoints[3], black);
        line(image, cubepoints[4], cubepoints[3], black);
        line(image, cubepoints[4], cubepoints[2], black);
        line(image, cubepoints[5], cubepoints[3], black);
        line(image, cubepoints[5], cubepoints[1], black);
        line(image, cubepoints[6], cubepoints[2], black);
        line(image, cubepoints[6], cubepoints[1], black);
        line(image, cubepoints[7], cubepoints[6], black);
        line(image, cubepoints[7], cubepoints[5], black);
        line(image, cubepoints[7], cubepoints[4], black);
        
        video << image;
    }
    
    //NEW SHAPE: tetrahedron 4 sides
    std::vector<std::vector<double>> tetra = {{1.0/sqrt(3), 0, 0, 1},
                                             {0, 0, 2/sqrt(6), 1},
                                             {-sqrt(3)/6.0, 0.5, 0, 1},
                                             {-sqrt(3)/6.0, -0.5, 0, 1}};
    //fourth vertex??
    vector<Mat> tetraVert;

    for(int i = 0; i < tetra.size(); i++) {
        Mat point = (Mat_<double>(4, 1) << tetra[i][0], tetra[i][1], tetra[i][2], tetra[i][3]);
        //TRANSFORMATION 1: scaling
        double x_scale=80;
        double y_scale=80;
        double z_scale=80;
        std::vector<std::vector<double>> t = {{x_scale, 0, 0, 0},{0, y_scale, 0, 0},
                                                {0, 0, z_scale, 0},{0, 0, 0, 1}};
        Mat scale = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
        Mat scaled = scale * point;
        //vector multiplication to get new mat
        tetraVert.push_back(scaled);
    }

    for(int b = 0; b <= amt; b++) {
        Mat image(600, 800, CV_8UC3, white);
        vector<Mat> tetraPoints;
        
        
        double currAng=a*b;
        for(int i = 0; i < tetraVert.size(); i++) {
            //TRANSFORMATION 2: rotate x
            Mat firstTra = myXRotate(tetraVert[i], currAng);
            //TRANSFORMATION 3: translate
            Mat translate = myTranslate(firstTra);
            tetraPoints.push_back(translate);
            
            //copy to new vec
            std::vector<double> vec(translate.begin<double>(), translate.end<double>());
            Point pr=Point(vec[0],vec[1]);
            //draw circle use first two points
            circle(image, pr, 3, black, 1);
        }
        
        //PROJECTION
        vector<Point> drawLines;
        for(int i = 0; i < tetraPoints.size(); i++) {
            std::vector<double> vec(tetraPoints[i].begin<double>(), tetraPoints[i].end<double>());
            //only look at first two points for 2D
            drawLines.push_back(Point(vec[0], vec[1]));
        }
        //connect all the lines
        line(image, drawLines[1], drawLines[0], black);
        line(image, drawLines[1], drawLines[2], black);
        line(image, drawLines[1], drawLines[3], black);
        line(image, drawLines[2], drawLines[0], black);
        line(image, drawLines[2], drawLines[3], black);
        line(image, drawLines[0], drawLines[3], black);
        video << image;
    }
    
    
}

int main(int argc, char** argv) {
    part1();
    return 0;
}


