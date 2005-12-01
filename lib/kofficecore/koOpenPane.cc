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

#include "koOpenPane.h"

#include <qvbox.h>
#include <qlayout.h>
#include <qheader.h>
#include <qwidgetstack.h>
#include <qlabel.h>
#include <qvaluelist.h>

#include <klocale.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klistview.h>

#include "koFilterManager.h"
#include "koTemplates.h"
#include "koDocument.h"
#include "koDetailsPane.h"

class KoOpenPanePrivate
{
  public:
    KoOpenPanePrivate() :
      m_instance(0)
    {
    }

    KInstance* m_instance;
};

KoOpenPane::KoOpenPane(QWidget *parent, KInstance* instance, const QString& templateType)
  : KoOpenPaneBase(parent, "OpenPane")
{
  d = new KoOpenPanePrivate;
  d->m_instance = instance;

  m_sectionList->header()->hide();
  m_sectionList->setSorting(-1); // Disable sorting
  connect(m_sectionList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));

  KGuiItem openExistingGItem(i18n("Open Existing Document"), "fileopen");
  m_openExistingButton->setGuiItem(openExistingGItem);
  connect(m_openExistingButton, SIGNAL(clicked()), this, SLOT(showOpenFileDialog()));

  initRecentDocs();

  QListViewItem* separator = new QListViewItem(m_sectionList, m_sectionList->lastItem(), "");
  separator->setEnabled(false);

  initTemplates(templateType);

  QValueList<int> sizes;
  sizes << 20 << width() - 20;
  m_splitter->setSizes(sizes);
}

KoOpenPane::~KoOpenPane()
{
  delete d;
}

void KoOpenPane::showOpenFileDialog()
{
  const QStringList mimeFilter = KoFilterManager::mimeFilter(KoDocument::readNativeFormatMimeType(),
      KoFilterManager::Import, KoDocument::readExtraNativeMimeTypes());

  KURL url = KFileDialog::getOpenURL(":OpenDialog", mimeFilter.join(" "), this);

  if(!url.isEmpty()) {
    KConfigGroup cfgGrp(d->m_instance->config(), "TemplateChooserDialog");
    cfgGrp.writeEntry("LastReturnType", "File");
    emit openExistingFile(url.path());
  }
}

void KoOpenPane::initRecentDocs()
{
  KoRecentDocumentsPane* recentDocPane = new KoRecentDocumentsPane(this, d->m_instance);
  connect(recentDocPane, SIGNAL(openFile(const QString&)), this, SIGNAL(openExistingFile(const QString&)));
  KListViewItem* item = addPane(i18n("Recent Documents"), "fileopen", recentDocPane);

  if(d->m_instance->config()->hasGroup("RecentFiles")) {
    m_sectionList->setSelected(item, true);
  }
}

void KoOpenPane::initTemplates(const QString& templateType)
{
  KListViewItem* selectItem = 0;
  KListViewItem* firstItem = 0;

  if(!templateType.isEmpty())
  {
    KoTemplateTree templateTree(templateType.local8Bit(), d->m_instance, true);

    for (KoTemplateGroup *group = templateTree.first(); group != 0L; group = templateTree.next()) {
      if (group->isHidden()) {
        continue;
      }

      KoTemplatesPane* pane = new KoTemplatesPane(this, d->m_instance, group);
      connect(pane, SIGNAL(openTemplate(const QString&)), this, SIGNAL(openTemplate(const QString&)));
      connect(pane, SIGNAL(alwaysUseChanged(KoTemplatesPane*, const QString&)),
              this, SIGNAL(alwaysUseChanged(KoTemplatesPane*, const QString&)));
      connect(this, SIGNAL(alwaysUseChanged(KoTemplatesPane*, const QString&)),
              pane, SLOT(changeAlwaysUseTemplate(KoTemplatesPane*, const QString&)));
      KListViewItem* item = addPane(group->name(), group->first()->loadPicture(d->m_instance), pane);

      if(!firstItem)
        firstItem = item;

      if(pane->isSelected()) {
        selectItem = item;
      }
    }
  }

  KConfigGroup cfgGrp(d->m_instance->config(), "TemplateChooserDialog");

  if(selectItem && (cfgGrp.readEntry("LastReturnType") == "Template")) {
    m_sectionList->setSelected(selectItem, true);
  } else if(!m_sectionList->selectedItem() && firstItem) {
    m_sectionList->setSelected(firstItem, true);
  }
}

KListViewItem* KoOpenPane::addPane(const QString& title, const QString& icon, QWidget* widget)
{
  return addPane(title, SmallIcon(icon, KIcon::SizeLarge, KIcon::DefaultState, d->m_instance), widget);
}

KListViewItem* KoOpenPane::addPane(const QString& title, const QPixmap& icon, QWidget* widget)
{
  if(!widget) {
    return 0;
  }

  KListViewItem* listItem = new KListViewItem(m_sectionList, m_sectionList->lastItem(), title);

  if(!icon.isNull()) {
    listItem->setPixmap(0, icon);
  }

  int id = m_widgetStack->addWidget(widget);
  listItem->setText(1, QString::number(id));

  return listItem;
}

void KoOpenPane::selectionChanged(QListViewItem* item)
{
  m_headerLabel->setText(item->text(0));
  m_widgetStack->raiseWidget(item->text(1).toInt());
}


#include "koOpenPane.moc"
