/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include <qlayout.h>
#include <klocale.h>
#include "stencil_text_dlg.h"

KivioStencilTextDlg::KivioStencilTextDlg( QWidget *p, const QString &initText )
    : KDialogBase(p,"Kivio Stencil Text Dialog", true, i18n("Stencil Text"),
                  KDialogBase::Ok | KDialogBase::Cancel)
{
    m_text = new QMultiLineEdit( this, "multilineedit" );
    m_text->setText(initText);
    setMainWidget(m_text);

    m_text->setFocus();
 
    resize(350,200);
}

KivioStencilTextDlg::~KivioStencilTextDlg()
{
}

QString KivioStencilTextDlg::text()
{
    return m_text->text();
}

