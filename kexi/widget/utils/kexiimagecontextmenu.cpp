/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexiimagecontextmenu.h"

#include <kactioncollection.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kdebug.h>
#include <kmessagebox.h>

#include <qfiledialog.h>
#include <qapplication.h>

#ifdef Q_WS_WIN
#include <win32_utils.h>
#include <krecentdirs.h>
#endif

//! @internal
class KexiImageContextMenu::Private
{
public:
	Private(QWidget *parent)
	 : actionCollection(parent)
	{
	}

	KActionCollection actionCollection;
	KAction *insertFromFileAction, *saveAsAction, *cutAction, *copyAction, *pasteAction,
		*deleteAction
#ifdef KEXI_NO_UNFINISHED 
		, *propertiesAction
#endif
		;
};

//------------

KexiImageContextMenu::KexiImageContextMenu(QWidget* parent)
 : KMenu(parent)
 , d( new Private(this) )
{
	setName("KexiImageContextMenu");
	insertTitle(QString::null);

	d->insertFromFileAction = new KAction(i18n("Insert From &File..."), SmallIconSet("fileopen"), 0,
			this, SLOT(insertFromFile()), &d->actionCollection, "insert");
	d->insertFromFileAction->plug(this);
	d->saveAsAction = KStdAction::saveAs(this, SLOT(saveAs()), &d->actionCollection);
//	d->saveAsAction->setText(i18n("&Save &As..."));
	d->saveAsAction->plug(this);
	insertSeparator();
	d->cutAction = KStdAction::cut(this, SLOT(cut()), &d->actionCollection);
	d->cutAction->plug(this);
	d->copyAction = KStdAction::copy(this, SLOT(copy()), &d->actionCollection);
	d->copyAction->plug(this);
	d->pasteAction = KStdAction::paste(this, SLOT(paste()), &d->actionCollection);
	d->pasteAction->plug(this);
	d->deleteAction = new KAction(i18n("&Clear"), SmallIconSet("editdelete"), 0,
		this, SLOT(clear()), &d->actionCollection, "delete");
	d->deleteAction->plug(this);
#ifdef KEXI_NO_UNFINISHED 
	d->propertiesAction = 0;
#else
	insertSeparator();
	d->propertiesAction = new KAction(i18n("Properties"), 0, 0,
		this, SLOT(showProperties()), &d->actionCollection, "properties");
	d->propertiesAction->plug(this);
#endif
	connect(this, SIGNAL(aboutToShow()), this, SLOT(updateActionsAvailability()));
}

KexiImageContextMenu::~KexiImageContextMenu()
{
	delete d;
}

void KexiImageContextMenu::insertFromFile()
{
	QWidget *focusWidget = qApp->focusWidget();
#ifdef Q_WS_WIN
	QString recentDir;
	QString fileName = QFileDialog::getOpenFileName(
		KFileDialog::getStartURL(":LastVisitedImagePath", recentDir).path(), 
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Reading)), 
		this, 0, i18n("Insert Image From File"));
	KURL url;
	if (!fileName.isEmpty())
		url.setPath( fileName );
#else
	KURL url( KFileDialog::getImageOpenURL(
		":LastVisitedImagePath", this, i18n("Insert Image From File")) );
//	QString fileName = url.isLocalFile() ? url.path() : url.prettyURL();

	//! @todo download the file if remote, then set fileName properly
#endif
	if (!url.isValid()) {
		//focus the app again because to avoid annoying the user with unfocesed main window
		if (focusWidget) {
			focusWidget->raise();
			focusWidget->setFocus();
		}
		return;
	}
	kexipluginsdbg << "fname=" << url.prettyURL() << endl;

#ifdef Q_WS_WIN
	//save last visited path
//	KURL url(fileName);
	if (url.isLocalFile())
		KRecentDirs::add(":LastVisitedImagePath", url.directory());
#endif

	emit insertFromFileRequested(url);
	if (focusWidget) {
		focusWidget->raise();
		focusWidget->setFocus();
// todo: fix
	}
}

void KexiImageContextMenu::saveAs()
{
	QString origFilename, fileExtension;
	bool dataIsEmpty = false;
	emit aboutToSaveAsRequested(origFilename, fileExtension, dataIsEmpty);

	if (dataIsEmpty) {
		kWarning() << "KexiImageContextMenu::saveAs(): no data!" << endl;
		return;
	}
	if (!origFilename.isEmpty())
		origFilename = QString("/") + origFilename;

	if (fileExtension.isEmpty()) {
		// PNG data is the default
		fileExtension = "png";
	}
	
#ifdef Q_WS_WIN
	QString recentDir;
	QString fileName = QFileDialog::getSaveFileName(
		KFileDialog::getStartURL(":LastVisitedImagePath", recentDir).path() + origFilename,
		convertKFileDialogFilterToQFileDialogFilter(KImageIO::pattern(KImageIO::Writing)), 
		this, 0, i18n("Save Image to File"));
#else
	//! @todo add originalFileName! (requires access to KRecentDirs)
	QString fileName = KFileDialog::getSaveFileName(
		":LastVisitedImagePath", KImageIO::pattern(KImageIO::Writing), this, i18n("Save Image to File"));
#endif
	if (fileName.isEmpty())
		return;
	
	if (QFileInfo(fileName).extension().isEmpty())
		fileName += (QString(".")+fileExtension);
	kDebug() << fileName << endl;
	KURL url;
	url.setPath( fileName );

#ifdef Q_WS_WIN
	//save last visited path
	if (url.isLocalFile())
		KRecentDirs::add(":LastVisitedImagePath", url.directory());
#endif

	QFile f(fileName);
	if (f.exists() && KMessageBox::Yes != KMessageBox::warningYesNo(this, 
		"<qt>"+i18n("File \"%1\" already exists."
		"<p>Do you want to replace it with a new one?")
		.arg(QDir::convertSeparators(fileName))+"</qt>",0, 
		KGuiItem(i18n("&Replace")), KGuiItem(i18n("&Do not replace"))))
	{
		return;
	}

//! @todo use KURL?
	emit saveAsRequested(fileName);
}

void KexiImageContextMenu::cut()
{
	emit cutRequested();
}

void KexiImageContextMenu::copy()
{
	emit copyRequested();
}

void KexiImageContextMenu::paste()
{
	emit pasteRequested();
}

void KexiImageContextMenu::clear()
{
	emit clearRequested();
}

void KexiImageContextMenu::showProperties()
{
	emit showPropertiesRequested();
}

void KexiImageContextMenu::updateActionsAvailability()
{
	bool valueIsNull = true;
	bool valueIsReadOnly = true;
	emit updateActionsAvailabilityRequested(valueIsNull, valueIsReadOnly);

	d->insertFromFileAction->setEnabled( !valueIsReadOnly );
	d->saveAsAction->setEnabled( !valueIsNull );
	d->cutAction->setEnabled( !valueIsNull && !valueIsReadOnly );
	d->copyAction->setEnabled( !valueIsNull );
	d->pasteAction->setEnabled( !valueIsReadOnly );
	d->deleteAction->setEnabled( !valueIsNull && !valueIsReadOnly );
	if (d->propertiesAction)
		d->propertiesAction->setEnabled( !valueIsNull );
}

KActionCollection* KexiImageContextMenu::actionCollection() const
{
	return &d->actionCollection;
}

//static
bool KexiImageContextMenu::updateTitle(QPopupMenu *menu, const QString& title, const QString& icon)
{
	if (title.isEmpty())
		return false;

	/*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
	 (see doc/dev/settings.txt) */
	QString realTitle = i18n("%1 : Image").arg( title[0].upper() + title.mid(1) );

	const int id = menu->idAt(0);
	QMenuItem *item = menu->findItem(id);
	if (item && dynamic_cast<KPopupTitle *>(item->widget())) {
		if (icon.isEmpty())
			dynamic_cast<KPopupTitle *>(item->widget())->setTitle(realTitle);
		else {
			QPixmap pixmap(SmallIcon( icon ));
			dynamic_cast<KPopupTitle *>(item->widget())->setTitle(realTitle, &pixmap);
		}
	}
	else
		return false;

	return true;
}

#include "kexiimagecontextmenu.moc"
