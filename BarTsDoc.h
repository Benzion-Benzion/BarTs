
// BarTsDoc.h: CBarTsDoc 类的接口
//


#pragma once
#include "CColorButton.h"

class CBarTsDoc : public CDocument
{
protected: // 仅从序列化创建
	CBarTsDoc() noexcept;
	DECLARE_DYNCREATE(CBarTsDoc)

// 特性
public:
	nifti_image* m_pNiftiImg = nullptr;
	std::vector<int> m_sliceIndices;    // 当前每个模态的切片位置，即索引数组
	cv::Mat m_blendedFlair;// 保存一个融合后的 FLAIR 图像，用于显示或进一步处理
	cv::Mat m_blendedT1C; // 保存一个融合后的 T1C 图像，用于显示或进一步处理
	cv::Mat m_blendedT2; // 保存一个融合后的 T2 图像，用于显示或进一步处理
	cv::Mat m_blendedT1;// 保存一个融合后的 T1 图像，用于显示或进一步处理

	// 假设：4个模态，每个模态有 nz 张切片图像
	std::vector<std::vector<cv::Mat>> m_allModalImages;
	std::vector<cv::Mat> m_enhancedSlices;   // 每个模态的增强结果
	std::vector<std::vector<cv::Mat>> m_originalModalImages; // 保存原始图像备份，方便恢复

	std::vector<cv::Mat> m_grayMatterSlices;// 保存灰质结果
	std::vector<cv::Mat> m_whiteMatterSlices;// 保存白质结果

// 操作
public:
	bool LoadNiftiFile(const CString& path);
	void NotifyViewsRefresh();
	void UpdateSlices(const std::vector<int>& sliceIndices);
	void UpdateSingleModalSlice(int modalityIndex, int sliceIndex);//单模态切片
	void UpdateGrayWhiteMatterMask(int sliceIndex);//灰白质切换
	void ApplyCLAHEEnhancement(const std::vector<int>& sliceIndices);//图像增强操作
	void SobelEdge(const std::vector<int>& sliceIndices);// 边缘增强操作
	void AdapativeNoise(const std::vector<int>& sliceIndices);//自适应中值滤波操作
	void GaussianNoise(const std::vector<int>& sliceIndices);//高斯去噪操作
	void BilateralFilter(const std::vector<int>& sliceIndices);//双边滤波操作
	void HomomorphicCorrection(const std::vector<int>& sliceIndices);// 光照自适应调节，同态滤波
	void LightAllCorrection();
	cv::Mat ApplyHomomorphicFilter(const cv::Mat& input, double gammaHigh, double gammaLow, int radius);// 同态滤波辅助函数
	cv::Mat CreateHomomorphicFilter(const cv::Size& size, double gammaHigh, double gammaLow, int radius);// 创建同态滤波器函数

	// 背景感知的亮度归一化
	void BackgroundAwareNormalization(int currentModal, int currentSliceIdx, int referenceSliceIdx);

	// 辅助函数：创建脑组织掩码
	cv::Mat CreateBrainMask(const cv::Mat& image);

	// 辅助函数：计算前景统计量
	void ComputeForegroundStats(const cv::Mat& image, const cv::Mat& mask,
		double& mean, double& stddev);
	void Restore();// 恢复原图操作

	void PerformSegmentation();// 灰质白质图分割

	void SegmentBrain(const cv::Mat& inputSlice,
		cv::Mat& grayMatterMask,
		cv::Mat& whiteMatterMask,
		int modal);

	void RemoveSmallRegions(cv::Mat& mask, int areaThreshold);


// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CBarTsDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
