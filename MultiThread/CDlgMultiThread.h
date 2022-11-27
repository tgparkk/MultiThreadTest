#pragma once
#include "afxdialogex.h"

#include "CSqlConnect.h"
#include <thread>

// CDlgMultiThread 대화 상자

class CDlgMultiThread : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMultiThread)

public:
	CDlgMultiThread(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMultiThread();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MULTI_THREAD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
private:
	CButton m_btn_multi_sel;
public:
	afx_msg void OnBnClickedBtnMultiSel();
};
