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
   std::vector<std::vector<double>> t = {{1, 0, 0, 0},{0, cos(angle), -sin(angle), 0},{0, sin(angle), cos(angle), 0}, {0, 0, 0, 1}};
   
   Mat rotateX = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
   
   return rotateX * m;
}
Mat myZRotate(Mat m, double angle) {
    // Matrix for rotating against z axis
    std::vector<std::vector<double>> t = {{cos(angle), -sin(angle), 0, 0},
        {sin(angle), cos(angle), 0, 0},{0, 0, 1, 0},{0, 0, 0, 1}};
    Mat rotateZ = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3],
        t[1][0], t[1][1], t[1][2], t[1][3],t[2][0], t[2][1], t[2][2], t[2][3],
        t[3][0], t[3][1], t[3][2], t[3][3]);

    return rotateZ * m;
}

double round(double value) {
    return std::round(value * 10.0) / 10.0;
}



void part2() {
    Scalar black = Scalar(0,0,0); Scalar white = Scalar(255,255,255);
    Scalar pink = Scalar(238, 120, 214);
    
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
        double x_scale=200; //80
        double y_scale=200;
        double z_scale=200;
        std::vector<std::vector<double>> t = {{x_scale, 0, 0, 0},{0, y_scale, 0, 0},
                                                {0, 0, z_scale, 0},{0, 0, 0, 1}};
        
        Mat scale = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
        
        Mat scaled = scale * point;
        cube.push_back(scaled);
    }
    
    int amt = 200;
    double a = 2 * 3.14 / static_cast<double>(amt); //for angle
    
//   ofstream results("coordinates.txt");
//   ofstream results2("coordinates2d.txt");
//    
    for(int b = 0; b <= amt; b++) {
        Mat image(600, 800, CV_8UC3, white);
        
        vector<Point> cubepoints;
        double currAng = a*b;
        for(int i = 0; i < cube.size(); i++) {
            //TRANSFORMATION 2: rotate x axis
            Mat firstTrans = myXRotate(cube[i], currAng);
            Mat secondTrans = myZRotate(firstTrans, currAng);
            //TRANSFORMATION 3: translate
            // Mat secondTrans = myTranslate(firstTrans);
            
            //cubeMove.push_back(secondTrans); //after transformations applied
            std::vector<double> vec(secondTrans.begin<double>(), secondTrans.end<double>());
            
            double eyeval = 10*80; //scaled
            double planeval = 5*80;
            //change plane and eyeval values
            
            //8.2 revision - switch order do it altogether before or not before at all
            
            //APPLY NEW FORMULA FOR U AND V:
            double newU = vec[1] * ((planeval-eyeval)/(vec[0]-eyeval));
            double newV = vec[2] * ((planeval-eyeval)/(vec[0]-eyeval));
            
            //scale and translate; before just rotate - second
            Mat makeMat = (Mat_<double>(4, 1) << newU, newV, 1, 1);
            Mat translatedMat = myTranslate(makeMat); //translate
            
            std::vector<double> matPoints(translatedMat.begin<double>(), translatedMat.end<double>());
            
            Point pr=Point(matPoints[0], matPoints[1]);
            circle(image, pr, 3, black, 1);
            cubepoints.push_back(pr);
            
            //change coordinates.txt here:
          int r = cube.size()-1; //last one
//          if(b < 4) {
//              if(i != r) {
//                  results << "(" << matPoints[0] << "," << (matPoints[1]) << "," << (matPoints[2]) << "), ";}
//              else {results << "(" << (matPoints[0]) << "," << (matPoints[1]) << "," << (matPoints[2]) << ")" << endl;}
//          }
//          if(b == 4) {results.close();}
//          if(b < 4) {
//              if(i != r) {
//                  results2 << "(" << (matPoints[0]) << "," << (matPoints[1]) << "), ";}
//              else {results2 << "(" << (matPoints[0]) << "," << (matPoints[1]) << ")" << endl;}
//          }
//          if(b == 4) {results2.close();}
        }
        
        //draw lines to connect vertices
        line(image, cubepoints[0], cubepoints[1], pink);
        line(image, cubepoints[0], cubepoints[2], pink);
        line(image, cubepoints[0], cubepoints[3], pink);
        line(image, cubepoints[4], cubepoints[3], pink);
        line(image, cubepoints[4], cubepoints[2], pink);
        line(image, cubepoints[5], cubepoints[3], pink);
        line(image, cubepoints[5], cubepoints[1], pink);
        line(image, cubepoints[6], cubepoints[2], pink);
        line(image, cubepoints[6], cubepoints[1], pink);
        line(image, cubepoints[7], cubepoints[6], pink);
        line(image, cubepoints[7], cubepoints[5], pink);
        line(image, cubepoints[7], cubepoints[4], pink);
        
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
        double x_scale=150;
        double y_scale=150;
        double z_scale=150;
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
        vector<Point> pointsTetra;
        
        
        double currAng=a*b;
        for(int i = 0; i < tetraVert.size(); i++) {
            //TRANSFORMATION 2: rotate x
            Mat firstTra = myXRotate(tetraVert[i], currAng);
            Mat secondTrans = myZRotate(firstTra, currAng);
            std::vector<double> vec(secondTrans.begin<double>(), secondTrans.end<double>());
            
            double planeval = 5*80;
            double eyeval = 10*80; //scaled
            
            //APPLY NEW FORMULA FOR U AND V:
            double newU = vec[1] * ((planeval-eyeval)/(vec[0]-eyeval));
            double newV = vec[2] * ((planeval-eyeval)/(vec[0]-eyeval));
            
            //scale and translate; before just rotate
            Mat makeMat = (Mat_<double>(4, 1) << newU, newV, 1, 1);
            Mat translatedMat = myTranslate(makeMat); //translate
            
            std::vector<double> matPoints(translatedMat.begin<double>(), translatedMat.end<double>());
            
            Point pr=Point(matPoints[0], matPoints[1]);
            circle(image, pr, 2, black, 1);
            pointsTetra.push_back(pr);
            
        }
        
        //connect all the lines
        line(image, pointsTetra[1], pointsTetra[0], pink);
        line(image, pointsTetra[1], pointsTetra[2], pink);
        line(image, pointsTetra[1], pointsTetra[3], pink);
        line(image, pointsTetra[2], pointsTetra[0], pink);
        line(image, pointsTetra[2], pointsTetra[3], pink);
        line(image, pointsTetra[0], pointsTetra[3], pink);
        video << image;
        
    }
    
    
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
    
  //  ofstream results("coordinates.txt");
    
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
            
//           if(b < 4) {
//               if(i==3){results << "(" << vec[0] << "," << vec[1] << "," << vec[2] << ")" << endl;}
//               else {results << "(" << vec[0] << "," << vec[1] << "," << vec[2] << "), ";}
//           }
//           if(b== 4) {results.close();}
//           
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
    part2();
    return 0;
}


