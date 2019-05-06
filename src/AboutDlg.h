/*____________________________________________________________________________

   ExifPro Image Viewer

   Copyright (C) 2000-2015 Michael Kowalski
____________________________________________________________________________*/

/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog used for App About

#include "Dib.h"
#include "LinkWnd.h"


class AboutDlg : public CDialog
{
public:
	AboutDlg();
	~AboutDlg();

// Dialog Data
	//{{AFX_DATA(AboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	version_;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* DX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(AboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* dc);
	afx_msg void OnLButtonUp(UINT flags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnTimer(UINT_PTR event_id);

	//AutoPtr<Dib> dib_about_;
	Dib dib_about_;
	CFont small_fnt_;
	CString about_;
	CString libs_;
	CLinkWnd link_wnd_;
	int scroll_pos_;
	int stop_delay_;
	int text_lines_;
	CBitmap scroll_bmp_;
	CDC scroll_dc_;
	CDC backgnd_dc_;
};
