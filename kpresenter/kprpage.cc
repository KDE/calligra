/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include <kprpage.h>
#include <kpresenter_view.h>

#include <kplineobject.h>
#include <kprectobject.h>
#include <kpellipseobject.h>
#include <kpautoformobject.h>
#include <kpclipartobject.h>
#include <kptextobject.h>
#include <kppixmapobject.h>
#include <kppieobject.h>
#include <kppartobject.h>
#include <kpgroupobject.h>
#include <kprcommand.h>
#include <kpfreehandobject.h>
#include <kppolylineobject.h>
#include <kpquadricbeziercurveobject.h>
#include <kpcubicbeziercurveobject.h>
#include <kppolygonobject.h>
#include <kdebug.h>
#include <koQueryTrader.h>
#include "kpresenter_doc.h"
#include "styledia.h"

#include <koStore.h>
#include <koStoreDevice.h>
#include <kurldrag.h>
#include <qclipboard.h>
#include <kozoomhandler.h>
#include <kprcommand.h>
#include "kprtextdocument.h"

#include <koRect.h>
#include <qapplication.h>

/******************************************************************/
/* class KPrPage - KPrPage                                        */
/******************************************************************/

KPrPage::KPrPage(KPresenterDoc *_doc )
{
    kdDebug()<<"create page : KPrPage::KPrPage(KPresenterDoc *_doc )\n";
    m_doc=_doc;
    kpbackground= new KPBackGround( (m_doc->getImageCollection()), (m_doc->getGradientCollection()),
                                    (m_doc->getClipartCollection()), this );
    //create object list for each page.
    m_objectList.setAutoDelete( false );
    manualTitle=QString::null;
    noteText=QString::null;
}

KPrPage::~KPrPage()
{
    kdDebug()<<"Delete page :KPrPage::~KPrPage() \n";
    //delete object list.
    m_objectList.setAutoDelete( true );
    m_objectList.clear();
    delete kpbackground;
}

void KPrPage::setPageLayout(KoPageLayout /*pgLayout*/)
{
    //refresh background
    //FIXME
}

void KPrPage::appendObject(KPObject *_obj)
{
    m_objectList.append(_obj);
}

void KPrPage::takeObject(KPObject *_obj)
{
    m_objectList.take( m_objectList.findRef( _obj ) );
}

void KPrPage::insertObject(KPObject *_oldObj, KPObject *_newObject)
{
    unsigned int pos = m_objectList.findRef( _oldObj );
    m_objectList.take( pos );
    m_objectList.insert( pos, _newObject );
}

void KPrPage::insertObject(KPObject *_obj,int pos)
{
    m_objectList.insert(pos,_obj);
}

void KPrPage::removeObject( int pos)
{
    m_objectList.remove(pos);
}

void KPrPage::deleteObjs( bool _add )
{
    //todo
    QPtrList<KPObject> _objects;
    _objects.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
	    _objects.append( it.current() );
    }
    m_doc->deSelectAllObj();

    DeleteCmd *deleteCmd = new DeleteCmd( i18n( "Delete object(s)" ), _objects, m_doc,this );
    deleteCmd->execute();

    if ( _add )
        m_doc->addCommand( deleteCmd );

    m_doc->setModified(true);
}

void KPrPage::copyObjs()
{
    if ( !numSelected() )
        return;

    QDomDocument doc("DOC");
    QDomElement presenter=doc.createElement("DOC");
    presenter.setAttribute("editor", "KPresenter");
    presenter.setAttribute("mime", "application/x-kpresenter-selection");
    doc.appendChild(presenter);

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            QDomElement object=doc.createElement("OBJECT");
            object.setAttribute("type", static_cast<int>( it.current()->getType() ));
            //kpobject->moveBy( -diffx, -diffy );
            object.appendChild(it.current()->save( doc,0 ));
            presenter.appendChild(object);
            //kpobject->moveBy( diffx, diffy );
        }
    }

    QStoredDrag * drag = new QStoredDrag( "application/x-kpresenter-selection" );
    drag->setEncodedData( doc.toCString() );
    QApplication::clipboard()->setData( drag );
}

void KPrPage::pasteObjs( const QByteArray & data, int currPage )
{
    m_doc->deSelectAllObj();
    m_doc->pasting = true;
    m_doc->pasteXOffset = 20;
    m_doc->pasteYOffset = 20;
    QString clip_str = QString::fromUtf8( data );
    if ( clip_str.isEmpty() ) return;
    m_doc->loadPastedObjs( clip_str, currPage,this );

    m_doc->pasting = false;
    m_doc->setModified(true);
}

int KPrPage::numSelected() const
{
    int num = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            num++;
    }

    return num;
}

KPObject* KPrPage::getSelectedObj()
{
    KPObject *kpobject = 0;
    // ### Use iterator!
    for ( int i = 0; i < static_cast<int>( m_objectList.count() ); i++ ) {
	kpobject = m_objectList.at( i );
	if ( kpobject->isSelected() )
            return kpobject;
    }
    return 0L;
}

void KPrPage::groupObjects()
{
    QPtrList<KPObject> objs;
    objs.setAutoDelete( false );
    KPObject *kpobject;
    for ( kpobject = m_objectList.first(); kpobject; kpobject = m_objectList.next() ) {
	if ( kpobject->isSelected() )
	    objs.append( kpobject );
    }

    if ( objs.count() > 1 ) {
	GroupObjCmd *groupObjCmd = new GroupObjCmd( i18n( "Group Objects" ), objs, m_doc,this );
	m_doc->addCommand( groupObjCmd );
	groupObjCmd->execute();
    }
}

void KPrPage::ungroupObjects()
{
    KPObject *kpobject = getSelectedObj();
    if ( kpobject && kpobject->getType() == OT_GROUP ) {
	UnGroupObjCmd *unGroupObjCmd = new UnGroupObjCmd( i18n( "Ungroup Objects" ),
							  (KPGroupObject*)kpobject, m_doc, this );
	m_doc->addCommand( unGroupObjCmd );
	unGroupObjCmd->execute();
    }
}

/*=============================================================*/
QPen KPrPage::getPen( const QPen &pen )
{
    KPObject *kpobject = 0;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
	    switch ( kpobject->getType() ) {
	    case OT_LINE:
		return dynamic_cast<KPLineObject*>( kpobject )->getPen();
		break;
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getPen();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getPen();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getPen();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getPen();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getPen();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getPen();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getPen();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getPen();
		break;
            case OT_FREEHAND:
		return dynamic_cast<KPFreehandObject*>( kpobject )->getPen();
		break;
            case OT_POLYLINE:
		return dynamic_cast<KPPolylineObject*>( kpobject )->getPen();
		break;
            case OT_QUADRICBEZIERCURVE:
		return dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getPen();
		break;
            case OT_CUBICBEZIERCURVE:
		return dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getPen();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getPen();
                break;
	    default: break;
	    }
	}
    }

    return pen;
}

/*========================= get line begin ========================*/
LineEnd KPrPage::getLineBegin( LineEnd lb )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() )
	    {
	    case OT_LINE:
		return dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		break;
            case OT_FREEHAND:
		return dynamic_cast<KPFreehandObject*>( kpobject )->getLineBegin();
		break;
            case OT_POLYLINE:
		return dynamic_cast<KPPolylineObject*>( kpobject )->getLineBegin();
		break;
            case OT_QUADRICBEZIERCURVE:
		return dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineBegin();
		break;
            case OT_CUBICBEZIERCURVE:
		return dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineBegin();
		break;
	    default: break;
	    }
	}
    }

    return lb;
}

/*========================= get line end =========================*/
LineEnd KPrPage::getLineEnd( LineEnd le )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
	    switch ( kpobject->getType() ) {
	    case OT_LINE:
		return dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		break;
            case OT_FREEHAND:
		return dynamic_cast<KPFreehandObject*>( kpobject )->getLineEnd();
		break;
            case OT_POLYLINE:
		return dynamic_cast<KPPolylineObject*>( kpobject )->getLineEnd();
		break;
            case OT_QUADRICBEZIERCURVE:
		return dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineEnd();
		break;
            case OT_CUBICBEZIERCURVE:
		return dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineEnd();
		break;
	    default: break;
	    }
	}
    }

    return le;
}

/*========================= get brush =============================*/
QBrush KPrPage::getBrush( const QBrush &brush )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getBrush();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getBrush();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getBrush();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getBrush();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getBrush();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getBrush();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getBrush();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getBrush();
                break;
	    default: break;
	    }
	}
    }

    return brush;
}

/*================================================================*/
FillType KPrPage::getFillType( FillType ft )
{
    KPObject *kpobject = 0;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getFillType();
                break;
	    default: break;
	    }
	}
    }

    return ft;
}

/*================================================================*/
QColor KPrPage::getGColor1( const QColor &g1 )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getGColor1();
                break;
	    default: break;
	    }
	}
    }

    return g1;
}

/*================================================================*/
QColor KPrPage::getGColor2( const QColor &g2 )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getGColor2();
                break;
	    default: break;
	    }
	}
    }

    return g2;
}

/*================================================================*/
BCType KPrPage::getGType( BCType gt )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGType();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGType();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGType();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGType();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getGType();
                break;
	    default: break;
	    }
	}
    }

    return gt;
}

/*================================================================*/
bool KPrPage::getGUnbalanced( bool  unbalanced )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGUnbalanced();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGUnbalanced();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getGUnbalanced();
                break;
	    default: break;
	    }
	}
    }

    return unbalanced;
}

/*================================================================*/
int KPrPage::getGXFactor( int xfactor )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
            switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGXFactor();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGXFactor();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGXFactor();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGXFactor();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGXFactor();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGXFactor();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGXFactor();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGXFactor();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getGXFactor();
                break;
	    default: break;
	    }
	}
    }

    return xfactor;
}

/*================================================================*/
int KPrPage::getGYFactor( int yfactor )
{
    KPObject *kpobject = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    switch ( kpobject->getType() ) {
	    case OT_RECT:
		return dynamic_cast<KPRectObject*>( kpobject )->getGYFactor();
		break;
	    case OT_ELLIPSE:
		return dynamic_cast<KPEllipseObject*>( kpobject )->getGYFactor();
		break;
	    case OT_AUTOFORM:
		return dynamic_cast<KPAutoformObject*>( kpobject )->getGYFactor();
		break;
	    case OT_PIE:
		return dynamic_cast<KPPieObject*>( kpobject )->getGYFactor();
		break;
	    case OT_PART:
		return dynamic_cast<KPPartObject*>( kpobject )->getGYFactor();
		break;
	    case OT_PICTURE:
		return dynamic_cast<KPPixmapObject*>( kpobject )->getGYFactor();
		break;
	    case OT_CLIPART:
		return dynamic_cast<KPClipartObject*>( kpobject )->getGYFactor();
		break;
	    case OT_TEXT:
		return dynamic_cast<KPTextObject*>( kpobject )->getGYFactor();
		break;
            case OT_POLYGON:
                return dynamic_cast<KPPolygonObject*>( kpobject )->getGYFactor();
                break;
	    default: break;
	    }
	}
    }

    return yfactor;
}

/*================================================================*/
PieType KPrPage::getPieType( PieType pieType )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&& it.current()->getType()==OT_PIE)
	    return dynamic_cast<KPPieObject*>( it.current() )->getPieType();
    }

    return pieType;
}

bool KPrPage::getSticky( bool s )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            return it.current()->isSticky();
    }

    return s;
}

/*================================================================*/
int KPrPage::getPieLength( int pieLength )
{

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&& it.current()->getType()==OT_PIE)
	    return dynamic_cast<KPPieObject*>( it.current() )->getPieLength();
    }

    return pieLength;
}

/*================================================================*/
int KPrPage::getPieAngle( int pieAngle )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&&it.current()->getType()==OT_PIE)
	    return dynamic_cast<KPPieObject*>( it.current() )->getPieAngle();
    }

    return pieAngle;
}

/*================================================================*/
int KPrPage::getRndX( int _rx )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_RECT)
        {
	    int tmp;
	    dynamic_cast<KPRectObject*>( it.current() )->getRnds( _rx, tmp );
	    return _rx;
	}
    }

    return _rx;
}

/*================================================================*/
int KPrPage::getRndY( int _ry )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_RECT)
        {
	    int tmp;
	    dynamic_cast<KPRectObject*>( it.current() )->getRnds( tmp, _ry );
	    return _ry;
	}
    }

    return _ry;
}

/*================================================================*/
bool KPrPage::getPolygonSettings( bool *_checkConcavePolygon, int *_cornersValue, int *_sharpnessValue )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_POLYGON)
        {
            bool tmp_checkConcavePolygon;
            int tmp_cornersValue;
            int tmp_sharpnessValue;

            dynamic_cast<KPPolygonObject*>( it.current() )->getPolygonSettings( &tmp_checkConcavePolygon,
                                                                            &tmp_cornersValue,
                                                                            &tmp_sharpnessValue );

            *_checkConcavePolygon = tmp_checkConcavePolygon;
            *_cornersValue = tmp_cornersValue;
            *_sharpnessValue = tmp_sharpnessValue;

            return true;
        }
    }

    return false;
}

/*======================== lower objects =========================*/
void KPrPage::lowerObjs()
{
    KPObject *kpobject = 0;

    QPtrList<KPObject> _new;

    for ( unsigned int j = 0; j < m_objectList.count(); j++ )
	_new.append( m_objectList.at( j ) );

    _new.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( _new.count() ); i++ ) {
	kpobject = _new.at( i );
	if ( kpobject->isSelected() ) {
	    _new.take( i );
	    _new.insert( 0, kpobject );
	}
    }
    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Object(s)" ), m_objectList, _new, m_doc,this );
    lrCmd->execute();
    m_doc->addCommand( lrCmd );
    m_doc->raiseAndLowerObject = true;

}

/*========================= raise object =========================*/
void KPrPage::raiseObjs()
{
    KPObject *kpobject = 0;

    QPtrList<KPObject> _new;

    for ( unsigned int j = 0; j < m_objectList.count(); j++ )
	_new.append( m_objectList.at( j ) );

    _new.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( _new.count() ); i++ ) {
	kpobject = m_objectList.at( i );
	if ( kpobject->isSelected() ) {
	    _new.take( i );
	    _new.append( kpobject );
	}
    }
    LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Raise Object(s)" ), m_objectList, _new, m_doc,this );
    lrCmd->execute();
    m_doc->addCommand( lrCmd );
    m_doc->raiseAndLowerObject = true;
}

void KPrPage::insertLine( QRect r, QPen pen, LineEnd lb, LineEnd le, LineType lt )
{
    KPLineObject *kplineobject = new KPLineObject( pen, lb, le, lt );
    kplineobject->setOrig( r.x(), r.y() );
    kplineobject->setSize( r.width(), r.height() );
    kplineobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );

}

/*===================== insert a rectangle =======================*/
void KPrPage::insertRectangle( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
				     BCType gt, int rndX, int rndY, bool unbalanced, int xfactor, int yfactor )
{
    KPRectObject *kprectobject = new KPRectObject( pen, brush, ft, g1, g2, gt, rndX, rndY,
						   unbalanced, xfactor, yfactor );
    kprectobject->setOrig( r.x() , r.y() );
    kprectobject->setSize( r.width(), r.height() );
    kprectobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Rectangle" ), kprectobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*===================== insert a circle or ellipse ===============*/
void KPrPage::insertCircleOrEllipse( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
					   BCType gt, bool unbalanced, int xfactor, int yfactor )
{
    KPEllipseObject *kpellipseobject = new KPEllipseObject( pen, brush, ft, g1, g2, gt,
							    unbalanced, xfactor, yfactor );
    kpellipseobject->setOrig( r.x(), r.y() );
    kpellipseobject->setSize( r.width(), r.height() );
    kpellipseobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Ellipse" ), kpellipseobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*================================================================*/
void KPrPage::insertPie( QRect r, QPen pen, QBrush brush, FillType ft, QColor g1, QColor g2,
			       BCType gt, PieType pt, int _angle, int _len, LineEnd lb, LineEnd le,
			       bool unbalanced, int xfactor, int yfactor )
{
    KPPieObject *kppieobject = new KPPieObject( pen, brush, ft, g1, g2, gt, pt, _angle,
						_len, lb, le, unbalanced, xfactor, yfactor );
    kppieobject->setOrig( r.x(), r.y() );
    kppieobject->setSize( r.width(), r.height() );
    kppieobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Pie/Arc/Chord" ), kppieobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*===================== insert a textobject =====================*/
void KPrPage::insertTextObject( const QRect& r, const QString& text, KPresenterView *_view )
{
    KPTextObject *kptextobject = new KPTextObject( m_doc );
    kptextobject->setOrig( r.x(), r.y() );
    kptextobject->setSize( r.width(), r.height() );
    kptextobject->setSelected( true );
    if ( !text.isEmpty() && _view ) {
#if 0
        if(kptextobject->textObjectView())
        {
            kptextobject->textObjectView()->clear();
            kptextobject->textObjectView()->setText( text );
            kptextobject->textObject()->document()->setFontToAll( _view->currFont() );
            kptextobject->textObject()->document()->setColorToAll( _view->currColor() );
        }
#endif
    }
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Textbox" ), kptextobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*======================= insert an autoform ====================*/
void KPrPage::insertAutoform( QRect r, QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft,
                              QColor g1, QColor g2, BCType gt, const QString &fileName, bool unbalanced,
                              int xfactor, int yfactor ){
    KPAutoformObject *kpautoformobject = new KPAutoformObject( pen, brush, fileName, lb, le, ft,
							       g1, g2, gt, unbalanced, xfactor, yfactor );
    kpautoformobject->setOrig( r.x() , r.y()  );
    kpautoformobject->setSize( r.width(), r.height() );
    kpautoformobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertFreehand( const QPointArray &points, QRect r, QPen pen,
                                    LineEnd lb, LineEnd le )
{
    QSize size( r.width(), r.height() );
    KPFreehandObject *kpfreehandobject = new KPFreehandObject( points, size, pen, lb, le );
    kpfreehandobject->setOrig( r.x(), r.y() );
    kpfreehandobject->setSize( r.width(), r.height() );
    kpfreehandobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Freehand" ), kpfreehandobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertPolyline( const QPointArray &points, QRect r, QPen pen,
                                    LineEnd lb, LineEnd le )
{
    QSize size( r.width(), r.height() );
    KPPolylineObject *kppolylineobject = new KPPolylineObject( points, size, pen, lb, le );
    kppolylineobject->setOrig( r.x(), r.y() );
    kppolylineobject->setSize( r.width(), r.height() );
    kppolylineobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polyline" ), kppolylineobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*================ insert a quadric bezier curve =================*/
void KPrPage::insertQuadricBezierCurve( const QPointArray &points, const QPointArray &allPoints, QRect r, QPen pen,
                                            LineEnd lb, LineEnd le )
{
    QSize size( r.width(), r.height() );

    KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject( points, allPoints, size, pen, lb, le );
    kpQuadricBezierCurveObject->setOrig( r.x(), r.y() );
    kpQuadricBezierCurveObject->setSize( r.width(), r.height() );
    kpQuadricBezierCurveObject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Quadric Bezier Curve" ), kpQuadricBezierCurveObject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*================= insert a cubic bezier curve ==================*/
void KPrPage::insertCubicBezierCurve( const QPointArray &points, const QPointArray &allPoints, QRect r, QPen pen,
                                            LineEnd lb, LineEnd le )
{
    QSize size( r.width(), r.height() );

    KPCubicBezierCurveObject *kpCubicBezierCurveObject = new KPCubicBezierCurveObject( points, allPoints, size, pen, lb, le );
    kpCubicBezierCurveObject->setOrig( r.x(), r.y() );
    kpCubicBezierCurveObject->setSize( r.width(), r.height() );
    kpCubicBezierCurveObject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Cubic Bezier Curve" ), kpCubicBezierCurveObject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*======================= insert polygon ===========================*/
void KPrPage::insertPolygon( const QPointArray &points, QRect r, QPen pen, QBrush brush, FillType ft,
                                   QColor g1, QColor g2, BCType gt, bool unbalanced, int xfactor, int yfactor,
                                   bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
{
    QSize size( r.width(), r.height() );

    KPPolygonObject *kpPolygonObject = new KPPolygonObject( points, size, pen, brush, ft,
                                                            g1, g2, gt, unbalanced, xfactor, yfactor,
                                                            _checkConcavePolygon, _cornersValue, _sharpnessValue );
    kpPolygonObject->setOrig( r.x(), r.y() );
    kpPolygonObject->setSize( r.width(), r.height() );
    kpPolygonObject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polygon" ), kpPolygonObject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*======================== align objects left ===================*/
void KPrPage::alignObjsLeft()
{
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _x =  m_doc->zoomHandler()->zoomRect(getPageRect()).x();

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
	    _objects.append( it.current() );
            if( !newPosition &&_x != it.current()->getOrig().x())
                newPosition=true;
	    _diffs.append( new QPoint( _x - it.current()->getOrig().x(), 0 ) );
	}
    }

    if(newPosition)
    {
        MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) left" ), _diffs, _objects, m_doc );
        m_doc->addCommand( moveByCmd2 );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
}

/*==================== align objects center h ===================*/
void KPrPage::alignObjsCenterH()
{
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _x =  m_doc->zoomHandler()->zoomRect(getPageRect()).x();
    int _w = m_doc->zoomHandler()->zoomRect( getPageRect(  )).width();


    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
	    _objects.append( it.current() );
            if(!newPosition && (( _w - it.current()->getSize().width() ) / 2 - it.current()->getOrig().x() + _x)!=0)
                newPosition=true;
	    _diffs.append( new QPoint( ( _w - it.current()->getSize().width() ) / 2 - it.current()->getOrig().x() + _x, 0 ) );
	}
    }
    if(newPosition)
    {
        MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) centered (horizontal)" ),
                                                 _diffs, _objects, m_doc );
        m_doc->addCommand( moveByCmd2 );
        moveByCmd2->execute();
    }
    else
    {
         _diffs.setAutoDelete(true);
         _diffs.clear();
    }
}

/*==================== align objects right ======================*/
void KPrPage::alignObjsRight()
{
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int _w = m_doc->zoomHandler()->zoomRect(getPageRect()).x() + m_doc->zoomHandler()->zoomRect( getPageRect( )).width();

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
	    _objects.append( it.current() );
            if(!newPosition && (( _w - it.current()->getSize().width() ) != it.current()->getOrig().x()))
                newPosition=true;
	    _diffs.append( new QPoint( ( _w - it.current()->getSize().width() ) - it.current()->getOrig().x(), 0 ) );
	}
    }
    if(newPosition)
    {
        MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) right" ), _diffs, _objects, m_doc);
        m_doc->addCommand( moveByCmd2 );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
}

/*==================== align objects top ========================*/
void KPrPage::alignObjsTop()
{
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int  _y;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            _y = m_doc->zoomHandler()->zoomRect(getPageRect( )).y();
            _objects.append( it.current() );
            if(!newPosition && (_y != it.current()->getOrig().y()))
                newPosition=true;

            _diffs.append( new QPoint( 0, _y - it.current()->getOrig().y() ) );
	}
    }
    if(newPosition)
    {
        MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) top" ), _diffs, _objects, m_doc );
        m_doc->addCommand( moveByCmd2 );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
}

/*==================== align objects center v ===================*/
void KPrPage::alignObjsCenterV()
{
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int  _y, _h;


    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            _y = m_doc->zoomHandler()->zoomRect(getPageRect( )).y();
            _h = m_doc->zoomHandler()->zoomRect(getPageRect( )).height();
            _objects.append( it.current() );
            if(!newPosition &&(( _h - it.current()->getSize().height() ) / 2 - it.current()->getOrig().y() + _y )!=0)
                newPosition=true;
            _diffs.append( new QPoint( 0, ( _h - it.current()->getSize().height() ) / 2 -
                                       it.current()->getOrig().y() + _y ) );
	}
    }
    if(newPosition)
    {
        MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) center / vertical" ), _diffs, _objects, m_doc );
        m_doc->addCommand( moveByCmd2 );
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete(true);
        _diffs.clear();
    }
}

void KPrPage::alignObjsBottom()
{
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    int  _h;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            _h = m_doc->zoomHandler()->zoomRect(getPageRect( )).y() + m_doc->zoomHandler()->zoomRect(getPageRect()).height();
            _objects.append( it.current() );
            if(!newPosition && _h != (it.current()->getSize().height() + it.current()->getOrig().y()))
                newPosition=true;
            _diffs.append( new QPoint( 0, _h - it.current()->getSize().height() - it.current()->getOrig().y() ) );
	}
    }

    if(newPosition)
    {
        MoveByCmd2 *moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) bottom" ), _diffs, _objects, m_doc );
        m_doc->addCommand( moveByCmd2 );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
}



void KPrPage::insertClipart( const QString &filename )
{
    KPClipartKey key = m_doc->getClipartCollection()->loadClipart( filename ).key();
    kdDebug(33001) << "KPresenterDoc::insertClipart key=" << key.toString() << endl;

    KPClipartObject *kpclipartobject = new KPClipartObject(m_doc->getClipartCollection() , key );
    kpclipartobject->setOrig( ( (  10 ) / m_doc->rastX() ) * m_doc->rastX(), ( (  10 ) / m_doc->rastY() ) * m_doc->rastY() );
    kpclipartobject->setSize( 150, 150 );
    kpclipartobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertObject( const QRect& _rect, KoDocumentEntry& _e )
{

    KoDocument* doc = _e.createDoc( m_doc );
    if ( !doc || !doc->initDoc() ) {
	return;
    }

    KPresenterChild* ch = new KPresenterChild( m_doc, doc, _rect );

    m_doc->insertObject( ch );

    KPPartObject *kppartobject = new KPPartObject( ch );
    kppartobject->setOrig( _rect.x(), _rect.y() );
    kppartobject->setSize( _rect.width(), _rect.height() );
    kppartobject->setSelected( true );
    QWidget::connect(ch, SIGNAL(changed(KoChild *)), kppartobject, SLOT(slot_changed(KoChild *)) );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Embed Object" ), kppartobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
    //emit sig_insertObject( ch, kppartobject );

    m_doc->repaint( false );
}

bool KPrPage::setLineEnd( LineEnd le )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QPtrList<KPObject> _objects;
    QPtrList<PenBrushCmd::Pen> _oldPen;
    QPtrList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newPen.lineEnd = le;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    btmp = new PenBrushCmd::Brush;
	    ptmp = new PenBrushCmd::Pen;
	    switch ( kpobject->getType() )
	    {
	    case OT_LINE: {
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
            case OT_FREEHAND: {
		ptmp->pen = QPen( dynamic_cast<KPFreehandObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPFreehandObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPFreehandObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_POLYLINE: {
		ptmp->pen = QPen( dynamic_cast<KPPolylineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPPolylineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPolylineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_QUADRICBEZIERCURVE: {
		ptmp->pen = QPen( dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_CUBICBEZIERCURVE: {
		ptmp->pen = QPen( dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Line End" ), _oldPen, _oldBrush,
						    _newPen, _newBrush, _objects, m_doc, PenBrushCmd::LE_ONLY );
	m_doc->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    m_doc->setModified(true);
    return ret;
}

bool KPrPage::setLineBegin( LineEnd lb )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QPtrList<KPObject> _objects;
    QPtrList<PenBrushCmd::Pen> _oldPen;
    QPtrList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newPen.lineBegin = lb;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    btmp = new PenBrushCmd::Brush;
	    ptmp = new PenBrushCmd::Pen;
	    switch ( kpobject->getType() ) {
	    case OT_LINE: {
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
            case OT_FREEHAND: {
		ptmp->pen = QPen( dynamic_cast<KPFreehandObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPFreehandObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPFreehandObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_POLYLINE: {
		ptmp->pen = QPen( dynamic_cast<KPPolylineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPPolylineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPolylineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_QUADRICBEZIERCURVE: {
		ptmp->pen = QPen( dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_CUBICBEZIERCURVE: {
		ptmp->pen = QPen( dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Line Begin" ), _oldPen, _oldBrush,
						    _newPen, _newBrush, _objects, m_doc, PenBrushCmd::LB_ONLY );
	m_doc->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    m_doc->setModified(true);
    return ret;
}


/*===================== set pen and brush ========================*/
bool KPrPage::setPenBrush( QPen pen, QBrush brush, LineEnd lb, LineEnd le, FillType ft, QColor g1, QColor g2,
				 BCType gt, bool unbalanced, int xfactor, int yfactor, bool sticky )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QPtrList<KPObject> _objects;
    QPtrList<PenBrushCmd::Pen> _oldPen;
    QPtrList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newPen.pen = QPen( pen );
    _newPen.lineBegin = lb;
    _newPen.lineEnd = le;

    _newBrush.brush = QBrush( brush );
    _newBrush.fillType = ft;
    _newBrush.gColor1 = g1;
    _newBrush.gColor2 = g2;
    _newBrush.gType = gt;
    _newBrush.unbalanced = unbalanced;
    _newBrush.xfactor = xfactor;
    _newBrush.yfactor = yfactor;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
    	    kpobject->setSticky( sticky );
	    ptmp = new PenBrushCmd::Pen;
	    btmp = new PenBrushCmd::Brush;
	    switch ( kpobject->getType() ) {
	    case OT_LINE:
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
		break;
	    case OT_RECT:
		ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPRectObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPRectObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPRectObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_ELLIPSE:
		ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
		btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPEllipseObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPEllipseObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPEllipseObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_AUTOFORM:
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPAutoformObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPAutoformObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPAutoformObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_PIE:
		ptmp->pen = QPen( dynamic_cast<KPPieObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPieObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPieObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPPieObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPPieObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPPieObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_PART:
		ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPPartObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPPartObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPPartObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_TEXT:
		ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPTextObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPTextObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPTextObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_PICTURE:
		ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPPixmapObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPPixmapObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPPixmapObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
	    case OT_CLIPART:
		ptmp->pen = QPen( dynamic_cast<KPClipartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPClipartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		btmp->unbalanced = dynamic_cast<KPClipartObject*>( kpobject )->getGUnbalanced();
		btmp->xfactor = dynamic_cast<KPClipartObject*>( kpobject )->getGXFactor();
		btmp->yfactor = dynamic_cast<KPClipartObject*>( kpobject )->getGYFactor();
		ret = true;
		break;
             case OT_FREEHAND:
		ptmp->pen = QPen( dynamic_cast<KPFreehandObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPFreehandObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPFreehandObject*>( kpobject )->getLineEnd();
		ret = true;
		break;
             case OT_POLYLINE:
		ptmp->pen = QPen( dynamic_cast<KPPolylineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPPolylineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPolylineObject*>( kpobject )->getLineEnd();
		ret = true;
		break;
             case OT_QUADRICBEZIERCURVE:
		ptmp->pen = QPen( dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
		break;
             case OT_CUBICBEZIERCURVE:
		ptmp->pen = QPen( dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
		break;
	    default: break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Apply Styles" ), _oldPen, _oldBrush,
						    _newPen, _newBrush, _objects, m_doc );
	m_doc->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }
    m_doc->setModified(true);
    return ret;
}

int KPrPage::getPenBrushFlags()
{
    int flags = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            switch ( it.current()->getType() ) {
                case OT_LINE: case OT_FREEHAND: case OT_POLYLINE:
                case OT_QUADRICBEZIERCURVE: case OT_CUBICBEZIERCURVE:
                    flags = flags | StyleDia::SdPen;
                    flags = flags | StyleDia::SdEndBeginLine;
                    break;
                case OT_PIE:
                    flags=flags | StyleDia::SdPen;
                    if((static_cast<KPPieObject*>(it.current())->getPieType())!=PT_ARC)
                        flags=flags |StyleDia::SdBrush;
                    break;
                case OT_RECT: case OT_PART:  case OT_ELLIPSE:
                case OT_TEXT: case OT_PICTURE: case OT_CLIPART: {
                    flags = flags | StyleDia::SdPen;
                    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                }
                    break;
                case OT_AUTOFORM:
                {
                    flags = flags | StyleDia::SdPen;
                    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                    flags = flags | StyleDia::SdEndBeginLine;
                }
                break;
                default: break;
            }
        }
    }

    if ( flags == 0 )
	flags = StyleDia::SdAll;
    return flags;
}


bool KPrPage::setPieSettings( PieType pieType, int angle, int len )
{
    bool ret = false;

    KPObject *kpobject = 0;
    QPtrList<KPObject> _objects;
    QPtrList<PieValueCmd::PieValues> _oldValues;
    PieValueCmd::PieValues _newValues, *tmp;

    _objects.setAutoDelete( false );
    _oldValues.setAutoDelete( false );

    _newValues.pieType = pieType;
    _newValues.pieAngle = angle;
    _newValues.pieLength = len;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_PIE)
        {
            kpobject=it.current();
	    if ( kpobject->isSelected() ) {
		tmp = new PieValueCmd::PieValues;
		tmp->pieType = dynamic_cast<KPPieObject*>( kpobject )->getPieType();
		tmp->pieAngle = dynamic_cast<KPPieObject*>( kpobject )->getPieAngle();
		tmp->pieLength = dynamic_cast<KPPieObject*>( kpobject )->getPieLength();
		_oldValues.append( tmp );
		_objects.append( kpobject );
		ret = true;
	    }
	}
    }

    if ( !_objects.isEmpty() ) {
	PieValueCmd *pieValueCmd = new PieValueCmd( i18n( "Change Pie/Arc/Chord Values" ),
						    _oldValues, _newValues, _objects, m_doc );
	m_doc->addCommand( pieValueCmd );
	pieValueCmd->execute();
    } else {
	_oldValues.setAutoDelete( true );
	_oldValues.clear();
    }

    m_doc->setModified(true);
    return ret;
}

bool KPrPage::setRectSettings( int _rx, int _ry )
{
    bool ret = false;
    bool changed=false;
    KPObject *kpobject = 0;
    QPtrList<KPObject> _objects;
    QPtrList<RectValueCmd::RectValues> _oldValues;
    RectValueCmd::RectValues _newValues, *tmp;

    _objects.setAutoDelete( false );
    _oldValues.setAutoDelete( false );

    _newValues.xRnd = _rx;
    _newValues.yRnd = _ry;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_RECT)
        {
            kpobject=it.current();
    	    if ( kpobject->isSelected() ) {
		tmp = new RectValueCmd::RectValues;
		dynamic_cast<KPRectObject*>( kpobject )->getRnds( tmp->xRnd, tmp->yRnd );
		_oldValues.append( tmp );
		_objects.append( kpobject );
                if(!changed && (tmp->xRnd!=_newValues.xRnd
                                ||tmp->yRnd!=_newValues.yRnd) )
                    changed=true;
		ret = true;
	    }
	}
    }

    if ( !_objects.isEmpty() && changed ) {
	RectValueCmd *rectValueCmd = new RectValueCmd( i18n( "Change Rectangle values" ), _oldValues,
						       _newValues, _objects, m_doc );
	m_doc->addCommand( rectValueCmd );
	rectValueCmd->execute();
    } else {
	_oldValues.setAutoDelete( true );
	_oldValues.clear();
    }

    m_doc->setModified(true);
    return ret;
}

bool KPrPage::setPolygonSettings( bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
{
    bool ret = false;
    bool changed = false;

    KPObject *kpobject = 0;
    QPtrList<KPObject> _objects;
    QPtrList<PolygonSettingCmd::PolygonSettings> _oldSettings;
    PolygonSettingCmd::PolygonSettings _newSettings, *tmp;

    _objects.setAutoDelete( false );
    _oldSettings.setAutoDelete( false );

    _newSettings.checkConcavePolygon = _checkConcavePolygon;
    _newSettings.cornersValue = _cornersValue;
    _newSettings.sharpnessValue = _sharpnessValue;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_POLYGON)
        {
            kpobject=it.current();
            if ( kpobject->isSelected() ) {
                tmp = new PolygonSettingCmd::PolygonSettings;
                dynamic_cast<KPPolygonObject*>( kpobject )->getPolygonSettings( &tmp->checkConcavePolygon,
                                                                                &tmp->cornersValue,
                                                                                &tmp->sharpnessValue );
                _oldSettings.append( tmp );
                _objects.append( kpobject );

                if( !changed && ( tmp->checkConcavePolygon !=_newSettings.checkConcavePolygon
                                  || tmp->cornersValue != _newSettings.cornersValue
                                  || tmp->sharpnessValue != _newSettings.sharpnessValue ) )
                    changed = true;
                ret = true;
            }
        }
    }

    if ( !_objects.isEmpty() && changed ) {
        PolygonSettingCmd *polygonSettingCmd = new PolygonSettingCmd( i18n( "Change Polygon Settings" ), _oldSettings,
                                                                      _newSettings, _objects, m_doc );
        m_doc->addCommand( polygonSettingCmd );
        polygonSettingCmd->execute();
    }
    else {
        _oldSettings.setAutoDelete( true );
        _oldSettings.clear();
    }

    m_doc->setModified( true );

    return ret;
}

bool KPrPage::setPenColor( QColor c, bool fill )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QPtrList<KPObject> _objects;
    QPtrList<PenBrushCmd::Pen> _oldPen;
    QPtrList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    if ( !fill )
	_newPen.pen = Qt::NoPen;
    else
	_newPen.pen = QPen( c );

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );


    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
	    ptmp = new PenBrushCmd::Pen;
	    btmp = new PenBrushCmd::Brush;
	    switch ( kpobject->getType() ) {
	    case OT_LINE: {
		ptmp->pen = QPen( dynamic_cast<KPLineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPLineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPLineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
	    case OT_RECT: {
		ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_ELLIPSE: {
		ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
		btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PIE: {
		ptmp->pen = QPen( dynamic_cast<KPPieObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPieObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPieObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PART: {
		ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_TEXT: {
		ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PICTURE: {
		ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_CLIPART: {
		ptmp->pen = QPen( dynamic_cast<KPClipartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPClipartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
            case OT_FREEHAND: {
		ptmp->pen = QPen( dynamic_cast<KPFreehandObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPFreehandObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPFreehandObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_POLYLINE: {
		ptmp->pen = QPen( dynamic_cast<KPPolylineObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPPolylineObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPolylineObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_QUADRICBEZIERCURVE: {
		ptmp->pen = QPen( dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPQuadricBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_CUBICBEZIERCURVE: {
		ptmp->pen = QPen( dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getPen() );
		ptmp->lineBegin = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPCubicBezierCurveObject*>( kpobject )->getLineEnd();
		ret = true;
	    } break;
            case OT_POLYGON: {
                ptmp->pen = QPen( dynamic_cast<KPPolygonObject*>( kpobject )->getPen() );
                btmp->brush = dynamic_cast<KPPolygonObject*>( kpobject )->getBrush();
                btmp->fillType = dynamic_cast<KPPolygonObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPolygonObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPolygonObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPolygonObject*>( kpobject )->getGType();
                ret = true;
            } break;
	    default: break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Pen" ), _oldPen, _oldBrush, _newPen,
						    _newBrush, _objects, m_doc, PenBrushCmd::PEN_ONLY );
	m_doc->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    m_doc->setModified(true);
    return ret;
}

bool KPrPage::setBrushColor( QColor c, bool fill )
{
    KPObject *kpobject = 0;
    bool ret = false;

    QPtrList<KPObject> _objects;
    QPtrList<PenBrushCmd::Pen> _oldPen;
    QPtrList<PenBrushCmd::Brush> _oldBrush;
    PenBrushCmd::Pen _newPen, *ptmp;
    PenBrushCmd::Brush _newBrush, *btmp;

    _newBrush.fillType = FT_BRUSH;
    if ( !fill )
	_newBrush.brush = Qt::NoBrush;
    else
	_newBrush.brush = QBrush( c );

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        kpobject=it.current();
	if ( kpobject->isSelected() && kpobject->getType() != OT_LINE
                                    && kpobject->getType() != OT_FREEHAND
                                    && kpobject->getType() != OT_POLYLINE
                                    && kpobject->getType() != OT_QUADRICBEZIERCURVE
                                    && kpobject->getType() != OT_CUBICBEZIERCURVE ) {
	    ptmp = new PenBrushCmd::Pen;
	    btmp = new PenBrushCmd::Brush;
	    switch ( kpobject->getType() )
	    {
	    case OT_RECT: {
		ptmp->pen = QPen( dynamic_cast<KPRectObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPRectObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPRectObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPRectObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPRectObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPRectObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_ELLIPSE: {
		ptmp->pen = QPen( dynamic_cast<KPEllipseObject*>( kpobject )->getPen() );
		btmp->brush = dynamic_cast<KPEllipseObject*>( kpobject )->getBrush();
		btmp->fillType = dynamic_cast<KPEllipseObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPEllipseObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPEllipseObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_AUTOFORM: {
		ptmp->pen = QPen( dynamic_cast<KPAutoformObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPAutoformObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPAutoformObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPAutoformObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPAutoformObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPAutoformObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPAutoformObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PIE: {
		ptmp->pen = QPen( dynamic_cast<KPPieObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPieObject*>( kpobject )->getBrush() );
		ptmp->lineBegin = dynamic_cast<KPPieObject*>( kpobject )->getLineBegin();
		ptmp->lineEnd = dynamic_cast<KPPieObject*>( kpobject )->getLineEnd();
		btmp->fillType = dynamic_cast<KPPieObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPieObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPieObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPieObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PART: {
		ptmp->pen = QPen( dynamic_cast<KPPartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_TEXT: {
		ptmp->pen = QPen( dynamic_cast<KPTextObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPTextObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPTextObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPTextObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPTextObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPTextObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_PICTURE: {
		ptmp->pen = QPen( dynamic_cast<KPPixmapObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPPixmapObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPPixmapObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPPixmapObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPPixmapObject*>( kpobject )->getGType();
		ret = true;
	    } break;
	    case OT_CLIPART: {
		ptmp->pen = QPen( dynamic_cast<KPClipartObject*>( kpobject )->getPen() );
		btmp->brush = QBrush( dynamic_cast<KPClipartObject*>( kpobject )->getBrush() );
		btmp->fillType = dynamic_cast<KPClipartObject*>( kpobject )->getFillType();
		btmp->gColor1 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor1();
		btmp->gColor2 = dynamic_cast<KPClipartObject*>( kpobject )->getGColor2();
		btmp->gType = dynamic_cast<KPClipartObject*>( kpobject )->getGType();
		ret = true;
	    } break;
            case OT_POLYGON: {
                ptmp->pen = QPen( dynamic_cast<KPPolygonObject*>( kpobject )->getPen() );
                btmp->brush = QBrush( dynamic_cast<KPPolygonObject*>( kpobject )->getBrush() );
                btmp->fillType = dynamic_cast<KPPolygonObject*>( kpobject )->getFillType();
                btmp->gColor1 = dynamic_cast<KPPolygonObject*>( kpobject )->getGColor1();
                btmp->gColor2 = dynamic_cast<KPPolygonObject*>( kpobject )->getGColor2();
                btmp->gType = dynamic_cast<KPPolygonObject*>( kpobject )->getGType();
                ret = true;
            } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	PenBrushCmd *penBrushCmd = new PenBrushCmd( i18n( "Change Brush" ), _oldPen, _oldBrush, _newPen,
						    _newBrush, _objects, m_doc, PenBrushCmd::BRUSH_ONLY );
	m_doc->addCommand( penBrushCmd );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    m_doc->setModified(true);
    return ret;
}

void KPrPage::slotRepaintVariable()
{
    KPObject *kpobject;
    for ( kpobject = m_objectList.first(); kpobject; kpobject = m_objectList.next() )
    {
	if ( kpobject->getType() == OT_TEXT )
            m_doc->repaint( kpobject );
    }
}

void KPrPage::recalcPageNum()
{
    KPObject *kpobject = 0;
    for ( kpobject = m_objectList.first(); kpobject; kpobject = m_objectList.next() ) {
	if ( kpobject->getType() == OT_TEXT )
	    ( (KPTextObject*)kpobject )->recalcPageNum( m_doc );
    }
}

void KPrPage::changePicture( const QString & filename )
{
    // filename has been chosen in KPresenterView with a filedialog,
    // so we know it exists
    KPImage image = m_doc->getImageCollection()->loadImage( filename );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_PICTURE)
        {
    	    KPPixmapObject *pix = new KPPixmapObject( m_doc->getImageCollection(), image.key() );

	    ChgPixCmd *chgPixCmd = new ChgPixCmd( i18n( "Change pixmap" ), dynamic_cast<KPPixmapObject*>( it.current() ),pix, m_doc,this );
	    chgPixCmd->execute();
	    m_doc->addCommand( chgPixCmd );
	    break;
	}
    }

    m_doc->setModified(true);
}

void KPrPage::changeClipart( const QString & filename )
{
    // filename has been chosen in KPresenterView with a filedialog,
    // so we know it exists
    KPClipart clipart = m_doc->getClipartCollection()->loadClipart( filename );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&& it.current()->getType()==OT_CLIPART)
        {
	    ChgClipCmd *chgClipCmd = new ChgClipCmd( i18n( "Change clipart" ),
						     dynamic_cast<KPClipartObject*>( it.current() ),
						     dynamic_cast<KPClipartObject*>( it.current() )->getKey(),
						     clipart.key(), m_doc);
	    chgClipCmd->execute();
	    m_doc->addCommand( chgClipCmd );
	    break;
	}
    }

    m_doc->setModified(true);
}


void KPrPage::insertPicture( const QString &filename, int _x , int _y )
{
    KPImageKey key = m_doc->getImageCollection()->loadImage( filename ).key();
    KPPixmapObject *kppixmapobject = new KPPixmapObject(m_doc->getImageCollection() , key );
    kppixmapobject->setOrig( (   _x  / m_doc->rastX() ) * m_doc->rastX(), ( _y  / m_doc->rastY() ) * m_doc->rastY() );
    kppixmapobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );

    QRect s = getZoomPageRect();
    float fakt = 1;
    if ( kppixmapobject->getSize().width() > s.width() )
	fakt = (float)s.width() / (float)kppixmapobject->getSize().width();
    if ( kppixmapobject->getSize().height() > s.height() )
	fakt = QMIN( fakt, (float)s.height() / (float)kppixmapobject->getSize().height() );

    if ( fakt < 1 ) {
	int w = (int)( fakt * (float)kppixmapobject->getSize().width() );
	int h = (int)( fakt * (float)kppixmapobject->getSize().height() );
	kppixmapobject->setSize( w, h );
	m_doc->repaint( false );
    }

    m_doc->setModified(true);
}

void KPrPage::enableEmbeddedParts( bool f )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_PART)
	    dynamic_cast<KPPartObject*>( it.current() )->enableDrawing( f );
    }
}

void KPrPage::deletePage( )
{
    KPObject *kpobject = 0L;
    for ( int i = 0; i < static_cast<int>( m_objectList.count() ); i++ ) {
	kpobject = m_objectList.at( i );
        kpobject->setSelected( true );
    }

    deleteObjs( false );
}

void KPrPage::setBackColor( QColor backColor1, QColor backColor2, BCType bcType,
				  bool unbalanced, int xfactor, int yfactor )
{
    kpbackground->setBackColor1( backColor1 );
    kpbackground->setBackColor2( backColor2 );
    kpbackground->setBackColorType( bcType );
    kpbackground->setBackUnbalanced( unbalanced );
    kpbackground->setBackXFactor( xfactor );
    kpbackground->setBackYFactor( yfactor );
}

void KPrPage::setBackPixmap( const KPImageKey & key )
{
    kpbackground->setBackPixmap( key.filename(), key.lastModified() );
}

bool KPrPage::getBackUnbalanced(  )
{
    return kpbackground->getBackUnbalanced();
}

void KPrPage::setBackClipart( const KPClipartKey & key )
{
    kpbackground->setBackClipart( key.filename(), key.lastModified() );
}

void KPrPage::setBackView( BackView backView )
{
    kpbackground->setBackView( backView );
}

void KPrPage::setBackType( BackType backType )
{
    kpbackground->setBackType( backType );
}

void KPrPage::setPageEffect(  PageEffect pageEffect )
{
    kpbackground->setPageEffect( pageEffect );
}

void KPrPage::setPageTimer(  int pageTimer )
{
    kpbackground->setPageTimer( pageTimer );
}

void KPrPage::setPageSoundEffect(  bool soundEffect )
{
    kpbackground->setPageSoundEffect( soundEffect );
}

void KPrPage::setPageSoundFileName(  const QString &fileName )
{
    kpbackground->setPageSoundFileName( fileName );
}


BackType KPrPage::getBackType(  )
{
    return kpbackground->getBackType();
}

BackView KPrPage::getBackView( )
{
    return kpbackground->getBackView();
}

KoImageKey KPrPage::getBackPixKey( )
{
    return kpbackground->getBackPixKey();
}

KPClipartKey KPrPage::getBackClipKey(  )
{
    return kpbackground->getBackClipKey();
}

QColor KPrPage::getBackColor1( )
{
    return kpbackground->getBackColor1();
}

QColor KPrPage::getBackColor2(  )
{
    return kpbackground->getBackColor2();
}

int KPrPage::getBackXFactor(  )
{
    return kpbackground->getBackXFactor();
}

int KPrPage::getBackYFactor(  )
{
    return kpbackground->getBackYFactor();
}

BCType KPrPage::getBackColorType( )
{
    return kpbackground->getBackColorType();
}

PageEffect KPrPage::getPageEffect( )
{
    return kpbackground->getPageEffect();
}

int KPrPage::getPageTimer(  )
{
    return kpbackground->getPageTimer();
}

bool KPrPage::getPageSoundEffect( )
{
    return kpbackground->getPageSoundEffect();
}

QString KPrPage::getPageSoundFileName(  )
{
    return kpbackground->getPageSoundFileName();
}

KoRect KPrPage::getPageRect() const {
    const KoPageLayout& p = m_doc->pageLayout();
    return KoRect( p.ptLeft, p.ptTop, p.ptWidth - p.ptRight - p.ptLeft, p.ptHeight - p.ptTop - p.ptBottom );
}

QRect KPrPage::getZoomPageRect()const {
    return m_doc->zoomHandler()->zoomRect(getPageRect());
}

void KPrPage::completeLoading( bool _clean, int lastObj )
{
    KPObject *kpobject = 0;
    for ( kpobject = m_objectList.first(); kpobject; kpobject = m_objectList.next() ) {
        if ( kpobject->getType() == OT_PICTURE ) {
            if ( _clean || m_objectList.findRef( kpobject ) > lastObj )
                dynamic_cast<KPPixmapObject*>( kpobject )->reload();
        } else if ( kpobject->getType() == OT_CLIPART )
            dynamic_cast<KPClipartObject*>( kpobject )->reload();
        else if ( kpobject->getType() == OT_TEXT )
            dynamic_cast<KPTextObject*>( kpobject )->recalcPageNum( m_doc );
    }
}


/*====================== replace objects =========================*/
KCommand * KPrPage::replaceObjs( bool createUndoRedo, unsigned int _orastX,unsigned int _orastY,const QColor & _txtBackCol, const QColor & _otxtBackCol )
{
    KPObject *kpobject = 0;
    int ox, oy;
    QPtrList<KPObject> _objects;
    QPtrList<QPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( m_objectList.count() ); i++ ) {
	kpobject = m_objectList.at( i );
	ox = kpobject->getOrig().x();
	oy = kpobject->getOrig().y();
	ox = ( ox / m_doc->rastX() ) * m_doc->rastX();
	oy = ( oy / m_doc->rastY() ) * m_doc->rastY();

	_diffs.append( new QPoint( ox - kpobject->getOrig().x(), oy - kpobject->getOrig().y() ) );
	_objects.append( kpobject );
    }

    SetOptionsCmd *setOptionsCmd = new SetOptionsCmd( i18n( "Set new options" ), _diffs, _objects, m_doc->rastX(), m_doc->rastY(),
						      _orastX, _orastY, _txtBackCol, _otxtBackCol, m_doc );
    if ( createUndoRedo )
        return setOptionsCmd;
    else
    {
       delete setOptionsCmd;
       return 0L;
    }
}


QString KPrPage::getManualTitle()
{
    return manualTitle;
}

void KPrPage::insertManualTitle(const QString & title)
{
    manualTitle=title;
}

QString KPrPage::pageTitle( const QString &_title ) const
{
    // If a user sets a title with manual, return it.
    if ( !manualTitle.isEmpty() )
        return manualTitle;

    QPtrList<KPTextObject> objs;

    // Create list of text objects in page pgNum

   QPtrListIterator<KPObject> it( m_objectList );
   for ( ; it.current() ; ++it )
   {
       if(it.current()->getType()==OT_TEXT)
            objs.append( static_cast<KPTextObject*>( it.current() ) );
   }

    if ( objs.isEmpty() )
        return QString( _title );

    // Find object most on top
    KPTextObject *tmp = objs.first();
    KPTextObject *textobject=tmp;
    for ( tmp = objs.next(); tmp; tmp = objs.next() )
        if ( tmp->getOrig().y() < textobject->getOrig().y() )
            textobject = tmp;

    // this can't happen, but you never know :- )
    if ( !textobject )
        return QString( _title );

    QString txt = textobject->textDocument()->text().stripWhiteSpace();
    if ( txt.isEmpty() )
        return _title;
    unsigned int i=0;
    while( i<txt.length() && txt[i]=='\n' )
        ++i;
    int j=txt.find('\n', i);
    if(i==0 && j==-1)
        return txt;
    return txt.mid(i, j);
}


void KPrPage::setNoteText(  const QString &_text )
{
    noteText=_text;
    if(!_text.isEmpty())
        m_doc->setModified(true);
}

QString KPrPage::getNoteText(  )
{
    return noteText;
}

void KPrPage::makeUsedPixmapList()
{
   QPtrListIterator<KPObject> it( m_objectList );
   for ( ; it.current() ; ++it )
   {
       if( it.current()->getType()==OT_PICTURE || it.current()->getType()==OT_CLIPART)
       {
           if( it.current()->getType()==OT_PICTURE)
               m_doc->appendPixmapKey(dynamic_cast<KPPixmapObject*>( it.current() )->getKey() );
           else
               m_doc->appendClipartKey(dynamic_cast<KPClipartObject*>( it.current())->getKey());
       }
   }

   if( kpbackground->getBackType()==BT_PICTURE)
       m_doc->appendPixmapKey(kpbackground->getBackPixKey());
   else if( kpbackground->getBackType()==BT_CLIPART)
       m_doc->appendClipartKey(kpbackground->getBackClipKey());
}


QValueList<int> KPrPage::reorderPage()
{
    QValueList<int> orderList;
    orderList.append( 0 );
    QPtrListIterator<KPObject> oIt( m_objectList );
    for ( ; oIt.current() ; ++oIt )
    {
        if ( orderList.find( oIt.current()->getPresNum() ) == orderList.end() )
        {
            if ( orderList.isEmpty() )
                orderList.append( oIt.current()->getPresNum() );
            else
            {
                QValueList<int>::Iterator it = orderList.begin();
                for ( ; *it < oIt.current()->getPresNum() && it != orderList.end(); ++it );
		    orderList.insert( it, oIt.current()->getPresNum() );
		}
	    }
	    if ( oIt.current()->getDisappear() && orderList.find( oIt.current()->getDisappearNum() ) == orderList.end() )
            {
		if ( orderList.isEmpty() )
		    orderList.append( oIt.current()->getDisappearNum() );
		else
                {
		    QValueList<int>::Iterator it = orderList.begin();
		    for ( ; *it < oIt.current()->getDisappearNum() && it != orderList.end(); ++it );
		    orderList.insert( it, oIt.current()->getDisappearNum() );
		}
	    }
    }
    return orderList;
}
