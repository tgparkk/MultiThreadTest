
// MultiThreadView.cpp : implementation of the CMultiThreadView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MultiThread.h"
#endif

#include "MultiThreadDoc.h"
#include "MultiThreadView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMultiThreadView

IMPLEMENT_DYNCREATE(CMultiThreadView, CView)

BEGIN_MESSAGE_MAP(CMultiThreadView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMultiThreadView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMultiThreadView construction/destruction

CMultiThreadView::CMultiThreadView() noexcept
{
	// TODO: add construction code here

}

CMultiThreadView::~CMultiThreadView()
{
}

BOOL CMultiThreadView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMultiThreadView drawing

void CMultiThreadView::OnDraw(CDC* /*pDC*/)
{
	CMultiThreadDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}


// CMultiThreadView printing


void CMultiThreadView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMultiThreadView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMultiThreadView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMultiThreadView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMultiThreadView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMultiThreadView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMultiThreadView diagnostics

#ifdef _DEBUG
void CMultiThreadView::AssertValid() const
{
	CView::AssertValid();
}

void CMultiThreadView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMultiThreadDoc* CMultiThreadView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMultiThreadDoc)));
	return (CMultiThreadDoc*)m_pDocument;
}
#endif //_DEBUG


// CMultiThreadView message handlers
