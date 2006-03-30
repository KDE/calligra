/* This file is part of the KDE project
   Copyright (C) 2000 Robert JACOLIN <rjacolin@ifrance.com>

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

#include "xsltimportdia.h"
#include "xsltimportdia.moc"

//#include <stdio.h>

#include <qcombobox.h>
#include <qcursor.h>
#include <qstringlist.h>
#include <qdir.h>
//Added by qt3to4:
#include <Q3CString>

#include <kapplication.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <krecentdocument.h>
#include <KoFilterManager.h>
#include <ktempfile.h>
#include <kdebug.h>

#include <xsltproc.h>

/*
 *  Constructs a XSLTImportDia which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
XSLTImportDia::XSLTImportDia(KoStore* out, const Q3CString &format, QWidget* parent,  const char* name_, bool modal, Qt::WFlags fl )
    : XSLTDialog( parent, name_, modal, fl )
{
	int i = 0;
	_out = out;
	_format = format;
	setCaption(i18n("Import XSLT Configuration"));
	
	/* Recent files */
	_config = new KConfig("xsltdialog");
	_config->setGroup( "XSLT import filter" );
	QString value;
	while(i < 10)
	{
		value = _config->readPathEntry( QString("Recent%1").arg(i) );
		kDebug() << "recent : " << value << endl;
		if(!value.isEmpty())
		{
			_recentList.append( value );
			recentBox->insertItem(value);
		}
		else
			i = 10;
		i = i + 1;
	}

	/* Common xslt files box */
	QString appName = (const char*) KGlobal::instance()->instanceName();
	kDebug() << "app name = " << appName << endl;
	
	QString filenames = QString("xsltfilter") + QDir::separator() + QString("import") +
			QDir::separator() + appName + QDir::separator() + "*/*.xsl";
	QStringList commonFilesList = KGlobal::dirs()->findAllResources("data", filenames, true);
	kDebug() << "There are " << commonFilesList.size() << " entries like  " << filenames << endl;
	
	QStringList tempList;
	QString name;
	QString file;

	for(QStringList::Iterator it = commonFilesList.begin(); it != commonFilesList.end(); ++it)
	{
		tempList = QStringList::split("/", (*it));
		file = tempList.last();
		tempList.pop_back();
		name = tempList.last();
		tempList.pop_back();
		kDebug() << name << " " << file << endl;
		if(!_namesList.contains(name) && file == "main.xsl")
		{
			_filesList.append(file);
			_namesList.append(name);
			_dirsList.append(tempList.join("/"));
			kDebug() << file << " get" << endl;
		}
	}
	
	xsltList->insertStringList(_namesList);
	kapp->restoreOverrideCursor();
}

/*
 *  Destroys the object and frees any allocated resources
 */
XSLTImportDia::~XSLTImportDia()
{
    // no need to delete child widgets, Qt does it all for us
	delete _config;
}

/**
 * Called when thecancel button is clicked.
 * Close the dialog box.
 */
void XSLTImportDia::cancelSlot()
{
	kDebug() << "import cancelled" << endl;
	reject();
}

/**
 * Called when the choose button is clicked. A file dialog is open to allow to choose
 * the xslt to use.
 * Change the value of the current file.
 */
void XSLTImportDia::chooseSlot()
{

	/* Use dir from currently selected file */
    QString dir = QString::null;
    if ( _currentFile.isLocalFile() && QFile::exists( _currentFile.path() ) )
        dir = QFileInfo( _currentFile.path() ).absoluteFilePath();

    KFileDialog *dialog = new KFileDialog(dir, QString::null, 0L);
    dialog->setCaption( i18n("Open Document") );
    dialog->setMimeFilter( KoFilterManager::mimeFilter( _format, KoFilterManager::Import ) );
	KUrl u;

    if(dialog->exec() == QDialog::Accepted)
    {
        u = dialog->selectedURL();
        KRecentDocument::add(dialog->selectedURL().url(), !dialog->selectedURL().isLocalFile());
    } else //revert state
    {
        //if (bEmpty) openEmpty();
        //if (bTemplates) openTemplate();
    }

    delete dialog;

    QString filename = u.path();
    QString url = u.url();
    bool local = u.isLocalFile();

    bool ok = !url.isEmpty();
    if (local) // additional checks for local files
        ok = ok && (QFileInfo( filename ).isFile() ||
                    (QFileInfo( filename ).isSymLink() &&
                     !QFileInfo( filename ).readLink().isEmpty() &&
                     QFileInfo( QFileInfo( filename ).readLink() ).isFile() ) );

    if ( ok )
    {
        _currentFile = u;
        okSlot();
    }
}

/**
 * Called when the user clic on an element in the recent list.
 * Change the value of the current file.
 */
void XSLTImportDia::chooseRecentSlot()
{
	kDebug() << "recent slot : " << recentBox->currentText() << endl;
	_currentFile = recentBox->currentText();
}

/**
 * Called when teh user clic on an element in the common list of xslt sheet.
 * Change the value of the current file.
 */
void XSLTImportDia::chooseCommonSlot()
{
	int num = xsltList->currentItem();
	_currentFile = QDir::separator() + _dirsList[num] + QDir::separator() +
			xsltList->currentText() + QDir::separator() + _filesList[num];
	kDebug() << "common slot : " << _currentFile.url() << endl;
}

/**
 * Called when the user clic on the ok button. The xslt sheet is put on the recent list which is
 * saved, then the xslt processor is called to export the document.
 */
void XSLTImportDia::okSlot()
{
  	kapp->setOverrideCursor(QCursor(Qt::WaitCursor));
	hide();
	if(_currentFile.url().isEmpty())
		return;
	kDebug() << "XSLT FILTER --> BEGIN" << endl;
	_out->open("root");
	QString stylesheet = _currentFile.directory() + "/" + _currentFile.fileName();

	/* Add the current file in the recent list if is not and save the list. */
	if(_recentList.contains(stylesheet) == 0)
	{
		kDebug() << "Style sheet add to recent list" << endl;
		/* Remove the older stylesheet used */
		if(_recentList.size() >= 10)
			_recentList.pop_back();

		/* Add the new */
		_recentList.prepend(stylesheet);

		/* Save the new list */
		kDebug() << "Recent list save " << _recentList.size() << " entrie(s)" << endl;
		int i = 0;
		while(_recentList.size() > 0)
		{
			kDebug() << "save : " << _recentList.first() << endl;
#if KDE_IS_VERSION(3,1,3)
			_config->writePathEntry( QString("Recent%1").arg(i), _recentList.first());
#else
			_config->writeEntry( QString("Recent%1").arg(i), _recentList.first());
#endif
			_recentList.pop_front();
			i = i + 1;
		}
		/* Write config on disk */
		_config->sync();
	}
	
	/* Create a temp file */
	KTempFile temp("xsltimport-", "kwd");
	temp.setAutoDelete(true);

	QFile* tempFile = temp.file();
	tempFile->open(QIODevice::WriteOnly);

	/* Generate the data in the temp file */
	XSLTProc* xsltproc = new XSLTProc(_fileIn, temp.name(), stylesheet);
	xsltproc->parse();

	/* Save the temp file in the store */
	tempFile->open(QIODevice::ReadOnly);
	_out->write(tempFile->readAll());
	
	delete tempFile;
	delete xsltproc;

	_out->close();
	temp.close();

	kDebug() << "XSLT FILTER --> END" << endl;
	reject();
}

