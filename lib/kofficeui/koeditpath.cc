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

#include <kdeversion.h>
#include <klocale.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include "koeditpath.h"
#include <keditlistbox.h>
#include <kfiledialog.h>
#include <kurlrequester.h>
#include <qhbox.h>
#include <klineedit.h>

KoEditPathDia::KoEditPathDia( const QString & _path, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Edit Path") );
    QWidget *page = new QWidget( this );
    setMainWidget(page);
    QGridLayout * grid = new QGridLayout(page, 5, 2, KDialog::marginHint(), KDialog::spacingHint());

#if KDE_VERSION < 307

    m_listpath =  new KEditListBox( i18n("Expression path:"),
                    page, "list_editor" , false, KEditListBox::Add|KEditListBox::Remove );
#else
    urlReq = new KURLRequester();
    urlReq->fileDialog()->setMode(KFile::Directory | KFile::LocalOnly);

    KEditListBox::CustomEditor tmp(urlReq, urlReq->lineEdit());

    m_listpath =  new KEditListBox( i18n("Expression path:"),
                    tmp,page, "list_editor" , false, KEditListBox::Add|KEditListBox::Remove );
#endif

    grid->addMultiCellWidget(m_listpath, 0, 4, 0, 0);
    m_listpath->listBox()->insertStringList(QStringList::split(QString(";"), _path));
    setFocus();
    resize( 500, 300);
}

QString KoEditPathDia::newPath()const
{
    QString tmp;
    for (int i = 0; i <(int)m_listpath->listBox()->count(); i++)
    {
        if ( i!=0)
            tmp +=";";
        tmp += m_listpath->listBox()->text( i );
    }
    return tmp;
}

#include "koeditpath.moc"
