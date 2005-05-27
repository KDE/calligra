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

#include "KexiStartupFileDialog.h"

#include <kexidb/driver.h>
#include <core/kexi.h>

#include <qlayout.h>
#include <qobjectlist.h>
#include <qpushbutton.h>
#include <qapplication.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kfile.h>
#include <kurlcombobox.h>

KexiStartupFileDialog::KexiStartupFileDialog(
		const QString& startDir, Mode mode,
		QWidget *parent, const char *name)
	:  KexiStartupFileDialogBase(startDir, "", parent, name, 0)
	, m_confirmOverwrites(true)
{
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	setMode( mode );
	
	QPoint point( 0, 0 );
	reparent( parentWidget(), point );

	if (layout())
		layout()->setMargin(0);
	setMinimumHeight(100);
	setSizeGripEnabled ( FALSE );

	//dirty hack to customize filedialog view:
	{
		QObjectList *l = queryList( "QPushButton" );
		QObjectListIt it( *l );
		QObject *obj;
		while ( (obj = it.current()) != 0 ) {
			++it;
			static_cast<QPushButton*>(obj)->hide();
		}
		delete l;
	}
	{
		QObjectList *l = queryList("QWidget");
		QObjectListIt it( *l );
		QObject *obj;
		while ( (obj = it.current()) != 0 ) {
			++it;
			static_cast<QPushButton*>(obj)->installEventFilter(this);
		}
		delete l;
	}	
	
#ifndef Q_WS_WIN
	toggleSpeedbar(false);
	setFocusProxy( locationEdit );//locationWidget() );
#endif

}
	
void KexiStartupFileDialog::setMode(KexiStartupFileDialog::Mode mode, 
	const QStringList &additionalMimeTypes)
{
	m_mode = mode;
	clearFilter();
	
	QString filter;
	KMimeType::Ptr mime;
	QStringList allfilters;
	
	if (m_mode == KexiStartupFileDialog::Opening 
		|| m_mode == KexiStartupFileDialog::SavingFileBasedDB) {
		mime = KMimeType::mimeType( KexiDB::Driver::defaultFileBasedDriverMimeType() );
		if (mime) {
			filter += fileDialogFilterString(mime);
//			filter += mime->patterns().join(" ") + "|" + mime->comment() 
//					+ " ("+mime->patterns().join(" ")+")\n";
			allfilters += mime->patterns().join(" ");
		}
	}
#ifdef KEXI_SERVER_SUPPORT
	if (m_mode == KexiStartupFileDialog::Opening 
		|| m_mode == KexiStartupFileDialog::SavingServerBasedDB) {
		mime = KMimeType::mimeType("application/x-kexiproject-shortcut");
		if (mime) {
//			filter += mime->patterns().join(" ") + "|" + mime->comment() 
//				+ " ("+mime->patterns().join(" ")+")\n";
			filter += fileDialogFilterString(mime);
//			allfilters += mime->patterns().join(" ");
		}
	}
#endif
	foreach (QStringList::ConstIterator, it, additionalMimeTypes) {
//		mime = KMimeType::mimeType(*it);
//		filter += mime->patterns().join(" ") + "|" + mime->comment() + " ("
//			+ mime->patterns().join(" ")+")\n";
		filter += fileDialogFilterString(*it);
	}

	filter += fileDialogFilterString("all/allfiles");
//	mime = KMimeType::mimeType("all/allfiles");
//	if (mime) {
//		filter += QString(mime->patterns().isEmpty() ? "*" : mime->patterns().join(" ")) 
//			+ "|" + mime->comment()+ " (*)\n";
//	}
	
	if (allfilters.count()>1) {//prepend "all supoported files" entry
		filter = allfilters.join(" ")+"|" + i18n("All Kexi Files")+" ("+allfilters.join(" ")+")\n" + filter;
	}
	
	if (filter.right(1)=="\n")
		filter.truncate(filter.length()-1);
	setFilter(filter);
	
	if (m_mode == KexiStartupFileDialog::Opening) {
		KexiStartupFileDialogBase::setMode( KFile::ExistingOnly | KFile::LocalOnly | KFile::File );
		setOperationMode( KFileDialog::Opening );
	} else {
		KexiStartupFileDialogBase::setMode( KFile::LocalOnly | KFile::File );
		setOperationMode( KFileDialog::Saving );
	}
}

void KexiStartupFileDialog::show()
{
	m_lastFileName = QString::null;
//	m_lastUrl = KURL();
	KexiStartupFileDialogBase::show();
}

//KURL KexiStartupFileDialog::currentURL()
QString KexiStartupFileDialog::currentFileName()
{
	setResult( QDialog::Accepted ); // selectedURL tests for it
	
#ifdef Q_WS_WIN
//	QString path = selectedFile();
	//js @todo
//	kdDebug() << "selectedFile() == " << path << " '" << url().fileName() << "' " << m_lineEdit->text() << endl;
	QString path = dir()->absPath();
	if (!path.endsWith("/") && !path.endsWith("\\"))
		path.append("/");
  path += m_lineEdit->text();
//	QString path = QFileInfo(selectedFile()).dirPath(true) + "/" + m_lineEdit->text();
#else
//	QString path = locationEdit->currentText().stripWhiteSpace(); //url.path().stripWhiteSpace(); that does not work, if the full path is not in the location edit !!!!!
	QString path=KexiStartupFileDialogBase::selectedURL().path();
	kdDebug() << "prev selectedURL() == " << path <<endl;
	kdDebug() << "locationEdit == " << locationEdit->currentText().stripWhiteSpace() <<endl;
	//make sure user-entered path is acceped:
	setSelection( locationEdit->currentText().stripWhiteSpace() );
	
	path=KexiStartupFileDialogBase::selectedURL().path();
	kdDebug() << "selectedURL() == " << path <<endl;
	
#endif
	
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
	kdDebug() << "KexiStartupFileDialog::currentFileName() == " << path <<endl;
	return path;
//	return KFileDialog::selectedURL();
}

//bool KexiStartupFileDialog::checkURL()
bool KexiStartupFileDialog::checkFileName()
{
//	KURL url = currentURL();
//	QString path = url.path().stripWhiteSpace();
	QString path = currentFileName().stripWhiteSpace();
	
//	if (url.fileName().stripWhiteSpace().isEmpty()) {
	if (path.isEmpty()) {
		KMessageBox::error( this, i18n( "Enter a filename" ));
		return false;
	}
	
	kdDebug() << "KexiStartupFileDialog::checkURL() path: " << path  << endl;
//	kdDebug() << "KexiStartupFileDialog::checkURL() fname: " << url.fileName() << endl;
//todo	if ( url.isLocalFile() ) {
		QFileInfo fi(path);
		if (mode() & KFile::ExistingOnly) {
			if ( !fi.exists() ) {
				KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" doesn't exist." )
					.arg( QDir::convertSeparators(path) ) );
				return false;
			}
			else if ((mode() & KFile::File) && (!fi.isFile() || !fi.isReadable())) {
				KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" is not readable." )
					.arg( QDir::convertSeparators(path) ) );
				return false;
			}
		}
		else if (m_confirmOverwrites && fi.exists()) {
			if (KMessageBox::Yes!=KMessageBox::warningYesNo( this, i18n( "The file \"%1\" already exists.\n"
			"Do you want to overwrite it?").arg( QDir::convertSeparators(path) ) )) {
				return false;
			}
		}
//	}
	return true;
}

void KexiStartupFileDialog::accept()
{
//	locationEdit->setFocus();
//	QKeyEvent ev(QEvent::KeyPress, Qt::Key_Enter, '\n', 0);
//	QApplication::sendEvent(locationEdit, &ev);
//	QApplication::postEvent(locationEdit, &ev);
	
//	kdDebug() << "KexiStartupFileDialog::accept() m_lastUrl == " << m_lastUrl.path() << endl;
//	if (m_lastUrl.path()==currentURL().path()) {//(js) to prevent more multiple kjob signals (i dont know why this is)
	if (m_lastFileName==currentFileName()) {//(js) to prevent more multiple kjob signals (i dont know why this is)
//		m_lastUrl=KURL();
		m_lastFileName=QString::null;
		kdDebug() << "m_lastFileName==currentFileName()" << endl;
#ifdef Q_WS_WIN
		return;
#endif
	}
//	kdDebug() << "KexiStartupFileDialog::accept(): url = " << currentURL().path() << " " << endl;
	kdDebug() << "KexiStartupFileDialog::accept(): path = " << currentFileName() << endl;
//	if ( checkURL() ) {
	if ( checkFileName() ) {
		emit accepted();
	}
//	else {
//		m_lastUrl = KURL();
//	}
//	m_lastUrl = currentURL();
	m_lastFileName = currentFileName();
}

void KexiStartupFileDialog::reject()
{
	kdDebug() << "KexiStartupFileDialog: reject!" << endl;
	emit rejected();
}

/*#ifndef Q_WS_WIN
KURLComboBox *KexiStartupFileDialog::locationWidget() const
{
	return locationEdit;
}
#endif
*/

void KexiStartupFileDialog::setLocationText(const QString& fn)
{
#ifdef Q_WS_WIN
	//js @todo
	setSelection(fn);
#else
	setSelection(fn);
//	locationEdit->setCurrentText(fn);
//	locationEdit->lineEdit()->setEdited( true );
//	setSelection(fn);
#endif
}

void KexiStartupFileDialog::setFocus()
{
#ifdef Q_WS_WIN
	m_lineEdit->setFocus();
#else
	locationEdit->setFocus();
#endif
}

bool KexiStartupFileDialog::eventFilter ( QObject * watched, QEvent * e )
{
	//filter-out ESC key
	if (e->type()==QEvent::KeyPress && static_cast<QKeyEvent*>(e)->key()==Qt::Key_Escape
	 && static_cast<QKeyEvent*>(e)->state()==Qt::NoButton) {
		static_cast<QKeyEvent*>(e)->accept();
		emit rejected();
		return true;
	}
	return KexiStartupFileDialogBase::eventFilter(watched,e);
} 

#include "KexiStartupFileDialog.moc"

