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
#include "KWDocument.h"

#include <q3vbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <q3listbox.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <kmessagebox.h>
#include "KWImportStyleDia.h"
#include <KoStore.h>
#include <qfile.h>

#include <kfiledialog.h>
#include <kdebug.h>
#include <qlabel.h>

#include "KWTextParag.h"

KWImportStyleDia::KWImportStyleDia( KWDocument *_doc, KoStyleCollection* currentCollection, QWidget *parent, const char *name )
    :KoImportStyleDia( currentCollection, parent, name ),
     m_doc(_doc)
{

}

KWImportStyleDia::~KWImportStyleDia()
{
}


void KWImportStyleDia::loadFile()
{
    KFileDialog fd( QString::null, QString::null, 0, 0, true );
    QStringList lst = "application/x-kword";
#if 0
    lst << "application/vnd.oasis.opendocument.text";
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

    KoStore* store=KoStore::createStore( this, url, KoStore::Read );
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
                // TODO: use loadOasisStyleTemplates() for OASIS
                //       (and put as much code as possible in koimportstyledia of course)
                //
                // I guess we'll have to keep this old loading code forever though,
                // so we can't really get rid of the subclasses.

                Q3ValueList<QString> followingStyles;
                QDomNodeList listStyles = stylesElem.elementsByTagName( "STYLE" );
                for (unsigned int item = 0; item < listStyles.count(); item++)
                {
                    QDomElement styleElem = listStyles.item( item ).toElement();

                    KoParagStyle *sty = new KoParagStyle( QString::null );
                    // Load the paraglayout from the <STYLE> element
                    sty->loadStyle( styleElem, m_doc->syntaxVersion() ); //#### wrong syntaxVersion!

                    QString name = sty->displayName();
                    if ( currentCollection()->findStyle( name ) )
                        sty->setName(generateStyleName(sty->name() + "-%1"));
                    // ### TODO: we should offer the option of updating the
                    // existing style instead of creating a foo-1 style. Any ideas for a GUI?
                    if ( currentCollection()->findStyleByDisplayName( name ) )
                        sty->setDisplayName(generateStyleDisplayName(sty->displayName() + "-%1"));
                    insertStyle.insert( name, sty->name() ); // old name -> new name

                    QDomElement formatElem = styleElem.namedItem( "FORMAT" ).toElement();
                    if ( !formatElem.isNull() )
                        sty->format() = KWTextParag::loadFormat( formatElem, 0L, m_doc->defaultFont(), m_doc->globalLanguage(), m_doc->globalHyphenation() );
                    else
                        kWarning(32001) << "No FORMAT tag in <STYLE>" << endl; // This leads to problems in applyStyle().

                    // Style created, now let's try to add it
                    sty = m_styleList.addStyle(sty);

                    if( m_styleList.count() >= 0 && uint( m_styleList.count() ) > followingStyles.count() )
                    {
                        QString following = styleElem.namedItem("FOLLOWING").toElement().attribute("name");
                        followingStyles.append( following );
                    }
                    else
                        kWarning () << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
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
            if(m_styleList.count() == 0) {
                KMessageBox::error( this,
                                    i18n("File does not contain any styles. It may be the wrong version."),
                                    i18n("Import Style"));
            }

        }
        else if ( store->hasFile( "content.xml" ) )
        {
            //oasis file format (for koffice-1.5)
        }
        else
        {
            KMessageBox::error( this,
                                i18n("This file is not a KWord file!"),
                                i18n("Import Style"));
        }
        store->close();
    }
    delete store;
}

KWImportFrameTableStyleDia::KWImportFrameTableStyleDia( KWDocument *_doc, const QStringList &_list, StyleType _type, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel, Ok, true )
{
    setCaption( i18n("Import Style") );
    m_doc=_doc;
    m_typeStyle = _type;
    m_list =_list;
    KVBox *page = makeVBoxMainWidget();
    new QLabel(i18n("Select style to import:"), page);
    m_listStyleName = new Q3ListBox( page );
    m_listStyleName->setSelectionMode( Q3ListBox::Multi );
    loadFile();
    resize (300, 400);
    setFocus();
}

KWImportFrameTableStyleDia::~KWImportFrameTableStyleDia()
{
    m_frameStyleList.setAutoDelete(true);
    m_tableStyleList.setAutoDelete(true);
    m_frameStyleList.clear();
    m_tableStyleList.clear();
}

QString KWImportFrameTableStyleDia::generateStyleName( const QString & templateName )
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


void KWImportFrameTableStyleDia::loadFile()
{
    KFileDialog fd( QString::null, QString::null, 0, 0, true );
    QStringList lst = "application/x-kword";
#if 0
    lst << "application/vnd.oasis.opendocument.text";
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
    KoStore* store=KoStore::createStore( this, url, KoStore::Read );
    if (store )
    {
        if (store->open("maindoc.xml") )
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
                        sty->setDisplayName(generateStyleName( sty->displayName() + QString( "-%1")));
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
                        sty->setName(generateStyleName( sty->displayName() + QString( "-%1")));
                    m_tableStyleList.append( sty);
                }
            }
            initList();
            if(m_tableStyleList.count() == 0 && m_frameStyleList.count()==0) {
            KMessageBox::error( this,
                                i18n("File does not contain any styles. It may be the wrong version."),
                                i18n("Import Style"));
            }

        }
        else if ( store->hasFile( "content.xml" ) )
        {
            //oasis file format (for koffice-1.5)
        }
        else
        {
            KMessageBox::error( this,
                                i18n("This file is not a KWord file!"),
                                i18n("Import Style"));
        }

        store->close();
    }

    delete store;
}

void KWImportFrameTableStyleDia::initList()
{
    QStringList lst;
    if ( m_typeStyle ==frameStyle )
    {
        for ( KWFrameStyle * p = m_frameStyleList.first(); p != 0L; p = m_frameStyleList.next() )
        {
            lst<<p->displayName();
        }
    }
    else
    {
        for ( KWTableStyle * p = m_tableStyleList.first(); p != 0L; p = m_tableStyleList.next() )
        {
            lst<<p->displayName();
        }
    }

    m_listStyleName->insertStringList(lst);
}

void KWImportFrameTableStyleDia::slotOk()
{
    for (uint i = 0; i< m_listStyleName->count();i++)
    {
        if ( !m_listStyleName->isSelected( i ))
        {
            QString name = m_listStyleName->text(i );
            if ( m_typeStyle ==frameStyle ) // frame styles
            {
                //remove this style from list
                Q3PtrListIterator<KWFrameStyle> styleIt( m_frameStyleList );
                for ( ; styleIt.current(); ++styleIt )
                {
                    if ( styleIt.current()->displayName() == name )
                    {
                        m_frameStyleList.remove(styleIt.current());
                        break;
                    }
                }
            }
            else // then it will have to be table styles
            {
                //remove this style from list
                Q3PtrListIterator<KWTableStyle> styleIt( m_tableStyleList );
                for ( ; styleIt.current(); ++styleIt )
                {
                    if ( styleIt.current()->displayName() == name )
                    {
                        m_tableStyleList.remove(styleIt.current());
                        break;
                    }
                }
            }
        }
    }
    KDialogBase::slotOk();
}
#include "KWImportStyleDia.moc"
