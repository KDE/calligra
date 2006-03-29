/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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

#include <float.h>

#include <qlabel.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>

#include <knuminput.h>

#include <KoUnit.h>
#include <KoUnitWidgets.h>

#include <kspread_canvas.h>
#include <kspread_doc.h>
#include <kspread_global.h>
#include <kspread_locale.h>
#include <kspread_sheet.h>
#include <kspread_undo.h>
#include <kspread_view.h>
#include "manipulator.h"
#include "selection.h"

#include "kspread_dlg_resize2.h"

using namespace KSpread;

ResizeRow::ResizeRow( View* parent, const char* name )
	: KDialogBase( parent, name, true, i18n("Resize Row"), Ok|Cancel|Default )
{
    m_pView = parent;

    QWidget *page = new QWidget( this );
    setMainWidget( page );

    Q3VBoxLayout *vLay = new Q3VBoxLayout( page, 0, spacingHint() );
    Q3HBoxLayout *hLay = new Q3HBoxLayout( vLay );

    QRect selection( m_pView->selectionInfo()->selection() );
    RowFormat* rl = m_pView->activeSheet()->rowFormat( selection.top() );
    rowHeight = rl->dblHeight();

    QLabel * label1 = new QLabel( page, "label1" );
    label1->setText( i18n( "Height:" ) );
    hLay->addWidget( label1 );

    m_pHeight = new KoUnitDoubleSpinBox( page );
    m_pHeight->setValue( rowHeight );
    m_pHeight->setUnit( m_pView->doc()->unit() );

    hLay->addWidget( m_pHeight );

    QWidget *hSpacer = new QWidget( page );
    hSpacer->setMinimumSize( spacingHint(), spacingHint() );
    hLay->addWidget( hSpacer );

    QWidget *vSpacer = new QWidget( page );
    vSpacer->setMinimumSize( spacingHint(), spacingHint() );
    vLay->addWidget( vSpacer );

    m_pHeight->setFocus();

    //store the visible value, for later check for changes
    rowHeight = m_pHeight->value();
}

void ResizeRow::slotOk()
{
  double height = m_pHeight->value();

  //Don't generate a resize, when there isn't a change or the change is only a rounding issue
  if ( fabs( height - rowHeight ) > DBL_EPSILON )
  {
    ResizeRowManipulator* manipulator = new ResizeRowManipulator();
    manipulator->setSheet(m_pView->activeSheet());
    manipulator->setSize(height);
    // TODO Stefan:
    manipulator->setOldSize(rowHeight);
    manipulator->add(*m_pView->selectionInfo());
    manipulator->execute();
  }
  accept();
}

void ResizeRow::slotDefault()
{
    m_pHeight->setValue( POINT_TO_MM( heightOfRow ) );
}

ResizeColumn::ResizeColumn( View* parent, const char* name )
	: KDialogBase( parent, name, true, i18n("Resize Column"), Ok|Cancel|Default )
{
    m_pView = parent;

    QWidget *page = new QWidget( this );
    setMainWidget(page);

    Q3VBoxLayout *vLay = new Q3VBoxLayout( page, 0, spacingHint() );
    Q3HBoxLayout *hLay = new Q3HBoxLayout( vLay );

    QRect selection( m_pView->selectionInfo()->selection() );
    ColumnFormat* cl = m_pView->activeSheet()->columnFormat( selection.left() );
    columnWidth = cl->dblWidth();

    QLabel * label1 = new QLabel( page, "label1" );
    label1->setText( i18n( "Width:" ) );
    hLay->addWidget( label1 );

    m_pWidth = new KoUnitDoubleSpinBox( page );
    m_pWidth->setValue( columnWidth );
    m_pWidth->setUnit( m_pView->doc()->unit() );

    hLay->addWidget( m_pWidth );

    QWidget *hSpacer = new QWidget( page );
    hSpacer->setMinimumSize( spacingHint(), spacingHint() );
    hLay->addWidget( hSpacer );

    QWidget *vSpacer = new QWidget( page );
    vSpacer->setMinimumSize( spacingHint(), spacingHint() );
    vLay->addWidget( vSpacer );

    m_pWidth->setFocus();

    //store the visible value, for later check for changes
    columnWidth = m_pWidth->value();
}

void ResizeColumn::slotOk()
{
  double width = m_pWidth->value();

  //Don't generate a resize, when there isn't a change or the change is only a rounding issue
  if ( fabs( width - columnWidth ) > DBL_EPSILON )
  {
    ResizeColumnManipulator* manipulator = new ResizeColumnManipulator();
    manipulator->setSheet(m_pView->activeSheet());
    manipulator->setSize(width);
    // TODO Stefan:
    manipulator->setOldSize(columnWidth);
    manipulator->add(*m_pView->selectionInfo());
    manipulator->execute();
  }
  accept();
}

void ResizeColumn::slotDefault()
{
    m_pWidth->setValue( POINT_TO_MM(colWidth ) );
}


#include "kspread_dlg_resize2.moc"
