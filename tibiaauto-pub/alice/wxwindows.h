#ifndef WXWINDOWS_H
#define WXWINDOWS_H

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(ADD_TAB, 1)
DECLARE_EVENT_TYPE(UPDATE_TAB, 2)
END_DECLARE_EVENT_TYPES()

DEFINE_EVENT_TYPE(ADD_TAB)
DEFINE_EVENT_TYPE(UPDATE_TAB)


class GuiStream;

class JThread : public wxThread, public StreamProvider
{
public:
	JThread() : wxThread()
	{
	}

	Stream *getStream(const char *cs);
	void Output(const char *csn, const char *csm, bool read);

	virtual void *Entry();
private:
	map<string, GuiStream *> streams;
};

class GuiStream : public Stream
{
public:
	GuiStream()
	{
		thread = NULL;
	}

	void Read(const char *cs)
	{
		if (thread != NULL)
			((JThread *)thread)
			->Output(name.c_str(), cs, true);
	}

	void Write(const char *cs)
	{
		if (thread != NULL)
			((JThread *)thread)
			->Output(name.c_str(), cs, false);
	}

	void setThread(wxThread *t)
	{
		thread = t;
	}

	void setName(const string &s)
	{
		name = s;
	}

private:
	wxThread *thread;
	string name;
};


class JFrame : public wxFrame
{
public:
	JFrame(wxWindow *parent = NULL);

	bool loadedOk()
	{
		return init;
	}

	void addTabbedPane(wxEvent &evt);
	void appendText(wxEvent &evt);
	void onClose(wxCloseEvent &);
	void OnExit(wxCommandEvent & WXUNUSED(event));
	void OnAbout(wxCommandEvent & WXUNUSED(event));
private:
	bool init;
	/*
	        An array containing all the text controls
	        for each tab in our notebook.
	 */
	map<string, wxTextCtrl *> controls;

	DECLARE_EVENT_TABLE()
};

class NewTabEvent : public wxEvent
{
public:
	NewTabEvent(const string &name, int size)
		: wxEvent(0, ADD_TAB)
	{
		this->name = name;
		this->size = size;
	}

	virtual wxEvent *Clone() const
	{
		return new NewTabEvent(name, size);
	}

	string name;
	int size;
};

class UpdateTabEvent : public wxEvent
{
public:
	UpdateTabEvent(const string &name, const string &text, bool read)
		: wxEvent(0, UPDATE_TAB)
	{
		this->name = name;
		this->text = text;
		this->read = read;
	}

	virtual wxEvent *Clone() const
	{
		return new UpdateTabEvent(name, text, read);
	}

	string name, text;
	bool read;
};


BEGIN_EVENT_TABLE(JFrame, wxFrame)
EVT_CUSTOM(ADD_TAB, -1, JFrame::addTabbedPane)
EVT_CUSTOM(UPDATE_TAB, -1, JFrame::appendText)
EVT_CLOSE(JFrame::onClose)
EVT_MENU(XRCID("menu_quit"), JFrame::OnExit)
EVT_MENU(XRCID("menu_about"), JFrame::OnAbout)
END_EVENT_TABLE()


#endif // ifndef WXWINDOWS_H
