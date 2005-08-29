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
#include "docstruct.h"
#include "docstruct.moc"
#include "kwtableframeset.h"
#include "kwpartframeset.h"
#include "kwformulaframe.h"
#include <koparagcounter.h>
#include "kwtextdocument.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klistviewsearchline.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kpixmap.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>

KWDocListViewItem::KWDocListViewItem(QListViewItem *_parent, const QString &_text)
    :KListViewItem( _parent, _text )
{
}

KWDocListViewItem::KWDocListViewItem( QListViewItem *_parent, QListViewItem *_after, const QString &_text )
    :KListViewItem( _parent, _after, _text )
{
}

/******************************************************************/
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

KWDocStructParagItem::KWDocStructParagItem( QListViewItem *_parent, const QString &_text, KWTextParag *_parag, KWGUI*__parent )
    : KWDocListViewItem(_parent,_text)
{
    parag = _parag;
    gui = __parent;
}

KWDocStructParagItem::KWDocStructParagItem( QListViewItem *_parent, QListViewItem *_after, const QString &_text, KWTextParag *_parag, KWGUI*__parent )
    : KWDocListViewItem( _parent, _after, _text )
{
    parag = _parag;
    gui = __parent;
}

void KWDocStructParagItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructParagItem::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
    {
        KWTextFrameSet * fs = parag->kwTextDocument()->textFrameSet();
        gui->getView()->openDocStructurePopupMenu( p, fs);
    }
}

void KWDocStructParagItem::selectFrameSet()
{
    KWTextFrameSet * fs = parag->kwTextDocument()->textFrameSet();
    QPoint iPoint = parag->rect().topLeft(); // small bug if a paragraph is cut between two pages.
    KoPoint dPoint;
    fs->internalToDocument( iPoint, dPoint );
    QPoint nPoint = fs->kWordDocument()->zoomPoint( dPoint );
    gui->canvasWidget()->scrollToOffset( fs->kWordDocument()->unzoomPoint( nPoint ) );

}

void KWDocStructParagItem::editFrameSet()
{
    gui->canvasWidget()->editTextFrameSet( parag->kwTextDocument()->textFrameSet(), parag, 0 );
}

void KWDocStructParagItem::deleteFrameSet()
{
}


/******************************************************************/
/* Class: KWDocStructFrameItem                                    */
/******************************************************************/

KWDocStructFrameItem::KWDocStructFrameItem( QListViewItem *_parent, const QString &_text, KWFrameSet *_frameset, KWFrame *_frame, KWGUI*__parent )
    : KWDocListViewItem( _parent, _text )
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

void KWDocStructFrameItem::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, frameset);
}

void KWDocStructFrameItem::selectFrameSet()
{
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructFrameItem::editFrameSet()
{
    gui->canvasWidget()->editTextFrameSet( frameset, 0L, 0 );
}

void KWDocStructFrameItem::deleteFrameSet()
{
    gui->getView()->deleteFrameSet( frameset );
}

void KWDocStructFrameItem::editProperties()
{
     gui->canvasWidget()->editFrameProperties( frameset );
}

/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

KWDocStructTableItem::KWDocStructTableItem( QListViewItem *_parent, const QString &_text, KWTableFrameSet *_table, KWGUI*__parent )
    :KWDocListViewItem( _parent, _text )
{
    table = _table;
    gui = __parent;
}

void KWDocStructTableItem::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, table);
}


void KWDocStructTableItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructTableItem::selectFrameSet()
{
    KWFrame *frame = table->getCell( 0, 0 )->frame( 0 );
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructTableItem::editFrameSet()
{
    //activate the first cell
    gui->canvasWidget()->editTextFrameSet( table->getCell(0,0), 0L, 0 );
}

void KWDocStructTableItem::deleteFrameSet()
{
    gui->getView()->deleteFrameSet( table );
}

void KWDocStructTableItem::editProperties()
{
     gui->canvasWidget()->editFrameProperties( table );
}

/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

KWDocStructPictureItem::KWDocStructPictureItem( QListViewItem *_parent, const QString &_text, KWPictureFrameSet *_pic, KWGUI*__parent )
    : KWDocListViewItem( _parent, _text )
{
    pic = _pic;
    gui = __parent;
}

void KWDocStructPictureItem::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, pic);
}


void KWDocStructPictureItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructPictureItem::selectFrameSet()
{
    KWFrame *frame = pic->frame( 0 );
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );

}

void KWDocStructPictureItem::deleteFrameSet()
{
    gui->getView()->deleteFrameSet( pic );
}

void KWDocStructPictureItem::editProperties()
{
    gui->canvasWidget()->editFrameProperties( pic );
}

/******************************************************************/
/* Class: KWDocStructFormulaItem                                  */
/******************************************************************/

KWDocStructFormulaItem::KWDocStructFormulaItem( QListViewItem *_parent, const QString &_text, KWFormulaFrameSet *_form, KWGUI*__parent )
    : KWDocListViewItem( _parent, _text )
{
    form = _form;
    gui = __parent;
}

void KWDocStructFormulaItem::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, form);
}

void KWDocStructFormulaItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructFormulaItem::selectFrameSet()
{
    KWFrame *frame = form->frame( 0 );
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructFormulaItem::editFrameSet()
{
    gui->canvasWidget()->editFrameSet(form );

}

void KWDocStructFormulaItem::deleteFrameSet()
{
    gui->getView()->deleteFrameSet( form );
}

void KWDocStructFormulaItem::editProperties()
{
     gui->canvasWidget()->editFrameProperties( form );
}

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

KWDocStructPartItem::KWDocStructPartItem( QListViewItem *_parent, const QString &_text, KWPartFrameSet *_part, KWGUI*__parent )
    : KWDocListViewItem( _parent, _text )
{
    part = _part;
    gui = __parent;
}
void KWDocStructPartItem::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, part);
}


void KWDocStructPartItem::slotDoubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructPartItem::selectFrameSet()
{
    KWFrame *frame = part->frame( 0 );
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructPartItem::editFrameSet()
{
    //todo
}

void KWDocStructPartItem::deleteFrameSet()
{
    gui->getView()->deleteFrameSet( part );
}

void KWDocStructPartItem::editProperties()
{
     gui->canvasWidget()->editFrameProperties( part );
}

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

KWDocStructRootItem::KWDocStructRootItem( QListView *_parent, KWDocument *_doc, TypeStructDocItem _type, KWGUI*__parent )
    : KListViewItem( _parent )
{
    doc = _doc;
    type = _type;
    gui = __parent;

    switch ( type ) {
        case Arrangement: {
            setText( 0, i18n( "Arrangement" ) );
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "tree_arrange", KIcon::Small ) );
        } break;
        case TextFrames: {
            setText( 0, i18n( "Text Frames/Frame Sets" ) );
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_text", KIcon::Small ) );
        } break;
        case FormulaFrames: {
            setText( 0, i18n( "Formula Frames" ) );
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_formula", KIcon::Small ) );
        }break;
        case Tables: {
            setText( 0, i18n( "Tables" ) );
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "inline_table", KIcon::Small ) );
        } break;
        case Pictures:
        {
            setText( 0, i18n( "Pictures" ) );
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_image", KIcon::Small ) );
        } break;
        case Embedded: {
            setText( 0, i18n( "Embedded Objects" ) );
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_query", KIcon::Small ) );
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
    KoTextDocument * textdoc=0L;

    QListViewItem *item = 0L;
    QString _name;

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->getGroupManager() && frameset->getNumFrames()>0)
        {
            item = new KListViewItem( this, frameset->getName() );
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
                    QObject::connect( listView(), SIGNAL( returnPressed( QListViewItem* ) ), parags[ _depth ], SLOT( slotDoubleClicked( QListViewItem* ) ) );
                    QObject::connect( listView(), SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )), parags[_depth], SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));

                }
                parag = static_cast<KWTextParag *>(parag->next());
            }
        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );

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

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->getGroupManager() && frameset->getNumFrames()>0)
        {
            item = new KListViewItem( this, frameset->getName() );

            for ( int j = frameset->getNumFrames() - 1; j >= 0; j-- )
            {
                if ( i == 0 && doc->processingType() == KWDocument::WP )
                {
                    if ( doc->numColumns() == 1 )
                        _name=i18n( "Page %1" ).arg(QString::number(j + 1));
                    else
                        _name=i18n( "Column %1" ).arg(QString::number(j + 1));
                }
                else
                    _name=i18n( "Text Frame %1" ).arg(QString::number(j + 1));
                child = new KWDocStructFrameItem( item, _name, frameset, frameset->frame( j ), gui );
                QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
                QObject::connect( listView(), SIGNAL( returnPressed( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
                QObject::connect( listView(), SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )), child, SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));


            }
        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
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

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_FORMULA &&
            frameset->getNumFrames()>0  )
        {
            _name=i18n("Formula Frame %1").arg(QString::number(i+1));
            child = new KWDocStructFormulaItem( this, _name, dynamic_cast<KWFormulaFrameSet*>( frameset ), gui );
            QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            QObject::connect( listView(), SIGNAL( returnPressed( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            QObject::connect( listView(), SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )), child, SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));

        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
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

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        KWFrameSet *fs = doc->frameSet(i);
        if ( fs->type() != FT_TABLE)
            continue;
        KWTableFrameSet *tfs = static_cast<KWTableFrameSet *> (fs);
        if(!tfs->isActive() )
            continue;

        _name=i18n( "Table %1" ).arg(QString::number( i + 1 ));
        child = new KWDocStructTableItem( this, _name, tfs, gui );
        QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
        QObject::connect( listView(), SIGNAL( returnPressed( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
        QObject::connect( listView(), SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )), child, SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));


    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
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
    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_PICTURE && frameset->getNumFrames()>0)
        {
            _name=i18n("Picture (%1) %2").arg(dynamic_cast<KWPictureFrameSet*>( frameset )->key().filename()).arg(++j);
            child = new KWDocStructPictureItem( this, _name, dynamic_cast<KWPictureFrameSet*>( frameset ), gui );
            QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            QObject::connect( listView(), SIGNAL( returnPressed( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            QObject::connect( listView(), SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )), child, SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));


        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
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

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_PART && frameset->getNumFrames()>0)
        {
            // Use the name of the frameset as the entry for the object.
            _name=frameset->getName();
            child = new KWDocStructPartItem( this, _name, dynamic_cast<KWPartFrameSet*>( frameset ), gui );
            QObject::connect( listView(), SIGNAL( doubleClicked( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            QObject::connect( listView(), SIGNAL( returnPressed( QListViewItem* ) ), child, SLOT( slotDoubleClicked( QListViewItem* ) ) );
            QObject::connect( listView(), SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )), child, SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));

        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
}

/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

KWDocStructTree::KWDocStructTree( QWidget *_parent, KWDocument *_doc, KWGUI*__parent )
    : KListView( _parent )
{
    doc = _doc;
    gui = __parent;

    embedded = new KWDocStructRootItem( this, doc, Embedded, gui );
    formulafrms = new KWDocStructRootItem( this, doc, FormulaFrames, gui );
    tables = new KWDocStructRootItem( this, doc, Tables, gui );
    pictures = new KWDocStructRootItem( this, doc, Pictures, gui );
    textfrms = new KWDocStructRootItem( this, doc, TextFrames, gui );
    arrangement = new KWDocStructRootItem( this, doc, Arrangement, gui );

    addColumn( i18n( "Document Structure" ) );
    setFullWidth( true );
}

KWDocStructTree::~KWDocStructTree()
{
    delete embedded;
    delete formulafrms;
    delete tables;
    delete pictures;
    delete textfrms;
    delete arrangement;
}

void KWDocStructTree::setup()
{
    setRootIsDecorated( true );
    setSorting( -1 );
    refreshTree((int)TextFrames);
    refreshTree((int)FormulaFrames);
    refreshTree((int)Tables);
    refreshTree((int)Pictures);
    refreshTree((int)Embedded);
}

void KWDocStructTree::refreshTree(int _type)
{
    if(((int)Arrangement) & _type)
        arrangement->setupArrangement();
    if(((int)TextFrames) & _type)
    {
        if(testExistTypeOfFrame(TextFrames))
            textfrms->setupTextFrames();
    }
    if(((int)FormulaFrames) & _type)
    {
        if(testExistTypeOfFrame(FormulaFrames))
            formulafrms->setupFormulaFrames();
    }
    if(((int)Tables) & _type)
    {
        if(testExistTypeOfFrame(Tables))
            tables->setupTables();
    }
    if(((int)Pictures) & _type)
    {
        if(testExistTypeOfFrame(Pictures))
            pictures->setupPictures();
    }
    if(((int)Embedded) & _type)
    {
        if(testExistTypeOfFrame(Embedded))
            embedded->setupEmbedded();
    }
}

bool KWDocStructTree::testExistTypeOfFrame(TypeStructDocItem _type)
{
    KWFrameSet *frameset = 0L;
    KWTableFrameSet *tfs=0L;
    KWTextFrameSet *tmpParag=0L;
    KWTextParag *parag = 0L;
    KoTextDocument * textdoc=0L;

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        switch ( _type )
        {
            case Arrangement:
                if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->getGroupManager() && frameset->getNumFrames()>0)
                {

                    tmpParag = dynamic_cast<KWTextFrameSet*> (frameset) ;
                    textdoc= tmpParag->textDocument();
                    parag = static_cast<KWTextParag *>(textdoc->firstParag());
                    while ( parag )
                    {
                        KoParagCounter *tmpCounter=parag->counter();
                        if (tmpCounter!=0 && (tmpCounter->style() != KoParagCounter::STYLE_NONE) &&  (tmpCounter->numbering() == KoParagCounter::NUM_CHAPTER) )
                            return true;
                        parag = static_cast<KWTextParag *>(parag->next());
                    }
                }
                break;
            case TextFrames:
                if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->getGroupManager() && frameset->getNumFrames()>0)
                    return true;
                break;
            case FormulaFrames:
                if ( frameset->type() == FT_FORMULA &&
                     frameset->getNumFrames()>0  )
                    return true;
                break;
            case Tables:
                if ( frameset->type() != FT_TABLE)
                    continue;
                tfs = static_cast<KWTableFrameSet *> (frameset);
                if(!tfs->isActive() )
                    continue;
                return true;
                break;
            case Pictures:
                if ( frameset->type() == FT_PICTURE && frameset->getNumFrames()>0)
                    return true;
                break;
            case Embedded:
                if ( frameset->type() == FT_PART && frameset->getNumFrames()>0)
                    return true;
                break;
        }
    }
    return false;
}

void KWDocStructTree::selectFrameSet()
{
    QListViewItem * select=currentItem ();
    KWDocListViewItem *tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->selectFrameSet();
}

void KWDocStructTree::editFrameSet()
{
    QListViewItem * select=currentItem ();
    KWDocListViewItem *tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->editFrameSet();
}


void KWDocStructTree::deleteFrameSet()
{
    QListViewItem * select=currentItem ();
    KWDocListViewItem *tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->deleteFrameSet();
}


void KWDocStructTree::editProperties()
{
    QListViewItem * select=currentItem ();
    KWDocListViewItem *tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->editProperties();
}


/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

KWDocStruct::KWDocStruct( QWidget *_parent, KWDocument *_doc, KWGUI*__parent )
    : QWidget( _parent )
{
    doc = _doc;
    parent = __parent;

    QVBoxLayout* vBox = new QVBoxLayout( this );

    KToolBar* searchBar = new KToolBar( this );
    searchBar->setFlat( true );
    searchBar->setMovingEnabled( false );

    KToolBarButton* eraseButton = new KToolBarButton( "locationbar_erase", 0, searchBar );
    tree = new KWDocStructTree( this, doc, __parent );
    tree->setAlternateBackground( KGlobalSettings::alternateBackgroundColor() );
    KListViewSearchLine* searchLine = new KListViewSearchLine( searchBar, tree );
    searchBar->setStretchableWidget( searchLine );
    connect( eraseButton, SIGNAL( clicked() ), searchLine, SLOT( clear() ) );

    vBox->addWidget( searchBar );
    vBox->addWidget( tree );
    tree->setup();
}

void KWDocStruct::selectFrameSet()
{
    tree->selectFrameSet();
}

void KWDocStruct::editFrameSet()
{
    tree->editFrameSet();
}

void KWDocStruct::deleteFrameSet()
{
    tree->deleteFrameSet();
}

void KWDocStruct::editProperties()
{
    tree->editProperties();
}
