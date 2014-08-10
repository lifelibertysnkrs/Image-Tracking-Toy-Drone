#include "ColorViewer.h"
#include "HSVHistogram.h"
#include "ObjectTracker.h"
#include <iostream>
#include <fstream>
using namespace std;

// Camera
cv::VideoCapture camera(0);

// Objects
ColorViewer ColorTracker;
HSVHistogram Hist;
ObjectTracker TargetTracker;

// Mat
cv::Mat targetImage;
cv::Mat image;

// Boxes
cv::Rect boundingRect;
cv::Point point1, point2;

// Booleans
bool mouseButtonDown = false;
bool targetSelected = false;
bool HSVDefined = false;

// Window Names
string imageName = "Video";
string targetName = "Target";

// Colors
cv::Scalar tealColor = cv::Scalar(255,255,0);
cv::Scalar redColor = cv::Scalar(0,0,255);
cv::Scalar blackColor = cv::Scalar(0,0,0);
cv::Scalar yellowColor = cv::Scalar(0,255,255);
cv::Scalar greenColor = cv::Scalar(0,255,0);

cv::Rect calibratedRect(const cv::Rect rect) {
	cv::Rect rect_;
	int x_padding = max(int(0.2*rect.width), 1);
	int y_padding = max(int(0.2*rect.height), 1);
	rect_.x = rect.x + x_padding;
	rect_.y = rect.y + y_padding;
	rect_.width = rect.width - 2*x_padding;
	rect_.height = rect.height - 2*y_padding;
	return rect_;
}
void CallBackFunc(int evnt, int x, int y, int flags, void* userdata) {
	if (evnt == cv::EVENT_LBUTTONDOWN) {
		mouseButtonDown = true;
		targetSelected = false;
		boundingRect = cv::Rect(0,0,0,0);
		point1 = cv::Point(x,y);
		cv::destroyWindow(targetName);
		cv::destroyWindow(ColorTracker.getColorSquareWindowName());
		targetImage.release();
	}
	if (evnt == cv::EVENT_MOUSEMOVE) {
		if (x < 0) x = 0;
		else if (x > image.cols) x = image.cols;
		if (y < 0) y = 0;
		else if (y > image.rows) y = image.rows;
		point2 = cv::Point(x,y);
		if (mouseButtonDown) {
			boundingRect = cv::Rect(point1,point2);
		}
		cv::imshow(imageName,image);
	}
	if (evnt == cv::EVENT_LBUTTONUP) {
		mouseButtonDown = false;
		if (boundingRect.area() != 0) {
			targetImage = image(calibratedRect(boundingRect));
			cv::imshow(targetName, targetImage);
		}
		else {
			boundingRect = cv::Rect(point1-cv::Point(5,5),point1+cv::Point(5,5));
			targetImage = image(calibratedRect(boundingRect));
			cv::imshow(targetName, targetImage);
		}
		targetSelected = true;
    }
}
void processMouseActions() {
	cv::setMouseCallback(imageName,CallBackFunc,NULL);
}
void showHistograms() {
	cv::imshow("Hue Histogram", Hist.getHueHistogramImage(targetImage));
	cv::imshow("Sat Histogram", Hist.getSatHistogramImage(targetImage));
	cv::imshow("Val Histogram", Hist.getValHistogramImage(targetImage));
}

// Quadrant Drawing
int frameWidth = int(camera.get(CV_CAP_PROP_FRAME_WIDTH));
int frameHeight = int(camera.get(CV_CAP_PROP_FRAME_HEIGHT));
int centerRectSize = 50;
cv::Point centerPoint = cv::Point(frameWidth/2, frameHeight/2);
cv::Rect centerRectangle = cv::Rect(cv::Point(centerPoint.x-centerRectSize, centerPoint.y-centerRectSize),
	cv::Point(centerPoint.x+centerRectSize, centerPoint.y+centerRectSize));
bool targetInQ1 = false;
bool targetInQ2 = false;
bool targetInQ3 = false;
bool targetInQ4 = false;
bool targetCentered = false;

void drawCenterBox(int thickness=1) {
	cv::rectangle(image, centerRectangle, redColor, thickness);
}
void drawCenterDot() {
	cv::circle(image, centerPoint, 3, redColor, -1);
}
void highlightQuadrant(int quadrantNum) {
	if (quadrantNum == 1) {
		cv::rectangle(image, cv::Point(centerPoint.x,0), cv::Point(frameWidth, centerPoint.y), yellowColor, 2);
		drawCenterDot();
		drawCenterBox();
	}
	else if (quadrantNum == 2) {
		cv::rectangle(image, cv::Point(0,0), centerPoint, yellowColor, 2);
		drawCenterDot();
		drawCenterBox();
	}
	else if (quadrantNum == 3) {
		cv::rectangle(image, cv::Point(0, centerPoint.y), cv::Point(centerPoint.x, frameHeight), yellowColor, 2);
		drawCenterDot();
		drawCenterBox();
	}
	else if (quadrantNum == 4) {
		cv::rectangle(image, cv::Point(centerPoint.x,centerPoint.y), cv::Point(frameWidth, frameHeight), yellowColor, 2);
		drawCenterDot();
		drawCenterBox();
	}
}
void highlightAllQuadrants() {
	cv::line(image, cv::Point(0, centerPoint.y), cv::Point(frameWidth, centerPoint.y), greenColor, 2);
	cv::line(image, cv::Point(centerPoint.x, 0), cv::Point(centerPoint.x, frameHeight), greenColor, 2);
	drawCenterDot();
	drawCenterBox(2);
}
void drawQuadrants(cv::Point targetCoord) {
	cv::line(image, cv::Point(0, centerPoint.y), cv::Point(frameWidth, centerPoint.y), blackColor);
	cv::line(image, cv::Point(centerPoint.x, 0), cv::Point(centerPoint.x, frameHeight), blackColor);
	if (targetCoord != cv::Point(999,999)) {
		if (centerRectangle.contains(targetCoord)) {
			targetCentered = true;
			targetInQ1 = targetInQ2 = targetInQ3 = targetInQ4 = false;
			highlightAllQuadrants();
		}
		else if (targetCoord.x >= centerPoint.x) {
			if (targetCoord.y <= centerPoint.y) {
				targetInQ1 = true;
				targetInQ2 = targetInQ3 = targetInQ4 = targetCentered = false;
				highlightQuadrant(1);
			}
			else if (targetCoord.y > centerPoint.y) {
				targetInQ4 = true;
				targetInQ1 = targetInQ2 = targetInQ3 = targetCentered = false;
				highlightQuadrant(4);
			}
		}
		else if (targetCoord.x < centerPoint.x) {
			if (targetCoord.y <= centerPoint.y) {
				targetInQ2 = true;
				targetInQ1 = targetInQ3 = targetInQ4 = targetCentered = false;
				highlightQuadrant(2);
			}
			else if (targetCoord.y > centerPoint.y) {
				targetInQ3 = true;
				targetInQ1 = targetInQ2 = targetInQ4 = targetCentered = false;
				highlightQuadrant(3);
			}
		}
	}
}
void displayTargetStatus() {
	double scale = 0.65;
	int thickness = 2;
	cv::Point textPos = cv::Point(10,30);
	if (targetInQ1)
		cv::putText(image, "UPPER RIGHT", textPos, cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
	else if (targetInQ2)
		cv::putText(image, "UPPER LEFT", textPos, cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
	else if (targetInQ3)
		cv::putText(image, "LOWER LEFT", textPos, cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
	else if (targetInQ4)
		cv::putText(image, "LOWER RIGHT", textPos, cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
	else if (targetCentered)
		cv::putText(image, "TARGET CENTERED", textPos, cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
}

void writeTargetStatusToFile() {
	std::ofstream targetFile("TargetStatus.txt");
	if (targetInQ1)
		targetFile << "UPPER RIGHT" << endl;
	else if (targetInQ2)
		targetFile << "UPPER LEFT" << endl;
	else if (targetInQ3)
		targetFile << "LOWER LEFT" << endl;
	else if (targetInQ4)
		targetFile << "LOWER RIGHT" << endl;
	else if (targetCentered)
		targetFile << "TARGET CENTERED" << endl;
}

int main() {
	cout << "Click the colored object you would like to track." << endl;
	if (!camera.isOpened()) return 1;
	ColorTracker.createColorSquare();
	TargetTracker.setThresh(20, 50, 70);
	TargetTracker.setMinContourArea(500);
	while (cv::waitKey(20) != 13) {
		if (!camera.read(image)) return 1;
		cv::flip(image, image, 1);
		processMouseActions();
		if (mouseButtonDown) {
			cv::rectangle(image, boundingRect, tealColor, 1);
			targetSelected = false;
			HSVDefined = false;
		}
		if (targetSelected && !HSVDefined) { 
			targetImage = image(calibratedRect(boundingRect));
			cv::imshow(targetName, targetImage);
			int hue = Hist.getMostAbundantHue(Hist.getHueHistogram(targetImage));
			int sat = Hist.getMostAbundantSat(Hist.getSatHistogram(targetImage));
			int val = Hist.getMostAbundantVal(Hist.getValHistogram(targetImage));
			ColorTracker.setColorSquareHSV(hue, sat, val);
			TargetTracker.setHSVToTrack(hue, sat, val);
			TargetTracker.findTargetAndUpdateRectangle(image);
			HSVDefined = true;
		}
		if (targetSelected && HSVDefined) {
			TargetTracker.findTargetAndUpdateRectangle(image);
			boundingRect = TargetTracker.getBoundingRect();
			targetImage = image(calibratedRect(boundingRect));
			cv::destroyWindow(targetName);
		}
		drawQuadrants(TargetTracker.getTargetCoordinates());
		displayTargetStatus();
		writeTargetStatusToFile();
		cv::imshow(imageName,image);
	}
}