/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KexiStartupFileDialog_h_
#define _KexiStartupFileDialog_h_

#include <kfiledialog.h>

#ifdef Q_WS_WIN
# include "KexiStartupFileDialogBase_win.h"
#else
  typedef KFileDialog KexiStartupFileDialogBase;
#endif


//! Widget for Kexi files opening/saving
class KexiStartupFileDialog : public KexiStartupFileDialogBase
{
	Q_OBJECT
	
public:

	/*! Dialog mode:
	 - Opening opens existing database (or shortcut)
	 - SavingFileBasedDB saves file-based database file
	 - SavingServerBasedDB saves server-based (shortcut) file
	*/
	typedef enum Mode { Opening = 1, SavingFileBasedDB = 2, SavingServerBasedDB = 3 };
	
	KexiStartupFileDialog(
		const QString& startDir, Mode mode,
		QWidget *parent=0, const char *name=0);
		
	void setMode(KexiStartupFileDialog::Mode mode);
	
//	KURL currentURL();
	QString currentFileName();

//#ifndef Q_WS_WIN
//	KURLComboBox *locationWidget() const;
//#endif
	//! just sets locationWidget()->setCurrentText(fn)
	//! (and something similar on win32)
	void setLocationText(const QString& fn);
	
	/*! \return true if the current URL meets requiec constraints (eg. exists);
	 shows appropriate msg box if not */
	bool checkFileName();
//	bool checkURL();

	/*! If true, user will be asked to accept overwriting existing file. 
	 This is true by default. */
	void setConfirmOverwrites(bool set) { m_confirmOverwrites = set; }

public slots:
	virtual void show();

	virtual void setFocus();

signals:
	//entered file name is accepted
	void accepted();
	void rejected();
	
protected:
	// Typing a file that doesn't exist closes the file dialog, we have to
	// handle this case better here.
	virtual void accept();
	virtual void reject();

private:
//	KURL m_lastUrl;
	QString m_lastFileName;
	Mode m_mode;
	bool m_confirmOverwrites : 1;
};

#endif

