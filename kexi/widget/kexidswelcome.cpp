/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#include <qlayout.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kglobalsettings.h>
#include <kstandardguiitem.h>
#include <kdeversion.h>

#include "kexidatasourcewizard.h"
#include "kexidswelcome.h"

KexiDSWelcome::KexiDSWelcome(KexiDataSourceWizard *parent)
 : QWidget(parent)
{
  m_wiz = parent;
  KexiDSPixmap *pic = new KexiDSPixmap(this);

  QLabel *lText = new QLabel(i18n("Kexi can help you with creation of %2 using data sources in almost no time with the \"%1 Wizard\""), this);
  lText->setAlignment(AlignTop | AlignLeft | WordBreak);
  QCheckBox *useWizard = new QCheckBox(i18n("Create %1 using the \"%1 Wizard\""), this);
  connect(useWizard, SIGNAL(toggled(bool)), this, SLOT(setUseWizard(bool)));
  useWizard->setChecked(true);

  QSpacerItem *spacer = new QSpacerItem(320, 220);
  QCheckBox *dontShow = new QCheckBox(i18n("Do not show this wizard again"), this);

  QGridLayout *g = new QGridLayout(this);

  g->addMultiCellWidget(pic, 0, 4, 0, 0);
  g->addWidget(lText, 0, 1);
  g->addWidget(useWizard, 2, 1);
  g->addItem(spacer, 3, 1);
  g->addWidget(dontShow, 4, 1);
}

void
KexiDSWelcome::setUseWizard(bool use)
{
#if !defined(Q_WS_WIN)
  bool useIcons = KGlobalSettings::showIconsOnPushButtons();
#endif
  if(use)
  {
    KGuiItem forward = KStandardGuiItem::forward(KStandardGuiItem::UseRTL);

    if(useIcons)
      m_wiz->nextButton()->setIconSet( forward.iconSet() );

    m_wiz->nextButton()->setText(i18n("&Next"));
  }
  else
  {
    if(useIcons)
      m_wiz->nextButton()->setIconSet(KIcon("dialog-ok"));

    m_wiz->nextButton()->setText(i18n("&Finish"));
  }

  m_wiz->finishNext(!use);
}

KexiDSWelcome::~KexiDSWelcome()
{
}

#include "kexidswelcome.moc"

