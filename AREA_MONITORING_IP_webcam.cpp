#include <iostream>
#include <Windows.h>
#include <opencv2\opencv.hpp>`
#include <opencv2\videoio.hpp>

#define REGION_OF_INTREST_FLAG			1	// if need region of Interst on image 	
#define CAMERA_DISTANCE_FROM_AREA		20	// in meters
#define DEFAULT_CONTOURS_SIZE			60 
//#define CONTOURS_SIZE_SCALE				(60 * (CAMERA_DISTANCE_FROM_AREA/20)

#define X		360		// spacify the Top Left X-cordinate of Region of interest  
#define Y		50		// spacify the Top Left Y-cordinate of Region of interest  
#define width	340		// spacify the Width pixels of Region of interest  
#define	height	400		// spacify the Height pixels of Region of interest  

using namespace std;
using namespace cv;

/* Routine to change text color */
void textcolor(int color)
{
	static int __BACKGROUND;

	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbiInfo;


	GetConsoleScreenBufferInfo(h, &csbiInfo);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
		color + (__BACKGROUND << 4));
}

/* Start of main */
void main()
{
	/* IP_ webcam URL to access camera */
	char *url = "http://192.168.43.1:8080/video";  //replace with your Webcam url

	cv::VideoCapture cap;

	Mat frame, fgMask;
	short flag = 0;
	/* Open IP webcamera */
	try {
		if (!cap.open(0))		// if you want to access PC webcam just replace it with 0
		{
			printf("Error Opening Capture card!, check your webcam URL!");
			throw "Cap Error";
		}
	}
	catch (char *s)
	{
		cout << s << endl;
	}
	/* check is Webcamera status */
	int FrameNo = 0;
	if (!cap.isOpened())
	{
		cout << "Error in opening Web Camera!" << endl;
	}
	/* Clear screen */
	system("CLS");
	/* Change the display text color */
	textcolor(1);
	cout << "\n\n\tOBJECT DETECTION IN RISTRICTED AREA!\n";
	textcolor(5);
	cout << "\tDONE BASED ON BACKGROUND SUBSTRACTION AND \n\tFILTERING OF RISTRICTED AREA!\n";
	textcolor(2);
	cout << "\tCONSOL INDICATEDS NUMBER OF FRAME OBJECT DETECTED\n";
	
	Mat bg_image,bg_gray;
	/* Read Background frame, skip initial 29 frames due to well setel of camera parameters */
	for(short i =0; i < 30; i++)
		cap.read(bg_image);

	flip(bg_image, bg_gray, 1);
	/* Convert BGR image to gray image */
	cvtColor(bg_gray, bg_gray, COLOR_BGR2GRAY);

	/* If you selected region of interest to be monitored */
	{ /* This portion will provide background image area with region of interest */
#if REGION_OF_INTREST_FLAG	
		Mat tempImg = Mat::zeros(Size(bg_gray.cols, bg_gray.rows), CV_8UC1);
		/* Draw rectangle on Image */
		cv::rectangle(tempImg, Rect(X, Y, width, height), Scalar(255), CV_FILLED);
		/* Masking image with Region of Interest */
		cv::bitwise_and(tempImg, bg_gray, bg_gray);
#endif
	}

	/* Real time monitoring of area */
	int cnt = 0;
	while (1)
	{
		Mat gray, difference, thresh;
	
		try {
			/* Read Frame */
			cap.read(frame);
		}
		catch (cv::Exception)
		{
			cout << "frames reading Error!" << endl;
			return;
		}

		/* Flip image with X-axis to match real view */
		flip(frame, frame, 1);
		/* Convert BGR to Gray image */
		cvtColor(frame, gray, COLOR_BGR2GRAY);

		{ /* This portion will provide background image area with region of interest */
#if REGION_OF_INTREST_FLAG
		/* Draw Rectangle on image */
			cv::rectangle(frame, Rect(X, Y, width, height), Scalar(0, 255, 0), 2);
			Mat tempGray = Mat::zeros(frame.size(), CV_8UC1);
			/* Get a mask of region image area */
			cv::rectangle(tempGray, Rect(X, Y, width, height), Scalar(255), CV_FILLED);
			/* Mask real time image to get Region of Intrest */
			cv::bitwise_and(tempGray, gray, gray);
#endif
		}

		/* In each iteration, calculate absolute difference between current frame and reference frame */
		absdiff(gray, bg_gray, difference);

		/* Canny edge detection at white pixels */
		Mat CannnyOut;
		Canny(difference, CannnyOut, 254, 255);

		vector<vector<Point>> contours, sorted_counters;
		vector<Point> cnts;

		/* Fing contours on image */
		findContours(CannnyOut, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

		Mat drawing = Mat::zeros(CannnyOut.size(), CV_8UC3);
		flag = 0;
		/* Predict the object based on contours size */
		if (contours.size() > 0)
		{
			for (int i = 0; i < contours.size(); i++)
			{
				if (contours[i].size() > DEFAULT_CONTOURS_SIZE) //60
				{
					drawContours(drawing, contours, i, Scalar(255, 0, 255), 1, 8);
					drawContours(frame, contours, i, Scalar(0, 0, 255), 3, 8);
					/* Over lay text on image indicating Object detected */
					putText(frame, "* Object Detected", Point(390, 25), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 255), 1, LINE_AA);
					putText(frame, "In Restricted Area", Point(390, 45), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 255), 1, LINE_AA);
					/* Display the image */
					imshow("FramesStreamOverIP", frame);
					waitKey(1);
					flag = 1;
					printf("Alert Object is detected : %d!!!\n", FrameNo++);
				}
			}
		}
	
		/* This logic helps to blink screen when No object detected */
		if (flag == 0)
		{   /* Displaying at every alternative frame */
			if (cnt % 2 != 0)
			{	
				putText(frame, "* NO Objects", Point(410, 40), FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 255, 0), 1, LINE_AA);
			}
			imshow("FramesStreamOverIP", frame);
			waitKey(1);
		}

		cnt++;
		//get the input from the keyboard
		int keyboard = waitKey(30);
		if (keyboard == 'q' || keyboard == 27)
			break;
	}
}









#if 0


#include <iostream>
#include <opencv2\opencv.hpp>`
#include <opencv2\videoio.hpp>
using namespace std;
using namespace cv;

char *url = "http://192.168.0.106:4747/video";  //4747
cv::VideoCapture cap(url);
Mat frame, fgMask;
void main()
{
	int FrameNo = 0;

	Ptr<BackgroundSubtractor> pBackSub;
	if (1) //parser.get<String>("algo") == "MOG2"
		pBackSub = createBackgroundSubtractorMOG2();
	else
		pBackSub = createBackgroundSubtractorKNN();

	if (!cap.isOpened())
	{
		cout << "Error!" << endl;
	}

	Mat first_gray = imread("D:\\Visual Studio Projects\\Timer thread\\Timer thread\\Frames\\frameBG_825.jpg", 0); //1459
	//cvtColor(frame, gray, COLOR_BGR2GRAY);

	cap.set(cv::CAP_PROP_FRAME_WIDTH, 720);
	cap.set(cv::CAP_PROP_FRAME_HEIGHT, 576);

	while (1)
	{
		Mat gray, difference, thresh;
		cap.read(frame);
		resize(frame, frame, Size(720, 576));
		//frame = imread("D:\\Visual Studio Projects\\Timer thread\\Timer thread\\Frames\\frameBG_1459.jpg", 1); //1459

		//update the background model
		//pBackSub->apply(frame, fgMask);
		
		cvtColor(frame, gray, COLOR_BGR2GRAY);
			

		//In each iteration, calculate absolute difference between current frame and reference frame
		absdiff(gray, first_gray, difference);

		// Apply thresholding to eliminate noise
		threshold(difference, thresh, 25, 255, THRESH_BINARY);
		dilate(thresh, thresh, NULL);

		//show the current frame and the fg masks
		imshow("Frame", thresh);
//		imshow("FG Mask", fgMask);
#if 0	
		FrameNo++;
		char extention[12];
		_itoa(FrameNo, extention, 10);
		strcat(extention, ".jpg");
		resize(frame, frame, Size(720, 576));	
		char frm[250] = "D:\\Visual Studio Projects\\Timer thread\\Timer thread\\Frames\\frameBG_";
		strcat(frm, extention);
		imwrite(frm, frame);
#endif
		imshow("Image", frame);
		//get the input from the keyboard
		int keyboard = waitKey(30);
		if (keyboard == 'q' || keyboard == 27)
			break;
	}


}
#endif