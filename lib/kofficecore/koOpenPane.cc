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

#include <kjanuswidget.h>
#include <klocale.h>
#include <kdeversion.h>
#include <kfiledialog.h>
#include <kinstance.h>
#include <kpushbutton.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kdialog.h>

#include "koFilterManager.h"
#include "koTemplates.h"
#include "koDocument.h"
#include "koDetailsPane.h"

class KoOpenPanePrivate
{
  public:
    KoOpenPanePrivate() :
      m_instance(0),
      m_mainWidget(0)
    {
    }

    KInstance* m_instance;
    KJanusWidget* m_mainWidget;
};

KoOpenPane::KoOpenPane(QWidget *parent, KInstance* instance, const QString& templateType)
  : QWidget(parent, "OpenPane")
{
  d = new KoOpenPanePrivate;
  d->m_instance = instance;

  QVBoxLayout* layout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
  layout->setAutoAdd(true);
  d->m_mainWidget = new KJanusWidget(this,"OpenPane", KJanusWidget::TreeList);

  KGuiItem openExistingGItem(i18n("Open Existing Document"), "fileopen");
  d->m_mainWidget->addButtonBelowList(openExistingGItem, this, SLOT(showOpenFileDialog()));
  d->m_mainWidget->setRootIsDecorated(false);
  d->m_mainWidget->setShowIconsInTreeList(true);

  QVBox* page = d->m_mainWidget->addVBoxPage(i18n("Recent Documents"), i18n("Recent Documents"),
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

      page = d->m_mainWidget->addVBoxPage(group->name(), group->name(),
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

  KURL url = KFileDialog::getOpenURL(":OpenDialog", mimeFilter.join(" "), this);

  emit openExistingFile(url.path());
}

void KoOpenPane::addCustomDocumentPane(const QString& title, const QString& icon, QWidget* widget)
{
  QVBox* page = d->m_mainWidget->addVBoxPage(title, title, SmallIcon(icon,
                                             48, KIcon::DefaultState, d->m_instance));
  widget->reparent(page, QPoint(0, 0));
}

#include "koOpenPane.moc"
