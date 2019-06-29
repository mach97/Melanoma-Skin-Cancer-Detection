#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;


cv::Point findcentroid(Mat image,int num)
{
	int sumx=0;int sumy=0;
	for(int i=0;i<image.rows;i++)
		for(int j=0;j<image.cols;j++)
		{
			if(image.at<uchar>(i,j)==255)
			{
				sumx+=i;sumy+=j;
			}				
		}
	int cx,cy;	
	cx=sumx/num;
	cy=sumy/num;
	return Point(cx,cy);
}
int extractnpixels(Mat image){
	int npixels=0;
	for(int i = 0; i < image.rows; i++)
  	{
   		 for(int j = 0; j < image.cols ; j++)
  		 {
		      if(image.at<uchar>(i,j)==255){
       			npixels+=1;}}}
	return npixels;}	

float extractGD(Mat image,int area)
{	Point ptcentr=findcentroid(image,area);
	int maxi=0;
	for(int i = 0; i < image.rows; i++)
  	{
   		 for(int j = 0; j < image.cols ; j++)
  		 {
		      if(image.at<uchar>(i,j)==255){
				Point xy(i,j);
				double res = cv::norm(ptcentr-xy);
				if (res>maxi){
					maxi=res;}
       			}
		 }
	}
	
	return maxi*2;
}


float extractSD(Mat image,int area)
{	Point ptcentr=findcentroid(image,area);
	int mini=1000;
	for(int i = 0; i < image.rows; i++)
  	{
   		 for(int j = 0; j < image.cols ; j++)
  		 {
		      if(image.at<uchar>(i,j)==255){
				Point xy(i,j);
				double res = cv::norm(ptcentr-xy);
				if (res<mini && res>0){
					mini=res;}
       			}
		 }
	}
	
	return mini*2;
}

int posy(Mat im){
	for(int i=0;i<im.rows;i++)
		for(int j=0;j<im.cols;j++)
			if(im.at<uchar>(i,j)==255)
				return i;
	return -1;
}

int posx(Mat im){
	for(int i=0;i<im.cols;i++)
		for(int j=0;j<im.rows;j++)
			if(im.at<uchar>(j,i)==255)
				return i;
	return -1;
}

int posyMax(Mat im){
	for(int i=im.rows;i>=0;i--)
		for(int j=im.cols;j>=0;j--)
			if(im.at<uchar>(i,j)==255)
				return i;
	return -1;
}

int posxMax(Mat im){
	for(int i=im.cols;i>=0;i--)
		for(int j=im.rows;j>=0;j--)
			if(im.at<uchar>(j,i)==255)
				return i;
	return -1;
}
	
Mat recortada(Mat croppedImg){
	int y=posy(croppedImg);
	int x=posx(croppedImg);
	int yMax=posyMax(croppedImg);
	int xMax=posxMax(croppedImg);

	Rect myR(x,y,xMax-x,yMax-2*y);

	Mat fin;
	croppedImg(myR).copyTo(fin);
	return fin;
}

Mat outerborder(Mat fin){
	int idx = -1;
	double maxArea = 0.0;
	vector<vector<Point> > contours;
  	vector<Vec4i> h;
	findContours(fin, contours,h, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE,Point(0,0));

	 Mat drawing = Mat::zeros(fin.size(), CV_8UC3);
	 for( int i = 0; i< contours.size(); i++ ) // iterate through each contour. 
      	{
 	 double area = cv::contourArea(contours[i]);
  	if( area > maxArea){
         	maxArea = area;
         	idx = i;
}
}
	Scalar color( 255,255,255);
	drawContours( drawing, contours,idx, color,0, 8, h); 
	return drawing;
	
}

float dividir(Mat img,Mat splitted[2],float area){
	splitted[0]=img(Rect(0,0,img.cols,img.rows/2));
	splitted[1]=img(Rect(0,img.rows/2+1,img.cols,img.rows/2-2));
	int cont1=0,cont2=0;
	for(int i=0;i<splitted[0].rows;i++)
		for(int j=0;j<splitted[0].cols;j++)
			if(splitted[0].at<uchar>(i,j)==255)
				cont1++;

	for(int i=0;i<splitted[1].rows;i++)
		for(int j=0;j<splitted[1].cols;j++)
			if(splitted[1].at<uchar>(i,j)==255)
				cont2++;
	cout<<cont1<<" , "<<cont2<<endl;
	

	return abs(cont1-cont2)/area*10;

}

float border(float area, float perimeter){
	return pow(perimeter,2)/(4*22.7*area);
}

int detectar(Mat img_rgb,float area){
	int cont=0;
	float res=area*2/100;
  Mat hsv;
	cvtColor(img_rgb, hsv, CV_BGR2HSV);
  Mat1b cwithe,cblack,cred,cblue,clbrown,cdbrown,cgray ;

  inRange(img_rgb, Scalar(0, 0, 212), Scalar(131, 255, 255), cwithe);
  //inRange(img_rgb, Scalar(0, 0, 212), Scalar(131, 255, 255), cwithe);
  float contwhite= extractnpixels(cwithe);
	if(contwhite>=res)
		cont++;
  inRange(hsv, Scalar(0,0,0), Scalar(50,50,100), cblack);
  float contblack= extractnpixels(cblack);
	if(contblack>=res)
		cont++;  
	
  inRange(img_rgb, Scalar(17, 15, 100), Scalar(50, 56, 200), cred);
  float contred= extractnpixels(cred);
if(contred>=res)
		cont++;
  inRange(img_rgb, Scalar(103, 86, 65), Scalar(145, 133, 128), cgray);
  float contgray= extractnpixels(cgray);
	if(contgray>=res)
		cont++;
  inRange(hsv, Scalar(2, 100, 65), Scalar(12, 170, 100), clbrown);
  float contlbrown= extractnpixels(clbrown);
	if(contlbrown>=res)
		cont++;
  inRange(hsv, Scalar(170, 100, 20), Scalar(180, 255, 255), cdbrown);
  float contdbrown= extractnpixels(cdbrown);
	if(contdbrown>=res)
		cont++;

	return cont;
}


