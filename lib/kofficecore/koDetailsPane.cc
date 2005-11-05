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
#include <qtextedit.h>

#include <kinstance.h>
#include <klocale.h>
#include <klistview.h>
#include <kpushbutton.h>

#include "koTemplates.h"

KoTemplatesPane::KoTemplatesPane(QWidget* parent, KInstance* instance, KoTemplateGroup *group)
  : KoDetailsPaneBase(parent, "TemplatesPane")
{
    m_openButton->setText(i18n("&Use This Template"));
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
  m_detailsText->setText(item->text(1));
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


KoRecentDocumentsPane::KoRecentDocumentsPane(QWidget* parent, KInstance* instance)
  : KoDetailsPaneBase(parent, "RecentDocsPane")
{
    m_openButton->setText(i18n("&Open"));
    m_documentList->setColumnText (0, i18n("Documents"));
    m_alwaysUseCheckbox->hide();
}

#include "koDetailsPane.moc"
