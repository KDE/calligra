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

#include <klocale.h>
#include <kinstance.h>

#include <qlayout.h>
#include <qfile.h>
#include <qvbox.h>
#include <kdebug.h>
#include <kstddirs.h>

#include <qlabel.h>
#include <qlineedit.h>

#include "kwinsertlink.h"


KWInsertLinkDia::KWInsertLinkDia( QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, i18n("Insert Link: "), Ok|Cancel, Ok, true )
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QGridLayout *grid = new QGridLayout( page, 4, 1, KDialog::marginHint(), KDialog::spacingHint() );

    QLabel *lab=new QLabel(i18n( "Link name:" ), page );
    grid->addWidget(lab,0,0);
    m_linkName= new QLineEdit(page);
    grid->addWidget(m_linkName,1,0);

    lab=new QLabel(i18n( "Reference name:" ), page );
    grid->addWidget(lab,2,0);

    m_hrefName= new QLineEdit(page);
    grid->addWidget(m_hrefName,3,0);
    connect(m_hrefName,SIGNAL(textChanged ( const QString & )),this,SLOT(slotTextChanged ( const QString & )));
    connect(m_linkName,SIGNAL(textChanged ( const QString & )),this,SLOT(slotTextChanged ( const QString & )));

    enableButtonOK( !(m_linkName->text().isEmpty()  || m_hrefName->text().isEmpty()));
    m_linkName->setFocus();
    resize(300,200);
}

void KWInsertLinkDia::slotTextChanged ( const QString & )
{
    enableButtonOK( !(m_linkName->text().isEmpty()  || m_hrefName->text().isEmpty()));
}

bool KWInsertLinkDia::createLinkDia(QString & _linkName, QString & _hrefName)
{
    bool res = false;

    KWInsertLinkDia *dlg = new KWInsertLinkDia( 0L, "Insert Link" );
    if ( dlg->exec() == Accepted )
    {
        _linkName = dlg->linkName();
        _hrefName = dlg->hrefName();
        res = true;
    }
    delete dlg;

    return res;
}

QString KWInsertLinkDia::linkName()
{
    return  m_linkName->text();
}

QString KWInsertLinkDia::hrefName()
{
    return  m_hrefName->text();
}

void KWInsertLinkDia::slotOk()
{
    KDialogBase::slotOk();
}



#include "kwinsertlink.moc"
