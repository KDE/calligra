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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
   This file use code from koTemplateOpenDia for the method chooseSlot.
*/

#include "xsltexportdia.h"
#include "xsltexportdia.moc"

#include <stdio.h>
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
XSLTExportDia::XSLTExportDia(const KoStore& in, const QCString &format, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : XSLTDialog( parent, name, modal, fl )
{
	_in = new KoStore(in);
	_format = format;
	setCaption(i18n("Export XSLT Configuration"));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
XSLTExportDia::~XSLTExportDia()
{
    // no need to delete child widgets, Qt does it all for us
}

void XSLTExportDia::cancelSlot()
{
	kdDebug() << "export cancelled" << endl;
	reject();
}

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

void XSLTExportDia::okSlot()
{
	hide();
	kdDebug() << "XSLT FILTER --> BEGIN" << endl;
	_in->open("root");
	QString stylesheet = _currentFile.directory() + "/" + _currentFile.filename();

	/* Save the input file in a temp file */
	QByteArray array = _in->read(_in->size());
	QString tempFileName = tempnam(NULL, "xslt");
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

