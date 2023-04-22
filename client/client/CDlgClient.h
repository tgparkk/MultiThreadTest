#pragma once
#include "afxdialogex.h"


// CDlgClient 대화 상자

class CDlgClient : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgClient)

public:
	CDlgClient(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgClient();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CLIENT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edit_request;
	afx_msg void OnBnClickedOk();
};
