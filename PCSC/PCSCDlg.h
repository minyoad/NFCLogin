// PCSCDlg.h : 头文件
//

#pragma once

#include"winscard.h"
#include "afxwin.h"
// CPCSCDlg 对话框
class CPCSCDlg : public CDialog
{
// 构造
public:
	CPCSCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_PCSC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedScardestablishcontext();
	afx_msg void OnBnClickedButton9();
	afx_msg void OnBnClickedClear();
	CString edit;
	afx_msg void OnBnClickedScardlistreaders();
	afx_msg void OnBnClickedScardconnect();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	CString port;
	afx_msg void OnBnClickedScarddisconnect();
	afx_msg void OnBnClickedScardbegintransaction();
	afx_msg void OnBnClickedScardstatus();
	afx_msg void OnBnClickedScardtransmit();
	CEdit EControl;
	CFont m_editFont;
	afx_msg void OnBnClickedScardendtransaction();
	afx_msg void OnBnClickedClearinput();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedApdu();
	afx_msg void OnBnClickedGetuid();
	afx_msg void OnBnClickedVersion();
	afx_msg void OnBnClickedGetuid1();
	afx_msg void OnBnClickedGetuid2();
	afx_msg void OnBnClickedVison();

	void DispScreenInfo(char *value, BYTE* sendInfo, int len);
};


