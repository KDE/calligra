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
*/
#include "xsltimportdia.h"
#include "xsltimportdia.moc"

#include <stdio.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <krecentdocument.h>
#include <koFilterManager.h>
#include <kdebug.h>
#include <xsltproc.h>

/* 
 *  Constructs a XSLTImportDia which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
XSLTImportDia::XSLTImportDia(const KoStore& out, const QCString &format, QWidget* parent,  const char* name, bool modal, WFlags fl )
    : XSLTDialog( parent, name, modal, fl )
{
	_out = new KoStore(out);
	_format = format;
	kdDebug() << "creation" << endl;
	setCaption(i18n("Import XSLT Configuration"));
}

/*  
 *  Destroys the object and frees any allocated resources
 */
XSLTImportDia::~XSLTImportDia()
{
    // no need to delete child widgets, Qt does it all for us
}

void XSLTImportDia::cancelSlot()
{
	kdDebug() << "import cancelled" << endl;
	reject();
}

void XSLTImportDia::chooseSlot()
{
	
	/* Use dir from currently selected file */
    QString dir = QString::null;
    if ( _currentFile.isLocalFile() && QFile::exists( _currentFile.path() ) )
        dir = QFileInfo( _currentFile.path() ).absFilePath();

    KFileDialog *dialog = new KFileDialog(dir, QString::null, 0L, "file dialog", true);
    dialog->setCaption( i18n("Open document") );
    dialog->setMimeFilter( KoFilterManager::mimeFilter( _format, KoFilterManager::Import ) );
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

void XSLTImportDia::okSlot()
{
	hide();
	kdDebug() << "XSLT FILTER --> BEGIN" << endl;
	_out->open("root");
	QString stylesheet = _currentFile.directory() + "/" + _currentFile.filename();

	/* Create a temp file */
	QString tempFileName = tempnam(NULL, "xslt");
	QFile* tempFile = new QFile(tempFileName);
	tempFile->open(IO_WriteOnly);
	tempFile->close();

	/* Generate the data in the temp file */
	XSLTProc* xsltproc = new XSLTProc(_fileIn.latin1(), tempFileName.latin1(),
							 stylesheet.latin1());
	xsltproc->parse();

	/* Save the temp file in the store */
	tempFile->open(IO_ReadOnly);
	_out->write(tempFile->readAll());
	tempFile->close();
	_out->close();

	/* delete the temp file */
	QFile::remove(tempFileName);
	delete tempFile;
	delete xsltproc;

	kdDebug() << "XSLT FILTER --> END" << endl;
	reject();
}

