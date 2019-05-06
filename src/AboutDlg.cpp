/*____________________________________________________________________________

   ExifPro Image Viewer

   Copyright (C) 2000-2015 Michael Kowalski
____________________________________________________________________________*/

/////////////////////////////////////////////////////////////////////////////
// AboutDlg dialog used for App About

#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.h"
#include "CatchAll.h"
#include "PNGImage.h"
#include "UIElements.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


AboutDlg::AboutDlg() : CDialog(AboutDlg::IDD)
{
	//{{AFX_DATA_INIT(AboutDlg)
	version_ = _T("");
	//}}AFX_DATA_INIT
	scroll_pos_ = 0;
	stop_delay_ = 0;
	text_lines_ = 0;
}


AboutDlg::~AboutDlg()
{
	scroll_dc_.DeleteDC();	// delete dc before the bmp selected into it
	backgnd_dc_.DeleteDC();
}


void AboutDlg::DoDataExchange(CDataExchange* DX)
{
	CDialog::DoDataExchange(DX);
	//{{AFX_DATA_MAP(AboutDlg)
//	DDX_Text(DX, IDC_VERSION, version_);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(AboutDlg, CDialog)
	//{{AFX_MSG_MAP(AboutDlg)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// an area in a bitmap to print extra text info
namespace {
	const CPoint LEFTTOP(273, 144);
	const CSize AREA_SIZE(140, 161);
	const COLORREF TEXT_COLOR= RGB(211,221,255);
}


extern AutoPtr<Dib> LoadJpeg(UINT img_id);


extern CString ReadAppVersion(bool including_build)
{
	CString version;

	HRSRC rsrc= ::FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	HGLOBAL global;
	if (rsrc && (global = ::LoadResource(AfxGetInstanceHandle(),rsrc)) != NULL)
	{
		VS_FIXEDFILEINFO* ver= (VS_FIXEDFILEINFO*)((BYTE*)::LockResource(global) + 0x28);
		if (ver->dwSignature == 0xfeef04bd)
		{
			if (including_build)
				version.Format(_T("Version %d.%u.%u Build %u\n%s Build"),
					(int)HIWORD(ver->dwProductVersionMS), (int)LOWORD(ver->dwProductVersionMS),
					(int)HIWORD(ver->dwProductVersionLS), (int)LOWORD(ver->dwProductVersionLS),
					sizeof(TCHAR) == 1 ? _T("ASCII") : _T("Unicode"));
			else
				version.Format(_T("Version %d.%u.%u\n%s Build"),
					(int)HIWORD(ver->dwProductVersionMS), (int)LOWORD(ver->dwProductVersionMS),
					(int)HIWORD(ver->dwProductVersionLS),
					sizeof(TCHAR) == 1 ? _T("ASCII") : _T("Unicode"));
		}
		::FreeResource(global);
	}

	return version;
}


BOOL AboutDlg::OnInitDialog()
{
	try
	{
		version_ = ReadAppVersion(true);
		about_ = version_ + _T("\n\nRelease Version ")
#ifdef _WIN64
			_T("x64")
#else
			_T("x86")
#endif
			_T("\n\n");

		about_ += _T("Free software released under the terms of the GNU Public License.");

		CDialog::OnInitDialog();

		LOGFONT lf;
		lf.lfHeight = -Pixels(12);
		lf.lfWidth = 0;
		lf.lfEscapement = 0;
		lf.lfOrientation = 0;
		lf.lfWeight = FW_BOLD;
		lf.lfItalic = false;
		lf.lfUnderline = false;
		lf.lfStrikeOut = false;
		lf.lfCharSet = DEFAULT_CHARSET;
		lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
		lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
		lf.lfQuality = ANTIALIASED_QUALITY;
		lf.lfPitchAndFamily = FF_SWISS;
		_tcscpy(lf.lfFaceName, _T("Arial"));

		small_fnt_.CreateFontIndirect(&lf);

		if (!PNGImage().Load(IDR_ABOUT, dib_about_) || !dib_about_.IsValid())
			EndDialog(IDCANCEL);

		CRect rect(CPoint(0, 0), dib_about_.GetSize());
		::AdjustWindowRectEx(rect, GetStyle(), false, GetExStyle());
		SetWindowPos(0, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

		CPoint pos= CPoint(Pixels(LEFTTOP.x), Pixels(LEFTTOP.y));
		pos.y += Pixels(AREA_SIZE.cy - 12);
		link_wnd_.Create(this, pos, _T("ExifPro Web Site"), _T("http://www.exifpro.com/"), &small_fnt_);
		link_wnd_.rgb_text_color_ = TEXT_COLOR;

		libs_ =
			_T("\n")
			_T("ExifPro is based in part on the work of the Independent JPEG Group\nCopyright � 1991-1996, Thomas G. Lane\n")
			_T("TIFF library is copyright as follows:\n")
			_T("Copyright � 1988-1997 Sam Leffler\nCopyright � 1991-1997 Silicon Graphics, Inc.\n")
			_T("PNG library is copyright as follows:\n")
			_T("Copyright � 1998-2002 Glenn Randers-Pehrson\nCopyright � 1996-1997 Andreas Dilger\nCopyright � 1995-1996 Guy Eric Schalnat, Group 42, Inc.\n")
			_T("ZLib library is copyright as follows:\n")
			_T("� 1995-2002 Jean-loup Gailly and Mark Adler\n")
			_T("Boost libraries: www.boost.org\n")
			_T("CoolSB library: � J. Brown 2001\n")
			_T("LCMS library: www.littlecms.com\n")
			_T("Lua interpreter: www.lua.org\n")
			_T("XMP library: Copyright � 1999-2012, Adobe Systems Incorporated\n")
			_T("Scintilla: Copyright 1998-2003 by Neil Hodgson\n")
			_T("LibJPEG-Turbo: www.libjpeg-turbo.org\n")
			_T("\n");

		const TCHAR* b= libs_;
		const TCHAR* e= b + libs_.GetLength();
		text_lines_ = static_cast<int>(std::count(b, e, L'\n'));	// lines of text to scroll

		SetTimer(1, 25, 0);

		return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}
	CATCH_ALL

	EndDialog(IDCANCEL);

	return true;
}


BOOL AboutDlg::OnEraseBkgnd(CDC* dc)
{
	if (!dib_about_.IsValid())
		return CDialog::OnEraseBkgnd(dc);

	dib_about_.DibDraw(dc, CPoint(0, 0));

	CFont* old= dc->SelectObject(&small_fnt_);

	dc->SetTextColor(TEXT_COLOR);
	dc->SetBkMode(TRANSPARENT);
	dc->DrawText(about_, CRect(CPoint(Pixels(LEFTTOP.x), Pixels(LEFTTOP.y)), CSize(Pixels(AREA_SIZE.cx), Pixels(AREA_SIZE.cy))), DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK | DT_END_ELLIPSIS);

	dc->SelectObject(old);

	return true;
}


void AboutDlg::OnLButtonUp(UINT flags, CPoint point)
{
	EndDialog(IDCANCEL);
}


void AboutDlg::OnTimer(UINT_PTR event_id)
{
	CDialog::OnTimer(event_id);

	if (event_id != 1)
		return;

	if (stop_delay_ > 0)
	{
		--stop_delay_;
		return;
	}

	// scroll text

	CClientDC dc(this);

	CRect rect;
	GetClientRect(rect);
	rect.DeflateRect(Pixels(20), Pixels(1), Pixels(2), Pixels(1));
	rect.top = rect.bottom - Pixels(16);

	// prepare bmp to erase background behind text
	if (scroll_bmp_.m_hObject == 0)
	{
		scroll_bmp_.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		scroll_dc_.CreateCompatibleDC(&dc);
		scroll_dc_.SelectObject(&scroll_bmp_);
		backgnd_dc_.CreateCompatibleDC(&dc);
		backgnd_dc_.SelectObject(dib_about_.GetBmp());
	}

	CFont* old= scroll_dc_.SelectObject(&small_fnt_);

	scroll_dc_.SetTextColor(TEXT_COLOR);
	scroll_dc_.SetBkColor(0);
	scroll_dc_.SetBkMode(TRANSPARENT);
	scroll_dc_.BitBlt(0, 0, rect.Width(), rect.Height(), &backgnd_dc_, rect.left, rect.top, SRCCOPY);
	scroll_dc_.SetViewportOrg(0, -scroll_pos_);
	CRect bounds(0, 0, rect.Width(), rect.Height());
	scroll_dc_.DrawText(libs_, bounds, DT_LEFT | DT_BOTTOM | DT_NOPREFIX | DT_WORDBREAK | DT_NOCLIP);
	scroll_dc_.DrawText(libs_, bounds, DT_LEFT | DT_BOTTOM | DT_NOPREFIX | DT_WORDBREAK | DT_NOCLIP | DT_CALCRECT);
	scroll_dc_.SetViewportOrg(0, 0);

	scroll_dc_.SelectObject(old);

	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &scroll_dc_, 0, 0, SRCCOPY);

	int line_height = bounds.Height() / (text_lines_ + 1);
	if (line_height > 0 && scroll_pos_ % line_height == 0)		// pause after scrolling whole line
	{
		stop_delay_ = 90;
		if (scroll_pos_ / line_height == text_lines_)
			scroll_pos_ = 0;
	}

	++scroll_pos_;
}
