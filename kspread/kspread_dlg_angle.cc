/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000, 2001 Montel Laurent <lmontel@mandrakesoft.com>
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kspread_dlg_angle.h"
#include "kspread_view.h"
#include "kspread_table.h"
#include <qlayout.h>

#include <kbuttonbox.h>
#include <klocale.h>
#include <kdebug.h>
#include <knuminput.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

KSpreadAngle::KSpreadAngle( KSpreadView* parent, const char* name,const QPoint &_marker)
	: KDialogBase( parent, name,TRUE,i18n("Change Angle" ), Ok|Cancel)
{

  m_pView=parent;
  marker=_marker;
  QWidget *page = new QWidget( this );
  setMainWidget(page);
  QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );
  KSpreadCell *cell = m_pView->activeTable()->cellAt( marker.x(), marker.y() );
  int size=-(cell->getAngle(marker.x(), marker.y()));
  m_pSize2=new KIntNumInput(size, page, 10);
  m_pSize2->setRange(-90, 90, 1);
  m_pSize2->setLabel(i18n("Angle:"));
  m_pSize2->setSuffix(" °");
  lay1->addWidget(m_pSize2);
  m_pDefault=new QCheckBox(i18n("Default (0Â°)"),page);
  lay1->addWidget(m_pDefault);

  lay1->activate();
  m_pSize2->setFocus();
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( m_pDefault, SIGNAL(clicked() ),this, SLOT(slotChangeState()));

}

void KSpreadAngle::slotChangeState()
{
    m_pSize2->setEnabled(!m_pDefault->isChecked());
}

void KSpreadAngle::slotOk()
{
    if(!m_pDefault->isChecked())
        m_pView->activeTable()->setSelectionAngle(m_pView->selectionInfo(),
                                                  -m_pSize2->value());
    else
        m_pView->activeTable()->setSelectionAngle(m_pView->selectionInfo(), 0);
    accept();
}


#include "kspread_dlg_angle.moc"
