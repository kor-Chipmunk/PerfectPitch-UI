
// PerfectPitch-UIDlg.h: 헤더 파일
//

#pragma once


// CPerfectPitchUIDlg 대화 상자
class CPerfectPitchUIDlg : public CDialogEx
{
// 생성입니다.
public:
	CPerfectPitchUIDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PERFECTPITCHUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	bool SetPNGAlpha(CImage& _PictureImage, CString _FileName);
	bool SetPNGReSizeDraw(HDC _Hdc, CImage& _PictureImage, CRect _Rect);
	bool SetPNGReSizeRatioDraw(HDC _Hdc, CImage& _PictureImage, CRect _Rect);
	bool SetPNGReSizeRatioDrawCenter(HDC _Hdc, CImage& _PictureImage, CRect _Rect);
	CRect GetReSizeRatioRect(CRect _OriginRect, CRect _PrototypeRect);

public:
	CImage m_PictureImage;
	CStatic ScoreImage;
	CButton btnLoad;
	CButton btnBin;
	CButton btnDetLine;
	CButton btnRemDup;
	CButton btnDetNote;
	CButton btnPlay;
	CButton btnStop;
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnBin();
	afx_msg void OnBnClickedBtnCut();
	afx_msg void OnBnClickedBtnDetline();
	afx_msg void OnBnClickedBtnRemdup();
	afx_msg void OnBnClickedBtnDetnote();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnStop();
};

UINT MyThreadBin(LPVOID pParam);

UINT MyThreadCut(LPVOID pParam);

UINT MyThreadDetnote(LPVOID pParam);

UINT MyThreadProc(LPVOID pParam);
