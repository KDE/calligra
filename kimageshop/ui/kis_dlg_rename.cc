/*
 *  kis_dlg_rename.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter <elter@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qhbox.h>
#include <qlabel.h>
#include <qstring.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include <kstddirs.h>
#include <klocale.h>

#include "kis_dlg_rename.h"

KisDlgRename::KisDlgRename(QString oldname, QWidget *parent, 
    const char *name, bool modal, WFlags f)
  : KDialog ( parent, name, modal, f)
{
    QGridLayout *layout = new QGridLayout( this, 3, 2, 15, 7 );

    m_pEdit = new QLineEdit( oldname, this );
    layout->addWidget( m_pEdit, 0, 1 );

    QLabel *label1 = new QLabel( m_pEdit, i18n( "Name" ), this );
    layout->addWidget( label1, 0, 0 );

    layout->setRowStretch( 1, 1 );

    QHBox *buttons = new QHBox( this );
    layout->addMultiCellWidget( buttons, 2, 2, 0, 1 );

    (void) new QWidget( buttons );

    QPushButton *pbOk = new QPushButton( i18n( "OK" ), buttons );
    pbOk->setDefault( true );
    QObject::connect( pbOk, SIGNAL( clicked() ), this, SLOT( accept() ) );

    QPushButton *pbCancel = new QPushButton( i18n( "Cancel" ), buttons );
    QObject::connect( pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    m_pEdit->setFocus();
}


QString KisDlgRename::name()
{
    return m_pEdit->text();
}

#include "kis_dlg_rename.moc"
