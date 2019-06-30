#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <vector>
#include "assimetry.h"

using namespace std;
using namespace cv;



int main(){
	// Read img
Mat ref= imread("/home/andres/Escritorio/ImageProce/Lab8/dataset/melanoma-skin-cancer.jpg",1);
int x = ref.rows;  //height
int y = ref.cols; //width
const char* datasetB[15]={"/home/andres/NOmelanomaphotos/165021.jpg", "/home/andres/NOmelanomaphotos/170216.jpg", "/home/andres/NOmelanomaphotos/174267.jpg", 
			 "/home/andres/NOmelanomaphotos/194942.jpg", "/home/andres/NOmelanomaphotos/202700-h.jpg", "/home/andres/NOmelanomaphotos/223638.jpg", 
			 "/home/andres/NOmelanomaphotos/232221.jpg", "/home/andres/NOmelanomaphotos/254043.jpg", "/home/andres/NOmelanomaphotos/255780.jpg", 
			 "/home/andres/NOmelanomaphotos/256908-h.jpg", "/home/andres/NOmelanomaphotos/315917.jpg", "/home/andres/NOmelanomaphotos/321466.jpg", 
			 "/home/andres/NOmelanomaphotos/323418.jpg", "/home/andres/NOmelanomaphotos/194946.jpg", "/home/andres/NOmelanomaphotos/324813-h.jpg"};

const char* datasetM[15]={"/home/andres/MelanomaPhotos/408295.jpg", "/home/andres/MelanomaPhotos/432078.jpg", "/home/andres/MelanomaPhotos/503496.jpg", 
			 "/home/andres/MelanomaPhotos/512681-h.jpg", "/home/andres/MelanomaPhotos/521443.jpg", "/home/andres/MelanomaPhotos/550952.jpg", 
			 "/home/andres/MelanomaPhotos/555361-h.jpg", "/home/andres/MelanomaPhotos/561152-h.jpg", "/home/andres/MelanomaPhotos/586355.jpg", 
			 "/home/andres/MelanomaPhotos/608702.jpg", "/home/andres/MelanomaPhotos/610149.jpg", "/home/andres/MelanomaPhotos/625759.jpg", 
			 "/home/andres/MelanomaPhotos/1651872.jpg", "/home/andres/MelanomaPhotos/1657793.jpg", "/home/andres/MelanomaPhotos/1685446.jpg"};

for(int numero=0; numero<15; numero++)
{
	cout<<"======================================================================"<<endl;
	cout<<"           "<<endl;
	cout<<numero<<endl;
	cout<<"           "<<endl;
	cout<<"======================================================================"<<endl;
	const char* Variable="/home/andres/Escritorio/ImageProce/Lab8/dataset/melanoma-skin-cancer.jpg";
	
	Mat im2= imread(datasetB[numero],1);
	Mat im;	
	resize(im2,im,Size(x,y));	
	//cout<<im.size().width <<endl;
	//cout<<im.size().height <<endl;
	Mat img;
	cvtColor(im,img,CV_RGB2GRAY);
	Mat croppedImg;
	
	Mat imageseg=segmented(im);
	erode(imageseg,imageseg,getStructuringElement(MORPH_ELLIPSE,Size(19,19)));
	Mat bincanny;
	Canny(imageseg,bincanny,20,100,3);
	Mat images1[2]={imageseg,bincanny};
	Mat complete1;
	hconcat(images1,2,complete1);
	namedWindow("Original Image",WINDOW_NORMAL);
	resizeWindow("Original Image",450,450);
	/*imshow( "Original Image",  im );*/
	namedWindow("Original - Canny",CV_WINDOW_NORMAL);
	resizeWindow("Original - Canny",600,600);	
	/*imshow("Original - Canny",complete1);*/
	//waitKey(0);
	
	threshold(img,croppedImg,110,255,THRESH_BINARY_INV);
	//croppedImg = otsu(img);	
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
	Mat one=splitted[0];Mat two=splitted[1];

	/*cout<<"Area: "<<area<<endl;
	cout<<"Perimeter: "<<perimeter<<endl;
	cout<<"Assimetry: "<<Ass<<endl;
	cout<<"Border: "<<bord<<endl;
	cout<<"Color: "<<color<<endl;
	cout<<"Diameter: "<<diam<<endl;*/

	float result=Ass*1.3+bord*0.1+color*0.5+diam*0.5;

	cout<<"-----------RESULT------------"<<endl;
	
	cout<<"TDS: "<<result<<endl;
	if(result<5.45) cout<<"Benign Melanoma lesion"<<endl;
	else if(result>6.0) cout<<"Highly lesion suspicious for melanoma"<<endl;
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
	//namedWindow("Border Image",WINDOW_NORMAL);
	//resizeWindow("Border Image",450,450);
	/*imshow( "Original Image",  im );
	imshow( "Cropped Image",  fin);
	imshow( "1/2 Image",  splitted[0]);
	imshow( "2/2 Image",  splitted[1]);*/
	//imshow("Border Image",drawing);
}

	//waitKey();
}








