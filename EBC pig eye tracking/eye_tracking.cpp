/**
 * eye-tracking.cpp:
 * Eye Blink Detection with OpenCV and Odroid exynos_5410_gpio library
 *
 * This program attempts to count the eyeblinks during trial period by correlation
 * of histograms at different time intervals.
 * At startup, the program performs Base image detection by using the webcam, 
 * a callibration. Afterwards it goes on a loop of randomized intertrial periods 
 * and trials. The trial consists of while loop where the histogram during the tone
 * period is compared to the histogram of the base image, where upon a change (based
 * on set threshold) will indicate a blink. The program also auto adjusts its base image
 * as trials go on. It is set to 20 trials.
 *
 * ECE 445 Senior Design project
 * James Sangmok Yun
 */
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
extern "C"{
#include "exynos_5410_gpio.h" 
}
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define EXYNOS_5410_GPIO_REG_BASE_ADDR 0x13400000
// For a given bank of GPIOs, the registers are laid out in memory as: CON DATA UPD DRIVESTR
#define EXYNOS_GPIO_CON_REG_OFFSET 0 //CON (data direction) register
#define EXYNOS_GPIO_DATA_REG_OFFSET 4 // Data register
#define EXYNOS_GPIO_UPD_REG_OFFSET 8 //UPD (pullup/pulldown) register
#define EXYNOS_GPIO_DRIVE_STRENGTH_REG_OFFSET 12 //Drive strength register

#define DISABLE_PULLUP_PULLDOWN 0x0 // No PU or PD
#define ENABLE_PULLUP 0x1 // PU resistor
#define ENABLE_PULLDOWN 0x2 // PD resistor

using namespace std;
using namespace cv;

CascadeClassifier face_cascade;
CascadeClassifier eye_cascade;

int flag = 1;
int flagblink = 0;
typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
	struct timeval now;
	gettimeofday (&now, NULL);
	return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}

void print_all_gpio_info() {
/*
	for(int i = 20; i <= 24; i+=2) {
		printf("Pin %d = %u\n", i, odroid_xu_gpio_read(i));
	}*/
	printf("Pin 20 = %u\n", odroid_xu_gpio_read(20));
	printf("Pin 23 = %u\n", odroid_xu_gpio_read(23));
	printf("Pin 25 = %u\n", odroid_xu_gpio_read(25));
	printf("+++++++\n");
}

Mat calculate_histogram(string const& name, Mat1b const& image)
{
	// Set histogram bins count
	int bins = 256;
	int histSize[] = { bins };

	// Set ranges for histogram bins
	float lranges[] = { 0, 256 };
	const float* ranges[] = { lranges };

	// create matrix for histogram
	Mat hist;
	int channels[] = { 0 };

	// create matrix for histogram visualization
	int const hist_height = 256;
	Mat3b hist_image = Mat3b::zeros(hist_height, bins);

	// calculate histogram for each frame
	calcHist(&image, 1, channels, Mat(), hist, 1, histSize, ranges, true, false);

	double max_val = 0;
	minMaxLoc(hist, 0, &max_val);

	// visualize each bin
	for (int b = 0; b < bins; b++) 
	{
		float const binVal = hist.at<float>(b);
		int const height = cvRound(binVal*hist_height / max_val);
		line(hist_image, Point(b, hist_height - height), Point(b, hist_height), Scalar::all(255));
	}
	
	imshow(name, hist_image);
	return hist;
}

int main(int argc, char** argv)
{

	print_all_gpio_info();
	odroid_xu_gpio_setup_all_output();
	odroid_xu_gpio_write(20, 1); //Turn on LED
	//odroid_xu_gpio_write(22, 0); //initial states tone
	odroid_xu_gpio_write(23, 0); // valve
	odroid_xu_gpio_write(25, 0); //Tone
	print_all_gpio_info();

	int k =0;
	srand(time(NULL));

	// Load the cascade classifiers
	// Make sure you point the XML files to the right path, or 
	// just copy the files from [OPENCV_DIR]/data/haarcascades directory
	face_cascade.load("haarcascade_frontalface_alt2.xml");
	eye_cascade.load("haarcascade_eye.xml");

	//Open webcam
	VideoCapture cap(0);

	int tick = getTickCount(); //tick frequency on win computer is 2338345 clk cycles/sec.
	int desired = 100; //time we're looking to take a frame from the video feed
	//printf("tick = %d\n",tick);
	tick = 0;

	// Check if everything is ok
	if (face_cascade.empty() || eye_cascade.empty() || !cap.isOpened())
	{
		return 1;
	}

	// Set video to 640x480
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	Mat frame, frame2, gray0, gray_freeze;
	Mat baseHist, blinkHist, newbaseHist, gray;
	double diff;

	// Pull the second frame and its histogram to use as a reference
	// to the rest of the video feed
	double seconds_passed = 0, update = 0;
	cap >> frame;
	time_t start = time(0);

	while(cvWaitKey(1)!=27)
	{
		cap >> frame;
		flip(frame, frame, 1);
		cvtColor(frame, gray0, CV_BGR2GRAY);
		imshow("Base Image", gray0);
		baseHist = calculate_histogram("Base Histogram", gray0);
		seconds_passed = difftime(time(0), start);

		if(update!= seconds_passed){
			update = seconds_passed;
			cout << "Initial Calibration = " << 6 - seconds_passed << endl;
		}

		if (seconds_passed == 5)
		{
			printf("CALIBRATE\n");
			break;
		}
	}

	int eyeblinkcount = 0;
	int trialcount = 0;
/******************     Start Session       **********************/
	while (cvWaitKey(1)!=27) //Session while loop
	{
		k = (rand() % 5) + 4;
		//cout << "Intertrial Time Period = " << k << endl;
		//printf("k = %d\n", k);	
		start = time(0);

		trialcount++;
		printf("Trial Count: %d\n", trialcount); 
		odroid_xu_gpio_write(25, 0); //TONE
		/***********     Intertrial Time Period       **************/		
		while(cvWaitKey(1)!=27) 
		{
			cap >> frame;

			if (frame.empty())
				break;

			flip(frame, frame, 1);
			cvtColor(frame, gray, CV_BGR2GRAY);
			imshow("Video", gray);
			calculate_histogram("Video Histogram", gray); 

			seconds_passed = difftime(time(0), start);
			//printf("seconds_passed = %f\n",seconds_passed);

			if(update!= seconds_passed)
			{
				update = seconds_passed;
				//cout << "Seconds Passed = " << seconds_passed << endl;
			}

			if (seconds_passed == k)
			{
				//printf("OUT\n");
				break;
			}
		}

		//push the feed into the frame
		cap >> frame;
		
		if (frame.empty())
			break;

		// Flip the frame horizontally
		flip(frame, frame, 1);

		// Convert to grayscale and adjust the image contrast using histogram equalization
		cvtColor(frame, gray, CV_BGR2GRAY);

		//tick++;
		//cout << tick << endl;
/****************** Recapture Base Image **********************/
		cap >> frame;
		flip(frame, frame, 1);
		cvtColor(frame, gray0, CV_BGR2GRAY);

		newbaseHist = calculate_histogram("Base Histogram", gray0);
		diff = compareHist(baseHist, newbaseHist, CV_COMP_CORREL);
		if(diff <= .7 || diff >= .92)
		{
			baseHist = newbaseHist;
			imshow("Base Image", gray0);
		} 

/******************     Start Tone       **********************/
timestamp_t t0 = get_timestamp();	
	odroid_xu_gpio_write(25, 1);//tone
	//printf("tone\n");
	//print_all_gpio_info();
usleep(100000); //sleep for 100ms
timestamp_t t1 = get_timestamp();
/******************    Start Eye Blink    **********************/
	//start = time(0);
	timestamp_t start = get_timestamp();
		while(cvWaitKey(1)!=27) //Eye blink detection
		{
			cap >> frame2;

			if (frame2.empty())
				break;
			flip(frame2, frame2, 1);
			cvtColor(frame2, gray_freeze, CV_BGR2GRAY);
			blinkHist = calculate_histogram("Tone Histogram", gray_freeze);
			diff = compareHist(baseHist, blinkHist, CV_COMP_CORREL);
			//cout << diff << endl;
		//	printf("Correlation = %f\n",diff);


	/****** Blink detect *****/
			if(diff < .80){
				imshow("Blink Cam", gray_freeze);
				calculate_histogram("Blink Histogram", gray_freeze); 
				if(flagblink==0)
				{ //count eyeblink during phase once.
					eyeblinkcount++;
					flagblink = 1;
				}
			}

       /***** Time stamp  ******/
			timestamp_t end = get_timestamp();
			seconds_passed = (end-start) / 1000000.0L;
			//seconds_passed = difftime(time(0), start);
			if (seconds_passed >= .500) 
			{	//update eyeblink number
				printf("eyeblink #%d\n",eyeblinkcount);
				flagblink = 0;
				//timestamp_t enddetect = get_timestamp();
				break;
			}

		}
timestamp_t blinkend = get_timestamp();
usleep(5000); //sleep for 5ms
/******************     End Blink Detection, Start valve   **********************/
timestamp_t tvalve = get_timestamp();
		odroid_xu_gpio_write(23, 1); //valve
		usleep(50000); //sleep for 50ms

/******************    End valve, tone, end trial ***************************/
		odroid_xu_gpio_write(23, 0); //valve
		odroid_xu_gpio_write(25, 0);//tone
timestamp_t t2 = get_timestamp();
double secs = (t2-t0) / 1000000.0L;
double tonestart = (t0-t0) /1000000.0L;
double detstart = (start-t0) / 1000000.0L;
double valstart = (tvalve - t0) / 1000000.0L;
double detend = (blinkend - t0) / 1000000.0L;

printf("start tone = %f\n", tonestart);
printf("start detection = %f\n", detstart);
printf("end detection = %f\n", detend);
printf("start valve = %f\n", valstart);
printf("end valve,tone,trial = %f\n",secs);

		//print_all_gpio_info();
		if (trialcount >= 20)
		{
			break;
		}
	}

	return 0;
}
