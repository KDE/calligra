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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#if 0 // KWORD_HORIZONTAL_LINE
#include <klocale.h>
#include "kwdoc.h"
#include <qvbox.h>
#include <qwhatsthis.h>
#include <qlistbox.h>
#include "kwinserthorizontallinedia.h"
#include <kpixmapio.h>
#include <qdir.h>
#include <qlayout.h>
#include <qlabel.h>

KWinsertHorizontalLineDia::KWinsertHorizontalLineDia( KWDocument *_doc, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Insert Horizontal Line") );
    QVBox *page = makeVBoxMainWidget();
    QVBoxLayout *lay1 = new QVBoxLayout( page, 0, spacingHint() );

    QLabel *label = new QLabel( i18n("Select line type to insert:"), page );
    lay1->addWidget( label );

    m_listHorizLine = new QListBox( page );
    m_listHorizLine->insertItem( i18n("Simple Line"));
    QStringList path =  _doc->horizontalLinePath();
    for ( QStringList::Iterator it = path.begin(); it != path.end(); ++it )
    {
        QDir dir( *it );
        if ( dir.exists() )
        {
            QStringList tmp = dir.entryList("*.png");
            for ( QStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2 )
                m_files.append( QString( (*it) + (*it2)));
        }
    }
    for( QStringList::Iterator it = m_files.begin(); it != m_files.end(); ++it )
    {
        KPixmapIO io;
        QPixmap pix;
        pix.load( *it );
        m_listHorizLine->insertItem( new QListBoxPixmap ( pix ));
    }
    connect (m_listHorizLine, SIGNAL( doubleClicked ( QListBoxItem *)), this, SLOT( accept()));
    resize( 300, 200);
}

QString KWinsertHorizontalLineDia::horizontalLineName() const
{
    if ( m_listHorizLine->currentItem() !=-1)
    {
        if ( m_listHorizLine->currentItem()==0 )
            return QString::null;
        else
            return m_files[m_listHorizLine->currentItem()-1];
    }
    else
        return QString::null;
}


#include "kwinserthorizontallinedia.moc"
#endif
