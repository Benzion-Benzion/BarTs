#pragma once
#include "atlimage.h"

#include <complex>
using namespace std;

#define  MAX_SIZE 1000
const int Smoth_Kernel[9] = {1,1,1,1,1,1,1,1,1};
#define Pi 3.1415926535897932354626
#define Ei 2.71828

class CDib :
	public CImage
{
public:
	CDib(void);
	CDib(CDib &Dib);
	~CDib(void);

public:
	enum chan_color {chan_red=1,chan_green,chan_blue,chan_black};
	enum HSIChannel{HUE=0,SATURATION,INTENSITY};

public:
	void LoadFile(LPCTSTR lpszPathName);
	static cv::Mat applyWatershed(cv::Mat& image, double thresh = 100.0);
	static cv::Mat applyWatershed_T1c(cv::Mat& image, int lowThresh, int highThresh);
	static cv::Mat fillMaskHoles(const cv::Mat& src);
	static cv::Mat getTumorRegionMask(const cv::Mat& markers, int& tumor_label, int& tumor_area);
	static cv::Mat applyTumorOverlay(const cv::Mat& image, const cv::Mat& mask,
		double alpha = 0.5, cv::Vec3b color = cv::Vec3b(255, 0, 0));
	static HBITMAP MatToHBITMAP(const cv::Mat& mat);

	static cv::Mat GetFlairTumorMask(const cv::Mat& flairSlice,bool Light);
	static cv::Mat GetT1cTumorMask(const cv::Mat& t1cSlice,bool Light);
	static cv::Mat GetT2TumorMask(const cv::Mat& t2Slice,bool Light);
	
public:
	void Invert();

public:
	long* GrayValueCount();

public:
	long m_nWidth;
	long m_nHeight;
	int m_nWidthBytes;
	int m_nBitCount;
	unsigned char *m_pDibBits;
	
private:
	long *m_pGrayValueCount;
};

