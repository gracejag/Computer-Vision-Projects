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

Mat myTranslate(Mat m, double x, double y, double z) {
  double xScale= x; //centered
  double yScale= y;
  double zScale= z;
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

void part3(int argc, char *argv[]) {
   double nx=1; double ny=3; double nz=2;
   double ax=400; double ay=300; double az=500;
   double ex=700; double ey=300; double ez=700;
   std::vector<cv::Scalar> colors = {
           cv::Scalar(255, 0, 0),   // Blue
           cv::Scalar(0, 255, 0),   // Green
           cv::Scalar(0, 0, 255),   // Red
           cv::Scalar(255, 255, 0), // Cyan
           cv::Scalar(255, 0, 255), // Magenta
           cv::Scalar(0, 255, 255), // Yellow
           cv::Scalar(128, 0, 128), // Purple
           cv::Scalar(0, 128, 128), // Teal
           cv::Scalar(128, 128, 0), // Olive
           cv::Scalar(255, 165, 0), // Orange
           cv::Scalar(0, 128, 0)    // Dark Green
   };
   
   if(argc>1){
       for(int i=0;i<argc;i++){
           if(i!=0){ //not the first one
               char* c = argv[i-1]; //right before it
               if((string)c=="-a"){
                   string s = (string)argv[i];  //"(1,2,3.12)"
                   string s1 = s.substr(1, s.length()-1);
                   vector<string> v;
                   stringstream ss(s1);
                   while(ss.good()) {
                       string substr;
                       getline(ss, substr, ',');
                       v.push_back(substr);
                   }
                   ax=stod(v[0]); ay=stod(v[1]); az=stod(v[2]);
               }
               if((string)c=="-e"){
                   string s = (string)argv[i];  //"(1,2,3.12)"
                   string s1 = s.substr(1, s.length()-1);
                   vector<string> v;
                   stringstream ss(s1);
                   while(ss.good()) {
                       string substr;
                       getline(ss, substr, ',');
                       v.push_back(substr);
                   }
                   ex=stod(v[0]); ey=stod(v[1]); ez=stod(v[2]);
               }
               if((string)c=="-n"){
                   string s = (string)argv[i];  //"(1,2,3.12)"
                   string s1 = s.substr(1, s.length()-1);
                   vector<string> v;
                   stringstream ss(s1);
                   while(ss.good()) {
                       string substr;
                       getline(ss, substr, ',');
                       v.push_back(substr);
                   }
                   nx=stod(v[0]); ny=stod(v[1]); nz=stod(v[2]);
               }
               
           }
       }
   }
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
       double x_scale=130; //80
       double y_scale=130;
       double z_scale=130;
       std::vector<std::vector<double>> t = {{x_scale, 0, 0, 0},{0, y_scale, 0, 0},
                                               {0, 0, z_scale, 0},{0, 0, 0, 1}};
       
       Mat scale = (Mat_<double>(4, 4) << t[0][0], t[0][1], t[0][2], t[0][3], t[1][0], t[1][1], t[1][2], t[1][3], t[2][0], t[2][1], t[2][2], t[2][3], t[3][0], t[3][1], t[3][2], t[3][3]);
       
       Mat scaled = scale * point;
       cube.push_back(scaled);
   }
   
   int amt = 250;
   double a = 2 * 3.14 / static_cast<double>(amt); //for angle

   //eye and plane are matrix, a is point on plane
   Mat eye = (Mat_<double>(3, 1) << ex, ey, ez);//
   Mat normal = (Mat_<double>(3, 1) << nx, ny, nz); //123
   Mat aPoint = (Mat_<double>(3, 1) << ax, ay, az); //point
   //to define a plane, all vertices are on one side of it
   //check with (x-a) * n = 0
   
   //first four frames:
   std::vector<cv::Mat> fourPoints = {
       (cv::Mat_<double>(3, 1) << 0,0,0),
       (cv::Mat_<double>(3, 1) << 1,1,1),
       (cv::Mat_<double>(3, 1) << 1,1,-1),
       (cv::Mat_<double>(3, 1) << 1,-1,0)};
   
   //find all the projections of what it would be on this plane
   std::vector<cv::Mat> firstFour;
   //check to make sure all points of cube are on other side of plane
   for(int i=0;i<4;i++){
       Mat m = fourPoints[i]; //point
       Mat eyetoPlane = aPoint - eye;
       double distPlane = eyetoPlane.dot(normal); //dist from eye to plane
       Mat dist = m - eye;
       double s = distPlane / dist.dot(normal);
       Mat pt = m - dist * s; //projected point
       firstFour.push_back(pt);
   }
   
   //w1 and w2 obtain orthogonal unit vectors
   //2d coordinate system defined by W1 Po and W2
   //x=Po+u*Va + v*Vb
   //x=Po+v*W1 + v*W2
   //W1 W2 perpendicular, mag=1
   
   //gram schmit
   
   //(V1,V2) and (V2,V3) are edges
   Mat Va = firstFour[0]-firstFour[1];
   Mat Vb = firstFour[2]-firstFour[1];
   //normalize to get unit vector of magnitude one
   double vA1 = Va.at<double>(0, 0)*Va.at<double>(0, 0);
   double vA2 = Va.at<double>(1, 0)*Va.at<double>(1, 0);
   double vA3 = Va.at<double>(2, 0)*Va.at<double>(2, 0);
   double s = sqrt(vA1+vA2+vA3); //magA
   Mat vANormalized = Va/s; //w1
   
   //projection of b on a
   Mat projBA = Vb-Va * (Vb.dot(Va)/(s*s));
   
   double vB1 = projBA.at<double>(0, 0)*projBA.at<double>(0, 0);
   double vB2 = projBA.at<double>(1, 0)*projBA.at<double>(1, 0);
   double vB3 = projBA.at<double>(2, 0)*projBA.at<double>(2, 0);
   double s2 = sqrt(vB1+vB2+vB3);
   Mat vBNormalized = projBA/s2; //w2
   
   //output ALL information onto log.txt
   //1 or 3 decimal values
   //a, n, e, what is p0 and what is w1 and w2
   
  ofstream results("log.txt");
  
  results << "For the chosen plane, here are the values that correspond: \n";
  results << "Normal is: (" << normal.at<double>(0)<<","<< normal.at<double>(1)<< ","<< normal.at<double>(2)<< ")\n";
  results << "Eye is: (" << eye.at<double>(0)<<","<< eye.at<double>(1)<< ","<< eye.at<double>(2)<< ")\n";
  results << "Point a is: (" << aPoint.at<double>(0)<<","<< aPoint.at<double>(1)<< ","<< aPoint.at<double>(2)<< ")\n";
  //p0 is projection at 0,0,0
  results << "P0 is: ("<< firstFour[0].at<double>(0)<<","<<firstFour[0].at<double>(1)<<","<<firstFour[0].at<double>(2)<<")\n";
  //w1 and w2
  results << "W1 is : ("<<vANormalized.at<double>(0)<<","<<vANormalized.at<double>(1)<<","<<vANormalized.at<double>(2)<<")\n";
  results << "W2 is : ("<<vBNormalized.at<double>(0)<<","<<vBNormalized.at<double>(1)<<","<<vBNormalized.at<double>(2)<<")\n";

   
     ofstream resultsC("coordinates.txt");
     ofstream resultsC2("coordinates2d.txt");
   for(int b = 0; b <= amt; b++) {
       Mat image(600, 800, CV_8UC3, white);
       
       vector<Point> cubePoints;
       vector<vector<double>> homogenousCoords;
       double currAng = a*b;
       
       for(int i = 0; i < cube.size(); i++) {
           //TRANSFORMATION 2: rotate x axis
           Mat firstTrans = myXRotate(cube[i], currAng);
           Mat secondTrans = myZRotate(firstTrans, currAng);
//            if(b!=0){ //don't translate for the first frame
//                secondTrans = myTranslate(secondTrans, 50, 40, 0); //no translate???
//            }
           std::vector<double> vec(secondTrans.begin<double>(), secondTrans.end<double>());
           //create homogenous coord by dividing by fourth element for points after transformations
           vector<double> coords = {vec[0]/vec[3], vec[1]/vec[3], vec[2]/vec[3]};
           homogenousCoords.push_back(coords);
           Mat mm = (Mat_<double>(3,1) << coords[0], coords[1], coords[2]);
           Mat proj; //projection of point onto plane
           
           Mat eP = aPoint - eye;
           double dP = eP.dot(normal); //dist from eye to plane
           Mat dist1 = mm - eye;
           double ss = dP / dist1.dot(normal);
           Mat pt1 = mm - dist1 * ss; //projected point
           proj = pt1;
           
           //old way
           //double valU = vec[1] * ((planeval-eyeval)/(vec[0]-eyeval));
           //double valV = vec[2] * ((planeval-eyeval)/(vec[0]-eyeval));
           //new u and v?? formula
           double u = (proj-firstFour[0]).dot(vANormalized) / vANormalized.dot(vANormalized);
           double v = (proj-firstFour[0]).dot(vBNormalized) / vBNormalized.dot(vBNormalized);
           
           //x= Po + u*W1 + v*W2;
           //600 is width, half 300
           //(0,0) is (300,400) from top left?
           //+400, +300
           double uval = u+400;
           double vval = v+300; //makes center?
           Point dot = Point(uval, vval);
           circle(image, dot, 5, pink, 4);
           cubePoints.push_back(dot);
           
          int r = cube.size()-1; //last one
          if(b < 4) { //3d coordinates
              resultsC << "(" << coords[0] << "," << coords[1] << "," << coords[2]<<")";
              if(i==r){ resultsC << "\n";}
              else{ resultsC << ", ";}
          }
          if(b == 4) {resultsC.close();}
          if(b < 4) { //2d coordinates
              resultsC2 << "(" << uval << "," << vval << ")";
              if(i==r){ resultsC2 << "\n";}
              else{ resultsC2 << ", ";}
          }
          if(b == 4) {resultsC2.close();}
          
       }
       //only for first four display values now
       vector<Point> edges = {Point(0,1), Point(0,2), Point(0,3), Point(7,6), Point(7,5), Point(7,4), Point(4,3), Point(4,2), Point(5,3), Point(5,1), Point(6,2), Point(6,1)};


      if(b<4){
          //display 12 edges of 3d coordinates
          results << "\n\nThe frame "<<b+1<<" in 3d has the following edges:\n";
          for(int g=0;g<edges.size();g++){
              int f1 = edges[g].x;
              int f2 = edges[g].y;
              results << "(" << homogenousCoords[f1][0] << "," << homogenousCoords[f1][1] << "," << homogenousCoords[f1][2] << "), ";
              results << "(" << homogenousCoords[f2][0] << "," << homogenousCoords[f2][1] << "," << homogenousCoords[f2][2] << ")\n";
          }
          results << "\nThe frame "<<b+1<<" in 2d has the following edges:\n";
          for(int t=0;t<edges.size();t++){
              int f1 = edges[t].x; //0
              int f2 = edges[t].y; //1
              results<<"("<<cubePoints[f1].x<<","<<cubePoints[f1].y<<"), ";
              results<<"("<<cubePoints[f2].x<<","<<cubePoints[f2].y<<")\n";
          }
      }
      if(b==4){
          results.close();
      }
       
       //draw lines to connect vertices
       line(image, cubePoints[0], cubePoints[1], pink);
       line(image, cubePoints[0], cubePoints[2], colors[0]);
       line(image, cubePoints[0], cubePoints[3], colors[1]);
       line(image, cubePoints[4], cubePoints[3], colors[2]);
       line(image, cubePoints[4], cubePoints[2], colors[3]);
       line(image, cubePoints[5], cubePoints[3], colors[4]);
       line(image, cubePoints[5], cubePoints[1], colors[5]);
       line(image, cubePoints[6], cubePoints[2], colors[6]);
       line(image, cubePoints[6], cubePoints[1], colors[7]);
       line(image, cubePoints[7], cubePoints[6], colors[8]);
       line(image, cubePoints[7], cubePoints[5], colors[9]);
       line(image, cubePoints[7], cubePoints[4], colors[10]);
       
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
           vector<double> coords = {vec[0]/vec[3], vec[1]/vec[3], vec[2]/vec[3]};
           Mat mm = (Mat_<double>(3,1) << coords[0], coords[1], coords[2]);
           Mat proj; //projection of point onto plane
           
           Mat eP = aPoint - eye;
           double dP = eP.dot(normal); //dist from eye to plane
           Mat dist1 = mm - eye;
           double ss = dP / dist1.dot(normal);
           Mat pt1 = mm - dist1 * ss; //projected point
           proj = pt1;
           
           double u = (proj-firstFour[0]).dot(vANormalized) / vANormalized.dot(vANormalized);
           double v = (proj-firstFour[0]).dot(vBNormalized) / vBNormalized.dot(vBNormalized);
           
           Point pr=Point(u+400, v+300);
           circle(image, pr, 2, black, 1);
           pointsTetra.push_back(pr);
       }
       
       //connect all the lines
       line(image, pointsTetra[1], pointsTetra[0], colors[0]);
       line(image, pointsTetra[1], pointsTetra[2], colors[1]);
       line(image, pointsTetra[1], pointsTetra[3], colors[2]);
       line(image, pointsTetra[2], pointsTetra[0], colors[3]);
       line(image, pointsTetra[2], pointsTetra[3], colors[4]);
       line(image, pointsTetra[0], pointsTetra[3], colors[5]);
       video << image;
       
   }
   
   
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
           Mat translatedMat = myTranslate(makeMat, 50,40,0); //translate
           
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
           Mat translatedMat = myTranslate(makeMat,50,40,0); //translate
           
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

int main(int argc, char** argv) {
   part3(argc, argv);
   return 0;
}


