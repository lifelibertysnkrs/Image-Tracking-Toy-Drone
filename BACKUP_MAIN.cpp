#include "ColorViewer.h"
//#include "ContentFinder.h"
#include "HSVHistogram.h"
//#include "HSVSlider.h"
#include <iostream>
using namespace std;

ColorViewer ColorTracker;
HSVHistogram Hist;
//HSVSlider Slider;
//ContentFinder Finder;

cv::Mat croppedImage;
cv::Mat thresholded_hue;

cv::Rect boundingRect;
cv::Point point1, point2;
bool clicked = false;
cv::Mat image;

string imageName;

//bool histValuesPrinted = false;

void calibrateBoundingRect(cv::Rect& rectangle) {
	rectangle.x += 1;
	rectangle.y += 1;
	rectangle.height -= 2;
	rectangle.width -= 2;
}
void CallBackFunc(int evnt, int x, int y, int flags, void* userdata) {
	if (evnt == cv::EVENT_LBUTTONDOWN) {
		boundingRect = cv::Rect(0,0,0,0);
		point1 = cv::Point(x,y);
		clicked = true;
		cv::destroyWindow("Cropped");
		cv::destroyWindow("Thresholded Hue");
		cv::destroyWindow(ColorTracker.getColorSquareWindowName());
		croppedImage.release();
		thresholded_hue.release();
	}
	if (evnt == cv::EVENT_MOUSEMOVE) {
		if (x < 0) x = 0;
		else if (x > image.cols) x = image.cols;
		if (y < 0) y = 0;
		else if (y > image.rows) y = image.rows;
		point2 = cv::Point(x,y);
		if (clicked) {
			boundingRect = cv::Rect(point1,point2);
		}
		cv::imshow(imageName,image);
	}
	if (evnt == cv::EVENT_LBUTTONUP) {
		clicked = false;
		//calibrateBoundingRect(boundingRect);
		if (boundingRect.area() != 0) {
			calibrateBoundingRect(boundingRect);
			croppedImage = image(boundingRect);
			cv::imshow("Cropped", croppedImage);
		}
		else {
			boundingRect = cv::Rect(point1-cv::Point(4,4),point1+cv::Point(4,4));
			croppedImage = image(boundingRect);
			cv::imshow("Cropped", croppedImage);
			//cout << "size: " << boundingRect.area() << endl;
		}
		//histValuesPrinted = false;
    }
}
void processMouseActions() {
	cv::setMouseCallback(imageName,CallBackFunc,NULL);
}
void showHistograms() {
	cv::imshow("Hue Histogram", Hist.getHueHistogramImage(croppedImage));
	cv::imshow("Sat Histogram", Hist.getSatHistogramImage(croppedImage));
	cv::imshow("Val Histogram", Hist.getValHistogramImage(croppedImage));
}

int main() {
//	bool sliderActivated = false;
//	if (sliderActivated) {
//		Slider.createColorSquare();
//		Slider.activateTrackBars();
//	}
	ColorTracker.createColorSquare();
	while (cv::waitKey(10) != 13) {
		image = cv::imread("E:\\Libraries\\Pictures\\MouseGlove.png");
		if (clicked) {
			cv::rectangle(image,boundingRect,cv::Scalar(255,255,255),1);
		}
		if(!croppedImage.empty()) {
			showHistograms();
			//Finder.setHueHistogram(Hist.getHueHistogram(croppedImage));
			//thresholded_hue = Finder.find_hue(image);
			//cv::imshow("Thresholded Hue", thresholded_hue);
			//if (!histValuesPrinted) {
			//	histValuesPrinted = true;
			//}
			int hue = Hist.getMostAbundantHue(Hist.getHueHistogram(croppedImage));
			int sat = Hist.getMostAbundantSat(Hist.getSatHistogram(croppedImage));
			int val = Hist.getMostAbundantVal(Hist.getValHistogram(croppedImage));
			ColorTracker.setColorSquareHSV(hue, sat, val);
		}
		imageName = "Image";
		cv::imshow(imageName,image);
		processMouseActions();
//		if (sliderActivated) {
//			Slider.updateColorSquare();
//		}
	}
}