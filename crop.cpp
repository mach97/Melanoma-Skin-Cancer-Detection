#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <vector>
#include "assimetry.h"

using namespace std;
using namespace cv;



int main(){
	// Read img
	Mat im= imread("melanoma3.png",1);
	
	Mat img = imread("melanoma3.png",CV_LOAD_IMAGE_GRAYSCALE);
	Mat croppedImg;

	// This line picks out the rectangle from the image
	// and copies to a new Mat
	//img(Rect(xMin,yMin,xMax-xMin,yMax-yMin)).copyTo(croppedImg);

	threshold(img,croppedImg,110,255,THRESH_BINARY_INV);

	Mat fin=recortada(croppedImg);

	Mat edge;
	blur( fin, fin, Size(3,3) );
	Canny(fin,edge,10,20,3);

	Mat drawing=outerborder(fin);

	int area=extractnpixels(fin);
	int perimeter=extractnpixels(drawing);
	float diam = extractGD(fin,area)/118.0;
	int color=detectar(im,area);
	Mat splitted[2];
	float Ass=dividir(fin,splitted,area);
	float bord=border(area,perimeter);
	if(Ass>2) Ass=2;

	cout<<"Area: "<<area<<endl;
	cout<<"Perimeter: "<<perimeter<<endl;
	cout<<"Assimetry: "<<Ass<<endl;
	cout<<"Border: "<<bord<<endl;
	cout<<"Color: "<<color<<endl;
	cout<<"Diameter: "<<diam<<endl;

	float result=Ass*1.3+bord*0.1+color*0.5+diam*0.5;

	cout<<"-----------RESULT------------"<<endl;
	
	cout<<"TDS: "<<result<<endl;
	if(result<4.75) cout<<"Benign Melanoma lesion"<<endl;
	else if(result>5.45) cout<<"Highly lesion suspicious for melanoma"<<endl;
	else cout<<"Suspicious lesion"<<endl;
	

	// Display diff
	namedWindow("Original Image",WINDOW_NORMAL);
	namedWindow("Cropped Image",WINDOW_NORMAL);
	resizeWindow("Original Image",450,450);
	resizeWindow("Cropped Image",450,450);
	namedWindow("1/2 Image",WINDOW_NORMAL);
	namedWindow("2/2 Image",WINDOW_NORMAL);
	resizeWindow("1/2 Image",450,450);
	resizeWindow("2/2 Image",450,450);
	namedWindow("Border Image",WINDOW_NORMAL);
	resizeWindow("Border Image",450,450);
	imshow( "Original Image",  im );
	imshow( "Cropped Image",  fin);
	imshow( "1/2 Image",  splitted[0]);
	imshow( "2/2 Image",  splitted[1]);
	imshow("Border Image",drawing);

	waitKey();
}
