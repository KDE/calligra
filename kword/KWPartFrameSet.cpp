/* This file is part of the KDE project
   Copyright (C) 2000-2005 David Faure <faure@kde.org>

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

#include "KWPartFrameSet.h"
#include "KWDocument.h"
#include "KWViewMode.h"
#include "KWCommand.h"
#include "KWordPartFrameSetIface.h"
//#include "KWordPartFrameSetEditIface.h"

#include <kooasiscontext.h>
#include <KoXmlWriter.h>
#include <koxmlns.h>

#include <klocale.h>
#include <kapplication.h>

#include <assert.h>

KWPartFrameSet::KWPartFrameSet( KWDocument *_doc, KWChild *_child, const QString & name )
    : KWFrameSet( _doc ), m_child( 0 ), m_cmdMoveChild( 0 ), m_protectContent( false )
{
    if ( _child )
        setChild( _child );

    kdDebug(32001) << "KWPartFrameSet::KWPartFrameSet" << endl;
    if ( name.isEmpty() )
        m_name = _doc->generateFramesetName( i18n( "Object %1" ) );
    else
        m_name = name;
}

KWPartFrameSet::KWPartFrameSet( KWDocument* doc, const QDomElement& frameTag,
                                const QDomElement& objectTag, KoOasisContext& context )
    : KWFrameSet( doc ), m_child( 0 ), m_cmdMoveChild( 0 ), m_protectContent( false )
{
    m_name = frameTag.attributeNS( KoXmlNS::draw, "name", QString::null );
    if ( doc->frameSetByName( m_name ) ) // already exists!
        m_name = doc->generateFramesetName( m_name + " %1" );

    context.styleStack().save();
    context.fillStyleStack( frameTag, KoXmlNS::draw, "style-name" ); // get the style for the graphics element
    KWFrame* frame = loadOasisFrame( frameTag, context );
    context.styleStack().restore();

    // Create a KWChild, without KoDocument inside
    KWChild* child = doc->createChildDoc( frame->rect(), 0 );
    setChild( child );
    child->loadOasis( frameTag, objectTag );
    updateChildGeometry( doc->viewMode() );

    // This is what loads the KoDocument
    (void)child->loadOasisDocument( context.store(), context.manifestDocument() );
}

void KWPartFrameSet::setChild( KWChild* child )
{
    assert( !m_child );
    m_child = child;
    m_child->setPartFrameSet( this );
    QObject::connect( m_child, SIGNAL( changed( KoChild * ) ),
                      this, SLOT( slotChildChanged() ) );
}

KWPartFrameSet::~KWPartFrameSet()
{
}

KWordFrameSetIface* KWPartFrameSet::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KWordPartFrameSetIface( this );

    return m_dcop;
}


void KWPartFrameSet::drawFrameContents( KWFrame* frame, QPainter * painter, const QRect & /*crect TODO*/,
                                        const QColorGroup &, bool onlyChanged, bool,
                                        KWFrameSetEdit *, KWViewMode * )
{
    if (!onlyChanged)
    {
        if ( !m_child || !m_child->document() )
        {
            kdDebug(32001) << "KWPartFrameSet::drawFrameContents " << this << " aborting. child=" << m_child << " child->document()=" << m_child->document() << endl;
            return;
        }

        // We have to define better what the rect that we pass, means. Does it include zooming ? (yes I think)
        // Does it define the area to be repainted only ? (here it doesn't, really, but it should)
        QRect rframe( 0, 0, kWordDocument()->zoomItX( frame->innerWidth() ),
                      kWordDocument()->zoomItY( frame->innerHeight() ) );
        //kdDebug(32001) << "rframe=" << rframe << endl;

        m_child->document()->paintEverything( *painter, rframe, true, 0L,
                                            kWordDocument()->zoomedResolutionX(), kWordDocument()->zoomedResolutionY() );

    } //else kdDebug(32001) << "KWPartFrameSet::drawFrameContents " << this << " onlychanged=true!" << endl;
}

void KWPartFrameSet::updateChildGeometry( KWViewMode* viewMode )
{
    if( m_frames.isEmpty() ) // Deleted frameset
        return;
    if ( viewMode ) {
        // We need to apply the viewmode conversion correctly (the child is in unzoomed view coords!)
        QRect r = m_doc->zoomRect( *m_frames.first() ); // in normal coordinates.
        KoRect childGeom = m_doc->unzoomRect( viewMode->normalToView( r ) );
        m_child->setGeometry( childGeom.toQRect() ); // rounding problems. TODO: port KoChild to KoRect.
        //m_child->setGeometry( viewMode->normalToView( m_doc->zoomRect( *m_frames.first() ) ) );
    } else
        m_child->setGeometry( m_frames.first()->toQRect() );
}

void KWPartFrameSet::slotChildChanged()
{
    // This is called when the KoDocumentChild is resized (using the KoFrame)
    // We need to react on it in KWPartFrameSetEdit because the view-mode has to be taken into account
    // (Currently not true, since the viewmode is the same in all views, but...)
    QPtrListIterator<KWFrame> listFrame = frameIterator();
    KWFrame *frame = listFrame.current();
    if ( frame )
    {
        // We need to apply the viewmode conversion correctly (the child is in unzoomed view coords!)
        KoRect childGeom = KoRect::fromQRect( getChild()->geometry() );
        // r is the rect in normal coordinates
        QRect r(m_doc->viewMode()->viewToNormal( m_doc->zoomRect( childGeom ) ) );
        frame->setLeft( r.left() / m_doc->zoomedResolutionX() );
        frame->setTop( r.top() / m_doc->zoomedResolutionY() );
        frame->setWidth( r.width() / m_doc->zoomedResolutionX() );
        frame->setHeight( r.height() / m_doc->zoomedResolutionY() );
        // ## TODO an undo/redo command (move frame)

        //kdDebug(32001) << "KWPartFrameSet::slotChildChanged child's geometry " << getChild()->geometry()
        //               << " frame set to " << *frame << endl;
        m_doc->frameChanged( frame );
        //there is just a frame
        if(m_cmdMoveChild)
            m_cmdMoveChild->listFrameMoved().newRect = frame->normalize();
    }
    else
        kdDebug(32001) << "Frame not found!" << endl;
}

QDomElement KWPartFrameSet::save( QDomElement &parentElem, bool saveFrames )
{
    if ( m_frames.isEmpty() ) // Deleted frameset -> don't save
        return QDomElement();
    KWFrameSet::saveCommon( parentElem, saveFrames );
    // Ok, this one is a bit hackish. KWDocument calls us for saving our stuff into
    // the SETTINGS element, which it creates for us. So our save() doesn't really have
    // the same behaviour as a normal KWFrameSet::save()....
    return QDomElement();
}

void KWPartFrameSet::saveOasis( KoXmlWriter& writer, KoSavingContext& context, bool ) const
{
    if ( m_frames.isEmpty() ) // Deleted frameset -> don't save
        return;
    // Save first frame with the whole contents
    KWFrame* frame = m_frames.getFirst();
    frame->startOasisFrame( writer, context.mainStyles(), name() );

    writer.startElement( "draw:object" );
    // #### let's hope name() is unique...
    m_child->saveOasisAttributes( writer, name() );

    writer.endElement(); // draw:object
    writer.endElement(); // draw:frame
}

void KWPartFrameSet::load( QDomElement &attributes, bool loadFrames )
{
    KWFrameSet::load( attributes, loadFrames );
}

void KWPartFrameSet::startEditing()
{
    // Content is protected -> can't edit. Maybe we should open part in readonly mode?
    if ( m_protectContent )
        return;
    kdDebug() << k_funcinfo << endl;
    //create undo/redo move command
    KWFrame* frame = m_frames.first();
    if (!frame)
        return;
    FrameIndex index( frame );
    FrameResizeStruct tmpMove( frame->normalize(), 0, KoRect() );

    if(!m_cmdMoveChild)
        m_cmdMoveChild=new KWFramePartMoveCommand( i18n("Move/Resize Frame"), index, tmpMove );
}

void KWPartFrameSet::endEditing()
{
    kdDebug() << k_funcinfo << endl;
    if( m_cmdMoveChild && m_cmdMoveChild->frameMoved() )
        m_doc->addCommand(m_cmdMoveChild);
    else
        delete m_cmdMoveChild;
    m_cmdMoveChild=0L;

}

void KWPartFrameSet::moveFloatingFrame( int frameNum, const KoPoint &position )
{
    //kdDebug()<<k_funcinfo<<" frame no="<<frameNum<<" to pos="<<position.x()<<","<<position.y()<<endl;
    KWFrame * frame = m_frames.at( frameNum );
    if ( frame )
    {
        KWFrameSet::moveFloatingFrame( frameNum, position );
        m_child->setGeometry( frame->toQRect(), true /* avoid circular events */ );
    }
}

KWFrameSetEdit * KWPartFrameSet::createFrameSetEdit( KWCanvas * /*canvas*/ )
{
    return 0L; // new KWPartFrameSetEdit( this, canvas );
}

#ifndef NDEBUG
void KWPartFrameSet::printDebug()
{
    KWFrameSet::printDebug();
    kdDebug() << " +-- Object Document: " << endl;
    if ( getChild() )
    {
        if ( getChild()->document() )
            kdDebug() << "     Url : " << getChild()->document()->url().url()<<endl;
        else
            kdWarning() << "NO DOCUMENT" << endl;
        kdDebug() << "     Rectangle : " << getChild()->geometry().x() << "," << getChild()->geometry().y() << " " << getChild()->geometry().width() << "x" << getChild()->geometry().height() << endl;
    } else
        kdWarning() << "NO CHILD" << endl;
}

#endif

void KWPartFrameSet::setDeleted( bool on)
{
    m_child->setDeleted( on );
}

void KWPartFrameSet::deleteFrame( unsigned int _num, bool remove, bool recalc )
{
    KWFrameSet::deleteFrame( _num, remove, recalc );
    if ( m_frames.isEmpty() )         // then the whole frameset and thus the child is deleted
        m_child->setDeleted();
}

void KWPartFrameSet::KWPartFrameSet::createEmptyRegion( const QRect &crect, QRegion &emptyRegion, KWViewMode *viewMode ) {
    Q_UNUSED(crect);
    Q_UNUSED(emptyRegion);
    Q_UNUSED(viewMode);

    // empty implementation since embedded parts can be transparant.
}

#if 0
KWPartFrameSetEdit::KWPartFrameSetEdit( KWPartFrameSet * fs, KWCanvas * canvas )
    : KWFrameSetEdit( fs, canvas )
{
    kdDebug(32001) << "KWPartFrameSetEdit::KWPartFrameSetEdit " << endl;
    m_dcop=0L;
    fs->startEditing();
    QObject::connect( m_canvas->gui()->getView(), SIGNAL( activated( bool ) ),
                      this, SLOT( slotChildActivated( bool ) ) );
}

KWPartFrameSetEdit::~KWPartFrameSetEdit()
{
    kdDebug(32001) << "KWPartFrameSetEdit::~KWPartFrameSetEdit" << endl;
    delete m_dcop;
}

DCOPObject* KWPartFrameSetEdit::dcopObject()
{
    if ( !m_dcop )
        m_dcop = new KWordPartFrameSetEditIface( this );
    return m_dcop;
}

void KWPartFrameSetEdit::slotChildActivated(bool b)
{
    kdDebug() << "KWPartFrameSetEdit::slotChildActivated " << b << endl;
    //we store command when we deactivate the child.
    if( !b )
        partFrameSet()->endEditing();

}
#endif

void KWPartFrameSet::storeInternal()
{
    if ( getChild()->document()->storeInternal() )
    {
        KWFramePartExternalCommand* cmd =new KWFramePartExternalCommand( i18n("Make Document External"), this );
        m_doc->addCommand(cmd);
        getChild()->document()->setStoreInternal(false);
    }
    else
    {
        KWFramePartInternalCommand* cmd =new KWFramePartInternalCommand( i18n("Make Document Internal"), this );
        m_doc->addCommand(cmd);
        getChild()->document()->setStoreInternal(true);
    }

    kdDebug()<<k_funcinfo<<"url: "<<getChild()->url().url()<<" store internal="<<getChild()->document()->storeInternal()<<endl;
}


/******************************************************************/
/* Class: KWChild                                              */
/******************************************************************/

KWChild::KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc )
    : KoDocumentChild( _wdoc, _doc, _rect ), m_partFrameSet( 0L )
{
}

KWChild::KWChild( KWDocument *_wdoc )
    : KoDocumentChild( _wdoc ), m_partFrameSet( 0L )
{
}

KWChild::~KWChild()
{
}

KoDocument* KWChild::hitTest( const QPoint& p, const QWMatrix& _matrix )
{
    Q_ASSERT( m_partFrameSet );
    if ( isDeleted() ) {
        //kdDebug() << k_funcinfo << "is deleted!" << endl;
        return 0L;
    }
    // Only activate when it's already selected.
    if ( !m_partFrameSet->frame(0)->isSelected() ) {
        //kdDebug() << k_funcinfo << " is not selected" << endl;
        return 0L;
    }
    // And only if CTRL isn't pressed.
#if KDE_IS_VERSION( 3, 4, 0 )
    if ( kapp->keyboardMouseState() & Qt::ControlButton )
        return 0;
#else
    if ( kapp->keyboardModifiers() & KApplication::ControlModifier )
        return 0;
#endif

    return KoDocumentChild::hitTest( p, _matrix );
}

#include "KWPartFrameSet.moc"
