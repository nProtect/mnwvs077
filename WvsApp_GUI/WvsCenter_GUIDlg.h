
// WvsCenter_GUIDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <string>


// CWvsCenter_GUIDlg dialog
class CWvsCenter_GUIDlg : public CDialogEx
{
// Construction
public:
	CWvsCenter_GUIDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WVSCENTER_GUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_ConsoleOutput, m_CommandLog;
	CEdit m_CommandInput;
	CStatic m_InputBoxCaption;
	afx_msg void OnEnChangeEdit1();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void AppendMessage(int nLogLevel, const std::string& sMsg);
};
