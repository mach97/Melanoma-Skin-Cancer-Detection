#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

void generateHistogram(Mat image, float histogram[])
{
	for (int i=0;i<256;i++)
		histogram[i]=0;
	for (int row=0; row<image.rows; row++)
		for (int col=0; col<image.cols; col++)
			histogram[(int)image.at<uchar>(row,col)]++;
	
}

void displayHistogram(float histogram[], const char* windowName)
{
	float histTmp[256];
	for (int i=0; i<256; i++)
		histTmp[i]=histogram[i];
	int hist_w =500; int hist_h=500;
	int bin_w = cvRound( (double) hist_w/256);
	cv::Mat histImage(hist_h, hist_w, CV_8UC1, Scalar (255,255,255));
	float max=histTmp[0];
	for (int i =1; i<256; i++)
		if (max <histTmp[i])
			max=histTmp[i];
	for (int i=0; i<256; i++)
		histTmp[i] = ( (double) histTmp[i]/max) * histImage.rows;
	for (int i=0; i<256; i++)
		line(histImage, Point(bin_w * i, hist_h),
				    Point(bin_w * i , hist_h-histTmp[i]), 
				    Scalar(0,0,0),
				    1,8,0);
	//DISPLAY HISTOGRAM
 	namedWindow( windowName, cv::WINDOW_NORMAL );
	resizeWindow( windowName, 450,450);
  	imshow( windowName, histImage );

}

Mat otsu(Mat image)
{
	int w,h,pos,OPT_TH;
	float hist_val[256],p1K,meanitr_K,meanitr_G,param1,param2,param3,ihist[256];
	h=image.rows;
	w=image.cols;  
	generateHistogram(image,ihist);
	//displayHistogram(ihist, "Original Histogram");
	p1K=0;meanitr_K=0;meanitr_G=0;OPT_TH=0;
	
	//Normalize histogram
	for(int i=0;i<256;i++)
	{
		hist_val[i]=(float)(ihist[i])/(float)(w*h);
		meanitr_G+=(float)(i*hist_val[i]);
	}
	
	//OTSU
	Mat imageRes;
	for(int i=0;i<255;i++)
	{
		p1K+=(float)hist_val[i];
		meanitr_K+=(float)(i * hist_val[i]);
		param1 = (float)(meanitr_G * p1K) - meanitr_K;
		param2 = (float)(param1*param1)/(float)p1K*(1-p1K);
        	if(param2>param3)
		{
			param3=param2;
			OPT_TH=i;
        	}
      	}	
	//printf("Optimal value by Otsu = %d\n",OPT_TH);
	threshold(image,imageRes,OPT_TH,255,0);
	return imageRes;
}

Mat segmented(Mat img_rgb)
{
	Mat img_hsv,eqv,hsveq,out;
	cvtColor(img_rgb,img_hsv,CV_RGB2HSV);
	
	vector<cv::Mat> channels;
	split(img_hsv, channels);
	//equalizeHist(channels[1], eqv);
	//channels[1]=eqv;
	//equalizeHist(channels[2], eqv);
	//channels[2]=eqv;
	equalizeHist(channels[0], eqv);
	channels[0]=eqv;
	merge(channels,hsveq);
	cvtColor(hsveq,out,CV_HSV2RGB);
	cvtColor(out,out,CV_RGB2GRAY);
	Mat image_blurred;
    	GaussianBlur(out, image_blurred, Size(11,11), 0);
	Mat binary=otsu(image_blurred);		
	//threshold( out, binary, 0, 255,CV_THRESH_OTSU);
    	return binary;
}


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
int extractnpixels(Mat image)
{
	int npixels=0;
	for(int i = 0; i < image.rows; i++)
  	{
		for(int j = 0; j < image.cols ; j++)
  		{
		      if(image.at<uchar>(i,j)==255)
       			npixels+=1;
		}
	}
	return npixels;
}	

float extractGD(Mat image,int area)
{	Point ptcentr=findcentroid(image,area);
	int maxi=0;
	for(int i = 0; i < image.rows; i++)
  	{
   		 for(int j = 0; j < image.cols ; j++)
  		 {
		      if(image.at<uchar>(i,j)==255)
			{
				Point xy(i,j);
				double res = cv::norm(ptcentr-xy);
				if (res>maxi)
					maxi=res;
       			}
		 }
	}
	
	return maxi*2;
}


float extractSD(Mat image,int area)
{	
Point ptcentr=findcentroid(image,area);
	int mini=1000;
	for(int i = 0; i < image.rows; i++)
  	{
   		 for(int j = 0; j < image.cols ; j++)
  		 {
		     	if(image.at<uchar>(i,j)==255)
			{
				Point xy(i,j);
				double res = cv::norm(ptcentr-xy);
				if (res<mini && res>0)
					mini=res;
       			}
		 }
	}
	
	return mini*2;
}

int posy(Mat im)	
{
	for(int i=0;i<im.rows;i++)
		for(int j=0;j<im.cols;j++)
			if(im.at<uchar>(i,j)==255)
				return i;
	return -1;
}

int posx(Mat im)
{
	for(int i=0;i<im.cols;i++)
		for(int j=0;j<im.rows;j++)
			if(im.at<uchar>(j,i)==255)
				return i;
	return -1;
}

int posyMax(Mat im)
{
	for(int i=im.rows;i>=0;i--)
		for(int j=im.cols;j>=0;j--)
			if(im.at<uchar>(i,j)==255)
				return i;
	return -1;
}

int posxMax(Mat im)
{
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

Mat outerborder(Mat fin)
{
	int idx = -1;
	double maxArea = 0.0;
	vector<vector<Point> > contours;
  	vector<Vec4i> h;
	findContours(fin, contours,h, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE,Point(0,0));

	Mat drawing = Mat::zeros(fin.size(), CV_8UC3);
	for( int i = 0; i< contours.size(); i++ ) 
      	{
 	 	double area = cv::contourArea(contours[i]);
  		if( area > maxArea)
		{
         		maxArea = area;
         		idx = i;
		}
	}
	Scalar color( 255,255,255);
	drawContours( drawing, contours,idx, color,0, 8, h); 
	return drawing;
	
}

float dividir(Mat img,Mat splitted[2],float area)
{
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

	

	return abs(cont1-cont2)/area*10;

}

Mat invers(Mat src, Mat dst)
{
	for(int i=0;i<src.rows;i++)
		for(int j=0;j<src.cols;j++)
			if(src.at<uchar>(i,j)==0)
				dst.at<uchar>(i,j)=1;
			else
				dst.at<uchar>(i,j)=0;
}

float border(float area, float perimeter){
	return pow(perimeter,2)/(4*22.7*area);
}

int detectar(Mat img_rgb,float area)
{
	int cont=0;
	float res=area*2/100;
	Mat hsv;
	bool values[6]={0,0,0,0,0,0};
	//char* w[6]={"WHITE","BLACK","RED","GRAY","LIGHT BROWN","DARK BROWN"};
	cvtColor(img_rgb, hsv, CV_BGR2HSV);
	Mat cwithe,cblack,cred,cblue,clbrown,cdbrown,cgray ;
	//WHITE COLOR
  	inRange(img_rgb, Scalar(0, 0, 212), Scalar(131, 255, 255), cwithe);
	float contwhite= extractnpixels(cwithe);
	if(contwhite>=res)
	{
		cont++;
		values[0]=1;	
	}
	//BLACK COLOR
	inRange(hsv, Scalar(0,0,0), Scalar(50,50,100), cblack);
  	float contblack= extractnpixels(cblack);
	if(contblack>=res)
	{
		cont++;
		values[1]=1;
	}  
	//RED COLOR
	inRange(img_rgb, Scalar(17, 15, 100), Scalar(50, 56, 200), cred);
	float contred= extractnpixels(cred);
	if(contred>=res)
	{
		cont++;
		values[2]=1;
	}
	//GRAY COLOR
	inRange(img_rgb, Scalar(103, 86, 65), Scalar(145, 133, 128), cgray);
	float contgray= extractnpixels(cgray);
	if(contgray>=res)
	{
		cont++;
		values[3]=1;
	}
  	//LIGHT BROWN
	inRange(hsv, Scalar(2, 100, 65), Scalar(12, 170, 100), clbrown);
  	float contlbrown= extractnpixels(clbrown);
	if(contlbrown>=res)
	{
		cont++;
		values[4]=1;	
	}
	//DARK BROWN
	inRange(hsv, Scalar(170, 100, 20), Scalar(180, 255, 255), cdbrown);
  	float contdbrown= extractnpixels(cdbrown);
	if(contdbrown>=res)
	{
		cont++;
		values[5]=1;
	}
	//printf("Colores Encontrados:\t");
	/*for(int i=0;i<6;i++)
		if(values[i]==1)
			printf("%s\t",w[i]);*/
	
	Mat images1[3]={cwithe,cblack,cred};
	Mat images2[3]={cgray,clbrown,cdbrown};
	Mat complete1;
	hconcat(images1,3,complete1);
	Mat complete2;
	hconcat(images2,3,complete2);
	Mat image3[2]={complete1,complete2};	
	Mat complete3;
	vconcat(image3,2,complete3);
	namedWindow("Colors",CV_WINDOW_NORMAL);
	resizeWindow("Colors",600,600);	
	imshow("Colors",complete3);

	return cont;


}


