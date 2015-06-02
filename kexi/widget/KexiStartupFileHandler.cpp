/* This file is part of the KDE project
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include <db/driver.h>
#include <db/utils.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiContextMessage.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kfile.h>
#include <kurlcombobox.h>
#include <kactioncollection.h>
#include <kfiledialog.h>
#include <kurlrequester.h>

#include <QKeyEvent>
#include <QEvent>
#include <QAction>
#include <QLineEdit>
#include <QEventLoop>
#include <QApplication>
#include <QMimeDatabase>
#include <QMimeType>
#include <QUrl>

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
    ~Private() {
        if (messageWidgetLoop) {
            messageWidgetLoop->exit(0);
            messageWidgetLoop->processEvents(); // for safe exit
            messageWidgetLoop->exit(0);
            delete messageWidgetLoop;
        }
    }

    void setUrl(const QUrl &url)
    {
        if (requester) {
            requester->setUrl(url);
        }
        else {
            dialog->setUrl(url);
        }
    }

    QPointer<KFileDialog> dialog;
    QPointer<KUrlRequester> requester;
    QString lastFileName;
    KexiStartupFileHandler::Mode mode;
    QSet<QString> additionalMimeTypes, excludedMimeTypes;
    QString defaultExtension;
    bool confirmOverwrites;
    QString recentDirClass;
    
    QPointer<QEventLoop> messageWidgetLoop;
    //! Used in KexiStartupFileHandler::askForOverwriting() to remember path that
    //! was recently accepted for overwrite by the user.
    QString recentFilePathConfirmed;
};

//------------------

KexiStartupFileHandler::KexiStartupFileHandler(
    const QUrl &startDirOrVariable, Mode mode, KFileDialog *dialog)
    :  QObject(dialog->parent())
    , d(new Private)
{
    d->dialog = dialog;
    init(startDirOrVariable, mode);
}

KexiStartupFileHandler::KexiStartupFileHandler(
    const QUrl &startDirOrVariable, Mode mode, KUrlRequester *requester)
    :  QObject(requester->parent())
    , d(new Private)
{
    d->requester = requester;
    d->dialog = d->requester->fileDialog();
    init(startDirOrVariable, mode);
}

void KexiStartupFileHandler::init(const QUrl &startDirOrVariable, Mode mode)
{
    connect(d->dialog, SIGNAL(accepted()), this, SLOT(slotAccepted()));
    QUrl url;
    if (startDirOrVariable.scheme() == "kfiledialog") {
        url = KFileDialog::getStartUrl(startDirOrVariable, d->recentDirClass);
    }
    else {
        url = startDirOrVariable;
    }
    d->setUrl(url);
    setMode(mode);
    QAction *previewAction = d->dialog->actionCollection()->action("preview");
    if (previewAction)
        previewAction->setChecked(false);
}

KexiStartupFileHandler::~KexiStartupFileHandler()
{
    saveRecentDir();
    delete d;
}

void KexiStartupFileHandler::saveRecentDir()
{
    if (!d->recentDirClass.isEmpty()) {
        kDebug() << d->recentDirClass;
        
        QUrl dirUrl;
        if (d->requester)
            dirUrl = d->requester->url();
        else if (d->dialog)
            dirUrl = d->dialog->selectedUrl();
        kDebug() << dirUrl;
        if (dirUrl.isValid() && dirUrl.isLocalFile()) {
            dirUrl = dirUrl.adjusted(QUrl::RemoveFilename);
            dirUrl.setPath(dirUrl.path() + QString());
            kDebug() << "Added" << dirUrl.url() << "to recent dirs class" << d->recentDirClass;
            KRecentDirs::add(d->recentDirClass, dirUrl.url());
        }
    }
}

KexiStartupFileHandler::Mode KexiStartupFileHandler::mode() const
{
    return d->mode;
}

void KexiStartupFileHandler::setMode(Mode mode)
{
    //delayed
    d->mode = mode;
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
    updateFilters();
}

void KexiStartupFileHandler::updateFilters()
{
    d->lastFileName.clear();
    d->dialog->clearFilter();

    QString filter;
    QMimeDatabase db;
    QMimeType mime;
    QStringList allfilters;

    const bool normalOpeningMode = d->mode & Opening && !(d->mode & Custom);
    const bool normalSavingMode = d->mode & SavingFileBasedDB && !(d->mode & Custom);

    if (normalOpeningMode || normalSavingMode) {
        mime = db.mimeTypeForName(KexiDB::defaultFileBasedDriverMimeType());
        if (mime && !d->excludedMimeTypes.contains(mime.name().toLower())) {
            filter += KexiUtils::fileDialogFilterString(mime);
            allfilters += mime.globPatterns();
        }
    }
    if (normalOpeningMode || d->mode & SavingServerBasedDB) {
        mime = db.mimeTypeForName("application/x-kexiproject-shortcut");
        if (mime && !d->excludedMimeTypes.contains(mime.name().toLower())) {
            filter += KexiUtils::fileDialogFilterString(mime);
            allfilters += mime.globPatterns();
        }
    }
    if (normalOpeningMode || d->mode & SavingServerBasedDB) {
        mime = db.mimeTypeForName("application/x-kexi-connectiondata");
        if (mime && !d->excludedMimeTypes.contains(mime.name().toLower())) {
            filter += KexiUtils::fileDialogFilterString(mime);
            allfilters += mime.globPatterns();
        }
    }

//! @todo hardcoded for MSA:
    if (normalOpeningMode) {
        mime = db.mimeTypeForName("application/vnd.ms-access");
        if (mime && !d->excludedMimeTypes.contains(mime.name().toLower())) {
            filter += KexiUtils::fileDialogFilterString(mime);
            allfilters += mime.globPatterns();
        }
    }

    foreach(const QString& mimeName, d->additionalMimeTypes) {
        if (mimeName == "all/allfiles")
            continue;
        if (d->excludedMimeTypes.contains(mimeName.toLower()))
            continue;
        filter += KexiUtils::fileDialogFilterString(mimeName);
        mime = db.mimeTypeForName(mimeName);
        allfilters += mime.globPatterns();
    }

    if (!d->excludedMimeTypes.contains("all/allfiles"))
        filter += KexiUtils::fileDialogFilterString("all/allfiles");
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

//! @todo
/*TODO
QString KexiStartupFileDialog::selectedFile() const
{
#ifdef Q_WS_WIN
// QString path = selectedFile();
  //js @todo
// kDebug() << "selectedFile() == " << path << " '" << url().fileName() << "' " << m_lineEdit->text();
  QString path = dir()->absolutePath();
  if (!path.endsWith('/') && !path.endsWith("\\"))
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
      const QStringList filters( currentFilter().split(' ') );
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
      }
    }
  }
  kDebug() << "KexiStartupFileDialog::currentFileName() == " << path;
  return path;
}
*/

bool KexiStartupFileHandler::checkSelectedUrl()
{
    //kDebug() << "d->highlightedUrl: " << d->highlightedUrl;

    QUrl url;
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
            d->highlightedUrl = QUrl::fromLocalFile(firstUrl);
        else
            d->highlightedUrl.addPath(firstUrl);
    }
#endif
    //kDebug() << "d->highlightedUrl: " << d->highlightedUrl;
    if (!url.isValid() || QFileInfo(url.path()).isDir()) {
        KMessageBox::error(d->dialog->parentWidget(), i18n("Enter a filename."));
        return false;
    }

    if (!d->dialog->currentFilter().isEmpty()) {
        if (d->mode & SavingFileBasedDB) {
            const QStringList filters( d->dialog->currentFilter().split(' ') );
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
                url = QUrl(path);
                d->setUrl(url);
            }
        }
    }

// kDebug() << "KexiStartupFileDialog::checkURL() path: " << d->highlightedUrl;
// kDebug() << "KexiStartupFileDialog::checkURL() fname: " << url.fileName();
//! @todo if ( url.isLocalFile() ) {
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
    }
    else if (d->confirmOverwrites && !askForOverwriting(url.toLocalFile()))
    {
        return false;
    }
    return true;
}

void KexiStartupFileHandler::messageWidgetActionYesTriggered()
{
    d->messageWidgetLoop->exit(1);
}

void KexiStartupFileHandler::messageWidgetActionNoTriggered()
{
    d->messageWidgetLoop->exit(0);
}

void KexiStartupFileHandler::updateUrl(const QString &name)
{
    QUrl url = d->requester->url();
    QString fn = KexiUtils::stringToFileName(name);
    if (!fn.isEmpty() && !fn.endsWith(".kexi"))
        fn += ".kexi";
    url = url.adjusted(QUrl::RemoveFilename);
    url.setPath(url.path() + fn);
    d->requester->setUrl(url);
}

bool KexiStartupFileHandler::askForOverwriting(const QString& filePath)
{
    QFileInfo fi(filePath);
    if (d->recentFilePathConfirmed == filePath) {
        return true;
    }
    d->recentFilePathConfirmed.clear();
    if (!fi.exists())
        return true;
    KexiContextMessage message(
        i18n("This file already exists. Do you want to overwrite it?"));
    QScopedPointer<QAction> messageWidgetActionYes(new QAction(i18n("Overwrite"), 0));
    connect(messageWidgetActionYes.data(), SIGNAL(triggered()),
            this, SLOT(messageWidgetActionYesTriggered()));
    message.addAction(messageWidgetActionYes.data());
    QScopedPointer<QAction> messageWidgetActionNo(new QAction(KStandardGuiItem::no().text(), 0));
    connect(messageWidgetActionNo.data(), SIGNAL(triggered()),
            this, SLOT(messageWidgetActionNoTriggered()));
    message.addAction(messageWidgetActionNo.data());
    message.setDefaultAction(messageWidgetActionNo.data());
    emit askForOverwriting(message);
    if (!d->messageWidgetLoop) {
        d->messageWidgetLoop = new QEventLoop;
    }
    bool ok = d->messageWidgetLoop->exec();
    if (ok) {
        d->recentFilePathConfirmed = filePath;
    }
    return ok;
}

void KexiStartupFileHandler::setLocationText(const QString& fn)
{
    d->dialog->locationEdit()->setUrl(QUrl(fn));
}

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

