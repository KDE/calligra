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
#include "kwdoc.h"
#include <qvbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <kmessagebox.h>
#include "kwimportstyledia.h"
#include <koStore.h>
#include <qfile.h>
#include <kfiledialog.h>
#include <kdebug.h>
#include <qlabel.h>

KWImportStyleDia::KWImportStyleDia( KWDocument *_doc, const QStringList &_list, StyleType _type, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Import Style") );
    m_doc=_doc;
    m_typeStyle = _type;
    m_list =_list;
    QVBox *page = makeVBoxMainWidget();
    QLabel *lab = new QLabel(i18n("Select Style to import:"), page);
    m_listStyleName = new QListBox( page );
    m_listStyleName->setSelectionMode( QListBox::Multi );
    loadFile();
    resize (300, 400);
}

KWImportStyleDia::~KWImportStyleDia()
{
    m_frameStyleList.clear();
    m_tableStyleList.clear();
}

QString KWImportStyleDia::generateStyleName( const QString & templateName )
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


void KWImportStyleDia::loadFile()
{
    KFileDialog fd( QString::null, QString::null, 0, 0, TRUE );
    fd.setMimeFilter( "application/x-kword" );
    fd.setCaption(i18n("Insert File"));
    KURL url;
    if ( fd.exec() != QDialog::Accepted )
        return;
    url = fd.selectedURL();
    if( url.isEmpty() )
    {
        KMessageBox::sorry( this,
                            i18n("File name is empty"),
                            i18n("Import Style"));
        return;
    }
    // ### TODO network transparency
    KoStore* store=KoStore::createStore( url.path(), KoStore::Read );
    if ( store->open("maindoc.xml") )
    {
        QDomDocument doc;
        doc.setContent( store->device() );
        QDomElement word = doc.documentElement();
        if ( m_typeStyle ==frameStyle )
        {
            QDomNodeList listStyles = word.elementsByTagName( "FRAMESTYLE" );
            for (unsigned int item = 0; item < listStyles.count(); item++) {
                QDomElement styleElem = listStyles.item( item ).toElement();

                KWFrameStyle *sty = new KWFrameStyle( styleElem );
                QString name =sty->name();
                if ( m_list.findIndex( name )!=-1 )
                    sty->setName(generateStyleName( sty->translatedName() + QString( "- %1")));
                m_frameStyleList.append( sty);
            }
        }
        else
        {
            QDomNodeList listStyles = word.elementsByTagName( "TABLESTYLE" );
            for (unsigned int item = 0; item < listStyles.count(); item++) {
                QDomElement styleElem = listStyles.item( item ).toElement();
                KWTableStyle *sty = new KWTableStyle( styleElem,m_doc,2 );
                QString name =sty->name();
                if ( m_list.findIndex( name )!=-1 )
                    sty->setName(generateStyleName( sty->translatedName() + QString( "- %1")));
                m_tableStyleList.append( sty);
            }
        }
        initList();
    }
    else
    {
        KMessageBox::error( this,
                            i18n("File is not a kword file"),
                            i18n("Import Style"));
    }
    store->close();
    delete store;
}

void KWImportStyleDia::initList()
{
    QStringList lst;
    if ( m_typeStyle ==frameStyle )
    {
        for ( KWFrameStyle * p = m_frameStyleList.first(); p != 0L; p = m_frameStyleList.next() )
        {
            lst<<p->translatedName();
        }
    }
    else
    {
        for ( KWTableStyle * p = m_tableStyleList.first(); p != 0L; p = m_tableStyleList.next() )
        {
            lst<<p->translatedName();
        }
    }
    m_listStyleName->insertStringList(lst);
}

void KWImportStyleDia::slotOk()
{
    for (uint i = 0; i< m_listStyleName->count();i++)
    {
        if ( !m_listStyleName->isSelected( i ))
        {
            if ( m_typeStyle ==frameStyle )
            {
                //remove this style from list
                QPtrListIterator<KWFrameStyle> styleIt( m_frameStyleList );
                for ( ; styleIt.current(); ++styleIt )
                {
                    if ( styleIt.current()->name() == m_listStyleName->text(i ) )
                    {
                        m_frameStyleList.remove(styleIt.current());
                    }
                }
            }
            else
            {
                //remove this style from list
                QPtrListIterator<KWTableStyle> styleIt( m_tableStyleList );
                for ( ; styleIt.current(); ++styleIt )
                {
                    if ( styleIt.current()->name() == m_listStyleName->text(i ) )
                    {
                        m_tableStyleList.remove(styleIt.current());
                    }
                }
            }
        }
    }
    KDialogBase::slotOk();
}
#include "kwimportstyledia.moc"
