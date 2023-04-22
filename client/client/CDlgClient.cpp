// CDlgClient.cpp: 구현 파일
//

#include "pch.h"
#include "client.h"
#include "afxdialogex.h"
#include "CDlgClient.h"

#include "ASyncTCPClient.h"

// CDlgClient 대화 상자

IMPLEMENT_DYNAMIC(CDlgClient, CDialogEx)

CDlgClient::CDlgClient(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_CLIENT, pParent)
{

}

CDlgClient::~CDlgClient()
{
}

void CDlgClient::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_REQUEST, m_edit_request);
}


BEGIN_MESSAGE_MAP(CDlgClient, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgClient::OnBnClickedOk)
END_MESSAGE_MAP()


void handler(unsigned int request_id,
	const std::string& response,
	const boost::system::error_code& ec)
{
	if (!ec.failed()) {
		std::cout << "Request #" << request_id
			<< " has completed. Response: "
			<< response << std::endl;
	}
	else if (ec == boost::asio::error::operation_aborted) {
		std::cout << "Request #" << request_id
			<< " has been cancelled by the user."
			<< std::endl;
	}
	else {
		std::cout << "Request #" << request_id
			<< " failed! Error code = " << ec.value()
			<< ". Error message = " << ec.message()
			<< std::endl;
	}
	return;
}



// CDlgClient 메시지 처리기

void CDlgClient::OnBnClickedOk()
{
	std::thread t1;

	try {
		ASyncTCPClient client;
		// Here we emulate the user's behavior.
		// User initiates a request with id 1.
		client.emulateLongComputationOp(10, "112.175.61.171", 4000, handler, 1);

		//client.emulateLongComputationOp(10, "127.0.0.1", 3333, handler, 1);

		/*
		// Then does nothing for 5 seconds.
		std::this_thread::sleep_for(std::chrono::seconds(5));
		// Then initiates another request with id 2.
		client.emulateLongComputationOp(11, "127.0.0.1", 3334,
			handler, 2);
		// Then decides to cancel the request with id 1.
		client.cancelRequest(1);
		// Does nothing for another 6 seconds.
		std::this_thread::sleep_for(std::chrono::seconds(6));
		// Initiates one more request assigning ID3 to it.
		client.emulateLongComputationOp(12, "127.0.0.1", 3335,
			handler, 3);
		// Does nothing for another 15 seconds.
		std::this_thread::sleep_for(std::chrono::seconds(15));
		*/

		// Decides to exit the application.
		client.close();
	}
	catch (boost::system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();
		//return e.code().value();

		std::cout << e.code().value() << std::endl;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
