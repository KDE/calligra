/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include "KexiStartupFileWidget.h"
#include <kexi_global.h>

#include <kexidb/driver.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

#include <qlayout.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <QKeyEvent>
#include <QEvent>
#include <QAction>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kfile.h>
#include <kurlcombobox.h>
#include <KToolBar>
#include <KActionCollection>

//! @internal
class KexiStartupFileWidget::Private
{
	public:
		Private()
		 : confirmOverwrites(true)
		 , filtersUpdated(false)
		{
		}
	
	QString lastFileName;
	KexiStartupFileWidget::Mode mode;
	QSet<QString> additionalMimeTypes, excludedMimeTypes;
	QString defaultExtension;
	bool confirmOverwrites : 1;
	bool filtersUpdated : 1;
	KUrl highlightedUrl;
};

//------------------

KexiStartupFileWidget::KexiStartupFileWidget(
		const KUrl &startDirOrVariable, Mode mode, QWidget *parent)
	:  KFileWidget(startDirOrVariable, parent)
	, d( new Private() )
{
	setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
	setMode( mode );
	QAction *previewAction = actionCollection()->action("preview");
	if (previewAction)
		previewAction->setChecked(false);
	
	//setMinimumHeight(100);
	//setSizeGripEnabled ( false );

	//dirty hack to customize filedialog view:
/*	{
		QList<QPushButton*> l = findChildren<QPushButton*>();
		foreach (QPushButton* btn, l)
			btn->hide();
		QList<QWidget*> wl = findChildren<QWidget*>();
		foreach (QWidget* w, wl)
			w->installEventFilter(this);
	}	*/
	
/* Qt4
#ifdef Q_WS_WIN
	if (startDirOrVariable.startsWith(":"))
		m_lastVisitedPathsVariable = startDirOrVariable; //store for later use
#else*/
//	toggleSpeedbar(false);
	setFocusProxy( locationEdit() );
//#endif
	
	connect(this,SIGNAL(fileHighlighted(const QString&)),
		this,SLOT(slotExistingFileHighlighted(const QString&)));
}

KexiStartupFileWidget::~KexiStartupFileWidget()
{
	delete d;
//Qt4 #ifdef Q_WS_WIN
//	saveLastVisitedPath(currentFileName());
//#endif
}

void KexiStartupFileWidget::slotExistingFileHighlighted(const QString& fileName)
{
kexidbg << fileName << endl;
	d->highlightedUrl = KUrl(fileName);
	//updateDialogOKButton(0);
	emit fileHighlighted();
}

QString KexiStartupFileWidget::highlightedFile() const
{
	return d->highlightedUrl.path();
}

void KexiStartupFileWidget::setMode(Mode mode)
{
	//delayed
	d->mode = mode;
	d->filtersUpdated = false;
	updateFilters();
}

QSet<QString> KexiStartupFileWidget::additionalFilters() const
{
	return d->additionalMimeTypes;
}

void KexiStartupFileWidget::setAdditionalFilters(const QSet<QString> &mimeTypes)
{
	//delayed
	d->additionalMimeTypes = mimeTypes;
	d->filtersUpdated = false;
}

QSet<QString> KexiStartupFileWidget::excludedFilters() const
{
	return d->excludedMimeTypes;
}

void KexiStartupFileWidget::setExcludedFilters(const QSet<QString> &mimeTypes)
{
	//delayed
	d->excludedMimeTypes.clear();
	//convert to lowercase
	foreach (const QString& mimeType, mimeTypes)
		d->excludedMimeTypes.insert( mimeType.toLower() );
	d->filtersUpdated = false;
}

void KexiStartupFileWidget::updateFilters()
{
	if (d->filtersUpdated)
		return;
	d->filtersUpdated = true;

	d->lastFileName.clear();
//	m_lastUrl = KUrl();

	clearFilter();
	
	QString filter;
	KMimeType::Ptr mime;
	QStringList allfilters;

	const bool normalOpeningMode = d->mode & Opening && !(d->mode & Custom);
	const bool normalSavingMode = d->mode & SavingFileBasedDB && !(d->mode & Custom);

	if (normalOpeningMode || normalSavingMode) {
		mime = KMimeType::mimeType( KexiDB::Driver::defaultFileBasedDriverMimeType() );
		if (mime && !d->excludedMimeTypes.contains(mime->name().toLower())) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}
	if (normalOpeningMode || d->mode & SavingServerBasedDB) {
		mime = KMimeType::mimeType("application/x-kexiproject-shortcut");
		if (mime && !d->excludedMimeTypes.contains(mime->name().toLower())) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}
	if (normalOpeningMode || d->mode & SavingServerBasedDB) {
		mime = KMimeType::mimeType("application/x-kexi-connectiondata");
		if (mime && !d->excludedMimeTypes.contains(mime->name().toLower())) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}

//! @todo hardcoded for MSA:
	if (normalOpeningMode) {
		mime = KMimeType::mimeType("application/vnd.ms-access");
		if (mime && !d->excludedMimeTypes.contains(mime->name().toLower())) {
			filter += KexiUtils::fileDialogFilterString(mime);
			allfilters += mime->patterns();
		}
	}

	foreach (const QString& mimeName, d->additionalMimeTypes) {
		if (mimeName == "all/allfiles")
			continue;
		if (d->excludedMimeTypes.contains(mimeName.toLower()))
			continue;
		filter += KexiUtils::fileDialogFilterString(mimeName);
		mime = KMimeType::mimeType(mimeName);
		allfilters += mime->patterns();
	}

	if (!d->excludedMimeTypes.contains("all/allfiles"))
		filter += KexiUtils::fileDialogFilterString("all/allfiles");
//	mime = KMimeType::mimeType("all/allfiles");
//	if (mime) {
//		filter += QString(mime->patterns().isEmpty() ? "*" : mime->patterns().join(" ")) 
//			+ "|" + mime->comment()+ " (*)\n";
//	}
	//remove duplicates made because upper- and lower-case extenstions are used:
	QStringList allfiltersUnique = allfilters.toSet().toList();
	qSort(allfiltersUnique);
	
	if (allfiltersUnique.count()>1) {//prepend "all supoported files" entry
		filter.prepend(allfilters.join(" ")+"|"
		+ i18n("All Supported Files (%1)", allfiltersUnique.join(", "))+"\n");
	}
	
	if (filter.right(1)=="\n")
		filter.truncate(filter.length()-1);
	setFilter(filter);

	if (d->mode & Opening) {
		KFileWidget::setMode( KFile::ExistingOnly | KFile::LocalOnly | KFile::File );
		setOperationMode( KFileWidget::Opening );
	} else {
		KFileWidget::setMode( KFile::LocalOnly | KFile::File );
		setOperationMode( KFileWidget::Saving );
	}
}

void KexiStartupFileWidget::showEvent( QShowEvent * event )
{
	d->filtersUpdated = false;
	updateFilters();
	KFileWidget::showEvent(event);
}

/*TODO
QString KexiStartupFileWidget::selectedFile() const
{
//Qt4	setResult( QDialog::Accepted ); // selectedURL tests for it
	
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
	QString path( KFileWidget::selectedFile() );
	kDebug() << "prev selectedFile() == " << path <<endl;
	kDebug() << "locationEdit == " << locationEdit()->currentText().trimmed() <<endl;
	//make sure user-entered path is acceped:
#ifdef __GNUC__
#warning TODO?	setSelection( locationEdit()->currentText().trimmed() );
#else
#pragma WARNING( TODO?	setSelection( locationEdit()->currentText().trimmed() ); )
#endif
//	path = KFileWidget::selectedFile();
	path = locationEdit()->currentText().trimmed();
	kDebug() << "selectedFile() == " << path <<endl;
	
#endif
	
	if (!currentFilter().isEmpty()) {
		if (d->mode & SavingFileBasedDB) {
			const QStringList filters( currentFilter().split(" ") );
			kDebug()<< " filter == " << filters << endl;
			QString ext( QFileInfo(path).suffix() );
			bool hasExtension = false;
			foreach (const QString& filter, filters) {
				const QString f( filter.trimmed() );
				hasExtension = !f.mid(2).isEmpty() && ext==f.mid(2);
				if (hasExtension)
					break;
			}
			if (!hasExtension) {
				//no extension: add one
				QString defaultExtension( d->defaultExtension );
				if (defaultExtension.isEmpty())
					defaultExtension = filters.first().trimmed().mid(2); //first one
				path += (QString(".")+defaultExtension);
				kDebug() << "KexiStartupFileWidget::checkURL(): append extension, " << path << endl;
//Qt4				setSelection( path );
			}
		}
	}
	kDebug() << "KexiStartupFileWidget::currentFileName() == " << path <<endl;
	return path;
}
*/

bool KexiStartupFileWidget::checkSelectedFile()
{
	//accept();

//	KUrl url = currentURL();
//	QString path = url.path().trimmed();
//	QString path = selectedFile().trimmed();

	if (d->highlightedUrl.isEmpty() && !locationEdit()->urls().isEmpty()) {
		kDebug() << locationEdit()->urls() <<endl;
		kDebug() << baseUrl() <<endl;
		
		d->highlightedUrl = baseUrl();
		d->highlightedUrl.addPath( locationEdit()->urls().first() );
	}
	
	kDebug() << "d->highlightedUrl: " << d->highlightedUrl << endl;
//	if (url.fileName().trimmed().isEmpty()) {
	if (d->highlightedUrl.isEmpty()) {
		KMessageBox::error( this, i18n( "Enter a filename." ));
		return false;
	}
	
	kDebug() << "KexiStartupFileWidget::checkURL() path: " << d->highlightedUrl  << endl;
//	kDebug() << "KexiStartupFileWidget::checkURL() fname: " << url.fileName() << endl;
//todo	if ( url.isLocalFile() ) {
		QFileInfo fi(d->highlightedUrl.path());
		if (mode() & KFile::ExistingOnly) {
			if ( !fi.exists() ) {
				KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" does not exist.",
					QDir::convertSeparators(d->highlightedUrl.path()) ) );
				return false;
			}
			else if (mode() & KFile::File) {
				if (!fi.isFile()) {
					KMessageBox::error( this, "<qt>"+i18n( "Enter a filename." ) );
					return false;
				}
				else if (!fi.isReadable()) {
					KMessageBox::error( this, "<qt>"+i18n( "The file \"%1\" is not readable.",
						QDir::convertSeparators(d->highlightedUrl.path()) ) );
					return false;
				}
			}
		}
		else if (d->confirmOverwrites && !askForOverwriting( d->highlightedUrl.path(), this )) {
			return false;
		}
//	}
	return true;
}

//static 
bool KexiStartupFileWidget::askForOverwriting(const QString& filePath, QWidget *parent)
{
	QFileInfo fi(filePath);
	if (!fi.exists())
		return true;
	const int res = KMessageBox::warningYesNo( parent, 
		i18n( "The file \"%1\" already exists.\n"
		"Do you want to overwrite it?", QDir::convertSeparators(filePath) ), QString(), 
			KGuiItem(i18n("Overwrite")), KStandardGuiItem::no() );
	if (res == KMessageBox::Yes)
		return true;
	return false;
}

void KexiStartupFileWidget::accept()
{
	kexidbg << "KexiStartupFileWidget::accept()..." << endl;
	
	KFileWidget::accept();
//	kexidbg << selectedFile() << endl;
	
//	locationEdit->setFocus();
//	QKeyEvent ev(QEvent::KeyPress, Qt::Key_Enter, '\n', 0);
//	QApplication::sendEvent(locationEdit, &ev);
//	QApplication::postEvent(locationEdit, &ev);
	
//	kDebug() << "KexiStartupFileWidget::accept() m_lastUrl == " << m_lastUrl.path() << endl;
//	if (m_lastUrl.path()==currentURL().path()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
/*
	if (d->lastFileName==selectedFile()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
//		m_lastUrl=KUrl();
		d->lastFileName.clear();
		kDebug() << "d->lastFileName==selectedFile()" << endl;
#ifdef Q_WS_WIN
		return;
#endif
	}
	kDebug() << "KexiStartupFileWidget::accept(): path = " << selectedFile() << endl;
	if ( checkSelectedFile() ) {
		emit accepted();
	}
	d->lastFileName = selectedFile();

#ifdef Q_WS_WIN
	saveLastVisitedPath(d->lastFileName);
#endif*/
}

void KexiStartupFileWidget::reject()
{
	kDebug() << "KexiStartupFileWidget: reject!" << endl;
	emit rejected();
}

/*#ifndef Q_WS_WIN
KUrlComboBox *KexiStartupFileWidget::locationWidget() const
{
	return locationEdit;
}
#endif
*/

void KexiStartupFileWidget::setLocationText(const QString& fn)
{
	locationEdit()->setUrl( KUrl(fn) );
/*
#ifdef Q_WS_WIN
	//js @todo
	setSelection(fn);
#else
	setSelection(fn);
//	locationEdit->setCurrentText(fn);
//	locationEdit->lineEdit()->setEdited( true );
//	setSelection(fn);
#endif*/
}

void KexiStartupFileWidget::focusInEvent( QFocusEvent * )
{
	locationEdit()->setFocus();
}

/*bool KexiStartupFileWidget::eventFilter ( QObject * watched, QEvent * e )
{
	//filter-out ESC key
	if (e->type()==QEvent::KeyPress && static_cast<QKeyEvent*>(e)->key()==Qt::Key_Escape
	 && static_cast<QKeyEvent*>(e)->state()==Qt::NoButton) {
		static_cast<QKeyEvent*>(e)->accept();
		emit rejected();
		return true;
	}
	return KexiStartupFileWidgetBase::eventFilter(watched,e);
} */

void KexiStartupFileWidget::setDefaultExtension(const QString& ext)
{
	d->defaultExtension = ext;
}

void KexiStartupFileWidget::setConfirmOverwrites(bool set)
{
	d->confirmOverwrites = set;
}

#include "KexiStartupFileWidget.moc"
