// MainProcessDlg.cpp : implementation file
//
#include "afxcmn.h"
#include "stdafx.h"
#include "ImageProcessing.h"
#include "MainProcessDlg.h"
#include "Image.h"

#include "MainFrm.h"		// 추가
#include "ChildFrm.h"		// 추가
#include "ImageProcessingDoc.h"		// 추가
#include "ImageProcessingView.h"	// 추가

#include "YuvOptionDlg.h"
#include "SaveScreenOpt.h"
#include "StitchBasic.h"

#include <Eigen/Dense>
#include <Eigen/Eigenvalues> 

#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\video\tracking.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\stitching\detail\blenders.hpp"
#include "opencv2\stitching\detail\seam_finders.hpp"

#include<vector>

#include <unsupported/Eigen/MatrixFunctions>

#include <time.h>

#include <afxtempl.h>

#include <io.h>
#include <windowsx.h>

#include <iostream>

using namespace std;
using namespace cv::detail;
using namespace Eigen;

#define WARP_H 600
#define WARP_W 1200

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainProcessDlg dialog


CMainProcessDlg::CMainProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainProcessDlg::IDD, pParent)
	, m_processNoise(0)
	, m_measureNoise(0)
	, m_Edit_EnlargeRate(0)
	, m_nStitchW(0)
	, m_nStitchH(0)
	, m_Stitch_Scale(1)
{
	//{{AFX_DATA_INIT(CMainProcessDlg)
	m_nStart = 0;
	m_nEnd = 0;
	m_nGW = 320;
	m_nGH = 240;
	m_nFileType = 0;
	m_nGap = 30;
	m_nSleep = 0;
	m_nSourceType = 0;
	m_bSubFolder = FALSE;
	m_bStep = FALSE;
	m_nSeqType = 8;
	m_bSaveScreen = FALSE;
	m_nZoomScale = 0;
	m_nSearch = 3;
	m_nGrow = 3;
	m_nMask = 2;
	m_bViewPsnr = FALSE;
	m_nRunType = 0;
	m_nPsnrType =0;
	m_nUseColor = 0;
	m_nMeGap = 3;
	m_nThreDiff = 0;
	//}}AFX_DATA_INIT

	m_bSequenceTimerRun = false;
	m_bCamTimerRun = false;
	m_bAVITimerRun = false;
	m_bYUVTimerRun = false;
	m_bTimerPause = false;

	m_nMW = 2000;
	m_nMH = 2000;

	m_nH_Total = 720;
	m_nW_Total = 1280;

	AVIFileInit();
	 
	m_psForSave = NULL;
	m_psCompressedForSave = NULL;
	m_pfileForSave = NULL;
}

void CMainProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainProcessDlg)
	DDX_Control(pDX, IDC_SEQ_TYPE, m_SeqTypeCtrl);
	DDX_Control(pDX, IDC_SOURCE_TYPE0, m_SourceType0Ctrl);
	DDX_Control(pDX, IDC_SOURCE_TYPE1, m_SourceType1Ctrl);
	DDX_Control(pDX, IDC_SOURCE_TYPE2, m_SourceType2Ctrl);
	DDX_Control(pDX, IDC_GW, m_GwCtrl);
	DDX_Control(pDX, IDC_GH, m_GhCtrl);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	DDX_Control(pDX, IDC_SLEEP, m_SleepCtrl);
	DDX_Control(pDX, IDC_FILE_LIST, m_FileListCtrl);
	DDX_Text(pDX, IDC_START, m_nStart);
	DDX_Text(pDX, IDC_END, m_nEnd);
	DDX_Text(pDX, IDC_GW, m_nGW);
	DDX_Text(pDX, IDC_GH, m_nGH);
	DDX_Radio(pDX, IDC_TYPE, m_nFileType);
	DDX_Text(pDX, IDC_GAP, m_nGap);
	DDX_Slider(pDX, IDC_SLEEP, m_nSleep);
	DDX_Radio(pDX, IDC_SOURCE_TYPE0, m_nSourceType);
	DDX_Check(pDX, IDC_SUB_FOLDER, m_bSubFolder);
	DDX_Check(pDX, IDC_STEP, m_bStep);
	DDX_CBIndex(pDX, IDC_SEQ_TYPE, m_nSeqType);
	DDX_Check(pDX, IDC_SAVE_SCREEN, m_bSaveScreen);
	DDX_Radio(pDX, IDC_ZOOM_SCALE0, m_nZoomScale);
	DDX_Text(pDX, IDC_SEARCH, m_nSearch);
	DDX_Text(pDX, IDC_GROW, m_nGrow);
	DDX_Text(pDX, IDC_MASK, m_nMask);
	DDX_Check(pDX, IDC_VIEW_PSNR, m_bViewPsnr);
	DDX_Radio(pDX, IDC_RUN_TYPE0, m_nRunType);
	DDX_Radio(pDX, IDC_PSNR_TYPE0, m_nPsnrType);
	DDX_Radio(pDX, IDC_USE_COLOR_TYPE0, m_nUseColor);
	DDX_Text(pDX, IDC_ME_GAP, m_nMeGap);
	DDX_Text(pDX, IDC_THRE_DIFF, m_nThreDiff);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO_METHOD1, m_Radio_Method1);
	DDX_Control(pDX, IDC_RADIO_METHOD2, m_Radio_Method2);
	DDX_Control(pDX, IDC_RADIO_METHOD3, m_Radio_Method3);
	DDX_Control(pDX, IDC_LIST2, m_List_Video);
	DDX_Control(pDX, IDC_DEBUG, m_CHECK_DEBUG);
	DDX_Control(pDX, IDC_CHECKFILE, m_CheckFile);
	DDV_MinMaxDouble(pDX, m_processNoise, 0, 100);
	DDV_MinMaxDouble(pDX, m_measureNoise, 0, 100);
	DDX_Control(pDX, IDC_CHECK_STOPVIEW, m_Check_StopView);
	DDX_Control(pDX, IDC_CHECK_ENLARGE, m_Check_Enlarge);
	DDX_Text(pDX, IDC_EDIT_ENLARGE_RATE, m_Edit_EnlargeRate);
	DDV_MinMaxInt(pDX, m_Edit_EnlargeRate, 0, 10);
	DDX_Control(pDX, IDC_CHECK_MULTIBAND, m_Check_MultiBandBlend);
	DDX_Text(pDX, IDC_STITCH_W, m_nStitchW);
	DDX_Text(pDX, IDC_STITCH_H, m_nStitchH);
	DDX_Text(pDX, IDC_STITCH_SCALE, m_Stitch_Scale);
}


BEGIN_MESSAGE_MAP(CMainProcessDlg, CDialog)
	//{{AFX_MSG_MAP(CMainProcessDlg)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_FILE_LIST, OnDblclkFileList)
	ON_BN_CLICKED(IDC_PAUSE, OnPause)
	ON_BN_CLICKED(IDC_SEL_FOLDER2, OnSelFolder2)
	ON_BN_CLICKED(IDC_GRAB, OnGrab)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_VS, OnVs)
	ON_BN_CLICKED(IDC_CAM_ON, OnCamOn)
	ON_BN_CLICKED(IDC_RESET_G_NUM, OnResetGNum)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLEEP, OnCustomdrawSleep)
	ON_BN_CLICKED(IDC_SEL_SOURCE_LEFT, OnSelSourceLeft)
	ON_BN_CLICKED(IDC_RUN, OnRun)
	ON_BN_CLICKED(IDC_SAVE_SCREEN, OnSaveScreen)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SEL_SOURCE_RIGHT, &CMainProcessDlg::OnSelSourceRight)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainProcessDlg message handlers

BOOL CMainProcessDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	LV_COLUMN lvColumn;
	SHFILEINFO shInfo;
	CRect FileListRect;
	HIMAGELIST hImageList=(HIMAGELIST)SHGetFileInfo(_T("C:\\"),0,&shInfo,sizeof(SHFILEINFO),SHGFI_SYSICONINDEX|SHGFI_SMALLICON);
	m_ImageList.Attach(hImageList);

	m_FileListCtrl.GetClientRect(&FileListRect);

	TCHAR *list = _T("File name");
	int nWidth;
	nWidth = FileListRect.Width()-55;

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.pszText = list;
	lvColumn.iSubItem = 0;
	lvColumn.cx = nWidth;
	m_FileListCtrl.InsertColumn(0, &lvColumn);
	m_FileListCtrl.InsertColumn(1, _T("Path"), LVCFMT_LEFT, 200, 0);

	long lSetStyle;
	lSetStyle = GetWindowLong(m_FileListCtrl.m_hWnd, GWL_STYLE);
	lSetStyle &= ~LVS_TYPEMASK;
	lSetStyle |= LVS_REPORT;
	SetWindowLong(m_FileListCtrl.m_hWnd, GWL_STYLE, lSetStyle);

	m_FileListCtrl.SetImageList(&m_ImageList,LVSIL_SMALL);

	m_FileListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_FileListCtrl.SetBkColor(RGB(255,255,255));
	m_FileListCtrl.SetTextBkColor(RGB(255,255,200));
	m_FileListCtrl.SetTextColor(RGB(100,20,20));

	m_CurrentImageRed = cmatrix(m_nMH, m_nMW);
	m_CurrentImageGreen = cmatrix(m_nMH, m_nMW);
	m_CurrentImageBlue = cmatrix(m_nMH, m_nMW);

	m_SleepCtrl.SetRange(0, 1000);

	m_SeqTypeCtrl.InsertString(0, _T("*.*"));
	m_SeqTypeCtrl.InsertString(1, _T("BMP"));
	m_SeqTypeCtrl.InsertString(2, _T("JPG"));
	m_SeqTypeCtrl.InsertString(3, _T("PCX"));
	m_SeqTypeCtrl.InsertString(4, _T("256"));
	m_SeqTypeCtrl.InsertString(5, _T("320"));
	m_SeqTypeCtrl.InsertString(6, _T("640"));
	m_SeqTypeCtrl.InsertString(7, _T("BMP+JPG"));
	m_SeqTypeCtrl.InsertString(8, _T("Images"));

	m_SeqTypeCtrl.SetCurSel(8);

	m_SourceType0Ctrl.SetCheck(BST_UNCHECKED);
	m_SourceType1Ctrl.SetCheck(BST_CHECKED);
	m_SourceType2Ctrl.SetCheck(BST_UNCHECKED);

	m_Radio_Method1.SetCheck(BST_CHECKED);
	m_Radio_Method2.SetCheck(BST_UNCHECKED);
	m_Radio_Method3.SetCheck(BST_UNCHECKED);

	m_CHECK_DEBUG.SetCheck(BST_UNCHECKED);	
	m_CheckFile.SetCheck(BST_UNCHECKED);

	m_List_Video.InsertColumn(0, _T("FRAME"), LVCFMT_LEFT, 40);
	m_List_Video.InsertColumn(1, _T("Total(s)"), LVCFMT_LEFT, 70);
	m_List_Video.InsertColumn(2, _T("Est Homo(s)"), LVCFMT_LEFT, 70);
	m_List_Video.InsertColumn(3, _T("Cor Homo(s)"), LVCFMT_LEFT, 70);
	m_List_Video.InsertColumn(4, _T("Warping(s)"), LVCFMT_LEFT, 70);
	m_ListIdx_Video = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMainProcessDlg::OnTimer(UINT nIDEvent) 
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	UpdateData(TRUE);

	while(true)
	{
		if(m_bTimerPause) break;

		if(nIDEvent == TID_YUV_RUN)
		{
			Sleep(m_nSleep);

			if(m_nCurrentProcessingNum > m_nEnd) 
			{
				KillTimer(TID_YUV_RUN);

				SetDlgItemText(IDC_RUN, _T("Run"));

				m_SourceType0Ctrl.EnableWindow(TRUE);
				m_SourceType1Ctrl.EnableWindow(TRUE);
				m_SourceType2Ctrl.EnableWindow(TRUE);

				m_bYUVTimerRun = false;

				m_YuvFile.Close();

				break;
			}

			int nW = m_nYuvW, nH = m_nYuvH;
			
			BYTE *ReadY = new BYTE [nW*nH];
			BYTE *ReadU = new BYTE [nW*nH/4];
			BYTE *ReadV = new BYTE [nW*nH/4];

			m_YuvFile.Read(ReadY, nW*nH);
			m_YuvFile.Read(ReadU, nW*nH/4);
			m_YuvFile.Read(ReadV, nW*nH/4);

			BYTE **R, **G, **B;

			R = cmatrix(nH, nW);
			G = cmatrix(nH, nW);
			B = cmatrix(nH, nW);

			YUV2RGB(ReadY, ReadU, ReadV, R, G, B, nW, nH, m_nYuvType);

			//DisplayCimage2DColor(R, G, B, nW, nH, 0, 0, false, true);

			BYTE *Image1D = new BYTE[GetBmp24Size(nW, nH)];
	
			int x, y, Pos;
			for(y = 0 ; y < nH ; y++)
			{
				for(x = 0 ; x < nW ; x++)
				{
					Pos = GetBmp24Pos(nW, nH, x, y);
					Image1D[Pos++] = B[y][x];
					Image1D[Pos++] = G[y][x];
					Image1D[Pos] = R[y][x];
				}
			}


			if(pChild) 
			{
				CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

				if(pView)
				{
					if(m_nRunType == 0)
						m_Sr.Run(Image1D, nW, nH, pView->m_CurrentPoint.x, pView->m_CurrentPoint.y, (m_nZoomScale+1)<<1, m_nMask, m_nSearch, m_nGrow, m_bViewPsnr?true:false, m_bFirstFrame);
					else if(m_nRunType == 1)
						m_Fruc.Run(Image1D, nW, nH, (m_nZoomScale+1)<<1,m_nMask, m_nSearch, m_nGrow, m_nMeGap, m_nUseColor, m_nThreDiff,
							m_bViewPsnr?true:false, m_nPsnrType, m_bFirstFrame);
						//m_MyFRUC.Run(Image1D, nW, nH, (m_nZoomScale+1)<<1,m_nMask, m_nSearch, m_nGrow, m_nMeGap, m_nUseColor, m_nThreDiff,
							//m_bViewPsnr?true:false, m_nPsnrType, m_bFirstFrame);
					else if(m_nRunType == 2)
						m_Stitch.Run(Image1D, nW, nH, m_bFirstFrame);
				
				}
				else
				{
					if(m_nRunType == 0)
						m_Sr.Run(Image1D, nW, nH, 100, 100, (m_nZoomScale+1)<<1, m_nMask, m_nSearch, m_nGrow, m_bViewPsnr?true:false, m_bFirstFrame);
					else if(m_nRunType == 1)
						m_Fruc.Run(Image1D, nW, nH, (m_nZoomScale+1)<<1,m_nMask, m_nSearch, m_nGrow, m_nMeGap, m_nUseColor, m_nThreDiff,
							m_bViewPsnr?true:false, m_nPsnrType, m_bFirstFrame);
						//m_MyFRUC.Run(Image1D, nW, nH, (m_nZoomScale+1)<<1,m_nMask, m_nSearch, m_nGrow, m_nMeGap, m_nUseColor, m_nThreDiff,
							//m_bViewPsnr?true:false, m_nPsnrType, m_bFirstFrame);
					else if(m_nRunType == 2)
						m_Stitch.Run(Image1D, nW, nH, m_bFirstFrame);
				}
			}

			delete [] Image1D;


//			CString FileName;
//			FileName.Format(_T("C:\\Users\\Nize\\Desktop\\FOREMAN_CIF15\\aa%07d.bmp"), m_nCurrentProcessingNum);
//			SaveImage2DColorBmp(FileName, R, G, B, nW, nH);

			CString DispFrameNum;
			DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nYuvLength-1);
			SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

			m_nCurrentProcessingNum++;

			free_cmatrix(R, nH, nW);
			free_cmatrix(G, nH, nW);
			free_cmatrix(B, nH, nW);

			delete [] ReadY;
			delete [] ReadU;
			delete [] ReadV;

			m_bFirstFrame = false;

			if(m_YuvFile.GetPosition() == m_YuvFile.GetLength())
			{
				m_bYUVTimerRun = false;
				SetDlgItemText(IDC_RUN, _T("Run"));

				m_SourceType0Ctrl.EnableWindow(TRUE);
				m_SourceType1Ctrl.EnableWindow(TRUE);
				m_SourceType2Ctrl.EnableWindow(TRUE);

				KillTimer(TID_YUV_RUN);

				m_YuvFile.Close();
			}
		}
		else if(nIDEvent == TID_AVI_RUN)
		{
			Sleep(m_nSleep);

			LPBITMAPINFOHEADER pbmih_L, pbmih_R;
			pbmih_L = (LPBITMAPINFOHEADER) AVIStreamGetFrame(m_pfrm_L, m_nCurrentProcessingNum);
			pbmih_R = (LPBITMAPINFOHEADER)AVIStreamGetFrame(m_pfrm_R, m_nCurrentProcessingNum);	
			
			BYTE *imageL = (unsigned char *) ((LPSTR)pbmih_L + (WORD)pbmih_L->biSize); 
			BYTE *imageR = (unsigned char *)((LPSTR)pbmih_R + (WORD)pbmih_R->biSize);

//			int x, y, offset;
			bool bExit = false;
			BYTE *Image1D_L = NULL;
			BYTE *Image1D_R = NULL;

			// 왼쪽 이미지 받아오기
			if(pbmih_L->biBitCount == 24 || pbmih_L->biBitCount == 32 || pbmih_L->biBitCount == 16) 
			{
				Image1D_L = new BYTE[GetBmp24Size(pbmih_L->biWidth, pbmih_L->biHeight)];

				if(pbmih_L->biBitCount == 24)
					memcpy(Image1D_L, imageL, GetBmp24Size(pbmih_L->biWidth, pbmih_L->biHeight));
				else if(pbmih_L->biBitCount == 32)
				{
					int x, y, offset1, offset2;
					for (y = 0; y < pbmih_L->biHeight ; y++)
						for (x = 0; x < pbmih_L->biWidth ; x++)
						{
							offset1 = (y*pbmih_L->biWidth+x)*pbmih_L->biBitCount/8;
							offset2 = (y*pbmih_L->biWidth+x)*24/8;
							memcpy(Image1D_L+offset2, imageL+offset1, 3);
						}
				}
				else if(pbmih_L->biBitCount == 16)
				{
					int x, y, offset1, offset2;
					for (y = 0; y < pbmih_L->biHeight ; y++)
						for (x = 0; x < pbmih_L->biWidth ; x++)
						{
							offset1 = (y*pbmih_L->biWidth*2+3)/4*4 + x*2;

							offset2 = (y*pbmih_L->biWidth+x)*24/8;
							
							WORD img = *(WORD *)(imageL+offset1);

							if(pbmih_L->biCompression == BI_BITFIELDS)
							{
								Image1D_L[offset2++] = (BYTE)(((img>>0) & 0x001F) * 8);
								Image1D_L[offset2++] = (BYTE)(((img>>5) & 0x003F) * 4);
								Image1D_L[offset2] = (BYTE)(((img>>11) & 0x001F) * 8);
							}
							else
							{
								Image1D_L[offset2++] = (BYTE)(((img>>0) & 0x001F) * 8);
								Image1D_L[offset2++] = (BYTE)(((img>>5) & 0x001F) * 8);
								Image1D_L[offset2] = (BYTE)(((img>>10) & 0x001F) * 8);
							}
						}
				}

				// 오른쪽 이미지 받아오기
				if (pbmih_R->biBitCount == 24 || pbmih_R->biBitCount == 32 || pbmih_R->biBitCount == 16)
				{
					Image1D_R = new BYTE[GetBmp24Size(pbmih_R->biWidth, pbmih_R->biHeight)];

					if (pbmih_R->biBitCount == 24)
						memcpy(Image1D_R, imageR, GetBmp24Size(pbmih_R->biWidth, pbmih_R->biHeight));
					else if (pbmih_R->biBitCount == 32)
					{
						int x, y, offset1, offset2;
						for (y = 0; y < pbmih_R->biHeight; y++)
							for (x = 0; x < pbmih_R->biWidth; x++)
							{
								offset1 = (y*pbmih_R->biWidth + x)*pbmih_R->biBitCount / 8;
								offset2 = (y*pbmih_R->biWidth + x) * 24 / 8;
								memcpy(Image1D_R + offset2, imageR + offset1, 3);
							}
					}
					else if (pbmih_R->biBitCount == 16)
					{
						int x, y, offset1, offset2;
						for (y = 0; y < pbmih_R->biHeight; y++)
							for (x = 0; x < pbmih_R->biWidth; x++)
							{
								offset1 = (y*pbmih_R->biWidth * 2 + 3) / 4 * 4 + x * 2;

								offset2 = (y*pbmih_R->biWidth + x) * 24 / 8;

								WORD img = *(WORD *)(imageR + offset1);

								if (pbmih_R->biCompression == BI_BITFIELDS)
								{
									Image1D_R[offset2++] = (BYTE)(((img >> 0) & 0x001F) * 8);
									Image1D_R[offset2++] = (BYTE)(((img >> 5) & 0x003F) * 4);
									Image1D_R[offset2] = (BYTE)(((img >> 11) & 0x001F) * 8);
								}
								else
								{
									Image1D_R[offset2++] = (BYTE)(((img >> 0) & 0x001F) * 8);
									Image1D_R[offset2++] = (BYTE)(((img >> 5) & 0x001F) * 8);
									Image1D_R[offset2] = (BYTE)(((img >> 10) & 0x001F) * 8);
								}
							}
					}
				}
				CString SelFolder;
				CString FileName;

				// Processing
				// value for time
				clock_t Tstart_Match, Tend_Match;
				clock_t Tstart_Homo, Tend_Homo;
				clock_t Tstart_Warp, Tend_Warp;

				double tMatch, tHomo, tWarp;
				
				// initialize Data
				m_nHL = pbmih_L->biHeight;
				m_nWL = pbmih_L->biWidth;

				m_nHR = pbmih_R->biHeight;
				m_nWR = pbmih_R->biWidth;

				Mat tmpMatL, tmpMatR;

				// 이전 이미지 할당
				if (m_nCurrentProcessingNum == 0)
				{
					// 현재 이미지 2D 할당.
					m_MImgLCurColor = Mat(cvSize(m_nWL, m_nHL), CV_8UC3, cvScalar(0));
					m_MImgRCurColor = Mat(cvSize(m_nWR, m_nHR), CV_8UC3, cvScalar(0));
					
					m_MImgLCurGray = Mat(cvSize(m_nWL, m_nHL), CV_8UC1, cvScalar(0));
					m_MImgRCurGray = Mat(cvSize(m_nWR, m_nHR), CV_8UC1, cvScalar(0));
					m_MImgLPreGray = Mat(cvSize(m_nWL, m_nHL), CV_8UC1, cvScalar(0));
					m_MImgRPreGray = Mat(cvSize(m_nWR, m_nHR), CV_8UC1, cvScalar(0));

					m_MImgLTransColor = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC3, cvScalar(0));
					m_MImgRTransColor = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC3, cvScalar(0));

					m_MImgLTransGray = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC1, cvScalar(0));
					m_MImgRTransGray = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC1, cvScalar(0));

					m_MImgMask = Mat(cvSize(m_nWL, m_nHL), CV_8UC1, cvScalar(255));
					m_MImgMaskSmall = Mat(cvSize(m_nWL, m_nHL), CV_8UC1, cvScalar(0));
					m_MImgEdgeMask = Mat(cvSize(m_nWL, m_nHL), CV_8UC1, cvScalar(0));
					
					for (int i = 10; i < m_nHL - 10; i++)
						for (int j = 10; j < m_nWL - 10; j++)
						{
							m_MImgEdgeMask.at<uchar>(i, j) = 255;
						}

					for (int i = 20; i < m_nHL - 20; i++)
						for (int j = 20; j < m_nWL - 20; j++)
						{
							m_MImgEdgeMask.at<uchar>(i, j) = 0;
							m_MImgMaskSmall.at<uchar>(i, j) = 255;
						}

					m_MImgLTransMask = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC1, cvScalar(0));
					m_MImgRTransMask = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC1, cvScalar(0));
					
					// warp 이미지 할당,!!!!!!!
					m_ImgWarpR = cmatrix(m_nH_Total, m_nW_Total);
					m_ImgWarpG = cmatrix(m_nH_Total, m_nW_Total);
					m_ImgWarpB = cmatrix(m_nH_Total, m_nW_Total);

					m_MImgWarp = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8UC1, cvScalar(0));
				}

				Byte1DToMatColor(Image1D_L, m_MImgLCurColor.data, m_nHL, m_nWL);
				Byte1DToMatColor(Image1D_R, m_MImgRCurColor.data, m_nHR, m_nWR);

				cvtColor(m_MImgLCurColor, m_MImgLCurGray, CV_BGR2GRAY);
				cvtColor(m_MImgRCurColor, m_MImgRCurGray, CV_BGR2GRAY);

				for (int i = 0; i < m_nH_Total; i++)
					for (int j = 0; j < m_nW_Total; j++)
					{
						m_ImgWarpR[i][j] = 0;
						m_ImgWarpG[i][j] = 0;
						m_ImgWarpB[i][j] = 0;
					}

				// CONTROL 확인

				m_bDebug = false;
				m_bEnlarge = false;
				m_bStopView = false;
				m_bMultiBandBlend = false;

				if (m_CHECK_DEBUG.GetCheck() == BST_CHECKED)
					m_bDebug = true;

				if (m_Check_Enlarge.GetCheck() == BST_CHECKED)
					m_bEnlarge = true;

				if (m_Check_StopView.GetCheck() == BST_CHECKED)
					m_bStopView = true;

				if (m_Check_MultiBandBlend.GetCheck() == BST_CHECKED)
					m_bMultiBandBlend = true;

				QueryPerformanceFrequency((LARGE_INTEGER*)&TimeFreq);
				QueryPerformanceCounter((LARGE_INTEGER*)&TimeStart_Total);

				//***************************************************************************
				//********* Step 1. Camera path Estimation***********************************
				//***************************************************************************	

				// Left, Right Image Homography 구하기
				if (m_nCurrentProcessingNum == 0)
				{
					GethomographySIFT(m_MImgLCurGray, m_nHL, m_nWL, m_MImgRCurGray, m_nHR, m_nWR, m_Homo);// 프래임 사이 호모그래피 구하기
					
					m_CPL_cur = Mat::eye(3, 3, CV_64F);
					m_CPR_cur = Mat::eye(3, 3, CV_64F);
				}

				QueryPerformanceCounter((LARGE_INTEGER*)&TimeStart_EstimateHomo);
				
				// InterFrame Homography 구하기
				// Method1 : GFTT 이용
				if (m_Radio_Method1.GetCheck() == BST_CHECKED)
				{
					if (m_nCurrentProcessingNum > 0)
					{
						if (m_nCurrentProcessingNum == 1)
						{
							GethomographyGoodFeature(m_arrPtPreL, m_homoL, m_homoL_pre, 50, 10, 1, true, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, LEFT);// 프래임 사이 호모그래피 구하기
							GethomographyGoodFeature(m_arrPtPreR, m_homoR, m_homoR_pre, 50, 10, 1, true, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, RIGHT);// 프래임 사이 호모그래피 구하기
						}

						if (m_nCurrentProcessingNum > 1)
						{
							GethomographyGoodFeature(m_arrPtPreL, m_homoL, m_homoL_pre, 50, 10, 1, false, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, LEFT);// 프래임 사이 호모그래피 구하기
							GethomographyGoodFeature(m_arrPtPreR, m_homoR, m_homoR_pre, 50, 10, 1, false, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, RIGHT);// 프래임 사이 호모그래피 구하기
						}

						m_CPL_cur = m_homoL * m_CPL_pre;
						m_CPR_cur = m_homoR * m_CPR_pre;

						m_homoL_pre = m_homoL;
						m_homoR_pre = m_homoR;
					}
				}

				// Method2 : Grid 이용
				if (m_Radio_Method2.GetCheck() == BST_CHECKED)
				{
					if (m_nCurrentProcessingNum > 0)
					{
						if (m_nCurrentProcessingNum == 1)
						{
							GethomographyNew(m_arrPtPreL, m_homoL, m_homoL_pre, 50, 10, 1, true, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, LEFT);// 프래임 사이 호모그래피 구하기
							GethomographyNew(m_arrPtPreR, m_homoR, m_homoR_pre, 50, 10, 1, true, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, RIGHT);// 프래임 사이 호모그래피 구하기
						}

						if (m_nCurrentProcessingNum > 1)
						{
							GethomographyNew(m_arrPtPreL, m_homoL, m_homoL_pre, 50, 10, 1, false, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, LEFT);// 프래임 사이 호모그래피 구하기
							GethomographyNew(m_arrPtPreR, m_homoR, m_homoR_pre, 50, 10, 1, false, m_bDebug, m_bEnlarge, m_Edit_EnlargeRate, RIGHT);// 프래임 사이 호모그래피 구하기
						}

						m_CPL_cur = m_homoL * m_CPL_pre;

						m_CPR_cur = m_homoR * m_CPR_pre;

						m_homoL_pre = m_homoL;
						m_homoR_pre = m_homoR;
					}
				}

				QueryPerformanceCounter((LARGE_INTEGER*)&TimeEnd_EstimateHomo);

				// 현재 이미지 이전 이미지로 이동

				m_MImgLPreGray = m_MImgLCurGray.clone();
				m_MImgRPreGray = m_MImgRCurGray.clone();
				
				// 현재 CameraPath 이전 CameraPath로 이동

				m_CPL_pre = m_CPL_cur.clone();
				m_CPR_pre = m_CPR_cur.clone();

				//***************************************************************************
				//********* Step 2. Homography Refinement ***********************************
				//***************************************************************************
				
				if (m_bStopView == true)
				{
					m_warpCPL = m_CPL_cur.inv();
					m_warpCPR = m_Homo * m_CPR_cur.inv();
				}

				warpPerspective(m_MImgLCurColor, m_MImgLTransColor, m_warpCPL, Size(m_nW_Total, m_nH_Total), INTER_LINEAR);
				warpPerspective(m_MImgRCurColor, m_MImgRTransColor, m_warpCPR, Size(m_nW_Total, m_nH_Total), INTER_LINEAR);

				warpPerspective(m_MImgMask, m_MImgLTransMask, m_warpCPL, Size(m_nW_Total, m_nH_Total), 0);
				warpPerspective(m_MImgMask, m_MImgRTransMask, m_warpCPR, Size(m_nW_Total, m_nH_Total), 0);
				
				warpPerspective(m_MImgMaskSmall, m_MImgLTransMaskSmall, m_warpCPL, Size(m_nW_Total, m_nH_Total), 0);
				warpPerspective(m_MImgMaskSmall, m_MImgRTransMaskSmall, m_warpCPR, Size(m_nW_Total, m_nH_Total), 0);

				warpPerspective(m_MImgEdgeMask, m_MImgEdgeLTransMask, m_warpCPL, Size(m_nW_Total, m_nH_Total), 0);
				warpPerspective(m_MImgEdgeMask, m_MImgEdgeRTransMask, m_warpCPR, Size(m_nW_Total, m_nH_Total), 0);

				cvtColor(m_MImgLTransColor, m_MImgLTransGray, CV_BGR2GRAY);
				cvtColor(m_MImgRTransColor, m_MImgRTransGray, CV_BGR2GRAY);



				QueryPerformanceCounter((LARGE_INTEGER*)&TimeStart_CorrectHomo);
				
				/*if (m_nCurrentProcessingNum >= 1)
				{
					if (m_arrPtPreR.size() > 0)
						CorrectHomography(m_arrPtPreR, m_bDebug);
				}
*/
				QueryPerformanceCounter((LARGE_INTEGER*)&TimeEnd_CorrectHomo);

				//***************************************************************************
				//********* Step 3. Warp ****************************************************
				//***************************************************************************
				QueryPerformanceCounter((LARGE_INTEGER*)&TimeStart_Warp);

				WarpHomographyForVideo(m_warpCPL, m_warpCPR, m_nHL, m_nWL, m_nHR, m_nWR, m_bMultiBandBlend, m_bDebug);

				QueryPerformanceCounter((LARGE_INTEGER*)&TimeEnd_Warp);
				QueryPerformanceCounter((LARGE_INTEGER*)&TimeEnd_Total);
				
				//realse Data
				result_Total = (double)(TimeEnd_Total - TimeStart_Total) / (double)TimeFreq;
				result_EstimateHomo = (double)(TimeEnd_EstimateHomo - TimeStart_EstimateHomo) / (double)TimeFreq;
				result_CorrectHomo = (double)(TimeEnd_CorrectHomo - TimeStart_CorrectHomo) / (double)TimeFreq;
				result_Warp = (double)(TimeEnd_Warp - TimeStart_Warp) / (double)TimeFreq;

				// 리스트 스크롤 이동
				if (m_nCurrentProcessingNum >= 0 && m_nCurrentProcessingNum < 30)
				{
					int nCount = m_List_Video.GetItemCount();
					m_List_Video.EnsureVisible(m_ListIdx_Video - 1, FALSE);

					// 리스트에 시간 출력.
					CString strFrame, strResult_Total, strResult_EstimateHomo, strResult_CorrectHomo, strResult_Warp;
					strFrame.Format(_T("%d"), m_nCurrentProcessingNum);
					strResult_Total.Format(_T("%4.3f"), result_Total);
					strResult_EstimateHomo.Format(_T("%4.3f"), result_EstimateHomo);
					strResult_CorrectHomo.Format(_T("%4.3f"), result_CorrectHomo);
					strResult_Warp.Format(_T("%4.3f"), result_Warp);

					m_List_Video.SetItemState(m_ListIdx_Video, LVIS_SELECTED, LVIS_SELECTED);
					m_List_Video.EnsureVisible(m_ListIdx_Video, FALSE);

					m_List_Video.InsertItem(m_ListIdx_Video, strFrame);
					m_List_Video.SetItem(m_ListIdx_Video, 1, LVIF_TEXT, strResult_Total, NULL, NULL, NULL, NULL);
					m_List_Video.SetItem(m_ListIdx_Video, 2, LVIF_TEXT, strResult_EstimateHomo, NULL, NULL, NULL, NULL);
					m_List_Video.SetItem(m_ListIdx_Video, 3, LVIF_TEXT, strResult_CorrectHomo, NULL, NULL, NULL, NULL);
					m_List_Video.SetItem(m_ListIdx_Video, 4, LVIF_TEXT, strResult_Warp, NULL, NULL, NULL, NULL);
				}

				delete [] Image1D_L;
				delete[] Image1D_R;
				
				CString DispFrameNum;
				DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nAviLength_L-1);
				SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);
				
				m_nCurrentProcessingNum++;
			}
			else
				bExit = true;

			if((DWORD)m_nCurrentProcessingNum >= m_si_L.dwLength || (DWORD)m_nCurrentProcessingNum >= m_si_R.dwLength || bExit || m_nCurrentProcessingNum > m_nEnd)
			{
				// Warp이미지 제거
				free_cmatrix(m_ImgWarpR, WARP_H, WARP_W);
				free_cmatrix(m_ImgWarpG, WARP_H, WARP_W);
				free_cmatrix(m_ImgWarpB, WARP_H, WARP_W);
	
				KillTimer(TID_AVI_RUN);

				SetDlgItemText(IDC_RUN, _T("Run"));

				m_SourceType0Ctrl.EnableWindow(TRUE);
				m_SourceType1Ctrl.EnableWindow(TRUE);
				m_SourceType2Ctrl.EnableWindow(TRUE);

				m_bAVITimerRun = FALSE;

				break;
			}
////////
		}
		else if(nIDEvent == TID_SEQUENCE_RUN)
		{
			Sleep(m_nSleep);

			if(m_nCurrentProcessingNum > m_nEnd) 
			{
				KillTimer(TID_SEQUENCE_RUN);

				SetDlgItemText(IDC_RUN, _T("Run"));

				m_SourceType0Ctrl.EnableWindow(TRUE);
				m_SourceType1Ctrl.EnableWindow(TRUE);
				m_SourceType2Ctrl.EnableWindow(TRUE);

				m_bSequenceTimerRun = false;

				break;
			}

			m_FileListCtrl.SetItemState(m_nCurrentProcessingNum, LVIS_SELECTED, LVIS_SELECTED);
			m_FileListCtrl.EnsureVisible(m_nCurrentProcessingNum, FALSE);

			CString FileName = m_FileListCtrl.GetItemText(m_nCurrentProcessingNum, 1);
			FileName.MakeUpper();

			BYTE *Image1D = NULL;
			int nW, nH;

			if(FileName.Right(4) == _T(".BMP"))
			{
				Image1D = ReadBmp(FileName, &nW, &nH);
			}
			else if(FileName.Right(4) == _T(".256"))
			{
				nW = 256;
				nH = 256;

				Image1D = ReadRaw(FileName, nW, nH);
			}
			else if(FileName.Right(4) == _T(".320"))
			{
				nW = 320;
				nH = 240;

				Image1D = ReadRaw(FileName, nW, nH);
			}
			else if(FileName.Right(4) == _T(".640"))
			{
				nW = 640;
				nH = 480;

				Image1D = ReadRaw(FileName, nW, nH);
			}
			else if(FileName.Right(4) == _T(".JPG"))
			{
				Image1D = ReadJpeg(FileName, &nW, &nH);
			}

			if(Image1D)
			{
				// Processing
				//DisplayCimage1D(Image1D, nW, nH, 0, 0, false, true);
				if(pChild) 
				{
					CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

					if(pView)
					{
						if(m_nRunType == 0)
							m_Sr.Run(Image1D, nW, nH, pView->m_CurrentPoint.x, pView->m_CurrentPoint.y, (m_nZoomScale+1)<<1, m_nMask, m_nSearch, m_nGrow, m_bViewPsnr?true:false, m_bFirstFrame);
						else if(m_nRunType == 1)
							m_Fruc.Run(Image1D, nW, nH, (m_nZoomScale+1)<<1,m_nMask, m_nSearch, m_nGrow, m_nMeGap, m_nUseColor, m_nThreDiff,
							m_bViewPsnr?true:false, m_nPsnrType, m_bFirstFrame);
						else if(m_nRunType == 2)
							m_Stitch.Run(Image1D, nW, nH, m_bFirstFrame);
					}
					else
					{
						if(m_nRunType == 0)
							m_Sr.Run(Image1D, nW, nH, 100, 100, (m_nZoomScale+1)<<1, m_nMask, m_nSearch, m_nGrow, m_bViewPsnr?true:false, m_bFirstFrame);
						else if(m_nRunType == 1)
							m_Fruc.Run(Image1D, nW, nH, (m_nZoomScale+1)<<1,m_nMask, m_nSearch, m_nGrow, m_nMeGap, m_nUseColor, m_nThreDiff,
							m_bViewPsnr?true:false, m_nPsnrType, m_bFirstFrame);
						else if(m_nRunType == 2)
							m_Stitch.Run(Image1D, nW, nH, m_bFirstFrame);
					}
				}

				//
				m_bFirstFrame = false;
				delete [] Image1D;
			}
			
			CString DispFrameNum;
			DispFrameNum.Format(_T("%d(0,%d)"), m_nCurrentProcessingNum, m_nSequenceLength-1);
			SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

			m_nCurrentProcessingNum++;

		}
		else if(nIDEvent == TID_CAM_RUN)
		{
			capGrabFrame(m_hWndCap);

			DisplayCimage2DColor(m_CurrentImageRed, m_CurrentImageGreen, m_CurrentImageBlue, 
				m_nGW, m_nGH, 0, 0, false, true);

			CString SelFolder;
			CString FileName;

			GetDlgItemText(IDC_DISP_FOLDER2, SelFolder);

			if(m_bGrabOn && !SelFolder.IsEmpty())
			{
				if(m_nElapsedTime > m_nGap)
				{
					if(m_nFileType == 0)
					{
						FileName.Format(_T("%s\\aa%07d.jpg"), SelFolder, m_nCurrentProcessingNum);
						SaveImage2DColorJpeg(FileName, 
							m_CurrentImageRed, m_CurrentImageGreen, m_CurrentImageBlue,
							m_nGW, m_nGH, true, 100);
					}
					else if(m_nFileType == 1)
					{
						FileName.Format(_T("%s\\aa%07d.bmp"), SelFolder, m_nCurrentProcessingNum);
						SaveImage2DColorBmp(FileName, 
							m_CurrentImageRed, m_CurrentImageGreen, m_CurrentImageBlue,
							m_nGW, m_nGH);
					}
					else if(m_nFileType == 2)
					{
						FileName.Format(_T("%s\\aa%07d.%d"), SelFolder, m_nCurrentProcessingNum, m_nGW);
					}

					CString Msg;
					Msg.Format(_T("Grabbing(%7d)"), m_nCurrentProcessingNum);
					SetDlgItemText(IDC_GRAB, Msg);

					m_nCurrentProcessingNum++;

					m_nElapsedTime = 0;
				}
			}
			else
			{
				m_bGrabOn = false;
				SetDlgItemText(IDC_GRAB, _T("Grab"));
			}

			QueryPerformanceCounter((LARGE_INTEGER*)&m_TimeEnd);
			double timeDiff = (double)(m_TimeEnd - m_TimeStart)/(double)m_TimeFreq; 

			m_nElapsedTime += (int)(timeDiff*1000);

			QueryPerformanceFrequency((LARGE_INTEGER*)&m_TimeFreq);
			QueryPerformanceCounter((LARGE_INTEGER*)&m_TimeStart);
		}

		if(m_bInitSaveAvi && m_bSaveScreen)
		{
			BYTE *SaveAviFrameImage = new BYTE [GetBmp24Size(m_nSaveAviW, m_nSaveAviH)];
			int x, y;

			if(pChild) 
			{
				CImageProcessingView *pView = (CImageProcessingView *)pChild->GetActiveView();

				if(pView)
				{
					int Offset;
					for(y = 0 ; y < m_nSaveAviH ; y++)
					{
						Offset = (m_nSaveAviW*3+3)/4*4 * (m_nSaveAviH-y-1);
						for(x = 0 ; x < m_nSaveAviW ; x++)
						{
							pView->GetCurrPosColor1D(x, y, SaveAviFrameImage+Offset+x*3);
						}
					}
				}
			}

			AVIStreamWrite(m_psCompressedForSave, m_nCurrentProcessingNum-1, 1, 
				SaveAviFrameImage, GetBmp24Size(m_nSaveAviW, m_nSaveAviH),
				AVIIF_KEYFRAME, NULL, NULL);

			delete [] SaveAviFrameImage;
		}

		if (m_bStep)
		{
			m_bTimerPause = true;
		}
		break;
	}
	
	CDialog::OnTimer(nIDEvent);
}

void CMainProcessDlg::OnDblclkFileList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	POSITION index;

	index = m_FileListCtrl.GetFirstSelectedItemPosition();

	if((int)index-1 >= 0 && (int)index-1 < m_FileListCtrl.GetItemCount())
	{
		m_nStart = (int)(index-1);
		UpdateData(FALSE);
	}	
	
	*pResult = 0;

	CString FileName = m_FileListCtrl.GetItemText(m_nStart, 1);

	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	CChildFrame *pChild = (CChildFrame *)pMain->MDIGetActive();

	if(!pChild) return;
	CImageProcessingDoc *pDoc = (CImageProcessingDoc *)pChild->GetActiveDocument();
	if(pDoc)
		pDoc->FileOpenCur(FileName);
}

void CMainProcessDlg::OnPause() 
{
	m_bTimerPause = !m_bTimerPause;	
}

void CMainProcessDlg::OnSelFolder2() 
{
	ITEMIDLIST	*pidlBrowse;
	TCHAR pszPathname[MAX_PATH];

	BROWSEINFO BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;
	memset(&BrInfo, 0 , sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("Select Folder");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&BrInfo);

	CString SelFolder;
	if(pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, pszPathname);
		SelFolder.Format(_T("%s"), pszPathname);

		SetDlgItemText(IDC_DISP_FOLDER2, SelFolder);
	}
}
static LRESULT	CALLBACK capCallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr)
{
	UINT i;
	int x, y;

	CMainProcessDlg *pDlg = (CMainProcessDlg *)CMainProcessDlg::FromHandle(::GetParent(hWnd));

	for(i = 0 ; i < lpVHdr->dwBufferLength ; i+=3)
	{
		y = (lpVHdr->dwBufferLength-i-1)/(pDlg->m_nGW*3);
		x = i%(pDlg->m_nGW*3)/3;

		pDlg->m_CurrentImageRed[y][x] = *(lpVHdr->lpData+i+2);
		pDlg->m_CurrentImageGreen[y][x] = *(lpVHdr->lpData+i+1);
		pDlg->m_CurrentImageBlue[y][x] = *(lpVHdr->lpData+i);
	}

	return (LRESULT) true;
}

void CMainProcessDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	free_cmatrix(m_CurrentImageRed, m_nMH, m_nMW);
	free_cmatrix(m_CurrentImageGreen, m_nMH, m_nMW);
	free_cmatrix(m_CurrentImageBlue, m_nMH, m_nMW);	

	if(m_psForSave)
	{
		AVIStreamRelease(m_psForSave);
		m_psForSave = NULL;
	}

	if(m_psCompressedForSave)
	{
		AVIStreamRelease(m_psCompressedForSave);
		m_psCompressedForSave = NULL;
	}

	if(m_pfileForSave)
	{
		AVIFileRelease(m_pfileForSave);
		m_pfileForSave = NULL;
	}
		
	AVIFileExit();
}

void CMainProcessDlg::OnVs() 
{
	capDlgVideoSource(m_hWndCap);
}

void CMainProcessDlg::OnCamOn() 
{
	UpdateData(TRUE);

	if(m_nGW > m_nMW || m_nGH > m_nMH) return;

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun)
	{
		m_bCamTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;
		SetDlgItemText(IDC_CAM_ON, _T("Cam. OFF"));

		m_nCurrentProcessingNum = 0;
		m_bGrabOn = false;
		m_nElapsedTime = m_nGap+1;

		m_hWndCap = capCreateCaptureWindow(_T("Grab Window"), WS_CHILD,
			0, 0, m_nGW, m_nGH, GetSafeHwnd(), NULL);

		capSetCallbackOnFrame(m_hWndCap, capCallbackOnFrame);

		int Result;

		if(Result = capDriverConnect(m_hWndCap, 0))
		{
			capGetVideoFormat(m_hWndCap, &m_BmInfo, sizeof(BITMAPINFO));

			m_BmInfo.bmiHeader.biBitCount = 24;
			m_BmInfo.bmiHeader.biCompression = 0;
			m_BmInfo.bmiHeader.biWidth = m_nGW;
			m_BmInfo.bmiHeader.biHeight = m_nGH;
			m_BmInfo.bmiHeader.biSizeImage = m_BmInfo.bmiHeader.biWidth * m_BmInfo.bmiHeader.biHeight * 3;
		
			int rtn = capSetVideoFormat(m_hWndCap, &m_BmInfo, sizeof(BITMAPINFO));

			capOverlay(m_hWndCap, false);
			capPreview(m_hWndCap, false);
		}

		m_GwCtrl.EnableWindow(FALSE);
		m_GhCtrl.EnableWindow(FALSE);

		SetTimer(TID_CAM_RUN, 0, NULL);
	}
	else if(m_bCamTimerRun)
	{
		m_bCamTimerRun = false;
		SetDlgItemText(IDC_CAM_ON, _T("Cam. ON"));

		KillTimer(TID_CAM_RUN);

		m_GwCtrl.EnableWindow(TRUE);
		m_GhCtrl.EnableWindow(TRUE);

		capDriverDisconnect(m_hWndCap);
	}
}

void CMainProcessDlg::OnGrab() 
{
	m_bGrabOn = !m_bGrabOn;
	m_nElapsedTime = m_nGap+1;

	if(m_bGrabOn)
		SetDlgItemText(IDC_GRAB, _T("Grabbing"));
	else
		SetDlgItemText(IDC_GRAB, _T("Grab"));
}

void CMainProcessDlg::OnResetGNum() 
{
	m_nCurrentProcessingNum = 0;
}

void CMainProcessDlg::OnCustomdrawSleep(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_nSleep = m_SleepCtrl.GetPos();


	CString DispDelayTime;
	DispDelayTime.Format(_T("%dms"), m_nSleep);
	SetDlgItemText(IDC_DISP_SLEEP, DispDelayTime);
	
	*pResult = 0;
}

BOOL CMainProcessDlg::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE))
		pMsg->wParam = NULL;
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CMainProcessDlg::OnSelSourceLeft() 
{
	UpdateData(TRUE);
	
	CFileDialog dlg(TRUE, _T("*.avi"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("AVI 파일(*.avi)|*.avi||"));

	if (dlg.DoModal() == IDOK)
	{
		CString FileName = dlg.GetPathName();
		SetDlgItemText(IDC_DISP_SOURCE_LEFT, FileName);

		GetDlgItemText(IDC_DISP_SOURCE_LEFT, m_AviFileName_L);

		if (m_AviFileName_L.IsEmpty())
		{
			return;
		}

		if (AVIFileOpen(&m_pavi_L, m_AviFileName_L, OF_READ | OF_SHARE_DENY_NONE, NULL) != 0)
		{
			return;
		}
		AVIFileInfo(m_pavi_L, &m_fi_L, sizeof(AVIFILEINFO));

		m_nStitchW = m_fi_L.dwWidth;
		m_nStitchH = m_fi_L.dwHeight;

		UpdateData(FALSE);

		UINT stream;

		for (stream = 0; stream < m_fi_L.dwStreams; stream++)
		{
			AVIFileGetStream(m_pavi_L, &m_pstm_L, 0, stream);
			AVIStreamInfo(m_pstm_L, &m_si_L, sizeof(AVISTREAMINFO));

			if (m_si_L.fccType == streamtypeVIDEO)
			{
				m_pfrm_L = AVIStreamGetFrameOpen(m_pstm_L, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT);
				m_nAviLength_L = AVIStreamLength(m_pstm_L);
				break;
			}
		}

		if (stream == m_fi_L.dwStreams || m_pfrm_L == NULL)
		{
			AVIStreamRelease(m_pstm_L);
			AVIFileRelease(m_pavi_L);

			return;
		}

		m_nStart = 0;
		m_nEnd = m_nAviLength_L - 1;

		UpdateData(FALSE);

		/*CString DispFrameNum;
		DispFrameNum.Format(_T("(0,%d)"), m_nAviLength - 1);
		SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);*/

		///////////////////
	}
}

void CMainProcessDlg::SelFolder() 
{
	ITEMIDLIST	*pidlBrowse;
	TCHAR pszPathname[MAX_PATH];

	BROWSEINFO BrInfo;
	BrInfo.hwndOwner = GetSafeHwnd();
	BrInfo.pidlRoot = NULL;
	memset(&BrInfo, 0 , sizeof(BrInfo));
	BrInfo.pszDisplayName = pszPathname;
	BrInfo.lpszTitle = _T("Select Folder");
	BrInfo.ulFlags = BIF_RETURNONLYFSDIRS;
	pidlBrowse = ::SHBrowseForFolder(&BrInfo);

	CString SelFolder;
	if(pidlBrowse != NULL)
	{
		SHGetPathFromIDList(pidlBrowse, pszPathname);
		SelFolder.Format(_T("%s"), pszPathname);

		ReadFile(SelFolder);
		SetDlgItemText(IDC_DISP_SOURCE, SelFolder);
	}		
}

void CMainProcessDlg::ReadFile(CString SelFolder)
{
	m_FileListCtrl.DeleteAllItems();

	UpdateData(TRUE);
	ReadFile(SelFolder, m_bSubFolder?true:false);

	CString DispFrameNum;
	DispFrameNum.Format(_T("(0,%d)"), m_nSequenceLength-1);
	SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);
}

void CMainProcessDlg::ReadFile(CString SelFolder, bool bSubFolder)
{
	CFileFind Finder; 
	BOOL Continue;
	SHFILEINFO shInfo;

	CString Name;

	Continue = Finder.FindFile(SelFolder+_T("\\*.*"));

	CList<CString, CString &> listSubFolder;

	while(Continue)
	{
		Continue = Finder.FindNextFile();

		if(Finder.IsDirectory() || Finder.IsDots())
		{
			if(Finder.IsDirectory() && !Finder.IsDots())
			{
				listSubFolder.AddTail(Finder.GetFilePath());
			}

			continue;
		}

		Name = Finder.GetFileName();
		Name.MakeUpper();
		if(m_nSeqType == 1 && Name.Right(4) != _T(".BMP"))
			continue;
		else if(m_nSeqType == 2 && Name.Right(4) != _T(".JPG"))
			continue;
		else if(m_nSeqType == 3 && Name.Right(4) != _T(".PCX"))
			continue;
		else if(m_nSeqType == 4 && Name.Right(4) != _T(".256"))
			continue;
		else if(m_nSeqType == 5 && Name.Right(4) != _T(".320"))
			continue;
		else if(m_nSeqType == 6 && Name.Right(4) != _T(".640"))
			continue;
		else if(m_nSeqType == 7 && (Name.Right(4) != _T(".BMP") && Name.Right(4) != _T(".JPG")))
			continue;
		else if(m_nSeqType == 8 && (Name.Right(4) != _T(".BMP") && Name.Right(4) != _T(".JPG")
			&& Name.Right(4) != _T(".PCX") && Name.Right(4) != _T(".256") && Name.Right(4) != _T(".320") 
			&& Name.Right(4) != _T(".640")))
			continue;

		LV_ITEM lvItem;

		int nItemNum = m_FileListCtrl.GetItemCount();
			
		SHGetFileInfo(Finder.GetFilePath(),0,&shInfo,sizeof(SHFILEINFO),SHGFI_TYPENAME|SHGFI_SYSICONINDEX);

		lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
		lvItem.iItem = nItemNum;
		lvItem.iSubItem = 0;
		lvItem.iImage = shInfo.iIcon;
		lvItem.pszText = Name.GetBuffer(Name.GetLength());
		m_FileListCtrl.InsertItem(&lvItem);

		m_FileListCtrl.SetItemText(nItemNum, 1, Finder.GetFilePath());
	}

	m_nEnd = m_FileListCtrl.GetItemCount()-1;
	m_nSequenceLength = m_FileListCtrl.GetItemCount();
	UpdateData(FALSE);

	if(!bSubFolder) return;

	POSITION pos = listSubFolder.GetHeadPosition();

	if(!pos) return;

	while(pos)
	{
		CString SubFolder = listSubFolder.GetNext(pos);
		ReadFile(SubFolder, bSubFolder);
	}
}

void CMainProcessDlg::SelAvi() 
{
	//CFileDialog dlg(TRUE, _T("*.avi"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("AVI 파일(*.avi)|*.avi||"));
	//
	//if(dlg.DoModal() == IDOK) 
	//{
	//	CString FileName = dlg.GetPathName();
	//	SetDlgItemText(IDC_DISP_SOURCE, FileName);

	//	GetDlgItemText(IDC_DISP_SOURCE, m_AviFileName);

	//	if(m_AviFileName.IsEmpty())
	//	{
	//		return;
	//	}
 //
	//	if(AVIFileOpen(&m_pavi, m_AviFileName, OF_READ | OF_SHARE_DENY_NONE, NULL) != 0) 
	//	{
	//		return;
	//	}
	//	AVIFileInfo(m_pavi, &m_fi, sizeof(AVIFILEINFO));

	//	UINT stream;

	//	for(stream = 0 ; stream < m_fi.dwStreams ; stream++)
	//	{
	//		AVIFileGetStream(m_pavi, &m_pstm, 0, stream);
	//		AVIStreamInfo(m_pstm, &m_si, sizeof(AVISTREAMINFO)); 
 //
	//		if (m_si.fccType == streamtypeVIDEO) 
	//		{
	//			m_pfrm = AVIStreamGetFrameOpen(m_pstm, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT);
	//			m_nAviLength = AVIStreamLength(m_pstm);
	//			break;
	//		}
	//	}

	//	if(stream == m_fi.dwStreams || m_pfrm == NULL)
	//	{
	//		AVIStreamRelease(m_pstm);    
	//		AVIFileRelease(m_pavi);    

	//		return;
	//	}

	//	m_nStart = 0;
	//	m_nEnd = m_nAviLength-1;

	//	UpdateData(FALSE);

	//	CString DispFrameNum;
	//	DispFrameNum.Format(_T("(0,%d)"), m_nAviLength-1);
	//	SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);

	//	AVIStreamGetFrameClose(m_pfrm);
	//	AVIStreamRelease(m_pstm);    
	//	AVIFileRelease(m_pavi);    

	//	/////////////////
	//}	
}

void CMainProcessDlg::SelYuv() 
{
	CFileDialog dlg(TRUE, _T("*.yuv"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("YUV 파일(*.yuv)|*.yuv||"));
	
	if(dlg.DoModal() == IDOK) 
	{
		CString FileName = dlg.GetPathName();
		SetDlgItemText(IDC_DISP_SOURCE, FileName);

		CYuvOptionDlg dlg;
		if(dlg.DoModal() == IDOK)
		{
			m_nYuvW = dlg.m_nW;
			m_nYuvH = dlg.m_nH;
			m_nYuvType = dlg.m_nYuvType;


			GetDlgItemText(IDC_DISP_SOURCE, m_YuvFileName);

			if(m_YuvFileName.IsEmpty() || m_nYuvType < 0)
			{
				m_nYuvType = -1;
				return;
			}
				

			BOOL bOpen = m_YuvFile.Open(m_YuvFileName, CFile::modeRead | CFile::typeBinary);
			if(!bOpen)
			{
				m_nYuvType = -1;
				return;
			}

			m_nYuvLength = m_YuvFile.GetLength()/(m_nYuvW*m_nYuvH*3/2);

			m_YuvFile.Close();

			m_nStart = 0;
			m_nEnd = m_nYuvLength-1;

			UpdateData(FALSE);

			CString DispFrameNum;
			DispFrameNum.Format(_T("(0,%d)"), m_nYuvLength-1);
			SetDlgItemText(IDC_TOTAL_FRAME, DispFrameNum);
		}
		else
			m_nYuvType = -1;
	}
}

void CMainProcessDlg::OnRun() 
{
	UpdateData(TRUE);
	
	if(m_nSourceType == 0)
		SequenceRun();
	else if (m_nSourceType == 1)
		AviRun();
	else if(m_nSourceType == 2)
		YuvRun();
	
}

void CMainProcessDlg::SequenceRun() 
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun)
	{
		m_bSequenceTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;
		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nCurrentProcessingNum = m_nStart;
		
		SetTimer(TID_SEQUENCE_RUN, 0, NULL);
	}
	else if(m_bSequenceTimerRun)
	{
		m_bSequenceTimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);

		KillTimer(TID_SEQUENCE_RUN);
	}
}

void CMainProcessDlg::AviRun() 
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun)
	{
		m_bAVITimerRun = true;
		m_bTimerPause = false;
		m_List_Video.DeleteAllItems();

		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nCurrentProcessingNum = m_nStart;

		SetTimer(TID_AVI_RUN, 0, NULL);
	}
	else if(m_bAVITimerRun)
	{
		KillTimer(TID_AVI_RUN);

		m_bAVITimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);
	}
}

void CMainProcessDlg::YuvRun() 
{
	UpdateData(TRUE);

	if(!m_bSequenceTimerRun && !m_bCamTimerRun && !m_bAVITimerRun && !m_bYUVTimerRun)
	{
		m_bYUVTimerRun = true;
		m_bTimerPause = false;
		m_bFirstFrame = true;

		GetDlgItemText(IDC_DISP_SOURCE, m_YuvFileName);

		if(m_YuvFileName.IsEmpty() || m_nYuvType < 0)
		{
			m_bYUVTimerRun = false;
			return;
		}

		BOOL bOpen = m_YuvFile.Open(m_YuvFileName, CFile::modeRead | CFile::typeBinary);
		if(!bOpen)
		{
			m_bYUVTimerRun = false;
			return;
		}

		SetDlgItemText(IDC_RUN, _T("Stop"));

		m_SourceType0Ctrl.EnableWindow(FALSE);
		m_SourceType1Ctrl.EnableWindow(FALSE);
		m_SourceType2Ctrl.EnableWindow(FALSE);

		m_nCurrentProcessingNum = m_nStart;

		m_YuvFile.Seek(m_nStart*(m_nYuvW*m_nYuvH*3/2), CFile::begin);
		
		SetTimer(TID_YUV_RUN, 0, NULL);
	}
	else if(m_bYUVTimerRun)
	{
		m_bYUVTimerRun = false;
		SetDlgItemText(IDC_RUN, _T("Run"));

		m_SourceType0Ctrl.EnableWindow(TRUE);
		m_SourceType1Ctrl.EnableWindow(TRUE);
		m_SourceType2Ctrl.EnableWindow(TRUE);

		KillTimer(TID_YUV_RUN);

		m_YuvFile.Close();
	}
}

void CMainProcessDlg::OnSaveScreen() 
{
	UpdateData(TRUE);

	if(m_bSaveScreen)
	{
		WORD wVer;
		bool bInit;
	
		wVer = HIWORD(VideoForWindowsVersion());
		if (wVer < 0x010a)
			bInit = false;
		else
			bInit = true;

		if(m_psForSave != NULL)	{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)	{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)	{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}

		if(bInit)
		{
			bInit = AVIFileInitFunction();
		}

		m_bInitSaveAvi = bInit;
	}
	else
	{
		m_bInitSaveAvi = false;

		if(m_psForSave)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}

		if(m_pfileForSave)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
	}
}

bool CMainProcessDlg::AVIFileInitFunction()
{
	AVISTREAMINFO strhdr;
	HRESULT hr;
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};

//	char tmpName[MAX_PATH];

	CFileDialog FileSaveDlg(FALSE, "*.avi", "Data.avi", 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
			"AVI Files(*.avi)|*.avi||");

	if(FileSaveDlg.DoModal() != IDOK)
		return false;

	CSaveScreenOpt dlg;

	m_nSaveAviW = 640;
	m_nSaveAviH = 480;

	if(dlg.DoModal() == IDOK)
	{
		m_nSaveAviW = dlg.m_nW;
		m_nSaveAviH = dlg.m_nH;
	}

	m_BmInfoForSaveAvi.biSize = sizeof(BITMAPINFOHEADER);
	m_BmInfoForSaveAvi.biWidth = m_nSaveAviW;
	m_BmInfoForSaveAvi.biHeight = m_nSaveAviH;
	m_BmInfoForSaveAvi.biPlanes = 1;
	m_BmInfoForSaveAvi.biBitCount = 24;
	m_BmInfoForSaveAvi.biCompression = BI_RGB;
	m_BmInfoForSaveAvi.biSizeImage = (m_BmInfoForSaveAvi.biWidth*3+3)/4*4  * m_BmInfoForSaveAvi.biHeight;
	m_BmInfoForSaveAvi.biXPelsPerMeter = 0;
	m_BmInfoForSaveAvi.biYPelsPerMeter = 0;
	m_BmInfoForSaveAvi.biClrUsed = 0;
	m_BmInfoForSaveAvi.biClrImportant = 0;
			
	if(!access(FileSaveDlg.GetPathName(), 00))
	{
	/*	CFile DelFile;
		BOOL bOpen;
		
		bOpen = DelFile.Open(FileSaveDlg.GetPathName(), CFile::modeWrite | CFile::typeBinary | CFile::modeCreate);
	
		if(!bOpen) return false;

		DelFile.Close();*/
	}
	

	hr = AVIFileOpen(&m_pfileForSave,		    // returned file pointer
			   //tmpName,		            // file name
			   FileSaveDlg.GetPathName(),
			   OF_CREATE | OF_WRITE,	    // mode to open file with
			   NULL);			    // use handler determined
						// from file extension....
	if(hr) 
	{
		if(m_pfileForSave != NULL) 
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

	// The video stream will run in 30ths of a second....

	_fmemset(&strhdr, 0, sizeof(strhdr));
//	memset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType                = streamtypeVIDEO;// stream type
	strhdr.fccHandler             = 0;
	strhdr.dwScale                = 1;
	
	strhdr.dwRate                 = 30;		    // fps

    strhdr.dwSuggestedBufferSize  = m_BmInfoForSaveAvi.biSizeImage;

	// And create the stream;
	hr = AVIFileCreateStream(m_pfileForSave,		    // file pointer
					 &m_psForSave,		    // returned stream pointer
					 &strhdr);	    // stream header

	if (hr != AVIERR_OK) {
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

//	_fmemset(&opts, 0, sizeof(opts));

		memset(&opts, 0, sizeof(opts));
		opts.fccType = streamtypeVIDEO;
		//opts.fccHandler             = getFOURCC(_compressor);
		//opts.fccHandler  = 0;
		opts.fccHandler            = mmioFOURCC('D','I','B',' '); // Uncompressed
		//opts.fccHandler             = mmioFOURCC('C','V','I','D'); // Cinpak
		//opts.fccHandler             = mmioFOURCC('I','V','3','2'); // Intel video 3.2
	//	opts.fccHandler             = mmioFOURCC('M','S','V','C'); // Microsoft video 1
		//opts.fccHandler             = mmioFOURCC('I','V','5','0'); // Intel video 5.0
		opts.dwKeyFrameEvery = 0;//5
		opts.dwQuality		= 7500;
		//opts.dwBytesPerSecond
		opts.dwFlags                = 8;//AVICOMPRESSF_KEYFRAMES;
		//opts.lpFormat 
		//opts.cbFormat
		//opts.lpParms
		//opts.cbParms = 4;
		//opts.dwInterleaveEvery

	///////////////////
	if(!AVISaveOptions(NULL, 0, 1, &m_psForSave, (LPAVICOMPRESSOPTIONS FAR *) &aopts))  // option dlg
	{
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

	hr = AVIMakeCompressedStream(&m_psCompressedForSave, m_psForSave, &opts, NULL);
	if (hr != AVIERR_OK) {
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}
		
		return false;
	}

    hr = AVIStreamSetFormat(m_psCompressedForSave, 0,
			   &m_BmInfoForSaveAvi,	    // stream format
			   m_BmInfoForSaveAvi.biSize +   // format size
			   m_BmInfoForSaveAvi.biClrUsed * sizeof(RGBQUAD));

	if (hr != AVIERR_OK) {
		if(m_psForSave != NULL)
		{
			AVIStreamRelease(m_psForSave);
			m_psForSave = NULL;
		}

		if(m_psCompressedForSave != NULL)
		{
			AVIStreamRelease(m_psCompressedForSave);
			m_psCompressedForSave = NULL;
		}
		
		if(m_pfileForSave != NULL)
		{
			AVIFileRelease(m_pfileForSave);
			m_pfileForSave = NULL;
		}

		
		return false;
	}

	return true;
}


void CMainProcessDlg::OnSelSourceRight()
{
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, _T("*.avi"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("AVI 파일(*.avi)|*.avi||"));

	if (dlg.DoModal() == IDOK)
	{
		CString FileName = dlg.GetPathName();
		SetDlgItemText(IDC_DISP_SOURCE_RIGHT, FileName);

		GetDlgItemText(IDC_DISP_SOURCE_RIGHT, m_AviFileName_R);

		if (m_AviFileName_R.IsEmpty())
		{
			return;
		}

		if (AVIFileOpen(&m_pavi_R, m_AviFileName_R, OF_READ | OF_SHARE_DENY_NONE, NULL) != 0)
		{
			return;
		}
		AVIFileInfo(m_pavi_R, &m_fi_R, sizeof(AVIFILEINFO));

		UINT stream;

		for (stream = 0; stream < m_fi_R.dwStreams; stream++)
		{
			AVIFileGetStream(m_pavi_R, &m_pstm_R, 0, stream);
			AVIStreamInfo(m_pstm_R, &m_si_R, sizeof(AVISTREAMINFO));

			if (m_si_R.fccType == streamtypeVIDEO)
			{
				m_pfrm_R = AVIStreamGetFrameOpen(m_pstm_R, (LPBITMAPINFOHEADER)AVIGETFRAMEF_BESTDISPLAYFMT);
				m_nAviLength_R = AVIStreamLength(m_pstm_R);
				break;
			}
		}

		if (stream == m_fi_R.dwStreams || m_pfrm_R == NULL)
		{
			AVIStreamRelease(m_pstm_R);
			AVIFileRelease(m_pavi_R);

			return;
		}

		m_nStart = 0;
		m_nEnd = m_nAviLength_R - 1;

		UpdateData(FALSE);

		///////////////////
	}
}

void CMainProcessDlg::WarpHomographyForVideo(Mat& BL, Mat& BR, int nHL, int nWL, int nHR, int nWR, bool bMultiBandBlend, bool bDebug)
{
	int y, x;
	
	if (bMultiBandBlend == true)
	{
		Mat MimgL, MimgR;
		Mat result, result_mask;

		// mask 제작
		MakeMaskForBlend();

		MultiBandBlender blender(false, 2);

		blender.prepare(Rect(0, 0, m_nW_Blend, m_nH_Blend));

		blender.feed(m_MImgLBlendColor, m_MImgLBlendMask, Point(0, 0));
		blender.feed(m_MImgRBlendColor, m_MImgRBlendMask, Point(0, 0));

		blender.blend(result, m_MImgBlendMask);

		result.convertTo(m_MImgBlend, CV_8U);

		//imshow("result", m_MImgBlend);
		//imshow("result mask", m_MImgBlendMask);

		//// warp
		m_MImgWarp = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8U, Scalar(0));
		m_MImgRTransColor.copyTo(m_MImgWarp, m_MImgRTransMaskSmall);
		m_MImgLTransColor.copyTo(m_MImgWarp, m_MImgLTransMaskSmall);
		m_MImgBlend(Rect(1, 1, m_nW_Blend-1, m_nH_Blend-1)).copyTo(m_MImgWarp(Rect(m_offsetX_Blend+1, m_offsetY_Blend+1, m_nW_Blend-1, m_nH_Blend-1)), m_MImgBlendMask(Rect(1, 1, m_nW_Blend-1, m_nH_Blend-1)));

		MatToByte2DColor(m_MImgWarp.data, m_ImgWarpR, m_ImgWarpG, m_ImgWarpB, m_nH_Total, m_nW_Total);

		if (bDebug == false)
		{
			DisplayCimage2DColor(m_ImgWarpR, m_ImgWarpG, m_ImgWarpB, m_nW_Total, m_nH_Total, 0, 0, true, true);
		}
	}
	//---image warping
	
	if (bMultiBandBlend == false)
	{
		m_MImgWarp = Mat(cvSize(m_nW_Total, m_nH_Total), CV_8U, Scalar(0));
		m_MImgLTransColor.copyTo(m_MImgWarp, m_MImgLTransMaskSmall);
		m_MImgRTransColor.copyTo(m_MImgWarp, m_MImgRTransMaskSmall);

		MatToByte2DColor(m_MImgWarp.data, m_ImgWarpR, m_ImgWarpG, m_ImgWarpB, m_nH_Total, m_nW_Total);

		// display Image
		if (bDebug == false)
		{
			DisplayCimage2DColor(m_ImgWarpR, m_ImgWarpG, m_ImgWarpB, m_nW_Total, m_nH_Total, 0, 0, true, true);
		}
	}

	if (bDebug == true)
	{
		Vec3b BGR_R, BGR_L;

		BYTE** imgBlend_R, ** imgBlend_G, ** imgBlend_B;
		//---image warping
		Point2f inputPoint, transPoint;
	
		imgBlend_R = cmatrix(m_nH_Total, m_nW_Total);
		imgBlend_G = cmatrix(m_nH_Total, m_nW_Total);
		imgBlend_B = cmatrix(m_nH_Total, m_nW_Total);

		for (y = 0; y < m_nH_Total; y++)
			for (x = 0; x < m_nW_Total; x++) {
				imgBlend_R[y][x] = 0;
				imgBlend_G[y][x] = 0;
				imgBlend_B[y][x] = 0;
			}

		for (y = 0; y < m_nH_Total; y++)
			for (x = 0; x < m_nW_Total; x++) {

				BGR_L = m_MImgLTransColor.at<Vec3b>(y, x);
				BGR_R = m_MImgRTransColor.at<Vec3b>(y, x);

				imgBlend_B[y][x] = (BGR_L.val[0] + BGR_R.val[0]) / 2.;
				imgBlend_G[y][x] = (BGR_L.val[1] + BGR_R.val[1]) / 2.;
				imgBlend_R[y][x] = (BGR_L.val[2] + BGR_R.val[2]) / 2.;
			}

		DisplayCimage2DColor(m_ImgWarpR, m_ImgWarpG, m_ImgWarpB, m_nW_Total, m_nH_Total, 0, 0, false, true, 50);
		DisplayCimage2DColor(imgBlend_R, imgBlend_G, imgBlend_B, m_nW_Total, m_nH_Total, m_nW_Total / 2, 0, true, true, 50);

		free_cmatrix(imgBlend_R, m_nH_Total, m_nW_Total);
		free_cmatrix(imgBlend_G, m_nH_Total, m_nW_Total);
		free_cmatrix(imgBlend_B, m_nH_Total, m_nW_Total);
	}
}

void  CMainProcessDlg::GethomographySIFT(Mat& dstImg, int nH_dst, int nW_dst, Mat& srcImg, int nH_src, int nW_src, Mat& homo)
{
	//Detect the keypoints using SURF Detector

	SiftFeatureDetector detector(3000);
	std::vector< KeyPoint > keypoints_1, keypoints_2;
	detector.detect(dstImg, keypoints_1);
	detector.detect(srcImg, keypoints_2);

	//-- Step 2: Calculate descriptors (feature vectors)

	SiftDescriptorExtractor extractor;
	Mat descriptors_1, descriptors_2;
	extractor.compute(dstImg, keypoints_1, descriptors_1);
	extractor.compute(srcImg, keypoints_2, descriptors_2);

	//-- Step 3: Matching descriptor vectors using FLANN matcher

	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);
	double max_dist = 0; double min_dist = 100;

	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}

	vector< Point2f > GodPoint1, GodPoint2;
	for (unsigned int i = 0; i < matches.size(); i++)
	{
		//-- Get the keypoints from the good matches
		GodPoint1.push_back(keypoints_1[matches[i].queryIdx].pt);
		GodPoint2.push_back(keypoints_2[matches[i].trainIdx].pt);
	}

	homo = findHomography(GodPoint2, GodPoint1, CV_RANSAC);
}

void CMainProcessDlg::GethomographyGoodFeature(vector<Point2f>& arrPtPre, Mat& homo, Mat& homo_pre, int thresh_distance, int minFeature, double ransac, bool bFirst, bool bDebug, bool bEnlarge, int nEnlargeRate, LR bLR)
{
	int i, j;
	int x, y;

	// value for goodFeatureToTrack

	Mat M_ImgPreGray;
	Mat M_ImgCurGray;

	vector<Point2f> arrPt, arrPtTrack;
	vector<Point2f> arrPtOptical, arrPtTrackOptical;

	// value for optical Flow
	int nRawPoint, nOpticalPt, nInlierPt, nHomoPt;

	// value for filtering point
	vector<Point2f>  arrGodPt, arrGodPtTrack;

	int nH_Warp = 600;
	int nW_Warp = 1200;

	// value for debug
	int nDebugOffset;

	//-------------------------------------------------------------
	if (bLR == LEFT)
	{
		M_ImgPreGray = m_MImgLPreGray.clone();
		M_ImgCurGray = m_MImgLCurGray.clone();
	}

	if (bLR == RIGHT)
	{
		M_ImgPreGray = m_MImgRPreGray.clone();
		M_ImgCurGray = m_MImgRCurGray.clone();
	}

	// =====================================
	// GoodFeatureToTrack!!!!!
	// =====================================

	int64 TimeStart, TimeEnd;

	double qualityLevel = 0.01;
	double minDistance = 30;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	double maxCorners = 1000;
	double distance;

	goodFeaturesToTrack(M_ImgPreGray, arrPt, maxCorners, qualityLevel, minDistance, Mat(), blockSize, useHarrisDetector, k);

	nRawPoint = arrPt.size();

	// sparsely Feature

	// **Debug : 첫 특징점 출력
	if (bDebug == true)
	{
		// 왼쪽 이미지 오프셋
		if (bLR == LEFT)
			nDebugOffset = (nH_Warp / 2);
		// 오른쪽 이미지 오프셋
		if (bLR == RIGHT)
			nDebugOffset = (nH_Warp / 2) + (m_nHL / 2);

		BYTE** imgDebug;
		CString strNum;
		strNum.Format(_T("%d"), nRawPoint);

		imgDebug = cmatrix(m_nHL, m_nWL);

		MatToByteGray(M_ImgPreGray.data, imgDebug, m_nHL, m_nWL);

		for (i = 0; i < nRawPoint; i++)
			DrawCross(imgDebug, m_nWL, m_nHL, CPoint(arrPt[i].x, arrPt[i].y), 0);

		DrawTextOnImag2DGray(imgDebug, m_nWL, m_nHL, 0, 0, strNum, DT_LEFT, 100, true);

		DisplayCimage2D(imgDebug, m_nWL, m_nHL, 0, nDebugOffset, false, true, 50);

		free_cmatrix(imgDebug, m_nHL, m_nWL);
	}

	// =====================================
	// Optical Flow !!!!!
	// =====================================

	vector<uchar> status(nRawPoint);
	vector<float> err(nRawPoint);
	int threshErr = 15;

	if (nRawPoint > 0)
	{
		calcOpticalFlowPyrLK(M_ImgPreGray, M_ImgCurGray, arrPt, arrPtTrack, status, err, Size(9, 9), 3);
	}

	if (nRawPoint > 0)
	{
		for (i = 0; i < nRawPoint; i++)
		{
			if (status[i] == 1 && err[i] < threshErr)
			{
				arrPtOptical.push_back(arrPt[i]);
				arrPtTrackOptical.push_back(arrPtTrack[i]);
			}
		}
	}

	nOpticalPt = arrPtOptical.size();

	// **Debug : 옵티컬 플로우 출력
	if (bDebug == true)
	{
		BYTE** imgDebugR, ** imgDebugG, ** imgDebugB;
		CString strNum;

		strNum.Format(_T("%d / %d"), nOpticalPt, nRawPoint);

		imgDebugR = cmatrix(m_nHL / 2, m_nWL / 2);
		imgDebugG = cmatrix(m_nHL / 2, m_nWL / 2);
		imgDebugB = cmatrix(m_nHL / 2, m_nWL / 2);

		for (i = 0; i < m_nHL / 2; i++)
			for (j = 0; j < m_nWL / 2; j++)
			{
				imgDebugR[i][j] = abs(m_MImgLCurGray.at<uchar>(i * 2, j * 2) - m_MImgLPreGray.at<uchar>(i * 2, j * 2));
				imgDebugG[i][j] = abs(m_MImgLCurGray.at<uchar>(i * 2, j * 2) - m_MImgLPreGray.at<uchar>(i * 2, j * 2));
				imgDebugB[i][j] = abs(m_MImgLCurGray.at<uchar>(i * 2, j * 2) - m_MImgLPreGray.at<uchar>(i * 2, j * 2));
			}

		if (bEnlarge == true)
		{
			int R, G, B;
			Point2f dirVec, enlargePt;

			for (i = 0; i < nRawPoint; i++)
			{
				if (status[i] == 1 && err[i] < threshErr)
				{
					R = 255; G = 0; B = 0;
				}
				else
				{
					R = 255; G = 0; B = 255;
				}

				dirVec.y = arrPtTrack[i].y - arrPt[i].y;
				dirVec.x = arrPtTrack[i].x - arrPt[i].x;

				enlargePt.x = (nEnlargeRate * dirVec.x) + arrPt[i].x;
				enlargePt.y = (nEnlargeRate * dirVec.y) + arrPt[i].y;

				if (enlargePt.x >= 0 && enlargePt.x < m_nWL && enlargePt.y >= 0 && enlargePt.y < m_nHL)
				{
					DrawLine(imgDebugR, m_nWL / 2, m_nHL / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, R);
					DrawLine(imgDebugG, m_nWL / 2, m_nHL / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, G);
					DrawLine(imgDebugB, m_nWL / 2, m_nHL / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, B);
				}
			}
		}
		else
		{
			int R, G, B;

			for (i = 0; i < nRawPoint; i++)
			{
				if (status[i] == 1 && err[i] < threshErr)
				{
					R = 255; G = 0; B = 0;
				}
				else
				{
					R = 255; G = 0; B = 255;
				}

				DrawLine(imgDebugR, m_nWL / 2, m_nHL / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, R);
				DrawLine(imgDebugG, m_nWL / 2, m_nHL / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, G);
				DrawLine(imgDebugB, m_nWL / 2, m_nHL / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, B);
			}
		}

		DrawTextOnImag2DGray(imgDebugR, m_nWL / 2, m_nHL / 2, 0, 0, strNum, DT_LEFT, 255, true);
		DrawTextOnImag2DGray(imgDebugG, m_nWL / 2, m_nHL / 2, 0, 0, strNum, DT_LEFT, 255, true);
		DrawTextOnImag2DGray(imgDebugB, m_nWL / 2, m_nHL / 2, 0, 0, strNum, DT_LEFT, 255, true);

		DisplayCimage2DColor(imgDebugR, imgDebugG, imgDebugB, m_nWL / 2, m_nHL / 2, (m_nWL / 2), nDebugOffset, false, true, 100);

		free_cmatrix(imgDebugR, m_nHL / 2, m_nWL / 2);
		free_cmatrix(imgDebugG, m_nHL / 2, m_nWL / 2);
		free_cmatrix(imgDebugB, m_nHL / 2, m_nWL / 2);
	}

	// =====================================
	// Get Homography !!!!!
	// =====================================
	Point2f pt_homoPre;

	arrGodPt.clear();
	arrGodPtTrack.clear();
	arrPtPre.clear();

	for (i = 0; i < nOpticalPt; i++)
		arrPtPre.push_back(arrPtTrackOptical[i]);

	if (bFirst == true)
	{
		homo = findHomography(arrPtOptical, arrPtTrackOptical, CV_RANSAC, ransac);
	}

	if (bFirst == false)
	{
		// ==============================================
		// Filtering Inlier using previous homography
		// ==============================================

		// 이전 호모 적용 갯수 체크 
		for (i = 0; i < nOpticalPt; i++)
		{
			GetPerspectivePoint(arrPtOptical[i], pt_homoPre, homo_pre);
			distance = GetDistance(arrPtTrackOptical[i].x, arrPtTrackOptical[i].y, pt_homoPre.x, pt_homoPre.y);

			if (distance < thresh_distance)
			{
				arrGodPt.push_back(arrPtOptical[i]);
				arrGodPtTrack.push_back(arrPtTrackOptical[i]);
			}
		}

		nInlierPt = arrGodPt.size();

		// **Debug : Inlier 출력
		if (bDebug == true)
		{
			BYTE** imgDebug;
			CString strNum;

			strNum.Format(_T("%d"), nInlierPt);

			imgDebug = cmatrix(m_nHL, m_nWL);

			MatToByteGray(M_ImgCurGray.data, imgDebug, m_nHL, m_nWL);

			for (i = 0; i < nInlierPt; i++)
				DrawCross(imgDebug, m_nWL, m_nHL, CPoint(arrGodPtTrack[i].x, arrGodPtTrack[i].y), 0);

			DrawTextOnImag2DGray(imgDebug, m_nWL, m_nHL, 0, 0, strNum, DT_LEFT, 100, true);

			DisplayCimage2D(imgDebug, m_nWL, m_nHL, m_nWL, nDebugOffset, false, true, 50);

			free_cmatrix(imgDebug, m_nHL, m_nWL);
		}

		// ============================================
		// Exception handling : less than inlier point
		// ============================================

		// Inlier 내부에서 구하기
		if (nInlierPt > minFeature)
		{
			homo = findHomography(arrGodPt, arrGodPtTrack, CV_RANSAC, ransac);
		}

		// 이전 특징점을 이용해 구하기
		if (nInlierPt <= minFeature)
		{
			homo = homo_pre;
		}

		// 임계값 보다 크다. => 거기에서 호모그래피를 구함.

		// **Debug : homography구할 때 쓰인 Point 출력
		if (bDebug == true)
		{
			BYTE** imgDebug;
			CString strNum;
			Point2f pt_homo;
			int nHomoPt = 0;

			imgDebug = cmatrix(m_nHL, m_nWL);

			MatToByteGray(M_ImgCurGray.data, imgDebug, m_nHL, m_nWL);

			for (i = 0; i < nInlierPt; i++)
			{
				GetPerspectivePoint(arrGodPt[i], pt_homo, homo);
				distance = GetDistance(arrGodPtTrack[i].x, arrGodPtTrack[i].y, pt_homo.x, pt_homo.y);

				if (distance < ransac)
				{
					DrawCross(imgDebug, m_nWL, m_nHL, CPoint(arrGodPtTrack[i].x, arrGodPtTrack[i].y), 0);
					nHomoPt++;
				}
				else
					DrawCross(imgDebug, m_nWL, m_nHL, CPoint(arrGodPtTrack[i].x, arrGodPtTrack[i].y), 255);
			}

			strNum.Format(_T("%d"), nHomoPt);

			DrawTextOnImag2DGray(imgDebug, m_nWL, m_nHL, 0, 0, strNum, DT_LEFT, 100, true);

			DisplayCimage2D(imgDebug, m_nWL, m_nHL, m_nWL / 2 * 3, nDebugOffset, true, true, 50);

			free_cmatrix(imgDebug, m_nHL, m_nWL);
		}
	}
}

void CMainProcessDlg::GethomographyNew(vector<Point2f>& arrPtPre, Mat& homo, Mat& homo_pre, int thresh_distance, int minFeature, double ransac, bool bFirst, bool bDebug, bool bEnlarge, int nEnlargeRate, LR bLR)
{
	int i, j;
	int x, y;

	int nH, nW;

	Mat M_ImgPreGray, M_ImgCurGray;

	// value for goodFeatureToTrack
	vector<Point2f> arrPt, arrPtTrack;
	vector<Point2f> arrPtOptical, arrPtTrackOptical;

	// value for optical Flow
	int nRawPoint, nOpticalPt, nInlierPt, nHomoPt;
	int win_size = 10;
	char* feature_found;
	float* feature_error;

	// value for filtering point
	vector<Point2f>  arrGodPt, arrGodPtTrack;

	int nH_Warp = 600;
	int nW_Warp = 1200;

	// value for debug
	int nDebugOffset;

	//-------------------------------------------------------------

	if (bLR == LEFT)
	{
		M_ImgPreGray = m_MImgLPreGray.clone();
		M_ImgCurGray = m_MImgLCurGray.clone();
		nH = m_nHL;
		nW = m_nWL;
	}

	if (bLR == RIGHT)
	{
		M_ImgPreGray = m_MImgRPreGray.clone();
		M_ImgCurGray = m_MImgRCurGray.clone();
		nH = m_nHR;
		nW = m_nWR;
	}

	// =====================================
	// Get Feature Point
	// =====================================

	int gridSize = 30;
	int threshold = 5;
	double distance;

	for (y = 0; y < nH; y += gridSize)
		for (x = 0; x < nW; x += gridSize)
		{
			if (abs(M_ImgPreGray.at<uchar>(y, x) - M_ImgCurGray.at<uchar>(y, x)) > threshold)
			{
				arrPt.push_back(Point2f(x, y));
			}
		}

	nRawPoint = arrPt.size();

	// sparsely Feature

	// **Debug : 첫 특징점 출력
	if (bDebug == true)
	{
		// 왼쪽 이미지 오프셋
		if (bLR == LEFT)
			nDebugOffset = (nH_Warp / 2);
		// 오른쪽 이미지 오프셋
		if (bLR == RIGHT)
			nDebugOffset = (nH_Warp / 2) + (nH / 2);

		BYTE** imgDebug;
		CString strNum;
		strNum.Format(_T("%d"), nRawPoint);

		imgDebug = cmatrix(nH, nW);

		MatToByteGray(M_ImgPreGray.data, imgDebug, m_nHL, m_nWL);

		for (i = 0; i < nRawPoint; i++)
			DrawCross(imgDebug, nW, nH, CPoint(arrPt[i].x, arrPt[i].y), 0);

		DrawTextOnImag2DGray(imgDebug, nW, nH, 0, 0, strNum, DT_LEFT, 100, true);

		DisplayCimage2D(imgDebug, nW, nH, 0, nDebugOffset, false, true, 50);

		free_cmatrix(imgDebug, nH, nW);
	}

	// =====================================
	// Optical Flow !!!!!
	// =====================================

	int64 TimeStart_Optical, TimeEnd_Optical;

	vector<uchar> status(nRawPoint);
	vector<float> err(nRawPoint);
	int threshErr = 15;

	if (nRawPoint > 0)
	{
		calcOpticalFlowPyrLK(M_ImgPreGray, M_ImgCurGray, arrPt, arrPtTrack, status, err, Size(10, 10), 3);
	}


	if (nRawPoint > 0)
	{
		for (i = 0; i < nRawPoint; i++)
		{
			if (status[i] == 1 && err[i] < threshErr)
			{
				arrPtOptical.push_back(arrPt[i]);
				arrPtTrackOptical.push_back(arrPtTrack[i]);
			}
		}
	}

	nOpticalPt = arrPtOptical.size();

	// **Debug : 옵티컬 플로우 출력
	if (bDebug == true)
	{
		BYTE** imgDebugR, ** imgDebugG, ** imgDebugB;
		CString strNum;

		strNum.Format(_T("%d / %d"), nOpticalPt, nRawPoint);

		imgDebugR = cmatrix(nH / 2, nW / 2);
		imgDebugG = cmatrix(nH / 2, nW / 2);
		imgDebugB = cmatrix(nH / 2, nW / 2);

		for (i = 0; i < nH / 2; i++)
			for (j = 0; j < nW / 2; j++)
			{
				imgDebugR[i][j] = abs(M_ImgPreGray.at<uchar>(i * 2, j * 2) - M_ImgCurGray.at<uchar>(i * 2, j * 2));
				imgDebugG[i][j] = abs(M_ImgPreGray.at<uchar>(i * 2, j * 2) - M_ImgCurGray.at<uchar>(i * 2, j * 2));
				imgDebugB[i][j] = abs(M_ImgPreGray.at<uchar>(i * 2, j * 2) - M_ImgCurGray.at<uchar>(i * 2, j * 2));
			}

		if (bEnlarge == true)
		{
			int R, G, B;
			Point2f dirVec, enlargePt;

			for (i = 0; i < nRawPoint; i++)
			{
				if (status[i] == 1 && err[i] < threshErr)
				{
					R = 255; G = 0; B = 0;
				}
				else
				{
					R = 255; G = 0; B = 255;
				}

				dirVec.y = arrPtTrack[i].y - arrPt[i].y;
				dirVec.x = arrPtTrack[i].x - arrPt[i].x;

				enlargePt.x = (nEnlargeRate * dirVec.x) + arrPt[i].x;
				enlargePt.y = (nEnlargeRate * dirVec.y) + arrPt[i].y;

				if (enlargePt.x >= 0 && enlargePt.x < nW && enlargePt.y >= 0 && enlargePt.y < nH)
				{
					DrawLine(imgDebugR, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, R);
					DrawLine(imgDebugG, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, G);
					DrawLine(imgDebugB, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, B);
				}
			}
		}
		else
		{
			int R, G, B;

			for (i = 0; i < nRawPoint; i++)
			{
				if (status[i] == 1 && err[i] < threshErr)
				{
					R = 255; G = 0; B = 0;
				}
				else
				{
					R = 255; G = 0; B = 255;
				}

				DrawLine(imgDebugR, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, R);
				DrawLine(imgDebugG, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, G);
				DrawLine(imgDebugB, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, B);
			}
		}

		DrawTextOnImag2DGray(imgDebugR, nW / 2, nH / 2, 0, 0, strNum, DT_LEFT, 255, true);
		DrawTextOnImag2DGray(imgDebugG, nW / 2, nH / 2, 0, 0, strNum, DT_LEFT, 255, true);
		DrawTextOnImag2DGray(imgDebugB, nW / 2, nH / 2, 0, 0, strNum, DT_LEFT, 255, true);

		DisplayCimage2DColor(imgDebugR, imgDebugG, imgDebugB, nW / 2, nH / 2, nW / 2, nDebugOffset, false, true, 100);

		free_cmatrix(imgDebugR, nH / 2, nW / 2);
		free_cmatrix(imgDebugG, nH / 2, nW / 2);
		free_cmatrix(imgDebugB, nH / 2, nW / 2);
	}
}

void CMainProcessDlg::GethomographyHarris(vector<Point2f>& arrPtPre, Mat& homo, Mat& homo_pre, int thresh_distance, int minFeature, double ransac, bool bFirst, bool bDebug, bool bEnlarge, int nEnlargeRate, LR bLR)
{
		int i, j;
		int x, y;

		int nH, nW;

		Mat M_ImgPreGray, M_ImgCurGray;

		// value for goodFeatureToTrack
		vector<Point2f> arrPt, arrPtTrack;
		vector<Point2f> arrPtOptical, arrPtTrackOptical;

		// value for optical Flow
		int nRawPoint, nOpticalPt, nInlierPt, nHomoPt;
		int win_size = 10;
		char* feature_found;
		float* feature_error;

		// value for filtering point
		vector<Point2f>  arrGodPt, arrGodPtTrack;

		int nH_Warp = 600;
		int nW_Warp = 1200;

		// value for debug
		int nDebugOffset;

		//-------------------------------------------------------------

		if (bLR == LEFT)
		{
			M_ImgPreGray = m_MImgLPreGray.clone();
			M_ImgCurGray = m_MImgLCurGray.clone();
			nH = m_nHL;
			nW = m_nWL;
		}

		if (bLR == RIGHT)
		{
			M_ImgPreGray = m_MImgRPreGray.clone();
			M_ImgCurGray = m_MImgRCurGray.clone();
			nH = m_nHR;
			nW = m_nWR;
		}

		// =====================================
		// Get Feature Point
		// =====================================

		int gridSize = 30;
		int threshold = 5;
		double distance;

		for (y = 0; y < nH; y += gridSize)
			for (x = 0; x < nW; x += gridSize)
			{
				if (abs(M_ImgPreGray.at<uchar>(y, x) - M_ImgCurGray.at<uchar>(y, x)) > threshold)
				{
					arrPt.push_back(Point2f(x, y));
				}
			}

		nRawPoint = arrPt.size();

		// sparsely Feature

		// **Debug : 첫 특징점 출력
		if (bDebug == true)
		{
			// 왼쪽 이미지 오프셋
			if (bLR == LEFT)
				nDebugOffset = (nH_Warp / 2);
			// 오른쪽 이미지 오프셋
			if (bLR == RIGHT)
				nDebugOffset = (nH_Warp / 2) + (nH / 2);

			BYTE** imgDebug;
			CString strNum;
			strNum.Format(_T("%d"), nRawPoint);

			imgDebug = cmatrix(nH, nW);

			MatToByteGray(M_ImgPreGray.data, imgDebug, m_nHL, m_nWL);

			for (i = 0; i < nRawPoint; i++)
				DrawCross(imgDebug, nW, nH, CPoint(arrPt[i].x, arrPt[i].y), 0);

			DrawTextOnImag2DGray(imgDebug, nW, nH, 0, 0, strNum, DT_LEFT, 100, true);

			DisplayCimage2D(imgDebug, nW, nH, 0, nDebugOffset, false, true, 50);

			free_cmatrix(imgDebug, nH, nW);
		}

		// =====================================
		// Optical Flow !!!!!
		// =====================================

		int64 TimeStart_Optical, TimeEnd_Optical;

		vector<uchar> status(nRawPoint);
		vector<float> err(nRawPoint);
		int threshErr = 15;

		if (nRawPoint > 0)
		{
			calcOpticalFlowPyrLK(M_ImgPreGray, M_ImgCurGray, arrPt, arrPtTrack, status, err, Size(10, 10), 3);
		}


		if (nRawPoint > 0)
		{
			for (i = 0; i < nRawPoint; i++)
			{
				if (status[i] == 1 && err[i] < threshErr)
				{
					arrPtOptical.push_back(arrPt[i]);
					arrPtTrackOptical.push_back(arrPtTrack[i]);
				}
			}
		}

		nOpticalPt = arrPtOptical.size();

		// **Debug : 옵티컬 플로우 출력
		if (bDebug == true)
		{
			BYTE** imgDebugR, ** imgDebugG, ** imgDebugB;
			CString strNum;

			strNum.Format(_T("%d / %d"), nOpticalPt, nRawPoint);

			imgDebugR = cmatrix(nH / 2, nW / 2);
			imgDebugG = cmatrix(nH / 2, nW / 2);
			imgDebugB = cmatrix(nH / 2, nW / 2);

			for (i = 0; i < nH / 2; i++)
				for (j = 0; j < nW / 2; j++)
				{
					imgDebugR[i][j] = abs(M_ImgPreGray.at<uchar>(i * 2, j * 2) - M_ImgCurGray.at<uchar>(i * 2, j * 2));
					imgDebugG[i][j] = abs(M_ImgPreGray.at<uchar>(i * 2, j * 2) - M_ImgCurGray.at<uchar>(i * 2, j * 2));
					imgDebugB[i][j] = abs(M_ImgPreGray.at<uchar>(i * 2, j * 2) - M_ImgCurGray.at<uchar>(i * 2, j * 2));
				}

			if (bEnlarge == true)
			{
				int R, G, B;
				Point2f dirVec, enlargePt;

				for (i = 0; i < nRawPoint; i++)
				{
					if (status[i] == 1 && err[i] < threshErr)
					{
						R = 255; G = 0; B = 0;
					}
					else
					{
						R = 255; G = 0; B = 255;
					}

					dirVec.y = arrPtTrack[i].y - arrPt[i].y;
					dirVec.x = arrPtTrack[i].x - arrPt[i].x;

					enlargePt.x = (nEnlargeRate * dirVec.x) + arrPt[i].x;
					enlargePt.y = (nEnlargeRate * dirVec.y) + arrPt[i].y;

					if (enlargePt.x >= 0 && enlargePt.x < nW && enlargePt.y >= 0 && enlargePt.y < nH)
					{
						DrawLine(imgDebugR, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, R);
						DrawLine(imgDebugG, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, G);
						DrawLine(imgDebugB, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, enlargePt.x / 2, enlargePt.y / 2, B);
					}
				}
			}
			else
			{
				int R, G, B;

				for (i = 0; i < nRawPoint; i++)
				{
					if (status[i] == 1 && err[i] < threshErr)
					{
						R = 255; G = 0; B = 0;
					}
					else
					{
						R = 255; G = 0; B = 255;
					}

					DrawLine(imgDebugR, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, R);
					DrawLine(imgDebugG, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, G);
					DrawLine(imgDebugB, nW / 2, nH / 2, arrPt[i].x / 2, arrPt[i].y / 2, arrPtTrack[i].x / 2, arrPtTrack[i].y / 2, B);
				}
			}

			DrawTextOnImag2DGray(imgDebugR, nW / 2, nH / 2, 0, 0, strNum, DT_LEFT, 255, true);
			DrawTextOnImag2DGray(imgDebugG, nW / 2, nH / 2, 0, 0, strNum, DT_LEFT, 255, true);
			DrawTextOnImag2DGray(imgDebugB, nW / 2, nH / 2, 0, 0, strNum, DT_LEFT, 255, true);

			DisplayCimage2DColor(imgDebugR, imgDebugG, imgDebugB, nW / 2, nH / 2, nW / 2, nDebugOffset, false, true, 100);

			free_cmatrix(imgDebugR, nH / 2, nW / 2);
			free_cmatrix(imgDebugG, nH / 2, nW / 2);
			free_cmatrix(imgDebugB, nH / 2, nW / 2);
	}

	// =====================================
	// Get Homography !!!!!
	// =====================================
	
	Point2f pt_homoPre;

	arrGodPt.clear();
	arrGodPtTrack.clear();
	arrPtPre.clear();

	// 프래임 특징점 저장
	for (i = 0; i < nOpticalPt; i++)
	{
		arrPtPre.push_back(arrPtTrackOptical[i]);
	}

	if (bFirst == true)
	{
		homo = findHomography(arrPtOptical, arrPtTrackOptical, CV_RANSAC, ransac);
	}

	if (bFirst == false)
	{
		// ==============================================
		// Filtering Inlier using previous homography
		// ==============================================

		// 이전 호모 적용 갯수 체크 
		for (i = 0; i < nOpticalPt; i++)
		{
			GetPerspectivePoint(arrPtOptical[i], pt_homoPre, homo_pre);
			distance = GetDistance(arrPtTrackOptical[i].x, arrPtTrackOptical[i].y, pt_homoPre.x, pt_homoPre.y);

			if (distance < thresh_distance)
			{
				arrGodPt.push_back(arrPtOptical[i]);
				arrGodPtTrack.push_back(arrPtTrackOptical[i]);
			}
		}

		nInlierPt = arrGodPt.size();

		// **Debug : Inlier 출력
		if (bDebug == true)
		{
			BYTE** imgDebug;
			CString strNum;

			strNum.Format(_T("%d"), nInlierPt);

			imgDebug = cmatrix(nH, nW);

			MatToByteGray(M_ImgPreGray.data, imgDebug, m_nHL, m_nWL);

			for (i = 0; i < nInlierPt; i++)
				DrawCross(imgDebug, nW, nH, CPoint(arrGodPtTrack[i].x, arrGodPtTrack[i].y), 0);

			DrawTextOnImag2DGray(imgDebug, nW, nH, 0, 0, strNum, DT_LEFT, 100, true);

			DisplayCimage2D(imgDebug, nW, nH, nW, nDebugOffset, false, true, 50);

			free_cmatrix(imgDebug, nH, nW);
		}

		// ============================================
		// Exception handling : less than inlier point
		// ============================================

		// Inlier 내부에서 구하기
		if (nInlierPt > minFeature)
		{
			homo = findHomography(arrGodPt, arrGodPtTrack, CV_RANSAC, ransac);
		}

		// 이전 특징점을 이용해 구하기
		if (nInlierPt <= minFeature)
		{
			homo = homo_pre;
		}

		// 임계값 보다 크다. => 거기에서 호모그래피를 구함.

		// **Debug : homography구할 때 쓰인 Point 출력
		if (bDebug == true)
		{
			BYTE** imgDebug;
			CString strNum;
			Point2f pt_homo;
			int nHomoPt = 0;

			imgDebug = cmatrix(nH, nW);

			MatToByteGray(M_ImgPreGray.data, imgDebug, m_nHL, m_nWL);

			for (i = 0; i < nInlierPt; i++)
			{
				GetPerspectivePoint(arrGodPt[i], pt_homo, homo);
				distance = GetDistance(arrGodPtTrack[i].x, arrGodPtTrack[i].y, pt_homo.x, pt_homo.y);

				if (distance < ransac)
				{
					DrawCross(imgDebug, nW, nH, CPoint(arrGodPtTrack[i].x, arrGodPtTrack[i].y), 0);
					nHomoPt++;
				}
				else
					DrawCross(imgDebug, nW, nH, CPoint(arrGodPtTrack[i].x, arrGodPtTrack[i].y), 255);
			}

			strNum.Format(_T("%d"), nHomoPt);

			DrawTextOnImag2DGray(imgDebug, nW, nH, 0, 0, strNum, DT_LEFT, 100, true);

			DisplayCimage2D(imgDebug, nW, nH, nW / 2 * 3, nDebugOffset, true, true, 50);

			free_cmatrix(imgDebug, nH, nW);
		}
	}
}

void CMainProcessDlg::CorrectHomography(vector<Point2f>& arrPtR, bool bDebug)
{
	int i, j, y, x;
	
	int nNumPtR;

	// m_arrPtPreL, m_Homo를 이용해 arrPtPreL_Trans 제작.
	vector<Point2f> arrPtR_trans;
	Point2f transPoint;

	Mat transRHomo = m_Homo * m_CPR_cur.inv();

	nNumPtR = arrPtR.size();

	arrPtR_trans.clear();

	for (i = 0; i < nNumPtR; i++)
	{
		GetPerspectivePoint(arrPtR[i], transPoint, transRHomo);

		if (transPoint.y >= 0 && transPoint.y < m_nH_Total && transPoint.x >= 0 && transPoint.x < m_nW_Total)
		{
			arrPtR_trans.push_back(transPoint);
		}
	}

	// =====================================
	// Correct Matching Point
	// =====================================

	double qualityLevel = 0.05;
	double minDistance = 1;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	double maxCorners = 5;

	int ptX, ptY;
	int matchX, matchY;
	int winSize = 21;
	int winSizeHalf = int(winSize / 2);
	double dDistance;
	double dMinDist;
	vector<Point2f> arrPtL_correct;
	int nNumPtL_correct = 0;
	int nNumPtRtrans;
	
	Mat M_ROIL(cvSize(winSize, winSize), CV_8UC1, cvScalar(0));

	arrPtL_correct.clear();
	
	arrMatchL.clear();
	arrMatchR.clear();

	nNumPtRtrans = arrPtR_trans.size();

	for (i = 0; i < nNumPtRtrans; i++)
	{
		dMinDist = 9999;
		matchX = -1;
		matchY = -1;

		// arrPtPreL_Trans의 위치를 이용해 마스크 제작
		ptX = int(arrPtR_trans[i].x);
		ptY = int(arrPtR_trans[i].y);

		if (m_MImgRTransGray.at<uchar>(ptY, ptX) > 0 && m_MImgLTransGray.at<uchar>(ptY, ptX)> 0
			&& ptY > winSize && ptY < m_nH_Total - winSize
			&& ptX > winSize && ptX < m_nW_Total - winSize)
		{
			M_ROIL = m_MImgLTransGray(Rect(ptX - winSizeHalf, ptY - winSizeHalf, winSize, winSize));

			//// 마스크 생성 후 GoodFeatureToTrack 적용
			arrPtL_correct.clear();

			goodFeaturesToTrack(M_ROIL, arrPtL_correct, maxCorners, qualityLevel, minDistance);
			nNumPtL_correct = arrPtL_correct.size();

			if (nNumPtL_correct > 0)
			{
				// Nearest Neighbor 이용해 가까이 있는 arrPtR 선택
				for (j = 0; j < nNumPtL_correct; j++)
				{
					dDistance = GetDistance(arrPtL_correct[j].x, arrPtL_correct[j].y, winSizeHalf, winSizeHalf);

					if (dMinDist > dDistance && dDistance > 0)
					{
						dMinDist = dDistance;
						matchX = arrPtL_correct[j].x + ptX + (-1 * winSizeHalf);
						matchY = arrPtL_correct[j].y + ptY + (-1 * winSizeHalf);
					}
				}

				// arrPtR 있을 시에 arrMatchR, arrMatchL 제작
				if (matchX > 0 || matchY > 0)
				{
					arrMatchR.push_back(Point2f(ptX, ptY));
					arrMatchL.push_back(Point2f(matchX, matchY));
				}

				/*if (false)
				{
					BYTE** ROIR, ** ROIG, ** ROIB;
				
					ROIR = cmatrix(winSize, winSize);
					ROIG = cmatrix(winSize, winSize);
					ROIB = cmatrix(winSize, winSize);

					MatToByteGray(M_ROIL.data, ROIR, winSize, winSize);
					MatToByteGray(M_ROIL.data, ROIG, winSize, winSize);
					MatToByteGray(M_ROIL.data, ROIB, winSize, winSize);

					for (j = 0; j < arrPtL_correct.size(); j++)
					{
						ROIR[int(arrPtL_correct[j].y)][int(arrPtL_correct[j].x)] = 255;
						ROIG[int(arrPtL_correct[j].y)][int(arrPtL_correct[j].x)] = 0;
						ROIB[int(arrPtL_correct[j].y)][int(arrPtL_correct[j].x)] = 0;
					}

					ROIG[matchY - ptY + (winSize / 2)][matchX - ptX + (winSize / 2)] = 255;

					DisplayCimage2DColor(ROIR, ROIG, ROIB, winSize, winSize, winSize * i, m_nH_Total + m_nHL, false, true);

					for (y = -winSizeHalf; y <= winSizeHalf; y++)
						for (x = -winSizeHalf; x <= winSizeHalf; x++)
						{
							ROIR[winSizeHalf + y][winSizeHalf + x] = m_ImgRTransGray[ptY + y][ptX + x];
							ROIG[winSizeHalf + y][winSizeHalf + x] = m_ImgRTransGray[ptY + y][ptX + x];
							ROIB[winSizeHalf + y][winSizeHalf + x] = m_ImgRTransGray[ptY + y][ptX + x];
						}

					ROIR[winSizeHalf][winSizeHalf] = 255;
					ROIG[winSizeHalf][winSizeHalf] = 0;
					ROIB[winSizeHalf][winSizeHalf] = 0;

					DisplayCimage2DColor(ROIR, ROIG, ROIB, winSize, winSize, winSize * i, m_nH_Total + m_nHL + winSize, false, true);

					free_cmatrix(ROIR, winSize, winSize);
					free_cmatrix(ROIG, winSize, winSize);
					free_cmatrix(ROIB, winSize, winSize);
				}*/
			}
		}
	}

	// =====================================
	// Determine if refine Homography
	// =====================================

	


	// =====================================
	// Correct Homography
	// =====================================

	int nNumMatch;

	nNumMatch = arrMatchL.size();

	// arrMatchR, arrMatchL를 이용해 Homography 생성
	if (nNumMatch > 20)
	{
		Mat correctH;

		correctH = findHomography(arrMatchR, arrMatchL, CV_RANSAC);
		m_Homo = correctH * m_Homo;

		// < 디버그모드 > H를 적용했을 때 위치 와 매칭한 위치 알아보기
		// 외쪽 영상, 오른쪽 영상
		// arrPtPreL_Trans : 회색
		// arrMatchL, arrMatchR : 같은 색깔로 표시

		if (bDebug == true)
		{
			BYTE** imgMatchR, ** imgMatchG, ** imgMatchB;
			int nRanColor[3];

			imgMatchR = cmatrix(m_nH_Total, 2 * m_nW_Total);
			imgMatchG = cmatrix(m_nH_Total, 2 * m_nW_Total);
			imgMatchB = cmatrix(m_nH_Total, 2 * m_nW_Total);

			for (i = 0; i < m_nH_Total; i++)
				for (j = 0; j < m_nW_Total; j++)
				{
					imgMatchR[i][j] = m_MImgLTransGray.at<uchar>(i, j);
					imgMatchG[i][j] = m_MImgLTransGray.at<uchar>(i, j);
					imgMatchB[i][j] = m_MImgLTransGray.at<uchar>(i, j);
				}

			for (i = 0; i < m_nH_Total; i++)
				for (j = 0; j < m_nW_Total; j++)
				{
					imgMatchR[i][j + m_nW_Total] = m_MImgRTransGray.at<uchar>(i, j);
					imgMatchG[i][j + m_nW_Total] = m_MImgRTransGray.at<uchar>(i, j);
					imgMatchB[i][j + m_nW_Total] = m_MImgRTransGray.at<uchar>(i, j);
				}

			srand(time(NULL));

			for (i = 0; i < nNumMatch; i++)
			{
				for (j = 0; j < 3; j++)
				{
					nRanColor[j] = (rand() % 255);
				}

				DrawCross(imgMatchR, 2 * m_nW_Total, m_nH_Total, CPoint(arrMatchL[i].x, arrMatchL[i].y), nRanColor[0]);
				DrawCross(imgMatchG, 2 * m_nW_Total, m_nH_Total, CPoint(arrMatchL[i].x, arrMatchL[i].y), nRanColor[1]);
				DrawCross(imgMatchB, 2 * m_nW_Total, m_nH_Total, CPoint(arrMatchL[i].x, arrMatchL[i].y), nRanColor[2]);

				DrawCross(imgMatchR, 2 * m_nW_Total, m_nH_Total, CPoint(arrMatchR[i].x + m_nW_Total, arrMatchR[i].y), nRanColor[0]);
				DrawCross(imgMatchG, 2 * m_nW_Total, m_nH_Total, CPoint(arrMatchR[i].x + m_nW_Total, arrMatchR[i].y), nRanColor[1]);
				DrawCross(imgMatchB, 2 * m_nW_Total, m_nH_Total, CPoint(arrMatchR[i].x + m_nW_Total, arrMatchR[i].y), nRanColor[2]);
			}

			for (i = 0; i < nNumPtRtrans; i++)
			{
				ptX = int(arrPtR_trans[i].x);
				ptY = int(arrPtR_trans[i].y);

				if (m_MImgLTransGray.at<uchar>(ptY, ptX) != 0)
				{
					DrawCross(imgMatchR, 2 * m_nW_Total, m_nH_Total, CPoint(int(arrPtR_trans[i].x), int(arrPtR_trans[i].y)), 255);
					DrawCross(imgMatchG, 2 * m_nW_Total, m_nH_Total, CPoint(int(arrPtR_trans[i].x), int(arrPtR_trans[i].y)), 255);
					DrawCross(imgMatchB, 2 * m_nW_Total, m_nH_Total, CPoint(int(arrPtR_trans[i].x), int(arrPtR_trans[i].y)), 255);
				}
			}

			DisplayCimage2DColor(imgMatchR, imgMatchG, imgMatchB, m_nW_Total * 2, m_nH_Total, 0, m_nH_Total / 2 + m_nHL, false, true, 50);

			free_cmatrix(imgMatchR, m_nH_Total, m_nW_Total * 2);
			free_cmatrix(imgMatchG, m_nH_Total, m_nW_Total * 2);
			free_cmatrix(imgMatchB, m_nH_Total, m_nW_Total * 2);
		}
	}
	
}

void CMainProcessDlg::MakeMaskForBlend()
{
	int y, x;

	// transPoint 구하기
	int nH_min = 9999, nH_max = 0;
	int nW_min = 9999, nW_max = 0;

	for (y = 0; y < m_nH_Total; y++)
		for (x = 0; x < m_nW_Total; x++)
		{
			if (m_MImgLTransMask.at<uchar>(y, x) == 255 && m_MImgRTransMask.at<uchar>(y, x) == 255)
			{
				if (y < nH_min) nH_min = y;
				if (y > nH_max) nH_max = y;

				if (x < nW_min) nW_min = x;
				if (x > nW_max) nW_max = x;
			}
		}

	m_offsetX_Blend = nW_min; 
	m_offsetY_Blend = nH_min;

	m_nH_Blend = nH_max - nH_min + 1;
	m_nW_Blend = nW_max - nW_min + 1;

	// blending을 위한 mask생성
	m_MImgLBlendMask = Mat(Size(m_nW_Blend, m_nH_Blend), CV_8U, Scalar(0));
	m_MImgRBlendMask = Mat(Size(m_nW_Blend, m_nH_Blend), CV_8U, Scalar(0));

	m_MImgLTransMaskSmall(Rect(m_offsetX_Blend, m_offsetY_Blend, m_nW_Blend, m_nH_Blend)).copyTo(m_MImgLBlendMask(Rect(0, 0, m_nW_Blend, m_nH_Blend)));
	m_MImgRTransMaskSmall(Rect(m_offsetX_Blend, m_offsetY_Blend, m_nW_Blend, m_nH_Blend)).copyTo(m_MImgRBlendMask(Rect(0, 0, m_nW_Blend, m_nH_Blend)));

	for (y = 0; y < m_nH_Blend; y++)
		for (x = 0; x < m_nW_Blend; x++)
		{
			if (m_MImgEdgeLTransMask.at<uchar>(y + nH_min, x + nW_min) == 255
				&& m_MImgRTransMaskSmall.at<uchar>(y + nH_min, x + nW_min) == 255)
			{
				m_MImgLBlendMask.at<uchar>(y, x) = 255;
			}

			if (m_MImgLTransMaskSmall.at<uchar>(y + nH_min, x + nW_min) == 255
				&& m_MImgRTransMaskSmall.at<uchar>(y + nH_min, x + nW_min) == 255)
			{
				m_MImgRBlendMask.at<uchar>(y, x) = 0;
			}
		}

	m_MImgLBlendColor = m_MImgLTransColor(Rect(m_offsetX_Blend, m_offsetY_Blend, m_nW_Blend, m_nH_Blend));
	m_MImgRBlendColor = m_MImgRTransColor(Rect(m_offsetX_Blend, m_offsetY_Blend, m_nW_Blend, m_nH_Blend));

	/*imshow("b", m_MImgLBlendMask);
	imshow("c", m_MImgRBlendMask);
	imshow("d", m_MImgLBlendColor);
	imshow("e", m_MImgRBlendColor);*/
}

void CMainProcessDlg::ScaleHomography(Mat& Homo, Mat& scaledHomo, double dScale)
{
	Mat tmp(Size(3, 3), CV_64F);

	tmp.at<double>(0, 0) = dScale;
	tmp.at<double>(0, 1) = 0;
	tmp.at<double>(0, 2) = 0;

	tmp.at<double>(1, 0) = 0;
	tmp.at<double>(1, 1) = dScale;
	tmp.at<double>(1, 2) = 0;

	tmp.at<double>(2, 0) = 0;
	tmp.at<double>(2, 1) = 0;
	tmp.at<double>(2, 2) = 1;

	scaledHomo = tmp * Homo;
}