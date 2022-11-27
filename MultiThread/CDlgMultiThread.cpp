// CDlgMultiThread.cpp: 구현 파일
//

#include "pch.h"
#include "MultiThread.h"
#include "afxdialogex.h"
#include "CDlgMultiThread.h"


// CDlgMultiThread 대화 상자

IMPLEMENT_DYNAMIC(CDlgMultiThread, CDialogEx)

CDlgMultiThread::CDlgMultiThread(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_MULTI_THREAD, pParent)
	, m_btn_multi_sel()
{

}

CDlgMultiThread::~CDlgMultiThread()
{
}

void CDlgMultiThread::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_MULTI_SEL, m_btn_multi_sel);
}


BEGIN_MESSAGE_MAP(CDlgMultiThread, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_MULTI_SEL, &CDlgMultiThread::OnBnClickedBtnMultiSel)
END_MESSAGE_MAP()


// CDlgMultiThread 메시지 처리기


void CDlgMultiThread::OnBnClickedBtnMultiSel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CSqlConnect sc;
	//sc.Connect();

	CSqlConnect sc2;
	//sc2.Connect();

	std::thread t1(&CSqlConnect::Connect, sc);
	std::thread t2(&CSqlConnect::Connect, sc2);

	t1.join();
	t2.join();

}
