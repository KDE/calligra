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

class KoSectionListItem : public QListViewItem
{
  public:
    KoSectionListItem(KListView* listView, const QString& name, int sortWeight, int widgetIndex = -1)
      : QListViewItem(listView, name), m_sortWeight(sortWeight), m_widgetIndex(widgetIndex)
    {
    }

    virtual int compare(QListViewItem* i, int, bool) const
    {
      KoSectionListItem* item = dynamic_cast<KoSectionListItem*>(i);

      if(!item)
        return 0;

      return sortWeight() - item->sortWeight();
    }

    int sortWeight() const { return m_sortWeight; }
    int widgetIndex() const { return m_widgetIndex; }

  private:
    int m_sortWeight;
    int m_widgetIndex;
};

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
  m_sectionList->setSorting(0);
  connect(m_sectionList, SIGNAL(selectionChanged(QListViewItem*)),
          this, SLOT(selectionChanged(QListViewItem*)));

  KGuiItem openExistingGItem(i18n("Open Existing Document"), "fileopen");
  m_openExistingButton->setGuiItem(openExistingGItem);
  connect(m_openExistingButton, SIGNAL(clicked()), this, SLOT(showOpenFileDialog()));

  initRecentDocs();
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
  QListViewItem* item = addPane(i18n("Recent Documents"), "fileopen", recentDocPane, 0);

  KoSectionListItem* separator = new KoSectionListItem(m_sectionList, "", 1);
  separator->setEnabled(false);

  if(d->m_instance->config()->hasGroup("RecentFiles")) {
    m_sectionList->setSelected(item, true);
  }
}

void KoOpenPane::initTemplates(const QString& templateType)
{
  QListViewItem* selectItem = 0;
  QListViewItem* firstItem = 0;
  int templateOffset = 1000;

  if(!templateType.isEmpty())
  {
    KoTemplateTree templateTree(templateType.local8Bit(), d->m_instance, true);

    for (KoTemplateGroup *group = templateTree.first(); group != 0L; group = templateTree.next()) {
      if (group->isHidden()) {
        continue;
      }

      KoTemplatesPane* pane = new KoTemplatesPane(this, d->m_instance,
          group, templateTree.defaultTemplate());
      connect(pane, SIGNAL(openTemplate(const QString&)), this, SIGNAL(openTemplate(const QString&)));
      connect(pane, SIGNAL(alwaysUseChanged(KoTemplatesPane*, const QString&)),
              this, SIGNAL(alwaysUseChanged(KoTemplatesPane*, const QString&)));
      connect(this, SIGNAL(alwaysUseChanged(KoTemplatesPane*, const QString&)),
              pane, SLOT(changeAlwaysUseTemplate(KoTemplatesPane*, const QString&)));
      QListViewItem* item = addPane(group->name(), group->first()->loadPicture(d->m_instance),
                                    pane, group->sortingWeight() + templateOffset);

      if(!firstItem) {
        firstItem = item;
      }

      if(group == templateTree.defaultGroup()) {
        firstItem = item;
      }

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

QListViewItem* KoOpenPane::addPane(const QString& title, const QString& icon, QWidget* widget, int sortWeight)
{
  return addPane(title, SmallIcon(icon, KIcon::SizeLarge, KIcon::DefaultState, d->m_instance),
                 widget, sortWeight);
}

QListViewItem* KoOpenPane::addPane(const QString& title, const QPixmap& icon, QWidget* widget, int sortWeight)
{
  if(!widget) {
    return 0;
  }

  int id = m_widgetStack->addWidget(widget);
  KoSectionListItem* listItem = new KoSectionListItem(m_sectionList, title, sortWeight, id);

  if(!icon.isNull()) {
    listItem->setPixmap(0, icon);
  }

  return listItem;
}

void KoOpenPane::selectionChanged(QListViewItem* item)
{
  KoSectionListItem* section = dynamic_cast<KoSectionListItem*>(item);

  if(!item)
    return;

  m_headerLabel->setText(section->text(0));
  m_widgetStack->raiseWidget(section->widgetIndex());
}


#include "koOpenPane.moc"
