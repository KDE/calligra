/* This file is part of the KDE project
   Copyright (C) 2002 Robert JACOLIN <rjacolin@ifrance.com>

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
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include "xsltexportdia.h"
#include "xsltexportdia.moc"

#include <stdio.h>
#include <stdlib.h>

#include <qcombobox.h>
#include <kapplication.h>
#include <qstringlist.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <krecentdocument.h>
#include <koFilterManager.h>
#include <kdebug.h>
#include <xsltproc.h>

/*
 *  Constructs a XSLTExportDia which is a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
XSLTExportDia::XSLTExportDia(KoStore* in, const QCString &format, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : XSLTDialog( parent, name, modal, fl )
{
	int i = 0;
	_in = in;
	_format = format;
	setCaption(i18n("Export XSLT Configuration"));

	/* Recent files */
	_config = new KConfig("xsltdialog");
	_config->setGroup( "XSLT export filter" );
	QString value;
	while(i < 10)
	{
		value = _config->readEntry( QString("Recent%1").arg(i) );
		kdDebug() << "recent : " << value << endl;
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
	
}

/*
 *  Destroys the object and frees any allocated resources
 */
XSLTExportDia::~XSLTExportDia()
{
    //delete _in;
	delete _config;
}
/**
 * Called when thecancel button is clicked.
 * Close the dialog box.
 */
void XSLTExportDia::cancelSlot()
{
	kdDebug() << "export cancelled" << endl;
	reject();
}

/**
 * Called when the choose button is clicked. A file dialog is open to allow to choose
 * the xslt to use.
 * Change the value of the current file.
 */
void XSLTExportDia::chooseSlot()
{

	/* Use dir from currently selected file */
    QString dir = QString::null;
    if ( _currentFile.isLocalFile() && QFile::exists( _currentFile.path() ) )
        dir = QFileInfo( _currentFile.path() ).absFilePath();

    KFileDialog *dialog = new KFileDialog(dir, QString::null, 0L, "file dialog", true);
    dialog->setCaption( i18n("Open document") );
    dialog->setMimeFilter( KoFilterManager::mimeFilter( _format, KoFilterManager::Export ) );
	KURL u;

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
    if (local) // additionnal checks for local files
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
void XSLTExportDia::chooseRecentSlot()
{
	kdDebug() << "recent slot : " << recentBox->currentText() << endl;
	_currentFile = recentBox->currentText();
}

/**
 * Called when teh user clic on an element in the common list of xslt sheet.
 * Change the value of the current file.
 */
void XSLTExportDia::chooseCommonSlot()
{
	kdDebug() << "common slot : " << _commonDir << "/" << xsltList->currentText() << endl;
	_currentFile = _commonDir + "/" + xsltList->currentText();
}

/**
 * Called when the user clic on the ok button. The xslt sheet is put on the recent list which is
 * saved, then the xslt processor is called to export the document.
 */
void XSLTExportDia::okSlot()
{
	hide();
	kdDebug() << "XSLT FILTER --> BEGIN" << endl;
	_in->open("root");
	QString stylesheet = _currentFile.directory() + "/" + _currentFile.fileName();

	/* Add the current file in the recent list if is not and save the list. */
	if(_recentList.contains(stylesheet) == 0)
	{
		kdDebug() << "Style sheet add to recent list" << endl;
		/* Remove the older stylesheet used */
		if(_recentList.size() >= 10)
			_recentList.pop_back();

		/* Add the new */
		_recentList.prepend(stylesheet);

		/* Save the new list */
		kdDebug() << "Recent list save " << _recentList.size() << " entrie(s)" << endl;
		int i = 0;
		while(_recentList.size() > 0)
		{
			kdDebug() << "save : " << _recentList.first() << endl;
			_config->writeEntry( QString("Recent%1").arg(i), _recentList.first());
			_recentList.pop_front();
			i = i + 1;
		}
		/* Write config on disk */
		_config->sync();
	}

	/* Save the input file in a temp file */
	QByteArray array = _in->read(_in->size());
	// use mkstemp, not tempname
	char* temp = strdup("xsltXXXXXX");
	mkstemp(temp);
	QString tempFileName = QString(temp);
	//QString tempFileName = tempnam(NULL, "xslt");
	
	QFile* tempFile = new QFile(tempFileName);
	tempFile->open(IO_WriteOnly);
	tempFile->writeBlock(array);
	tempFile->close();
	kdDebug() << stylesheet << endl;
	XSLTProc* xsltproc = new XSLTProc(tempFileName.latin1(), _fileOut.latin1(),
							 stylesheet.latin1());
	xsltproc->parse();

	/* delete the temp file */
	QFile::remove(tempFileName);
	delete tempFile;
	delete xsltproc;
	_in->close();
	kdDebug() << "XSLT FILTER --> END" << endl;
	reject();
}

