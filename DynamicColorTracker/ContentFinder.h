#ifndef CONTENT_FINDER_H
#define CONTENT_FINDER_H
#include "OpenCVIncludes.h"

class ContentFinder {
private:
	float hranges[2];
	const float* ranges[1];
	int channels[1];
	float threshold;
	cv::MatND hue_hist;
	cv::MatND sat_hist;
public:
	ContentFinder() : threshold(-1.0f) {
		setThreshold(0.05f);
		ranges[0] = hranges;
	}
	void setThreshold(float thresh) {
		threshold = thresh;
	}
	float getThreshold() {
		return threshold;
	}
	// sets the reference histogram
	void setHueHistogram(const cv::MatND& referenceHist) {
		hue_hist = referenceHist;
		cv::normalize(hue_hist, hue_hist, 1.0);
	}
	void setSatHistogram(const cv::MatND& referenceHist) {
		sat_hist = referenceHist;
		cv::normalize(sat_hist, sat_hist, 1.0);
	}
	cv::Mat find_hue(const cv::Mat& image, float minValue=0.0f, float maxValue=180.0f, int dim=1) {
		cv::Mat HSV_img;
		cv::cvtColor(image, HSV_img, CV_BGR2HSV);
		cv::Mat result;
		hranges[0] = minValue;
		hranges[1] = maxValue;
		minValue++; maxValue++;
		cv::calcBackProject(&HSV_img, 1, channels, hue_hist, result, ranges, 255.0);
		// input image, number of images used, list of channels used, hue_hist, resulting backprojection, range of values, scaling factor
		// Threshold backprojection to obtain binary image
		if (threshold > 0.0)
			cv::threshold(result, result, 255*threshold, 255, cv::THRESH_BINARY);
		return result;
	}
	cv::Mat find_sat(const cv::Mat& image, float minValue, float maxValue, int* channels, int dim) {
		cv::Mat HSV_img;
		cv::cvtColor(image, HSV_img, CV_BGR2HSV);
		cv::Mat result;
		hranges[0] = minValue;
		hranges[1] = maxValue;
		for(int i=0; i<dim; i++)
			this->channels[i] = channels[i];
		cv::calcBackProject(&HSV_img, 1, channels, sat_hist, result, ranges, 255.0);
		// input image, list of channels used, hue_hist, resulting backprojection, range of values, scaling factor
		// Threshold backprojection to obtain binary image
		if (threshold > 0.0)
			cv::threshold(result, result, 255*threshold, 255, cv::THRESH_BINARY);
		return result;
	}
};

#endif CONTENT_FINDER_H