/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiStartupFileDialog.h"

#include <qlayout.h>
#include <qobjectlist.h>
#include <qpushbutton.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kurlcombobox.h>

KexiStartupFileDialog::KexiStartupFileDialog(
		const QString& startDir, Mode mode,
		QWidget *parent, const char *name)
	:  KFileDialog(startDir, "", parent, name, 0)
{
	toggleSpeedbar(false);
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	
	setMode( mode );
	
	QPoint point( 0, 0 );
	reparent( parentWidget(), point );

	if (layout())
		layout()->setMargin(0);
	setMinimumHeight(100);
	setSizeGripEnabled ( FALSE );

	//dirty hack to customize filedialog view:
	QObjectList *l = queryList( "QPushButton" );
	QObjectListIt it( *l );
	QObject *obj;
	while ( (obj = it.current()) != 0 ) {
		++it;
		((QPushButton*)obj)->hide();
	}
	delete l;
	
#ifndef Q_WS_WIN
	setFocusProxy( locationWidget() );
#endif
}
	
void KexiStartupFileDialog::setMode(KexiStartupFileDialog::Mode mode)
{
	m_mode = mode;
	clearFilter();
	
	QString filter;
	KMimeType::Ptr mime;
	QStringList allfilters;
	
	if (m_mode == KexiStartupFileDialog::Opening 
		|| m_mode == KexiStartupFileDialog::SavingFileBasedDB) {
		mime = KMimeType::mimeType("application/x-kexiproject-sqlite");
		if (mime) {
			filter += mime->patterns().join(" ") + "|" + mime->comment() + " ("+mime->patterns().join(" ")+")\n";
			allfilters += mime->patterns().join(" ");
		}
	}
	if (m_mode == KexiStartupFileDialog::Opening 
		|| m_mode == KexiStartupFileDialog::SavingServerBasedDB) {
		mime = KMimeType::mimeType("application/x-kexiproject-shortcut");
		if (mime) {
			filter += mime->patterns().join(" ") + "|" + mime->comment() + " ("+mime->patterns().join(" ")+")\n";
			allfilters += mime->patterns().join(" ");
		}
	}
	mime = KMimeType::mimeType("all/allfiles");
	if (mime) {
		filter += (mime->patterns().isEmpty() ? "*" : mime->patterns().join(" ")) + "|" + mime->comment()+ " (*)\n";
	}
	
	if (allfilters.count()>1) {//prepend "all supoported files" entry
		filter = allfilters.join(" ")+"|" + i18n("All Kexi Files")+" ("+allfilters.join(" ")+")\n" + filter;
	}
	
	if (filter.right(1)=="\n")
		filter.truncate(filter.length()-1);
	setFilter(filter);
	
	if (m_mode == KexiStartupFileDialog::Opening) {
		KFileDialog::setMode( KFile::ExistingOnly | KFile::LocalOnly | KFile::File );
		setOperationMode( KFileDialog::Opening );
	} else {
		KFileDialog::setMode( KFile::LocalOnly | KFile::File );
		setOperationMode( KFileDialog::Saving );
	}
}

void KexiStartupFileDialog::show()
{
	m_lastUrl = KURL();
	KFileDialog::show();
}

KURL KexiStartupFileDialog::currentURL()
{
	setResult( QDialog::Accepted ); // selectedURL tests for it
	
//	KURL url = KFileDialog::selectedURL();
#ifdef Q_WS_WIN
	QString path = selectedFile();
	//js @todo
#else
	QString path = locationEdit->currentText().stripWhiteSpace(); //url.path().stripWhiteSpace();
#endif
	kdDebug() << "KFileDialog::selectedURL() == " << KFileDialog::selectedURL().path() <<endl;
	
	if (!currentFilter().isEmpty()) {
		if (m_mode == KexiStartupFileDialog::SavingFileBasedDB) {
			QString filter = QStringList::split(" ", currentFilter()).first().stripWhiteSpace();
			kdDebug()<< " filter == " << filter <<endl;
			QString ext = QFileInfo(path).extension(false);
			if ( !filter.mid(2).isEmpty() && ext!=filter.mid(2) ) {
				path+=(QString(".")+filter.mid(2));
				kdDebug() << "KexiStartupFileDialog::checkURL(): append extension, " << path << endl;
				setSelection( path );
			}
		}
	}
	kdDebug() << "KexiStartupFileDialog::currentURL() == " << path <<endl;
	return KFileDialog::selectedURL();
}

bool KexiStartupFileDialog::checkURL()
{
	KURL url = currentURL();
	QString path = url.path().stripWhiteSpace();
	
	if (url.fileName().stripWhiteSpace().isEmpty()) {
		KMessageBox::error( this, i18n( "Enter a filename" ));
		return false;
	}
	
	kdDebug() << "KexiStartupFileDialog::checkURL() path: " << path  << endl;
	kdDebug() << "KexiStartupFileDialog::checkURL() fname: " << url.fileName() << endl;
	if ( url.isLocalFile() ) {
		QFileInfo fi(path);
		if (mode() & KFile::ExistingOnly) {
			if ( !fi.exists() ) {
				KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" doesn't exist." ).arg( path ) );
				return false;
			}
			else if ((mode() & KFile::File) && (!fi.isFile() || !fi.isReadable())) {
				KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" is not readable." ).arg( path ) );
				return false;
			}
		}
		else if (fi.exists()) {
			if (KMessageBox::warningYesNo( this, i18n( "The file \"%1\" already exists.\n"
			"Do you want to overwrite it?").arg( path ) ) != KMessageBox::Yes) {
				return false;
			}
		}
	}
	return true;
}

void KexiStartupFileDialog::accept()
{
	kdDebug() << "KexiStartupFileDialog::accept() m_lastUrl == " << m_lastUrl.path() << endl;
	if (m_lastUrl.path()==currentURL().path()) {//(js) to prevent more multiple kjob signals (i dont know why this is)
		m_lastUrl=KURL();
		kdDebug() << "m_lastUrl==currentURL()" << endl;
		return;
	}
	kdDebug() << "KexiStartupFileDialog::accept(): url = " << currentURL().path() << " " << endl;
	if ( checkURL() ) {
		emit accepted();
	}
//	else {
//		m_lastUrl = KURL();
//	}
	m_lastUrl = currentURL();
}

void KexiStartupFileDialog::reject()
{
	kdDebug() << "KexiStartupFileDialog: reject!" << endl;
	emit cancelClicked();
}

#ifndef Q_WS_WIN
KURLComboBox *KexiStartupFileDialog::locationWidget() const
{
	return locationEdit;
}
#endif

void KexiStartupFileDialog::setLocationText(const QString& fn)
{
#ifdef Q_WS_WIN
	//js @todo
	setSelection(fn);
#else
	setSelection(fn);
	locationWidget()->setCurrentText(fn);
	locationWidget()->lineEdit()->setEdited( true );
#endif
}


#include "KexiStartupFileDialog.moc"

