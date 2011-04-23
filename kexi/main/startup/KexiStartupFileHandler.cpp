/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

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

#include "KexiStartupFileHandler.h"
#include <kexi_global.h>

#include <kexidb/driver.h>
#include <kexidb/utils.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

#include <QKeyEvent>
#include <QEvent>
#include <QAction>
#include <QLineEdit>

#include <KMessageBox>
#include <klocale.h>
#include <kdebug.h>
#include <kmimetype.h>
#include <kfile.h>
#include <kurlcombobox.h>
#include <KToolBar>
#include <KActionCollection>
#include <KFileDialog>
#include <KUrlRequester>
#include <KUrl>

// added because of lack of krecentdirs.h
namespace KRecentDirs
{
    KDE_IMPORT void add(const QString &fileClass, const QString &directory);
};

//! @internal
class KexiStartupFileHandler::Private
{
public:
    Private()
            : confirmOverwrites(true)
            //, filtersUpdated(false)
    {
    }

    QPointer<KFileDialog> dialog;
    QPointer<KUrlRequester> requester;
    QString lastFileName;
    KexiStartupFileHandler::Mode mode;
    QSet<QString> additionalMimeTypes, excludedMimeTypes;
    QString defaultExtension;
    bool confirmOverwrites;
    //bool filtersUpdated;
    //KUrl highlightedUrl;
    QString recentDirClass;
};

//------------------

KexiStartupFileHandler::KexiStartupFileHandler(
    const KUrl &startDirOrVariable, Mode mode, KFileDialog *dialog)
    :  QObject(dialog->parent())
    , d(new Private)
{
    d->dialog = dialog;
    init(startDirOrVariable, mode);
}

KexiStartupFileHandler::KexiStartupFileHandler(
    const KUrl &startDirOrVariable, Mode mode, KUrlRequester *requester)
    :  QObject(requester->parent())
    , d(new Private)
{
    d->requester = requester;
    d->dialog = d->requester->fileDialog();
    init(startDirOrVariable, mode);
}

void KexiStartupFileHandler::init(const KUrl &startDirOrVariable, Mode mode)
{
/*    if (d->requester || d->dialog) {
        QWidget *w = d->requester ? static_cast<QWidget*>(d->requester) : 
            static_cast<QWidget*>(d->dialog);
        connect(w, SIGNAL(destroyed()), this, SLOT(saveRecentDir()));
    }*/
    connect(d->dialog, SIGNAL(accepted()), this, SLOT(slotAccepted()));
    //d->dialog->setStartDir(startDirOrVariable);
    KUrl url;
    if (startDirOrVariable.protocol() == "kfiledialog") {
        url = KFileDialog::getStartUrl(startDirOrVariable, d->recentDirClass);
    }
    else {
        url = startDirOrVariable;
    }
    if (d->requester)
        d->requester->setUrl(url);
    else
        d->dialog->setUrl(url);
    //setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMode(mode);
    QAction *previewAction = d->dialog->actionCollection()->action("preview");
    if (previewAction)
        previewAction->setChecked(false);

    // setFocusProxy(locationEdit());

//    connect(dialog, SIGNAL(fileHighlighted(const QString&)),
//            this, SLOT(slotExistingFileHighlighted(const QString&)));
}

KexiStartupFileHandler::~KexiStartupFileHandler()
{
    saveRecentDir();
    delete d;
//Qt4 #ifdef Q_WS_WIN
// saveLastVisitedPath(currentFileName());
//#endif
}

void KexiStartupFileHandler::saveRecentDir()
{
    if (!d->recentDirClass.isEmpty()) {
        kDebug() << d->recentDirClass;
        
        KUrl dirUrl;
        if (d->requester)
            dirUrl = d->requester->url();
        else if (d->dialog)
            dirUrl = d->dialog->selectedUrl();
        kDebug() << dirUrl;
        if (dirUrl.isValid() && dirUrl.isLocalFile()) {
            dirUrl.setFileName(QString());
            kDebug() << "Added" << dirUrl.url() << "to recent dirs class" << d->recentDirClass;
            KRecentDirs::add(d->recentDirClass, dirUrl.url());
        }
    }
}

/*
void KexiStartupFileDialog::slotExistingFileHighlighted(const QString& fileName)
{
    kDebug() << fileName;
    d->highlightedUrl = KUrl(fileName);
    //updateDialogOKButton(0);
    emit fileHighlighted();
}

QString KexiStartupFileDialog::highlightedFile() const
{
    return d->highlightedUrl.toLocalFile();
}*/

KexiStartupFileHandler::Mode KexiStartupFileHandler::mode() const
{
    return d->mode;
}

void KexiStartupFileHandler::setMode(Mode mode)
{
    //delayed
    d->mode = mode;
    //d->filtersUpdated = false;
    updateFilters();
}

QSet<QString> KexiStartupFileHandler::additionalFilters() const
{
    return d->additionalMimeTypes;
}

void KexiStartupFileHandler::setAdditionalFilters(const QSet<QString> &mimeTypes)
{
    //delayed
    d->additionalMimeTypes = mimeTypes;
    //d->filtersUpdated = false;
    updateFilters();
}

QSet<QString> KexiStartupFileHandler::excludedFilters() const
{
    return d->excludedMimeTypes;
}

void KexiStartupFileHandler::setExcludedFilters(const QSet<QString> &mimeTypes)
{
    //delayed
    d->excludedMimeTypes.clear();
    //convert to lowercase
    foreach(const QString& mimeType, mimeTypes) {
        d->excludedMimeTypes.insert(mimeType.toLower());
    }
    //d->filtersUpdated = false;
    updateFilters();
}

void KexiStartupFileHandler::updateFilters()
{
    //if (d->filtersUpdated)
    //    return;
    //d->filtersUpdated = true;

    d->lastFileName.clear();
// m_lastUrl = KUrl();

    d->dialog->clearFilter();

    QString filter;
    KMimeType::Ptr mime;
    QStringList allfilters;

    const bool normalOpeningMode = d->mode & Opening && !(d->mode & Custom);
    const bool normalSavingMode = d->mode & SavingFileBasedDB && !(d->mode & Custom);

    if (normalOpeningMode || normalSavingMode) {
        mime = KMimeType::mimeType(KexiDB::defaultFileBasedDriverMimeType());
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

    foreach(const QString& mimeName, d->additionalMimeTypes) {
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
// mime = KMimeType::mimeType("all/allfiles");
// if (mime) {
//  filter += QString(mime->patterns().isEmpty() ? "*" : mime->patterns().join(" "))
//   + "|" + mime->comment()+ " (*)\n";
// }
    //remove duplicates made because upper- and lower-case extenstions are used:
    QStringList allfiltersUnique = allfilters.toSet().toList();
    qSort(allfiltersUnique);

    if (allfiltersUnique.count() > 1) {//prepend "all supoported files" entry
        filter.prepend(allfilters.join(" ") + "|"
                       + i18n("All Supported Files (%1)", allfiltersUnique.join(", ")) + "\n");
    }

    if (filter.right(1) == "\n")
        filter.truncate(filter.length() - 1);
    d->dialog->setFilter(filter);

    if (d->mode & Opening) {
        d->dialog->setMode(KFile::ExistingOnly | KFile::LocalOnly | KFile::File);
        d->dialog->setOperationMode(KFileDialog::Opening);
    } else {
        d->dialog->setMode(KFile::LocalOnly | KFile::File);
        d->dialog->setOperationMode(KFileDialog::Saving);
    }
}

/*void KexiStartupFileDialog::showEvent(QShowEvent * event)
{
    d->filtersUpdated = false;
    updateFilters();
    KFileWidget::showEvent(event);
}*/

/*TODO
QString KexiStartupFileDialog::selectedFile() const
{
//Qt4 setResult( QDialog::Accepted ); // selectedURL tests for it

#ifdef Q_WS_WIN
// QString path = selectedFile();
  //js @todo
// kDebug() << "selectedFile() == " << path << " '" << url().fileName() << "' " << m_lineEdit->text();
  QString path = dir()->absolutePath();
  if (!path.endsWith("/") && !path.endsWith("\\"))
    path.append("/");
  path += m_lineEdit->text();
// QString path = QFileInfo(selectedFile()).dirPath(true) + "/" + m_lineEdit->text();
#else
// QString path = locationEdit->currentText().trimmed(); //url.path().trimmed(); that does not work, if the full path is not in the location edit !!!!!
  QString path( KFileWidget::selectedFile() );
  kDebug() << "prev selectedFile() == " << path;
  kDebug() << "locationEdit == " << locationEdit()->currentText().trimmed();
  //make sure user-entered path is acceped:
#ifdef __GNUC__
#warning TODO? setSelection( locationEdit()->currentText().trimmed() );
#else
#pragma WARNING( TODO? setSelection( locationEdit()->currentText().trimmed() ); )
#endif
// path = KFileWidget::selectedFile();
  path = locationEdit()->currentText().trimmed();
  kDebug() << "selectedFile() == " << path;

#endif

  if (!currentFilter().isEmpty()) {
    if (d->mode & SavingFileBasedDB) {
      const QStringList filters( currentFilter().split(" ") );
      kDebug()<< " filter == " << filters;
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
        kDebug() << "KexiStartupFileDialog::checkURL(): append extension, " << path;
//Qt4    setSelection( path );
      }
    }
  }
  kDebug() << "KexiStartupFileDialog::currentFileName() == " << path;
  return path;
}
*/

bool KexiStartupFileHandler::checkSelectedUrl()
{
    //accept();

// KUrl url = currentURL();
// QString path = url.path().trimmed();
// QString path = selectedFile().trimmed();
    //kDebug() << "d->highlightedUrl: " << d->highlightedUrl;

    KUrl url;
    if (d->requester)
        url = d->requester->url();
    else
       url = d->dialog->selectedUrl();
    kDebug() << url;
#if 0
    if (/*d->highlightedUrl.isEmpty() &&*/ !locationEdit()->lineEdit()->text().isEmpty()) {
        kDebug() << locationEdit()->lineEdit()->text();
        //kDebug() << locationEdit()->urls();
        kDebug() << baseUrl();

        d->highlightedUrl = baseUrl();
        const QString firstUrl(locationEdit()->lineEdit()->text());   // FIXME: find first...
        if (QDir::isAbsolutePath(firstUrl))
            d->highlightedUrl = KUrl::fromPath(firstUrl);
        else
            d->highlightedUrl.addPath(firstUrl);
    }
#endif
    //kDebug() << "d->highlightedUrl: " << d->highlightedUrl;
// if (url.fileName().trimmed().isEmpty()) {
    if (!url.isValid()) {
        KMessageBox::error(d->dialog->parentWidget(), i18n("Enter a filename."));
        return false;
    }

    if (!d->dialog->currentFilter().isEmpty()) {
        if (d->mode & SavingFileBasedDB) {
            const QStringList filters( d->dialog->currentFilter().split(" ") );
            QString path = url.toLocalFile();
            kDebug()<< "filter:" << filters << "path:" << path;
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
                if (defaultExtension.isEmpty()) {
                    defaultExtension = filters.first().trimmed().mid(2); //first one
                }
                path += (QLatin1String(".") + defaultExtension);
                kDebug() << "appended extension, result:" << path;
                url = KUrl(path);
                d->dialog->setUrl(url);
            }
        }
    }

    //kDebug() << "KexiStartupFileDialog::checkURL() path: " << d->highlightedUrl;
// kDebug() << "KexiStartupFileDialog::checkURL() fname: " << url.fileName();
//todo if ( url.isLocalFile() ) {
    QFileInfo fi(url.toLocalFile());
    if (d->mode & KFile::ExistingOnly) {
        if (!fi.exists()) {
            KMessageBox::error(d->dialog->parentWidget(),
                               "<qt>" + i18n("The file \"%1\" does not exist.",
                               QDir::convertSeparators(url.toLocalFile())));
            return false;
        } else if (mode() & KFile::File) {
            if (!fi.isFile()) {
                KMessageBox::error(d->dialog->parentWidget(),
                                   "<qt>" + i18n("Enter a filename."));
                return false;
            } else if (!fi.isReadable()) {
                KMessageBox::error(d->dialog->parentWidget(),
                                   "<qt>" + i18n("The file \"%1\" is not readable.",
                                   QDir::convertSeparators(url.toLocalFile())));
                return false;
            }
        }
    } else if (d->confirmOverwrites
               && !askForOverwriting(url.toLocalFile(), d->dialog->parentWidget()))
    {
        return false;
    }
// }
    return true;
}

//static
bool KexiStartupFileHandler::askForOverwriting(const QString& filePath, QWidget *parent)
{
    QFileInfo fi(filePath);
    if (!fi.exists())
        return true;
    const int res = KMessageBox::warningYesNo(parent,
                        i18n("The file \"%1\" already exists.\n"
                             "Do you want to overwrite it?",
                             QDir::convertSeparators(filePath)), QString(),
                        KGuiItem(i18n("Overwrite")), KStandardGuiItem::no());
    return res == KMessageBox::Yes;
}

#if 0
void KexiStartupFileDialog::accept()
{
    kDebug() << "KexiStartupFileDialog::accept()...";

    KFileWidget::accept();
// kDebug() << selectedFile();

// locationEdit->setFocus();
// QKeyEvent ev(QEvent::KeyPress, Qt::Key_Enter, '\n', 0);
// QApplication::sendEvent(locationEdit, &ev);
// QApplication::postEvent(locationEdit, &ev);

// kDebug() << "KexiStartupFileDialog::accept() m_lastUrl == " << m_lastUrl.path();
// if (m_lastUrl.path()==currentURL().path()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
    /*
      if (d->lastFileName==selectedFile()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
    //  m_lastUrl=KUrl();
        d->lastFileName.clear();
        kDebug() << "d->lastFileName==selectedFile()";
    #ifdef Q_WS_WIN
        return;
    #endif
      }
      kDebug() << "KexiStartupFileDialog::accept(): path = " << selectedFile();
      if ( checkSelectedFile() ) {
        emit accepted();
      }
      d->lastFileName = selectedFile();

    #ifdef Q_WS_WIN
      saveLastVisitedPath(d->lastFileName);
    #endif*/
}

void KexiStartupFileDialog::reject()
{
    kDebug() << "KexiStartupFileDialog: reject!";
    emit rejected();
}

/*#ifndef Q_WS_WIN
KUrlComboBox *KexiStartupFileDialog::locationWidget() const
{
  return locationEdit;
}
#endif
*/
#endif

void KexiStartupFileHandler::setLocationText(const QString& fn)
{
    d->dialog->locationEdit()->setUrl(KUrl(fn));
    /*
    #ifdef Q_WS_WIN
      //js @todo
      setSelection(fn);
    #else
      setSelection(fn);
    // locationEdit->setCurrentText(fn);
    // locationEdit->lineEdit()->setEdited( true );
    // setSelection(fn);
    #endif*/
}

/*void KexiStartupFileDialog::focusInEvent(QFocusEvent *)
{
    locationEdit()->setFocus();
}*/

/*bool KexiStartupFileDialog::eventFilter ( QObject * watched, QEvent * e )
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

void KexiStartupFileHandler::setDefaultExtension(const QString& ext)
{
    d->defaultExtension = ext;
}

void KexiStartupFileHandler::setConfirmOverwrites(bool set)
{
    d->confirmOverwrites = set;
}

void KexiStartupFileHandler::slotAccepted()
{
    checkSelectedUrl();
}

#include "KexiStartupFileHandler.moc"
