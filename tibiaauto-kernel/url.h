#pragma once

class CURL
{
private:
	// The default browser
	CString m_strBrowser;
public:
	void Open(LPCTSTR lpszURL, bool bNewWindow = true)
	{
		if (bNewWindow)
			::ShellExecute(NULL, NULL, GetBrowser(), lpszURL, NULL, SW_SHOWNORMAL);
		else
			::ShellExecute(NULL, NULL, lpszURL, NULL, NULL, SW_SHOWNORMAL);
	}

	LPCTSTR GetBrowser(void)
	{
		// Do we have the default browser yet?
		if (m_strBrowser.IsEmpty())
		{
			// Get the default browser from HKCR\http\shell\open\command
			HKEY hKey = NULL;
			// Open the registry
			if (::RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("http\\shell\\open\\command"), 0,
			                   KEY_READ, &hKey) == ERROR_SUCCESS)
			{
				// Data size
				DWORD cbData = 0;
				// Get the default value
				if (::RegQueryValueEx(hKey, NULL, NULL, NULL, NULL, &cbData) == ERROR_SUCCESS && cbData > 0)
				{
					// Allocate a suitable buffer
					TCHAR* psz = new TCHAR [cbData];
					// Success?
					if (psz != NULL)
					{
						if (::RegQueryValueEx(hKey, NULL, NULL,
						                      NULL, (LPBYTE)psz, &cbData) ==
						    ERROR_SUCCESS)
							// Success!
							m_strBrowser = psz;
						delete [] psz;
					}
				}
				::RegCloseKey(hKey);
			}
			// Do we have the browser?
			if (m_strBrowser.GetLength() > 0)
			{
				// Strip the full path from the string
				int nStart = m_strBrowser.Find('"');
				int nEnd   = m_strBrowser.ReverseFind('"');
				// Do we have either quote?
				// If so, then the path contains spaces
				if (nStart >= 0 && nEnd >= 0)
				{
					// Are they the same?
					if (nStart != nEnd)
						// Get the full path
						m_strBrowser = m_strBrowser.Mid(nStart + 1, nEnd - nStart - 1);
				}
				else
				{
					// We may have a pathname with spaces but
					// no quotes (Netscape), e.g.:
					//   C:\PROGRAM FILES\NETSCAPE\COMMUNICATOR\PROGRAM\NETSCAPE.EXE -h "%1"
					// Look for the last backslash
					int nIndex = m_strBrowser.ReverseFind('\\');
					// Success?
					if (nIndex > 0)
					{
						// Look for the next space after the final
						// backslash
						int nSpace = m_strBrowser.Find(' ', nIndex);
						// Do we have a space?
						if (nSpace > 0)
							m_strBrowser = m_strBrowser.Left(nSpace);
					}
				}
			}
		}
		// Done
		return m_strBrowser;
	}
};
