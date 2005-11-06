/* This file is part of the KDE project
   Copyright (C) 2005 Peter Simonsson <psn@linux.se>

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
#include "koDetailsPane.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qfile.h>

#include <kinstance.h>
#include <klocale.h>
#include <klistview.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kurl.h>
#include <kactivelabel.h>
#include <kfileitem.h>
#include <kio/previewjob.h>

#include "koTemplates.h"

KoTemplatesPane::KoTemplatesPane(QWidget* parent, KInstance* instance, KoTemplateGroup *group)
  : KoDetailsPaneBase(parent, "TemplatesPane")
{
  KGuiItem openGItem(i18n("Use This Template"));
  m_openButton->setGuiItem(openGItem);
  m_documentList->setColumnText (0, i18n("Template"));

  for (KoTemplate* t = group->first(); t != 0L; t = group->next()) {
    if(t->isHidden())
      continue;

    KListViewItem* item = new KListViewItem(m_documentList, t->name(), t->description(), t->file());
    item->setPixmap(0, t->loadPicture(instance));

//       if (name == t->name())
//       {
//         itemtoreturn = item;
//       }
  }

  connect(m_documentList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));
  connect(m_documentList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
          this, SLOT(openTemplate(QListViewItem*)));
  connect(m_openButton, SIGNAL(clicked()), this, SLOT(openTemplate()));

  m_documentList->setSelected(m_documentList->firstChild(), true);
}

void KoTemplatesPane::selectionChanged(QListViewItem* item)
{
  m_titleLabel->setText(item->text(0));
  m_iconLabel->setPixmap(*(item->pixmap(0)));
  m_detailsLabel->setText(item->text(1));
}

void KoTemplatesPane::openTemplate()
{
  QListViewItem* item = m_documentList->selectedItem();
  openTemplate(item);
}

void KoTemplatesPane::openTemplate(QListViewItem* item)
{
  if(item) {
    emit openTemplate(item->text(2));
  }
}

class KoRecentDocumentsPanePrivate
{
  public:
    KoRecentDocumentsPanePrivate()
      : m_previewJob(0)
    {
      m_fileList.setAutoDelete(true);
    }

    ~KoRecentDocumentsPanePrivate()
    {
      if(m_previewJob)
        m_previewJob->kill();
    }

    KIO::PreviewJob* m_previewJob;
    KFileItemList m_fileList;
};

KoRecentDocumentsPane::KoRecentDocumentsPane(QWidget* parent, KInstance* instance)
  : KoDetailsPaneBase(parent, "RecentDocsPane")
{
  d = new KoRecentDocumentsPanePrivate;
  KGuiItem openGItem(i18n("Open Document"), "fileopen");
  m_openButton->setGuiItem(openGItem);
  m_documentList->setColumnText (0, i18n("Documents"));
  m_alwaysUseCheckbox->hide();

  QString oldGroup = instance->config()->group();
  instance->config()->setGroup("RecentFiles");

  int i = 0;
  QString value;

  do {
    QString key = QString("File%1").arg(i);
    value = instance->config()->readPathEntry(key);

    if(!value.isEmpty()) {
      QString path = value;
      QString name;

      // Support for kdelibs-3.5's new RecentFiles format: name[url]
      if(path.endsWith("]")) {
        int pos = path.find("[");
        name = path.mid(0, pos - 1);
        path = path.mid(pos + 1, path.length() - pos - 2);
      }

      KURL url(path);

      if(name.isEmpty())
        name = url.filename();

      if(!url.isLocalFile() || QFile::exists(url.path())) {
        KFileItem* fileItem = new KFileItem(KFileItem::Unknown, KFileItem::Unknown, url);
        d->m_fileList.append(fileItem);
        KListViewItem* item = new KListViewItem(m_documentList, name, url.path());
        item->setPixmap(0, fileItem->pixmap(32));
        item->setPixmap(2, fileItem->pixmap(64));
      }
    }

    i++;
  } while ( !value.isEmpty() || i<=10 );

  instance->config()->setGroup( oldGroup );

  connect(m_documentList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));
  connect(m_documentList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
          this, SLOT(openFile(QListViewItem*)));
  connect(m_openButton, SIGNAL(clicked()), this, SLOT(openFile()));

  m_documentList->setSelected(m_documentList->firstChild(), true);

  d->m_previewJob = KIO::filePreview(d->m_fileList, 128, 128);

  connect(d->m_previewJob, SIGNAL(result(KIO::Job*)), this, SLOT(previewResult(KIO::Job*)));
  connect(d->m_previewJob, SIGNAL(gotPreview(const KFileItem*, const QPixmap&)),
          this, SLOT(updatePreview(const KFileItem*, const QPixmap&)));
}

KoRecentDocumentsPane::~KoRecentDocumentsPane()
{
  delete d;
}

void KoRecentDocumentsPane::selectionChanged(QListViewItem* item)
{
  m_titleLabel->setText(item->text(0));
  m_detailsLabel->setText(item->text(1));
  m_iconLabel->setPixmap(*(item->pixmap(2)));
}

void KoRecentDocumentsPane::openFile()
{
  QListViewItem* item = m_documentList->selectedItem();
  openFile(item);
}

void KoRecentDocumentsPane::openFile(QListViewItem* item)
{
  if(item)
    emit openFile(item->text(1));
}

void KoRecentDocumentsPane::previewResult(KIO::Job* job)
{
  if(d->m_previewJob == job)
    d->m_previewJob = 0;
}

void KoRecentDocumentsPane::updatePreview(const KFileItem* fileItem, const QPixmap& preview)
{
  QListViewItemIterator it(m_documentList);

  while(it.current()) {
    if(it.current()->text(1) == fileItem->url().path()) {
      it.current()->setPixmap(2, preview);

      if(it.current()->isSelected()) {
        m_iconLabel->setPixmap(preview);
      }

      break;
    }
  }
}

#include "koDetailsPane.moc"
