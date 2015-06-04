/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#include "KexiFileWidget.h"
#include <kexi_global.h>

#include <db/driver.h>
#include <db/utils.h>
#include <core/kexi.h>
#include <core/KexiMainWindowIface.h>
#include <core/KexiMigrateManagerInterface.h>
#include <kexiutils/utils.h>
#include <migration/migratemanager.h>

#include <QObject>
#include <QPushButton>
#include <QApplication>
#include <QKeyEvent>
#include <QEvent>
#include <QAction>
#include <QMimeDatabase>
#include <QMimeType>

#include <kmessagebox.h>
#include <kdebug.h>
#include <KLocalizedString>

#include <kfile.h>
#include <kurlcombobox.h>
#include <kactioncollection.h>
#include <kfiledialog.h>

// added because of lack of krecentdirs.h
namespace KRecentDirs
{
    KDE_IMPORT void add(const QString &fileClass, const QString &directory);
};

//! @internal
class KexiFileWidget::Private
{
public:
    Private()
            : confirmOverwrites(true)
            , filtersUpdated(false) {
    }

    /*! Adds file dialog-compatible filter to @a filter and patterns to @allfilters based on
        @a mimeName mime type name. Does nothing is excludedMimeTypes contains this mime name. */
    bool addFilterForType(QString *filter, QStringList *allfilters, const QString &mimeName) const
    {
        QMimeDatabase db;
        const QMimeType mime = db.mimeTypeForName(mimeName);
        if (mime && !excludedMimeTypes.contains(mime.name().toLower())) {
            *filter += KexiUtils::fileDialogFilterString(mime);
            *allfilters += mime.globPatterns();
            return true;
        }
        return false;
    }

    QString lastFileName;
    KexiFileWidget::Mode mode;
    QSet<QString> additionalMimeTypes, excludedMimeTypes;
    QString defaultExtension;
    bool confirmOverwrites;
    bool filtersUpdated;
    QUrl highlightedUrl;
    QString recentDirClass;
};

//------------------

KexiFileWidget::KexiFileWidget(
    const QUrl &startDirOrVariable, Mode mode, QWidget *parent)
        :  KFileWidget(startDirOrVariable, parent)
        , d(new Private())
{
    kDebug() << startDirOrVariable.scheme();
    if (startDirOrVariable.scheme() == "kfiledialog") {
        KFileDialog::getStartUrl(startDirOrVariable, d->recentDirClass);
    }
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setMode(mode);
    QAction *previewAction = actionCollection()->action("preview");
    if (previewAction)
        previewAction->setChecked(false);
    setFocusProxy(locationEdit());
    connect(this, SIGNAL(fileHighlighted(QString)),
            this, SLOT(slotExistingFileHighlighted(QString)));
}

KexiFileWidget::~KexiFileWidget()
{
    kDebug() << d->recentDirClass;
    if (!d->recentDirClass.isEmpty()) {
        QString hf = highlightedFile();
        QUrl dir;
        if (hf.isEmpty()) {
            dir = baseUrl();
        }
        else {
            QFileInfo fi(hf);
            QString dirStr = fi.isDir() ? fi.absoluteFilePath() : fi.dir().absolutePath();
            dir = QUrl::fromLocalFile(dirStr);
        }
        kDebug() << dir;
        kDebug() << highlightedFile();
        if (!dir.isEmpty())
            KRecentDirs::add(d->recentDirClass, dir.url());
    }
    delete d;
}

void KexiFileWidget::slotExistingFileHighlighted(const QString& fileName)
{
    kDebug() << fileName;
    d->highlightedUrl = QUrl(fileName);
    emit fileHighlighted();
}

QString KexiFileWidget::highlightedFile() const
{
    return d->highlightedUrl.toLocalFile();
}

void KexiFileWidget::setMode(Mode mode)
{
    //delayed
    d->mode = mode;
    d->filtersUpdated = false;
    updateFilters();
}

QSet<QString> KexiFileWidget::additionalFilters() const
{
    return d->additionalMimeTypes;
}

void KexiFileWidget::setAdditionalFilters(const QSet<QString> &mimeTypes)
{
    //delayed
    d->additionalMimeTypes = mimeTypes;
    d->filtersUpdated = false;
}

QSet<QString> KexiFileWidget::excludedFilters() const
{
    return d->excludedMimeTypes;
}

void KexiFileWidget::setExcludedFilters(const QSet<QString> &mimeTypes)
{
    //delayed
    d->excludedMimeTypes.clear();
    //convert to lowercase
    foreach(const QString& mimeType, mimeTypes)
    d->excludedMimeTypes.insert(mimeType.toLower());
    d->filtersUpdated = false;
}

void KexiFileWidget::updateFilters()
{
    if (d->filtersUpdated)
        return;
    d->filtersUpdated = true;
    d->lastFileName.clear();
    clearFilter();

    QString filter;
    QStringList allfilters;

    const bool normalOpeningMode = d->mode & Opening && !(d->mode & Custom);
    const bool normalSavingMode = d->mode & SavingFileBasedDB && !(d->mode & Custom);

    if (normalOpeningMode || normalSavingMode) {
        d->addFilterForType(&filter, &allfilters, KexiDB::defaultFileBasedDriverMimeType());
    }
    if (normalOpeningMode || d->mode & SavingServerBasedDB) {
        d->addFilterForType(&filter, &allfilters, "application/x-kexiproject-shortcut");
    }
    if (normalOpeningMode || d->mode & SavingServerBasedDB) {
        d->addFilterForType(&filter, &allfilters, "application/x-kexi-connectiondata");
    }

    if (normalOpeningMode) {
        const QList<QString> supportedFileMimeTypes = KexiMainWindowIface::global()->migrateManager()->supportedFileMimeTypes();
        kDebug() << supportedFileMimeTypes;
        foreach (const QString& supportedFileMimeType, supportedFileMimeTypes) {
            d->addFilterForType(&filter, &allfilters, supportedFileMimeType);
        }
    }

    foreach(const QString& mimeName, d->additionalMimeTypes) {
        if (mimeName == "all/allfiles")
            continue;
        d->addFilterForType(&filter, &allfilters, mimeName);
    }

    if (!d->excludedMimeTypes.contains("all/allfiles")) {
        filter += filterWidget()->defaultFilter();
    }

    //remove duplicates made because upper- and lower-case extenstions are used:
    QStringList allfiltersUnique = allfilters.toSet().toList();
    qSort(allfiltersUnique);

    if (allfiltersUnique.count() > 1) {//prepend "all supoported files" entry
        filter.prepend(allfilters.join(" ") + "|"
                       + xi18n("All Supported Files (%1)", allfiltersUnique.join(", ")) + "\n");
    }

    if (filter.right(1) == "\n")
        filter.truncate(filter.length() - 1);
    setFilter(filter);

    if (d->mode & Opening) {
        KFileWidget::setMode(KFile::ExistingOnly | KFile::LocalOnly | KFile::File);
        setOperationMode(KFileWidget::Opening);
    } else {
        KFileWidget::setMode(KFile::LocalOnly | KFile::File);
        setOperationMode(KFileWidget::Saving);
    }
}

void KexiFileWidget::showEvent(QShowEvent * event)
{
    d->filtersUpdated = false;
    updateFilters();
    KFileWidget::showEvent(event);
}

/*TODO
QString KexiFileWidget::selectedFile() const
{
#ifdef Q_OS_WIN
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
        kDebug() << "KexiFileWidget::checkURL(): append extension, " << path;
      }
    }
  }
  kDebug() << "KexiFileWidget::currentFileName() == " << path;
  return path;
}
*/

bool KexiFileWidget::checkSelectedFile()
{
    kDebug() << "d->highlightedUrl: " << d->highlightedUrl;

    if (!locationEdit()->lineEdit()->text().isEmpty()) {
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

    kDebug() << "d->highlightedUrl: " << d->highlightedUrl;
    if (d->highlightedUrl.isEmpty()) {
        KMessageBox::error(this, xi18n("Enter a filename."));
        return false;
    }

    if (!currentFilter().isEmpty()) {
        if (d->mode & SavingFileBasedDB) {
            const QStringList filters( currentFilter().split(' ') );
            QString path = highlightedFile();
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
                path += (QLatin1String(".")+defaultExtension);
                kDebug() << "appended extension" << path;
                setSelection( path );
                d->highlightedUrl = QUrl(path);
            }
        }
    }

    kDebug() << "KexiFileWidget::checkURL() path: " << d->highlightedUrl;
// kDebug() << "KexiFileWidget::checkURL() fname: " << url.fileName();
//! @todo if ( url.isLocalFile() ) {
    QFileInfo fi(d->highlightedUrl.toLocalFile());
    if (mode() & KFile::ExistingOnly) {
        if (!fi.exists()) {
            KMessageBox::error(this, "<qt>" + xi18n("The file \"%1\" does not exist.",
                                                   QDir::convertSeparators(d->highlightedUrl.toLocalFile())));
            return false;
        } else if (mode() & KFile::File) {
            if (!fi.isFile()) {
                KMessageBox::error(this, "<qt>" + xi18n("Enter a filename."));
                return false;
            } else if (!fi.isReadable()) {
                KMessageBox::error(this, "<qt>" + xi18n("The file \"%1\" is not readable.",
                                                       QDir::convertSeparators(d->highlightedUrl.path())));
                return false;
            }
        }
    } else if (d->confirmOverwrites && !askForOverwriting(d->highlightedUrl.path(), this)) {
        return false;
    }
    return true;
}

//static
bool KexiFileWidget::askForOverwriting(const QString& filePath, QWidget *parent)
{
    QFileInfo fi(filePath);
    if (!fi.exists())
        return true;
    const int res = KMessageBox::warningYesNo(parent,
                    xi18n("The file \"%1\" already exists.\n"
                         "Do you want to overwrite it?", QDir::convertSeparators(filePath)), QString(),
                    KGuiItem(xi18n("Overwrite")), KStandardGuiItem::no());
    if (res == KMessageBox::Yes)
        return true;
    return false;
}

void KexiFileWidget::accept()
{
    kDebug() << "KexiFileWidget::accept()...";

    KFileWidget::accept();
// kDebug() << selectedFile();

// locationEdit->setFocus();
// QKeyEvent ev(QEvent::KeyPress, Qt::Key_Enter, '\n', 0);
// QApplication::sendEvent(locationEdit, &ev);
// QApplication::postEvent(locationEdit, &ev);

// kDebug() << "KexiFileWidget::accept() m_lastUrl == " << m_lastUrl.path();
// if (m_lastUrl.path()==currentURL().path()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
    /*
      if (d->lastFileName==selectedFile()) {//(js) to prevent more multiple kjob signals (I do not know why this is)
    //  m_lastUrl=QUrl();
        d->lastFileName.clear();
        kDebug() << "d->lastFileName==selectedFile()";
    #ifdef Q_OS_WIN
        return;
    #endif
      }
      kDebug() << "KexiFileWidget::accept(): path = " << selectedFile();
      if ( checkSelectedFile() ) {
        emit accepted();
      }
      d->lastFileName = selectedFile();

    #ifdef Q_OS_WIN
      saveLastVisitedPath(d->lastFileName);
    #endif*/
}

void KexiFileWidget::reject()
{
    kDebug() << "KexiFileWidget: reject!";
    emit rejected();
}

void KexiFileWidget::setLocationText(const QString& fn)
{
    locationEdit()->setUrl(QUrl(fn));
    /*
    #ifdef Q_OS_WIN
      //js @todo
      setSelection(fn);
    #else
      setSelection(fn);
    // locationEdit->setCurrentText(fn);
    // locationEdit->lineEdit()->setEdited( true );
    // setSelection(fn);
    #endif*/
}

void KexiFileWidget::focusInEvent(QFocusEvent *)
{
    locationEdit()->setFocus();
}

/*bool KexiFileWidget::eventFilter ( QObject * watched, QEvent * e )
{
  //filter-out ESC key
  if (e->type()==QEvent::KeyPress && static_cast<QKeyEvent*>(e)->key()==Qt::Key_Escape
   && static_cast<QKeyEvent*>(e)->state()==Qt::NoButton) {
    static_cast<QKeyEvent*>(e)->accept();
    emit rejected();
    return true;
  }
  return KexiFileWidgetBase::eventFilter(watched,e);
} */

void KexiFileWidget::setDefaultExtension(const QString& ext)
{
    d->defaultExtension = ext;
}

void KexiFileWidget::setConfirmOverwrites(bool set)
{
    d->confirmOverwrites = set;
}
