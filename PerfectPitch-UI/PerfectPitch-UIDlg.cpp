
// PerfectPitch-UIDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "PerfectPitch-UI.h"
#include "PerfectPitch-UIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <chrono>

#pragma comment(lib,"winmm.lib")



int linearScore::index = 0;

int LUT_original[2][20] = {
		{ 41,39,37,36,34,32,30,29,27,25,24,22,20,18,17,15,13,12,10,8 },
	{ 20,18,17,15,13,12,10,8,6,5,3,1,0,-2,-4,-6,-7,-9,-11,-12 } };

HMIDIOUT     m_DevHandle=Midi::Open(0);
MMRESULT     m_MMErrCode;
MIDIOUTCAPS  m_NowMidiOutCaps;



Score myMusic(m_DevHandle, 150);
Score FinalScore(m_DevHandle, 150);


bool EoI = false;
cv::Mat image;
vector<int> lineArr;
vector<linearScore> LinScores;
int NumofLinears = 0;
//lines5=[NumofLinears][2][5]꼴임
int*** lines5 = new int**[NumofLinears];


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPerfectPitchUIDlg 대화 상자



CPerfectPitchUIDlg::CPerfectPitchUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PERFECTPITCHUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPerfectPitchUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCOREIMAGE, ScoreImage);
	DDX_Control(pDX, IDC_BTN_LOAD, btnLoad);
	DDX_Control(pDX, IDC_BTN_BIN, btnBin);
	DDX_Control(pDX, IDC_BTN_DETLINE, btnDetLine);
	DDX_Control(pDX, IDC_BTN_REMDUP, btnRemDup);
	DDX_Control(pDX, IDC_BTN_CUT, btnCut);
	DDX_Control(pDX, IDC_BTN_DETNOTE, btnDetNote);
	DDX_Control(pDX, IDC_BTN_PLAY, btnPlay);
}

BEGIN_MESSAGE_MAP(CPerfectPitchUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOAD, &CPerfectPitchUIDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_BIN, &CPerfectPitchUIDlg::OnBnClickedBtnBin)
	ON_BN_CLICKED(IDC_BTN_CUT, &CPerfectPitchUIDlg::OnBnClickedBtnCut)
	ON_BN_CLICKED(IDC_BTN_DETLINE, &CPerfectPitchUIDlg::OnBnClickedBtnDetline)
	ON_BN_CLICKED(IDC_BTN_REMDUP, &CPerfectPitchUIDlg::OnBnClickedBtnRemdup)
	ON_BN_CLICKED(IDC_BTN_DETNOTE, &CPerfectPitchUIDlg::OnBnClickedBtnDetnote)
	ON_BN_CLICKED(IDC_BTN_PLAY, &CPerfectPitchUIDlg::OnBnClickedBtnPlay)
END_MESSAGE_MAP()


// CPerfectPitchUIDlg 메시지 처리기

BOOL CPerfectPitchUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	CPerfectPitchUIDlg::btnBin.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_BIN)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnDetLine.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DETLINE)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnRemDup.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_REMDUP)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnCut.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CUT)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnDetNote.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DETNOTE)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnPlay.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CPerfectPitchUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPerfectPitchUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPerfectPitchUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CPerfectPitchUIDlg::SetPNGAlpha(CImage& _PictureImage, CString _FileName)
{
	if (_PictureImage.GetBPP() == 32)
	{
		if (_FileName.Right(3).CompareNoCase(_T("png")) == 0)
		{
			unsigned char * pCol = 0;
			long w = _PictureImage.GetWidth();
			long h = _PictureImage.GetHeight();
			for (long y = 0; y < h; y++)
			{
				for (long x = 0; x < w; x++)
				{
					pCol = (unsigned char *)_PictureImage.GetPixelAddress(x, y);
					unsigned char alpha = pCol[3];
					if (alpha != 255)
					{
						pCol[0] = ((pCol[0] * alpha) + 128) >> 8;
						pCol[1] = ((pCol[1] * alpha) + 128) >> 8;
						pCol[2] = ((pCol[2] * alpha) + 128) >> 8;
					}
				}
			}
		}
		_PictureImage.SetHasAlphaChannel(true);
	}
	return true;
}

bool CPerfectPitchUIDlg::SetPNGReSizeDraw(HDC _Hdc, CImage& _PictureImage, CRect _Rect)
{
	_PictureImage.Draw(_Hdc, _Rect.TopLeft().x, _Rect.TopLeft().y, _Rect.Width(), _Rect.Height());
	return true;
}

bool CPerfectPitchUIDlg::SetPNGReSizeRatioDraw(HDC _Hdc, CImage& _PictureImage, CRect _Rect)
{
	CRect Result = GetReSizeRatioRect(_Rect, CRect(0, 0, _PictureImage.GetWidth(), _PictureImage.GetHeight()));
	SetPNGReSizeDraw(_Hdc, _PictureImage, Result);
	return true;
}

bool CPerfectPitchUIDlg::SetPNGReSizeRatioDrawCenter(HDC _Hdc, CImage& _PictureImage, CRect _Rect)
{
	CRect Result = GetReSizeRatioRect(_Rect, CRect(0, 0, _PictureImage.GetWidth(), _PictureImage.GetHeight()));
	float _OriginWidthCenter = (float)_Rect.Width() / 2;
	float _OriginHeightCenter = (float)_Rect.Height() / 2;
	float _TransWidthLength = (float)Result.Width() / 2;
	float _TransHeightLength = (float)Result.Height() / 2;

	float _TransMoveWidth = _OriginWidthCenter - _TransWidthLength;
	float _TransMoveHeight = _OriginHeightCenter - _TransHeightLength;

	Result.left += _TransMoveWidth;
	Result.right += _TransMoveWidth;
	Result.top += _TransMoveHeight;
	Result.bottom += _TransMoveHeight;

	SetPNGReSizeDraw(_Hdc, _PictureImage, Result);

	return true;
}

CRect CPerfectPitchUIDlg::GetReSizeRatioRect(CRect _OriginRect, CRect _PrototypeRect)
{
	float OriginRatio = (float)_OriginRect.Width() / (float)_OriginRect.Height(); // 액자
	float PrototypeRatio = (float)_PrototypeRect.Width() / (float)_PrototypeRect.Height(); // 이미지

	float _ResultWidth = 0;
	float _ResultHeight = 0;
	if (OriginRatio >= PrototypeRatio)
	{
		// 높이를 기준으로 맞춘다
		_ResultHeight = _OriginRect.Height();
		float Ratio = (float)_OriginRect.Height() / (float)_PrototypeRect.Height();
		_ResultWidth = Ratio * _PrototypeRect.Width();
	}
	else
	{
		// 너비를 기준으로 맞춘다
		_ResultWidth = _OriginRect.Width();
		float Ratio = (float)_OriginRect.Width() / (float)_PrototypeRect.Width();
		_ResultHeight = Ratio * _PrototypeRect.Height();
	}

	return CRect(0, 0, _ResultWidth, _ResultHeight);
}

void CPerfectPitchUIDlg::OnBnClickedBtnLoad()
{
	CString strPathName;
	CString strFileName;

	char szFilter[] = "Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG;*.bmp;*.gif;*.jpg;*.png | All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter, 0, 0, 1);

	if (IDOK == dlg.DoModal())
	{
		strPathName = dlg.GetPathName();
		strFileName = dlg.GetFileName();
	}

	if (NULL != m_PictureImage)
	{
		m_PictureImage.Destroy();
		// 안해주면 기존의 이미지가 지워지지 않음
		Invalidate();
		UpdateWindow();
	}

	HRESULT hResult = m_PictureImage.Load(strPathName);

	if (FAILED(hResult)) {
		CString strTmp = _T("ERROR: Failed to load :");
		strTmp += strPathName;
		AfxMessageBox(strPathName);
		return;
	}


	if (ScoreImage.GetBitmap() == NULL)
	{
		SetPNGAlpha(m_PictureImage, strPathName);

		//정상적으로 띄워짐
		CRect StaticPictureRect;
		ScoreImage.GetClientRect(StaticPictureRect);
		SetPNGReSizeRatioDrawCenter(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
		//SetPNGReSizeRatioDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
		//SetPNGReSizeDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);

	}
	/////////////미디 생성 및 설정//////////////////////////////

	CPerfectPitchUIDlg::btnDetLine.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DETLINE)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnRemDup.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_REMDUP)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnCut.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_CUT)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnDetNote.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_DETNOTE)->EnableWindow(FALSE);
	CPerfectPitchUIDlg::btnPlay.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);

	
	//cstring을 string으로 변환
	CT2CA pszConvertedAnsiString(strPathName);
	string convStrPath(pszConvertedAnsiString);

	image = cv::imread(convStrPath, CV_LOAD_IMAGE_COLOR);
	//image = cv::imread("score_oneline.jpg", CV_LOAD_IMAGE_COLOR);
	//image = cv::imread("score.jpg", CV_LOAD_IMAGE_COLOR);

	CPerfectPitchUIDlg::btnBin.EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_BIN)->EnableWindow(TRUE);


}

void CPerfectPitchUIDlg::SaveImage(Mat targetMat, CString filePath)
{
	IplImage* targetIplImage = new IplImage(targetMat);
	if (targetIplImage != nullptr) {
		IplImage *tempImage = nullptr;

		if (targetIplImage->nChannels == 1)
		{
			tempImage = cvCreateImage(cvSize(targetIplImage->width, targetIplImage->height), IPL_DEPTH_8U, 3);
			cvCvtColor(targetIplImage, tempImage, CV_GRAY2BGR);
		}
		else if (targetIplImage->nChannels == 3)
		{
			tempImage = cvCloneImage(targetIplImage);
		}

		cvSaveImage(filePath, tempImage);
	}
}

void CPerfectPitchUIDlg::OnBnClickedBtnBin()
{
	AfxBeginThread(MyThreadBin, this);
	

}

UINT MyThreadBin(LPVOID pParam) {
	CPerfectPitchUIDlg* dialog = (CPerfectPitchUIDlg*)pParam;

	// 이진화 버튼 클릭시
	SetWindowText(dialog->m_hWnd, "Perfect Pitch - ( Processing Binarization... )");
	auto begin = chrono::high_resolution_clock::now();
	Pretreatment::Binarization(image, 200);
	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() / 1000.0;

	char sec[100];
	sprintf(sec, "Perfect Pitch - ( Binarization Finished, %f sec )", s);
	SetWindowText(dialog->m_hWnd, sec);

	dialog->SaveImage(image, "tempresult.png");

	if (NULL != dialog->m_PictureImage)
	{
		dialog->m_PictureImage.Destroy();
		// 안해주면 기존의 이미지가 지워지지 않음
		dialog->Invalidate();
		dialog->UpdateWindow();

		HRESULT hResult = dialog->m_PictureImage.Load("tempresult.png");

		if (FAILED(hResult)) {
			return 0;
		}


		if (dialog->ScoreImage.GetBitmap() == NULL)
		{
			dialog->SetPNGAlpha(dialog->m_PictureImage, "tempresult.png");

			//정상적으로 띄워짐
			CRect StaticPictureRect;
			dialog->ScoreImage.GetClientRect(StaticPictureRect);
			dialog->SetPNGReSizeRatioDrawCenter(dialog->ScoreImage.GetDC()->m_hDC, dialog->m_PictureImage, StaticPictureRect);
			//SetPNGReSizeRatioDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);

		}
		dialog->btnDetLine.EnableWindow(TRUE);
		dialog->GetDlgItem(IDC_BTN_DETLINE)->EnableWindow(TRUE);

	}

	return 0;
}

void CPerfectPitchUIDlg::OnBnClickedBtnDetline()
{
	// 오선 검출
	SetWindowText("Perfect Pitch - ( Detecting 5 Line... )");
	auto begin = chrono::high_resolution_clock::now();
	Pretreatment::DetectLine(image, lineArr);
	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() / 1000.0;

	char sec[100];
	sprintf(sec, "Perfect Pitch - ( Finished Detecting 5 Line, %f sec )", s);
	SetWindowText(sec);

	SaveImage(image, "tempresult.png");

	if (NULL != m_PictureImage)
	{
		m_PictureImage.Destroy();
		// 안해주면 기존의 이미지가 지워지지 않음
		Invalidate();
		UpdateWindow();

		HRESULT hResult = m_PictureImage.Load("tempresult.png");

		if (FAILED(hResult)) {
			return;
		}


		if (ScoreImage.GetBitmap() == NULL)
		{
			SetPNGAlpha(m_PictureImage, "tempresult.png");

			//정상적으로 띄워짐
			CRect StaticPictureRect;
			ScoreImage.GetClientRect(StaticPictureRect);
			SetPNGReSizeRatioDrawCenter(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeRatioDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);

		}

		CPerfectPitchUIDlg::btnRemDup.EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_REMDUP)->EnableWindow(TRUE);

	}
}

void CPerfectPitchUIDlg::OnBnClickedBtnRemdup()
{
	// 오선 픽셀 정리.
	SetWindowText("Perfect Pitch( Removing Duplicates  ... )");
	auto begin = chrono::high_resolution_clock::now();
	Pretreatment::RemoveDup(lineArr);
	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() / 1000.0;

	char sec[100];
	sprintf(sec, "Perfect Pitch - ( Finished Removing Duplicates, %f sec )", s);
	SetWindowText(sec);

	SaveImage(image, "tempresult.png");

	if (NULL != m_PictureImage)
	{
		m_PictureImage.Destroy();
		// 안해주면 기존의 이미지가 지워지지 않음
		Invalidate();
		UpdateWindow();

		HRESULT hResult = m_PictureImage.Load("tempresult.png");

		if (FAILED(hResult)) {
			return;
		}


		if (ScoreImage.GetBitmap() == NULL)
		{
			SetPNGAlpha(m_PictureImage, "tempresult.png");

			//정상적으로 띄워짐
			CRect StaticPictureRect;
			ScoreImage.GetClientRect(StaticPictureRect);
			SetPNGReSizeRatioDrawCenter(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeRatioDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);

		}

		CPerfectPitchUIDlg::btnCut.EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_CUT)->EnableWindow(TRUE);

	}
	return;
}


void CPerfectPitchUIDlg::OnBnClickedBtnCut()
{
	AfxBeginThread(MyThreadCut, this);
}

UINT MyThreadCut(LPVOID pParam) {
	//악보 자르기
	CPerfectPitchUIDlg* dialog = (CPerfectPitchUIDlg*)pParam;

	SetWindowText(dialog->m_hWnd, "Perfect Pitch - ( Cutting Music Sheet ... )");
	auto begin = chrono::high_resolution_clock::now();
	while (!EoI)
	{
		linearScore temp0(image, lineArr, EoI);
		LinScores.push_back(temp0);
		NumofLinears++;

		char* text = new char[100];
		sprintf(text, "Perfect Pitch - ( Cutting Music Sheet... # %d done )", NumofLinears);
		SetWindowText(dialog->m_hWnd, text);
	}

	for (int i = 0; i < NumofLinears; i++)
	{
		lines5[i] = new int*[2];
		for (int j = 0; j < 2; j++)
		{
			lines5[i][j] = new int[5];
		}
	}
	for (int n = 0; n < lineArr.size(); n++)
	{
		int i = n / 10;
		int j = n % 5;
		int k;
		if (n % 10 < 5)
			k = 0;
		else k = 1;
		lines5[i][k][j] = lineArr[n];

		char* text = new char[100];
		if (k == 0) {
			sprintf(text, "퍼펙트피치 - 영상처리 과제 ( 악보 자르는 중... %d번 보표의 오른손 멜로디 )", i);
		}
		else {
			sprintf(text, "퍼펙트피치 - 영상처리 과제 ( 악보 자르는 중... %d번 보표의 왼손 멜로디 )", i);
		}
		SetWindowText(dialog->m_hWnd, text);
	}

	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() / 1000.0;

	char sec[100];
	sprintf(sec, "Perfect Pitch - ( Finished Cutting Music Sheet, %f sec )", s);
	SetWindowText(dialog->m_hWnd, sec);
	
	Mat result = LinScores[0].oneline[0];
	for (int i = 0; i < NumofLinears; i++)
	{
		for (int j = 0; j < 2; j++)
		{	
			if (i == 0 && j == 0) continue;
			cv::vconcat(result, LinScores[i].oneline[j], result);
		}
	}

	dialog->SaveImage(result, "tempresult.png");
	
	if (NULL != dialog->m_PictureImage)
	{
		dialog->m_PictureImage.Destroy();
		// 안해주면 기존의 이미지가 지워지지 않음
		dialog->Invalidate();
		dialog->UpdateWindow();

		HRESULT hResult = dialog->m_PictureImage.Load("tempresult.png");

		if (FAILED(hResult)) {
			return 0;
		}


		if (dialog->ScoreImage.GetBitmap() == NULL)
		{
			dialog->SetPNGAlpha(dialog->m_PictureImage, "tempresult.png");

			//정상적으로 띄워짐
			CRect StaticPictureRect;
			dialog->ScoreImage.GetClientRect(StaticPictureRect);
			dialog->SetPNGReSizeRatioDrawCenter(dialog->ScoreImage.GetDC()->m_hDC, dialog->m_PictureImage, StaticPictureRect);
			//SetPNGReSizeRatioDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);

		}
		dialog->btnDetNote.EnableWindow(TRUE);
		dialog->GetDlgItem(IDC_BTN_DETNOTE)->EnableWindow(TRUE);

	}

	return 0;
}


void CPerfectPitchUIDlg::OnBnClickedBtnDetnote()
{
	AfxBeginThread(MyThreadDetnote, this);
}

UINT MyThreadDetnote(LPVOID pParam) {
	CPerfectPitchUIDlg* dialog = (CPerfectPitchUIDlg*)pParam;

	Midi::AllChannelSoundOff(m_DevHandle);
	Midi::SendShortMsg(m_DevHandle, 0xB0, 7, 127);

	SetWindowText(dialog->m_hWnd, "Perfect Pitch - ( Detecting Music Notes... )");
	//////////////////////////////악보 처리///////////////////////////////

	std::cout << "악보 처리 시작" << endl;
	auto begin = chrono::high_resolution_clock::now();
	for (int i = 0; i < NumofLinears; i++)
	{
		for (int RL = 0; RL < 2; RL++)
		{
			char* text = new char[100];
			sprintf(text, "Perfect Pitch - ( Detecting Music Notes... # %d done / %d )", i * 2 + RL + 1, NumofLinears * 2);
			SetWindowText(dialog->m_hWnd, text);
			ScoreProcessor myProc(LinScores[i].oneline[RL], &myMusic, lines5[i][RL], LUT_original, RL);
			if (RL == 0)
				std::cout << "\n" << i + 1 << "번째 오른손 라인 탐색 시작" << endl;
			else
				std::cout << "\n" << i + 1 << "번째 왼손 라인 탐색 시작" << endl;
			myProc.DetectNote();
		}
	}
	myMusic.NewNote(0, 8);
	myMusic.NewNote(1, 8);

	////////////////////////악보 정리(도돌이표)////////////////////////////////
	

	int RightRecurIndex = -1;
	int LeftRecurIndex = 0;
	bool jumpState = false;
	int jumpIndex = -1;
	std::cout << "////////////////////////////////////////////////////////////////////////////////" << endl;
	std::cout << "악보 정리 시작" << endl;
	int boundary = myMusic.Threads_R[0].notes.size();
	if (boundary > myMusic.Threads_L[0].notes.size())
		boundary = myMusic.Threads_L[0].notes.size();
	for (int i = 0; i < boundary; i++)
	{
		if (myMusic.Threads_R[0].notes[i] > -50) //오른손이 정상적인 음일때
		{
			if (myMusic.Threads_L[0].notes[i] > -50) //둘다 정상적인 음일때 그냥 푸쉬
			{
				for (int j = 0; j < 5; j++)
				{
					if (i < myMusic.Threads_R[j].notes.size())
					{
						FinalScore.Threads_R[j].notes.push_back(myMusic.Threads_R[j].notes[i]);
					}
					if (i < myMusic.Threads_L[j].notes.size())
					{
						FinalScore.Threads_L[j].notes.push_back(myMusic.Threads_L[j].notes[i]);
					}
				}
			}
			else //왼손이 비정상적인 음일때는 오른손만 푸쉬
			{
				for (int j = 0; j < 5; j++)
				{
					if (i < myMusic.Threads_R[j].notes.size())
					{
						FinalScore.Threads_R[j].notes.push_back(myMusic.Threads_R[j].notes[i]);
					}
				}
			}
		}
		else //오른손이 비정상적인 음일때
		{
			if (myMusic.Threads_L[0].notes[i] > -50) //왼손은 정상일때 왼손만 푸쉬
			{
				for (int j = 0; j < 5; j++)
				{
					if (i < myMusic.Threads_L[j].notes.size())
					{
						FinalScore.Threads_L[j].notes.push_back(myMusic.Threads_L[j].notes[i]);
					}
				}
			}
			else //둘다 비정상적인 음일때
			{
				if (myMusic.Threads_R[0].notes[i] == myMusic.Threads_L[0].notes[i]) //같은거 동시에 찾음
				{
					if (myMusic.Threads_R[0].notes[i] == -100) // 오른쪽 도돌이표 발견
					{
						RightRecurIndex = i;
						std::cout << "오른쪽 도돌표 발견, RightRecurIndex = " << RightRecurIndex << endl;////////////////
					}
					else if (myMusic.Threads_R[0].notes[i] == -101) // 왼쪽 도돌이표 발견
					{
						LeftRecurIndex = i;
						std::cout << "왼쪽 도돌이표 발견, JumpState =" << jumpState << ", jumpIndex:" << jumpIndex << ", LeftRecurIndex = " << LeftRecurIndex << endl;//
						if (jumpState&&RightRecurIndex < jumpIndex) //현재까지 건너뛰기 표시가 있었음
						{
							for (int j = RightRecurIndex + 1; j < jumpIndex; j++)
							{
								if (myMusic.Threads_R[0].notes[j] > -50 && myMusic.Threads_L[0].notes[j] > -50) //왼쪽 도돌이표 발견, state = true상태
								{
									for (int k = 0; k < 5; k++)
									{
										FinalScore.Threads_R[k].notes.push_back(myMusic.Threads_R[k].notes[j]);
										FinalScore.Threads_L[k].notes.push_back(myMusic.Threads_L[k].notes[j]);
									}
								}
							}
							std::cout << "state:true->false, " << RightRecurIndex + 1 << "부터 " << jumpIndex - 1 << "(점프인덱스)까지 복사함" << endl;///////////////
							jumpState = false;
						}
						else if (!jumpState)
						{
							for (int j = RightRecurIndex + 1; j < i; j++)
							{
								if (myMusic.Threads_R[0].notes[j] > -50 && myMusic.Threads_L[0].notes[i] > -50)
								{
									for (int k = 0; k < 5; k++)
									{
										FinalScore.Threads_R[k].notes.push_back(myMusic.Threads_R[k].notes[j]);
										FinalScore.Threads_L[k].notes.push_back(myMusic.Threads_L[k].notes[j]);
									}
								}
							}
							std::cout << RightRecurIndex + 1 << "부터 " << i - 1 << "까지 복사함" << endl;/////////////////
						}
					}
					else if (myMusic.Threads_R[0].notes[i] = -102) //건너뛰기 표시 발견
					{
						if (!jumpState)
						{
							jumpIndex = i;
							cout << "건너뛰기 표시 홀수 발견,jumpindex :" << jumpIndex << endl;
							jumpState = true;
						}
						else
						{
							cout << "건너뛰기 표시 짝수 발견, jumpIndex" << jumpIndex << endl;
							jumpIndex = -1;
							jumpState = false;
						}
					}
				}
			}
		}
	}



	std::cout << "악보 처리 완료" << endl;

	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() / 1000.0;

	char sec[100];
	sprintf(sec, "Perfect Pitch - ( Finished Detecting Music Notes, %f sec )", s);
	SetWindowText(dialog->m_hWnd, sec);

	Mat result = LinScores[0].oneline[0];
	for (int i = 0; i < NumofLinears; i++)
	{
		for (int j = 0; j < 2; j++)
		{	
			if (i == 0 && j == 0) continue;
			cv::vconcat(result, LinScores[i].oneline[j], result);
		}
	}

	dialog->SaveImage(result, "tempresult.png");

	if (NULL != dialog->m_PictureImage)
	{
		dialog->m_PictureImage.Destroy();
		// 안해주면 기존의 이미지가 지워지지 않음
		dialog->Invalidate();
		dialog->UpdateWindow();

		HRESULT hResult = dialog->m_PictureImage.Load("tempresult.png");

		if (FAILED(hResult)) {
			return 0;
		}


		if (dialog->ScoreImage.GetBitmap() == NULL)
		{
			dialog->SetPNGAlpha(dialog->m_PictureImage, "tempresult.png");

			//정상적으로 띄워짐
			CRect StaticPictureRect;
			dialog->ScoreImage.GetClientRect(StaticPictureRect);
			dialog->SetPNGReSizeRatioDrawCenter(dialog->ScoreImage.GetDC()->m_hDC, dialog->m_PictureImage, StaticPictureRect);
			//SetPNGReSizeRatioDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);
			//SetPNGReSizeDraw(ScoreImage.GetDC()->m_hDC, m_PictureImage, StaticPictureRect);

		}

	}
	dialog->btnPlay.EnableWindow(TRUE);
	dialog->GetDlgItem(IDC_BTN_PLAY)->EnableWindow(TRUE);

	return 0;
}

void CPerfectPitchUIDlg::OnBnClickedBtnPlay()
{
	AfxBeginThread(MyThreadProc, this);
	GetDlgItem(IDC_BTN_PLAY)->EnableWindow(FALSE);
}

UINT MyThreadProc(LPVOID pParam) {
	CPerfectPitchUIDlg* dialog = (CPerfectPitchUIDlg*)pParam;

	CString str;
	dialog->btnPlay.GetWindowTextA(str);
	if (str.Compare(""))
	SetWindowText(dialog->m_hWnd, "Perfect Pitch - ( Currently Playing Music... )");
	auto begin = chrono::high_resolution_clock::now();

	FinalScore.setTempo(90); //디폴트는 150, 버튼으로 템포 지정 가능하면 좋음. 75하면 두배빨라짐
	FinalScore.setVolume_R(100);
	FinalScore.setVolume_L(80);
	FinalScore.PlayMusic();

	auto end = chrono::high_resolution_clock::now();
	auto dur = end - begin;
	auto s = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() / 1000.0;

	char sec[100];
	sprintf(sec, "Perfect Pitch - (Finished Playing Music, %f sec )", s);
	SetWindowText(dialog->m_hWnd, sec);

	dialog->btnPlay.EnableWindow(TRUE);

	return 0;
}