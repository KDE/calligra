// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#include <kurl.h>

#include "kprpage.h"
#include "kpresenter_view.h"
#include "KPresenterPageIface.h"

#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kppieobject.h"
#include "kppartobject.h"
#include "kpgroupobject.h"
#include "kprcommand.h"
#include "kpfreehandobject.h"
#include "kppolylineobject.h"
#include "kpquadricbeziercurveobject.h"
#include "kpcubicbeziercurveobject.h"
#include "kppolygonobject.h"
#include "kpclosedlineobject.h"
#include <kdebug.h>
#include <koQueryTrader.h>
#include "kpresenter_doc.h"
#include "styledia.h"

#include <koStore.h>
#include <koStoreDevice.h>
#include <kozoomhandler.h>
#include "koPointArray.h"
#include "kprtextdocument.h"
#include <kotextobject.h>
#include <koRect.h>
#include <qapplication.h>
#include <kostyle.h>
#include <kovariable.h>
#include <korichtext.h>
#include <qbuffer.h>
#include <qregexp.h>

KPrPage::KPrPage(KPresenterDoc *_doc )
{
    kdDebug(33001)<<"create page : KPrPage::KPrPage(KPresenterDoc *_doc )"<<this<<endl;
    m_doc=_doc;
    dcop=0;
    kpbackground= new KPBackGround( this );
    //create object list for each page.
    m_objectList.setAutoDelete( false );
    m_manualTitle=QString::null;
    m_noteText=QString::null;
    m_selectedSlides=true;
    //dcopObject();
}

KPrPage::~KPrPage()
{
    kdDebug(33001)<<"Delete page :KPrPage::~KPrPage() :"<<this<<endl;
    //delete object list.
    m_objectList.setAutoDelete( true );
    m_objectList.clear();
    delete kpbackground;
    delete dcop;
}

DCOPObject* KPrPage::dcopObject()
{
    if ( !dcop )
        dcop = new KPresenterPageIface( this );

    return dcop;
}

KPObject *KPrPage::getObject(int num)
{
    Q_ASSERT( num < (int)m_objectList.count() );
    return m_objectList.at(num);
}

/*
 * Check if object name already exists.
 */
bool KPrPage::objectNameExists( KPObject *object, QPtrList<KPObject> &list ) {
    QPtrListIterator<KPObject> it( list );

    for ( it.toFirst(); it.current(); ++it ) {
        // object name can exist in current object.
        if ( it.current()->getObjectName() == object->getObjectName() &&
             it.current() != object ) {
            return true;
        }
        else if ( it.current()->getType() == OT_GROUP ) {
            QPtrList<KPObject> objectList( static_cast<KPGroupObject*>(it.current())->getObjects() );
            if ( objectNameExists( object, objectList ) ) {
                return true;
            }
        }
    }
    return false;
}

/*
 * Create a uniq name for a object.
 * if the name already exists append ' (x)'.
 */
void KPrPage::unifyObjectName( KPObject *object ) {
    if ( object->getObjectName().isEmpty() ) {
        object->setObjectName( object->getTypeString() );
    }
    QString objectName( object->getObjectName() );

    QPtrList<KPObject> list( m_objectList );

    int count = 1;

    while ( objectNameExists( object, list ) ) {
        count++;
        QRegExp rx( " \\(\\d{1,3}\\)$" );
        if ( rx.search( objectName ) != -1 ) {
            objectName.replace( rx, "" );
        }
        objectName += QString(" (%1)").arg( count );
        object->setObjectName( objectName );
    }
}

void KPrPage::appendObject(KPObject *_obj)
{
    unifyObjectName(_obj);
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

KCommand * KPrPage::deleteObjs( bool _add )
{
    QPtrList<KPObject> _objects;
    DeleteCmd *deleteCmd=0L;
    _objects.setAutoDelete( false );
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected())
        {
            _objects.append( it.current() );
            it.current()->setSelected(false);
        }
    }
    if ( _add &&_objects.count() > 0 ) {
        deleteCmd = new DeleteCmd( i18n( "Delete Objects" ), _objects, m_doc, this );
        deleteCmd->execute();
    }
    else
        m_doc->setModified(true);
    return deleteCmd ;
}

void KPrPage::copyObjs(QDomDocument &doc, QDomElement &presenter, QValueList<KoPictureKey> & savePictures) const
{
    if ( !numSelected() )
        return;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            if ( it.current()->getType() == OT_PICTURE )
            {
                KoPictureKey key = static_cast<KPPixmapObject *>( it.current() )->getKey();
                if ( !savePictures.contains( key ) )
                    savePictures.append( key );
            }
            QDomElement object=doc.createElement("OBJECT");
            object.setAttribute("type", static_cast<int>( it.current()->getType() ));
            object.appendChild(it.current()->save( doc,0 ));
            presenter.appendChild(object);
        }
    }
}

void KPrPage::pasteObjs( const QByteArray & data,int nbCopy, double angle,
                         double increaseX, double increaseY, double moveX, double moveY )
{
    m_doc->deSelectAllObj();
    int num = m_objectList.count();
    if ( !data.size() )
        return;
    QBuffer buffer( data );
    KoStore* store = KoStore::createStore( &buffer, KoStore::Read );
    int nbNewObject = -1 ;
    KMacroCommand *macro = 0L;
    if ( !store->bad() )
    {
        if ( store->open( "root" ) )
        {
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument domDoc;
            if ( !domDoc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn ) )
            {
                kdError (30003) << "Parsing Error! Aborting! (in KPrPage::PasteObj)" << endl
                                << "  Line: " << errorLine << " Column: " << errorColumn << endl
                                << "  Message: " << errorMsg << endl;
                delete store;
                return;
            }
            QDomElement topElem = domDoc.documentElement();

            for ( int i = 0 ; i < nbCopy ; i++ )
            {
                KCommand *cmd = m_doc->loadPastedObjs( domDoc.toString(),this );
                if (cmd )
                {
                    if ( !macro )
                        macro = new KMacroCommand( i18n("Paste Objects" ));
                    macro->addCommand( cmd );
                }
                if ( nbNewObject == -1 )
                    nbNewObject = m_objectList.count() - num;
            }

            m_doc->loadPictureMap( topElem );
            store->close();
            int nbNewPartObject = -1;
            int nbElementBefore = m_objectList.count();
            for ( int i = 0 ; i < nbCopy ; i++ )
            {
                m_doc->insertEmbedded( store, topElem, macro, this );
                if ( nbNewPartObject ==-1 )
                    nbNewObject = nbNewObject + (m_objectList.count() - nbElementBefore);
            }
        }
    }
    delete store;

    //move and select all new pasted in objects
    KPObject *_tempObj;
    int i = 0;
    int mod = 1;
    for (_tempObj = m_objectList.at(num); _tempObj; _tempObj = m_objectList.next(),  i++ ) {
        if ( i >= nbNewObject )
            mod++;
        _tempObj->moveBy( moveX*(double)mod,moveY*(double)mod);
        _tempObj->setSelected( true );
        if ( angle == 0.0 || (increaseY == 0.0 && increaseX == 0.0))
            m_doc->repaint(_tempObj);
    }

    if ( angle != 0.0)
    {
        KCommand *cmd = rotateObj(angle, true);
        if (cmd )
        {
            if ( !macro )
                macro = new KMacroCommand( i18n("Paste Objects" ));
            macro->addCommand( cmd );
        }
    }
    if ( increaseX != 0.0 || increaseY != 0.0 )
    {
        QPtrListIterator<KPObject> it( m_objectList );
        for ( ; it.current() ; ++it )
        {
            if(it.current()->isSelected())
            {
                KCommand *cmd =new ResizeCmd( i18n("Resize"), KoPoint(0, 0),
                                              KoSize(increaseX, increaseY), it.current(), m_doc );
                if ( cmd )
                {
                    cmd->execute();
                    if ( !macro )
                        macro = new KMacroCommand( i18n("Paste Objects" ));
                    macro->addCommand( cmd );
                }
            }
        }
    }

    if (macro)
        m_doc->addCommand(macro);
    //update sidebar
    if ( angle == 0.0 || (increaseY == 0.0 && increaseX == 0.0) )
    {
        int pos=m_doc->pageList().findRef(this);
        m_doc->updateSideBarItem(pos);
    }

    m_doc->setModified(true);
}

KPTextObject * KPrPage::textFrameSet ( unsigned int _num ) const
{
    unsigned int i = 0;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
        {
            if(i==_num)
                return static_cast<KPTextObject*>(it.current());
            i++;
        }
    }
    return 0L;
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

int KPrPage::numTextObject() const
{
    int num = 0;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
            num++;
    }

    return num;
}

KPObject* KPrPage::getSelectedObj() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            return it.current();
    }
    return 0L;
}

void KPrPage::groupObjects()
{
    QPtrList<KPObject> objs;
    objs.setAutoDelete( false );
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it ) {
        if(it.current()->isSelected() && (it.current()!= m_doc->header() && it.current()!= m_doc->footer()))
            objs.append( it.current() );
    }

    if ( objs.count() > 1 ) {
        GroupObjCmd *groupObjCmd = new GroupObjCmd( i18n( "Group Objects" ), objs, m_doc,this );
        m_doc->addCommand( groupObjCmd );
        groupObjCmd->execute();
    }
}

KCommand * KPrPage::ungroupObjects()
{
    KPObject *kpobject = getSelectedObj();

    if ( kpobject && kpobject->getType() == OT_GROUP ) {
        UnGroupObjCmd *unGroupObjCmd = new UnGroupObjCmd( i18n( "Ungroup Objects" ),
                                                          (KPGroupObject*)kpobject, m_doc, this );
        unGroupObjCmd->execute();
        return unGroupObjCmd;
    }
    return 0L;
}

QPen KPrPage::getPen( const QPen &pen ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            switch ( it.current()->getType() )
            {
            case OT_LINE:
            case OT_RECT:
            case OT_ELLIPSE:
            case OT_PIE:
            case OT_AUTOFORM:
            case OT_PART:
            case OT_CLIPART:
            case OT_PICTURE:
            case OT_TEXT:
            case OT_FREEHAND:
            case OT_POLYLINE:
            case OT_QUADRICBEZIERCURVE:
            case OT_CUBICBEZIERCURVE:
            case OT_POLYGON:
            case OT_CLOSED_LINE:
            {
                return it.current()->getPen();
            }
            default:
                break;
            }
        }
    }

    return pen;
}

LineEnd KPrPage::getLineBegin( LineEnd lb ) const
{

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            switch ( it.current()->getType() )
            {
            case OT_LINE:
            {
                KPLineObject*obj=dynamic_cast<KPLineObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }
            break;
            case OT_AUTOFORM:
            {
                KPAutoformObject*obj=dynamic_cast<KPAutoformObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }
            break;
            case OT_PIE:
            {
                KPPieObject*obj=dynamic_cast<KPPieObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }
            break;
            case OT_FREEHAND:
            {
                KPFreehandObject*obj=dynamic_cast<KPFreehandObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }
            break;
            case OT_POLYLINE:
            {
                KPPolylineObject*obj=dynamic_cast<KPPolylineObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }
            break;
            case OT_QUADRICBEZIERCURVE:
            {
                KPQuadricBezierCurveObject*obj=dynamic_cast<KPQuadricBezierCurveObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }

            break;
            case OT_CUBICBEZIERCURVE:
            {
                KPCubicBezierCurveObject*obj=dynamic_cast<KPCubicBezierCurveObject*>( it.current() );
                if(obj)
                    return obj->getLineBegin();
            }
            break;
            default: break;
            }
        }
    }

    return lb;
}

LineEnd KPrPage::getLineEnd( LineEnd le ) const
{

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            switch ( it.current()->getType() ) {
            case OT_LINE:
            {
                KPLineObject* obj=dynamic_cast<KPLineObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }
            break;
            case OT_AUTOFORM:
            {
                KPAutoformObject*obj=dynamic_cast<KPAutoformObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }

            break;
            case OT_PIE:
            {
                KPPieObject*obj=dynamic_cast<KPPieObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }
            break;
            case OT_FREEHAND:
            {
                KPFreehandObject*obj=dynamic_cast<KPFreehandObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }
            break;
            case OT_POLYLINE:
            {
                KPPolylineObject*obj=dynamic_cast<KPPolylineObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }
            break;
            case OT_QUADRICBEZIERCURVE:
            {
                KPQuadricBezierCurveObject*obj=dynamic_cast<KPQuadricBezierCurveObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }
            break;
            case OT_CUBICBEZIERCURVE:
            {
                KPCubicBezierCurveObject*obj=dynamic_cast<KPCubicBezierCurveObject*>( it.current() );
                if(obj)
                    return obj->getLineEnd();
            }
            break;
            default: break;
            }
        }
    }

    return le;
}

QBrush KPrPage::getBrush( const QBrush &brush )const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getBrush();
        }
    }
    return brush;
}

FillType KPrPage::getFillType( FillType ft ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getFillType();
        }
    }

    return ft;
}

QColor KPrPage::getGColor1( const QColor &g1 )const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getGColor1();
        }
    }

    return g1;
}

QColor KPrPage::getGColor2( const QColor &g2 )const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getGColor2();
        }
    }

    return g2;
}

BCType KPrPage::getGType( BCType gt )const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getGType();
        }
    }

    return gt;
}

bool KPrPage::getGUnbalanced( bool  unbalanced ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getGUnbalanced();
        }
    }

    return unbalanced;
}

int KPrPage::getGXFactor( int xfactor )const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getGXFactor();
        }
    }

    return xfactor;
}

int KPrPage::getGYFactor( int yfactor )const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            KP2DObject *obj=dynamic_cast<KP2DObject*>(it.current() );
            if(obj)
                return obj->getGYFactor();
        }
    }

    return yfactor;
}

PieType KPrPage::getPieType( PieType pieType ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&& it.current()->getType()==OT_PIE)
        {
            KPPieObject* obj=dynamic_cast<KPPieObject*>( it.current() );
            if(obj)
                return obj->getPieType();
        }
    }

    return pieType;
}

bool KPrPage::getSticky( bool s ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected())
            return it.current()->isSticky();
    }

    return s;
}

int KPrPage::getPieLength( int pieLength ) const
{

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&& it.current()->getType()==OT_PIE)
        {
            KPPieObject *obj=dynamic_cast<KPPieObject*>( it.current() );
            if(obj)
                return obj->getPieLength();
        }
    }

    return pieLength;
}

int KPrPage::getPieAngle( int pieAngle ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&&it.current()->getType()==OT_PIE)
        {
            KPPieObject *obj=dynamic_cast<KPPieObject*>( it.current() );
            if(obj)
                return obj->getPieAngle();
        }
    }

    return pieAngle;
}

int KPrPage::getRndX( int _rx ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_RECT)
        {
            int tmp;
            KPRectObject *obj=dynamic_cast<KPRectObject*>( it.current() );
            if(obj)
            {
                obj->getRnds( _rx, tmp );
                return _rx;
            }
        }
    }

    return _rx;
}

int KPrPage::getRndY( int _ry ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_RECT)
        {
            int tmp;
            KPRectObject *obj=dynamic_cast<KPRectObject*>( it.current() );
            if(obj)
            {
                obj->getRnds( tmp, _ry );
                return _ry;
            }
        }
    }

    return _ry;
}

bool KPrPage::getCheckConcavePolygon( bool check ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_POLYGON)
        {
            KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( it.current() );
            if(obj)
                return obj->getCheckConcavePolygon();
        }
    }

    return check;
}

int KPrPage::getCornersValue( int corners ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_POLYGON)
        {
            KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( it.current() );
            if(obj)
                return obj->getCornersValue();
        }
    }

    return corners;
}

int KPrPage::getSharpnessValue( int sharpness ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_POLYGON)
        {
            KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( it.current() );
            if(obj)
                return obj->getSharpnessValue();
        }
    }

    return sharpness;
}

PictureMirrorType KPrPage::getPictureMirrorType( PictureMirrorType type ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj )
                return obj->getPictureMirrorType();
        }
    }

    return type;
}

int KPrPage::getPictureDepth( int depth ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj )
                return obj->getPictureDepth();
        }
    }

    return depth;
}

bool KPrPage::getPictureSwapRGB( bool swapRGB ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj )
                return obj->getPictureSwapRGB();
        }
    }

    return swapRGB;
}

bool KPrPage::getPictureGrayscal( bool grayscal ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj )
                return obj->getPictureGrayscal();
        }
    }

    return grayscal;
}

int KPrPage::getPictureBright( int bright ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj )
                return obj->getPictureBright();
        }
    }

    return bright;
}

QPixmap KPrPage::getPicturePixmap() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj )
                return obj->getOriginalPixmap();
        }
    }

    return QPixmap();
}

void KPrPage::lowerObjs(bool backward)
{
    KPObject *kpobject = 0;

    QPtrList<KPObject> _new;

    for ( unsigned int j = 0; j < m_objectList.count(); j++ )
        _new.append( m_objectList.at( j ) );

    _new.setAutoDelete( false );
    bool createCmd = true;
    for ( int i = 0; i < static_cast<int>( _new.count() ); i++ ) {
        kpobject = _new.at( i );
        if ( kpobject->isSelected() ) {
            if ( i == 0 )
            {
                createCmd = false;
                break;
            }
            _new.take( i );
            if ( backward )
                _new.insert(QMAX(i-1,0) ,  kpobject);
            else
                _new.insert( 0, kpobject );
        }
    }
    if ( createCmd )
    {
        LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Objects" ), m_objectList, _new, m_doc,this );
        lrCmd->execute();
        m_doc->addCommand( lrCmd );
    }
    m_doc->raiseAndLowerObject = true;

}

void KPrPage::raiseObjs(bool forward)
{
    KPObject *kpobject = 0;

    QPtrList<KPObject> _new;

    for ( unsigned int j = 0; j < m_objectList.count(); j++ )
        _new.append( m_objectList.at( j ) );

    _new.setAutoDelete( false );
    bool createCmd = true;
    for ( int i = 0; i < static_cast<int>( _new.count() ); i++ ) {
        kpobject = m_objectList.at( i );
        if ( kpobject->isSelected() ) {
            _new.take( i );
            if ( i == static_cast<int>(_new.count()) )
            {
                createCmd = false;
                break;
            }
            if ( forward )
                _new.insert( QMIN( i+1, static_cast<int>(_new.count())),  kpobject);
            else
                _new.append( kpobject );
        }
    }
    if ( createCmd )
    {
        LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Raise Objects" ), m_objectList, _new, m_doc,this );
        lrCmd->execute();
        m_doc->addCommand( lrCmd );
    }
    m_doc->raiseAndLowerObject = true;
}

void KPrPage::insertLine( const KoRect &r, const QPen & pen, LineEnd lb, LineEnd le, LineType lt )
{
    KPLineObject *kplineobject = new KPLineObject( pen, lb, le, lt );
    kplineobject->setOrig( r.x(), r.y() );
    kplineobject->setSize( r.width(), r.height() );
    kplineobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Line" ), kplineobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertRectangle( const KoRect &r, const QPen & pen, const QBrush &brush, FillType ft,
                               const QColor &g1, const QColor &g2,
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

void KPrPage::insertCircleOrEllipse( const KoRect &r, const QPen &pen, const QBrush & brush, FillType ft,
                                     const QColor &g1, const QColor &g2,
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

void KPrPage::insertPie( const KoRect &r, const QPen &pen, const QBrush &brush, FillType ft,
                         const QColor &g1, const QColor &g2,
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

KPTextObject* KPrPage::insertTextObject( const KoRect& r, const QString& text, KPresenterView *_view )
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
    return kptextobject;
}

void KPrPage::insertAutoform( const KoRect &r, const QPen &pen, const QBrush &brush, LineEnd lb, LineEnd le, FillType ft,
                              const QColor &g1, const QColor &g2, BCType gt, const QString &fileName, bool unbalanced,
                              int xfactor, int yfactor ){
    KPAutoformObject *kpautoformobject = new KPAutoformObject( pen, brush, fileName, lb, le, ft,
                                                               g1, g2, gt, unbalanced, xfactor, yfactor );
    kpautoformobject->setOrig( r.x() , r.y()  );
    kpautoformobject->setSize( r.width(),r.height() );
    kpautoformobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Autoform" ), kpautoformobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertFreehand( const KoPointArray &points, const KoRect &r, const QPen &pen,
                              LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );
    KPFreehandObject *kpfreehandobject = new KPFreehandObject( points, size, pen, lb, le );
    kpfreehandobject->setOrig( r.x(), r.y() );
    kpfreehandobject->setSize( size );
    kpfreehandobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Freehand" ), kpfreehandobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertPolyline( const KoPointArray &points, const KoRect &r, const QPen &pen,
                              LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );
    KPPolylineObject *kppolylineobject = new KPPolylineObject( points, size, pen, lb, le );
    kppolylineobject->setOrig( r.x(), r.y() );
    kppolylineobject->setSize( size );
    kppolylineobject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polyline" ), kppolylineobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertQuadricBezierCurve( const KoPointArray &points, const KoPointArray &allPoints,
                                        const KoRect &r, const QPen &pen,
                                        LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );

    KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject(
        points, allPoints, size, pen, lb, le );
    kpQuadricBezierCurveObject->setOrig( r.x(), r.y() );
    kpQuadricBezierCurveObject->setSize( size );
    kpQuadricBezierCurveObject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Quadric Bezier Curve" ),
                                          kpQuadricBezierCurveObject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertCubicBezierCurve( const KoPointArray &points, const KoPointArray &allPoints,
                                      const KoRect &r, const QPen &pen,
                                      LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );

    KPCubicBezierCurveObject *kpCubicBezierCurveObject = new KPCubicBezierCurveObject( points, allPoints, size, pen, lb, le );
    kpCubicBezierCurveObject->setOrig( r.x(), r.y() );
    kpCubicBezierCurveObject->setSize( size );
    kpCubicBezierCurveObject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Cubic Bezier Curve" ), kpCubicBezierCurveObject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertPolygon( const KoPointArray &points, const KoRect &r, const QPen &pen, const QBrush &brush, FillType ft,
                             const QColor &g1, const QColor &g2, BCType gt, bool unbalanced, int xfactor, int yfactor,
                             bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
{
    KoSize size( r.width(), r.height() );

    KPPolygonObject *kpPolygonObject = new KPPolygonObject( points, size, pen, brush, ft,
                                                            g1, g2, gt, unbalanced, xfactor, yfactor,
                                                            _checkConcavePolygon, _cornersValue, _sharpnessValue );
    kpPolygonObject->setOrig( r.x(), r.y() );
    kpPolygonObject->setSize( size );
    kpPolygonObject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Polygon" ), kpPolygonObject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

void KPrPage::insertClosedLine( const KoPointArray &points, const KoRect &r, const QPen &pen, const QBrush &brush,
                                FillType ft,const QColor &g1, const QColor &g2,
                                BCType gt, bool unbalanced, int xfactor, int yfactor, ToolEditMode _mode )
{
    QString _type;
    QString _name;
    if ( _mode == INS_CLOSED_FREEHAND ) {
        _type = i18n( "Closed Freehand" );
        _name = i18n( "Insert Closed Freehand" );
    }
    else if ( _mode == INS_CLOSED_POLYLINE ) {
        _type = i18n( "Closed Polyline" );
        _name = i18n( "Insert Closed Polyline" );
    }
    else if ( _mode == INS_CLOSED_QUADRICBEZIERCURVE ) {
        _type = i18n( "Closed Quadric Bezier Curve" );
        _name = i18n( "Insert Closed Quadric Bezier Curve" );
    }
    else if ( _mode == INS_CLOSED_CUBICBEZIERCURVE ) {
        _type = i18n( "Closed Cubic Bezier Curve" );
        _name = i18n( "Insert Closed Cubic Bezier Curve" );
    }

    KoSize size( r.width(), r.height() );

    KPClosedLineObject *kpClosedLineObject = new KPClosedLineObject( points, size, pen, brush, ft,
                                                                     g1, g2, gt, unbalanced, xfactor, yfactor, _type );
    kpClosedLineObject->setOrig( r.x(), r.y() );
    kpClosedLineObject->setSize( size );
    kpClosedLineObject->setSelected( true );



    InsertCmd *insertCmd = new InsertCmd( _name, kpClosedLineObject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}


KCommand * KPrPage::alignObjsLeft(const KoRect &rect)
{
    MoveByCmd2 *moveByCmd2=0L;
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<KoPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );

    KoRect area(rect);
    if ( !area.isValid () )
        area = getPageRect( );

    double _x =  area.x();


    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected() && !it.current()->isProtect())
        {
            _objects.append( it.current() );
            if( !newPosition &&_x != it.current()->getRealOrig().x())
                newPosition=true;
            _diffs.append( new KoPoint( _x - it.current()->getRealOrig().x(), 0 ) );
        }
    }

    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align Objects Left" ), _diffs, _objects, m_doc, this );
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete( true );
        _diffs.clear();
    }
    return moveByCmd2;
}

KCommand * KPrPage::alignObjsCenterH(const KoRect &rect)
{
    MoveByCmd2 *moveByCmd2=0L;
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<KoPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    KoRect area(rect);
    if ( !area.isValid () )
        area = getPageRect( );

    double _x = area.x();
    double _w = area.width();

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected()&& !it.current()->isProtect())
        {
            _objects.append( it.current() );
            if(!newPosition && (( _w - it.current()->getRealSize().width() ) / 2 - it.current()->getRealOrig().x() + _x)!=0)
                newPosition=true;
            _diffs.append( new KoPoint( ( _w - it.current()->getRealSize().width() ) / 2 - it.current()->getRealOrig().x() + _x, 0 ) );
        }
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align Objects Centered (horizontal)" ),
                                     _diffs, _objects, m_doc, this );
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete(true);
        _diffs.clear();
    }
    return moveByCmd2;
}

KCommand * KPrPage::alignObjsRight(const KoRect &rect)
{
    MoveByCmd2 *moveByCmd2=0L;
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<KoPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    KoRect area(rect);
    if ( !area.isValid () )
        area = getPageRect( );

    double _w = area.x() + area.width();
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected()&& !it.current()->isProtect())
        {
            _objects.append( it.current() );
            if(!newPosition && (( _w - it.current()->getRealSize().width() ) != it.current()->getRealOrig().x()))
                newPosition=true;
            _diffs.append( new KoPoint( ( _w - it.current()->getRealSize().width() ) - it.current()->getRealOrig().x(), 0 ) );
        }
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align Objects Right" ), _diffs, _objects, m_doc, this);
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete( true );
        _diffs.clear();
    }
    return moveByCmd2;
}

KCommand *KPrPage::alignObjsTop(const KoRect &rect)
{
    MoveByCmd2 *moveByCmd2=0L;
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<KoPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    double  _y;

    KoRect area(rect);
    if ( !area.isValid () )
        area = getPageRect( );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected()&& !it.current()->isProtect())
        {
            _y = area.y();
            _objects.append( it.current() );
            if(!newPosition && (_y != it.current()->getRealOrig().y()))
                newPosition=true;

            _diffs.append( new KoPoint( 0, _y - it.current()->getRealOrig().y() ) );
        }
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align Objects Top" ), _diffs, _objects, m_doc, this );
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete( true );
        _diffs.clear();
    }
    return moveByCmd2;
}

KCommand * KPrPage::alignObjsCenterV(const KoRect &rect)
{
    MoveByCmd2 *moveByCmd2=0L;
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<KoPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    double  _y, _h;
    KoRect area(rect);
    if ( !area.isValid () )
        area = getPageRect( );


    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected()&& !it.current()->isProtect())
        {
            _y = area.y();
            _h = area.height();
            _objects.append( it.current() );
            if(!newPosition &&(( _h - it.current()->getRealSize().height() ) / 2 - it.current()->getRealOrig().y() + _y )!=0)
                newPosition=true;
            _diffs.append( new KoPoint( 0, ( _h - it.current()->getRealSize().height() ) / 2 -
                                        it.current()->getRealOrig().y() + _y ) );
        }
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align Objects Center/Vertical" ), _diffs, _objects, m_doc, this );
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete(true);
        _diffs.clear();
    }
    return moveByCmd2;
}

KCommand * KPrPage::alignObjsBottom(const KoRect &rect)
{
    MoveByCmd2 *moveByCmd2=0L;
    bool newPosition=false;
    QPtrList<KPObject> _objects;
    QPtrList<KoPoint> _diffs;
    _objects.setAutoDelete( false );
    _diffs.setAutoDelete( false );
    double  _h;
    KoRect area(rect);
    if ( !area.isValid () )
        area = getPageRect( );
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected()&& !it.current()->isProtect())
        {
            _h = area.y() + area.height();
            _objects.append( it.current() );
            if(!newPosition && _h != (it.current()->getRealSize().height() + it.current()->getRealOrig().y()))
                newPosition=true;
            _diffs.append( new KoPoint( 0, _h - it.current()->getRealSize().height() - it.current()->getRealOrig().y() ) );
        }
    }

    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align Objects Bottom" ), _diffs, _objects, m_doc, this );
        moveByCmd2->execute();
    }
    else
    {
        _diffs.setAutoDelete( true );
        _diffs.clear();
    }
    return moveByCmd2;
}

KPPartObject* KPrPage::insertObject( const KoRect& _rect, KoDocumentEntry& _e )
{
    KoDocument* doc = _e.createDoc( m_doc );
    if ( !doc )
        return NULL;

    doc->setInitDocFlags( KoDocument::InitDocEmbedded );
    if ( !doc->initDoc() )
        return NULL;

    QRect r = QRect( (int)_rect.left(), (int)_rect.top(),
                     (int)_rect.width(), (int)_rect.height() );
    KPresenterChild* ch = new KPresenterChild( m_doc, doc, r );
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

    return kppartobject;
}

KCommand* KPrPage::setPen( const QPen &pen, LineEnd lb, LineEnd le, int flags, QPtrList<KPObject> list )
{
    KPObject *kpobject = 0;
    PenCmd *penCmd=0L;
    bool cmdCreate=false;
    KMacroCommand *cmd=new KMacroCommand(i18n("Apply Styles"));

    QPtrList<KPObject> _objects;
    QPtrList<PenCmd::Pen> _oldPen;
    PenCmd::Pen _newPen, *ptmp;

    _newPen.pen = pen;
    _newPen.lineBegin = lb;
    _newPen.lineEnd = le;

    _objects.setAutoDelete( false );
    _oldPen.setAutoDelete( false );

    QPtrListIterator<KPObject> it( list );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
            ptmp = new PenCmd::Pen;
            switch ( kpobject->getType() ) {
            case OT_LINE:
            {
                KPLineObject *obj=dynamic_cast<KPLineObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_RECT:
            {
                KPRectObject *obj=dynamic_cast<KPRectObject*>( kpobject );
                if(obj)
                    ptmp->pen = obj->getPen();
            }
            break;
            case OT_ELLIPSE:
            {
                KPEllipseObject *obj=dynamic_cast<KPEllipseObject*>( kpobject );
                if(obj)
                    ptmp->pen = obj->getPen();
            }
            break;
            case OT_AUTOFORM:
            {
                KPAutoformObject* obj=dynamic_cast<KPAutoformObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_PIE:
            {
                KPPieObject *obj=dynamic_cast<KPPieObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_PART:
            {
                KPPartObject *obj=dynamic_cast<KPPartObject*>( kpobject );
                if(obj)
                    ptmp->pen = obj->getPen();
            }
            break;
            case OT_TEXT:
            {
                KPTextObject *obj=dynamic_cast<KPTextObject*>( kpobject );
                if(obj)
                    ptmp->pen = obj->getPen();
            }
            break;
            case OT_CLIPART:
            case OT_PICTURE:
            {
                KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
                if(obj)
                    ptmp->pen = obj->getPen();
            }
            break;
            case OT_FREEHAND:
            {
                KPFreehandObject *obj=dynamic_cast<KPFreehandObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_POLYLINE:
            {
                KPPolylineObject *obj=dynamic_cast<KPPolylineObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_QUADRICBEZIERCURVE:
            {
                KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_CUBICBEZIERCURVE:
            {
                KPCubicBezierCurveObject *obj=dynamic_cast<KPCubicBezierCurveObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = obj->getPen();
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_POLYGON:
            {
                KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
                if(obj)
                    ptmp->pen = obj->getPen();
            }
            break;
            case OT_CLOSED_LINE: {
                KPClosedLineObject *obj = dynamic_cast<KPClosedLineObject*>( kpobject );
                if ( obj )
                    ptmp->pen = obj->getPen();
            } break;

            case OT_GROUP:
            {
                KPGroupObject *obj=dynamic_cast<KPGroupObject*>( kpobject );
                if(obj)
                {
                    obj->selectAllObj();
                    KCommand *cmd2=setPen(pen, lb, le, flags, obj->objectList());
                    obj->deSelectAllObj();
                    if(cmd2)
                    {
                        cmd->addCommand(cmd2);
                        cmdCreate=true;
                    }
                }
            }
            break;
            default: break;
            }
            _oldPen.append( ptmp );
            _objects.append( kpobject );
        }
    }

    if ( !_objects.isEmpty() ) {
        penCmd = new PenCmd( i18n( "Apply Styles" ), _oldPen, _newPen, _objects, m_doc, this, flags );
        penCmd->execute();
        cmd->addCommand(penCmd);
        cmdCreate=true;
    } else {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
    }
    if(cmdCreate)
        return cmd;
    else
    {
        delete cmd;
        cmd=0L;
    }
    return cmd;
}

KCommand * KPrPage::setBrush( const QBrush &brush, FillType ft, const QColor &g1, const QColor &g2,
                              BCType gt, bool unbalanced, int xfactor, int yfactor, int flags, QPtrList<KPObject>list )
{
    KPObject *kpobject = 0;
    BrushCmd *brushCmd=0L;
    bool cmdCreate=false;
    KMacroCommand *cmd=new KMacroCommand(i18n("Apply Style"));

    QPtrList<KPObject> _objects;
    QPtrList<BrushCmd::Brush> _oldBrush;
    BrushCmd::Brush _newBrush, *btmp;

    _newBrush.brush = QBrush( brush );
    _newBrush.fillType = ft;
    _newBrush.gColor1 = g1;
    _newBrush.gColor2 = g2;
    _newBrush.gType = gt;
    _newBrush.unbalanced = unbalanced;
    _newBrush.xfactor = xfactor;
    _newBrush.yfactor = yfactor;

    _objects.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( list );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
            btmp = new BrushCmd::Brush;
            switch ( kpobject->getType() ) {
            case OT_RECT:
            {
                KPRectObject *obj=dynamic_cast<KPRectObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_ELLIPSE:
            {
                KPEllipseObject *obj=dynamic_cast<KPEllipseObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = obj->getBrush();
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_AUTOFORM:
            {
                KPAutoformObject* obj=dynamic_cast<KPAutoformObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_PIE:
            {
                KPPieObject *obj=dynamic_cast<KPPieObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_PART:
            {
                KPPartObject *obj=dynamic_cast<KPPartObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_TEXT:
            {
                KPTextObject *obj=dynamic_cast<KPTextObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_CLIPART:
            case OT_PICTURE:
            {
                KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_POLYGON:
            {
                KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = obj->getBrush();
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            }
            break;
            case OT_CLOSED_LINE: {
                KPClosedLineObject *obj = dynamic_cast<KPClosedLineObject*>( kpobject );
                if( obj ) {
                    btmp->brush = obj->getBrush();
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    btmp->unbalanced = obj->getGUnbalanced();
                    btmp->xfactor = obj->getGXFactor();
                    btmp->yfactor = obj->getGYFactor();
                }
            } break;
            case OT_GROUP:
            {
                KPGroupObject *obj=dynamic_cast<KPGroupObject*>( kpobject );
                if(obj)
                {
                    obj->selectAllObj();
                    KCommand *cmd2=setBrush(brush, ft, g1, g2, gt, unbalanced,
                                            xfactor,yfactor, flags, obj->objectList());
                    obj->deSelectAllObj();
                    if(cmd2)
                    {
                        cmd->addCommand(cmd2);
                        cmdCreate=true;
                    }
                }
            }
            break;
            default: break;
            }
            _oldBrush.append( btmp );
            _objects.append( kpobject );
        }
    }

    if ( !_objects.isEmpty() ) {
        brushCmd = new BrushCmd( i18n( "Apply Styles" ), _oldBrush, _newBrush, _objects, m_doc, this, flags );
        brushCmd->execute();
        cmd->addCommand(brushCmd);
        cmdCreate=true;
    } else {
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }
    if(cmdCreate)
        return cmd;
    else
    {
        delete cmd;
        cmd=0L;
    }
    return cmd;
}

int KPrPage::getPenBrushFlags( QPtrList<KPObject>list ) const
{
    int flags = 0;

    QPtrListIterator<KPObject> it( list);
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            switch ( it.current()->getType() ) {
            case OT_LINE:
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                flags = flags | StyleDia::SdEndBeginLine;
                break;
            case OT_FREEHAND:
                if(!static_cast<KPFreehandObject*>(it.current())->isClosed())
                    flags = flags | StyleDia::SdEndBeginLine;
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                break;
            case OT_POLYLINE:
                if(!static_cast<KPPolylineObject*>(it.current())->isClosed())
                    flags = flags | StyleDia::SdEndBeginLine;
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;

                break;
            case OT_QUADRICBEZIERCURVE:
                if(!static_cast<KPQuadricBezierCurveObject*>(it.current())->isClosed())
                    flags = flags | StyleDia::SdEndBeginLine;
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                break;
            case OT_CUBICBEZIERCURVE:
                if(!static_cast<KPCubicBezierCurveObject*>(it.current())->isClosed())
                    flags = flags | StyleDia::SdEndBeginLine;
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                break;
            case OT_PIE:
                flags=flags | StyleDia::SdPen | StyleDia::SdPie;
                if((static_cast<KPPieObject*>(it.current())->getPieType())!=PT_ARC)
                    flags=flags |StyleDia::SdBrush;
                break;
            case OT_RECT:
                flags = flags | StyleDia::SdPen | StyleDia::SdRectangle;
                flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                break;
            case OT_POLYGON:
                flags = flags | StyleDia::SdPen | StyleDia::SdPolygon;
                flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                break;
            case OT_PART:  case OT_ELLIPSE:
            case OT_TEXT:  case OT_CLOSED_LINE:
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                break;
            case OT_CLIPART:
            case OT_PICTURE:
                flags |= StyleDia::SdPen | StyleDia::SdPicture | StyleDia::SdBrush ;
                flags |= StyleDia::SdGradient;
                break;
            case OT_AUTOFORM:
            {
                flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
            }
            break;
            case OT_GROUP:
            {
                KPGroupObject *obj=dynamic_cast<KPGroupObject*>( it.current() );
                if(obj)
                {
                    obj->selectAllObj();
                    flags = flags | getPenBrushFlags( obj->objectList() );
                    obj->deSelectAllObj();
                }
            }
            break;
            default: break;
            }
        }
    }
    return flags;
}


KCommand* KPrPage::setPieSettings( PieType pieType, int angle, int len, int flags )
{
    PieValueCmd *pieValueCmd=0L;
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
            if ( it.current()->isSelected() ) {
                KPPieObject *obj= dynamic_cast<KPPieObject*>( it.current() );
                if(obj)
                {
                    tmp = new PieValueCmd::PieValues;
                    tmp->pieType = obj->getPieType();
                    tmp->pieAngle = obj->getPieAngle();
                    tmp->pieLength = obj->getPieLength();
                    _oldValues.append( tmp );
                    _objects.append( obj );
                }
            }
        }
    }

    if ( !_objects.isEmpty() ) {
        pieValueCmd = new PieValueCmd( i18n( "Change Pie/Arc/Chord Values" ),
                                       _oldValues, _newValues, _objects, m_doc,  this, flags );
        pieValueCmd->execute();
    } else {
        _oldValues.setAutoDelete( true );
        _oldValues.clear();
    }

    m_doc->setModified(true);
    return pieValueCmd;
}

KCommand* KPrPage::setRectSettings( int _rx, int _ry, int flags )
{
    RectValueCmd *rectValueCmd=0L;
    bool changed=false;
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
            if ( it.current()->isSelected() ) {
                KPRectObject *obj=dynamic_cast<KPRectObject*>( it.current() );
                if(obj)
                {
                    tmp = new RectValueCmd::RectValues;
                    obj->getRnds( tmp->xRnd, tmp->yRnd );
                    _oldValues.append( tmp );
                    _objects.append(it.current() );
                    if(!changed && (tmp->xRnd!=_newValues.xRnd
                                    ||tmp->yRnd!=_newValues.yRnd) )
                        changed=true;
                }
            }
        }
    }

    if ( !_objects.isEmpty() && changed ) {
        rectValueCmd = new RectValueCmd( i18n( "Change Rectangle Values" ), _oldValues,
                                         _newValues, _objects, m_doc, this, flags );
        rectValueCmd->execute();
    } else {
        _oldValues.setAutoDelete( true );
        _oldValues.clear();
    }

    m_doc->setModified(true);
    return rectValueCmd;
}

KCommand* KPrPage::setPolygonSettings( bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue, int flags )
{
    bool changed = false;
    PolygonSettingCmd *polygonSettingCmd=0L;
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
            if ( it.current()->isSelected() ) {
                tmp = new PolygonSettingCmd::PolygonSettings;
                tmp->checkConcavePolygon = static_cast<KPPolygonObject*>(it.current())->getCheckConcavePolygon();
                tmp->cornersValue = static_cast<KPPolygonObject*>(it.current())->getCornersValue();
                tmp->sharpnessValue = static_cast<KPPolygonObject*>(it.current())->getSharpnessValue();
                _oldSettings.append( tmp );
                _objects.append( it.current() );

                if( !changed && ( tmp->checkConcavePolygon !=_newSettings.checkConcavePolygon
                                  || tmp->cornersValue != _newSettings.cornersValue
                                  || tmp->sharpnessValue != _newSettings.sharpnessValue ) )
                    changed = true;
            }
        }
    }

    if ( !_objects.isEmpty() && changed ) {
        polygonSettingCmd = new PolygonSettingCmd( i18n( "Change Polygon Settings" ), _oldSettings,
                                                   _newSettings, _objects, m_doc, this, flags );
        polygonSettingCmd->execute();
    }
    else {
        _oldSettings.setAutoDelete( true );
        _oldSettings.clear();
    }

    m_doc->setModified( true );

    return polygonSettingCmd;
}

KCommand* KPrPage::setPictureSettings( PictureMirrorType _mirrorType, int _depth, bool _swapRGB, bool _grayscal, int _bright )
{
    bool changed = false;
    PictureSettingCmd *pictureSettingCmd = 0L;
    QPtrList<KPObject> _objects;
    QPtrList<PictureSettingCmd::PictureSettings> _oldSettings;
    PictureSettingCmd::PictureSettings _newSettings, *tmp;

    _objects.setAutoDelete( false );
    _oldSettings.setAutoDelete( false );

    _newSettings.mirrorType = _mirrorType;
    _newSettings.depth = _depth;
    _newSettings.swapRGB = _swapRGB;
    _newSettings.grayscal = _grayscal;
    _newSettings.bright = _bright;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_PICTURE ) {
            if ( it.current()->isSelected() ) {
                tmp = new PictureSettingCmd::PictureSettings;
                tmp->mirrorType = static_cast<KPPixmapObject*>(it.current())->getPictureMirrorType();
                tmp->depth =  static_cast<KPPixmapObject*>(it.current())->getPictureDepth();
                tmp->swapRGB =  static_cast<KPPixmapObject*>(it.current())->getPictureSwapRGB();
                tmp->grayscal =  static_cast<KPPixmapObject*>(it.current())->getPictureGrayscal();
                tmp->bright =  static_cast<KPPixmapObject*>(it.current())->getPictureBright();
                _oldSettings.append( tmp );
                _objects.append( it.current() );

                if( !changed && ( tmp->mirrorType != _newSettings.mirrorType
                                  || tmp->depth != _newSettings.depth
                                  || tmp->swapRGB != _newSettings.swapRGB
                                  || tmp->grayscal != _newSettings.grayscal
                                  || tmp->bright != _newSettings.bright ) )
                    changed = true;
            }
        }
    }

    if ( !_objects.isEmpty() && changed ) {
        pictureSettingCmd = new PictureSettingCmd( i18n( "Change Picture Settings" ),
                                                   _oldSettings, _newSettings, _objects, m_doc );
        pictureSettingCmd->execute();
    }
    else {
        _oldSettings.setAutoDelete( true );
        _oldSettings.clear();
    }

    m_doc->setModified( true );

    return pictureSettingCmd;
}

KCommand* KPrPage::setBrushColor( const QColor &c, bool fill, QPtrList<KPObject> list )
{
    KPObject *kpobject = 0;
    BrushCmd *brushCmd=0L;
    QPtrList<KPObject> _objects;
    QPtrList<BrushCmd::Brush> _oldBrush;
    BrushCmd::Brush _newBrush, *btmp;
    KMacroCommand *cmd=new KMacroCommand(i18n("Change Brush"));
    bool cmdCreate=false;
    _newBrush.fillType = FT_BRUSH;
    if ( !fill )
        _newBrush.brush = Qt::NoBrush;
    else
        _newBrush.brush = QBrush( c );

    _objects.setAutoDelete( false );
    _oldBrush.setAutoDelete( false );

    QPtrListIterator<KPObject> it( list );
    for ( ; it.current() ; ++it )
    {
        kpobject=it.current();
        if ( kpobject->isSelected() && kpobject->getType() != OT_LINE
             && kpobject->getType() != OT_FREEHAND
             && kpobject->getType() != OT_POLYLINE
             && kpobject->getType() != OT_QUADRICBEZIERCURVE
             && kpobject->getType() != OT_CUBICBEZIERCURVE ) {
            btmp = new BrushCmd::Brush;
            switch ( kpobject->getType() )
            {
            case OT_RECT: {
                KPRectObject *obj=dynamic_cast<KPRectObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_ELLIPSE: {
                KPEllipseObject *obj=dynamic_cast<KPEllipseObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = obj->getBrush();
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_AUTOFORM: {
                KPAutoformObject *obj=dynamic_cast<KPAutoformObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_PIE: {
                KPPieObject *obj=dynamic_cast<KPPieObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_PART: {
                KPPartObject *obj=dynamic_cast<KPPartObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_TEXT: {
                KPTextObject *obj=dynamic_cast<KPTextObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_CLIPART:
            case OT_PICTURE: {
                KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_CLOSED_LINE: {
                KPClosedLineObject *obj = dynamic_cast<KPClosedLineObject*>( kpobject );
                if( obj ) {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_POLYGON: {
                KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
                if(obj)
                {
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
            case OT_GROUP:
            {
                KPGroupObject *obj=dynamic_cast<KPGroupObject*>( kpobject );
                if(obj)
                {
                    obj->selectAllObj();
                    KCommand* cmd2= setBrushColor( c, fill, obj->objectList() );
                    obj->deSelectAllObj();
                    if(cmd2)
                    {
                        cmd->addCommand(cmd2);
                        cmdCreate=true;
                    }
                }
            }
            break;
            default: continue; break;
            }
            _oldBrush.append( btmp );
            _objects.append( kpobject );
        }
    }

    if ( !_objects.isEmpty() ) {
        brushCmd = new BrushCmd( i18n( "Change Brush" ), _oldBrush, _newBrush, _objects, m_doc, this );
        brushCmd->execute();
        cmd->addCommand( brushCmd );
        cmdCreate=true;
    } else {
        _oldBrush.setAutoDelete( true );
        _oldBrush.clear();
    }

    if(cmdCreate)
        return cmd;
    else
    {
        delete cmd;
        cmd=0L;
    }

    m_doc->setModified(true);
    return cmd;
}

void KPrPage::slotRepaintVariable()
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->getType() == OT_TEXT )
            m_doc->repaint( it.current() );
    }
}

void KPrPage::recalcPageNum()
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->getType() == OT_TEXT )
            ( (KPTextObject*)it.current() )->recalcPageNum( this );
    }
}

void KPrPage::changePicture( const KURL & url )
{
    // filename has been chosen in KPresenterView with a filedialog,
    // so we know it exists
    KoPicture image = m_doc->pictureCollection()->downloadPicture( url );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_PICTURE)
        {
            KPPixmapObject* obj=dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj)
            {
                KPPixmapObject *pix = new KPPixmapObject( m_doc->pictureCollection(), image.getKey() );
                ChgPixCmd *chgPixCmd = new ChgPixCmd( i18n( "Change Pixmap" ),obj,pix, m_doc,this );
                chgPixCmd->execute();
                m_doc->addCommand( chgPixCmd );
            }
            break;
        }
    }
}

void KPrPage::insertPicture( const QString &filename, int _x , int _y )
{
    KoPictureKey key = m_doc->pictureCollection()->loadPicture( filename ).getKey();
    KPPixmapObject *kppixmapobject = new KPPixmapObject(m_doc->pictureCollection() , key );
    double x=m_doc->zoomHandler()->unzoomItX(_x);
    double y=m_doc->zoomHandler()->unzoomItY(_y);

    kppixmapobject->setOrig( ( x  / m_doc->getGridX() ) * m_doc->getGridX(),
                             ( y  / m_doc->getGridY() ) * m_doc->getGridY());
    kppixmapobject->setSelected( true );

    kppixmapobject->setSize(m_doc->zoomHandler()->unzoomItX(kppixmapobject->originalSize().width()),
                            m_doc->zoomHandler()->unzoomItY(kppixmapobject->originalSize().height()));

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, m_doc, this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );

    KoRect s = getPageRect();
    float fakt = 1;
    if ( kppixmapobject->getSize().width() > s.width() )
        fakt = (float)s.width() / (float)kppixmapobject->getSize().width();
    if ( kppixmapobject->getSize().height() > s.height() )
        fakt = QMIN( fakt, (float)s.height() / (float)kppixmapobject->getSize().height() );

    //kdDebug(33001) << k_funcinfo << "Fakt: " << fakt << endl;

    if ( fakt < 1 ) {
        int w = (int)( fakt * (float)kppixmapobject->getSize().width() );
        int h = (int)( fakt * (float)kppixmapobject->getSize().height() );
        //kdDebug(33001) << k_funcinfo << "Size: " << w << ", " << h << endl;
        kppixmapobject->setOrig(0,0);
        kppixmapobject->setSize(w, h);
        m_doc->repaint( false );
    }
}

void KPrPage::insertPicture( const QString &_file, const KoRect &_rect )
{
    KoPictureKey key = m_doc->pictureCollection()->loadPicture( _file ).getKey();
    KPPixmapObject *kppixmapobject = new KPPixmapObject( m_doc->pictureCollection(), key );

    kppixmapobject->setOrig( _rect.x(), _rect.y() );
    kppixmapobject->setSize( _rect.width(), _rect.height() );
    kppixmapobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, m_doc, this );
    insertCmd->execute();

    m_doc->addCommand( insertCmd );
}

void KPrPage::enableEmbeddedParts( bool f )
{
    KPPartObject *obj=0L;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_PART)
        {
            obj=dynamic_cast<KPPartObject*>( it.current() );
            if(obj)
                obj->enableDrawing( f );
        }
    }
}

void KPrPage::deletePage( )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
        it.current()->setSelected( true );

    deleteObjs( false );
}

void KPrPage::setBackColor(const  QColor &backColor1, const QColor &backColor2, BCType bcType,
                           bool unbalanced, int xfactor, int yfactor )
{
    kpbackground->setBackColor1( backColor1 );
    kpbackground->setBackColor2( backColor2 );
    kpbackground->setBackColorType( bcType );
    kpbackground->setBackUnbalanced( unbalanced );
    kpbackground->setBackXFactor( xfactor );
    kpbackground->setBackYFactor( yfactor );
}

void KPrPage::setBackPicture( const KoPictureKey & key )
{
    kpbackground->setBackPicture( key );
}

bool KPrPage::getBackUnbalanced() const
{
    return kpbackground->getBackUnbalanced();
}

void KPrPage::setBackView( BackView backView )
{
    kpbackground->setBackView( backView );
}

void KPrPage::setBackType( BackType backType )
{
    kpbackground->setBackType( backType );
}

void KPrPage::setPageEffect( PageEffect pageEffect )
{
    kpbackground->setPageEffect( pageEffect );
}

void KPrPage::setPageTimer( int pageTimer )
{
    kpbackground->setPageTimer( pageTimer );
}

void KPrPage::setPageSoundEffect( bool soundEffect )
{
    kpbackground->setPageSoundEffect( soundEffect );
}

void KPrPage::setPageSoundFileName( const QString &fileName )
{
    kpbackground->setPageSoundFileName( fileName );
}

BackType KPrPage::getBackType() const
{
    return kpbackground->getBackType();
}

BackView KPrPage::getBackView() const
{
    return kpbackground->getBackView();
}

KoPictureKey KPrPage::getBackPictureKey() const
{
    return kpbackground->getBackPictureKey();
}

KoPicture KPrPage::getBackPicture() const
{
    return kpbackground->getBackPicture();
}

QColor KPrPage::getBackColor1() const
{
    return kpbackground->getBackColor1();
}

QColor KPrPage::getBackColor2() const
{
    return kpbackground->getBackColor2();
}

int KPrPage::getBackXFactor() const
{
    return kpbackground->getBackXFactor();
}

int KPrPage::getBackYFactor() const
{
    return kpbackground->getBackYFactor();
}

BCType KPrPage::getBackColorType() const
{
    return kpbackground->getBackColorType();
}

PageEffect KPrPage::getPageEffect() const
{
    return kpbackground->getPageEffect();
}

int KPrPage::getPageTimer() const
{
    return kpbackground->getPageTimer();
}

bool KPrPage::getPageSoundEffect() const
{
    return kpbackground->getPageSoundEffect();
}

QString KPrPage::getPageSoundFileName() const
{
    return kpbackground->getPageSoundFileName();
}

KoRect KPrPage::getPageRect() const
{
    const KoPageLayout& p = m_doc->pageLayout();
    return KoRect( p.ptLeft, p.ptTop, p.ptWidth - p.ptRight - p.ptLeft, p.ptHeight - p.ptTop - p.ptBottom );
}

QRect KPrPage::getZoomPageRect() const
{
    return m_doc->zoomHandler()->zoomRect(getPageRect());
}

void KPrPage::completeLoading( bool _clean, int lastObj )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        // Pictures and cliparts have been loaded from the store, we can now
        // get the picture from the collection, and set it in the image/clipart object
        if ( ( it.current()->getType() == OT_PICTURE )
             || ( it.current()->getType() == OT_CLIPART ) ) {
            if ( _clean || m_objectList.findRef( it.current() ) > lastObj )
            {
                KPPixmapObject* obj=dynamic_cast<KPPixmapObject*>( it.current());
                if(obj)
                    obj->reload();
            }
        }
        else if ( it.current()->getType() == OT_TEXT )
        {
            KPTextObject*obj=dynamic_cast<KPTextObject*>( it.current() );
            if(obj)
                obj->recalcPageNum( this );
        }
        else if ( it.current()->getType() == OT_GROUP )
            completeLoadingForGroupObject( it.current() );
    }
    kpbackground->reload();
}

void KPrPage::completeLoadingForGroupObject( KPObject *_obj )
{
    KPGroupObject *_groupObj = static_cast<KPGroupObject*>( _obj );

    if ( _groupObj ) {
        QPtrListIterator<KPObject> it( _groupObj->objectList() );
        for ( ; it.current(); ++it ) {
            if ( ( it.current()->getType() == OT_PICTURE )
                 || ( it.current()->getType() == OT_CLIPART ) ) {
                KPPixmapObject *_pixObj = dynamic_cast<KPPixmapObject*>( it.current() );
                if ( _pixObj )
                    _pixObj->reload();
            }
            else if ( it.current()->getType() == OT_TEXT ) {
                KPTextObject *_textObj=  dynamic_cast<KPTextObject*>( it.current() );
                if ( _textObj )
                    _textObj->recalcPageNum(  this );
            }
            else if ( it.current()->getType() == OT_GROUP )
                completeLoadingForGroupObject( it.current() ); // recursion
        }
    }
}

KCommand * KPrPage::replaceObjs( bool createUndoRedo, double _orastX,double _orastY,const QColor & _txtBackCol,
                                 const QColor & _otxtBackCol )
{
    KPObject *kpobject = 0;
    double ox=0, oy=0;
    QPtrList<KPObject> _objects;
    QValueList<KoPoint> _diffs;
    _objects.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( m_objectList.count() ); i++ ) {
        kpobject = m_objectList.at( i );
        ox = kpobject->getOrig().x();
        oy = kpobject->getOrig().y();
        ox = static_cast<int>(ox / m_doc->getGridX() * m_doc->getGridX());
        oy = static_cast<int>(oy / m_doc->getGridY() * m_doc->getGridY());

        _diffs.append( KoPoint( ox - kpobject->getOrig().x(), oy - kpobject->getOrig().y() ) );
        _objects.append( kpobject );
    }

    SetOptionsCmd *setOptionsCmd = new SetOptionsCmd( i18n( "Set New Options" ), _diffs, _objects, m_doc->getGridX(),
                                                      m_doc->getGridY(),_orastX, _orastY, _txtBackCol, _otxtBackCol, m_doc );
    if ( createUndoRedo )
        return setOptionsCmd;

    delete setOptionsCmd;
    return 0L;
}

QString KPrPage::manualTitle() const
{
    return m_manualTitle;
}

void KPrPage::insertManualTitle(const QString & title)
{
    m_manualTitle=title;
}

QString KPrPage::pageTitle( const QString &_title ) const
{
    // If a user sets a title with manual, return it.
    if ( !m_manualTitle.isEmpty() )
        return m_manualTitle;

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

    QString txt/* = textobject->textDocument()->text().stripWhiteSpace()*/;

    //search custom variable.
    if ( textobject->textDocument()->firstParag() )
    {
        QString tmp = textobject->textDocument()->firstParag()->string()->toString();
        for ( int i = 0 ; i < (int)tmp.length() ; ++i )
        {
            KoTextStringChar * ch = textobject->textDocument()->firstParag()->at( i );
            if ( ch->isCustom() )
            {
                KoVariable * var = dynamic_cast<KoVariable *>(ch->customItem());
                if ( var )
                    txt += var->text(true);
            }
            else
                txt += ch->c;
        }
    }
    if ( txt.stripWhiteSpace().isEmpty() || txt=="\n" )
        return _title;
    return txt;
}

void KPrPage::setNoteText( const QString &_text )
{
    m_noteText=_text;
    m_doc->setModified(true);
}

QString KPrPage::noteText(  )const
{
    return m_noteText;
}

void KPrPage::makeUsedPixmapList()
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if( it.current()->getType()==OT_PICTURE || it.current()->getType()==OT_CLIPART)
            m_doc->insertPixmapKey(static_cast<KPPixmapObject*>( it.current() )->getKey() );
        else if ( it.current()->getType() == OT_GROUP )
            makeUsedPixmapListForGroupObject( it.current() );
    }

    if( kpbackground->getBackType()==BT_PICTURE || kpbackground->getBackType()==BT_CLIPART)
        m_doc->insertPixmapKey(kpbackground->getBackPictureKey());
}

void KPrPage::makeUsedPixmapListForGroupObject( KPObject *_obj )
{
    KPGroupObject *_groupObj = static_cast<KPGroupObject*>( _obj );

    if ( _groupObj ) {
        QPtrListIterator<KPObject> it( _groupObj->objectList() );
        for ( ; it.current(); ++it ) {
            if ( ( it.current()->getType() == OT_PICTURE )
                 || ( it.current()->getType() == OT_CLIPART ) )
                m_doc->insertPixmapKey(static_cast<KPPixmapObject*>( it.current() )->getKey() );
            else if ( it.current()->getType() == OT_GROUP )
                makeUsedPixmapListForGroupObject( it.current() );  // recursion
        }
    }
}

QValueList<int> KPrPage::reorderPage() const
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

void KPrPage::deSelectAllObj()
{
    QPtrListIterator<KPObject> sIt( m_objectList );
    for ( ; sIt.current() ; ++sIt )
    {
        if(sIt.current()->isSelected())
            deSelectObj(sIt.current() );
    }

    // set current default pen color and brush color in tool bar
    if ( kPresenterDoc()->firstView())
    {
        QPen _pen = kPresenterDoc()->firstView()->getPen();
        QBrush _brush = kPresenterDoc()->firstView()->getBrush();
        kPresenterDoc()->firstView()->penColorChanged( _pen );
        kPresenterDoc()->firstView()->brushColorChanged( _brush );
    }
}

void KPrPage::deSelectObj( KPObject *kpobject )
{
    kpobject->setSelected( false );
    m_doc->repaint( kpobject );
}

QDomElement KPrPage::saveObjects( QDomDocument &doc, QDomElement &objects, double yoffset, KoZoomHandler* /*zoomHandler*/,
                                  int saveOnlyPage ) const
{
    QPtrListIterator<KPObject> oIt(m_objectList);
    for (; oIt.current(); ++oIt )
    {
        //don't store header/footer (store in header/footer section)
        if ( oIt.current()==m_doc->header()
             || oIt.current()==m_doc->footer()
             ||  oIt.current()->getType() == OT_PART)
            continue;
        QDomElement object=doc.createElement("OBJECT");
        object.setAttribute("type", static_cast<int>( oIt.current()->getType() ));
        bool _sticky = oIt.current()->isSticky();
        if (_sticky)
            object.setAttribute("sticky", static_cast<int>(_sticky));
        //QPoint orig =zoomHandler->zoomPoint(oIt.current()->getOrig());
        if ( saveOnlyPage != -1 )
            yoffset=0;
        //add yoffset to compatibility with koffice 1.1
        object.appendChild(oIt.current()->save( doc, yoffset ));

        objects.appendChild(object);
    }
    return objects;
}

bool KPrPage::oneObjectTextExist(bool forceAllTextObject)
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst,forceAllTextObject );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if ( (it.current() == m_doc->header() && !m_doc->hasHeader())
             || (it.current() == m_doc->footer() && !m_doc->hasFooter()) )
            continue;
        if (it.current()->getType()==OT_TEXT)
            return true;
    }
    return false;
}

bool KPrPage::oneObjectTextSelected()
{
    return oneObjectTextExist(false);
}


bool KPrPage::isOneObjectSelected()
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst );
    QPtrListIterator<KPObject> oIt( lst );
    return oIt.count()>0;
}

bool KPrPage::haveASelectedPartObj()
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current(); ++it ) {
        if (it.current()->getType() == OT_PART )
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedGroupObj()
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_GROUP )
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedPixmapObj()
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current() ; ++it ) {
        if (( it.current()->getType() == OT_PICTURE )
            || ( it.current()->getType() == OT_CLIPART ) )
            return true;
    }
    return false;
}

KoRect KPrPage::getBoundingRect(const KoRect &rect) const
{
    KoRect boundingRect =rect ;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected() && !it.current()->isProtect())
            boundingRect|=it.current()->getBoundingRect();

    }
    return boundingRect;
}

//return true if we change picture
bool KPrPage::chPic( KPresenterView *_view)
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() &&
             ( ( it.current()->getType() == OT_PICTURE )
               || ( it.current()->getType() == OT_CLIPART ) ) )
        {
            KPPixmapObject* obj=dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj)
            {
                _view->changePicture( obj->getFileName() );
                return true;
            }
        }
    }
    return false;
}

bool KPrPage::savePicture( KPresenterView *_view ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() &&
             ( ( it.current()->getType() == OT_PICTURE )
               || ( it.current()->getType() == OT_CLIPART ) ) )
        {
            KPPixmapObject* obj=dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj)
            {
                _view->savePicture( obj );
                return true;
            }
        }
    }
    return false;
}

// move object for releasemouseevent
KCommand *KPrPage::moveObject(KPresenterView *_view, double diffx, double diffy)
{
    bool createCommand=false;
    MoveByCmd *moveByCmd=0L;
    QPtrList<KPObject> _objects;
    _objects.setAutoDelete( false );
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current() == m_doc->header() || it.current() == m_doc->footer())
            continue;
        if ( it.current()->isSelected() && !it.current()->isProtect())
        {
            _objects.append( it.current() );
            QRect br = _view->zoomHandler()->zoomRect(it.current()->getBoundingRect() );
            br.moveBy( _view->zoomHandler()->zoomItX( diffx ), _view->zoomHandler()->zoomItY( diffy ) );
            m_doc->repaint( br ); // Previous position
            m_doc->repaint( it.current() ); // New position
            createCommand=true;
        }
    }
    if(createCommand) {
        moveByCmd = new MoveByCmd( i18n( "Move Objects" ), KoPoint( diffx, diffy ),
                                   _objects, m_doc,this );

        int pos=m_doc->pageList().findRef(this);
        m_doc->updateSideBarItem(pos, ( this == m_doc->stickyPage()) ? true: false );
    }
    return moveByCmd;
}

KCommand *KPrPage::moveObject(KPresenterView *m_view,const KoPoint &_move,bool key)
{
    QPtrList<KPObject> _objects;
    _objects.setAutoDelete( false );
    MoveByCmd *moveByCmd=0L;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't move a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if ( it.current()->isSelected() && !it.current()->isProtect()) {

            KoRect oldKoBoundingRect = it.current()->getBoundingRect();
            double _dx = oldKoBoundingRect.x() - 5.0;
            double _dy = oldKoBoundingRect.y() - 5.0;
            double _dw = oldKoBoundingRect.width() + 10.0;
            double _dh = oldKoBoundingRect.height() + 10.0;
            oldKoBoundingRect.setRect( _dx, _dy, _dw, _dh );
            QRect oldBoundingRect = m_view->zoomHandler()->zoomRect( oldKoBoundingRect );

            it.current()->moveBy( _move );
            _objects.append( it.current() );
            m_doc->repaint( oldBoundingRect );
            QRect br = m_view->zoomHandler()->zoomRect( it.current()->getBoundingRect() );
            m_doc->repaint( br );
            m_doc->repaint( it.current() );
        }
    }

    if ( key && !_objects.isEmpty())
        moveByCmd = new MoveByCmd( i18n( "Move Objects" ),
                                   KoPoint( _move ),
                                   _objects, m_doc,this );

    return moveByCmd;
}

void KPrPage::repaintObj()
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
            m_doc->repaint(it.current() );
    }
}

KCommand *KPrPage::rotateObj(float _newAngle,  bool addAngle)
{
    RotateCmd *rotateCmd=0L;
    bool newAngle=false;
    QPtrList<KPObject> _objects;
    QPtrList<RotateCmd::RotateValues> _oldRotate;
    RotateCmd::RotateValues *tmp;

    _objects.setAutoDelete( false );
    _oldRotate.setAutoDelete( false );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't rotate a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if ( it.current()->isSelected() ) {
            tmp = new RotateCmd::RotateValues;
            tmp->angle =it.current()->getAngle();

            if(!newAngle &&tmp->angle!= _newAngle)
                newAngle=true;

            _oldRotate.append( tmp );
            _objects.append( it.current() );
        }
    }

    if ( !_objects.isEmpty() && newAngle )
    {
        rotateCmd = new RotateCmd( i18n( "Change Rotation" ),
                                   _oldRotate, _newAngle, _objects, m_doc,addAngle );
        rotateCmd->execute();
    }
    else
    {
        _oldRotate.setAutoDelete( true );
        _oldRotate.clear();
    }
    return rotateCmd;
}
//necessary to use currentPage otherwise when we unsticke object
//it not take from sticky page.
KCommand *KPrPage::stickyObj(bool _sticky, KPrPage * currentPage)
{
    KPrStickyObjCommand *stickyCmd=0L;
    QPtrList<KPObject> _objects;

    _objects.setAutoDelete( false );
    //store all object which isSticky!=_sticky
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't unsticke stike a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if ( it.current()->isSelected() && it.current()->isSticky()!=_sticky)
            _objects.append( it.current() );
    }

    if ( !_objects.isEmpty() )
    {
        stickyCmd = new KPrStickyObjCommand( i18n( "Sticky Object" ),
                                             _objects,_sticky, currentPage, m_doc );
        stickyCmd->execute();
    }
    return stickyCmd;
}

KCommand *KPrPage::shadowObj(ShadowDirection dir,int dist, const QColor &col)
{
    ShadowCmd *shadowCmd=0L;
    bool newShadow=false;
    QPtrList<KPObject> _objects;
    QPtrList<ShadowCmd::ShadowValues> _oldShadow;
    ShadowCmd::ShadowValues _newShadow, *tmp;

    _objects.setAutoDelete( false );
    _oldShadow.setAutoDelete( false );

    _newShadow.shadowDirection = dir;
    _newShadow.shadowDistance = dist;
    _newShadow.shadowColor = col;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't shadow a header/footer
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if ( it.current()->isSelected() ) {
            tmp = new ShadowCmd::ShadowValues;
            tmp->shadowDirection = it.current()->getShadowDirection();
            tmp->shadowDistance =it.current()->getShadowDistance();
            tmp->shadowColor = it.current()->getShadowColor();

            if(!newShadow &&( tmp->shadowDirection!=_newShadow.shadowDirection
                              || tmp->shadowDistance!=_newShadow.shadowDistance
                              || tmp->shadowColor!=_newShadow.shadowColor))
                newShadow=true;

            _oldShadow.append( tmp );
            _objects.append( it.current() );
        }
    }

    if ( !_objects.isEmpty() && newShadow ) {
        shadowCmd = new ShadowCmd( i18n( "Change Shadow" ),
                                   _oldShadow, _newShadow, _objects, m_doc );
        shadowCmd->execute();
    }
    else
    {
        _oldShadow.setAutoDelete( true );
        _oldShadow.clear();
    }
    return shadowCmd;
}

QPtrList<KoTextObject> KPrPage::allTextObjects() const
{
    QPtrList<KoTextObject> lst;
    addTextObjects( lst );
    return lst;
}

void KPrPage::addTextObjects(QPtrList<KoTextObject>& lst) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
        it.current()->addTextObjects( lst );
}

KPObject * KPrPage::getCursor( const QPoint &pos )
{
    return getCursor( m_doc->zoomHandler()->unzoomPoint( pos ) );
}

KPObject * KPrPage::getCursor(const KoPoint &pos )
{
    QPtrListIterator<KPObject> it( m_objectList );
    KPObject *kpobject = it.toLast();
    while ( kpobject ) {
        if ( kpobject->contains( pos ) && kpobject->isSelected() )
            return kpobject;
        kpobject = --it;
    }
    return 0L;
}

KPObject * KPrPage::getObjectResized( const KoPoint &pos, ModifyType modType, bool &desel,
                                      bool &_over, bool &_resize )
{
    QPtrListIterator<KPObject> it( m_objectList );
    KPObject *kpobject = it.toLast();
    while ( kpobject ) {
        if ( !kpobject->isProtect() && kpobject->contains( pos ) ) {
            _over = true;
            if ( kpobject->isSelected() && modType == MT_MOVE )
                desel = false;
            if ( kpobject->isSelected() && modType != MT_MOVE && modType != MT_NONE )
                _resize = true;
            return kpobject;
        }
        kpobject = --it;
    }
    return 0L;
}

KPObject* KPrPage::getObjectAt( const KoPoint&pos ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    KPObject *o = it.toLast();
    while ( o ) {
        if ( o->contains( pos ) )
            return o;
        o = --it;
    }
    return 0L;
}

KPPixmapObject * KPrPage::picViewOrigHelper() const
{
    KPPixmapObject *obj=0L;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() &&
             ( ( it.current()->getType() == OT_PICTURE )
               || ( it.current()->getType() == OT_CLIPART ) ) )
        {
            obj=(KPPixmapObject*)it.current();
            break;
        }
    }
    return obj;
}

void KPrPage::applyStyleChange( StyleChangeDefMap changed )
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    QPtrListIterator<KPObject> it( lst );

    for ( ; it.current() ; ++it )
    {
        KPTextObject *obj=dynamic_cast<KPTextObject*>(it.current());
        if( obj)
            obj->applyStyleChange( changed );
    }
}

void KPrPage::reactivateBgSpellChecking(bool refreshTextObj)
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    QPtrListIterator<KPObject> oIt( lst );

    for ( ; oIt.current() ; ++oIt )
    {
        if(oIt.current()->getType()==OT_TEXT)
        {
            static_cast<KPTextObject*>( oIt.current() )->textObject()->setNeedSpellCheck(true);
            if(refreshTextObj)
                m_doc->repaint( oIt.current() );
        }
    }
}

bool KPrPage::getProtect( bool p ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected())
            return it.current()->isProtect();
    }
    return p;
}

bool KPrPage::differentProtect( bool p) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected())
            if ( p != it.current()->isProtect())
                return true;
    }
    return false;
}

bool KPrPage::differentKeepRatio( bool p) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected())
            if ( p != it.current()->isKeepRatio())
                return true;
    }
    return false;
}

bool KPrPage::getKeepRatio( bool p ) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected())
            return it.current()->isKeepRatio();
    }
    return p;
}

bool KPrPage::getProtectContent(bool p) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected() && it.current()->getType()==OT_TEXT )
            return static_cast<KPTextObject*>(it.current())->isProtectContent();
    }
    return p;
}

KoRect KPrPage::getBoundingAllObjectRect(const KoRect &rect) const
{
    KoRect boundingRect =rect ;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        boundingRect|=it.current()->getBoundingRect();
    }
    return boundingRect;
}

bool KPrPage::canMoveOneObject() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected()&& !it.current()->isProtect())
            return true;
    }
    return false;
}

KCommand *KPrPage::alignVertical( VerticalAlignmentType _type )
{
    KMacroCommand *macro = 0L;
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
        {
            KPTextObject *obj = dynamic_cast<KPTextObject *>(it.current());
            if ( obj  && !obj->isProtectContent())
            {
                KPrChangeVerticalAlignmentCommand * cmd = new KPrChangeVerticalAlignmentCommand( i18n("Change Vertical Alignment"),
                                                                                                 obj, obj->verticalAlignment(),_type, m_doc);
                if ( !macro )
                    macro = new KMacroCommand( i18n("Change Vertical Alignment"));
                macro->addCommand( cmd );
                cmd->execute();
            }
        }
    }
    return macro;
}

void KPrPage::changeTabStopValue ( double _tabStop )
{
    QPtrList<KPObject> lst;
    getAllObjectSelectedList(lst,true /*force*/ );
    QPtrListIterator<KPObject> it( lst );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType()==OT_TEXT)
        {
            KPTextObject *obj = dynamic_cast<KPTextObject *>(it.current());
            if ( obj )
            {
                obj->textDocument()->setTabStops( m_doc->zoomHandler()->ptToLayoutUnitPixX( _tabStop ));
                obj->layout();
                m_doc->repaint( obj );
            }
        }
    }
}

KPObject *KPrPage::nextTextObject(KPTextObject *obj)
{
    if ( m_objectList.count()==0 )
        return 0L;
    int pos = -1;
    if ( obj )
        pos = m_objectList.findNextRef(obj);

    if (pos != -1 )
    {
        KPObject *frm=0L;
        for ( frm=m_objectList.at(pos); frm != 0; frm=m_objectList.next() )
        {
            KPTextObject *newObj = frm->nextTextObject();
            if(newObj && newObj->nextTextObject()->textObject()->needSpellCheck())
                return frm;
        }
    }
    else
    {
        KPObject *frm=0L;
        for ( frm=m_objectList.at(0); frm != 0; frm=m_objectList.next() )
        {
            KPTextObject *newObj = frm->nextTextObject();
            if(newObj && newObj->nextTextObject()->textObject()->needSpellCheck())
                return frm;
        }
    }
    return 0L;
}

bool KPrPage::findTextObject( KPObject *obj )
{
    return (m_objectList.findRef( obj )>=0 );
}

void KPrPage::getAllObjectSelectedList(QPtrList<KPObject> &lst, bool force )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
        it.current()->getAllObjectSelectedList( lst,force );
}

void KPrPage::getAllEmbeddedObjectSelected(QPtrList<KoDocumentChild> &embeddedObjects )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PART )
            embeddedObjects.append( static_cast<KPPartObject *>(it.current())->getChild() );
    }
}

KPPixmapObject* KPrPage::getSelectedImage() const
{
    KPPixmapObject *obj=0L;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
        {
            obj=dynamic_cast<KPPixmapObject*>(it.current());
            break;
        }
    }
    return obj;
}

ImageEffect KPrPage::getImageEffect(ImageEffect eff) const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for (; it.current() ; ++it) {
        if (it.current()->isSelected() && it.current()->getType() == OT_PICTURE) {
            KPPixmapObject *obj = dynamic_cast<KPPixmapObject*>( it.current() );
            if (obj)
                return obj->getImageEffect();
        }
    }

    return eff;
}

KCommand * KPrPage::setImageEffect(ImageEffect eff, QVariant param1, QVariant param2, QVariant param3)
{
    bool changed = false;
    ImageEffectCmd *imageEffectCmd = 0L;
    QPtrList<KPObject> _objects;
    QPtrList<ImageEffectCmd::ImageEffectSettings> _oldSettings;
    ImageEffectCmd::ImageEffectSettings _newSettings, *tmp;

    _objects.setAutoDelete( false );
    _oldSettings.setAutoDelete( false );

    _newSettings.effect = eff;
    _newSettings.param1 = param1;
    _newSettings.param2 = param2;
    _newSettings.param3 = param3;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current(); ++it ) {
        if ( it.current()->getType() == OT_PICTURE && it.current()->isSelected()) {
            tmp = new ImageEffectCmd::ImageEffectSettings;
            tmp->effect = static_cast<KPPixmapObject*>(it.current())->getImageEffect();
            tmp->param1 = static_cast<KPPixmapObject*>(it.current())->getIEParam1();
            tmp->param2 = static_cast<KPPixmapObject*>(it.current())->getIEParam2();
            tmp->param3 = static_cast<KPPixmapObject*>(it.current())->getIEParam3();
            _oldSettings.append( tmp );
            _objects.append( it.current() );

            if( !changed && ( tmp->effect != _newSettings.effect
                              || tmp->param1 != _newSettings.param1
                              || tmp->param2 != _newSettings.param2
                              || tmp->param3 != _newSettings.param3 ) )
                changed = true;
        }
    }

    if ( !_objects.isEmpty() && changed ) {
        imageEffectCmd = new ImageEffectCmd( i18n( "Change Image Effect" ), _oldSettings, _newSettings,
                                             _objects, m_doc );
        imageEffectCmd->execute();
    }
    else {
        _oldSettings.setAutoDelete( true );
        _oldSettings.clear();
    }

    m_doc->setModified( true );

    return imageEffectCmd;
}
