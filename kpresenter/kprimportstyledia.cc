// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
#include "kpresenter_doc.h"
#include <qvbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <kmessagebox.h>
#include "kprimportstyledia.h"
#include <koStore.h>
#include <qfile.h>
#include <kfiledialog.h>
#include <kdebug.h>
#include <qlabel.h>
#include "kptextobject.h"

KPrImportStyleDia::KPrImportStyleDia( KPresenterDoc *_doc, const QStringList &_list,
                                      QWidget *parent, const char *name )
    :KoImportStyleDia( _list, parent, name ),
     m_doc(_doc)
{
}

KPrImportStyleDia::~KPrImportStyleDia()
{
}

void KPrImportStyleDia::loadFile()
{
    KFileDialog fd( QString::null, QString::null, 0, 0, TRUE );
    fd.setMimeFilter( "application/x-kpresenter" );
    fd.setCaption(i18n("Import Style"));
    KURL url;
    if ( fd.exec() != QDialog::Accepted )
        return;
    url = fd.selectedURL();
    if( url.isEmpty() )
    {
        KMessageBox::sorry( this,
                            i18n("File name is empty."),
                            i18n("Import Style"));
        return;
    }
    QMap<QString, QString>insertStyle;
    // ### TODO network transparency
    KoStore* store=KoStore::createStore( url.path(), KoStore::Read );
    if (store )
    {
        if (store->open("maindoc.xml") )
        {
            m_styleList.setAutoDelete(true);
            m_styleList.clear();
            m_listStyleName->clear();

            QDomDocument doc;
            doc.setContent( store->device() );
            QDomElement word = doc.documentElement();

            QDomElement stylesElem = word.namedItem( "STYLES" ).toElement();
            if ( !stylesElem.isNull() )
            {
                //todo
                //duplicate code try to remove it !
                QValueList<QString> followingStyles;
                QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
                for (unsigned int item = 0; item < listStyles.count(); item++)
                {
                    QDomElement styleElem = listStyles.item( item ).toElement();

                    KoParagStyle *sty = new KoParagStyle( QString::null );
	            // Load the style from the <STYLE> element
	            sty->loadStyle( styleElem );

                    QString name = sty->name();
                    if ( m_list.findIndex( name )!=-1 )
                        sty->setName(generateStyleName( sty->translatedName() + QString( "-%1")));

                    QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
                    if ( !formatElem.isNull() )
                        sty->format() = KPTextObject::loadFormat( formatElem, 0L, m_doc->defaultFont(),
                                                                  m_doc->globalLanguage(), m_doc->globalHyphenation() );
                    else
                        kdWarning(33001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().


                    // Style created, now let's try to add it
                    m_styleList.append(sty);

                    if(m_styleList.count() > followingStyles.count() )
                    {
                        QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
                        followingStyles.append( following );
                    }
                    else
                        kdWarning(33001) << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
                }

                Q_ASSERT( followingStyles.count() == m_styleList.count() );

                unsigned int i=0;
                for( QValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
                    QString newName =*it;
                    if ( insertStyle.contains( *it ) )
                        newName = (insertStyle)[ *it ];

                    KoParagStyle * style = findStyle(newName);
                    if ( style )
                        m_styleList.at(i++)->setFollowingStyle( style );
                }

            }
            initList();
        }
        else
        {
            KMessageBox::error( this,
                                i18n("File is not a KPresenter file!"),
                                i18n("Import Style"));
        }
        store->close();
    }
    delete store;
}


#include "kprimportstyledia.moc"
