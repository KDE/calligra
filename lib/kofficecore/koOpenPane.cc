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

#include <klocale.h>
#include <kdeversion.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kdebug.h>

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
  : KJanusWidget(parent, "OpenPane", TreeList)
{
  d = new KoOpenPanePrivate;
  d->m_instance = instance;

  addButtonBelowList(i18n("Open Existing Document"), this, SLOT(showOpenFileDialog()));
  setRootIsDecorated(false);
  setShowIconsInTreeList(true);

  QVBox* page = addVBoxPage(i18n("Recent Documents"), i18n("Recent Documents"),
                            SmallIcon("fileopen", 48, KIcon::DefaultState, instance));
  KoRecentDocumentsPane* recentDocPane = new KoRecentDocumentsPane(page, instance);
  connect(recentDocPane, SIGNAL(openFile(const QString&)), this, SIGNAL(openExistingFile(const QString&)));

  //kdDebug() << "Template type: " << templateType << endl;

  if(!templateType.isEmpty())
  {
    KoTemplateTree templateTree(templateType.local8Bit(), instance, true);

    for (KoTemplateGroup *group = templateTree.first(); group != 0L; group = templateTree.next()) {
      if (group->isHidden()) {
        continue;
      }

      page = addVBoxPage(group->name(), group->name(),
                         group->first()->loadPicture(instance));
      KoTemplatesPane* pane = new KoTemplatesPane(page, instance, group);
      connect(pane, SIGNAL(openTemplate(const QString&)), this, SIGNAL(openTemplate(const QString&)));
    }
  }
}

KoOpenPane::~KoOpenPane()
{
  delete d;
}

void KoOpenPane::showOpenFileDialog()
{
  const QStringList mimeFilter = KoFilterManager::mimeFilter(KoDocument::readNativeFormatMimeType(),
      KoFilterManager::Import,
      KoDocument::readExtraNativeMimeTypes());

  QString filename = KFileDialog::getOpenFileName(":OpenDialog", mimeFilter.join(" "), this);

  emit openExistingFile(filename);
}

// KoOpenPane::addTemplatesPanel(KoTemplateGroup *group)
// {
//     /*
//         instantiate new koTemplatesPane and add it to the m_widgetStack and a row to
//         the m_itemsList at position last, unless the customDocumentCreater is already
//         added, then its at position last - 1
//     */
// }
// 
// KoOpenPane::addCustomDocumentCreator(KoCustomDocumentCreator *cdc)
// {
//     /*
//     if (dynamic_cast(QWidget*) <cdc>) == 0) {
//         print error and return
//     }
// 
//     add it to the m_widgetStack and a row to the m_itemsList at position last
//     */
// }

#include "koOpenPane.moc"
