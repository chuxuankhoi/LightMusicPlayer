
// LightMusicPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LightMusicPlayer.h"
#include "LightMusicPlayerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLightMusicPlayerDlg dialog

CLightMusicPlayerDlg::CLightMusicPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLightMusicPlayerDlg::IDD, pParent)
	, m_iState(-1)
	, m_pGraph(NULL)
	, m_pEvent(NULL)
	, m_pControl(NULL)
	, m_pBasicAudio(NULL)
	, m_iPlayingFileIndex(0)
	, m_iPrevFileIndex(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CLightMusicPlayerDlg::~CLightMusicPlayerDlg()
{
	CleanUp();
	CoUninitialize();	
}

void CLightMusicPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILELISTBOX, m_cListBox);
	DDX_Control(pDX, IDC_BTNREMOVE, m_cBtnRemove);
	DDX_Control(pDX, IDC_BTNPLAYPAUSE, m_cBtnPlayPause);
	DDX_Control(pDX, IDC_CHECK1, m_cShuffle);
	DDX_Control(pDX, IDC_CURFILE, m_cCurFile);
	DDX_Control(pDX, IDC_SLDVOLUME, m_cVolume);
}

BEGIN_MESSAGE_MAP(CLightMusicPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PLAYPAUSE, &CLightMusicPlayerDlg::OnBnClickedPlaypause)
	ON_BN_CLICKED(IDC_BTNADD, &CLightMusicPlayerDlg::OnBnClickedBtnadd)
	ON_BN_CLICKED(IDC_BTNREMOVE, &CLightMusicPlayerDlg::OnBnClickedBtnremove)
	ON_BN_CLICKED(IDC_BTNSTOP, &CLightMusicPlayerDlg::OnBnClickedBtnstop)
	ON_BN_CLICKED(IDC_BTNPREV, &CLightMusicPlayerDlg::OnBnClickedBtnprev)
	ON_BN_CLICKED(IDC_BTNNEXT, &CLightMusicPlayerDlg::OnBnClickedBtnnext)
	ON_MESSAGE(WM_GRAPH_EVENT, OnGraphEvent)
	ON_BN_CLICKED(IDC_BTNSAVE, &CLightMusicPlayerDlg::OnBnClickedBtnsave)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CLightMusicPlayerDlg message handlers

BOOL CLightMusicPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	TrayMessage(NIM_ADD);

	// For initializing list - should load from file
	CStdioFile file;
	CString strLine;

	if ( file.Open( _T("list.dat"), CFile::modeRead) )
	{
		while(file.ReadString(strLine)) {		
			strLine.Trim();
			if(strLine != L"") {
				m_cListBox.AddItem(strLine);
			}
		}		
		file.Close ();
	}

	// Initialize COM
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot initialize COM!", MB_OK | MB_ICONSTOP);
		PostMessage(WM_CLOSE); // Close the application
		return TRUE;
	}

	// Set slider's properties
	m_cVolume.SetRange(0, 100);
	m_cVolume.SetPos(75);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CLightMusicPlayerDlg::TrayMessage(DWORD swMessage)
{
	CString sTip(_T("Light Music Player")); 
	NOTIFYICONDATA tnd;
	tnd.cbSize = sizeof(NOTIFYICONDATA);
	tnd.hWnd = m_hWnd;
	tnd.uID = IDR_TRAYICON;
	tnd.uFlags = NIF_MESSAGE|NIF_ICON;
	tnd.uCallbackMessage = WM_NOTIFYICON;
	tnd.uFlags = NIF_MESSAGE|NIF_ICON|NIF_TIP; 
	VERIFY( 
		tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), 
		MAKEINTRESOURCE (IDR_TRAYICON)) 
		);
	lstrcpyn(tnd.szTip, (LPCTSTR)sTip, sizeof(tnd.szTip));

	return Shell_NotifyIcon(swMessage, &tnd);
}

void CLightMusicPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if(nID == SC_MINIMIZE)
	{
		//TRACE(L"Minimize to system tray\n");
		ShowWindow(FALSE);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLightMusicPlayerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLightMusicPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLightMusicPlayerDlg::OnBnClickedPlaypause()
{
	HRESULT hr;
	if(m_iState == 0 || m_iState == -1) {
		// Update list of music
		int itemNum = m_cListBox.GetCount();
		if(itemNum == 0)
			return;
		//TRACE("Files number: %d\n", itemNum);
		filesList.clear();
		for(int i = 0; i < itemNum; i++) {
			filesList.push_back(m_cListBox.GetItemText(i));
		}

		m_cBtnRemove.EnableWindow(FALSE);

		// Create player's controller if needed including checking existance
		CFileStatus status;
		if(!CFile::GetStatus(filesList[0], status)) {
			CString message;
			message.Format(L"No file found in %s!", filesList[0]);
			AfxMessageBox(message, MB_OK | MB_ICONSTOP);
			return;
		}
		if(m_iState == -1) {
			if(!LoadFile(filesList[0])) {
				AfxMessageBox(L"The first file is incorrect!", MB_OK | MB_ICONSTOP);
				return;
			}
		}

		// Play loaded audio
		hr = m_pControl->Run();
		if (FAILED(hr))
		{
			AfxMessageBox(L"Cannot run!", MB_OK | MB_ICONSTOP);
			return;
		}

		m_iState = 1; // playing
		m_cBtnPlayPause.SetWindowTextW(L"Pause");
	} else if(m_iState == 1) {
		// Call to pause
		hr = m_pControl->Pause();
		if (FAILED(hr))
		{
			AfxMessageBox(L"Cannot pause!", MB_OK | MB_ICONSTOP);
			return;
		}

		m_iState = 0; // pausing
		m_cBtnPlayPause.SetWindowTextW(L"Play");
		m_cBtnRemove.EnableWindow(TRUE);
	} else {
		AfxMessageBox(L"Unexpected state!", MB_OK | MB_ICONSTOP);
	}
}


void CLightMusicPlayerDlg::OnBnClickedBtnadd()
{
	// TODO Add more audio file for filter
	TCHAR szFilters[]= _T("Audio Files (*.mp3)|*.mp3|All Files (*.*)|*.*|||");

	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |  OFN_ALLOWMULTISELECT, szFilters);
	OPENFILENAME& ofn = fileDlg.GetOFN();
	CString str;
	ofn.lpstrFile = str.GetBuffer(FILE_LIST_BUFFER_SIZE);
	ofn.nMaxFile = FILE_LIST_BUFFER_SIZE;

	if( fileDlg.DoModal() == IDOK )
	{
		POSITION pos ( fileDlg.GetStartPosition() );
		while( pos )
		{
			CString filename= fileDlg.GetNextPathName(pos);
			//TRACE(L"Path: %s - Length: %d\n", filename, filename.GetLength());
			if(filename != "") {
				m_cListBox.AddItem(filename);
			}
		}
	}
}


void CLightMusicPlayerDlg::OnBnClickedBtnremove()
{
	int curIdx = m_cListBox.GetSelItem();
	if(curIdx > -1) {
		m_cListBox.RemoveItem(curIdx);
	}
}


void CLightMusicPlayerDlg::OnBnClickedBtnstop()
{
	if(m_iState == -1)
		return;
	HRESULT hr = m_pControl->Stop();
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot stop!", MB_OK | MB_ICONSTOP);
		return;
	}

	m_cBtnRemove.EnableWindow(TRUE);
	m_cBtnPlayPause.SetWindowTextW(L"Play");
	m_iState = -1;
}


void CLightMusicPlayerDlg::OnBnClickedBtnprev()
{
	if(m_iState == -1)
		return;
	m_iPlayingFileIndex = m_iPrevFileIndex;
	if(!LoadFile(filesList[m_iPlayingFileIndex]))
		return;
	if(m_pControl)
		m_pControl->Run();
}


void CLightMusicPlayerDlg::OnBnClickedBtnnext()
{
		
	if(m_iState == -1)
		return;
	if(m_pControl) {
		m_pControl->Stop();
	}
	m_iPrevFileIndex = m_iPlayingFileIndex;
	if((m_cShuffle.GetState() & BST_CHECKED) != 0) {
		m_iPlayingFileIndex = GetRandomNumber(
			0, (int)filesList.size() - 1, m_iPlayingFileIndex);
	} else {
		m_iPlayingFileIndex++;
		if(m_iPlayingFileIndex >= filesList.size()) {
			m_iPlayingFileIndex = 0;
		}
	}
	if(!LoadFile(filesList[m_iPlayingFileIndex])) {
		OnBnClickedBtnnext();
		return;
	}
	if(m_pControl)
		m_pControl->Run();
}

LRESULT CLightMusicPlayerDlg::OnGraphEvent(WPARAM wParam, LPARAM lParam)
{
	if (!m_pEvent)
	{
		return 0;
	}

	long evCode = 0;
	LONG_PTR param1 = 0, param2 = 0;

	HRESULT hr = S_OK;

	// Get the events from the queue.
	while (SUCCEEDED(m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
	{
		// Invoke the callback.
		switch (evCode)
		{
		case EC_COMPLETE:
			OnBnClickedBtnnext();
			break;
		case EC_USERABORT:
			OnBnClickedBtnstop();
			break;

		case EC_ERRORABORT:
			OnBnClickedBtnstop();
			break;
		}

		// Free the event data.
		hr = m_pEvent->FreeEventParams(evCode, param1, param2);
		if (FAILED(hr))
		{
			AfxMessageBox(L"Fail when releasing event!", MB_OK | MB_ICONSTOP);
			break;
		}
	}

	return 0;
}

BOOL CLightMusicPlayerDlg::CleanUp()
{
	if(m_pBasicAudio) {
		m_pBasicAudio->Release();
		m_pBasicAudio = NULL;
	}
	if(m_pEvent) {
		m_pEvent->SetNotifyWindow(NULL, 0, 0);
		m_pEvent->Release();
		m_pEvent = NULL;
	}
	if(m_pControl) {
		m_pControl->Release();
		m_pControl = NULL;
	}	
	if(m_pGraph) {
		m_pGraph->Release();
		m_pGraph = NULL;
	}
	return TRUE;
}

BOOL CLightMusicPlayerDlg::LoadFile(CString filePath)
{
	HRESULT hr;
	CleanUp();
	// Filter Graph Manager
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, 
		CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&m_pGraph);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot create Filter Graph Manager!", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Media Controller
	hr = m_pGraph->QueryInterface(IID_IMediaControl, (void **)&m_pControl);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot create stream's controller!", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Events Manager
	hr = m_pGraph->QueryInterface(IID_IMediaEvent, (void **)&m_pEvent);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot create event manager!", MB_OK | MB_ICONSTOP);
		return FALSE;
	}
	// Set up event notification.
	hr = m_pEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_GRAPH_EVENT, NULL);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot create event notifier!", MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	// Volume manager
	hr = m_pGraph->QueryInterface(IID_IBasicAudio,(void**)&m_pBasicAudio);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot create audio controller!", MB_OK | MB_ICONSTOP);
		return TRUE;
	}

	//TODO: Seek manager

	hr = m_pGraph->RenderFile(filePath, NULL);
	if (FAILED(hr))
	{
		AfxMessageBox(L"Cannot render the file!\n");
		return FALSE;
	}

	long curVolume = m_cVolume.GetPos() * 10000 / 100 - 10000;
	hr = m_pBasicAudio->put_Volume(curVolume);
	if(FAILED(hr)) {
		CString msg;
		msg.Format(L"Cannot set volume with error: %x", hr);
		AfxMessageBox(msg, MB_OK | MB_ICONSTOP);
		return FALSE;
	}

	m_cCurFile.SetWindowTextW(filePath);
	return TRUE;
}

int CLightMusicPlayerDlg::GetRandomNumber(int start, int range, int exclude){
	int ret = 0;
	while ((ret = (rand() % (range - start) + start)) == exclude) {
		// Do next loop
	}
	return ret;
}

void CLightMusicPlayerDlg::OnBnClickedBtnsave()
{
	CString strList;
	for(int i = 0; i < m_cListBox.GetCount(); i++) {
		strList.Append(m_cListBox.GetItemText(i));
		strList.Append(L"\n");
	}
	CStdioFile file;

	if ( file.Open( _T("list.dat"), CFile::modeCreate|CFile::modeWrite) )
	{
		file.WriteString (strList);
		file.Close ();
	}
}


void CLightMusicPlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* object = (CSliderCtrl*)pScrollBar;
	if(object == &m_cVolume) {
		//TRACE(L"Received from slider: %d\n", m_cVolume.GetPos());
		long curVolume = m_cVolume.GetPos() * 10000 / 100 - 10000;
		HRESULT hr = m_pBasicAudio->put_Volume(curVolume);
		if(FAILED(hr)) {
			AfxMessageBox(L"Cannot set volume!", MB_OK | MB_ICONSTOP);
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}

LRESULT CLightMusicPlayerDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_NOTIFYICON){ 
		switch (lParam){

		case WM_LBUTTONDBLCLK: 
			switch (wParam) {

			case IDR_TRAYICON:
				ShowWindow(SW_NORMAL);
				SetForegroundWindow();
				SetFocus();
				return TRUE; 
				break; 
			} 
			break; 
		} 
	}

	return CDialogEx::WindowProc(message, wParam, lParam);
}
