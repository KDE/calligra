/* This file is part of the KDE project
   Copyright (C) 2003 Ulrich Kuettler <ulrich.kuettler@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwdoc.h"
#include "kwview.h"
#include "kwviewmode.h"
#include "kwcanvas.h"
#include "kwcommand.h"
#include "kwframe.h"
#include "defs.h"
#include "kwtextframeset.h"
#include "kwtableframeset.h"
#include "kwanchor.h"
#include "resizehandles.h"
#include <kotextobject.h> // for customItemChar!
#include <qpopupmenu.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>
#include <kformulaview.h>

#include <kcursor.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kparts/partmanager.h>
#include <kdebug.h>
#include <float.h>
#include "KWordFrameSetIface.h"
#include <dcopobject.h>
#include "qdrawutil.h"
#include "KWordTextFrameSetEditIface.h"
#include "KWordFormulaFrameSetIface.h"
#include "KWordFormulaFrameSetEditIface.h"
#include "KWordPictureFrameSetIface.h"
#include "KWordHorizontalLineFrameSetIface.h"

#include "kwformulaframe.h"

#include <assert.h>

// #ifdef __GNUC__
// #undef k_funcinfo
// #define k_funcinfo "[\033[36m" << __PRETTY_FUNCTION__ << "\033[m] "
// #endif

/******************************************************************/
/* Class: KWFormulaFrameSet                                       */
/******************************************************************/
KWFormulaFrameSet::KWFormulaFrameSet( KWDocument *_doc, const QString & name )
    : KWFrameSet( _doc ), m_changed( false ), m_edit( 0 )
{
    kdDebug() << k_funcinfo << endl;

    // The newly created formula is not yet part of the formula
    // document. It will be added when a frame is created.
    formula = _doc->getFormulaDocument()->createFormula( -1, false );

    // With the new drawing scheme (drawFrame being called with translated painter)
    // there is no need to move the KFormulaContainer anymore, it remains at (0,0).
    formula->moveTo( 0, 0 );

    connect( formula, SIGNAL( formulaChanged( double, double ) ),
             this, SLOT( slotFormulaChanged( double, double ) ) );
    connect( formula, SIGNAL( errorMsg( const QString& ) ),
             this, SLOT( slotErrorMessage( const QString& ) ) );
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Formula %1" ) );
    else
        m_name = name;

    /*
    if ( isFloating() ) {
        // we need to look for the anchor every time, don't cache this value.
        // undo/redo creates/deletes anchors
        KWAnchor * anchor = findAnchor( 0 );
        if ( anchor ) {
            KoTextFormat * format = anchor->format();
            formula->setFontSize( format->pointSize() );
        }
    }
    */
    QRect rect = formula->boundingRect();
    slotFormulaChanged(rect.width(), rect.height());
}

KWordFrameSetIface* KWFormulaFrameSet::dcopObject()
{
    if ( !m_dcop )
	m_dcop = new KWordFormulaFrameSetIface( this );

    return m_dcop;
}

KWFormulaFrameSet::~KWFormulaFrameSet()
{
    kdDebug() << k_funcinfo << endl;
    delete formula;
}

void KWFormulaFrameSet::addFrame( KWFrame *_frame, bool recalc )
{
    kdDebug() << k_funcinfo << endl;
    if ( formula ) {
        _frame->setWidth( formula->width() );
        _frame->setHeight( formula->height() );
    }
    KWFrameSet::addFrame( _frame, recalc );
    if ( formula ) {
        formula->registerFormula();
    }
}

void KWFormulaFrameSet::delFrame( unsigned int _num, bool remove, bool recalc )
{
    kdDebug() << k_funcinfo << endl;
    assert( _num == 0 );
    KWFrameSet::delFrame( _num, remove, recalc );
    formula->unregisterFormula();
}


void KWFormulaFrameSet::moveFrame( KWFrame* frame )
{
    formula->setDocumentPosition( frame->x(), frame->y()+formula->baseline() );
}


KWFrameSetEdit* KWFormulaFrameSet::createFrameSetEdit(KWCanvas* canvas)
{
    return new KWFormulaFrameSetEdit(this, canvas);
}

void KWFormulaFrameSet::drawFrameContents( KWFrame* /*frame*/,
                                           QPainter* painter, const QRect& crect,
                                           const QColorGroup& cg, bool onlyChanged,
                                           bool resetChanged,
                                           KWFrameSetEdit */*edit*/, KWViewMode * )
{
    if ( m_changed || !onlyChanged )
    {
        if ( resetChanged )
            m_changed = false;

        bool printing = painter->device()->devType() == QInternal::Printer;
        bool clipping = true;
        QPainter *p;
        QPixmap* pix = 0L;
        if ( printing ) {
            p = painter;
            clipping = painter->hasClipping();

            // That's unfortunate for formulas wider than the page.
            // However it helps a lot with ordinary formulas.
            painter->setClipping( false );
        }
        else {
            pix = doubleBufferPixmap( crect.size() );
            p = new QPainter( pix );
            p->translate( -crect.x(), -crect.y() );
        }

        if ( m_edit ) {
            //KWFormulaFrameSetEdit * formulaEdit = static_cast<KWFormulaFrameSetEdit *>(edit);
            if ( m_edit->getFormulaView() ) {
                m_edit->getFormulaView()->draw( *p, crect, cg );
            }
            else {
                formula->draw( *p, crect, cg );
            }
        }
        else {
            formula->draw( *p, crect, cg );
        }

        if ( !printing ) {
            p->end();
            delete p;
            painter->drawPixmap( crect.topLeft(), *pix );
        }
        else {
            painter->setClipping( clipping );
        }
    }
}


void KWFormulaFrameSet::slotFormulaChanged( double width, double height )
{
    if ( frames.isEmpty() )
        return;

    double oldWidth = frames.first()->width();
    double oldHeight = frames.first()->height();

    frames.first()->setWidth( width );
    frames.first()->setHeight( height );

    updateFrames();
    kWordDocument()->layout();
    if ( ( oldWidth != width ) || ( oldHeight != height ) ) {
        kWordDocument()->repaintAllViews( false );
        kWordDocument()->updateRulerFrameStartEnd();
    }

    m_changed = true;

    if ( !m_edit ) {
        // A change without a FrameSetEdit! This must be the result of
        // an undo. We need to evaluate.
        formula->startEvaluation();
    }
}

void KWFormulaFrameSet::slotErrorMessage( const QString& msg )
{
    KMessageBox::error( /*m_widget*/ 0, msg );
}

QDomElement KWFormulaFrameSet::save(QDomElement& parentElem, bool saveFrames)
{
    if ( frames.isEmpty() ) // Deleted frameset -> don't save
        return QDomElement();
    QDomElement framesetElem = parentElem.ownerDocument().createElement("FRAMESET");
    parentElem.appendChild(framesetElem);

    KWFrameSet::saveCommon(framesetElem, saveFrames);

    QDomElement formulaElem = parentElem.ownerDocument().createElement("FORMULA");
    framesetElem.appendChild(formulaElem);
    formula->save(formulaElem);
    return framesetElem;
}

void KWFormulaFrameSet::saveOasis(KoXmlWriter&, KoSavingContext&) const
{
    // TODO
}

void KWFormulaFrameSet::load(QDomElement& attributes, bool loadFrames)
{
    KWFrameSet::load(attributes, loadFrames);
    QDomElement formulaElem = attributes.namedItem("FORMULA").toElement();
    paste( formulaElem );
}

void KWFormulaFrameSet::paste( QDomNode& formulaElem )
{
    if (!formulaElem.isNull()) {
        if (formula == 0) {
            formula = m_doc->getFormulaDocument()->createFormula( -1, false );
            connect(formula, SIGNAL(formulaChanged(double, double)),
                    this, SLOT(slotFormulaChanged(double, double)));
            connect( formula, SIGNAL( errorMsg( const QString& ) ),
                     this, SLOT( slotErrorMessage( const QString& ) ) );
        }
        if ( !formula->load( formulaElem.firstChild().toElement() ) ) {
            kdError(32001) << "Error loading formula" << endl;
        }
    }
    else {
        kdError(32001) << "Missing FORMULA tag in FRAMESET" << endl;
    }
}

void KWFormulaFrameSet::zoom( bool forPrint )
{
    if ( !frames.isEmpty() ) {
        KWFrameSet::zoom( forPrint );
    }
}

void KWFormulaFrameSet::moveFloatingFrame( int frameNum, const KoPoint &position )
{
    kdDebug() << k_funcinfo << endl;
    KWFrameSet::moveFloatingFrame( frameNum, position );
    if ( !frames.isEmpty() ) {
        formula->setDocumentPosition( position.x(), position.y()+formula->baseline() );
    }
}

int KWFormulaFrameSet::floatingFrameBaseline( int /*frameNum*/ )
{
    if ( !frames.isEmpty() )
    {
        return m_doc->ptToLayoutUnitPixY( formula->baseline() );
    }
    return -1;
}

void KWFormulaFrameSet::setAnchorFormat( KoTextFormat* format, int /*frameNum*/ )
{
    if ( !frames.isEmpty() ) {
        formula->setFontSizeDirect( format->pointSize() );
    }
}

void KWFormulaFrameSet::showPopup( KWFrame *, KWView *view, const QPoint &point )
{
    QPopupMenu * popup = view->popupMenu("Formula");
    Q_ASSERT(popup);
    if (popup)
        popup->popup( point );
}


QPixmap* KWFormulaFrameSet::m_bufPixmap = 0;

// stolen from KWDocument
// However, I don't see if a formula frame can be an underlying
// frame. That is why I use my own buffer.
QPixmap* KWFormulaFrameSet::doubleBufferPixmap( const QSize& s )
{
    if ( !m_bufPixmap ) {
	int w = QABS( s.width() );
	int h = QABS( s.height() );
	m_bufPixmap = new QPixmap( w, h );
    } else {
	if ( m_bufPixmap->width() < s.width() ||
	     m_bufPixmap->height() < s.height() ) {
	    m_bufPixmap->resize( QMAX( s.width(), m_bufPixmap->width() ),
                                 QMAX( s.height(), m_bufPixmap->height() ) );
	}
    }

    return m_bufPixmap;
}


KWFormulaFrameSetEdit::KWFormulaFrameSetEdit(KWFormulaFrameSet* fs, KWCanvas* canvas)
        : KWFrameSetEdit(fs, canvas)
{
    formulaView = new KFormula::View( fs->getFormula() );

    connect( formulaView, SIGNAL( cursorChanged( bool, bool ) ),
             this, SLOT( cursorChanged( bool, bool ) ) );
    connect( fs->getFormula(), SIGNAL( leaveFormula( Container*, FormulaCursor*, int ) ),
             this, SLOT( slotLeaveFormula( Container*, FormulaCursor*, int ) ) );

    fs->m_edit = this;

    m_canvas->gui()->getView()->showFormulaToolbar(true);
    focusInEvent();
    dcop=0;
}

DCOPObject* KWFormulaFrameSetEdit::dcopObject()
{
    if ( !dcop )
	dcop = new KWordFormulaFrameSetEditIface( this );
    return dcop;
}

KWFormulaFrameSetEdit::~KWFormulaFrameSetEdit()
{
    formulaFrameSet()->m_edit = 0;
    focusOutEvent();
    // this causes a core dump on quit
    m_canvas->gui()->getView()->showFormulaToolbar(false);
    delete formulaView;
    formulaView = 0;
    formulaFrameSet()->getFormula()->startEvaluation();
    formulaFrameSet()->setChanged();
    m_canvas->repaintChanged( formulaFrameSet(), true );
    delete dcop;
}

const KFormula::View* KWFormulaFrameSetEdit::getFormulaView() const { return formulaView; }
KFormula::View* KWFormulaFrameSetEdit::getFormulaView() { return formulaView; }

void KWFormulaFrameSetEdit::keyPressEvent( QKeyEvent* event )
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::keyPressEvent" << endl;
    formulaView->keyPressEvent( event );
}

void KWFormulaFrameSetEdit::mousePressEvent( QMouseEvent* event,
                                             const QPoint&,
                                             const KoPoint& pos )
{
    // [Note that this method is called upon RMB and MMB as well, now]
    KoPoint tl = m_currentFrame->topLeft();
    formulaView->mousePressEvent( event, pos-tl );
}

void KWFormulaFrameSetEdit::mouseMoveEvent( QMouseEvent* event,
                                            const QPoint&,
                                            const KoPoint& pos )
{
    KoPoint tl = m_currentFrame->topLeft();
    formulaView->mouseMoveEvent( event, pos-tl );
}

void KWFormulaFrameSetEdit::mouseReleaseEvent( QMouseEvent* event,
                                               const QPoint&,
                                               const KoPoint& pos )
{
    KoPoint tl = m_currentFrame->topLeft();
    formulaView->mouseReleaseEvent( event, pos-tl );
}

void KWFormulaFrameSetEdit::focusInEvent()
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::focusInEvent" << endl;
    if ( formulaView != 0 ) {
        formulaView->focusInEvent(0);
    }
}

void KWFormulaFrameSetEdit::focusOutEvent()
{
    //kdDebug(32001) << "KWFormulaFrameSetEdit::focusOutEvent" <<
    //endl;
    if ( formulaView != 0 ) {
        formulaView->focusOutEvent(0);
    }
}

void KWFormulaFrameSetEdit::copy()
{
    formulaView->getDocument()->copy();
}

void KWFormulaFrameSetEdit::cut()
{
    formulaView->getDocument()->cut();
}

void KWFormulaFrameSetEdit::paste()
{
    formulaView->getDocument()->paste();
}

void KWFormulaFrameSetEdit::selectAll()
{
    formulaView->slotSelectAll();
}

void KWFormulaFrameSetEdit::moveHome()
{
    formulaView->moveHome( KFormula::WordMovement );
}
void KWFormulaFrameSetEdit::moveEnd()
{
    formulaView->moveEnd( KFormula::WordMovement );
}

void KWFormulaFrameSetEdit::exitLeft()
{
    if ( formulaFrameSet()->isFloating() ) {
        KWAnchor* anchor = formulaFrameSet()->findAnchor( 0 );
        int index = anchor->index();
        KoTextParag *parag = static_cast<KoTextParag*>( anchor->paragraph() );
        m_canvas->editTextFrameSet( formulaFrameSet()->anchorFrameset(), parag, index );
    }
}

void KWFormulaFrameSetEdit::exitRight()
{
    if ( formulaFrameSet()->isFloating() ) {
        KWAnchor* anchor = formulaFrameSet()->findAnchor( 0 );
        int index = anchor->index();
        KoTextParag *parag = static_cast<KoTextParag*>( anchor->paragraph() );
        m_canvas->editTextFrameSet( formulaFrameSet()->anchorFrameset(), parag, index+1 );
    }
}


void KWFormulaFrameSetEdit::exitAbove()
{
    exitLeft();
}


void KWFormulaFrameSetEdit::exitBelow()
{
    exitRight();
}


void KWFormulaFrameSetEdit::removeFormula()
{
    if ( formulaFrameSet()->isFloating() ) {
        KWCanvas* canvas = m_canvas;

        // This call will destroy us! We cannot use 'this' afterwards!
        exitRight();

        QKeyEvent keyEvent( QEvent::KeyPress, Key_Backspace, 0, 0 );
        canvas->currentFrameSetEdit()->keyPressEvent( &keyEvent );
    }
}

void KWFormulaFrameSetEdit::cursorChanged( bool visible, bool /*selecting*/ )
{
    if ( visible ) {
        if ( m_currentFrame )
        {
            // Add the cursor position to the (zoomed) frame position
            QPoint nPoint = frameSet()->kWordDocument()->zoomPoint( m_currentFrame->topLeft() );
            nPoint += formulaView->getCursorPoint();
            // Apply viewmode conversion
            QPoint p = m_canvas->viewMode()->normalToView( nPoint );
            m_canvas->ensureVisible( p.x(), p.y() );
        }
    }
    formulaFrameSet()->setChanged();
    m_canvas->repaintChanged( formulaFrameSet(), true );
}

void KWFormulaFrameSetEdit::slotLeaveFormula( KFormula::Container*,
                                              KFormula::FormulaCursor* cursor,
                                              int cmd )
{
    kdDebug() << k_funcinfo << endl;

    if ( cursor == formulaView->getCursor() ) {
        switch ( cmd ) {
        case KFormula::Container::EXIT_LEFT:
            exitLeft();
            break;
        case KFormula::Container::EXIT_RIGHT:
            exitRight();
            break;
        case KFormula::Container::EXIT_ABOVE:
            exitAbove();
            break;
        case KFormula::Container::EXIT_BELOW:
            exitBelow();
            break;
        case KFormula::Container::REMOVE_FORMULA:
            removeFormula();
            break;
        }
    }
}

#include "kwformulaframe.moc"
