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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWDocument.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWDocStruct.h"
#include "KWDocStruct.moc"
#include "KWTableFrameSet.h"
#include "KWPartFrameSet.h"
#include "KWFormulaFrameSet.h"
#include "KWPictureFrameSet.h"
#include <koparagcounter.h>
#include "KWTextDocument.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <klistviewsearchline.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kpixmap.h>
#include <ktoolbar.h>
#include <ktoolbarbutton.h>

/******************************************************************/
/* Class: KWOrderedFrameSet                                       */
/******************************************************************/

KWOrderedFrameSet::KWOrderedFrameSet(KWFrameSet* fs) :
    m_frameset(fs) { }

KWOrderedFrameSet::KWOrderedFrameSet() :
    m_frameset(0) { }

bool KWOrderedFrameSet::operator<( KWOrderedFrameSet ofs )
{
    if (!m_frameset) return false;
    KWFrame* frame1 = m_frameset->frame(0);
    if (!frame1) return false;
    KWFrameSet* frameset2 = ofs.frameSet();
    if (!frameset2) return false;
    KWFrame* frame2 = frameset2->frame(0);
    if (!frame2) return false;
    KoPoint p1 = frame1->topLeft();
    KoPoint p2 = frame2->topLeft();
    return (p1.y() < p2.y() || (p1.y() == p2.y() && p1.x() < p2.x()));
}

KWDocListViewItem::KWDocListViewItem(QListViewItem *_parent, const QString &_text)
    :KListViewItem( _parent, _text )
{
}

KWDocListViewItem::KWDocListViewItem( QListViewItem *_parent, QListViewItem *_after, const QString &_text )
    :KListViewItem( _parent, _after, _text )
{
}

KWDocListViewItem::KWDocListViewItem(QListView *_parent, const QString &_text)
    :KListViewItem( _parent, _text )
{
}

void KWDocListViewItem::deleteAllChildren()
{
    if ( childCount() > 0 )
    {
        QListViewItem *child = firstChild();
        QListViewItem *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }
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

void KWDocStructParagItem::doubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructParagItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
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
/* Class: KWDocStructFrameSetItem                                 */
/******************************************************************/

KWDocStructFrameSetItem::KWDocStructFrameSetItem( QListViewItem *_parent, const QString &_text, KWFrameSet *_frameset, KWGUI*__parent )
    : KWDocListViewItem( _parent, _text )
{
    frameset = _frameset;
    gui = __parent;
}

KWDocStructFrameSetItem::KWDocStructFrameSetItem( QListViewItem *_parent, QListViewItem *_after,
    const QString &_text, KWFrameSet *_frameset, KWGUI*__parent )
    : KWDocListViewItem( _parent, _after, _text )
{
    frameset = _frameset;
    gui = __parent;
}

void KWDocStructFrameSetItem::setupTextFrames(KWDocument* doc)
{
    deleteAllChildren();

    QString name;
    KWDocStructFrameItem* child;
    for ( int j = frameset->frameCount() - 1; j >= 0; j-- )
    {
        if ( KListViewItem::parent()->firstChild() == this && doc->processingType() == KWDocument::WP )
        {
            if ( doc->numColumns() == 1 )
                name=i18n( "Page %1" ).arg(QString::number(j + 1));
            else
                name=i18n( "Column %1" ).arg(QString::number(j + 1));
        }
        else
            name=i18n( "Text Frame %1" ).arg(QString::number(j + 1));
        child = new KWDocStructFrameItem( this, name, frameset, frameset->frame( j ), gui );
    }
}

void KWDocStructFrameSetItem::doubleClicked( QListViewItem *_item )
{
    if ( _item == this ) {
        KWFrame* frame = frameset->frame(0);
        if (frame)
            gui->canvasWidget()->scrollToOffset( frame->topLeft() );
    }
}

void KWDocStructFrameSetItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, frameset);
}

void KWDocStructFrameSetItem::selectFrameSet()
{
    KWFrame* frame = frameset->frame(0);
    if (!frame) return;
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructFrameSetItem::editFrameSet()
{
    gui->canvasWidget()->editTextFrameSet( frameset, 0L, 0 );
}

void KWDocStructFrameSetItem::deleteFrameSet()
{
    gui->getView()->deleteFrameSet( frameset );
}

void KWDocStructFrameSetItem::editProperties()
{
     gui->canvasWidget()->editFrameProperties( frameset );
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

void KWDocStructFrameItem::doubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructFrameItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
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

void KWDocStructTableItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, table);
}


void KWDocStructTableItem::doubleClicked( QListViewItem *_item )
{
    if ( _item == this )
        selectFrameSet();
}

void KWDocStructTableItem::selectFrameSet()
{
    KWFrame *frame = table->cell( 0, 0 )->frame( 0 );
    gui->canvasWidget()->scrollToOffset( frame->topLeft() );
}

void KWDocStructTableItem::editFrameSet()
{
    //activate the first cell
    gui->canvasWidget()->editTextFrameSet( table->cell(0,0), 0L, 0 );
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

void KWDocStructPictureItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, pic);
}


void KWDocStructPictureItem::doubleClicked( QListViewItem *_item )
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

void KWDocStructFormulaItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, form);
}

void KWDocStructFormulaItem::doubleClicked( QListViewItem *_item )
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
void KWDocStructPartItem::rightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    if ( _item == this )
        gui->getView()->openDocStructurePopupMenu( p, part);
}


void KWDocStructPartItem::doubleClicked( QListViewItem *_item )
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

KWDocStructRootItem::KWDocStructRootItem( QListView *_parent, const QString &_text,
    KWDocument *_doc, TypeStructDocItem _type, KWGUI*__parent ) :
    KWDocListViewItem( _parent, _text )
{
    doc = _doc;
    type = _type;
    gui = __parent;

    switch ( type ) {
        case Arrangement: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "tree_arrange", KIcon::Small ) );
        } break;
        case TextFrames: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_text", KIcon::Small ) );
        } break;
        case FormulaFrames: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_formula", KIcon::Small ) );
        }break;
        case Tables: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "inline_table", KIcon::Small ) );
        } break;
        case Pictures:
        {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_image", KIcon::Small ) );
        } break;
        case Embedded: {
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
                setupTextFrameSets();
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
    deleteAllChildren();

    QIntDict<KWDocStructParagItem> parags;
    parags.setAutoDelete( false );

    KWFrameSet *frameset = 0L;
    KWTextParag *parag = 0L;
    KoTextDocument * textdoc=0L;

    KWDocStructFrameSetItem *item = 0L;
    QString _name;

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->groupmanager() && frameset->frameCount()>0)
        {
            item = new KWDocStructFrameSetItem( this, frameset->name(), frameset, gui );
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
                }
                parag = static_cast<KWTextParag *>(parag->next());
            }
        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );

}

void KWDocStructRootItem::setupTextFrameSets()
{
    deleteAllChildren();

    KWFrameSet *frameset = 0L;
    KWDocStructFrameSetItem *item = 0L;

    QValueList<KWOrderedFrameSet> orderedFrameSets;
    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- ) {
        frameset = doc->frameSet(i);
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY &&
            !frameset->groupmanager() && frameset->frameCount()>0)

            orderedFrameSets.append(KWOrderedFrameSet(frameset));
    }
    qHeapSort(orderedFrameSets);

    for ( uint i = 0; i < orderedFrameSets.count(); i++ )
    {
        frameset = orderedFrameSets[i].frameSet();
        if (item)
            item = new KWDocStructFrameSetItem( this, item, frameset->name(), frameset, gui );
        else
            item = new KWDocStructFrameSetItem( this, frameset->name(), frameset, gui );
        item->setupTextFrames(doc);
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupFormulaFrames()
{
    deleteAllChildren();

    KWFrameSet *frameset = 0L;
    QString _name;
    KWDocStructFormulaItem *child;

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_FORMULA &&
            frameset->frameCount()>0  )
        {
            _name=i18n("Formula Frame %1").arg(QString::number(i+1));
            child = new KWDocStructFormulaItem( this, _name, dynamic_cast<KWFormulaFrameSet*>( frameset ), gui );
        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupTables()
{
    deleteAllChildren();

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
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupPictures()
{
    deleteAllChildren();

    KWFrameSet *frameset = 0L;
    QString _name;
    KWDocStructPictureItem *child;

    int j = 0;
    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_PICTURE && frameset->frameCount()>0)
        {
            _name=i18n("Picture (%1) %2").arg(dynamic_cast<KWPictureFrameSet*>( frameset )->key().filename()).arg(++j);
            child = new KWDocStructPictureItem( this, _name, dynamic_cast<KWPictureFrameSet*>( frameset ), gui );
        }
    }

    if ( childCount() == 0 )
        ( void )new KListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupEmbedded()
{
    deleteAllChildren();

    KWFrameSet *frameset = 0L;
    QString _name;
    KWDocStructPartItem *child;

    for ( int i = doc->numFrameSets() - 1; i >= 0; i-- )
    {
        frameset = doc->frameSet( i );
        if ( frameset->type() == FT_PART && frameset->frameCount()>0)
        {
            // Use the name of the frameset as the entry for the object.
            _name=frameset->name();
            child = new KWDocStructPartItem( this, _name, dynamic_cast<KWPartFrameSet*>( frameset ), gui );
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

    embedded = new KWDocStructRootItem( this, i18n( "Embedded Objects" ), doc, Embedded, gui );
    formulafrms = new KWDocStructRootItem( this, i18n( "Formula Frames" ), doc, FormulaFrames, gui );
    tables = new KWDocStructRootItem( this, i18n( "Tables" ), doc, Tables, gui );
    pictures = new KWDocStructRootItem( this, i18n( "Pictures" ), doc, Pictures, gui );
    textfrms = new KWDocStructRootItem( this, i18n( "Text Frames/Frame Sets" ), doc, TextFrames, gui );
    arrangement = new KWDocStructRootItem( this, i18n( "Arrangement" ), doc, Arrangement, gui );

    addColumn( i18n( "Document Structure" ) );
    setFullWidth( true );

    connect( this, SIGNAL( doubleClicked( QListViewItem* ) ),
        this, SLOT( slotDoubleClicked( QListViewItem* ) ) );
    connect( this, SIGNAL( returnPressed( QListViewItem* ) ),
        this, SLOT( slotDoubleClicked( QListViewItem* ) ) );
    connect( this, SIGNAL(rightButtonClicked ( QListViewItem *, const QPoint &,int )),
        this, SLOT( slotRightButtonClicked( QListViewItem *, const QPoint &, int )));
    connect( this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint &)),
        this, SLOT(slotContextMenu(KListView *, QListViewItem *, const QPoint &)) );
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
        textfrms->setupTextFrameSets();
    if(((int)FormulaFrames) & _type)
        formulafrms->setupFormulaFrames();
    if(((int)Tables) & _type)
        tables->setupTables();
    if(((int)Pictures) & _type)
        pictures->setupPictures();
    if(((int)Embedded) & _type)
        embedded->setupEmbedded();
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

void KWDocStructTree::slotContextMenu(KListView *lv, QListViewItem *i, const QPoint &p)
{
    if (lv != this)
        return;
    KWDocListViewItem *item = dynamic_cast<KWDocListViewItem *>(i);
    if (item)
        item->rightButtonClicked(item, p, 0);
}

void KWDocStructTree::slotRightButtonClicked( QListViewItem *_item, const QPoint &p, int )
{
    KWDocListViewItem *item = dynamic_cast<KWDocListViewItem *>(_item);
    if (item)
        item->rightButtonClicked(item, p, 0);
}


void KWDocStructTree::slotDoubleClicked( QListViewItem *_item )
{
    KWDocListViewItem *item = dynamic_cast<KWDocListViewItem *>(_item);
    if (item)
        item->doubleClicked(item);
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
    dirtyTreeTypes = 0;
}

void KWDocStruct::paintEvent ( QPaintEvent * ev)
{
    if (dirtyTreeTypes) {
        tree->refreshTree(dirtyTreeTypes);
        dirtyTreeTypes = 0;
    }
    QWidget::paintEvent(ev);
}

void KWDocStruct::refreshTree(int _type)
{
    if ((dirtyTreeTypes | _type) != dirtyTreeTypes) {
        dirtyTreeTypes |= _type;
        update();
    }
}

void KWDocStruct::setFocusHere()
{
    if (tree)
        if (tree->isVisible()) tree->setFocus();
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
