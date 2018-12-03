
// PerfectPitch-UIDlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "PerfectPitch-UI.h"
#include "PerfectPitch-UIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"

#pragma comment(lib,"winmm.lib")



int linearScore::index = 0;


HMIDIOUT     m_DevHandle=Midi::Open(0);
MMRESULT     m_MMErrCode;
MIDIOUTCAPS  m_NowMidiOutCaps;



Score myMusic(m_DevHandle, 150);


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
	DDX_Control(pDX, IDC_BTN_DETNOTE, btnDetNote);
	DDX_Control(pDX, IDC_BTN_PLAY, btnPlay);
	DDX_Control(pDX, IDC_BTN_STOP, btnStop);
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
	ON_BN_CLICKED(IDC_BTN_STOP, &CPerfectPitchUIDlg::OnBnClickedBtnStop)
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

	

	
	//cstring을 string으로 변환
	CT2CA pszConvertedAnsiString(strPathName);
	string convStrPath(pszConvertedAnsiString);

	image = cv::imread(convStrPath, CV_LOAD_IMAGE_COLOR);
	//image = cv::imread("score_oneline.jpg", CV_LOAD_IMAGE_COLOR);
	//image = cv::imread("score.jpg", CV_LOAD_IMAGE_COLOR);



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
	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 이진화 중... )");
	Pretreatment::Binarization(image, 200);
	dialog->SaveImage(image, "tempresult.png");
	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 이진화 완료 )");

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

	return 0;
}

void CPerfectPitchUIDlg::OnBnClickedBtnDetline()
{
	// 오선 검출
	SetWindowText("퍼펙트피치 - 영상처리 과제 ( 오선 검출 중... )");
	Pretreatment::DetectLine(image, lineArr);
	SetWindowText("퍼펙트피치 - 영상처리 과제 ( 오선 검출 완료 )");	
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

	}
}

void CPerfectPitchUIDlg::OnBnClickedBtnRemdup()
{
	// 오선 픽셀 정리.
	SetWindowText("퍼펙트피치 - 영상처리 과제 ( 오선 정리하는 중... )");
	Pretreatment::RemoveDup(lineArr);
	SetWindowText("퍼펙트피치 - 영상처리 과제 ( 오선 정리 완료 )");
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

	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 악보 자르는 중... )");
	while (!EoI)
	{
		linearScore temp0(image, lineArr, EoI);
		LinScores.push_back(temp0);
		NumofLinears++;

		char* text = new char[100];
		sprintf(text, "퍼펙트피치 - 영상처리 과제 ( 악보 자르는 중... %d개 완료 )", NumofLinears);
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
	}

	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 악보 자르기 완료 )");

	/*
	for (int i = NumofLinears - 1; i >= 0; i--)
	{
		for (int j = 0; j < 2; j++)
		{
			ostringstream o;
			o << 2 * i + j;
			string str = o.str();
			cv::namedWindow(str, CV_WINDOW_AUTOSIZE);
			cv::imshow(str, LinScores[i].oneline[j]);
		}
	}
	*/
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

	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 음표 검출 중... )");
	for (int i = 0; i < NumofLinears; i++)
	{
		for (int RL = 0; RL < 2; RL++)
		{
			char* text = new char[100];
			sprintf(text, "퍼펙트피치 - 영상처리 과제 ( 음표 검출 중... %d번 악보 / %d개 )", i * 2 + RL + 1, NumofLinears * 2);
			SetWindowText(dialog->m_hWnd, text);
			ScoreProcessor myProc(LinScores[i].oneline[RL], &myMusic, lines5[i][RL], RL);
			/*
			if (RL == 0)
				cout << "\n" << i + 1 << "번째 오른손 라인 탐색 시작" << endl;
			else
				cout << "\n" << i + 1 << "번째 왼손 라인 탐색 시작" << endl;
			*/
			myProc.DetectNote();
		}
	}
	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 음표 검출 완료 )");
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

	}

	return 0;
}

void CPerfectPitchUIDlg::OnBnClickedBtnPlay()
{
	AfxBeginThread(MyThreadProc, this);
}

UINT MyThreadProc(LPVOID pParam) {
	CPerfectPitchUIDlg* dialog = (CPerfectPitchUIDlg*)pParam;

	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 악보 연주 중... )");
	myMusic.setTempo(75); //디폴트는 150, 버튼으로 템포 지정 가능하면 좋음. 75하면 두배빨라짐
	myMusic.setVolume_R(100);
	myMusic.setVolume_L(0);
	myMusic.PlayMusic();
	SetWindowText(dialog->m_hWnd, "퍼펙트피치 - 영상처리 과제 ( 악보 연주  완료 )");

	return 0;
}


void CPerfectPitchUIDlg::OnBnClickedBtnStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetWindowText("퍼펙트피치 - 영상처리 과제 ( 악보 재생 멈춤 )");
	myMusic.setVolume_L(0);
	myMusic.setVolume_R(0);
}
