// Image.h : Subroutines Prototype, Structures and Constants
//
/////////////////////////////////////////////////////////////////////////////

#ifndef IMAGE_H_SUBROUTINE_STRUCTURE_CONSTANT
#define IMAGE_H_SUBROUTINE_STRUCTURE_CONSTANT

#include "opencv2\core\core.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\features2d\features2d.hpp"
#include "opencv2\nonfree\nonfree.hpp"
#include "opencv2\nonfree\features2d.hpp"
#include "opencv2\calib3d\calib3d.hpp"
#include<vector>

using namespace cv;

#define GRAY_CNT	256

// 영상 정보 저장 구조체
struct ImageInfo {
	int nW, nH;
	int nPosX, nPosY;

	BYTE *Image1D;
};

// 2차원 메모리 할당 및 해제
BYTE **cmatrix(int nH, int nW);
void free_cmatrix(BYTE **Image, int nH, int nW);

int **imatrix(int nH, int nW);
void free_imatrix(int **Image, int nH, int nW);

double **dmatrix(int nH, int nW);
void free_dmatrix(double **Image, int nH, int nW);

// 24비트 비트맵 영상의 크기 및 데이터 위치를 계산하는 함수
int GetBmp24Size(int nW, int nH);
int GetBmp24Pos(int nW, int nH, int x, int y);

// 비트맵 파일 읽기, raw 파일 읽기, 비트맵 파일 저장(24비트 컬러)
BYTE *ReadBmp(LPCTSTR FileName, int *pW, int *pH);
BYTE *ReadRaw(LPCTSTR FileName, int nW, int nH);
bool SaveBmp(LPCTSTR FileName, BYTE *Image1D, int nW, int nH);

BYTE *ReadPcx(LPCTSTR FileName, int *pW, int *pH);

BYTE *ReadJpeg(LPCTSTR FileName, int *pW, int *pH);
bool SaveJpeg(LPCTSTR FileName, BYTE *Image1D, int nW, int nH, bool bColor, int nQuality);

// 마지막 영상의 폭, 높이, 위치 읽기
bool GetCurrentImageInfo(int *pW, int *pH, int *pPosX, int *pPosY, int nIndex = 0);
// 마지막 영상을 2차원 회색조 정보로 읽기
bool GetCurrentImageGray(BYTE **ImageGray, int nIndex = 0);
// 2차원 회색조 영상을 출력
bool DisplayCimage2D(BYTE **ImageGray, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false, int Rate = 100);
// 마지막 영상을 2차원 컬러 정보로 읽기
bool GetCurrentImageColor(BYTE **ImageRed, BYTE **ImageGreen, BYTE **ImageBlue, int nIndex = 0);
bool GetCurrentImage1DColor(BYTE *Image1D, int nIndex = 0);
bool DisplayCimage1D(BYTE *Image1D, int nW, int nH, int nPosX, int nPosY, bool bErase, bool bDelete);
// 2차원 컬러 영상을 출력
bool DisplayCimage2DColor(BYTE **ImageRed, BYTE **ImageGreen, BYTE **ImageBlue, int nW, int nH, 
	int nPosX, int nPosY, bool bErase = true, bool bDelete = false, int Rate = 100);

bool DisplayIimage2D(int **ImageInt, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false);
bool DisplayDimage2D(double **ImageDouble, int nW, int nH, int nPosX, int nPosY, bool bErase = true, bool bDelete = false);

void SetViewMsg(CString ViewMsg, bool bErase = true);

void Move(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int x0, int y0);
void Rotate(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nOutW, int nOutH, double dDeg);
void ScaleX2(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void HistogramEqualization(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void MeanFilter(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nWinSize);
void MedianFilter(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nWinSize);
void Sobel(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void SobelXY(BYTE **ImageGray, double **OutputGrayX, double **OutputGrayY, int nW, int nH);
void Laplacian(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void LaplacianSharp(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void Thinning(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
int Labeling(BYTE **ImageGray, int **Label, int nW, int nH, int nConnNumThre);
int Labeling(BYTE **ImageGray, int **Label, int nW, int nH, int nConnNumThre, int *Area);
int Labeling(BYTE **ImageGray, int **Label, int nW, int nH, int nConnNumThre, int nConnNumThre2);
double DFT2D(BYTE **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH);
double RowColDFT2D(BYTE **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH);
void FFT2Radix(double *Xr, double *Xi, double *Yr, double *Yi, int nN, bool bInverse);
double FFT2D(BYTE **ImageGray, double **OutputReal, double **OutputImag, int nW, int nH);
void FFT2DInverse(double **InputReal, double **InputImag, double **OutputDouble, int nW, int nH);
void Filtering(BYTE **ImageGray, double **OutputDouble, int nW, int nH, int nPosX, int nPosY, 
	int nFilterType1, int nFilterType2, int nDC, int nN);
void DCT2D(BYTE **ImageGray, double **OutputDouble, int nW, int nH, int nBlockSize);
void Compression(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void pTileThresholding(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, double dRate, int nType);
void ValleyThresholding(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void IterateThresholding(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH);
void HoughTransform(BYTE **ImageGray, double **HoughSpace, int nW, int nH);
void Turtle(BYTE **ImageGray, int nW, int nH, CPoint *Boundary, int *pCnt);
void Turtle(int **Label, int nW, int nH, int SelLabel, CPoint *Boundary, int *pCnt);
void ComputekCosine(CPoint *Boundary, int nCnt, int k, double *kCosine);
void ComputekCosine2(CPoint *Boundary, int nCnt, int k, double *kCosine, bool *bConves, int xC, int yC);
void DrawLine(BYTE **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, BYTE Color);
void DrawLine1D(BYTE *Image1D, int nW, int nH, int x0, int y0, int x1, int y1, BYTE R, BYTE G, BYTE B);
void Dot(BYTE **ImageGray, int nW, int nH, int x, int y);
void Dot(BYTE **ImageGray, int nW, int nH, int x, int y, BYTE Color);
void DilationBinary(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nShape, int nSize);
void DilationSquareBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DilationHorizonBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DilationVertialBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ErosionBinary(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nShape, int nSize);
void ErosionSquareBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ErosionHorizonBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ErosionVertialBinaryFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DilationGray(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nShape, int nSize);
void ErosionGray(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nShape, int nSize);
bool InverseMatrix(double  **x, double **Inverse, int nN, double *Determinant);

void DrawLabelBoundBox(CRect *pBoundBox, int LabelCnt, BYTE **Image2DGray, BYTE Color);
void MakeImage2DGrayToImage1D(unsigned char **Image2DGray, int nW, int nH, unsigned char *Image1D);
void MakeImage1DToImage2DGray(unsigned char *Image1D, bool bBottomTop, int nW, int nH, unsigned char **Image2DGray, int nDesW = 0, int nDesH = 0);
void MakeImage1DToImage2DColor(unsigned char *Image1D, bool bBottomTop, int nW, int nH, unsigned char **Image2DRed, unsigned char **Image2DGreen, unsigned char **Image2DBlue, int nDesW = 0, int nDesH = 0);
void DrawTextOnImag2DGray(unsigned char **Image2DGray, int nW, int nH, int x, int y, CString Msg, UINT nFormat, unsigned char Color, bool bBig);
void DrawTextOnImag1D(unsigned char *Image1D, int nW, int nH, int x, int y, CString Msg, UINT nFormat, COLORREF Color, bool bBig);
void DrawCross(BYTE **Image, int nW, int nH, CPoint pt, int Gray);
void BigDot(BYTE **ImageGray, int nW, int nH, int x, int y, BYTE Color);
double Dist(int x1, int y1, int x2, int y2);
double FDist(double x1, double y1, double x2, double y2);
bool SaveImage2DColorJpeg(LPCTSTR FileName, BYTE **ImageRed, BYTE **ImageGreen, BYTE **ImageBlue, 
	int nW, int nH, bool bColor, int nQuality);
bool SaveImage2DColorBmp(LPCTSTR FileName, BYTE **ImageRed, BYTE **ImageGreen, BYTE **ImageBlue, 
	int nW, int nH);
void MedianBinaryFast2DGray(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void ComputekCosine(CPoint *Boundary, int nCnt, int *LeftK, int *RightK, double *kCosine, double sigma, bool *bConvex, int xC, int yC);
void GaussianSmooth(CPoint *Boundary, int nCnt, double sigma);
void GaussianSmooth(double *Data, int nCnt, double sigma);
void DrawArrow(BYTE **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, BYTE Color);
void DrawBigArrow(BYTE **ImageGray, int nW, int nH, int x1, int y1, int x2, int y2, BYTE Color);
void DrawBigLine(BYTE **ImageGray, int nW, int nH, int x0, int y0, int x1, int y1, BYTE Color);
void SetLabelBoundBox(int **Label, int nW, int nH, CRect *pBoundBox, int LabelCnt);
double ComputeSD(int *data, int cnt);
void DrawRect(CRect rt, BYTE **Image2DGray, int nW, int nH, BYTE Color);
void DrawRect1D(CRect rt, BYTE *Image1D, int nW, int nH, BYTE R, BYTE G, BYTE B);
void DrawBigRect(CRect rt, BYTE **Image2DGray, int nW, int nH, BYTE Color);
void KMeanBinary(BYTE **Image, BYTE ***OutputGray, int nW, int nH, int nK);
void Fill(BYTE **Image, int nW, int nH);
void RemoveSmallRegion(BYTE **Image, int nW, int nH, int nConnNumThre);
void DistanceTransform(BYTE **Image, double **Output, int nW, int nH);
double VectorAbs(double x, double y);
void CirclePoints(BYTE **ImageGray, int nW, int nH, int cx, int cy, int x, int y, BYTE Color);
void DrawCircle(BYTE **ImageGray, int nW, int nH, int x1, int y1, int radius, BYTE Color);
void DrawCircle2(BYTE **ImageGray, int nW, int nH, int x1, int y1, int radius, BYTE Color);
void DrawFillCircle(BYTE **ImageGray, int nW, int nH, int x1, int y1, int radius, BYTE Color);
void FastMedianFilter(BYTE **ImageGray, BYTE **OutputGray, int nW, int nH, int nOneSideSize);
void GetAffineParameters(CPoint D1, CPoint D2, CPoint D3, CPoint V1, CPoint V2, CPoint V3,
	double *pA1, double *pA2, double *pA0, double *pB1, double *pB2, double *pB0);
void Affine(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
	double a1, double a2, double a0, double b1, double b2, double b0);
void Bilinear(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut,
	double u0, double v0, double u1, double v1, double u2, double v2, double u3, double v3);
void Mean2DGrayFast(unsigned char **Image2DGray, int nW, int nH, int nSize, unsigned char **Image2DGrayOut);
void DcNotch2DGrayFast(BYTE **Image2DGray, BYTE **Image2DGrayOut, int nW, int nH, int nSize);
CPoint BMA(BYTE **CurrImageGray, BYTE **ReferImageGray, int left, int top, 
		 int nW, int nH, int nSearchLeft, int nSearchTop, int nSearchRight, int nSearchBottom, 
		 int BlockWidth, int BlockHeight);
void InversePerspectiveTable(int W, int H, int LW, int LH, POINT p0, POINT p1, POINT p2, POINT p3, POINT **Table);
void InversePerspective(int W, int H, int LW, int LH,unsigned char **img, unsigned char **out, POINT **Table);
void PerspectiveTable(int W, int H, int TW, int TH, POINT p0, POINT p1, POINT p2, POINT p3, POINT **Table);
void Perspective(int W, int H, int TW, int TH, unsigned char **img, unsigned char **out, POINT **Table);
void BilinearRadial(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut, int topLeft);
void InversePerspectivePt(int W, int H, int LW, int LH,
	POINT p0, POINT p1, POINT p2, POINT p3, double Ox, double Oy, double *Tx, double *Ty);
void DrawBoundary(BYTE **ImageGray, int nW, int nH, CPoint *Boundary, int Cnt, BYTE Color);
double MeanAngleLabel(int **Label, int nLabelNum, CRect boundbox);
void Harris2DGray(unsigned char **Image2DGray, int nW, int nH, int nSize, double **Image2DGrayOut, double dThre, double dK);
void ShoulderBinary2DGray(unsigned char **Image2DGray, int nW, int nH, unsigned char **Image2DGrayOut);
void VectorMedianFilter(int **OrgX, int **OrgY, int **OutX, int **OutY, int nW, int nH);
void YUV2RGB(BYTE *Y, BYTE *U, BYTE *V, BYTE **R, BYTE **G, BYTE **B, int nW, int nH, int nType);
void ImageOperation(int nOperationType);

//--------------------------------------------------------------------------------------------------------------
void MatToByteGray(BYTE* srcMatData, BYTE** desByte, int nH, int nW);
void ByteToMatGray(BYTE** srcByte, BYTE* desMatData, int nH, int nW);
void MatToByte2DColor(BYTE* srcMat, BYTE** desByteR, BYTE** desByteG, BYTE** desByteB, int nH, int nW, bool bBottomTop = true);
void Byte1DToMatColor(BYTE* srcByte, BYTE* desMatData, int nH, int nW, bool bBottomTop=true);


void GaussianFilter(double **Filter, double sigma, int size);

void BilinearInterpolation(double** image, double** output, int nW, int nH, double upRate);

void MirrorPadding(BYTE **Result, double **Filter, BYTE **Input, int FSize, int nW, int nH);
void MirrorPadding(double **Result, double **Filter, double **Input, int FSize, int nW, int nH);
void MirrorPadding(double **Result, double **Filter, BYTE **Input, int FSize, int nW, int nH);
void GetPerspectivePoint(Point2f& input, Point2f& output, Mat& homoMat);

//----------------------------------------------------------------------------------------------------------------
void GethomographySIFT(BYTE** dstImg, int nH_dst, int nW_dst, BYTE** srcImg, int nH_src, int nW_src, Mat& homo, vector<Point2f>& arrPt);
void GethomographySIFT(BYTE** dstImg, int nH_dst, int nW_dst, BYTE** srcImg, int nH_src, int nW_src, Mat& homo);
void GethomographyHarris(BYTE** dstImg, int nH_dst, int nW_dst, BYTE** srcImg, int nH_src, int nW_src, Mat& homo);

void GetFeaturePoint(BYTE** ImgR, int nHR, int nWR, BYTE** ImgL, int nHL, int nWL, vector<vector<Point2f>>& arrFeatureR, vector<vector<Point2f>>& arrFeatureL);
double Gaussian(double u1, double sigma1, double input);
int BilinearInterpolation(Point2f point, BYTE** image, int nH, int nW);

void Gethomography_LK(BYTE** dstImg, int nH_dst, int nW_dst, BYTE** srcImg, int nH_src, int nW_src, Mat& homo, vector<Point2f>& arrPt, float thresh, bool bDebug = false, bool bLR = false);
float GetDistance(float x1, float y1, float x2, float y2);

//----------------------------------------------------------------------------------------------------------------
int GenerateRandom(int dFirst, int dLast);
void findAffine(vector<Point2f>& point1, vector<Point2f>& point2, Mat& affine);
void GetAffinePoint(Point2f& input, Point2f& output, Mat& affineMat);
void GetAffineLSM(vector<Point2f>& inlier_src, vector<Point2f>& inlier_dst, Mat& affine);
void multiAffine(Mat& affine1, Mat& affine2, Mat& result);
void MakeWarpAffine(Mat& aff, Mat& OptimalAff, Mat& LRAff, Mat& warpAff);
#endif