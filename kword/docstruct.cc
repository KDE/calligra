/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "kwdoc.h"
#include "kwview.h"
#include "kwcanvas.h"
#include "kwframe.h"
#include "docstruct.h"
#include "docstruct.moc"
#include "kwtextframeset.h"
#include "kwtableframeset.h"
#include "counter.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>

#include <qframe.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qintdict.h>


/******************************************************************/
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

KWDocStructParagItem::KWDocStructParagItem( QListViewItem *_parent, QString _text, KWTextParag *_parag, KWGUI*__parent )
    : QListViewItem( _parent, _text )
{
    parag = _parag;
    gui = __parent;
}

KWDocStructParagItem::KWDocStructParagItem( QListViewItem *_parent, QListViewItem *_after, QString _text, KWTextParag *_parag, KWGUI*__parent )
    : QListViewItem( _parent, _after, _text )
{
    parag = _parag;
    gui = __parent;
}

void KWDocStructParagItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
    {
        KWTextFrameSet * fs = parag->kwTextDocument()->textFrameSet();
        QPoint iPoint = parag->rect().topLeft(); // small bug if a paragraph is cut between two pages.
        QPoint cPoint;
        fs->internalToNormal( iPoint, cPoint );
        gui->canvasWidget()->scrollToOffset( fs->kWordDocument()->unzoomPoint( cPoint ) );
    }
}

/******************************************************************/
/* Class: KWDocStructFrameItem                                    */
/******************************************************************/

KWDocStructFrameItem::KWDocStructFrameItem( QListViewItem *_parent, QString _text, KWFrameSet *_frameset, KWFrame *_frame, KWGUI*__parent )
    : QListViewItem( _parent, _text )
{
    frame = _frame;
    frameset = _frameset;
    gui = __parent;
}

void KWDocStructFrameItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

KWDocStructTableItem::KWDocStructTableItem( QListViewItem *_parent, QString _text, KWTableFrameSet *_table, KWGUI*__parent )
    : QListViewItem( _parent, _text )
{
    table = _table;
    gui = __parent;
}

void KWDocStructTableItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
    {
        KWFrame *frame = table->getCell( 0, 0 )->getFrame( 0 );
        gui->canvasWidget()->scrollToOffset( frame->topLeft() );
    }
}

/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

KWDocStructPictureItem::KWDocStructPictureItem( QListViewItem *_parent, QString _text, KWPictureFrameSet *_pic, KWGUI*__parent )
    : QListViewItem( _parent, _text )
{
    pic = _pic;
    gui = __parent;
}

void KWDocStructPictureItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
    {
        KWFrame *frame = pic->getFrame( 0 );
        gui->canvasWidget()->scrollToOffset( frame->topLeft() );
    }
}

/******************************************************************/
/* Class: KWDocStructFormulaItem                                  */
/******************************************************************/

KWDocStructFormulaItem::KWDocStructFormulaItem( QListViewItem *_parent, QString _text, KWFormulaFrameSet *_form, KWGUI*__parent )
    : QListViewItem( _parent, _text )
{
    form = _form;
    gui = __parent;
}

void KWDocStructFormulaItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
    {
        KWFrame *frame = form->getFrame( 0 );
        gui->canvasWidget()->scrollToOffset( frame->topLeft() );
    }
}

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

KWDocStructPartItem::KWDocStructPartItem( QListViewItem *_parent, QString _text, KWPartFrameSet *_part, KWGUI*__parent )
    : QListViewItem( _parent, _text )
{
    part = _part;
    gui = __parent;
}

void KWDocStructPartItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
    {
        KWFrame *frame = part->getFrame( 0 );
        gui->canvasWidget()->scrollToOffset( frame->topLeft() );
    }
}

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

KWDocStructRootItem::KWDocStructRootItem( QListView *_parent, KWDocument *_doc, TypeStructDocItem _type, KWGUI*__parent )
    : QListViewItem( _parent )
{
    doc = _doc;
    type = _type;
    gui = __parent;

    switch ( type ) {
        case Arrangement: {
            setText( 0, i18n( "Arrangement" ) );
            setPixmap( 0, KWBarIcon( "tree_arrange" ) );
        } break;
        case TextFrames: {
            setText( 0, i18n( "Text Frames/Frame Sets" ) );
            setPixmap( 0, KWBarIcon( "frame_text" ) );
        } break;
        case FormulaFrames: {
            setText( 0, i18n( "Formula Frames" ) );
            setPixmap( 0, KWBarIcon( "frame_formula" ) );
        }break;
        case Tables: {
            setText( 0, i18n( "Tables" ) );
            setPixmap( 0, KWBarIcon( "inline_table" ) );
        } break;
        case Pictures: {
            setText( 0, i18n( "Pictures" ) );
            setPixmap( 0, KWBarIcon( "frame_image" ) );
        } break;
        case Cliparts: {
            setText( 0, i18n( "Cliparts" ) );
            setPixmap( 0, KWBarIcon( "clipart" ) );
        } break;
        case Embedded: {
            setText( 0, i18n( "Embedded Objects" ) );
            setPixmap( 0, KWBarIcon( "frame_query" ) );
        } break;
    }
}

void KWDocStructRootItem::setOpen( bool o )
{
    if ( o )
    {
        switch ( type )
        {
            case Arrangement:
                setupArrangement();
                break;
            case TextFrames:
                setupTextFrames();
                break;
            case FormulaFrames:
                setupFormulaFrames();
                break;
            case Tables:
                setupTables();
                break;
            case Pictures:
                setupPictures();
                break;
            case Cliparts:
                setupCliparts();
                break;
            case Embedded:
                setupEmbedded();
                break;
        }
    }
    QListViewItem::setOpen( o );
}

void KWDocStructRootItem::setupArrangement()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild(), *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }

    QIntDict<KWDocStructParagItem> parags;
    parags.setAutoDelete( false );

    KWFrameSet *frameset = 0L;
    KWTextParag *parag = 0L;
    QTextDocument * textdoc=0L;

    QListViewItem *item = 0L;
    QString _name;

    for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->getFrameSet( i );
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->getGroupManager() && frameset->getNumFrames()>0)
        {
            item = new QListViewItem( this, frameset->getName() );
            KWTextFrameSet *tmpParag = dynamic_cast<KWTextFrameSet*> (frameset) ;
            textdoc= tmpParag->textDocument();
            parag = static_cast<KWTextParag *>(textdoc->firstParag());
            while ( parag )
            {
                KoParagCounter *tmpCounter=parag->counter();
                if (tmpCounter!=0 && (tmpCounter->style() != KoParagCounter::STYLE_NONE) &&  (tmpCounter->numbering() == KoParagCounter::NUM_CHAPTER) )
                {
                    int _depth = tmpCounter->depth();
                    if ( _depth == 0 )
                    {
                        if ( item->childCount() == 0 )
                            parags.replace( _depth, new KWDocStructParagItem( item,QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag, gui ) );
                        else
                            parags.replace( _depth, new KWDocStructParagItem( item, parags[ _depth ],QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag, gui ) );
                    }
                    else
                    {
                        if (parags[ _depth - 1 ]==0)
                            parags.replace( _depth, new KWDocStructParagItem( item,QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag, gui ) );
                        else if ( parags[ _depth - 1 ]->childCount() == 0 )
                            parags.replace( _depth, new KWDocStructParagItem( parags[ _depth - 1 ],QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag, gui ) );
                        else
                            parags.replace( _depth, new KWDocStructParagItem( parags[ _depth - 1 ], parags[ _depth ],QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag, gui ) );
                    }
                    QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), parags[ _depth ], SLOT( slotDoubleClicked( QListViewItem* ) ) );
                }
                parag = static_cast<KWTextParag *>(parag->next());
            }
        }
    }

    if ( childCount() == 0 )
        ( void )new QListViewItem( this, i18n( "Empty" ) );

}

void KWDocStructRootItem::setupTextFrames()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild(), *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }

    KWFrameSet *frameset = 0L;
    QListViewItem *item = 0L;
    QString _name;
    KWDocStructFrameItem *child;

    for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->getFrameSet( i );
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->getGroupManager() && frameset->getNumFrames()>0)
        {
            item = new QListViewItem( this, frameset->getName() );

            for ( int j = frameset->getNumFrames() - 1; j >= 0; j-- )
            {
                if ( i == 0 && doc->processingType() == KWDocument::WP )
                {
                    if ( doc->getColumns() == 1 )
                        _name=i18n( "Page %1" ).arg(QString::number(j + 1));
                    else
                        _name=i18n( "Column %1" ).arg(QString::number(j + 1));
                }
                else
                    _name=i18n( "Text Frame %1" ).arg(QString::number(j + 1));
                child = new KWDocStructFrameItem( item, _name, frameset, frameset->getFrame( j ), gui );
                QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            }
        }
    }

    if ( childCount() == 0 )
        ( void )new QListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupFormulaFrames()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild(), *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }

    KWFrameSet *frameset = 0L;
    QString _name;
    KWDocStructFormulaItem *child;

    for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->getFrameSet( i );
        if ( frameset->type() == FT_FORMULA &&
            frameset->getNumFrames()>0  )
        {
            _name=i18n("Formula frame %1").arg(QString::number(i+1));
            child = new KWDocStructFormulaItem( this, _name, dynamic_cast<KWFormulaFrameSet*>( frameset ), gui );
            QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
        }
    }

    if ( childCount() == 0 )
        ( void )new QListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupTables()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild(), *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }

    QString _name;
    KWDocStructTableItem *child;

    for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- )
    {
        KWFrameSet *fs = doc->getFrameSet(i);
        if ( fs->type() != FT_TABLE)
            continue;
        KWTableFrameSet *tfs = static_cast<KWTableFrameSet *> (fs);
        if(!tfs->isActive() )
            continue;

        _name=i18n( "Table %1" ).arg(QString::number( i + 1 ));
        child = new KWDocStructTableItem( this, _name, tfs, gui );
        QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
    }

    if ( childCount() == 0 )
        ( void )new QListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupPictures()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild(), *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }

    KWFrameSet *frameset = 0L;
    QString _name;
    KWDocStructPictureItem *child;

    int j = 0;
    for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->getFrameSet( i );
        if ( frameset->type() == FT_PICTURE && frameset->getNumFrames()>0)
        {
            _name=i18n("Picture (%1) %2").arg(dynamic_cast<KWPictureFrameSet*>( frameset )->key().filename()).arg(++j);
            child = new KWDocStructPictureItem( this, _name, dynamic_cast<KWPictureFrameSet*>( frameset ), gui );
            QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
        }
    }

    if ( childCount() == 0 )
        ( void )new QListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupCliparts()
{
}

void KWDocStructRootItem::setupEmbedded()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild(), *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }

    KWFrameSet *frameset = 0L;
    QString _name;
    KWDocStructPartItem *child;

    int j = 0;
    for ( int i = doc->getNumFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->getFrameSet( i );
        if ( frameset->type() == FT_PART && frameset->getNumFrames()>0)
        {
            _name=i18n( "Embedded Object %1" ).arg(QString::number( ++j ));
            child = new KWDocStructPartItem( this, _name, dynamic_cast<KWPartFrameSet*>( frameset ), gui );
            QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
        }
    }

    if ( childCount() == 0 )
        ( void )new QListViewItem( this, i18n( "Empty" ) );
}

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

KWDocStructTree::KWDocStructTree( QWidget *_parent, KWDocument *_doc, KWGUI*__parent )
    : QListView( _parent )
{
    doc = _doc;
    gui = __parent;

    addColumn( i18n( "Document Structure" ) );
    //addColumn( i18n( "Additional Info" ) );
    //setColumnWidthMode( 0, Manual );
    //setColumnWidthMode( 1, Manual );
}

void KWDocStructTree::setup()
{
    setRootIsDecorated( true );
    setSorting( -1 );

    embedded = new KWDocStructRootItem( this, doc, Embedded, gui );
    QListViewItem *item = new QListViewItem( embedded, i18n ("Empty" ) );

    cliparts = new KWDocStructRootItem( this, doc, Cliparts, gui );
    item = new QListViewItem( cliparts, i18n( "Empty" ) );

    pictures = new KWDocStructRootItem( this, doc,Pictures, gui );
    item = new QListViewItem( pictures, i18n ("Empty" ) );

    tables = new KWDocStructRootItem( this, doc, Tables, gui );
    item = new QListViewItem( tables, i18n ("Empty" ) );

    formulafrms = new KWDocStructRootItem( this, doc, FormulaFrames, gui );
    item = new QListViewItem( formulafrms, i18n ("Empty" ) );

    textfrms = new KWDocStructRootItem( this, doc, TextFrames, gui );
    item = new QListViewItem( textfrms, i18n ("Empty" ) );

    arrangement = new KWDocStructRootItem( this, doc, Arrangement, gui );
    item = new QListViewItem( arrangement, i18n ( "Empty" ) );
}

void KWDocStructTree::refreshTree(int _type)
{
    if(((int)Arrangement) & _type)
        arrangement->setupArrangement();
    if(((int)TextFrames) & _type)
        textfrms->setupTextFrames();
    if(((int)FormulaFrames) & _type)
        formulafrms->setupFormulaFrames();
    if(((int)Tables) & _type)
        tables->setupTables();
    if(((int)Pictures) & _type)
        pictures->setupPictures();
    if(((int)Cliparts) & _type)
        cliparts->setupCliparts();
    if(((int)Embedded) & _type)
        embedded->setupEmbedded();
}

/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

KWDocStruct::KWDocStruct( QWidget *_parent, KWDocument *_doc, KWGUI*__parent )
    : QWidget( _parent )
{
    doc = _doc;
    parent = __parent;

    layout = new QGridLayout( this, 1, 1, 0, 0 );

    tree = new KWDocStructTree( this, doc, __parent );
    tree->resize( tree->sizeHint() );
    layout->addWidget( tree, 0, 0 );
    layout->addColSpacing( 0, 0 );
    layout->addRowSpacing( 0, tree->width() );
    layout->setColStretch( 0, 1 );
    layout->setRowStretch( 0, 1 );
    tree->setup();

    layout->activate();
}



