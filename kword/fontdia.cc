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

#include "fontdia.h"
#include "kwdoc.h"

#include <kapp.h>
#include <kfontdialog.h>
#include <kbuttonbox.h>

#include <qlayout.h>
#include <qdialog.h>
#include <qvbox.h>
#include <klocale.h>

KWFontDia::KWFontDia( QWidget* parent, const char* name, const QFont &_font,  bool _subscript,bool _superscript,bool _withSubSuperScript)
    :QDialog( parent, name, true )
{
    newFont=_font;
    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( 5 );
    lay1->setSpacing( 10 );
    m_chooseFont=new KFontChooser(this);
    m_chooseFont->setFont(_font);
    lay1->addWidget(m_chooseFont);

    QGroupBox *grp=new QGroupBox(this);
    lay1->addWidget(grp);
    QGridLayout *grid=new QGridLayout( grp, 2, 2, 15, 7 );

    m_underline=new QCheckBox(i18n("Underline"),grp);
    m_underline->setChecked(_font.underline());
    grid->addWidget(m_underline,0,1);


    m_superScript=new QCheckBox(i18n("SuperScript"),grp);
    m_superScript->setChecked(_superscript);
    grid->addWidget(m_superScript,0,0);

    m_subScript=new QCheckBox(i18n("SubScript"),grp);
    m_subScript->setChecked( _subscript);
    grid->addWidget(m_subScript,1,0);

    if(!_withSubSuperScript)
    {
        m_subScript->setEnabled(false);
        m_superScript->setEnabled(false);
    }
    m_strikeOut=new QCheckBox(i18n("Strike Out"),grp);
    m_strikeOut->setChecked( _font.strikeOut());
    grid->addWidget(m_strikeOut,1,1);

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

    connect( m_underline,SIGNAL(clicked ()), this, SLOT( slotUnderlineClicked() ) );
    connect( m_strikeOut ,SIGNAL(clicked ()), this, SLOT( slotStrikeOutClicked() ) );

    connect( m_subScript,SIGNAL(clicked ()), this, SLOT( slotSubScriptClicked() ) );
    connect( m_superScript ,SIGNAL(clicked ()), this, SLOT( slotSuperScriptClicked() ) );
    connect( m_chooseFont,SIGNAL( fontSelected( const QFont & )), this,SLOT(slotFontChanged(const QFont &)));
}

void KWFontDia::slotFontChanged(const QFont &_f)
{
    newFont=_f;
}

void KWFontDia::slotUnderlineClicked()
{
    newFont.setUnderline(m_underline->isChecked());
    m_chooseFont->setFont(newFont);
}

void KWFontDia::slotStrikeOutClicked()
{
    newFont.setStrikeOut(m_strikeOut->isChecked());
    m_chooseFont->setFont(newFont);
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

#include "fontdia.moc"
