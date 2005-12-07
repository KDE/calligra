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
#include <kdebug.h>
#include <ktextbrowser.h>
#include <kapplication.h>

#include "koTemplates.h"

class KoFileListItem : public KListViewItem
{
  public:
    KoFileListItem(KListView* listView, QListViewItem* after, const QString& filename,
                   const QString& fullPath, KFileItem* fileItem)
      : KListViewItem(listView, after, filename, fullPath), m_fileItem(fileItem)
    {
    }

    ~KoFileListItem()
    {
      delete m_fileItem;
    }

    KFileItem* fileItem() const
    {
      return m_fileItem;
    }

  private:
    KFileItem* m_fileItem;
};

class KoTemplatesPanePrivate
{
  public:
    KoTemplatesPanePrivate()
      : m_instance(0), m_selected(false)
    {
    }

    KInstance* m_instance;
    bool m_selected;
    QString m_alwaysUseTemplate;
};


KoTemplatesPane::KoTemplatesPane(QWidget* parent, KInstance* instance,
                                 KoTemplateGroup *group, KoTemplate* defaultTemplate)
  : KoDetailsPaneBase(parent, "TemplatesPane")
{
  d = new KoTemplatesPanePrivate;
  d->m_instance = instance;
  KGuiItem openGItem(i18n("Use This Template"));
  m_openButton->setGuiItem(openGItem);
  m_documentList->header()->hide();
  KConfigGroup cfgGrp(d->m_instance->config(), "TemplateChooserDialog");
  QString fullTemplateName = cfgGrp.readPathEntry("FullTemplateName");
  d->m_alwaysUseTemplate = cfgGrp.readEntry("AlwaysUseTemplate");
  connect(m_alwaysUseCheckBox, SIGNAL(clicked()), this, SLOT(alwaysUseClicked()));
  changePalette();

  if(kapp) {
    connect(kapp, SIGNAL(kdisplayPaletteChanged()), this, SLOT(changePalette()));
  }

  QString dontShow = "imperial";

  if(KGlobal::locale()->measureSystem() == KLocale::Imperial) {
    dontShow = "metric";
  }

  KListViewItem* selectItem = 0;

  for (KoTemplate* t = group->first(); t != 0L; t = group->next()) {
    if(t->isHidden() || (t->measureSystem() == dontShow))
      continue;

    QString listText = "<b>" + t->name() + "</b>";

    if(!t->description().isEmpty()) {
      listText += "<br>" + t->description();
    }

    KListViewItem* item = new KListViewItem(m_documentList, t->name(), t->description(), t->file());
    item->setPixmap(0, t->loadPicture(instance));

    if(d->m_alwaysUseTemplate == t->file()) {
      selectItem = item;
    } else if(!selectItem && ((t->file() == fullTemplateName) || (t == defaultTemplate))) {
      selectItem = item;
    }
  }

  connect(m_documentList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));
  connect(m_documentList, SIGNAL(doubleClicked(QListViewItem*, const QPoint&, int)),
          this, SLOT(openTemplate(QListViewItem*)));
  connect(m_documentList, SIGNAL(returnPressed(QListViewItem*)),
          this, SLOT(openTemplate(QListViewItem*)));
  connect(m_openButton, SIGNAL(clicked()), this, SLOT(openTemplate()));

  if(selectItem) {
    m_documentList->setSelected(selectItem, true);
    d->m_selected = true;
  } else {
    m_documentList->setSelected(m_documentList->firstChild(), true);
  }
}

KoTemplatesPane::~KoTemplatesPane()
{
  delete d;
}

void KoTemplatesPane::selectionChanged(QListViewItem* item)
{
  if(item) {
    m_openButton->setEnabled(true);
    m_alwaysUseCheckBox->setEnabled(true);
    m_titleLabel->setText(item->text(0));
    m_previewLabel->setPixmap(*(item->pixmap(0)));
    m_detailsLabel->setText(item->text(1));
    m_alwaysUseCheckBox->setChecked(item->text(2) == d->m_alwaysUseTemplate);
  } else {
    m_openButton->setEnabled(false);
    m_alwaysUseCheckBox->setEnabled(false);
    m_alwaysUseCheckBox->setChecked(false);
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
    KConfigGroup cfgGrp(d->m_instance->config(), "TemplateChooserDialog");
    cfgGrp.writePathEntry("FullTemplateName", item->text(2));
    cfgGrp.writeEntry("LastReturnType", "Template");
    cfgGrp.writeEntry("AlwaysUseTemplate", d->m_alwaysUseTemplate);
    kdDebug() << "AlwaysUseTemplate == " << d->m_alwaysUseTemplate << endl;
    emit openTemplate(item->text(2));
  }
}

void KoTemplatesPane::changePalette()
{
  QPalette p = kapp ? kapp->palette() : palette();
  p.setBrush(QColorGroup::Base, p.brush(QPalette::Normal, QColorGroup::Background));
  p.setColor(QColorGroup::Text, p.color(QPalette::Normal, QColorGroup::Foreground));
  m_detailsLabel->setPalette(p);
}

bool KoTemplatesPane::isSelected()
{
  return d->m_selected;
}

void KoTemplatesPane::alwaysUseClicked()
{
  QListViewItem* item = m_documentList->selectedItem();

  if(!m_alwaysUseCheckBox->isChecked()) {
    KConfigGroup cfgGrp(d->m_instance->config(), "TemplateChooserDialog");
    cfgGrp.writeEntry("AlwaysUseTemplate", "");
    d->m_alwaysUseTemplate = "";
  } else {
    d->m_alwaysUseTemplate = item->text(2);
  }

  emit alwaysUseChanged(this, d->m_alwaysUseTemplate);
}

void KoTemplatesPane::changeAlwaysUseTemplate(KoTemplatesPane* sender, const QString& alwaysUse)
{
  if(this == sender)
    return;

  QListViewItem* item = m_documentList->selectedItem();

  // If the old always use template is selected uncheck the checkbox
  if(item && (item->text(2) == d->m_alwaysUseTemplate)) {
    m_alwaysUseCheckBox->setChecked(false);
  }

  d->m_alwaysUseTemplate = alwaysUse;
}


class KoRecentDocumentsPanePrivate
{
  public:
    KoRecentDocumentsPanePrivate()
      : m_previewJob(0), m_instance(0)
    {
    }

    ~KoRecentDocumentsPanePrivate()
    {
      if(m_previewJob)
        m_previewJob->kill();
    }

    KIO::PreviewJob* m_previewJob;
    KInstance* m_instance;
};

KoRecentDocumentsPane::KoRecentDocumentsPane(QWidget* parent, KInstance* instance)
  : KoDetailsPaneBase(parent, "RecentDocsPane")
{
  d = new KoRecentDocumentsPanePrivate;
  d->m_instance = instance;
  KGuiItem openGItem(i18n("Open This Document"), "fileopen");
  m_openButton->setGuiItem(openGItem);
  m_alwaysUseCheckBox->hide();
  m_documentList->header()->hide();
  m_documentList->setSorting(-1); // Disable sorting
  changePalette();

  if(kapp) {
    connect(kapp, SIGNAL(kdisplayPaletteChanged()), this, SLOT(changePalette()));
  }

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
        KoFileListItem* item = new KoFileListItem(m_documentList,
            m_documentList->lastItem(), name, url.path(), fileItem);
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
    m_titleLabel->setText(item->text(0));
    m_previewLabel->setPixmap(*(item->pixmap(2)));

    if(static_cast<KoFileListItem*>(item)->fileItem()) {
      KFileItem* fileItem = static_cast<KoFileListItem*>(item)->fileItem();
      QString details = "<center><table border=\"0\">";
      details += "<tr><td><b>Modified:</b></td>";
      details += "<td>" + fileItem->timeString(KIO::UDS_MODIFICATION_TIME) + "</td></tr>";
      details += "<tr><td><b>Accessed:</b></td>";
      details += "<td>" + fileItem->timeString(KIO::UDS_ACCESS_TIME) + "</td></tr>";
      details += "</table></center>";
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
  KConfigGroup cfgGrp(d->m_instance->config(), "TemplateChooserDialog");
  cfgGrp.writeEntry("LastReturnType", "File");

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
  if(preview.isNull()) {
    return;
  }

  QListViewItemIterator it(m_documentList);

  while(it.current()) {
    if(it.current()->text(1) == fileItem->url().path()) {
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

void KoRecentDocumentsPane::changePalette()
{
  QPalette p = kapp ? kapp->palette() : palette();
  p.setBrush(QColorGroup::Base, p.brush(QPalette::Normal, QColorGroup::Background));
  p.setColor(QColorGroup::Text, p.color(QPalette::Normal, QColorGroup::Foreground));
  m_detailsLabel->setPalette(p);
}

#include "koDetailsPane.moc"
