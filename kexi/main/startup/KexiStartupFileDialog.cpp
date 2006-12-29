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

#include "KexiStartupFileDialog.h"

#include <kexidb/driver.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

#include <qlayout.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qapplication.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kfile.h>
#include <kurlcombobox.h>

KexiStartupFileDialog::KexiStartupFileDialog(
		const QString& startDirOrVariable, int mode,
		QWidget *parent, const char *name)
	:  KexiStartupFileDialogBase(startDirOrVariable, "", parent, name, 0)
	, m_confirmOverwrites(true)
	, m_filtersUpdated(false)
{
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	setMode( mode );
	
	QPoint point( 0, 0 );
	reparent( parentWidget(), point );

	if (layout())
		layout()->setMargin(0);
	setMinimumHeight(100);
	setSizeGripEnabled ( false );

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
			static_cast<QWidget*>(obj)->installEventFilter(this);
		}
		delete l;
	}	
	
#ifdef Q_WS_WIN
	if (startDirOrVariable.startsWith(":"))
		m_lastVisitedPathsVariable = startDirOrVariable; //store for later use
#else
	toggleSpeedbar(false);
	setFocusProxy( locationEdit );//locationWidget() );
#endif
}

KexiStartupFileDialog::~KexiStartupFileDialog()
{
#ifdef Q_WS_WIN
	saveLastVisitedPath(currentFileName());
#endif
}

void KexiStartupFileDialog::setMode(int mode)
{
	//delayed
	m_mode = mode;
	m_filtersUpdated = false;
}

QStringList KexiStartupFileDialog::additionalFilters() const
{
	return m_additionalMimeTypes;
}

void KexiStartupFileDialog::setAdditionalFilters(const QStringList &mimeTypes)
{
	//delayed
	m_additionalMimeTypes = mimeTypes;
	m_filtersUpdated = false;
}

QStringList KexiStartupFileDialog::excludedFilters() const
{
	return m_excludedMimeTypes;
}

void KexiStartupFileDialog::setExcludedFilters(const QStringList &mimeTypes)
{
	//delayed
	m_excludedMimeTypes = mimeTypes;
	m_filtersUpdated = false;
}

void KexiStartupFileDialog::updateFilters()
{
	if (m_filtersUpdated)
		return;
	m_filtersUpdated = true;

	m_lastFileName.clear();
//	m_lastUrl = KUrl();

	clearFilter();
	
	QString filter;
	KMimeType::Ptr mime;
	QStringList allfilters;

	const bool normalOpeningMode = m_mode & Opening && !(m_mode & Custom);
	const bool normalSavingMode = m_mode & SavingFileBasedDB && !(m_mode & Custom);

	if (normalOpeningMode || normalSavingMode) {
		mime = KMimeType::mimeType( KexiDB::Driver::defaultFileBasedDriverMimeType() );
		if (mime && m_excludedMimeTypes.find(mime->name())==m_excludedMimeTypes.end()) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}
	if (normalOpeningMode || m_mode & SavingServerBasedDB) {
		mime = KMimeType::mimeType("application/x-kexiproject-shortcut");
		if (mime && m_excludedMimeTypes.find(mime->name())==m_excludedMimeTypes.end()) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}
	if (normalOpeningMode || m_mode & SavingServerBasedDB) {
		mime = KMimeType::mimeType("application/x-kexi-connectiondata");
		if (mime && m_excludedMimeTypes.find(mime->name())==m_excludedMimeTypes.end()) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}

//! @todo hardcoded for MSA:
	if (normalOpeningMode) {
		mime = KMimeType::mimeType("application/x-msaccess");
		if (mime && m_excludedMimeTypes.find(mime->name())==m_excludedMimeTypes.end()) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}

	foreach (QStringList::ConstIterator, it, m_additionalMimeTypes) {
		if (*it == "all/allfiles")
			continue;
		if (m_excludedMimeTypes.find(*it)!=m_excludedMimeTypes.end())
			continue;
		filter += KexiUtils::fileDialogFilterString(*it);
		mime = KMimeType::mimeType(*it);
		allfilters += mime->patterns();
	}

	if (m_excludedMimeTypes.find("all/allfiles")==m_excludedMimeTypes.end())
		filter += KexiUtils::fileDialogFilterString("all/allfiles");
//	mime = KMimeType::mimeType("all/allfiles");
//	if (mime) {
//		filter += QString(mime->patterns().isEmpty() ? "*" : mime->patterns().join(" ")) 
//			+ "|" + mime->comment()+ " (*)\n";
//	}
	//remove duplicates made because upper- and lower-case extenstions are used:
	QStringList allfiltersUnique;
	Q3Dict<char> uniqueDict(499, false);
	foreach (QStringList::ConstIterator, it, allfilters) {
//		kDebug() << *it << endl;
		uniqueDict.insert(*it, (char*)1);
	}
	foreach_dict (Q3DictIterator<char>, it, uniqueDict) {
		allfiltersUnique += it.currentKey();
	}
	allfiltersUnique.sort();
	
	if (allfiltersUnique.count()>1) {//prepend "all supoported files" entry
		filter.prepend(allfilters.join(" ")+"|" + i18n("All Supported Files")
			+" ("+allfiltersUnique.join(" ")+")\n");
	}
	
	if (filter.right(1)=="\n")
		filter.truncate(filter.length()-1);
	setFilter(filter);
	
	if (m_mode & Opening) {
		KexiStartupFileDialogBase::setMode( KFile::ExistingOnly | KFile::LocalOnly | KFile::File );
		setOperationMode( KFileDialog::Opening );
	} else {
		KexiStartupFileDialogBase::setMode( KFile::LocalOnly | KFile::File );
		setOperationMode( KFileDialog::Saving );
	}
}

void KexiStartupFileDialog::show()
{
	m_filtersUpdated = false;
	updateFilters();
	KexiStartupFileDialogBase::show();
}

//KUrl KexiStartupFileDialog::currentURL()
QString KexiStartupFileDialog::currentFileName()
{
	setResult( QDialog::Accepted ); // selectedURL tests for it
	
#ifdef Q_WS_WIN
//	QString path = selectedFile();
	//js @todo
//	kDebug() << "selectedFile() == " << path << " '" << url().fileName() << "' " << m_lineEdit->text() << endl;
	QString path = dir()->absolutePath();
	if (!path.endsWith("/") && !path.endsWith("\\"))
		path.append("/");
  path += m_lineEdit->text();
//	QString path = QFileInfo(selectedFile()).dirPath(true) + "/" + m_lineEdit->text();
#else
//	QString path = locationEdit->currentText().trimmed(); //url.path().trimmed(); that does not work, if the full path is not in the location edit !!!!!
	QString path=KexiStartupFileDialogBase::selectedURL().path();
	kDebug() << "prev selectedURL() == " << path <<endl;
	kDebug() << "locationEdit == " << locationEdit->currentText().trimmed() <<endl;
	//make sure user-entered path is acceped:
	setSelection( locationEdit->currentText().trimmed() );
	
	path=KexiStartupFileDialogBase::selectedURL().path();
	kDebug() << "selectedURL() == " << path <<endl;
	
#endif
	
	if (!currentFilter().isEmpty()) {
		if (m_mode & SavingFileBasedDB) {
			QStringList filters = QStringList::split(" ", currentFilter()); //.first().trimmed();
			kDebug()<< " filter == " << filters << endl;
			QString ext = QFileInfo(path).extension(false);
			bool hasExtension = false;
			for (QStringList::ConstIterator filterIt = filters.constBegin(); 
				filterIt != filters.constEnd() && !hasExtension; ++filterIt)
			{
				QString f( (*filterIt).trimmed() );
				hasExtension = !f.mid(2).isEmpty() && ext==f.mid(2);
			}
			if (!hasExtension) {
				//no extension: add one
				QString defaultExtension( m_defaultExtension );
				if (defaultExtension.isEmpty())
					defaultExtension = filters.first().trimmed().mid(2); //first one
				path+=(QString(".")+defaultExtension);
				kDebug() << "KexiStartupFileDialog::checkURL(): append extension, " << path << endl;
				setSelection( path );
			}
		}
	}
	kDebug() << "KexiStartupFileDialog::currentFileName() == " << path <<endl;
	return path;
//	return KFileDialog::selectedURL();
}

//bool KexiStartupFileDialog::checkURL()
bool KexiStartupFileDialog::checkFileName()
{
//	KUrl url = currentURL();
//	QString path = url.path().trimmed();
	QString path = currentFileName().trimmed();
	
//	if (url.fileName().trimmed().isEmpty()) {
	if (path.isEmpty()) {
		KMessageBox::error( this, i18n( "Enter a filename." ));
		return false;
	}
	
	kDebug() << "KexiStartupFileDialog::checkURL() path: " << path  << endl;
//	kDebug() << "KexiStartupFileDialog::checkURL() fname: " << url.fileName() << endl;
//todo	if ( url.isLocalFile() ) {
		QFileInfo fi(path);
		if (mode() & KFile::ExistingOnly) {
			if ( !fi.exists() ) {
				KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" does not exist." )
					.arg( QDir::convertSeparators(path) ) );
				return false;
			}
			else if (mode() & KFile::File) {
				if (!fi.isFile()) {
					KMessageBox::error( this, "<qt>"+i18n( "Enter a filename." ) );
					return false;
				}
				else if (!fi.isReadable()) {
					KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" is not readable." )
						.arg( QDir::convertSeparators(path) ) );
					return false;
				}
			}
		}
		else if (m_confirmOverwrites && fi.exists()) {
			if (KMessageBox::Yes!=KMessageBox::warningYesNo( this, i18n( "The file \"%1\" already exists.\n"
			"Do you want to overwrite it?").arg( QDir::convertSeparators(path) ), QString::null, i18n("Overwrite"), KStdGuiItem::cancel() )) {
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
	
//	kDebug() << "KexiStartupFileDialog::accept() m_lastUrl == " << m_lastUrl.path() << endl;
//	if (m_lastUrl.path()==currentURL().path()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
	if (m_lastFileName==currentFileName()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
//		m_lastUrl=KUrl();
		m_lastFileName.clear();
		kDebug() << "m_lastFileName==currentFileName()" << endl;
#ifdef Q_WS_WIN
		return;
#endif
	}
//	kDebug() << "KexiStartupFileDialog::accept(): url = " << currentURL().path() << " " << endl;
	kDebug() << "KexiStartupFileDialog::accept(): path = " << currentFileName() << endl;
//	if ( checkURL() ) {
	if ( checkFileName() ) {
		emit accepted();
	}
//	else {
//		m_lastUrl = KUrl();
//	}
//	m_lastUrl = currentURL();
	m_lastFileName = currentFileName();

#ifdef Q_WS_WIN
	saveLastVisitedPath(m_lastFileName);
#endif
}

void KexiStartupFileDialog::reject()
{
	kDebug() << "KexiStartupFileDialog: reject!" << endl;
	emit rejected();
}

/*#ifndef Q_WS_WIN
KUrlComboBox *KexiStartupFileDialog::locationWidget() const
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

