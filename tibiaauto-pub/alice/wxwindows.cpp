#include "stdafx.h"

#include "Stream.h"

#ifdef GUI

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif // ifdef __BORLANDC__

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif // ifndef WX_PRECOMP

#include "wx/notebook.h"
#include "wx/thread.h"
#include "wx/textctrl.h"
#include "wx/image.h"
#include "wx/splash.h"
#include "wx/xrc/xmlres.h"

#include "Main.h"

#include "wxwindows.h"

#include <iostream>

using namespace std;

#if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)
#include "j_alice_xpm.xpm"
#endif // if defined(__WXGTK__) || defined(__WXX11__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__)

static JFrame *mainFrame = NULL;

JFrame::JFrame(wxWindow *parent) {
	init = wxXmlResource::Get()->LoadFrame(this, parent, wxT("main_frame"));

	SetIcon(wxICON(j_alice));
}

/*
        Two custom events:
                - For new tabbed pane
                - For updating text of existing tabbed pane
 */


/*
        Adds a new tab to the notebook.
 */
void JFrame::addTabbedPane(wxEvent &evt) {
	//--	get the notebook control
	wxNotebook *notebook = XRCCTRL(*this, "notebook", wxNotebook);

	NewTabEvent *nte = (NewTabEvent *)(&evt);

	if (nte->size == 1)
		notebook->DeletePage(0);

	wxPanel *panel       = new wxPanel(notebook);
	wxTextCtrl *textctrl = new wxTextCtrl(panel, -1, "", wxPoint(180, 10), wxSize(240, 70), wxTE_MULTILINE | wxTE_RICH);
	wxFont font(8, wxMODERN, wxNORMAL, wxNORMAL);
	textctrl->SetDefaultStyle(wxTextAttr(wxNullColour, wxNullColour, font));
	controls[nte->name] = textctrl;
	wxBoxSizer *boxsizer = new wxBoxSizer(wxVERTICAL);
	boxsizer->Add(textctrl, 1, wxEXPAND);
	panel->SetSizer(boxsizer);
	notebook->AddPage(panel, nte->name.c_str());
}

/*
        Adds a line of text to the contents of
        a tab in the notebook.
 */
void JFrame::appendText(wxEvent &evt) {
	UpdateTabEvent *ute = (UpdateTabEvent *)(&evt);
	const string &name  = ute->name;
	const string &text  = ute->text;
	const bool &read    = ute->read;

	wxTextCtrl *tc = controls[name];

	wxTextAttr attr = tc->GetDefaultStyle();

	if (!read)
		tc->SetDefaultStyle(wxTextAttr(*wxRED));
	tc->AppendText(_T(text.c_str()));
	if (!read)
		tc->SetDefaultStyle(attr);
}

void JFrame::onClose(wxCloseEvent &) {
	cleanup_jalice();
	Destroy();
}

void JFrame::OnExit(wxCommandEvent& WXUNUSED(event)) {
	Close();
}

void JFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
//	wxDialog *f = new wxDialog();
//
//	wxXmlResource::Get()->LoadDialog(f, this, wxT("about_dialog"));
//	f->Show(true);
	wxBitmap bitmap;
	if (bitmap.LoadFile("resdata/about_box.bmp", wxBITMAP_TYPE_BMP))
	{
		wxSplashScreen* splash = new wxSplashScreen(bitmap,
		                                            wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_NO_TIMEOUT,
		                                            6000, mainFrame, -1, wxDefaultPosition, wxDefaultSize,
		                                            wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP);
	}
}

Stream *JThread::getStream(const char *cs) {
	const string name(cs);
	map<string, GuiStream *>::iterator itr = streams.find(name);

	if (itr == streams.end())
	{
		GuiStream *stream = new GuiStream();
		stream->setThread(this);
		stream->setName(name);

		streams[name] = stream;

		//--	post event to create a new tabbed pane for the notebook
		//--	will require the name
		NewTabEvent nte(name, streams.size());
		mainFrame/*->GetEventHandler()*/
		->AddPendingEvent(nte);
	}

	return streams[name];
}

void JThread::Output(const char *csn, const char *csm, bool read) {
	string name(csn);
	string text(csm);

	//--	send an event, along with name of pane & text to output
	UpdateTabEvent ute(name, text, read);
	mainFrame/*->GetEventHandler()*/
	->AddPendingEvent(ute);
}

void *JThread::Entry() {
	//register self as the *offical* output displayer :)..as opposed to console
	setStreamProvider(this);
	//run the core
	init_jalice();
	run_jalice();
	getStream("Console")->Write("All Services have terminated. Closing in 5 seconds...");
	Sleep(5000);
	mainFrame->Close();
	return NULL;
}

class Main : public wxApp
{
public:
	virtual bool OnInit();
private:
	JThread *jThread;
};

IMPLEMENT_APP(Main)

bool Main::OnInit() {
	wxImage::AddHandler(new wxGIFHandler);
	wxXmlResource::Get()->InitAllHandlers();
	wxXmlResource::Get()->Load(wxT("resdata/j-alice.xrc"));

	mainFrame = new JFrame();

	if (mainFrame->loadedOk())
	{
		mainFrame->Show(TRUE);
		SetTopWindow(mainFrame);

		jThread = new JThread();

		if (jThread->Create() != wxTHREAD_NO_ERROR)
			wxLogError(wxT("Can't create thread"));

		jThread->Run();

		return true;
	}
	else
	{
		return false;
	}
}

#endif // ifdef GUI
