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
#include <qimage.h>
#include <qheader.h>
#include <qrect.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qsimplerichtext.h>

#include <kinstance.h>
#include <klocale.h>
#include <klistview.h>
#include <kpushbutton.h>
#include <kconfig.h>
#include <kurl.h>
#include <kfileitem.h>
#include <kio/previewjob.h>
#include <kactivelabel.h>
#include <kdebug.h>

#include "koTemplates.h"

class KoRichTextListItemPrivate
{
  public:
    KoRichTextListItemPrivate()
      : m_fileItem(0)
    {
    }

    ~KoRichTextListItemPrivate()
    {
      delete m_fileItem;
    }

    KFileItem* m_fileItem;
};

KoRichTextListItem::KoRichTextListItem(QListView *parent)
  : KListViewItem(parent)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListViewItem *parent)
  : KListViewItem(parent)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListView *parent, QListViewItem *after)
  : KListViewItem(parent, after)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListViewItem *parent, QListViewItem *after)
  : KListViewItem(parent, after)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListView *parent,
                             QString label1, QString label2, QString label3, QString label4,
                             QString label5, QString label6, QString label7, QString label8)
  : KListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListViewItem *parent,
                             QString label1, QString label2, QString label3, QString label4,
                             QString label5, QString label6, QString label7, QString label8)
  : KListViewItem(parent, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListView *parent, QListViewItem *after,
                             QString label1, QString label2, QString label3, QString label4,
                             QString label5, QString label6, QString label7, QString label8)
  : KListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KoRichTextListItem::KoRichTextListItem(QListViewItem *parent, QListViewItem *after,
                             QString label1, QString label2, QString label3, QString label4,
                             QString label5, QString label6, QString label7, QString label8)
  : KListViewItem(parent, after, label1, label2, label3, label4, label5, label6, label7, label8)
{
  init();
}

KoRichTextListItem::~KoRichTextListItem()
{
  delete d;
}

void KoRichTextListItem::init()
{
  d = new KoRichTextListItemPrivate;
}

void KoRichTextListItem::paintCell(QPainter *p, const QColorGroup& cg, int column, int width, int alignment)
{
  QColorGroup _cg = cg;
  const QPixmap *pm = listView()->viewport()->backgroundPixmap();

  if (pm && !pm->isNull())
  {
    _cg.setBrush(QColorGroup::Base, QBrush(backgroundColor(column), *pm));
    QPoint o = p->brushOrigin();
    p->setBrushOrigin( o.x()-listView()->contentsX(), o.y()-listView()->contentsY() );
  }
  else
  {
    _cg.setColor((listView()->viewport()->backgroundMode() == Qt::FixedColor) ?
        QColorGroup::Background : QColorGroup::Base,
    backgroundColor(column));
  }

  QBrush paper;

  if(isSelected()) {
    paper = _cg.highlight();
    _cg.setColor(QColorGroup::Text, _cg.highlightedText());
  } else {
    paper = backgroundColor(column);
  }

  if(pm && !pm->isNull()) {
    paper.setPixmap(*pm);
  }

  p->fillRect(0, 0, width, height(), paper);
  int py = (height() - pixmap(column)->height()) / 2;
  p->drawPixmap(0, py, *pixmap(column));
  QSimpleRichText richText(text(column), listView()->font());
  richText.setWidth(width);
  int x = pixmap(column)->width() + listView()->itemMargin();
  int y = (height() - richText.height());

  if( y > 0) {
    y /= 2;
  } else {
    y = 0;
  }

  richText.draw(p, x, y, QRect(), _cg, &paper);
}

void KoRichTextListItem::setup()
{
  KListViewItem::setup();
  QSimpleRichText richText(text(0), listView()->font());
  richText.setWidth(listView()->width());
  int h = richText.height() + (2 * listView()->itemMargin());
  h = QMAX(height(), h);
  setHeight(h);
}

void KoRichTextListItem::setFileItem(KFileItem* item)
{
  d->m_fileItem = item;
}

KFileItem* KoRichTextListItem::fileItem() const
{
  return d->m_fileItem;
}


KoTemplatesPane::KoTemplatesPane(QWidget* parent, KInstance* instance, KoTemplateGroup *group)
  : KoDetailsPaneBase(parent, "TemplatesPane")
{
  KGuiItem openGItem(i18n("Use This Template"));
  m_openButton->setGuiItem(openGItem);
  m_documentList->header()->hide();

  for (KoTemplate* t = group->first(); t != 0L; t = group->next()) {
    if(t->isHidden())
      continue;

    QString listText = "<b>" + t->name() + "</b>";

    if(!t->description().isEmpty()) {
      listText += "<br>" + t->description();
    }

    KoRichTextListItem* item = new KoRichTextListItem(m_documentList, listText, t->name(), t->file());
    item->setPixmap(0, t->loadPicture(instance));
  }

  connect(m_documentList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));
  connect(m_documentList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
          this, SLOT(openTemplate(QListViewItem*)));
  connect(m_documentList, SIGNAL(returnPressed(QListViewItem*)),
          this, SLOT(openTemplate(QListViewItem*)));
  connect(m_openButton, SIGNAL(clicked()), this, SLOT(openTemplate()));

  m_documentList->setSelected(m_documentList->firstChild(), true);
}

void KoTemplatesPane::selectionChanged(QListViewItem* item)
{
  if(item) {
    m_openButton->setEnabled(true);
    m_alwaysUseCheckBox->setEnabled(true);
    m_titleLabel->setText(item->text(1));
    m_previewLabel->setPixmap(*(item->pixmap(0)));
  } else {
    m_openButton->setEnabled(false);
    m_alwaysUseCheckBox->setEnabled(false);
    m_titleLabel->setText("");
    m_previewLabel->setPixmap(QPixmap());
  }
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
    }

    ~KoRecentDocumentsPanePrivate()
    {
      if(m_previewJob)
        m_previewJob->kill();
    }

    KIO::PreviewJob* m_previewJob;
};

KoRecentDocumentsPane::KoRecentDocumentsPane(QWidget* parent, KInstance* instance)
  : KoDetailsPaneBase(parent, "RecentDocsPane")
{
  d = new KoRecentDocumentsPanePrivate;
  KGuiItem openGItem(i18n("Open Document"), "fileopen");
  m_openButton->setGuiItem(openGItem);
  m_alwaysUseCheckBox->hide();
  m_documentList->header()->hide();
  m_documentList->setSorting(-1); // Disable sorting

  QString oldGroup = instance->config()->group();
  instance->config()->setGroup("RecentFiles");

  int i = 0;
  QString value;
  KFileItemList fileList;
  fileList.setAutoDelete(false);

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
        fileList.append(fileItem);
        QString listText = "<b>" + name + "</b><br>" + url.path();
        KoRichTextListItem* item = new KoRichTextListItem(m_documentList, m_documentList->lastItem(),
            listText, name, url.path());
        item->setFileItem(fileItem);
        //center all icons in 64x64 area
        QImage icon = fileItem->pixmap(64).convertToImage();
        icon.setAlphaBuffer(true);
        icon = icon.copy((icon.width() - 64) / 2, (icon.height() - 64) / 2, 64, 64);
        item->setPixmap(0, QPixmap(icon));
        item->setPixmap(2, fileItem->pixmap(128));
      }
    }

    i++;
  } while ( !value.isEmpty() || i<=10 );

  instance->config()->setGroup( oldGroup );

  connect(m_documentList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));
  connect(m_documentList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
          this, SLOT(openFile(QListViewItem*)));
  connect(m_documentList, SIGNAL(returnPressed(QListViewItem*)),
          this, SLOT(openFile(QListViewItem*)));
  connect(m_openButton, SIGNAL(clicked()), this, SLOT(openFile()));

  m_documentList->setSelected(m_documentList->firstChild(), true);

  d->m_previewJob = KIO::filePreview(fileList, 200, 200);

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
  if(item) {
    m_openButton->setEnabled(true);
    m_titleLabel->setText(item->text(1));
    m_previewLabel->setPixmap(*(item->pixmap(2)));

    if(static_cast<KoRichTextListItem*>(item)->fileItem()) {
      KFileItem* fileItem = static_cast<KoRichTextListItem*>(item)->fileItem();
      QString details = "<table border=\"0\">";
      details += "<tr><td><b>Modified:</b></td>";
      details += "<td>" + fileItem->timeString(KIO::UDS_MODIFICATION_TIME) + "</td></tr>";
      details += "<tr><td><b>Accessed:</b></td>";
      details += "<td>" + fileItem->timeString(KIO::UDS_ACCESS_TIME) + "</td></tr>";
      details += "</table>";
      m_detailsLabel->setText(details);
    } else {
      m_detailsLabel->setText("");
    }
  } else {
    m_openButton->setEnabled(true);
    m_titleLabel->setText("");
    m_previewLabel->setPixmap(QPixmap());
    m_detailsLabel->setText("");
  }
}

void KoRecentDocumentsPane::openFile()
{
  QListViewItem* item = m_documentList->selectedItem();
  openFile(item);
}

void KoRecentDocumentsPane::openFile(QListViewItem* item)
{
  if(item)
    emit openFile(item->text(2));
}

void KoRecentDocumentsPane::previewResult(KIO::Job* job)
{
  if(d->m_previewJob == job)
    d->m_previewJob = 0;
}

void KoRecentDocumentsPane::updatePreview(const KFileItem* fileItem, const QPixmap& preview)
{
  if(preview.isNull()) {
    return;
  }

  QListViewItemIterator it(m_documentList);

  while(it.current()) {
    if(it.current()->text(2) == fileItem->url().path()) {
      it.current()->setPixmap(2, preview);
      QImage icon = preview.convertToImage();
      icon = icon.smoothScale(64, 64, QImage::ScaleMin);
      icon.setAlphaBuffer(true);
      icon = icon.copy((icon.width() - 64) / 2, (icon.height() - 64) / 2, 64, 64);
      it.current()->setPixmap(0, QPixmap(icon));

      if(it.current()->isSelected()) {
        m_previewLabel->setPixmap(preview);
      }

      break;
    }

    it++;
  }
}

#include "koDetailsPane.moc"
