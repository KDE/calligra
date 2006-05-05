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

// KWord includes.
#include "KWDocument.h"
#include "KWView.h"
#include "KWCanvas.h"
#include "KWDocStruct.h"
#include "KWDocStruct.moc"
#include "KWTableFrameSet.h"
#include "KWPartFrameSet.h"
#include "KWFormulaFrameSet.h"
#include "KWPictureFrameSet.h"
#include "KWTextDocument.h"

// KOffice includes.
#include <KoParagCounter.h>

// KDE includes.
#include <klocale.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <k3listviewsearchline.h>
#include <kglobalsettings.h>
#include <kglobal.h>
#include <kpixmap.h>
#include <ktoolbar.h>


#include <qlayout.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QList>
#include <QPaintEvent>
#include <Q3PtrList>
#include <q3tl.h>
#include <q3intdict.h>

static bool orderFrameSetByPosition( const KWFrameSet* lhs, const KWFrameSet* rhs )
{
    if (!lhs) return false;
    KWFrame* frame1 = lhs->frame(0);
    if (!frame1) return false;
    if (!rhs) return false;
    KWFrame* frame2 = rhs->frame(0);
    if (!frame2) return false;
    KoPoint p1 = frame1->topLeft();
    KoPoint p2 = frame2->topLeft();
    return (p1.y() < p2.y() || (p1.y() == p2.y() && p1.x() < p2.x()));
}

/******************************************************************/
/* Class: KWDocDocListViewItem                                    */
/******************************************************************/

KWDocListViewItem::KWDocListViewItem(Q3ListViewItem* parent, const QString& text)
    : K3ListViewItem(parent, text)
{
}

KWDocListViewItem::KWDocListViewItem(Q3ListViewItem* parent, Q3ListViewItem* after, const QString& text )
    :K3ListViewItem(parent, after, text)
{
}

KWDocListViewItem::KWDocListViewItem(Q3ListView* parent, const QString& text)
    :K3ListViewItem(parent, text)
{
}

void KWDocListViewItem::deleteAllChildren()
{
    if ( childCount() > 0 )
    {
        Q3ListViewItem *child = firstChild();
        Q3ListViewItem *delChild;

        while( child )
        {
            delChild = child;
            child = child->nextSibling();
            delete delChild;
        }
    }
}

KWDocument* KWDocListViewItem::doc()
{
    return dynamic_cast<KWDocStructTree *>(listView())->doc();
}

KWGUI* KWDocListViewItem::gui()
{
    return dynamic_cast<KWDocStructTree *>(listView())->gui();
}


/******************************************************************/
/* Class: KWDocStructParagItem                                    */
/******************************************************************/

KWDocStructParagItem::KWDocStructParagItem(Q3ListViewItem* parent, const QString& text, KWTextParag* parag )
    : KWDocListViewItem(parent, text), m_parag(parag)
{
}

KWDocStructParagItem::KWDocStructParagItem(Q3ListViewItem* parent, Q3ListViewItem* after,
    const QString& text, KWTextParag* parag )
    : KWDocListViewItem(parent, after, text), m_parag(parag)
{
}

void KWDocStructParagItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
    {
        KWTextFrameSet* fs = m_parag->kwTextDocument()->textFrameSet();
        gui()->getView()->openDocStructurePopupMenu(p, fs, m_parag);
    }
}

void KWDocStructParagItem::selectItem()
{
    KWTextFrameSet* fs = m_parag->kwTextDocument()->textFrameSet();
    QPoint iPoint = m_parag->rect().topLeft(); // small bug if a paragraph is cut between two pages.
    KoPoint dPoint;
    fs->internalToDocument(iPoint, dPoint);
    QPoint nPoint = fs->kWordDocument()->zoomPoint(dPoint);
    gui()->canvasWidget()->scrollToOffset(fs->kWordDocument()->unzoomPoint(nPoint));

}

void KWDocStructParagItem::editItem()
{
    gui()->canvasWidget()->editTextFrameSet(m_parag->kwTextDocument()->textFrameSet(), m_parag, 0);
}

void KWDocStructParagItem::deleteItem()
{
    editItem();
    KWTextFrameSetEdit *edit = dynamic_cast<KWTextFrameSetEdit *>(gui()->canvasWidget()->currentFrameSetEdit());
    if (edit) {
#warning TODO implement deleteParagraph
#if 0 // doesn't exist!!!
        edit->textView()->deleteParagraph(m_parag);
#endif
        doc()->refreshDocStructure(TextFrames | Tables);
    }
}

void KWDocStructParagItem::editProperties()
{
    gui()->canvasWidget()->editTextFrameSet(m_parag->kwTextDocument()->textFrameSet(), m_parag, 0);
    gui()->getView()->formatParagraph();
}

/******************************************************************/
/* Class: KWDocStructTextFrameItem                                */
/******************************************************************/

KWDocStructTextFrameItem::KWDocStructTextFrameItem(Q3ListViewItem* parent, const QString& text,
    KWTextFrameSet* frameset, KWFrame* frame)
    : KWDocListViewItem(parent, text), m_frame(frame), m_frameset(frameset)
{
}

KWDocStructTextFrameItem::KWDocStructTextFrameItem(Q3ListViewItem* parent, Q3ListViewItem* after,
    const QString& text, KWTextFrameSet* frameset, KWFrame* frame)
    : KWDocListViewItem(parent, after, text), m_frame(frame), m_frameset(frameset)
{
}

void KWDocStructTextFrameItem::setupTextParags()
{
    // Build a list of pointers to paragraphs.
    Q3PtrList<KWTextParag> paragPtrs;
    paragPtrs.setAutoDelete(false);
    KoTextParag* textParag = 0;
    KoTextParag* lastParag = 0;
    int index = 0;
    // Get upper left corner of the frame and get coordinates just inside it.
    KoPoint dPoint = m_frame->topLeft() + KoPoint(2,2);
    // Get the first paragraph of the frame.
    m_frameset->findPosition(dPoint, textParag, index);
    // Get lower right corner of the frame and get coordinate just inside it.
    dPoint = m_frame->bottomRight() - KoPoint(2,2);
    // Get the last paragraph of the frame.
    m_frameset->findPosition(dPoint, lastParag, index);
    if (lastParag) {
        while (textParag) {
            KWTextParag* parag = dynamic_cast<KWTextParag *>(textParag);
            if (parag) {
                // Don't display an empty paragraph.
                QString text = parag->toString().trimmed();
                if ( text.length() > 0)
                    paragPtrs.append(parag);
            }
            if (textParag == lastParag)
                textParag = 0;
            else
                textParag = textParag->next();
        }
    }

    // Remove deleted paragraphs from the listview.
    KWDocStructParagItem* item = dynamic_cast<KWDocStructParagItem *>(firstChild());
    KWDocStructParagItem* delItem;
    while (item) {
        delItem = item;
        item = dynamic_cast<KWDocStructParagItem *>(item->nextSibling());
        if (paragPtrs.containsRef(delItem->parag()) == 0) delete delItem;
    }

    // Add new paragraphs to the list or update existing ones.
    KWDocStructParagItem* after = 0;
    for (uint j = 0; j < paragPtrs.count(); j++) {
        KWTextParag* parag = paragPtrs.at(j);
        QString text = parag->toString().trimmed();
        QString name = text.left(20);
        KoParagCounter* tmpCounter = parag->counter();
        if (tmpCounter)
            name.prepend(tmpCounter->text(parag) + ' ');
        KWDocStructParagItem* child = findTextParagItem(parag);
        if (child)
            child->setText(0, name);
        else {
            if (after)
                child = new KWDocStructParagItem(this, after, name, parag);
            else
                child = new KWDocStructParagItem(this, name, parag);
        }
        after = child;
    }
}

void KWDocStructTextFrameItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
        gui()->getView()->openDocStructurePopupMenu(p, m_frameset, 0);
}

void KWDocStructTextFrameItem::selectItem()
{
    gui()->canvasWidget()->scrollToOffset(m_frame->topLeft());
}

void KWDocStructTextFrameItem::editItem()
{
    KoTextParag* parag = 0L;
    int index = 0;
    // Get upperleft corner of the frame and get coordinates just inside it.
    KoPoint dPoint = m_frame->topLeft() + KoPoint(2,2);
    m_frameset->findPosition(dPoint, parag, index);
    gui()->canvasWidget()->editTextFrameSet(m_frameset, parag, 0);
}

void KWDocStructTextFrameItem::deleteItem()
{
    gui()->getView()->deleteFrameSet(m_frameset);
}

void KWDocStructTextFrameItem::editProperties()
{
     gui()->canvasWidget()->editFrameProperties(m_frameset);
}

KWDocStructParagItem* KWDocStructTextFrameItem::findTextParagItem(const KWTextParag* parag)
{
    if ( childCount() > 0 )
    {
        Q3ListViewItem *child = firstChild();
        while( child )
        {
            if (dynamic_cast<KWDocStructParagItem *>(child)->parag() == parag)
                return dynamic_cast<KWDocStructParagItem *>(child);
            child = child->nextSibling();
        }
    }
    return 0;
}


/******************************************************************/
/* Class: KWDocStructTextFrameSetItem                             */
/******************************************************************/

KWDocStructTextFrameSetItem::KWDocStructTextFrameSetItem(Q3ListViewItem* parent, const QString& text,
    KWTextFrameSet* frameset)
    : KWDocListViewItem(parent, text ), m_frameset(frameset)
{
}

KWDocStructTextFrameSetItem::KWDocStructTextFrameSetItem(Q3ListViewItem* parent, Q3ListViewItem* after,
    const QString& text, KWTextFrameSet* frameset)
    : KWDocListViewItem(parent, after, text), m_frameset(frameset)
{
}

void KWDocStructTextFrameSetItem::setupTextFrames()
{
    // TODO: KWTextFrameSet::frame() method returns frames in screen order?
    // Build a list of frame pointers.
    Q3PtrList<KWFrame> framePtrs;
    framePtrs.setAutoDelete(false);
    for (uint j = 0; j < m_frameset->frameCount(); j++)
        framePtrs.append(m_frameset->frame(j));

    // Remove deleted frames from the listview.
    KWDocStructTextFrameItem* item = dynamic_cast<KWDocStructTextFrameItem *>(firstChild());
    KWDocStructTextFrameItem* delItem;
    while (item) {
        delItem = item;
        item = dynamic_cast<KWDocStructTextFrameItem *>(item->nextSibling());
        if (framePtrs.containsRef(delItem->frame()) == 0) delete delItem;
    }

    // Add new frames to the list or update existing ones.
    KWDocument* dok = doc();
    KWDocStructTextFrameItem* after = 0;
    for (uint j = 0; j < framePtrs.count(); j++)
    {
        KWFrame* frame = framePtrs.at(j);
        QString name;
        if ( K3ListViewItem::parent()->firstChild() == this && dok->processingType() == KWDocument::WP )
        {
            if ( dok->numColumns() == 1 )
                name=i18n( "Page %1" ,QString::number(j + 1));
            else
                name=i18n( "Column %1" ,QString::number(j + 1));
        }
        else
            name=i18n( "Text Frame %1" ,QString::number(j + 1));
        KWDocStructTextFrameItem* child = findTextFrameItem(frame);
        if (child)
            child->setText(0, name);
        else {
            if (after)
                child = new KWDocStructTextFrameItem(this, after, name, m_frameset, frame);
            else
                child = new KWDocStructTextFrameItem(this, name, m_frameset, frame);
        }
        child->setupTextParags();
        after = child;
    }
}

void KWDocStructTextFrameSetItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
        gui()->getView()->openDocStructurePopupMenu(p, m_frameset, 0);
}

void KWDocStructTextFrameSetItem::selectItem()
{
    KWFrame* frame = m_frameset->frame(0);
    if (!frame) return;
    gui()->canvasWidget()->scrollToOffset(frame->topLeft());
}

void KWDocStructTextFrameSetItem::editItem()
{
    gui()->canvasWidget()->editTextFrameSet(m_frameset, 0L, 0);
}

void KWDocStructTextFrameSetItem::deleteItem()
{
    gui()->getView()->deleteFrameSet(m_frameset);
}

void KWDocStructTextFrameSetItem::editProperties()
{
    gui()->canvasWidget()->editFrameProperties(m_frameset);
}

KWDocStructTextFrameItem* KWDocStructTextFrameSetItem::findTextFrameItem(const KWFrame* frame)
{
    if ( childCount() > 0 )
    {
        Q3ListViewItem *child = firstChild();
        while( child )
        {
            if (dynamic_cast<KWDocStructTextFrameItem *>(child)->frame() == frame)
                return dynamic_cast<KWDocStructTextFrameItem *>(child);
            child = child->nextSibling();
        }
    }
    return 0;
}


/******************************************************************/
/* Class: KWDocStructTableItem                                    */
/******************************************************************/

KWDocStructTableItem::KWDocStructTableItem(Q3ListViewItem* parent, const QString& text,
    KWTableFrameSet* table)
    :KWDocListViewItem(parent, text), m_table(table)
{
}

KWDocStructTableItem::KWDocStructTableItem(Q3ListViewItem* parent, Q3ListViewItem* after,
    const QString& text, KWTableFrameSet* table)
    :KWDocListViewItem(parent, after, text), m_table(table)
{
}

void KWDocStructTableItem::setupCells()
{
    // TODO: KWTableFrameSet::cell() method returns cells in screen order?
    // Build a list of cell pointers.
    Q3PtrList<KWTextFrameSet> cellPtrs;
    cellPtrs.setAutoDelete(false);
    for (uint row = 0; row < m_table->getRows(); ++row)
        for (uint col = 0; col < m_table->getColumns(); ++ col) {
            KWTextFrameSet* cell = m_table->cell(row, col);
            if (cell)
                cellPtrs.append(cell);
        }

    // Remove deleted cells from the listview.
    KWDocStructTextFrameItem* item = dynamic_cast<KWDocStructTextFrameItem *>(firstChild());
    KWDocStructTextFrameItem* delItem;
    while (item) {
        delItem = item;
        item = dynamic_cast<KWDocStructTextFrameItem *>(item->nextSibling());
        if (cellPtrs.containsRef(delItem->frameSet()) == 0) delete delItem;
    }

    // Add new cells to the list or update existing ones.
    // Note we skip over the frameset and add the frame instead,
    // as every cell has exactly one frame in the frameset.
    KWDocStructTextFrameItem* child;
    KWDocStructTextFrameItem* after = 0;
    for (uint j = 0; j < cellPtrs.count(); j++)
    {
        KWTextFrameSet* cell = cellPtrs.at(j);
        KWFrame* frame = cell->frame(0);
        if (frame) {
            QString name = cell->name();
            child = findCellItem(cell);
            if (child)
                child->setText(0, name);
            else {
                if (after)
                    child = new KWDocStructTextFrameItem(this, after, name, cell, frame);
                else
                    child = new KWDocStructTextFrameItem(this, name, cell, frame);
            }
            child->setupTextParags();
            after = child;
        }
    }
}

void KWDocStructTableItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
        gui()->getView()->openDocStructurePopupMenu(p, m_table, 0);
}

void KWDocStructTableItem::selectItem()
{
    KWFrame* frame = m_table->cell( 0, 0 )->frame( 0 );
    gui()->canvasWidget()->scrollToOffset(frame->topLeft());
}

void KWDocStructTableItem::editItem()
{
    //activate the first cell
    gui()->canvasWidget()->editTextFrameSet(m_table->cell(0,0), 0L, 0);
}

void KWDocStructTableItem::deleteItem()
{
    // TODO: The following statement isn't working for some reason.
    gui()->getView()->deselectAllFrames();
    gui()->getView()->deleteFrameSet(m_table);
}

void KWDocStructTableItem::editProperties()
{
     gui()->canvasWidget()->editFrameProperties(m_table);
}

KWDocStructTextFrameItem* KWDocStructTableItem::findCellItem(const KWTextFrameSet* cell)
{
    if ( childCount() > 0 )
    {
        Q3ListViewItem *child = firstChild();
        while( child )
        {
            if (dynamic_cast<KWDocStructTextFrameItem *>(child)->frameSet() == cell)
                return dynamic_cast<KWDocStructTextFrameItem *>(child);
            child = child->nextSibling();
        }
    }
    return 0;
}


/******************************************************************/
/* Class: KWDocStructPictureItem                                  */
/******************************************************************/

KWDocStructPictureItem::KWDocStructPictureItem(Q3ListViewItem* parent, const QString& text,
    KWPictureFrameSet* pic)
    : KWDocListViewItem(parent, text), m_pic(pic)
{
}

void KWDocStructPictureItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
        gui()->getView()->openDocStructurePopupMenu(p, m_pic, 0);
}


void KWDocStructPictureItem::selectItem()
{
    KWFrame *frame = m_pic->frame(0);
    gui()->canvasWidget()->scrollToOffset(frame->topLeft() );

}

void KWDocStructPictureItem::editItem()
{
    // Pictures cannot be edited.  Edit Properties instead.
    editProperties();
}

void KWDocStructPictureItem::deleteItem()
{
    gui()->getView()->deleteFrameSet(m_pic);
}

void KWDocStructPictureItem::editProperties()
{
    gui()->canvasWidget()->editFrameProperties(m_pic);
}

/******************************************************************/
/* Class: KWDocStructFormulaItem                                  */
/******************************************************************/

KWDocStructFormulaItem::KWDocStructFormulaItem(Q3ListViewItem* parent, const QString& text,
    KWFormulaFrameSet* form)
    : KWDocListViewItem(parent, text), m_form(form)
{
}

void KWDocStructFormulaItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
        gui()->getView()->openDocStructurePopupMenu(p, m_form, 0);
}

void KWDocStructFormulaItem::selectItem()
{
    KWFrame* frame = m_form->frame( 0 );
    gui()->canvasWidget()->scrollToOffset(frame->topLeft());
}

void KWDocStructFormulaItem::editItem()
{
    // TODO: Formula has to be selected first to bring it into view. Bug?
    selectItem();
    gui()->canvasWidget()->editFrameSet(m_form);

}

void KWDocStructFormulaItem::deleteItem()
{
    gui()->getView()->deleteFrameSet(m_form);
}

void KWDocStructFormulaItem::editProperties()
{
    gui()->canvasWidget()->editFrameProperties(m_form);
}

/******************************************************************/
/* Class: KWDocStructPartItem                                     */
/******************************************************************/

KWDocStructPartItem::KWDocStructPartItem(Q3ListViewItem* parent, const QString& text,
    KWPartFrameSet* part)
    : KWDocListViewItem(parent, text), m_part(part)
{
}

void KWDocStructPartItem::contextMenu(Q3ListViewItem* item, const QPoint& p, int )
{
    if (item == this)
        gui()->getView()->openDocStructurePopupMenu(p, m_part, 0);
}


void KWDocStructPartItem::selectItem()
{
    KWFrame* frame = m_part->frame(0);
    gui()->canvasWidget()->scrollToOffset(frame->topLeft());
}

void KWDocStructPartItem::editItem()
{
    // TODO:
    // part->startEditing();
    editProperties();
}

void KWDocStructPartItem::deleteItem()
{
    gui()->getView()->deleteFrameSet(m_part);
}

void KWDocStructPartItem::editProperties()
{
    gui()->canvasWidget()->editFrameProperties(m_part);
}

/******************************************************************/
/* Class: KWDocStructRootItem                                     */
/******************************************************************/

KWDocStructRootItem::KWDocStructRootItem(Q3ListView* parent, const QString& text,
    TypeStructDocItem type )
    : KWDocListViewItem(parent, text), m_type(type)
{
    switch ( type ) {
        case Arrangement: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "tree_arrange", K3Icon::Small ) );
        } break;
        case TextFrames: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_text", K3Icon::Small ) );
        } break;
        case FormulaFrames: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_formula", K3Icon::Small ) );
        }break;
        case Tables: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "inline_table", K3Icon::Small ) );
        } break;
        case Pictures: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_image", K3Icon::Small ) );
        } break;
        case Embedded: {
            setPixmap( 0, KGlobal::iconLoader()->loadIcon( "frame_query", K3Icon::Small ) );
        } break;
    }
}

void KWDocStructRootItem::setOpen(bool o)
{
    if ( o )
    {
        switch (m_type)
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
    Q3ListViewItem::setOpen(o);
}

void KWDocStructRootItem::setupArrangement()
{
    deleteAllChildren();

    Q3IntDict<KWDocStructParagItem> parags;
    parags.setAutoDelete( false );

    KWFrameSet* frameset = 0L;
    KWTextParag* parag = 0L;
    KoTextDocument* textdoc = 0L;

    KWDocument* dok = doc();
    KWDocStructTextFrameSetItem *item = 0L;
    QString _name;

    for ( int i = dok->frameSetCount() - 1; i >= 0; i-- )
    {
        frameset = dok->frameSet( i );
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY && !frameset->groupmanager() && frameset->frameCount()>0)
        {
            KWTextFrameSet *tmpParag = dynamic_cast<KWTextFrameSet*> (frameset) ;
            item = new KWDocStructTextFrameSetItem( this, frameset->name(), tmpParag);
            textdoc= tmpParag->textDocument();
            parag = static_cast<KWTextParag *>(textdoc->firstParag());
            while ( parag )
            {
                KoParagCounter *tmpCounter = parag->counter();
                if (tmpCounter !=0  && (tmpCounter->style() != KoParagCounter::STYLE_NONE) &&  (tmpCounter->numbering() == KoParagCounter::NUM_CHAPTER) )
                {
                    int _depth = tmpCounter->depth();
                    if ( _depth == 0 )
                    {
                        if ( item->childCount() == 0 )
                            parags.replace( _depth, new KWDocStructParagItem( item,QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag) );
                        else
                            parags.replace( _depth, new KWDocStructParagItem( item, parags[ _depth ],QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ),parag) );
                    }
                    else
                    {
                        if (parags[ _depth - 1 ]==0)
                            parags.replace( _depth, new KWDocStructParagItem( item,QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ), parag) );
                        else if ( parags[ _depth - 1 ]->childCount() == 0 )
                            parags.replace( _depth, new KWDocStructParagItem( parags[ _depth - 1 ],QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ), parag) );
                        else
                            parags.replace( _depth, new KWDocStructParagItem( parags[ _depth - 1 ], parags[ _depth ],QString( tmpCounter->text(parag) + "  " +parag->string()->toString().mid( 0, parag->string()->length() ) ), parag) );
                    }
                }
                parag = static_cast<KWTextParag *>(parag->next());
            }
        }
    }

    if ( childCount() == 0 )
        ( void )new K3ListViewItem( this, i18n( "Empty" ) );

}

void KWDocStructRootItem::setupTextFrameSets()
{

    // Delete Empty item from list.
    Q3ListViewItem* lvItem = firstChild();
    if (lvItem && (lvItem->text(0) == i18n("Empty"))) delete lvItem;

    // Build a list of framesets ordered by their screen position (top left corner).
    KWDocument* dok = doc();
    QList<KWFrameSet*> orderedFrameSets;
    for ( int i = dok->frameSetCount() - 1; i >= 0; i-- ) {
        KWFrameSet* frameset = dok->frameSet(i);
        if ( frameset->type() == FT_TEXT && frameset->frameSetInfo() == KWFrameSet::FI_BODY &&
            !frameset->groupmanager() && frameset->frameCount()>0)

            orderedFrameSets.append(frameset);
    }
    qSort(orderedFrameSets.begin(), orderedFrameSets.end(), orderFrameSetByPosition);

    // Build a list of frameset pointers from the sorted list.
    Q3PtrList<KWTextFrameSet> frameSetPtrs;
    frameSetPtrs.setAutoDelete(false);
    for ( uint i = 0; i < orderedFrameSets.count(); i++ )
        frameSetPtrs.append(dynamic_cast<KWTextFrameSet *>(orderedFrameSets[i]));

    // Remove deleted framesets from the listview.
    KWDocStructTextFrameSetItem* item = dynamic_cast<KWDocStructTextFrameSetItem *>(firstChild());
    KWDocStructTextFrameSetItem* delItem;
    while (item) {
        delItem = item;
        item = dynamic_cast<KWDocStructTextFrameSetItem *>(item->nextSibling());
        if (frameSetPtrs.containsRef(delItem->frameSet()) == 0) delete delItem;
    }

    // Add new framesets to the list or update existing ones.
    KWDocStructTextFrameSetItem* after = 0L;
    for ( uint i = 0; i < orderedFrameSets.count(); i++ )
    {
        KWTextFrameSet* textFrameset = dynamic_cast<KWTextFrameSet *>(orderedFrameSets[i]);
        item = findTextFrameSetItem(textFrameset);
        if (item)
            item->setText(0, textFrameset->name());
        else {
            if (after)
                item = new KWDocStructTextFrameSetItem(
                    this, after, textFrameset->name(), textFrameset);
            else
                item = new KWDocStructTextFrameSetItem(this, textFrameset->name(), textFrameset);
        }
        after = item;
        item->setupTextFrames();
    }

    if ( childCount() == 0 )
        ( void )new K3ListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupFormulaFrames()
{
    deleteAllChildren();

    KWFrameSet* frameset = 0L;
    QString _name;
    KWDocStructFormulaItem* child;
    KWDocument* dok = doc();

    for ( int i = dok->frameSetCount() - 1; i >= 0; i-- )
    {
        frameset = dok->frameSet( i );
        if ( frameset->type() == FT_FORMULA &&
            frameset->frameCount()>0  )
        {
            _name=i18n("Formula Frame %1",QString::number(i+1));
            child = new KWDocStructFormulaItem(this, _name, dynamic_cast<KWFormulaFrameSet*>( frameset ));
        }
    }

    if ( childCount() == 0 )
        ( void )new K3ListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupTables()
{
    // Delete Empty item from list.
    Q3ListViewItem* lvItem = firstChild();
    if (lvItem && (lvItem->text(0) == i18n("Empty"))) delete lvItem;

    // Build a list of framesets ordered by their screen position (top left corner).
    KWDocument* dok = doc();
    QList<KWFrameSet *> orderedFrameSets;
    for ( int i = dok->frameSetCount() - 1; i >= 0; i-- ) {
        KWFrameSet* frameset = dok->frameSet(i);
        if ( frameset->type() == FT_TABLE)
            orderedFrameSets.append(frameset);
    }
    qSort(orderedFrameSets.begin(), orderedFrameSets.end(), orderFrameSetByPosition);

    // Build a list of table pointers from the sorted list.
    Q3PtrList<KWTableFrameSet> frameSetPtrs;
    frameSetPtrs.setAutoDelete(false);
    for ( uint i = 0; i < orderedFrameSets.count(); i++ )
        frameSetPtrs.append(dynamic_cast<KWTableFrameSet *>(orderedFrameSets[i]));

    // Remove deleted tables from the listview.
    KWDocStructTableItem* item = dynamic_cast<KWDocStructTableItem *>(firstChild());
    KWDocStructTableItem* delItem;
    while (item) {
        delItem = item;
        item = dynamic_cast<KWDocStructTableItem *>(item->nextSibling());
        if (frameSetPtrs.containsRef(delItem->table()) == 0) delete delItem;
    }

    // Add new framesets to the list or update existing ones.
    KWDocStructTableItem* after = 0L;
    for ( uint i = 0; i < orderedFrameSets.count(); i++ )
    {
        KWTableFrameSet* tableFrameset = dynamic_cast<KWTableFrameSet *>(orderedFrameSets[i]);
        item = findTableItem(tableFrameset);
        if (item)
            item->setText(0, tableFrameset->name());
        else {
            if (after)
                item = new KWDocStructTableItem(
                    this, after, tableFrameset->name(), tableFrameset);
            else
                item = new KWDocStructTableItem(this, tableFrameset->name(), tableFrameset);
        }
        after = item;
        item->setupCells();
    }

    if ( childCount() == 0 )
        ( void )new K3ListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupPictures()
{
    deleteAllChildren();

    KWFrameSet* frameset = 0L;
    QString _name;
    KWDocStructPictureItem* child;
    KWDocument* dok = doc();

    int j = 0;
    for ( int i = dok->frameSetCount() - 1; i >= 0; i-- )
    {
        frameset = dok->frameSet( i );
        if ( frameset->type() == FT_PICTURE && frameset->frameCount()>0)
        {
            _name=i18n("Picture (%1) %2",static_cast<KWPictureFrameSet*>( frameset )->key().filename(),++j);
            child = new KWDocStructPictureItem(this, _name, dynamic_cast<KWPictureFrameSet*>( frameset ));
        }
    }

    if ( childCount() == 0 )
        ( void )new K3ListViewItem( this, i18n( "Empty" ) );
}

void KWDocStructRootItem::setupEmbedded()
{
    deleteAllChildren();

    KWFrameSet* frameset = 0L;
    QString _name;
    KWDocStructPartItem* child;
    KWDocument* dok = doc();

    for ( int i = dok->frameSetCount() - 1; i >= 0; i-- )
    {
        frameset = dok->frameSet( i );
        if ( frameset->type() == FT_PART && frameset->frameCount()>0)
        {
            // Use the name of the frameset as the entry for the object.
            _name=frameset->name();
            child = new KWDocStructPartItem(this, _name, dynamic_cast<KWPartFrameSet*>( frameset ));
        }
    }

    if ( childCount() == 0 )
        ( void )new K3ListViewItem( this, i18n( "Empty" ) );
}

KWDocStructTextFrameSetItem* KWDocStructRootItem::findTextFrameSetItem(const KWFrameSet* frameset)
{
    if ( childCount() > 0 )
    {
        Q3ListViewItem *child = firstChild();
        while( child )
        {
            if (dynamic_cast<KWDocStructTextFrameSetItem *>(child)->frameSet() == frameset)
                return dynamic_cast<KWDocStructTextFrameSetItem *>(child);
            child = child->nextSibling();
        }
    }
    return 0;
}

KWDocStructTableItem* KWDocStructRootItem::findTableItem(const KWFrameSet* frameset)
{
    if ( childCount() > 0 )
    {
        Q3ListViewItem *child = firstChild();
        while( child )
        {
            if (dynamic_cast<KWDocStructTableItem *>(child)->table() == frameset)
                return dynamic_cast<KWDocStructTableItem *>(child);
            child = child->nextSibling();
        }
    }
    return 0;
}



/******************************************************************/
/* Class: KWDocStructTree                                         */
/******************************************************************/

KWDocStructTree::KWDocStructTree(QWidget* parent, KWDocument* doc, KWGUI* gui)
    : K3ListView(parent), m_doc(doc), m_gui(gui)
{
    embedded = new KWDocStructRootItem( this, i18n( "Embedded Objects" ), Embedded);
    formulafrms = new KWDocStructRootItem( this, i18n( "Formula Frames" ), FormulaFrames);
    tables = new KWDocStructRootItem( this, i18n( "Tables" ), Tables);
    pictures = new KWDocStructRootItem( this, i18n( "Pictures" ), Pictures);
    textfrms = new KWDocStructRootItem( this, i18n( "Text Frames/Frame Sets" ), TextFrames);
    // arrangement = new KWDocStructRootItem( this, i18n( "Arrangement" ), Arrangement);

    addColumn( i18n( "Document Structure" ) );
    setFullWidth( true );

    connect( this, SIGNAL( doubleClicked(Q3ListViewItem*) ),
        this, SLOT( slotDoubleClicked(Q3ListViewItem*)) );
    connect( this, SIGNAL( returnPressed(Q3ListViewItem*) ),
        this, SLOT( slotReturnPressed(Q3ListViewItem* )) );
    connect( this, SIGNAL(rightButtonClicked(Q3ListViewItem*, const QPoint&,int)),
        this, SLOT( slotRightButtonClicked(Q3ListViewItem *, const QPoint&, int)));
    connect( this, SIGNAL(contextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)),
        this, SLOT(slotContextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)) );
}

KWDocStructTree::~KWDocStructTree()
{
    delete embedded;
    delete formulafrms;
    delete tables;
    delete pictures;
    delete textfrms;
    // delete arrangement;
}

void KWDocStructTree::setup()
{
    setRootIsDecorated( true );
    setSorting( -1 );
    refreshTree((int)(TextFrames | FormulaFrames | Tables | Pictures | Embedded));
}

void KWDocStructTree::refreshTree(int type)
{
    // TODO: Saving current position by listview item text doesn't work if an item is renamed.
    QString curItemText;
    if (currentItem()) curItemText = currentItem()->text(0);
    // if(((int)Arrangement) & type)
    //     arrangement->setupArrangement();
    if(((int)TextFrames) & type)
        textfrms->setupTextFrameSets();
    if(((int)FormulaFrames) & type)
        formulafrms->setupFormulaFrames();
    if(((int)Tables) & type)
        tables->setupTables();
    if(((int)Pictures) & type)
        pictures->setupPictures();
    if(((int)Embedded) & type)
        embedded->setupEmbedded();
    if (!curItemText.isEmpty()) {
        Q3ListViewItem* item = findItem(curItemText, 0);
        if (item) setCurrentItem(item);
    }
}

void KWDocStructTree::selectItem()
{
    Q3ListViewItem* select = currentItem ();
    KWDocListViewItem* tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->selectItem();
}

void KWDocStructTree::editItem()
{
    Q3ListViewItem* select = currentItem();
    KWDocListViewItem* tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->editItem();
}


void KWDocStructTree::deleteItem()
{
    Q3ListViewItem* select = currentItem();
    KWDocListViewItem *tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->deleteItem();
}

void KWDocStructTree::editProperties()
{
    Q3ListViewItem* select = currentItem();
    KWDocListViewItem* tmp = dynamic_cast<KWDocListViewItem *>(select);
    if ( tmp )
        tmp->editProperties();
}

void KWDocStructTree::slotContextMenu(K3ListView* lv, Q3ListViewItem* i, const QPoint& p)
{
    if (lv != this)
        return;
    KWDocListViewItem *item = dynamic_cast<KWDocListViewItem *>(i);
    if (item)
        item->contextMenu(item, p, 0);
}

void KWDocStructTree::slotRightButtonClicked(Q3ListViewItem* i, const QPoint& p, int)
{
    KWDocListViewItem* item = dynamic_cast<KWDocListViewItem *>(i);
    if (item)
        item->contextMenu(item, p, 0);
}

void KWDocStructTree::slotDoubleClicked(Q3ListViewItem* i)
{
    KWDocListViewItem* item = dynamic_cast<KWDocListViewItem *>(i);
    if (item)
        item->selectItem();
}

void KWDocStructTree::slotReturnPressed(Q3ListViewItem* i)
{
    KWDocListViewItem* item = dynamic_cast<KWDocListViewItem *>(i);
    if (item) {
        item->editItem();
        //return focus to canvas.
        m_gui->canvasWidget()->setFocus();
    }
}


/******************************************************************/
/* Class: KWDocStruct                                             */
/******************************************************************/

KWDocStruct::KWDocStruct(QWidget* parent, KWDocument* doc, KWGUI* gui)
    : QWidget(parent), m_doc(doc), m_gui(gui)
{
    m_layout = new Q3VBoxLayout( this );

    KToolBar* searchBar = new KToolBar( this );
    //searchBar->setFlat( true );
    //searchBar->setMovingEnabled( false );

#warning finish porting ( KToolBarButton -> QToolButton, K3ListViewSearchLine )
#if 0
    KToolBarButton* eraseButton = new KToolBarButton( "locationbar_erase", 0, searchBar );
    m_tree = new KWDocStructTree( this, doc, gui );
    m_tree->setAlternateBackground( KGlobalSettings::alternateBackgroundColor() );
    K3ListViewSearchLine* searchLine = new K3ListViewSearchLine( searchBar, m_tree );
    searchBar->setStretchableWidget( searchLine );
    connect( eraseButton, SIGNAL( clicked() ), searchLine, SLOT( clear() ) );

    m_layout->addWidget(searchBar);
    m_layout->addWidget(m_tree);
#endif
    m_tree->setup();
    dirtyTreeTypes = 0;
}

void KWDocStruct::paintEvent (QPaintEvent* ev)
{
    if (dirtyTreeTypes) {
        m_tree->refreshTree(dirtyTreeTypes);
        dirtyTreeTypes = 0;
    }
    QWidget::paintEvent(ev);
}

void KWDocStruct::refreshTree(int type)
{
    if ((dirtyTreeTypes | type) != dirtyTreeTypes) {
        dirtyTreeTypes |= type;
        update();
    }
}

void KWDocStruct::refreshEntireTree()
{
    refreshTree((int)(TextFrames | FormulaFrames | Tables | Pictures | Embedded));
}

void KWDocStruct::setFocusHere()
{
    if (m_tree)
        if (m_tree->isVisible()) m_tree->setFocus();
}

void KWDocStruct::selectItem()
{
    m_tree->selectItem();
}

void KWDocStruct::editItem()
{
    m_tree->editItem();
}

void KWDocStruct::deleteItem()
{
    m_tree->deleteItem();
}

void KWDocStruct::editProperties()
{
    m_tree->editProperties();
}
