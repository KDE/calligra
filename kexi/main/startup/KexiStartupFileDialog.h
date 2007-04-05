/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef _KexiStartupFileDialog_h_
#define _KexiStartupFileDialog_h_

#include <kfiledialog.h>

#ifdef Q_WS_WIN
# include "KexiStartupFileDialogBase_win.h"
#else
  typedef KFileDialog KexiStartupFileDialogBase;
#endif


//! @short Widget for opening/saving files supported by Kexi
class KEXIMAIN_EXPORT KexiStartupFileDialog : public KexiStartupFileDialogBase
{
	Q_OBJECT
	
public:
	/*! Dialog mode:
	 - Opening opens existing database (or shortcut)
	 - SavingFileBasedDB saves file-based database file
	 - SavingServerBasedDB saves server-based (shortcut) file
	 - CustomOpening can be used for opening other files, like CSV
	*/
	typedef enum Mode { 
		Opening = 1,
		SavingFileBasedDB = 2,
		SavingServerBasedDB = 4,
		Custom = 256
	};
	
	KexiStartupFileDialog(
		const QString& startDirOrVariable, int mode, QWidget *parent=0, const char *name=0);

	virtual ~KexiStartupFileDialog();

	/*! Helper. Displays "The file %1 already exists. Do you want to overwrite it?" yes/no message box.
	 \a parent is used as a parent of the KMessageBox.
	 \return true if \a filePath file does not exists or user has agreed on overwriting,
	 false in user do not want to overwrite. */
	static bool askForOverwriting(const QString& filePath, QWidget *parent = 0);

	void setMode(int mode);

	QStringList additionalFilters() const;

	//! Sets additional filters list, e.g. "text/x-csv"
	void setAdditionalFilters(const QStringList &mimeTypes);

	QStringList excludedFilters() const;

	//! Excludes filters list
	void setExcludedFilters(const QStringList &mimeTypes);

//	KURL currentURL();
	QString currentFileName();

//#ifndef Q_WS_WIN
//	KURLComboBox *locationWidget() const;
//#endif
	//! just sets locationWidget()->setCurrentText(fn)
	//! (and something similar on win32)
	void setLocationText(const QString& fn);

	//! Sets default extension which will be added after accepting
	//! if user didn't provided one. This method is usable when there is 
	//! more than one filter so there is no rule what extension should be selected
	//! (by default first one is selected).
	void setDefaultExtension(const QString& ext) { m_defaultExtension = ext; }
	
	/*! \return true if the current URL meets requies constraints 
	 (i.e. exists or doesn't exist);
	 shows appropriate message box if needed. */
	bool checkFileName();
//	bool checkURL();

	/*! If true, user will be asked to accept overwriting existing file. 
	 This is true by default. */
	void setConfirmOverwrites(bool set) { m_confirmOverwrites = set; }

	virtual bool eventFilter ( QObject * watched, QEvent * e );

public slots:
	virtual void show();

	virtual void setFocus();

	// Typing a file that doesn't exist closes the file dialog, we have to
	// handle this case better here.
	virtual void accept();

signals:
	//entered file name is accepted
	void accepted();
	void rejected();
	
protected slots:
	virtual void reject();

private:
	void updateFilters();

//	KURL m_lastUrl;
	QString m_lastFileName;
	int m_mode;
	QStringList m_additionalMimeTypes, m_excludedMimeTypes;
	QString m_defaultExtension;
	bool m_confirmOverwrites : 1;
	bool m_filtersUpdated : 1;
};

#endif

