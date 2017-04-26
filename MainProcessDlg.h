#if !defined(AFX_MAINPROCESSDLG_H__9D0BB200_9920_4FE1_84D5_8B823B5BAA47__INCLUDED_)
#define AFX_MAINPROCESSDLG_H__9D0BB200_9920_4FE1_84D5_8B823B5BAA47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainProcessDlg.h : header file
//

#include <vfw.h>

#include "SR.h"
#include "Fruc.h"
#include "Stitch.h"
#include "MyFRUC.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "opencv2\core\core.hpp"
#include "opencv2\video\tracking.hpp"

using namespace cv;

#define TID_SEQUENCE_RUN		0
#define TID_AVI_RUN				1
#define TID_YUV_RUN				2 
#define TID_CAM_RUN				3
#define TID_MYAVI_RUN			4
#define TID_MYYUV_RUN			5
/////////////////////////////////////////////////////////////////////////////
// CMainProcessDlg dialog

class CMainProcessDlg : public CDialog
{
// Construction
public:
	CMainProcessDlg(CWnd* pParent = NULL);   // standard constructor

	CImageList m_ImageList;
	int m_nCurrentProcessingNum;

	bool m_bSequenceTimerRun, m_bCamTimerRun, m_bAVITimerRun, m_bYUVTimerRun;
	bool m_bTimerPause;
	bool m_bFirstFrame;


	void SelFolder();
	void SelAvi();
	void SelYuv();

	void SequenceRun();
	void AviRun();
	void YuvRun();

	void ReadFile(CString SelFolder);
	void ReadFile(CString SelFolder, bool bSubFolder);

	HWND m_hWndCap;
	BITMAPINFO	m_BmInfo;
	BYTE **m_CurrentImageRed, **m_CurrentImageGreen, **m_CurrentImageBlue;
	int m_nMW, m_nMH;

	int m_nYuvW, m_nYuvH, m_nYuvType;
	CString m_YuvFileName;
	CFile m_YuvFile;
	int m_nYuvLength;

	bool m_bGrabOn;
	int m_nElapsedTime;

	int m_nSequenceLength;

	_int64 m_TimeFreq, m_TimeStart, m_TimeEnd;

	bool m_bInitSaveAvi;
	PAVISTREAM m_psCompressedForSave, m_psForSave;
	PAVIFILE m_pfileForSave;
	BITMAPINFOHEADER m_BmInfoForSaveAvi;
	int m_nSaveAviW, m_nSaveAviH;
		
	bool AVIFileInitFunction();

	CSR m_Sr;
	CFruc m_Fruc;
	CStitch m_Stitch;
	MyFRUC m_MyFRUC;

	// VALUE FOR VIDEO STITCH
	CString m_AviFileName_L;
	PAVIFILE m_pavi_L;
	AVIFILEINFO m_fi_L;
	PAVISTREAM m_pstm_L;
	AVISTREAMINFO m_si_L;
	PGETFRAME m_pfrm_L;
	int m_nAviLength_L;

	CString m_AviFileName_R;
	PAVIFILE m_pavi_R;
	AVIFILEINFO m_fi_R;
	PAVISTREAM m_pstm_R;
	AVISTREAMINFO m_si_R;
	PGETFRAME m_pfrm_R;
	int m_nAviLength_R;

	int m_ListIdx_Video;

// Dialog Data
	//{{AFX_DATA(CMainProcessDlg)
	enum { IDD = IDD_MAIN_PROCESS };
	CComboBox	m_SeqTypeCtrl;
	CButton	m_SourceType0Ctrl;
	CButton	m_SourceType1Ctrl;
	CButton	m_SourceType2Ctrl;
	CEdit	m_GwCtrl;
	CEdit	m_GhCtrl;
	CListBox	m_ListCtrl;
	CSliderCtrl	m_SleepCtrl;
	CListCtrl	m_FileListCtrl;
	CButton m_Radio_Method1;
	CButton m_Radio_Method2;
	CButton m_Radio_Method3;

	int		m_nStart;
	int		m_nEnd;
	int		m_nGW;
	int		m_nGH;
	int		m_nFileType;
	int		m_nGap;
	int		m_nSleep;
	int		m_nSourceType;
	BOOL	m_bSubFolder;
	BOOL	m_bStep;
	int		m_nSeqType;
	BOOL	m_bSaveScreen;
	int		m_nZoomScale;
	int		m_nSearch;
	int		m_nGrow;
	int		m_nMask;
	BOOL	m_bViewPsnr;
	int		m_nRunType;
	int		m_nPsnrType;
	int		m_nUseColor;
	int		m_nMeGap;
	int		m_nThreDiff;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainProcessDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainProcessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPause();
	afx_msg void OnSelFolder2();
	afx_msg void OnGrab();
	afx_msg void OnDestroy();
	afx_msg void OnVs();
	afx_msg void OnCamOn();
	afx_msg void OnResetGNum();
	afx_msg void OnCustomdrawSleep(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelSourceLeft();
	afx_msg void OnSelSourceRight();
	afx_msg void OnRun();
	afx_msg void OnSaveScreen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	CListCtrl m_List_Video;
	CButton m_CHECK_DEBUG;


	// Value for Video Stitching
	int m_nHL, m_nWL;
	int m_nHR, m_nWR;
	int m_nH_Total, m_nW_Total;
	int m_numC;
	Mat m_Mask;

	// Image
	Mat m_MImgLCurColor, m_MImgRCurColor;

	Mat m_MImgLCurGray, m_MImgRCurGray;
	Mat m_MImgLPreGray, m_MImgRPreGray;

	Mat m_MImgLTransColor, m_MImgRTransColor;
	Mat m_MImgLTransGray, m_MImgRTransGray;

	Mat m_MImgWarp;

	BYTE** m_ImgWarpR, **m_ImgWarpG, **m_ImgWarpB;

	Mat m_Homo;

	// CP
	Mat m_CPL_pre, m_CPR_pre;
	Mat m_CPL_cur, m_CPR_cur;

	Mat m_homoL, m_homoR;
	Mat m_homoL_pre, m_homoR_pre;
	
	Mat m_warpCPL, m_warpCPR;
	Mat m_warpCPL_scale, m_warpCPR_scale;

	vector<Point2f> m_arrPtPreL, m_arrPtPreR; // save Feature Points 
	vector<Point2f> arrMatchL, arrMatchR;

	// Blend
	Mat m_MImgMask, m_MImgEdgeMask, m_MImgMaskSmall;
	Mat m_MImgLTransMask, m_MImgRTransMask;
	Mat m_MImgEdgeLTransMask, m_MImgEdgeRTransMask;
	Mat m_MImgLTransMaskSmall, m_MImgRTransMaskSmall;

	Mat m_MImgLBlendMask, m_MImgRBlendMask;
	Mat m_MImgLBlendColor, m_MImgRBlendColor;

	Mat m_MImgBlend, m_MImgBlendMask;

	int m_offsetX_Blend, m_offsetY_Blend;
	int m_nH_Blend, m_nW_Blend;

	// control
	bool m_bDebug;
	bool m_bEnlarge;
	bool m_bStopView;
	bool m_bMultiBandBlend;
	enum LR{ LEFT, RIGHT };

	// time
	int64 TimeFreq;
	int64 TimeStart_Total, TimeEnd_Total;
	int64 TimeStart_EstimateHomo, TimeEnd_EstimateHomo;
	int64 TimeStart_CorrectHomo, TimeEnd_CorrectHomo;
	int64 TimeStart_Warp, TimeEnd_Warp;

	double result_Total;
	double result_EstimateHomo, result_CorrectHomo, result_Warp;

	// Function
	void GethomographySIFT(Mat& dstImg, int nH_dst, int nW_dst, Mat& srcImg, int nH_src, int nW_src, Mat& homo);
	void GethomographyGoodFeature(vector<Point2f>& arrPtPre, Mat& homo, Mat& homo_pre, int thresh_distance, int minFeature, double ransac, bool bFirst, bool bDebug = false, bool bEnlarge = false, int nEnlargeRate = 1, LR bLR = LEFT);
	void GethomographyNew(vector<Point2f>& arrPtPre, Mat& homo, Mat& homo_pre, int thresh_distance, int minFeature, double ransac, bool bFirst, bool bDebug = false, bool bEnlarge = false, int nEnlargeRate = 1, LR bLR = LEFT);
	void GethomographyHarris(vector<Point2f>& arrPtPre, Mat& homo, Mat& homo_pre, int thresh_distance, int minFeature, double ransac, bool bFirst, bool bDebug = false, bool bEnlarge = false, int nEnlargeRate = 1, LR bLR = LEFT);
	void CorrectHomography(vector<Point2f>& arrPtR, bool bDebug);
	void MakeMaskForBlend();
	void WarpHomographyForVideo(Mat& BL, Mat& BR, int nHL, int nWL, int nHR, int nWR, bool bMultiBandBlend, bool bDebug);

	void ScaleHomography(Mat& Homo, Mat& scaledHomo, double dScale);

	afx_msg void OnBnClickedOptimization();
	CButton m_CheckFile;

	clock_t Tstart, Tend;
	double m_processNoise;
	double m_measureNoise;
	CButton m_Check_StopView;
	CButton m_Check_Enlarge;
	int m_Edit_EnlargeRate;
	CButton m_Check_MultiBandBlend;
	int m_nStitchW;
	int m_nStitchH;
	double m_Stitch_Scale;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINPROCESSDLG_H__9D0BB200_9920_4FE1_84D5_8B823B5BAA47__INCLUDED_)
