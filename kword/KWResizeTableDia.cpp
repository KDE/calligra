/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2005 Thomas Zander <zander@kde.org>

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

#include "KWDocument.h"
#include "KWResizeTableDia.h"
#include "KWTableFrameSet.h"
#include "KWCommand.h"

#include <KoUnitWidgets.h>
#include <klocale.h>
#include <QLabel>
#include <kvbox.h>

KWResizeTableDia::KWResizeTableDia( QWidget *parent, KWTableFrameSet *table, KWDocument *doc, int resizeColumn )
    : KDialog( parent)
{
    setButtonText( KDialogBase::User1, i18n("Reset") );
    setCaption( i18n( "Resize Column" ) );
    setButtons( Ok | Cancel | User1 | Apply );
    setDefaultButton( Ok );
    m_table = table;
    m_doc = doc;

    setupTab1(resizeColumn);
}

void KWResizeTableDia::setupTab1(int resizeColumn) {
#if 0
    KVBox *page = makeVBoxMainWidget();
    QLabel *rc = new QLabel( i18n( "Column:" ), page );
    rc->resize( rc->sizeHint() );
    rc->setAlignment( Qt::AlignLeft | Qt::AlignBottom );
    m_value = new QSpinBox( 1, m_table->getColumns(), 1, page );
    m_value->resize( m_value->sizeHint() );
    m_value->setValue( resizeColumn );

    rc = new QLabel( i18n( "Width:" ), page );
    m_position= new KoUnitDoubleSpinBox( page, 0.01, m_table->anchorFrameset()->isFloating() ? m_table->anchorFrameset()->frame(0)->width(): 9999, 1, 0.0, m_doc->unit(), m_doc->unit() );
    slotValueChanged( m_value->value());
    connect( m_value, SIGNAL( valueChanged ( int )), this, SLOT( slotValueChanged( int )));
#endif
}

bool KWResizeTableDia::doResize()
{
#if 0
    unsigned int resize= m_value->value() - 1;
    KWFrame *frm = m_table->cell( 0, resize )->frame(0);
    if (frm) {
        FrameIndex index( frm );
        KoRect newRect( frm->normalize() );
        newRect.setWidth( m_position->value() );
        FrameResizeStruct resizeStruct( frm->normalize(), frm->minimumFrameHeight(), newRect );
        KWFrameResizeCommand * cmd =new KWFrameResizeCommand( i18n("Resize Column"), index, resizeStruct );
        cmd->execute();
        m_doc->addCommand( cmd );
    }
    return true;
#endif
}

void KWResizeTableDia::slotValueChanged( int pos)
{
#if 0
    KWFrame *frm = m_table->cell( 0, pos-1 )->frame(0);
    if (frm) {
        m_position->setValue( KoUnit::toUserValue( qMax(0.00, frm->normalize().width()), m_doc->unit() ) );
        m_resetValue = m_position->value();
    }
#endif
}

void KWResizeTableDia::slotUser1()
{
    m_position->setValue( KoUnit::toUserValue(m_resetValue, m_doc->unit() ) );
    doResize();
}

void KWResizeTableDia::slotApply()
{
    doResize();
}

void KWResizeTableDia::slotOk()
{
   if (doResize())
   {
      KDialogBase::slotOk();
   }
}

#include "KWResizeTableDia.moc"
