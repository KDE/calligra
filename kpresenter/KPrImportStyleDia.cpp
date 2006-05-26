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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <klocale.h>
#include "KPrDocument.h"
#include <q3vbox.h>
#include <QLayout>
#include <QLineEdit>
#include <QPushButton>
#include <q3listbox.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <kmessagebox.h>
#include "KPrImportStyleDia.h"
#include <KoStore.h>
#include <QFile>
#include <kfiledialog.h>
#include <kdebug.h>
#include <QLabel>
#include "KPrTextObject.h"

KPrImportStyleDia::KPrImportStyleDia( KPrDocument *_doc, KoStyleCollection* currentCollection,
                                      QWidget *parent, const char *name )
    :KoImportStyleDia( currentCollection, parent, name ),
     m_doc(_doc)
{
}

KPrImportStyleDia::~KPrImportStyleDia()
{
}

void KPrImportStyleDia::loadFile()
{
    KFileDialog fd( QString::null, QString::null, this);
    QStringList lst;
	lst<< "application/x-kpresenter";
#if 0 //For the future
    lst << "application/vnd.oasis.opendocument.presentation";
#endif
    fd.setMimeFilter( lst );
    fd.setCaption(i18n("Import Style"));
    KUrl url;
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
    KoStore* store=KoStore::createStore( this, url.path(), KoStore::Read );
    if (store )
    {
        if (store->open("maindoc.xml") )
        {
            clear();
            m_listStyleName->clear();

            QDomDocument doc;
            doc.setContent( store->device() );
            QDomElement word = doc.documentElement();

            QDomElement stylesElem = word.namedItem( "STYLES" ).toElement();
            if ( !stylesElem.isNull() )
            {
                //todo
                //duplicate code try to remove it !
                Q3ValueList<QString> followingStyles;
                QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
                for (unsigned int item = 0; item < listStyles.count(); item++)
                {
                    QDomElement styleElem = listStyles.item( item ).toElement();

                    KoParagStyle *sty = new KoParagStyle( QString::null );
                    // Load the style from the <STYLE> element
                    sty->loadStyle( styleElem );

                    if ( currentCollection()->findStyle( sty->name() ) )
                        sty->setName(generateStyleName(sty->name() + "-%1"));
                    QString name = sty->displayName();
                    // ### TODO: we should offer the option of updating the
                    // existing style instead of creating a foo-1 style. Any ideas for a GUI?
                    if ( currentCollection()->findStyleByDisplayName( name ) )
                        sty->setDisplayName(generateStyleDisplayName(sty->displayName() + "-%1"));
                    insertStyle.insert( name, sty->name() );

                    QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
                    if ( !formatElem.isNull() )
                        sty->format() = KPrTextObject::loadFormat( formatElem, 0L, m_doc->defaultFont(),
                                                                  m_doc->globalLanguage(), m_doc->globalHyphenation() );
                    else
                        kWarning(33001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().


                    // Style created, now let's try to add it
                    sty = m_styleList.addStyle(sty);

                    if( m_styleList.count() >= 0 && uint( m_styleList.count() ) > followingStyles.count() )
                    {
                        QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
                        followingStyles.append( following );
                    }
                    else
                        kWarning(33001) << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
                }

                Q_ASSERT( m_styleList.count() >= 0 && followingStyles.count() == uint( m_styleList.count() ) );

                unsigned int i=0;
                for( Q3ValueList<QString>::Iterator it = followingStyles.begin(); it != followingStyles.end(); ++it ) {
                    QString newName =*it;
                    if ( insertStyle.contains( *it ) )
                        newName = (insertStyle)[ *it ];

                    KoParagStyle * style = m_styleList.findStyle(newName);
                    if ( style )
                        m_styleList.styleAt(i++)->setFollowingStyle( style );
                }

            }
            initList();
        }
        else if ( store->hasFile( "content.xml" ) )
        {
            //oasis file format (for koffice-1.5)
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


#include "KPrImportStyleDia.moc"
