
// LightMusicPlayerDlg.h : header file
//

#pragma once
#include "afxvslistbox.h"

#include <vector>
#include <dshow.h>
#include "afxwin.h"
#include "afxcmn.h"

#define MAX_CFileDialog_FILE_COUNT 99
#define FILE_LIST_BUFFER_SIZE ((MAX_CFileDialog_FILE_COUNT * (MAX_PATH + 4)) + 1)
#define WM_GRAPH_EVENT WM_APP+13
#define WM_NOTIFYICON (WM_USER+2)

using namespace std;

// CLightMusicPlayerDlg dialog

class CLightMusicPlayerDlg : public CDialogEx
{
// Construction
public:
	CLightMusicPlayerDlg(CWnd* pParent = NULL);	// standard constructor
	~CLightMusicPlayerDlg();

// Dialog Data
	enum { IDD = IDD_LIGHTMUSICPLAYER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	BOOL TrayMessage(DWORD swMessage);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CVSListBox m_cListBox;
	INT m_iState; // -1: Stopping; 0: Pausing; 1: Playing
	INT m_iPlayingFileIndex;
	INT m_iPrevFileIndex;
	vector<CString> filesList;
public:
	afx_msg void OnBnClickedPlaypause();
	afx_msg void OnBnClickedBtnadd();
	afx_msg void OnBnClickedBtnremove();
	afx_msg void OnBnClickedBtnstop();
	afx_msg void OnBnClickedBtnprev();
	afx_msg void OnBnClickedBtnnext();
	LRESULT OnGraphEvent(WPARAM wParam, LPARAM lParam);
protected:
	CButton m_cBtnRemove;
	CButton m_cBtnPlayPause;
	// DirectShow
	IGraphBuilder *m_pGraph;
	IMediaControl *m_pControl;
	IMediaEventEx   *m_pEvent;
	IBasicAudio		*m_pBasicAudio;
	BOOL CleanUp();
	BOOL LoadFile(CString filePath);
	CButton m_cShuffle;
	int GetRandomNumber(int start, int range, int exclude = INT_MAX);
	CStatic m_cCurFile;
public:
	afx_msg void OnBnClickedBtnsave();
	CSliderCtrl m_cVolume;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
