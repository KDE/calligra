/* This file is part of the KDE project
   Copyright (C)  2001 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <qgroupbox.h>

#include "kwfont.h"
#include "kwview.h"
#include "kwdoc.h"

#include <kiconloader.h>
#include <kapp.h>
#include <kfontdialog.h>
#include <kbuttonbox.h>

#include <qlayout.h>
#include <qdialog.h>
#include <qvbox.h>
#include <qpushbutton.h>


KWFontDia::KWFontDia( KWView* parent, const char* name, const QFont &_font, bool _underline, bool _subscript,bool _superscript, bool _strikeOut)
    :QDialog( parent, name, true )
{
    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );
    m_chooseFont=new KFontChooser(this);
    m_chooseFont->setFont(_font);
    lay1->addWidget(m_chooseFont);
    m_underline=new QCheckBox(i18n("Underline"),this);
    m_underline->setChecked(_underline);
    lay1->addWidget(m_underline);


    m_superScript=new QCheckBox(i18n("SuperScript"),this);
    m_superScript->setChecked(_superscript);
    lay1->addWidget(m_superScript);

    m_subScript=new QCheckBox(i18n("SubScript"),this);
    m_subScript->setChecked( _subscript);
    lay1->addWidget(m_subScript);

    m_strikeOut=new QCheckBox(i18n("Strike Out"),this);
    m_strikeOut->setChecked( _strikeOut);
    lay1->addWidget(m_strikeOut);

    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    QPushButton *m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    QPushButton *m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    lay1->addWidget( bb );
    lay1->activate();
    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotCancel() ) );
    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );

    connect( m_subScript,SIGNAL(clicked ()), this, SLOT( slotSubScriptClicked() ) );
    connect( m_superScript ,SIGNAL(clicked ()), this, SLOT( slotSuperScriptClicked() ) );
}

void KWFontDia::slotSubScriptClicked()
{
    if(m_superScript->isChecked())
        m_superScript->setChecked(false);
}

void KWFontDia::slotSuperScriptClicked()
{
    if(m_subScript->isChecked())
        m_subScript->setChecked(false);
}

void KWFontDia::slotOk()
{
    emit okClicked();
    accept();
}

void KWFontDia::slotCancel()
{
    reject();
}

#include "kwfont.moc"
