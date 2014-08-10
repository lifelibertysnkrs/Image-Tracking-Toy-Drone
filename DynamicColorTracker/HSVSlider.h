#ifndef HSV_SLIDER_H
#define HSV_SLIDER_H
#include "OpenCVIncludes.h"

class HSVSlider {
private:
	cv::Mat colorSquare;
	std::string colorSquareWindowName;
	int h_val;
	int s_val;
	int v_val;
	bool trackBarsActive;
public:
	HSVSlider() {
		colorSquareWindowName = "HSV Slider";
		trackBarsActive = false;
	}
	void createColorSquare() {
		colorSquare = cv::Mat::zeros(100,500,CV_8UC3);
		setColorSquare(100,100,100);
		cv::cvtColor(colorSquare,colorSquare,CV_HSV2BGR);
		cv::imshow(colorSquareWindowName,colorSquare);
	}
	void setColorSquare(int H, int S, int V) {
		colorSquare.setTo(cv::Scalar(H,S,V));
		cv::cvtColor(colorSquare,colorSquare,CV_HSV2BGR);
		cv::imshow(colorSquareWindowName,colorSquare);
	}
	std::string getColorSquareWindowName() {
		return colorSquareWindowName;
	}
	cv::Mat getColorSquare() {
		return colorSquare;
	}
	cv::Scalar getColorSquareHSV() {
		return cv::Scalar(h_val,s_val,v_val);
	}
	void activateTrackBars() {
		cv::createTrackbar("H", getColorSquareWindowName(), &h_val, 179);
		cv::createTrackbar("S", getColorSquareWindowName(), &s_val, 255);
		cv::createTrackbar("V", getColorSquareWindowName(), &v_val, 255);
		cv::setTrackbarPos("H", getColorSquareWindowName(), 180/2);
		cv::setTrackbarPos("S", getColorSquareWindowName(), 256/2);
		cv::setTrackbarPos("V", getColorSquareWindowName(), 256/2);
	}
	void updateColorSquare() {
		setColorSquare(h_val,s_val,v_val);
	}
};

#endif HSV_SLIDER_H