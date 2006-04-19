/* This file is part of the KOffice project
 * Copyright (C) 2005 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; version 2.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "KWFrameView.h"
#include "KWFrameViewManager.h"
#include "KWFrame.h"
#include "KWFrameSet.h"
#include "KWTextFrameSet.h"
#include "KWTableFrameSet.h"
#include "KWPartFrameSet.h"
#include "KWVariable.h"
#include "KWPictureFrameSet.h"
#include "KWViewMode.h"
#include "KWDocument.h"
#include "KWCanvas.h"

#include "KoZoomHandler.h"

#include <klocale.h>
#include <kactionclasses.h>

#include <qapplication.h>
#include <q3popupmenu.h>
//Added by qt3to4:
#include <Q3PtrList>

static const double HORIZONTAL_SNAP = 6; // horizontal snap zone (in pt)
static const double VERTICAL_SNAP = 6; // vertical snap zone (in pt)

KWFrameView::KWFrameView(KWFrameViewManager *parent, KWFrame *frame) {
    m_manager = parent;
    Q_ASSERT(frame);
    Q_ASSERT(frame->frameSet());
    m_frame = frame;
    m_selected = false;
    if(frame->frameSet()->groupmanager() || dynamic_cast<KWTableFrameSet *>(frame->frameSet()) != 0)
        m_policy = new TableFramePolicy(this);
    else if(dynamic_cast<KWTextFrameSet *>(frame->frameSet()) != 0)
        m_policy = new TextFramePolicy(this);
    else if(dynamic_cast<KWPartFrameSet *>(frame->frameSet()) != 0)
        m_policy = new PartFramePolicy(this);
    else if(dynamic_cast<KWPictureFrameSet *>(frame->frameSet()) != 0)
        m_policy = new ImageFramePolicy(this);
    else {
        m_policy = new TextFramePolicy(this);
        kWarning() << "Unknown frameset supplied!" << endl;
    }
}

KWFrameView::~KWFrameView() {
    delete m_policy;
}

bool KWFrameView::isBorderHit(const KoPoint &point) const {
    return hit(point, true, true);
}

bool KWFrameView::contains(const KoPoint &point, bool fuzzy) const {
    return hit(point, fuzzy, false);
}

bool KWFrameView::hit(const KoPoint &point, bool fuzzy , bool borderOnly) const {
    //kDebug() << "hit " << point << " " << fuzzy << ", " << borderOnly << endl;
    double hs = 0, vs =0;
    if(fuzzy) {
        hs = HORIZONTAL_SNAP;
        if(frame()->width() < HORIZONTAL_SNAP * 3)
            hs = frame()->width() / 3;
        vs = VERTICAL_SNAP;
        if(frame()->height() < VERTICAL_SNAP * 3)
            vs = frame()->height() / 3;
    }
    if(point.x() < frame()->x() - hs)
        return false;
    if(point.x() > frame()->right() + hs)
        return false;
    if(point.y() < frame()->y() - vs)
        return false;
    if(point.y() > frame()->bottom() + vs)
        return false;

    if(borderOnly) { // also exclude inner part.
        if(point.x() > frame()->x() + hs && point.x() < frame()->right() - hs &&
                point.y() > frame()->y() + vs && point.y() < frame()->bottom() - vs )
            return false;
    }
    return true;
}

MouseMeaning KWFrameView::mouseMeaning( const KoPoint &point, int keyState ) {
    if(isBorderHit(point)) {
        MouseMeaning mm = m_policy->mouseMeaningOnBorder(point, keyState);
        if(mm != MEANING_NONE && ( frame()->frameSet()->isProtectSize() ||
                frame()->frameSet()->isMainFrameset() || frame()->frameSet()->isAHeader() ||
                frame()->frameSet()->isAFooter() ))
            mm = MEANING_FORBIDDEN;
        return mm;
    }
    if(hit(point, false, false))
        return m_policy->mouseMeaning(point, keyState);
    return MEANING_NONE;
}

void KWFrameView::setSelected(bool selected, MouseMeaning selectPolicy) {
    m_policy->setSelected(selectPolicy);
    if( m_selected == selected ) return;
    m_manager->slotFrameSelectionChanged();
    m_selected = selected;
}

void KWFrameView::showPopup( const KoPoint &point, KWView *view, const QPoint &popupPoint) const {
    view->unplugActionList( "tableactions" );
    view->unplugActionList( "frameset_type_action" );
    Q3PopupMenu *popup = m_policy->createPopup(point, view);
    Q_ASSERT(popup);
    popup->popup(popupPoint);
}

void KWFrameView::paintFrameAttributes(QPainter *painter, const QRect &crect, KWViewMode *vm, KoZoomHandler *zh) {
    if( !m_selected )
        return;

    class ResizeHandle {
      private:
        const int GRIP_SIZE;
        bool readOnly;
      public:
        ResizeHandle(KWFrameView *fv) : GRIP_SIZE(6) {
            KWFrameSet *fs = fv->frame()->frameSet();
            readOnly = fs->isProtectSize() || fs->isMainFrameset() ||
                fs->isAHeader() || fs->isAFooter() || fs->isFloating();
        }
        void paint(QPainter *p, int x, int y) {
            p->setPen( QPen( Qt::black, 1, Qt::SolidLine ) );
            p->setBrush( QApplication::palette().color( QPalette::Active, QColorGroup::Highlight ) );
            p->drawRect( x, y, GRIP_SIZE, GRIP_SIZE );
            if( readOnly ) { //protected frame
                QBrush brush = QApplication::palette().color( QPalette::Active,QColorGroup::Base );
                p->fillRect( x+1, y+1, GRIP_SIZE-2, GRIP_SIZE-2, brush );
            }
        }
    };

    const QRect frameRect = vm->normalToView( zh->zoomRect( *frame() ) );
    if ( crect.intersects( frameRect ) )
    {
        ResizeHandle handle(this);

        const int width = frameRect.width();
        const int height = frameRect.height();
        for(int y=0; y < 3; y++) {
            int offsetY = -1 + y + frameRect.y();
            if(y > 0)  offsetY += (height - 6) / (y == 1 ? 2:1);
            for(int x=0; x < 3; x++) {
                if(x == 1 && y == 1) continue; // don't draw in the center of the frame.
                int offsetX = -1 + x + frameRect.x();
                if(x > 0)  offsetX += (width - 6) / (x == 1 ? 2:1);
                handle.paint(painter, offsetX, offsetY);
            }
        }
    }
}

// *********** Policies *********
FramePolicy::FramePolicy(KWFrameView *view) {
    m_separator = new KActionSeparator();
    m_view = view;
}
void FramePolicy::addFloatingAction(KWView *view, QList<KAction*> &actionList) {
    if(m_view->frame()->frameSet()->isMainFrameset())
        return;
    actionList.append(m_separator);
    KToggleAction *action = dynamic_cast<KToggleAction*> (view->actionCollection()->
            action("inline_frame"));
    Q_ASSERT(action);
    KWFrameSet *parentFs = m_view->frame()->frameSet()->groupmanager() ?
        m_view->frame()->frameSet()->groupmanager() : m_view->frame()->frameSet();
    action->setChecked(parentFs->isFloating());
    actionList.append(action);
}
MouseMeaning FramePolicy::mouseMeaningOnBorder( const KoPoint &point, int keyState ) {
    Q_UNUSED(keyState);
    double hs = HORIZONTAL_SNAP;
    KWFrame *frame = m_view->frame();
    if(frame->width() < HORIZONTAL_SNAP * 3)
        hs = frame->width() / 3;
    double vs = VERTICAL_SNAP;
    if(frame->height() < VERTICAL_SNAP * 3)
        vs = frame->height() / 3;

    // Corners
    if( point.x() <= frame->x() + hs ) { // left edge
        if(point.y() <= frame->y() + vs)
            return MEANING_TOPLEFT;
        if(point.y() >= frame->bottom() - vs)
            return MEANING_BOTTOMLEFT;
        if( QABS(frame->y() + frame->height() / 2 - point.y()) <= vs )
            return MEANING_LEFT;
        return MEANING_MOUSE_MOVE;
    }
    if( point.x() >= frame->right() - hs) { // right edge
        if(point.y() <= frame->y() + vs)
            return MEANING_TOPRIGHT;
        if(point.y() >= frame->bottom() - vs)
            return MEANING_BOTTOMRIGHT;
        if( QABS(frame->y() + frame->height() / 2 - point.y()) <= vs )
            return MEANING_RIGHT;
        return MEANING_MOUSE_MOVE;
    }

    if( frame->y() + vs >= point.y() ) { // top edge
        if( QABS(frame->x() + frame->width() / 2 - point.x() ) <= hs )
            return MEANING_TOP;
        return MEANING_MOUSE_MOVE;
    }
    if( frame->bottom() - vs <= point.y() ) { // bottom edge
        if( QABS(frame->x() + frame->width() / 2 - point.x() ) <= hs )
            return MEANING_BOTTOM;
        return MEANING_MOUSE_MOVE;
    }
    return MEANING_NONE;
}

TableFramePolicy::TableFramePolicy(KWFrameView *view) : FramePolicy (view) {
}
MouseMeaning TableFramePolicy::mouseMeaning( const KoPoint &point, int keyState ) {
    Q_UNUSED(point);
    // Found a frame under the cursor
    // Ctrl -> select
    if ( keyState & Qt::ControlModifier )
        return MEANING_MOUSE_SELECT;
    // Shift _and_ at least a frame is selected already
    // (shift + no frame selected is used to select text)
    if ( (keyState & Qt::ShiftModifier) && m_view->parent()->selectedFrame() != 0 )
        return MEANING_MOUSE_SELECT;

    return MEANING_MOUSE_INSIDE_TEXT;
}
Q3PopupMenu* TableFramePolicy::createPopup( const KoPoint &point, KWView *view ) {
    view->plugActionList( "tableactions", view->tableActions() );
    if( m_view->isBorderHit(point) )
        return view->popupMenu("frame_popup_table");
    return view->popupMenu("text_popup");
}
MouseMeaning TableFramePolicy::mouseMeaningOnBorder(const KoPoint &point, int keyState) {
    KWFrame *frame = m_view->frame();
    double hs = HORIZONTAL_SNAP; // horizontal snap zone (in pt)
    double vs = VERTICAL_SNAP; // vertical snap zone (in pt)
    bool ctrl = keyState & Qt::ControlModifier;

    if ( QABS( frame->x() - point.x() ) < hs
            && point.y() >= frame->y() && point.y() <= frame->bottom() ) {
        if( static_cast<KWTableFrameSet::Cell *>(frame->frameSet())->firstColumn() == 0 )
            return MEANING_SELECT_ROW;
        if(!ctrl)
            return MEANING_RESIZE_COLUMN;
    }
    if ( QABS( frame->y() - point.y() ) < vs
            && point.x() >= frame->x() && point.x() <= frame->right() ) {
        if( static_cast<KWTableFrameSet::Cell *>(frame->frameSet())->firstRow() == 0 )
            return MEANING_SELECT_COLUMN;
        if(!ctrl)
            return MEANING_MOUSE_SELECT;
    }

    if (ctrl)
        return MEANING_MOUSE_SELECT;
    if ( QABS( frame->right() - point.x() ) < hs
            && point.y() >= frame->y() && point.y() <= frame->bottom() )
        return MEANING_RESIZE_COLUMN;
    if ( QABS( frame->bottom() - point.y() ) < vs
            && point.x() >= frame->x() && point.x() <= frame->right() )
        return MEANING_MOUSE_SELECT;
    return MEANING_NONE;
}
void TableFramePolicy::setSelected(MouseMeaning selectPolicy) {
    KWFrameSet *fs = m_view->frame()->frameSet();
    if( selectPolicy == MEANING_SELECT_COLUMN ) {
        unsigned int column = static_cast<KWTableFrameSet::Cell *>(fs)->firstColumn();
        for (KWTableFrameSet::TableIter cells(fs->groupmanager()); cells; ++cells) {
            if(cells->firstColumn() >= column && cells->lastColumn() <= column) {
                KWFrameView *fv = m_view->parent()->view(cells->frame(0));
                if(fv)
                    fv->setSelected(true);
            }
        }
    }
    else if( selectPolicy == MEANING_SELECT_ROW ) {
        unsigned int row = static_cast<KWTableFrameSet::Cell *>(fs)->firstRow();
        for (KWTableFrameSet::TableIter cells(fs->groupmanager()); cells; ++cells) {
            if(cells->firstRow() >= row && cells->lastRow() <= row) {
                KWFrameView *fv = m_view->parent()->view(cells->frame(0));
                if(fv)
                    fv->setSelected(true);
            }
        }
    }
    else if( selectPolicy == MEANING_SELECT_RANGE ) {
kDebug() << "not yet implemented; select table range\n"; // TODO
    }
}

PartFramePolicy::PartFramePolicy(KWFrameView *view) : FramePolicy (view) {
}
MouseMeaning PartFramePolicy::mouseMeaning( const KoPoint &point, int keyState ) {
    Q_UNUSED(point);
    // Clicking on a selected part frame, but not on its border -> either resize or "activate part"
    if( keyState & Qt::ControlModifier ) {
        return m_view->selected() ? MEANING_MOUSE_MOVE : MEANING_MOUSE_SELECT;
    }
    if ( m_view->selected() )
        return MEANING_ACTIVATE_PART;
    return MEANING_MOUSE_SELECT;
}
Q3PopupMenu* PartFramePolicy::createPopup( const KoPoint &point, KWView *view ) {
    Q_UNUSED(point);
    KWPartFrameSet *part = static_cast<KWPartFrameSet *>(m_view->frame()->frameSet());
    KActionSeparator *separator=new KActionSeparator();
    KActionCollection *actionCollection = view->actionCollection();
    QList<KAction*> actionList;
    actionList.append(separator);
    if( !part->protectContent() ) {
        KToggleAction *action = dynamic_cast<KToggleAction*>(actionCollection->action("embedded_store_internal"));
        Q_ASSERT(action);
        action->setChecked(part->getChild()->document()->storeInternal());
        action->setEnabled(part->getChild()->document()->hasExternURL());
        actionList.append(action);
    }
    addFloatingAction(view, actionList);
    view->plugActionList( "frameset_type_action", actionList );

    return view->popupMenu("frame_popup");
}

TextFramePolicy::TextFramePolicy(KWFrameView *view) : FramePolicy (view) {
}
MouseMeaning TextFramePolicy::mouseMeaning( const KoPoint &point, int keyState ) {
    if( (keyState & Qt::ControlModifier) == Qt::ControlModifier )
        return MEANING_MOUSE_SELECT;
    KWTextFrameSet *fs = dynamic_cast<KWTextFrameSet*>(m_view->frame()->frameSet());
    if(fs == 0 || fs->kWordDocument() == 0)
        return MEANING_MOUSE_INSIDE_TEXT;
    KWVariableCollection *varCol = fs->kWordDocument()->variableCollection();
    if (varCol->variableSetting()->displayLink() && varCol->variableSetting()->underlineLink()
            && fs->linkVariableUnderMouse( point ) )
        return MEANING_MOUSE_OVER_LINK;
    KoVariable* var = fs->variableUnderMouse(point);
    if ( var ) {
        KWFootNoteVariable * footNoteVar = dynamic_cast<KWFootNoteVariable *>( var );
        if ( footNoteVar )
            return MEANING_MOUSE_OVER_FOOTNOTE;
    }
    return MEANING_MOUSE_INSIDE_TEXT;
}
Q3PopupMenu* TextFramePolicy::createPopup( const KoPoint &point, KWView *view ) {
    if( m_view->isBorderHit(point) ) {
        KWFrameSet *fs = m_view->frame()->frameSet();
        KActionSeparator *separator=new KActionSeparator();
        KActionCollection *actionCollection = view->actionCollection();
        QList<KAction*> actionList;
        if(fs->isHeaderOrFooter()) {
            actionList.append(separator);
            actionList.append(actionCollection->action("configure_headerfooter"));
        }
        else if (fs->isFootEndNote()) {
            actionList.append(separator);
            KAction *action = actionCollection->action("goto_footendnote");
            action->setText( fs->isFootNote() ?
                    i18n( "Go to Footnote" ) : i18n( "Go to Endnote" ) );
            actionList.append(action);
        }
        addFloatingAction(view, actionList);
        view->plugActionList( "frameset_type_action", actionList );
        return view->popupMenu("frame_popup");
    }

    if ( view->getGUI()->canvasWidget()->currentFrameSetEdit()->frameSet()->type() == FT_FORMULA )
            return view->popupMenu("Formula");

    // yeah yeah, this is not great and needs a second look. (TZ)
    KWTextFrameSetEdit *fse = static_cast<KWTextFrameSetEdit*> (view->getGUI()->
            canvasWidget()->currentFrameSetEdit());

    QString word = fse->wordUnderCursor( *fse->cursor() );

    // Remove previous stuff
    view->unplugActionList( "datatools" );
    view->unplugActionList( "variable_action" );
    view->unplugActionList( "spell_result_action" );
    view->unplugActionList( "datatools_link" );

    // Those lists are stored in the KWView. Grab a ref to them.
    QList<KAction*> &actionList = view->dataToolActionList();
    QList<KAction*> &variableList = view->variableActionList();

    qDeleteAll( actionList );
    actionList.clear();
    qDeleteAll( variableList );
    variableList.clear();

    bool singleWord= false;
    KWDocument * doc = m_view->frame()->frameSet()->kWordDocument();
    actionList = fse->dataToolActionList(doc->instance(), word, singleWord);

    KoVariable* var = fse->variable();
    doc->variableCollection()->setVariableSelected(var);
    if ( var )
        variableList = doc->variableCollection()->popupActionList();

    if( variableList.count()>0) {
        view->plugActionList( "variable_action", variableList );
        return view->popupMenu("variable_popup");
    }

    //kDebug() << "TextFramePolicy::createPopup: plugging actionlist with " << actionList.count() << " actions" << endl;
    KoLinkVariable* linkVar = dynamic_cast<KoLinkVariable *>( var );
    if ( linkVar ) {
        view->plugActionList( "datatools_link", actionList );
        return view->popupMenu("text_popup_link");
    }
    view->plugActionList( "datatools", actionList );
    KoNoteVariable * noteVar = dynamic_cast<KoNoteVariable *>( var );
    if( noteVar )
        return view->popupMenu("comment_popup");

    KoCustomVariable * customVar = dynamic_cast<KoCustomVariable *>( var );
    if( customVar )
        return view->popupMenu("custom_var_popup");

    KWFootNoteVariable * footNoteVar = dynamic_cast<KWFootNoteVariable *>( var );
    if ( footNoteVar ) {
        view->changeFootNoteMenuItem( footNoteVar->noteType() == FootNote );
        return view->popupMenu("footnote_popup");
    }
    if ( singleWord ) {
        QList<KAction*> actionCheckSpellList = view->listOfResultOfCheckWord( word );
        if ( !actionCheckSpellList.isEmpty() ) {
            view->plugActionList( "spell_result_action", actionCheckSpellList );
            return view->popupMenu("text_popup_spell_with_result");
        }
        return view->popupMenu("text_popup_spell");
    }

    return view->popupMenu("text_popup");
}


ImageFramePolicy::ImageFramePolicy(KWFrameView *view) : FramePolicy (view) {
}
MouseMeaning ImageFramePolicy::mouseMeaning( const KoPoint &point, int keyState ) {
    Q_UNUSED(point);
    Q_UNUSED(keyState);
    return m_view->selected() ? MEANING_MOUSE_MOVE: MEANING_MOUSE_SELECT;
}
Q3PopupMenu* ImageFramePolicy::createPopup( const KoPoint &point, KWView *view ) {
    Q_UNUSED(point);
    KActionSeparator *separator=new KActionSeparator();
    KActionCollection *actionCollection = view->actionCollection();
    QList<KAction*> actionList;
    actionList.append(separator);
    KAction *action = actionCollection->action("change_picture");
    Q_ASSERT(action);
    actionList.append(action);
    action = actionCollection->action("save_picture");
    Q_ASSERT(action);
    actionList.append(action);
    addFloatingAction(view, actionList);
    view->plugActionList( "frameset_type_action", actionList );

    return view->popupMenu("frame_popup");
}
