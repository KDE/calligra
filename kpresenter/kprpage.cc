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
#include "KPresenterPageIface.h"

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
#include "koPointArray.h"
#include "kprtextdocument.h"
#include <kotextobject.h>
#include <koRect.h>
#include <qapplication.h>
#include <kostyle.h>

/******************************************************************/
/* class KPrPage - KPrPage                                        */
/******************************************************************/

KPrPage::KPrPage(KPresenterDoc *_doc )
{
    //kdDebug()<<"create page : KPrPage::KPrPage(KPresenterDoc *_doc )\n";
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
    kdDebug()<<"Delete page :KPrPage::~KPrPage() \n";
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
        deleteCmd = new DeleteCmd( i18n( "Delete object(s)" ), _objects, m_doc,this );
        deleteCmd->execute();
    }
    else
        m_doc->setModified(true);
    return deleteCmd ;
}

void KPrPage::copyObjs(QDomDocument &doc, QDomElement &presenter)
{
    if ( !numSelected() )
        return;

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            QDomElement object=doc.createElement("OBJECT");
            object.setAttribute("type", static_cast<int>( it.current()->getType() ));
            object.appendChild(it.current()->save( doc,0 ));
            presenter.appendChild(object);
        }
    }
}

void KPrPage::pasteObjs( const QByteArray & data,int nbCopy, double angle, double increaseX, double increaseY, double moveX, double moveY )
{
    m_doc->deSelectAllObj();
    int num = m_objectList.count();
    QString clip_str = QString::fromUtf8( data );
    if ( clip_str.isEmpty() ) return;
    KMacroCommand *macro = new KMacroCommand( i18n("Paste Objects" ));
    bool createMacro = false;
    int nbNewObject = -1 ;
    for ( int i = 0 ; i < nbCopy ; i++ )
    {
        KCommand *cmd = m_doc->loadPastedObjs( clip_str,this );
        if (cmd )
        {
            macro->addCommand( cmd );
            createMacro = true ;
        }
        if ( nbNewObject == -1 )
            nbNewObject = m_objectList.count() - num;
    }

    //move and select all new pasted in objects
    KPObject *_tempObj;
    int i = 0;
    int mod = 1;
    for (_tempObj = m_objectList.at(num); _tempObj; _tempObj = m_objectList.next(),  i++ ) {
        if ( i >= nbNewObject )
            mod++;
        if ( moveX != 0 || moveY != 0)
        {
            _tempObj->moveBy( moveX*(double)mod,moveY*(double)mod);
        }
        else
        {
            _tempObj->moveBy( 20.0*(double)mod,20.0*(double)mod );
        }
      _tempObj->setSelected( true );
      if ( angle == 0.0 || increaseY != 0.0 || increaseX != 0.0)
          m_doc->repaint(_tempObj);
    }

    if ( angle != 0.0)
    {
        KCommand *cmd = rotateObj(angle, true);
        if (cmd )
            macro->addCommand( cmd );
    }
    if ( increaseX != 0.0 || increaseY != 0.0 )
    {
        QPtrListIterator<KPObject> it( m_objectList );
        for ( ; it.current() ; ++it )
        {
            if(it.current()->isSelected())
            {
                KCommand *cmd =new ResizeCmd( i18n("Resize"), KoPoint(0, 0), KoSize(increaseX, increaseY), it.current(), m_doc );
                if ( cmd )
                {
                    cmd->execute();
                    macro->addCommand( cmd );
                }
            }
        }
    }

    if (createMacro)
        m_doc->addCommand(macro);
    else
        delete macro;

    m_doc->setModified(true);
}

KPTextObject * KPrPage::textFrameSet ( unsigned int _num)
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

KPObject* KPrPage::getSelectedObj()
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
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
	    objs.append( it.current() );
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
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
	    switch ( it.current()->getType() ) {
	    case OT_LINE:
            {
                KPLineObject*kpobject=dynamic_cast<KPLineObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_RECT:
            {
                KPRectObject*kpobject=dynamic_cast<KPRectObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_ELLIPSE:
            {
                KPEllipseObject*kpobject=dynamic_cast<KPEllipseObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_PIE:
            {
                KPPieObject*kpobject=dynamic_cast<KPPieObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_AUTOFORM:
            {
                KPAutoformObject*kpobject=dynamic_cast<KPAutoformObject*>( it.current() );
                if(kpobject)
                    return  kpobject->getPen();
            }
            break;
	    case OT_PART:
            {
                KPPartObject*kpobject=dynamic_cast<KPPartObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_PICTURE:
            {
                KPPixmapObject*kpobject=dynamic_cast<KPPixmapObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_CLIPART:
            {
                KPClipartObject*kpobject=dynamic_cast<KPClipartObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
	    case OT_TEXT:
            {
                KPTextObject*kpobject=dynamic_cast<KPTextObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
            case OT_FREEHAND:
            {
                KPFreehandObject*kpobject=dynamic_cast<KPFreehandObject*>( it.current() );
                if(kpobject)
                    return dynamic_cast<KPFreehandObject*>( kpobject )->getPen();
            }
            break;
            case OT_POLYLINE:
            {
                KPPolylineObject*kpobject=dynamic_cast<KPPolylineObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
            case OT_QUADRICBEZIERCURVE:
            {
                KPQuadricBezierCurveObject*kpobject=dynamic_cast<KPQuadricBezierCurveObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
            case OT_CUBICBEZIERCURVE:
            {
                KPCubicBezierCurveObject*kpobject=dynamic_cast<KPCubicBezierCurveObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
            break;
            case OT_POLYGON:
            {
                KPPolygonObject*kpobject=dynamic_cast<KPPolygonObject*>( it.current() );
                if(kpobject)
                    return kpobject->getPen();
            }
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

/*========================= get line end =========================*/
LineEnd KPrPage::getLineEnd( LineEnd le )
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

/*========================= get brush =============================*/
QBrush KPrPage::getBrush( const QBrush &brush )const
{

  QPtrListIterator<KPObject> it( m_objectList );
  for ( ; it.current() ; ++it )
    {

      if(it.current()->isSelected())
        {
	  switch ( it.current()->getType() ) {
	  case OT_RECT:
	    {
	      KPRectObject*obj=dynamic_cast<KPRectObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }
	    break;
	  case OT_ELLIPSE:
	    {
	      KPEllipseObject*obj=dynamic_cast<KPEllipseObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

	    break;
	  case OT_AUTOFORM:
	    {
	      KPAutoformObject*obj=dynamic_cast<KPAutoformObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

	    break;
	  case OT_PIE:
	    {
	      KPPieObject*obj=dynamic_cast<KPPieObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

	    break;
	  case OT_PART:
	    {
	      KPPartObject*obj=dynamic_cast<KPPartObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

	    break;
	  case OT_PICTURE:
	    {
	      KPPixmapObject*obj=dynamic_cast<KPPixmapObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

	    break;
	  case OT_CLIPART:
	    {
	      KPClipartObject*obj=dynamic_cast<KPClipartObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }
	    break;
	  case OT_TEXT:
	    {
	      KPTextObject*obj=dynamic_cast<KPTextObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

	    break;
	  case OT_POLYGON:
	    {
	      KPPolygonObject*obj=dynamic_cast<KPPolygonObject*>( it.current() );
	      if(obj)
		return obj->getBrush();
	    }

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

/*================================================================*/
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

/*================================================================*/
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

/*================================================================*/
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

/*================================================================*/
bool KPrPage::getGUnbalanced( bool  unbalanced )
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

/*================================================================*/
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

/*================================================================*/
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

/*================================================================*/
PieType KPrPage::getPieType( PieType pieType )
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

bool KPrPage::getSticky( bool s )
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

/*================================================================*/
int KPrPage::getPieLength( int pieLength )
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

/*================================================================*/
int KPrPage::getPieAngle( int pieAngle )
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

/*================================================================*/
int KPrPage::getRndX( int _rx )
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

/*================================================================*/
int KPrPage::getRndY( int _ry )
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
                return obj->getOrignalPixmap();
        }
    }

    return QPixmap();
}

/*======================== lower objects =========================*/
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
        LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Lower Object(s)" ), m_objectList, _new, m_doc,this );
        lrCmd->execute();
        m_doc->addCommand( lrCmd );
    }
    m_doc->raiseAndLowerObject = true;

}

/*========================= raise object =========================*/
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
        LowerRaiseCmd *lrCmd = new LowerRaiseCmd( i18n( "Raise Object(s)" ), m_objectList, _new, m_doc,this );
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

/*===================== insert a rectangle =======================*/
void KPrPage::insertRectangle( const KoRect &r, const QPen & pen, const QBrush &brush, FillType ft, const QColor &g1, const QColor &g2,
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
void KPrPage::insertCircleOrEllipse( const KoRect &r, const QPen &pen, const QBrush & brush, FillType ft, const QColor &g1, const QColor &g2,
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
void KPrPage::insertPie( const KoRect &r, const QPen &pen, const QBrush &brush, FillType ft, const QColor &g1, const QColor &g2,
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

/*======================= insert an autoform ====================*/
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

/*================ insert a quadric bezier curve =================*/
void KPrPage::insertQuadricBezierCurve( const KoPointArray &points, const KoPointArray &allPoints, const KoRect &r, const QPen &pen,
                                            LineEnd lb, LineEnd le )
{
    KoSize size( r.width(), r.height() );

    KPQuadricBezierCurveObject *kpQuadricBezierCurveObject = new KPQuadricBezierCurveObject( points, allPoints, size, pen, lb, le );
    kpQuadricBezierCurveObject->setOrig( r.x(), r.y() );
    kpQuadricBezierCurveObject->setSize( size );
    kpQuadricBezierCurveObject->setSelected( true );
    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Quadric Bezier Curve" ), kpQuadricBezierCurveObject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

/*================= insert a cubic bezier curve ==================*/
void KPrPage::insertCubicBezierCurve( const KoPointArray &points, const KoPointArray &allPoints, const KoRect &r, const QPen &pen,
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

/*======================= insert polygon ===========================*/
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

/*======================== align objects left ===================*/
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
            if( !newPosition &&_x != it.current()->getOrig().x())
                newPosition=true;
	    _diffs.append( new KoPoint( _x - it.current()->getOrig().x(), 0 ) );
	}
    }

    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) left" ), _diffs, _objects, m_doc );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
    return moveByCmd2;
}

/*==================== align objects center h ===================*/
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
            if(!newPosition && (( _w - it.current()->getSize().width() ) / 2 - it.current()->getOrig().x() + _x)!=0)
                newPosition=true;
	    _diffs.append( new KoPoint( ( _w - it.current()->getSize().width() ) / 2 - it.current()->getOrig().x() + _x, 0 ) );
	}
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) centered (horizontal)" ),
                                                 _diffs, _objects, m_doc );
        moveByCmd2->execute();
    }
    else
    {
         _diffs.setAutoDelete(true);
         _diffs.clear();
    }
    return moveByCmd2;
}

/*==================== align objects right ======================*/
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
            if(!newPosition && (( _w - it.current()->getSize().width() ) != it.current()->getOrig().x()))
                newPosition=true;
	    _diffs.append( new KoPoint( ( _w - it.current()->getSize().width() ) - it.current()->getOrig().x(), 0 ) );
	}
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) right" ), _diffs, _objects, m_doc);
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
    return moveByCmd2;
}

/*==================== align objects top ========================*/
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
            if(!newPosition && (_y != it.current()->getOrig().y()))
                newPosition=true;

            _diffs.append( new KoPoint( 0, _y - it.current()->getOrig().y() ) );
	}
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) top" ), _diffs, _objects, m_doc );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
    return moveByCmd2;
}

/*==================== align objects center v ===================*/
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
            if(!newPosition &&(( _h - it.current()->getSize().height() ) / 2 - it.current()->getOrig().y() + _y )!=0)
                newPosition=true;
            _diffs.append( new KoPoint( 0, ( _h - it.current()->getSize().height() ) / 2 -
                                       it.current()->getOrig().y() + _y ) );
	}
    }
    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) center / vertical" ), _diffs, _objects, m_doc );
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
            if(!newPosition && _h != (it.current()->getSize().height() + it.current()->getOrig().y()))
                newPosition=true;
            _diffs.append( new KoPoint( 0, _h - it.current()->getSize().height() - it.current()->getOrig().y() ) );
	}
    }

    if(newPosition)
    {
        moveByCmd2 = new MoveByCmd2( i18n( "Align object(s) bottom" ), _diffs, _objects, m_doc );
        moveByCmd2->execute();
    }
    else
    {
	_diffs.setAutoDelete( true );
	_diffs.clear();
    }
    return moveByCmd2;
}



void KPrPage::insertClipart( const QString &filename )
{
    KoPictureKey key = m_doc->getClipartCollection()->loadPicture( filename ).getKey();
    kdDebug(33001) << "KPresenterDoc::insertClipart key=" << key.toString() << endl;

    KPClipartObject *kpclipartobject = new KPClipartObject(m_doc->getClipartCollection() , key );
    double x=(m_doc->zoomHandler()->unzoomItX(10)/m_doc->getGridX())*m_doc->getGridX();
    double y=(m_doc->zoomHandler()->unzoomItY(10)/m_doc->getGridY())*m_doc->getGridY();
    kpclipartobject->setOrig( x, y);
    kpclipartobject->setSize( m_doc->zoomHandler()->unzoomItX(150), m_doc->zoomHandler()->unzoomItY(150) );
    kpclipartobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, m_doc,this );
    insertCmd->execute();
    m_doc->addCommand( insertCmd );
}

KPPartObject* KPrPage::insertObject( const KoRect& _rect, KoDocumentEntry& _e )
{
    KoDocument* doc = _e.createDoc( m_doc );
    if ( !doc || !doc->initDoc() ) {
	return NULL;
    }

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
                KPLineObject * obj=dynamic_cast<KPLineObject *>(kpobject);
                if( obj)
		{
                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                    ret = true;
		}
	    } break;
	    case OT_AUTOFORM: {
                KPAutoformObject *obj=dynamic_cast<KPAutoformObject *>(kpobject);
                if(obj)
		{

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                    ret = true;
		}
	    } break;
            case OT_FREEHAND: {
                KPFreehandObject *obj=dynamic_cast<KPFreehandObject *>(kpobject);
                if(obj)
		{

                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                    ret = true;
		}
	    } break;
            case OT_POLYLINE: {
                KPPolylineObject *obj=dynamic_cast<KPPolylineObject *>(kpobject);
                if(obj)
		{
                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                    ret = true;
		}
	    } break;
            case OT_QUADRICBEZIERCURVE: {
                KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject *>(kpobject);
                if(obj)
		{
                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                    ret = true;
		}
	    } break;
            case OT_CUBICBEZIERCURVE: {
                KPCubicBezierCurveObject*obj=dynamic_cast<KPCubicBezierCurveObject *>(kpobject);
                if(obj)
		{

                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                    ret = true;
		}
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
                KPLineObject* obj=dynamic_cast<KPLineObject*>( kpobject );
                if(obj)
		{
		  ptmp->pen = QPen(obj->getPen() );
		  ptmp->lineBegin = obj->getLineBegin();
		  ptmp->lineEnd = obj->getLineEnd();
		  ret = true;
		}
	    } break;
	    case OT_AUTOFORM: {
                KPAutoformObject* obj=dynamic_cast<KPAutoformObject*>( kpobject );
                if(obj)
		{
		  ptmp->pen = QPen( obj->getPen() );
		  btmp->brush = QBrush( obj->getBrush() );
		  ptmp->lineBegin = obj->getLineBegin();
		  ptmp->lineEnd = obj->getLineEnd();
		  btmp->fillType = obj->getFillType();
		  btmp->gColor1 = obj->getGColor1();
		  btmp->gColor2 = obj->getGColor2();
		  btmp->gType = obj->getGType();
		  ret = true;
		}
	    } break;
            case OT_FREEHAND: {
                KPFreehandObject* obj=dynamic_cast<KPFreehandObject*>( kpobject );
                if(obj)
		{
		  ptmp->pen = QPen( obj->getPen() );
		  ptmp->lineBegin = obj->getLineBegin();
		  ptmp->lineEnd = obj->getLineEnd();
		  ret = true;
		}
	    } break;
            case OT_POLYLINE: {
                KPPolylineObject* obj=dynamic_cast<KPPolylineObject*>( kpobject );
                if(obj)
		{
		  ptmp->pen = QPen( obj->getPen() );
		  ptmp->lineBegin = obj->getLineBegin();
		  ptmp->lineEnd = obj->getLineEnd();
		  ret = true;
		}
	    } break;
            case OT_QUADRICBEZIERCURVE: {
                KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject*>( kpobject );
                if(obj)
		{
		  ptmp->pen = QPen( obj->getPen() );
		  ptmp->lineBegin = obj->getLineBegin();
		  ptmp->lineEnd = obj->getLineEnd();
		  ret = true;
		}
	    } break;
            case OT_CUBICBEZIERCURVE: {
                KPCubicBezierCurveObject *obj=dynamic_cast<KPCubicBezierCurveObject*>( kpobject );
                if(obj)
		{
		  ptmp->pen = QPen( obj->getPen() );
		  ptmp->lineBegin = obj->getLineBegin();
		  ptmp->lineEnd = obj->getLineEnd();
		  ret = true;
		}
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
KCommand * KPrPage::setPenBrush( const QPen &pen, const QBrush &brush, LineEnd lb, LineEnd le, FillType ft, const QColor &g1, const QColor &g2,
                                    BCType gt, bool unbalanced, int xfactor, int yfactor,QPtrList<KPObject>list )
{
    KPObject *kpobject = 0;
    PenBrushCmd *penBrushCmd=0L;
    bool cmdCreate=false;
    KMacroCommand *cmd=new KMacroCommand(i18n("Apply Style"));

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

    QPtrListIterator<KPObject> it( list );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            kpobject=it.current();
            ptmp = new PenBrushCmd::Pen;
            btmp = new PenBrushCmd::Brush;
            switch ( kpobject->getType() ) {
            case OT_LINE:
            {
                KPLineObject *obj=dynamic_cast<KPLineObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
	    break;
            case OT_RECT:
            {
                KPRectObject *obj=dynamic_cast<KPRectObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
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
                    ptmp->pen = QPen( obj->getPen() );
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
                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
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
                    ptmp->pen = QPen(obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
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
                    ptmp->pen = QPen( obj->getPen() );
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
                    ptmp->pen = QPen( obj->getPen() );
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
            case OT_PICTURE:
            {
                KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
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
            {
                KPClipartObject *obj=dynamic_cast<KPClipartObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
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
            case OT_FREEHAND:
            {
                KPFreehandObject *obj=dynamic_cast<KPFreehandObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
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
                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
            }
            break;
            case OT_POLYGON:
            {
                KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
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

            case OT_GROUP:
            {
                KPGroupObject *obj=dynamic_cast<KPGroupObject*>( kpobject );
                if(obj)
                {
                    obj->selectAllObj();
                    KCommand *cmd2=setPenBrush(pen, brush, lb, le, ft, g1, g2,
                                               gt,unbalanced, xfactor,yfactor, obj->objectList() );
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
            _oldBrush.append( btmp );
            _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
        penBrushCmd = new PenBrushCmd( i18n( "Apply Styles" ), _oldPen, _oldBrush,
                                                    _newPen, _newBrush, _objects, m_doc );
        penBrushCmd->execute();
        cmd->addCommand(penBrushCmd);
        cmdCreate=true;
    } else {
        _oldPen.setAutoDelete( true );
        _oldPen.clear();
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

int KPrPage::getPenBrushFlags( QPtrList<KPObject>list )
{
    int flags = 0;

    QPtrListIterator<KPObject> it( list);
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected())
        {
            switch ( it.current()->getType() ) {
                case OT_LINE: case OT_FREEHAND: case OT_POLYLINE:
                case OT_QUADRICBEZIERCURVE: case OT_CUBICBEZIERCURVE:
                    flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                    flags = flags | StyleDia::SdEndBeginLine;
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
                case OT_TEXT: case OT_CLIPART:
                    flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                    break;
                case OT_PICTURE:
                    flags = flags | StyleDia::SdPen | StyleDia::SdPicture;
                    break;
                case OT_AUTOFORM:
                {
                    flags = flags | StyleDia::SdPen | StyleDia::SdOther;
                    flags = flags | StyleDia::SdBrush | StyleDia::SdGradient;
                    flags = flags | StyleDia::SdEndBeginLine;
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


KCommand* KPrPage::setPieSettings( PieType pieType, int angle, int len )
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
		tmp = new PieValueCmd::PieValues;
                KPPieObject *obj= dynamic_cast<KPPieObject*>( it.current() );
                if(obj)
                {
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
						    _oldValues, _newValues, _objects, m_doc );
	pieValueCmd->execute();
    } else {
	_oldValues.setAutoDelete( true );
	_oldValues.clear();
    }

    m_doc->setModified(true);
    return pieValueCmd;
}

KCommand* KPrPage::setRectSettings( int _rx, int _ry )
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
		tmp = new RectValueCmd::RectValues;
                KPRectObject *obj=dynamic_cast<KPRectObject*>( it.current() );
                if(obj)
                {

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
	rectValueCmd = new RectValueCmd( i18n( "Change Rectangle values" ), _oldValues,
						       _newValues, _objects, m_doc );
	rectValueCmd->execute();
    } else {
	_oldValues.setAutoDelete( true );
	_oldValues.clear();
    }

    m_doc->setModified(true);
    return rectValueCmd;
}

KCommand* KPrPage::setPolygonSettings( bool _checkConcavePolygon, int _cornersValue, int _sharpnessValue )
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
                tmp->checkConcavePolygon = dynamic_cast<KPPolygonObject*>(it.current())->getCheckConcavePolygon();
                tmp->cornersValue = dynamic_cast<KPPolygonObject*>(it.current())->getCornersValue();
                tmp->sharpnessValue = dynamic_cast<KPPolygonObject*>(it.current())->getSharpnessValue();
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
                                                   _newSettings, _objects, m_doc );
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
                tmp->mirrorType = dynamic_cast<KPPixmapObject*>(it.current())->getPictureMirrorType();
                tmp->depth = dynamic_cast<KPPixmapObject*>(it.current())->getPictureDepth();
                tmp->swapRGB = dynamic_cast<KPPixmapObject*>(it.current())->getPictureSwapRGB();
                tmp->grayscal = dynamic_cast<KPPixmapObject*>(it.current())->getPictureGrayscal();
                tmp->bright = dynamic_cast<KPPixmapObject*>(it.current())->getPictureBright();
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
        pictureSettingCmd = new PictureSettingCmd( i18n( "Change Picture Settings" ), _oldSettings, _newSettings, _objects, m_doc );
        pictureSettingCmd->execute();
    }
    else {
        _oldSettings.setAutoDelete( true );
        _oldSettings.clear();
    }

    m_doc->setModified( true );

    return pictureSettingCmd;
}

KCommand* KPrPage::setPenColor( const QColor &c, bool fill )
{
    KPObject *kpobject = 0;
    PenBrushCmd *penBrushCmd=0L;
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
                KPLineObject *obj=dynamic_cast<KPLineObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
	    } break;
	    case OT_RECT: {
                KPRectObject *obj=dynamic_cast<KPRectObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
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

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
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

                    ptmp->pen = QPen( obj->getPen() );
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
                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
	    } break;
	    case OT_PICTURE: {
                KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
	    } break;
	    case OT_CLIPART: {
                KPClipartObject *obj=dynamic_cast<KPClipartObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
	    } break;
            case OT_FREEHAND: {
                KPFreehandObject *obj=dynamic_cast<KPFreehandObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
	    } break;
            case OT_POLYLINE: {
                KPPolylineObject *obj=dynamic_cast<KPPolylineObject *>(kpobject);
                if(obj)
		{

                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
	    } break;
            case OT_QUADRICBEZIERCURVE: {
                KPQuadricBezierCurveObject *obj=dynamic_cast<KPQuadricBezierCurveObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
	    } break;
            case OT_CUBICBEZIERCURVE: {
                KPCubicBezierCurveObject *obj=dynamic_cast<KPCubicBezierCurveObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
                }
	    } break;
            case OT_POLYGON: {
                KPPolygonObject *obj=dynamic_cast<KPPolygonObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = obj->getBrush();
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
	    default: break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	penBrushCmd = new PenBrushCmd( i18n( "Change Pen" ), _oldPen, _oldBrush, _newPen,
						    _newBrush, _objects, m_doc, PenBrushCmd::PEN_ONLY );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    m_doc->setModified(true);
    return penBrushCmd;
}

KCommand* KPrPage::setBrushColor( const QColor &c, bool fill )
{
    KPObject *kpobject = 0;
    PenBrushCmd *penBrushCmd=0L;
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
                KPRectObject *obj=dynamic_cast<KPRectObject*>( kpobject );
                if(obj)
                {
                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
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

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    ptmp->lineBegin = obj->getLineBegin();
                    ptmp->lineEnd = obj->getLineEnd();
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

                    ptmp->pen = QPen( obj->getPen() );
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
                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
	    } break;
	    case OT_PICTURE: {
                KPPixmapObject *obj=dynamic_cast<KPPixmapObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
	    } break;
	    case OT_CLIPART: {
                KPClipartObject *obj=dynamic_cast<KPClipartObject*>( kpobject );
                if(obj)
                {

                    ptmp->pen = QPen( obj->getPen() );
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

                    ptmp->pen = QPen( obj->getPen() );
                    btmp->brush = QBrush( obj->getBrush() );
                    btmp->fillType = obj->getFillType();
                    btmp->gColor1 = obj->getGColor1();
                    btmp->gColor2 = obj->getGColor2();
                    btmp->gType = obj->getGType();
                }
            } break;
	    default: continue; break;
	    }
	    _oldPen.append( ptmp );
	    _oldBrush.append( btmp );
	    _objects.append( kpobject );
	}
    }

    if ( !_objects.isEmpty() ) {
	penBrushCmd = new PenBrushCmd( i18n( "Change Brush" ), _oldPen, _oldBrush, _newPen,
						    _newBrush, _objects, m_doc, PenBrushCmd::BRUSH_ONLY );
	penBrushCmd->execute();
    } else {
	_oldPen.setAutoDelete( true );
	_oldPen.clear();
	_oldBrush.setAutoDelete( true );
	_oldBrush.clear();
    }

    m_doc->setModified(true);
    return penBrushCmd;
}

void KPrPage::slotRepaintVariable()
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
	if ( it.current()->getType() == OT_TEXT )
            m_doc->repaint( it.current() );
    }
}

void KPrPage::recalcPageNum()
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
	if ( it.current()->getType() == OT_TEXT )
	    ( (KPTextObject*)it.current() )->recalcPageNum( m_doc, this );
    }
}

void KPrPage::changePicture( const QString & filename )
{
    // filename has been chosen in KPresenterView with a filedialog,
    // so we know it exists
    KoPicture image = m_doc->getImageCollection()->loadPicture( filename );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType()==OT_PICTURE)
        {
    	    KPPixmapObject *pix = new KPPixmapObject( m_doc->getImageCollection(), image.getKey() );
            KPPixmapObject* obj=dynamic_cast<KPPixmapObject*>( it.current() );
            if( obj)
            {
                ChgPixCmd *chgPixCmd = new ChgPixCmd( i18n( "Change pixmap" ), dynamic_cast<KPPixmapObject*>( it.current() ),pix, m_doc,this );
                chgPixCmd->execute();
                m_doc->addCommand( chgPixCmd );
            }
            else
                delete pix;
	    break;
	}
    }
}

void KPrPage::changeClipart( const QString & filename )
{
    // filename has been chosen in KPresenterView with a filedialog,
    // so we know it exists
    KoPicture clipart = m_doc->getClipartCollection()->loadPicture( filename );

    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected()&& it.current()->getType()==OT_CLIPART)
        {
            KPClipartObject* obj=dynamic_cast<KPClipartObject*>( it.current() );
            if (obj )
            {
                ChgClipCmd *chgClipCmd = new ChgClipCmd( i18n( "Change clipart" ),
                                                         dynamic_cast<KPClipartObject*>( it.current() ),
                                                         dynamic_cast<KPClipartObject*>( it.current() )->getKey(),
                                                         clipart.getKey(), m_doc);
                chgClipCmd->execute();
                m_doc->addCommand( chgClipCmd );
            }
	    break;
	}
    }
}


void KPrPage::insertPicture( const QString &filename, int _x , int _y )
{
    KoPictureKey key = m_doc->getImageCollection()->loadPicture( filename ).getKey();
    KPPixmapObject *kppixmapobject = new KPPixmapObject(m_doc->getImageCollection() , key );
    double x=m_doc->zoomHandler()->unzoomItX(_x);
    double y=m_doc->zoomHandler()->unzoomItY(_y);

    kppixmapobject->setOrig( (   x  / m_doc->getGridX() ) * m_doc->getGridX(), ( y  / m_doc->getGridY() ) * m_doc->getGridY());
    kppixmapobject->setSelected( true );

    kppixmapobject->setSize( m_doc->zoomHandler()->unzoomItX( 10 ),m_doc->zoomHandler()->unzoomItY( 10 ) );

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
}

void KPrPage::insertPicture( const QString &_file, const KoRect &_rect )
{
    KoPictureKey key = m_doc->getImageCollection()->loadPicture( _file ).getKey();
    KPPixmapObject *kppixmapobject = new KPPixmapObject( m_doc->getImageCollection() , key );

    kppixmapobject->setOrig( _rect.x(), _rect.y() );
    kppixmapobject->setSize( _rect.width(), _rect.height() );
    kppixmapobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Picture" ), kppixmapobject, m_doc, this );
    insertCmd->execute();

    m_doc->addCommand( insertCmd );
}

void KPrPage::insertClipart( const QString &_file, const KoRect &_rect )
{
    KoPictureKey key = m_doc->getClipartCollection()->loadPicture( _file ).getKey();
    kdDebug(33001) << "KPresenterDoc::insertClipart key=" << key.toString() << endl;

    KPClipartObject *kpclipartobject = new KPClipartObject( m_doc->getClipartCollection() , key );

    kpclipartobject->setOrig( _rect.x(), _rect.y() );
    kpclipartobject->setSize( _rect.width(), _rect.height() );
    kpclipartobject->setSelected( true );

    InsertCmd *insertCmd = new InsertCmd( i18n( "Insert Clipart" ), kpclipartobject, m_doc, this );
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

void KPrPage::setBackPixmap( const KoPictureKey & key )
{
    kpbackground->setBackPixmap( key.filename(), key.lastModified() );
}

bool KPrPage::getBackUnbalanced(  )const
{
    return kpbackground->getBackUnbalanced();
}

void KPrPage::setBackClipart( const KoPictureKey & key )
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


BackType KPrPage::getBackType(  )const
{
    return kpbackground->getBackType();
}

BackView KPrPage::getBackView( )const
{
    return kpbackground->getBackView();
}

KoPictureKey KPrPage::getBackPixKey( )const
{
    return kpbackground->getBackPixKey();
}

KoPictureKey KPrPage::getBackClipKey(  )const
{
    return kpbackground->getBackClipKey();
}

QColor KPrPage::getBackColor1( )const
{
    return kpbackground->getBackColor1();
}

QColor KPrPage::getBackColor2(  )const
{
    return kpbackground->getBackColor2();
}

int KPrPage::getBackXFactor(  )const
{
    return kpbackground->getBackXFactor();
}

int KPrPage::getBackYFactor(  )const
{
    return kpbackground->getBackYFactor();
}

BCType KPrPage::getBackColorType( )const
{
    return kpbackground->getBackColorType();
}

PageEffect KPrPage::getPageEffect( )const
{
    return kpbackground->getPageEffect();
}

int KPrPage::getPageTimer(  )const
{
    return kpbackground->getPageTimer();
}

bool KPrPage::getPageSoundEffect( )const
{
    return kpbackground->getPageSoundEffect();
}

QString KPrPage::getPageSoundFileName(  )const
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
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        // Pictures and cliparts have been loaded from the store, we can now
        // get the pixmap/picture from the collection, and set it in the image/clipart object
        if ( it.current()->getType() == OT_PICTURE ) {
            if ( _clean || m_objectList.findRef( it.current() ) > lastObj )
            {
                KPPixmapObject* obj=dynamic_cast<KPPixmapObject*>( it.current());
                if(obj)
                    obj->reload();
            }
        }
        else if ( it.current()->getType() == OT_CLIPART )
        {
            KPClipartObject*obj=dynamic_cast<KPClipartObject*>( it.current() );
            if(obj)
                obj->reload();
        }
        else
        {
            if ( it.current()->getType() == OT_TEXT )
            {
                KPTextObject*obj=dynamic_cast<KPTextObject*>( it.current() );
                if(obj)
                    obj->recalcPageNum( m_doc,this );
            }
        }
    }
    kpbackground->reload();
}


/*====================== replace objects =========================*/
KCommand * KPrPage::replaceObjs( bool createUndoRedo, double _orastX,double _orastY,const QColor & _txtBackCol, const QColor & _otxtBackCol )
{
    KPObject *kpobject = 0;
    double ox, oy;
    QPtrList<KPObject> _objects;
    QValueList<KoPoint> _diffs;
    _objects.setAutoDelete( false );

    for ( int i = 0; i < static_cast<int>( m_objectList.count() ); i++ ) {
	kpobject = m_objectList.at( i );
	ox = kpobject->getOrig().x();
	oy = kpobject->getOrig().y();
	ox = static_cast<int>(( ox / m_doc->getGridX() )) * m_doc->getGridX();
	oy = static_cast<int>(( oy / m_doc->getGridY() )) * m_doc->getGridY();

	_diffs.append( KoPoint( ox - kpobject->getOrig().x(), oy - kpobject->getOrig().y() ) );
	_objects.append( kpobject );
    }

    SetOptionsCmd *setOptionsCmd = new SetOptionsCmd( i18n( "Set new options" ), _diffs, _objects, m_doc->getGridX(), m_doc->getGridY(),_orastX, _orastY, _txtBackCol, _otxtBackCol, m_doc );
    if ( createUndoRedo )
        return setOptionsCmd;

    delete setOptionsCmd;
    return 0L;
}


QString KPrPage::manualTitle()const
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
       {
           if( it.current()->getType()==OT_PICTURE)
               m_doc->insertPixmapKey(dynamic_cast<KPPixmapObject*>( it.current() )->getKey() );
           else
               m_doc->insertClipartKey(dynamic_cast<KPClipartObject*>( it.current())->getKey());
       }
   }

   if( kpbackground->getBackType()==BT_PICTURE)
       m_doc->insertPixmapKey(kpbackground->getBackPixKey());
   else if( kpbackground->getBackType()==BT_CLIPART)
       m_doc->insertClipartKey(kpbackground->getBackClipKey());
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

void KPrPage::deSelectAllObj()
{
    QPtrListIterator<KPObject> sIt( m_objectList );
    for ( ; sIt.current() ; ++sIt )
    {
        if(sIt.current()->isSelected())
            deSelectObj(sIt.current() );
    }

    // set current default pen color and brush color in tool bar
    QPen _pen = kPresenterDoc()->getKPresenterView()->getPen();
    QBrush _brush = kPresenterDoc()->getKPresenterView()->getBrush();
    kPresenterDoc()->getKPresenterView()->penColorChanged( _pen );
    kPresenterDoc()->getKPresenterView()->brushColorChanged( _brush );
}

void KPrPage::deSelectObj( KPObject *kpobject )
{
    kpobject->setSelected( false );
    m_doc->repaint( kpobject );
}

QDomElement KPrPage::saveObjects( QDomDocument &doc, QDomElement &objects, double yoffset, KoZoomHandler* /*zoomHandler*/, int saveOnlyPage )
{
    QPtrListIterator<KPObject> oIt(m_objectList);
    for (; oIt.current(); ++oIt )
    {
        //don't store header/footer (store in header/footer section)
        if ( oIt.current()==m_doc->header() || oIt.current()==m_doc->footer())
            continue;
        if ( oIt.current()->getType() == OT_PART )
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

bool KPrPage::oneObjectTextExist() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( (it.current()== m_doc->header() && !m_doc->hasHeader())|| (it.current()== m_doc->footer() && !m_doc->hasFooter()))
            continue;
        if (  it.current()->getType()==OT_TEXT)
            return true;
    }
    return false;
}

bool KPrPage::isOneObjectSelected() const
{
    QPtrListIterator<KPObject> oIt( m_objectList );
    for (; oIt.current(); ++oIt )
        if ( oIt.current()->isSelected() )
            return true;

    return false;
}

bool KPrPage::haveASelectedClipartObj() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->isSelected() && it.current()->getType() == OT_CLIPART)
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedPartObj() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PART )
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedGroupObj() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current(); ++it ) {
        if ( it.current()->isSelected() && it.current()->getType() == OT_GROUP )
            return true;
    }
    return false;
}

bool KPrPage::haveASelectedPixmapObj() const
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it ) {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
            return true;
    }
    return false;
}

KoRect KPrPage::getBoundingRect(const KoRect &rect, KPresenterDoc *doc)
{
    KoRect boundingRect =rect ;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;

        if(it.current()->isSelected() && !it.current()->isProtect())
        {
            boundingRect|=it.current()->getBoundingRect(doc->zoomHandler());
        }
    }
    return boundingRect;
}

//return true if we change picture
bool KPrPage::chPic( KPresenterView *_view)
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && it.current()->getType() == OT_PICTURE )
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

bool KPrPage::chClip(KPresenterView *_view)
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && it.current()->getType() == OT_CLIPART )
        {
	  KPClipartObject* obj=dynamic_cast<KPClipartObject*>( it.current() );
	  if(obj)
	    {
	      _view->changeClipart( obj->getFileName() );
	      return true;
	    }
        }
    }
    return false;
}
// move object for releasemouseevent
KCommand *KPrPage::moveObject(KPresenterView *_view,int diffx,int diffy)
{
    bool createCommand=false;
    MoveByCmd *moveByCmd=0L;
    QPtrList<KPObject> _objects;
    _objects.setAutoDelete( false );
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()->isSelected() && !it.current()->isProtect())
        {
            _objects.append( it.current() );
            QRect br = _view->zoomHandler()->zoomRect(it.current()->getBoundingRect(_view->zoomHandler()) );
            br.moveBy( diffx, diffy );
            m_doc->repaint( br ); // Previous position
            m_doc->repaint( it.current() ); // New position
            createCommand=true;
        }
    }
    if(createCommand)
        moveByCmd = new MoveByCmd( i18n( "Move object(s)" ),
                                   KoPoint( _view->zoomHandler()->unzoomItX (diffx),_view->zoomHandler()->unzoomItY( diffy) ),
                                   _objects, m_doc,this );
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

            KoRect oldKoBoundingRect = it.current()->getBoundingRect(m_view->zoomHandler());
            double _dx = oldKoBoundingRect.x() - 5.0;
            double _dy = oldKoBoundingRect.y() - 5.0;
            double _dw = oldKoBoundingRect.width() + 10.0;
            double _dh = oldKoBoundingRect.height() + 10.0;
            oldKoBoundingRect.setRect( _dx, _dy, _dw, _dh );
            QRect oldBoundingRect = m_view->zoomHandler()->zoomRect( oldKoBoundingRect );

            it.current()->moveBy( _move );
            _objects.append( it.current() );
            m_doc->repaint( oldBoundingRect );
            QRect br = m_view->zoomHandler()->zoomRect( it.current()->getBoundingRect(m_view->zoomHandler()) );
            m_doc->repaint( br );
            m_doc->repaint( it.current() );
        }
    }

    if ( key && !_objects.isEmpty()) {
        moveByCmd = new MoveByCmd( i18n( "Move object(s)" ),
                                   KoPoint( _move ),
                                   _objects, m_doc,this );
    }
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

        if ( it.current()->isSelected() && it.current()->isSticky()!=_sticky) {
	    _objects.append( it.current() );
	}
    }

    if ( !_objects.isEmpty() )
    {
        stickyCmd = new KPrStickyObjCommand( i18n( "Sticky object" ),
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


QPtrList<KoTextObject> KPrPage::objectText()
{
    QPtrList<KoTextObject>lst;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->getType() == OT_TEXT)
	  {
	    KPTextObject* obj=dynamic_cast<KPTextObject*>(it.current());
	    if(obj && !obj->isProtectContent())
	      {
		lst.append(obj->textObject());
	      }
	  }
    }
    return lst;
}


KPObject * KPrPage::getCursor(const QPoint &pos )
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if(it.current()->contains(m_doc->zoomHandler()->zoomPoint(pos),m_doc->zoomHandler())) {
            if(it.current()->isSelected())
                return it.current();
            break;
        }
    }
    return 0L;
}

KPObject * KPrPage::getCursor(const KoPoint &pos )
{
    KPObject *kpobject=0L;
    for ( int i = m_objectList.count() - 1; i >= 0; i-- ) {
        kpobject = m_objectList.at( i );
            if ( kpobject->contains( pos,m_doc->zoomHandler() )) {
                if ( kpobject->isSelected() ) {
                    return kpobject;
                }
            }
    }
    return 0L;
}

KPObject * KPrPage::getObjectResized( const KoPoint &pos, ModifyType modType, bool &desel, bool &_over, bool &_resize )
{
    KPObject *kpobject=0L;
    if ( (int)m_objectList.count() - 1 >= 0 ) {
        for ( int i = m_objectList.count() - 1; i >= 0 ; i-- ) {
            kpobject = m_objectList.at( i );
            if ( !kpobject->isProtect() && kpobject->contains( pos,m_doc->zoomHandler() ) ) {
                _over = true;
                if ( kpobject->isSelected() && modType == MT_MOVE )
                    desel = false;
                if ( kpobject->isSelected() && modType != MT_MOVE && modType != MT_NONE )
                    _resize = true;
                return kpobject;
            }
        }
    }
    return 0L;
}

KPObject * KPrPage::getEditObj(const KoPoint & pos)
{
    KPObject *kpobject=0L;
    if ( (int)m_objectList.count() - 1 >= 0 ) {
        for ( int i = m_objectList.count()  - 1; i >= 0; i-- ) {
            kpobject = m_objectList.at( i );
            if ( kpobject->contains( pos,m_doc->zoomHandler() ) ) {
                return kpobject;
            }
        }
    }
    return 0L;
}


KPObject* KPrPage::getObjectAt( const KoPoint&pos )
{
  KPObject *obj=0L;
  for ( int i = m_objectList.count() - 1; i >= 0 ; i-- ) {
    obj = m_objectList.at( i );
    if ( obj->contains( pos,m_doc->zoomHandler() ) )
      return obj;
  }

  return 0L;
}

KPPixmapObject * KPrPage::picViewOrigHelper( )
{
  KPPixmapObject *obj=0L;
  QPtrListIterator<KPObject> it( m_objectList );
  for ( ; it.current() ; ++it )
  {
      if ( it.current()->isSelected()&& it.current()->getType()==OT_PICTURE )
      {
          obj=(KPPixmapObject*)it.current();
          break;
      }
  }
  return obj;
}

void KPrPage::applyStyleChange( KoStyle * changedStyle, int paragLayoutChanged, int formatChanged )
{
  QPtrListIterator<KPObject> it( m_objectList );
  for ( ; it.current() ; ++it )
  {
      KPTextObject *obj=dynamic_cast<KPTextObject*>(it.current());
      if( obj)
          obj->applyStyleChange( changedStyle, paragLayoutChanged, formatChanged );
  }
}

void KPrPage::reactivateBgSpellChecking(bool refreshTextObj)
{
    QPtrListIterator<KPObject> oIt(m_objectList )  ;
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

bool KPrPage::getProtect( bool p )
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

bool KPrPage::differentProtect( bool p)
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected())
        {
            if ( p != it.current()->isProtect())
                return true;
        }
    }
    return false;
}


bool KPrPage::differentKeepRatio( bool p)
{
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        //don't test header/footer all the time sticky
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        if(it.current()->isSelected())
        {
            if ( p != it.current()->isKeepRatio())
                return true;
        }
    }
    return false;
}

bool KPrPage::getKeepRatio( bool p )
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

KoRect KPrPage::getBoundingAllObjectRect(const KoRect &rect, KPresenterDoc *doc)
{
    KoRect boundingRect =rect ;
    QPtrListIterator<KPObject> it( m_objectList );
    for ( ; it.current() ; ++it )
    {
        if ( it.current()== m_doc->header() || it.current()== m_doc->footer())
            continue;
        boundingRect|=it.current()->getBoundingRect(doc->zoomHandler());
    }
    return boundingRect;
}

bool KPrPage::canMoveOneObject()
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
