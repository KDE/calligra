/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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
#include <qvbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include "koimportstyledia.h"

#include <kdebug.h>
#include <qlabel.h>

KoImportStyleDia::KoImportStyleDia( const QStringList &_list, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel|User1, Ok, true )
{
    setCaption( i18n("Import Style") );
    m_list =_list;
    QVBox *page = makeVBoxMainWidget();
    new QLabel(i18n("Select Style to import:"), page);
    m_listStyleName = new QListBox( page );
    m_listStyleName->setSelectionMode( QListBox::Multi );
    enableButtonOK( (m_listStyleName->count()!=0) );
    setButtonText( KDialogBase::User1, i18n("Load") );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotLoadFile()));
    resize (300, 400);
}

KoImportStyleDia::~KoImportStyleDia()
{
}

void KoImportStyleDia::slotLoadFile()
{
    loadFile();
}

void KoImportStyleDia::loadFile()
{
    m_styleList.setAutoDelete(true);
    m_styleList.clear();
}

void KoImportStyleDia::initList()
{
    QStringList lst;
    for ( KoStyle * p = m_styleList.first(); p != 0L; p = m_styleList.next() )
    {
        lst<<p->translatedName();
    }
    m_listStyleName->insertStringList(lst);
}

void KoImportStyleDia::slotOk()
{
    generateStyleList();
    KDialogBase::slotOk();
}

QString KoImportStyleDia::generateStyleName( const QString & templateName )
{
    QString name;
    int num = 1;
    bool exists;
    do {
        name = templateName.arg( num );
        exists = (m_list.findIndex( name )!=-1);
        ++num;
    } while ( exists );
    return name;
}


#include "koimportstyledia.moc"
