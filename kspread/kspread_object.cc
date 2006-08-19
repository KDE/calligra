/* This file is part of the KDE project
   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "kspread_object.h"
#include "kspread_canvas.h"
#include "kspread_doc.h"
#include "kspread_sheet.h"
#include "kspread_view.h"

#include <assert.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kimageeffect.h>
#include <kparts/partmanager.h>
#include <koChart.h>

#include <qbitmap.h>
#include <qbuffer.h>
#include <qcursor.h>
#include <qdom.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwmatrix.h>

#include <KoDocument.h>
#include <KoDocumentChild.h>
#include <KoDom.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

using namespace KSpread;

class Sheet;
class View;

/**********************************************************
 *
 * EmbeddedObject
 *
 **********************************************************/
EmbeddedObject::EmbeddedObject( Sheet *_sheet, const KoRect& _geometry )
  : m_geometry( _geometry), m_sheet(_sheet), m_objectName(""), m_selected(false), m_protect(false), m_keepRatio(false), pen( Qt::black, 1, QPen::SolidLine )
{
  angle = 0.0;
  inObjList = true;
  cmds = 0;
}

EmbeddedObject::~EmbeddedObject()
{
}
KoRect EmbeddedObject::geometry()
{
    return m_geometry;
}
void EmbeddedObject::setGeometry( const KoRect &rect )
{
    m_geometry = rect;
}

void EmbeddedObject::moveBy( const KoPoint &_point )
{
  m_geometry.moveTopLeft( m_geometry.topLeft() + _point );
}

void EmbeddedObject::moveBy( double _dx, double _dy )
{
  m_geometry.moveTopLeft( m_geometry.topLeft() + KoPoint( _dx, _dy ) );
}

void EmbeddedObject::resizeBy( const KoSize & _size )
{
  resizeBy( _size.width(), _size.height() );
}

void EmbeddedObject::resizeBy( double _dx, double _dy)
{
  m_geometry.setSize( KoSize( m_geometry.width()+_dx, m_geometry.height()+_dy) );
} // call (possibly reimplemented) setSize

bool EmbeddedObject::load( const QDomElement& /*element*/ )
{
    kdDebug() << "Loading EmbeddedObject" << endl;
    return false;
}

void EmbeddedObject::loadOasis(const QDomElement &element, KoOasisLoadingContext & context )
{
  if(element.hasAttributeNS( KoXmlNS::draw, "name" ))
    m_objectName = element.attributeNS( KoXmlNS::draw, "name", QString::null);
  m_geometry.setX( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x", QString::null ) ) );
  m_geometry.setY( KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y", QString::null ) ) );
  m_geometry.setWidth(KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "width", QString::null )) );
  m_geometry.setHeight(KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "height", QString::null ) ) );
    //kdDebug()<<" orig.x() :"<<orig.x() <<" orig.y() :"<<orig.y() <<"ext.width() :"<<ext.width()<<" ext.height(): "<<ext.height()<<endl;
  KoStyleStack &styleStack = context.styleStack();

  styleStack.setTypeProperties( "" ); //no type default type
}


QDomElement EmbeddedObject::save( QDomDocument& /*doc*/ )
{
    kdDebug() << "Saving EmbeddedObject..." << endl;
    return QDomElement();
}

void EmbeddedObject::saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const
{
    xmlWriter.addAttribute( "draw:id", "object" + QString::number( indexObj ) );
    //save all into pt
    xmlWriter.addAttributePt( "svg:x", sheet()->doc()->savingWholeDocument() ? m_geometry.x() : m_geometry.x() + 20.0 );
    xmlWriter.addAttributePt( "svg:y", sheet()->doc()->savingWholeDocument() ? m_geometry.y() : m_geometry.y() + 20.0 );
    xmlWriter.addAttributePt( "svg:width", m_geometry.width() );
    xmlWriter.addAttributePt( "svg:height", m_geometry.height() );

//     if ( kAbs( angle ) > 1E-6 )
//     {
//         double value = -1 * ( ( double )angle* M_PI )/180.0;
//         QString str=QString( "rotate (%1)" ).arg( value );
//         xmlWriter.addAttribute( "draw:transform", str );
//     }
}

bool EmbeddedObject::saveOasisObjectAttributes( KSpreadOasisSaveContext &/* sc */ ) const
{
    kdDebug() << "bool saveOasisObjectAttributes not implemented";
    return true;
}

bool EmbeddedObject::saveOasisObject( KSpreadOasisSaveContext &sc ) const
{
    sc.xmlWriter.startElement( getOasisElementName() );
    //sc.xmlWriter.addAttribute( "draw:style-name", getStyle( sc ) );
    saveOasisPosObject( sc.xmlWriter, sc.indexObj );
    if( !getObjectName().isEmpty())
        sc.xmlWriter.addAttribute( "draw:name", getObjectName() );

    saveOasisObjectAttributes( sc );

    sc.xmlWriter.endElement();
    return true;
}

void EmbeddedObject::draw( QPainter *_painter )
{
  paintSelection(_painter, SM_MOVERESIZE );
}

QPixmap EmbeddedObject::toPixmap()
{
	return toPixmap( 1.0 , 1.0 );
}

QPixmap EmbeddedObject::toPixmap(QSize size)
{
	double xZoom;
	double yZoom;

	calculateRequiredZoom( size , xZoom , yZoom );

	return toPixmap(xZoom,yZoom);
}

QPixmap EmbeddedObject::toPixmap(double /*xZoom*/ , double /*yZoom*/)
{
	return QPixmap();

}

void EmbeddedObject::calculateRequiredZoom( QSize desiredSize, double& xZoom, double& yZoom)
{
	QSize actualSize = geometry().size().toQSize();

	xZoom = (double) desiredSize.width() / (double)actualSize.width();
	yZoom = (double) desiredSize.height() / (double)actualSize.height();
}

void EmbeddedObject::paintSelection( QPainter *_painter, SelectionMode mode )
{
  if ( !m_selected || mode == SM_NONE )
    return;

  _painter->save();
  KoRect bound( geometry().left(), geometry().top(),
                geometry().width() , geometry().height() );
  QRect zoomedBound = sheet()->doc()->zoomRect( bound ) ;

  //_painter->setPen( QPen( Qt::black, 1, QPen::SolidLine ) );
  _painter->setPen( pen );
  _painter->setBrush( kapp->palette().color( QPalette::Active, QColorGroup::Highlight ) );

  //KoRect r = rotateRectObject(); // TODO: rotation
  KoRect r = /*KoRect::fromQRect*/( bound );
  int x = sheet()->doc()->zoomItX( r.left() /*- orig.x()*/);
  int y = sheet()->doc()->zoomItY( r.top() /*- orig.y()*/);
  int zX6 = /*sheet()->doc()->zoomItX*/( 6 );
  int zY6 = /*sheet()->doc()->zoomItY*/( 6 );
  int w = sheet()->doc()->zoomItX(r.width()) - 6;
  int h = sheet()->doc()->zoomItY(r.height()) - 6;

  if ( mode == SM_MOVERESIZE ) {
    _painter->drawRect( x, y,  zX6, zY6 );
    _painter->drawRect( x, y + h / 2, zX6, zY6 );
    _painter->drawRect( x, y + h, zX6, zY6 );
    _painter->drawRect( x + w, y, zX6, zY6 );
    _painter->drawRect( x + w, y + h / 2, zX6, zY6 );
    _painter->drawRect( x + w, y + h, zX6, zY6 );
    _painter->drawRect( x + w / 2, y,zX6, zY6 );
    _painter->drawRect( x + w / 2, y + h, zX6, zY6 );
  }
  else if ( mode == SM_PROTECT) {
    _painter->drawRect( x, y,  zX6, zY6 );
    _painter->drawRect( x, y + h / 2, zX6, zY6 );
    _painter->drawRect( x, y + h, zX6, zY6 );
    _painter->drawRect( x + w, y, zX6, zY6 );
    _painter->drawRect( x + w, y + h / 2, zX6, zY6 );
    _painter->drawRect( x + w, y + h, zX6, zY6 );
    _painter->drawRect( x + w / 2, y,zX6, zY6 );
    _painter->drawRect( x + w / 2, y + h, zX6, zY6 );

    x= x + 1;
    y= y + 1;
    zX6=zX6-2;
    zY6=zY6-2;

    QBrush brush=kapp->palette().color( QPalette::Active,QColorGroup::Base );
    _painter->fillRect( x, y,  zX6, zY6, brush );
    _painter->fillRect( x, y + h / 2, zX6, zY6, brush);
    _painter->fillRect( x, y + h, zX6, zY6, brush );
    _painter->fillRect( x + w, y, zX6, zY6, brush );
    _painter->fillRect( x + w, y + h / 2, zX6, zY6, brush );
    _painter->fillRect( x + w  , y + h , zX6 , zY6 , brush );
    _painter->fillRect( x + w / 2 , y ,zX6 , zY6 , brush );
    _painter->fillRect( x + w / 2, y + h , zX6 , zY6 , brush );
  }
  else if ( mode == SM_ROTATE ) {
    _painter->drawEllipse( x, y,  zX6, zY6 );
    _painter->drawEllipse( x, y + h, zX6, zY6 );
    _painter->drawEllipse( x + w, y, zX6, zY6 );
    _painter->drawEllipse( x + w, y + h, zX6, zY6 );
  }

  _painter->restore();
}

QCursor EmbeddedObject::getCursor( const QPoint &_point, ModifyType &_modType, QRect &geometry) const
{
    int px = /*sheet()->doc()->zoomItX*/(_point.x());
    int py = /*sheet()->doc()->zoomItY*/(_point.y());
    int ox = /*sheet()->doc()->zoomItX*/(/*orig*/geometry.x());
    int oy = /*sheet()->doc()->zoomItY*/(/*orig*/geometry.y());
    int ow = /*sheet()->doc()->zoomItX*/(/*ext*/geometry.width());
    int oh = /*sheet()->doc()->zoomItY*/(/*ext*/geometry.height());

//     if ( angle != 0.0 )
//     {
//         QRect rr = sheet()->doc()->zoomRect( rotateRectObject() );
//         ox = rr.x();
//         oy = rr.y();
//         ow = rr.width();
//         oh = rr.height();
//     }

    int sz = 4;
    if ( px >= ox && py >= oy && px <= ox + QMIN( ow / 3, sz ) && py <= oy + QMIN( oh / 3, sz ) )
    {
        _modType = MT_RESIZE_LU;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeFDiagCursor;
    }

    if ( px >= ox && py >= oy + oh / 2 - QMIN( oh / 6, sz / 2 )
         && px <= ox + QMIN( ow / 3, sz)
         && py <= oy + oh / 2 + QMIN( oh / 6, sz / 2 ) )
    {
        _modType = MT_RESIZE_LF;
        if ( m_protect)
            return Qt::ForbiddenCursor;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox && py >= oy + oh - QMIN( oh / 3, sz ) && px <= ox + QMIN( ow / 3, sz ) && py <= oy + oh )
    {
        _modType = MT_RESIZE_LD;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow / 2 - QMIN( ow / 6, sz / 2 ) && py >= oy
         && px <= ox + ow / 2 + QMIN( ow / 6, sz / 2 )
         && py <= oy + QMIN( oh / 3, sz ) )
    {
        _modType = MT_RESIZE_UP;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow / 2 - QMIN( ow / 6, sz / 2 ) && py >= oy + oh - QMIN( oh / 3, sz )
         && px <= ox + ow / 2 + QMIN( ow / 6, sz / 2 ) && py <= oy + oh )
    {
        _modType = MT_RESIZE_DN;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeVerCursor;
    }

    if ( px >= ox + ow - QMIN( ow / 3, sz ) && py >= oy && px <= ox + ow && py <= oy + QMIN( oh / 3, sz) )
    {
        _modType = MT_RESIZE_RU;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeBDiagCursor;
    }

    if ( px >= ox + ow - QMIN( ow / 3, sz ) && py >= oy + oh / 2 - QMIN( oh / 6, sz / 2 )
         && px <= ox + ow && py <= oy + oh / 2 + QMIN( oh / 6, sz / 2) )
    {
        _modType = MT_RESIZE_RT;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeHorCursor;
    }

    if ( px >= ox + ow - QMIN( ow / 3, sz ) && py >= oy + oh - QMIN( oh / 3, sz)
         && px <= ox + ow && py <= oy + oh )
    {
        _modType = MT_RESIZE_RD;
        if ( m_protect )
            return Qt::ForbiddenCursor;
        return Qt::sizeFDiagCursor;
    }

    _modType = MT_MOVE;
    return Qt::sizeAllCursor;
}


void EmbeddedObject::doDelete()
{
    if ( cmds == 0 && !inObjList )
        delete this;
}

/**********************************************************
 *
 * EmbeddedKOfficeObject
 *
 **********************************************************/
EmbeddedKOfficeObject::EmbeddedKOfficeObject( Doc *parent, Sheet *_sheet, KoDocument* doc, const KoRect& geometry )
    : EmbeddedObject( _sheet, geometry ), m_parent(parent)
{
    m_embeddedObject = new KoDocumentChild(parent, doc, geometry.toQRect() );
}

EmbeddedKOfficeObject::EmbeddedKOfficeObject( Doc *parent, Sheet *_sheet )
    : EmbeddedObject( _sheet, KoRect() ), m_parent(parent)
{
    m_embeddedObject = new KoDocumentChild( parent );
}

EmbeddedKOfficeObject::~EmbeddedKOfficeObject()
{
  delete m_embeddedObject;
}

Doc* EmbeddedKOfficeObject::parent()
{
    return m_parent;
}

KoDocumentChild* EmbeddedKOfficeObject::embeddedObject()
{
    return m_embeddedObject;
}

bool EmbeddedKOfficeObject::saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const
{
    kdDebug() << "EmbeddedKOfficeObject::saveOasisPart " << sc.partIndexObj << endl;

    sc.xmlWriter.startElement( "draw:object" );
    const QString name = QString( "Object_%1" ).arg( sc.partIndexObj + 1 );
    ++sc.partIndexObj;
    m_embeddedObject->saveOasisAttributes( sc.xmlWriter, name );

    if ( getType() != OBJECT_CHART )
        sc.xmlWriter.endElement();
    return true;
}

const char * EmbeddedKOfficeObject::getOasisElementName() const
{
    return "draw:frame";
}

bool EmbeddedKOfficeObject::load( const QDomElement& element )
{
    kdDebug() << "Loading EmbeddedKOfficeObject" << endl;
    bool result = embeddedObject()->load( element );
    setGeometry( KoRect::fromQRect( embeddedObject()->geometry() ) );
    return result;
}

void EmbeddedKOfficeObject::loadOasis(const QDomElement &element, KoOasisLoadingContext &context/*, KPRLoadingInfo *info*/)
{
    kdDebug()<<"void EmbeddedKOfficeObject::loadOasis(const QDomElement &element)******************\n";
    EmbeddedObject::loadOasis( element, context );

    QDomElement objectElement = KoDom::namedItemNS( element, KoXmlNS::draw, "object" );
    m_embeddedObject->loadOasis( element, objectElement );
    if( element.hasAttributeNS( KoXmlNS::draw, "name" ) )
        m_objectName = element.attributeNS( KoXmlNS::draw, "name", QString::null);
    (void)m_embeddedObject->loadOasisDocument( context.store(), context.manifestDocument() );
}

QDomElement EmbeddedKOfficeObject::save( QDomDocument& doc )
{
    kdDebug() << "Saving EmbeddedKOfficeObject" << endl;
    embeddedObject()->setGeometry( geometry().toQRect() );
    return m_embeddedObject->save( doc );
}

void EmbeddedKOfficeObject::draw( QPainter *_painter )
{
  kdDebug() << "Painting..." << endl;

  int const penw = pen.width() ;
  KoRect bound( 0, 0,
                geometry().width() - ( 2 * penw ), geometry().height() - ( 2 * penw ) );
  QRect const zoomedBound = sheet()->doc()->zoomRect( bound );

  _painter->save();
  int const xOffset = sheet()->doc()->zoomItX( geometry().left() + penw);
  int const yOffset = sheet()->doc()->zoomItY( geometry().top() + penw );

  QRect new_geometry = zoomedBound;

  //if ( translate )
  //{
  	_painter->translate( xOffset , yOffset );

  	new_geometry.moveBy(  xOffset , yOffset );
  	new_geometry.moveBy( -_painter->window().x(), -_painter->window().y() );
  //}

  _painter->setClipRect( zoomedBound, QPainter::CoordPainter );

   assert( embeddedObject()->document() != 0 );

   double zoomX = static_cast<double>( sheet()->doc()->zoom() ) / 100;
   double zoomY = static_cast<double>( sheet()->doc()->zoom() ) / 100;
   embeddedObject()->document()->paintEverything( *_painter,
        zoomedBound,
        embeddedObject()->isTransparent(),
        0 /* View isn't known from here - is that a problem? */,
        zoomX,
        zoomY );


   embeddedObject()->setGeometry( new_geometry );
  _painter->restore();

  EmbeddedObject::draw( _painter );
}

QPixmap EmbeddedKOfficeObject::toPixmap( double xZoom , double yZoom )
{
	QPixmap pixmap( (int)( geometry().width()*xZoom ), (int)( geometry().height()*yZoom ) );

	QPainter painter(&pixmap);
	QRect  bound( 0,0,(int)( geometry().width()*xZoom ), (int)(geometry().height()*yZoom) );
	embeddedObject()->document()->paintEverything(painter,bound,
					embeddedObject()->isTransparent(),
					0,
					xZoom,
					yZoom);
	return pixmap;
}

void EmbeddedKOfficeObject::activate( View *_view, Canvas* /* canvas */ )
{
    KoDocument* part = embeddedObject()->document();
    if ( !part )
        return;
    kdDebug() << "Activating..." << endl;

    _view->partManager()->addPart( part, false );
    _view->partManager()->setActivePart( part, _view );
}

void EmbeddedKOfficeObject::deactivate()
{
}


void EmbeddedKOfficeObject::updateChildGeometry()
{
//   KoZoomHandler* zh = m_sheet->doc();
//   embeddedObject()->setGeometry( zh->zoomRect( geometry() ), true );

//   return;
//   QRect r = sheet()->doc()->zoomRect( geometry() );
//   if ( _canvas )
//   {
//     kdDebug() << "_canvas->xOffset():" << _canvas->xOffset() << endl;
//     kdDebug() << "         _canvas->xOffset()*_canvas->doc()->zoomedResolutionX():" << _canvas->xOffset() * _canvas->doc()->zoomedResolutionX() << endl;
//     kdDebug() << "_canvas->yOffset():" << _canvas->yOffset() << endl;
//     kdDebug() << "         _canvas->yOffset()*_canvas->doc()->zoomedResolutionY():" << _canvas->yOffset() * _canvas->doc()->zoomedResolutionY() << endl;
//     r.moveBy( -_canvas->xOffset() / _canvas->doc()->zoomedResolutionX() , -_canvas->yOffset() / _canvas->doc()->zoomedResolutionY() );
//   }
//   embeddedObject()->setGeometry( r , true );
}

/**********************************************************
 *
 * EmbeddedChart
 *
 **********************************************************/

EmbeddedChart::EmbeddedChart( Doc *_spread, Sheet *_sheet, KoDocument* doc, const KoRect& geometry )
  : EmbeddedKOfficeObject( _spread, _sheet, doc, geometry )
{
    m_pBinding = 0;
}

EmbeddedChart::EmbeddedChart( Doc *_spread, Sheet *_sheet )
  : EmbeddedKOfficeObject( _spread, _sheet )
{
    m_pBinding = 0;
}

EmbeddedChart::~EmbeddedChart()
{
  if ( m_embeddedObject->isDeleted() )
    delete m_pBinding;
}

void EmbeddedChart::setDataArea( const QRect& _data )
{
    if ( m_pBinding == 0L )
        m_pBinding = new ChartBinding( m_sheet, _data, this );
    else
        m_pBinding->setDataArea( _data );
}

void EmbeddedChart::update()
{
    if ( m_pBinding )
        m_pBinding->cellChanged( 0 );
}

const char * EmbeddedChart::getOasisElementName() const
{
    return "draw:frame";
}

bool EmbeddedChart::load( const QDomElement& element )
{
    kdDebug() << "Loading EmbeddedChart" << endl;
    if ( !EmbeddedKOfficeObject::load( element ) )
        return false;

    if ( element.hasAttribute( "left-cell" ) &&
         element.hasAttribute( "top-cell" ) &&
         element.hasAttribute( "right-cell" ) &&
         element.hasAttribute( "bottom-cell" ) )
    {
        QRect r;
        r.setCoords( element.attribute( "left-cell" ).toInt(),
                     element.attribute( "top-cell" ).toInt(),
                     element.attribute( "right-cell" ).toInt(),
                     element.attribute( "bottom-cell" ).toInt() );

        setDataArea( r );
    }

    return true;
}

void EmbeddedChart::loadOasis(const QDomElement &element, KoOasisLoadingContext &context/*, KPRLoadingInfo *info*/)
{
    kdDebug()<<"void EmbeddedChart::loadOasis(const QDomElement &element)******************\n";
    EmbeddedKOfficeObject::loadOasis( element, context );

    QDomElement objectElement = KoDom::namedItemNS( element, KoXmlNS::draw, "object" );
    QString str_range = objectElement.attributeNS( KoXmlNS::draw, "notify-on-update-of-ranges", QString::null);

    if ( !str_range.isNull() )
    {
      str_range = Oasis::decodeFormula( str_range );
      Range range( str_range );
      if ( range.isValid() )
        setDataArea( range.range() );
    }
}


bool EmbeddedChart::saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const
{
    kdDebug() << "EmbeddedChart::saveOasisPart " << sc.partIndexObj << endl;

    EmbeddedKOfficeObject::saveOasisObjectAttributes( sc );

    if(m_pBinding) { // see http://bugs.kde.org/show_bug.cgi?id=120395
        QRect dataArea = m_pBinding->dataArea();
        QString rangeName = util_rangeName( dataArea);
        rangeName.insert( rangeName.find(':') +1, sheet()->sheetName() + "." );
        rangeName.prepend( sheet()->sheetName() + "." );
        sc.xmlWriter.addAttribute( "draw:notify-on-update-of-ranges", rangeName );
    }
    sc.xmlWriter.endElement();

    return true;
}

QDomElement EmbeddedChart::save( QDomDocument& doc )
{
    kdDebug() << "Saving EmbeddedChart" << endl;
    QDomElement element = EmbeddedKOfficeObject::save( doc );
    element.setTagName( "chart" );

    element.setAttribute( "left-cell", m_pBinding->dataArea().left() );
    element.setAttribute( "right-cell", m_pBinding->dataArea().right() );
    element.setAttribute( "top-cell", m_pBinding->dataArea().top() );
    element.setAttribute( "bottom-cell", m_pBinding->dataArea().bottom() );

    return element;
}
void EmbeddedChart::draw( QPainter *_painter )
{
  EmbeddedKOfficeObject::draw( _painter );
}

bool EmbeddedChart::loadDocument( KoStore* _store )
{
    bool res = /*EmbeddedKOfficeObject::*/m_embeddedObject->loadDocument( _store );
    if ( !res )
        return res;

    // Did we see a cell rectangle ?
    if ( !m_pBinding )
        return true;

    update();

    chart()->setCanChangeValue( false  );
    return true;
}

KoChart::Part* EmbeddedChart::chart()
{
    assert( m_embeddedObject->document()->inherits( "KoChart::Part" ) );
    return static_cast<KoChart::Part *>( m_embeddedObject->document() );
}

/**********************************************************
 *
 * EmbeddedPictureObject
 *
 **********************************************************/
EmbeddedPictureObject::EmbeddedPictureObject( Sheet *_sheet, const KoRect& _geometry, KoPictureCollection *_imageCollection )
   : EmbeddedObject( _sheet, _geometry )
{
    imageCollection = _imageCollection;
    pen = KoPen( Qt::black, 1.0, Qt::NoPen );
    mirrorType = PM_NORMAL;
    depth = 0;
    swapRGB = false;
    grayscal = false;
    bright = 0;
    m_effect = IE_NONE;
    m_ie_par1 = QVariant();
    m_ie_par2 = QVariant();
    m_ie_par3 = QVariant();
    // Forbid QPixmap to cache the X-Window resources (Yes, it is slower!)
    m_cachedPixmap.setOptimization(QPixmap::MemoryOptim);
}


EmbeddedPictureObject::EmbeddedPictureObject( Sheet *_sheet, const KoRect& _geometry, KoPictureCollection *_imageCollection, const KoPictureKey & key )
    : EmbeddedObject( _sheet, _geometry )
{
    imageCollection = _imageCollection;

    //ext = KoSize(); // invalid size means unset
    pen = KoPen( Qt::black, 1.0, Qt::NoPen );
    mirrorType = PM_NORMAL;
    depth = 0;
    swapRGB = false;
    grayscal = false;
    bright = 0;
    m_effect = IE_NONE;
    m_ie_par1 = QVariant();
    m_ie_par2 = QVariant();
    m_ie_par3 = QVariant();
    // Forbid QPixmap to cache the X-Window resources (Yes, it is slower!)
    m_cachedPixmap.setOptimization(QPixmap::MemoryOptim);

    setPicture( key );
}

EmbeddedPictureObject::EmbeddedPictureObject( Sheet *_sheet, KoPictureCollection *_imageCollection )
  : EmbeddedObject( _sheet, KoRect(0,0,0,0) )
{
  imageCollection = _imageCollection;

  //ext = KoSize(); // invalid size means unset
  pen = KoPen( Qt::black, 1.0, Qt::NoPen );
  mirrorType = PM_NORMAL;
  depth = 0;
  swapRGB = false;
  grayscal = false;
  bright = 0;
  m_effect = IE_NONE;
  m_ie_par1 = QVariant();
  m_ie_par2 = QVariant();
  m_ie_par3 = QVariant();
    // Forbid QPixmap to cache the X-Window resources (Yes, it is slower!)
  m_cachedPixmap.setOptimization(QPixmap::MemoryOptim);
}

EmbeddedPictureObject::~EmbeddedPictureObject()
{
}

bool EmbeddedPictureObject::load( const QDomElement& /*element*/ )
{
    return false;
}

QDomElement EmbeddedPictureObject::save( QDomDocument& /*doc*/ )
{
    kdDebug() << "Saving EmbeddedPictureObject" << endl;
    return QDomElement();
}

QString EmbeddedPictureObject::convertValueToPercent( int val ) const
{
   return QString::number( val )+"%";
}

void EmbeddedPictureObject::saveOasisPictureElement( KoGenStyle &styleobjectauto ) const
{

    if ( bright != 0 )
    {
        styleobjectauto.addProperty( "draw:luminance", convertValueToPercent( bright ) );
    }
    if ( grayscal )
    {
        styleobjectauto.addProperty( "draw:color-mode","greyscale" );
    }

    switch (m_effect)
    {
    case IE_NONE:
        //nothing
        break;
    case IE_CHANNEL_INTENSITY:
    {
        //for the moment kpresenter support just one channel
        QString percent = convertValueToPercent( m_ie_par1.toInt() );
        KImageEffect::RGBComponent channel = static_cast<KImageEffect::RGBComponent>( m_ie_par2.toInt() );
        switch( channel )
        {
        case KImageEffect::Red:
            styleobjectauto.addProperty( "draw:red", percent );
            styleobjectauto.addProperty( "draw:blue", "0%" );
            styleobjectauto.addProperty( "draw:green", "0%" );
            break;
        case KImageEffect::Green:
            styleobjectauto.addProperty( "draw:green", percent );
            styleobjectauto.addProperty( "draw:red", "0%" );
            styleobjectauto.addProperty( "draw:blue", "0%" );
            break;
        case KImageEffect::Blue:
            styleobjectauto.addProperty( "draw:blue", percent );
            styleobjectauto.addProperty( "draw:red", "0%" );
            styleobjectauto.addProperty( "draw:green", "0%" );
            break;
        case KImageEffect::Gray:
            break;
        case KImageEffect::All:
            break;
        }
    }
    break;
    case IE_FADE:
        break;
    case IE_FLATTEN:
        break;
    case IE_INTENSITY:
        break;
    case IE_DESATURATE:
        break;
    case IE_CONTRAST:
    {
        //kpresenter use value between -255 and 255
        //oo impress between -100% and 100%
        int val =  m_ie_par1.toInt();
        val = ( int )( ( double )val*100.0/255.0 );
        styleobjectauto.addProperty( "draw:contrast", convertValueToPercent( val ) );
    }
    break;
    case IE_NORMALIZE:
        break;
    case IE_EQUALIZE:
        break;
    case IE_THRESHOLD:
        break;
    case IE_SOLARIZE:
        break;
    case IE_EMBOSS:
        break;
    case IE_DESPECKLE:
        break;
    case IE_CHARCOAL:
        break;
    case IE_NOISE:
        break;
    case IE_BLUR:
        break;
    case IE_EDGE:
        break;
    case IE_IMPLODE:
        break;
    case IE_OIL_PAINT:
        break;
    case IE_SHARPEN:
        break;
    case IE_SPREAD:
        break;
    case IE_SHADE:
        break;
    case IE_SWIRL:
        break;
    case IE_WAVE:
        break;
    }
}

bool EmbeddedPictureObject::saveOasisObjectAttributes( KSpreadOasisSaveContext &sc ) const
{
    sc.xmlWriter.startElement( "draw:image" );
    sc.xmlWriter.addAttribute( "xlink:type", "simple" );
    sc.xmlWriter.addAttribute( "xlink:show", "embed" );
    sc.xmlWriter.addAttribute( "xlink:actuate", "onLoad" );
    sc.xmlWriter.addAttribute( "xlink:href", imageCollection->getOasisFileName( image ) );
    sc.xmlWriter.endElement();

    return true;
}

const char * EmbeddedPictureObject::getOasisElementName() const
{
    return "draw:frame";
}


void EmbeddedPictureObject::loadPicture( const QString & fileName )
{
    image = imageCollection->loadPicture( fileName );
}

EmbeddedPictureObject &EmbeddedPictureObject::operator=( const EmbeddedPictureObject & )
{
    return *this;
}

void EmbeddedPictureObject::setPicture( const KoPictureKey & key )
{
    image = imageCollection->findPicture( key );
}

void EmbeddedPictureObject::reload( void )
{
    // ### FIXME: this seems wrong, KoPictureCollection will never reload it (or perhaps it is the function name that is wrong)
    setPicture( image.getKey() );
}

// KSpread doesn't support pictures in it's old XML file format.
// QDomDocumentFragment EmbeddedPictureObject::save( QDomDocument& doc, double offset )
// {
//     QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
//     QDomElement elem=doc.createElement("KEY");
//     image.getKey().saveAttributes(elem);
//     fragment.appendChild(elem);
//
//     QDomElement elemSettings = doc.createElement( "PICTURESETTINGS" );
//
//     elemSettings.setAttribute( "mirrorType", static_cast<int>( mirrorType ) );
//     elemSettings.setAttribute( "depth", depth );
//     elemSettings.setAttribute( "swapRGB", static_cast<int>( swapRGB ) );
//     elemSettings.setAttribute( "grayscal", static_cast<int>( grayscal ) );
//     elemSettings.setAttribute( "bright", bright );
//     fragment.appendChild( elemSettings );
//
//     if (m_effect!=IE_NONE) {
//         QDomElement imageEffects = doc.createElement("EFFECTS");
//         imageEffects.setAttribute("type", static_cast<int>(m_effect));
//         if (m_ie_par1.isValid())
//             imageEffects.setAttribute("param1", m_ie_par1.toString());
//         if (m_ie_par2.isValid())
//             imageEffects.setAttribute("param2", m_ie_par2.toString());
//         if (m_ie_par3.isValid())
//             imageEffects.setAttribute("param3", m_ie_par3.toString());
//         fragment.appendChild( imageEffects );
//     }
//
//     return fragment;
// }

void EmbeddedPictureObject::loadOasisPictureEffect(KoOasisLoadingContext & context )
{
    KoStyleStack &styleStack = context.styleStack();
    styleStack.setTypeProperties( "graphic" );
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "color-mode" ) &&  ( styleStack.attributeNS( KoXmlNS::draw, "color-mode" )=="greyscale" ) )
    {
        grayscal = true;
    }

    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "contrast" ) )
    {
        QString str( styleStack.attributeNS( KoXmlNS::draw, "contrast" ) );
        str = str.remove( '%' );
        int val = str.toInt();
        m_effect = IE_CONTRAST;
        val = ( int )( 255.0 *val/100.0 );
        m_ie_par1 = QVariant(val);
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "red" ) && styleStack.attributeNS( KoXmlNS::draw, "red" ) != "0%" )
    {
        QString str( styleStack.attributeNS( KoXmlNS::draw, "red" ) );
        str = str.remove( '%' );
        int val = str.toInt();
        m_effect = IE_CHANNEL_INTENSITY;
        m_ie_par1 = QVariant(val);
        m_ie_par2 = QVariant( ( int )KImageEffect::Red );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "green" ) && styleStack.attributeNS( KoXmlNS::draw, "green" ) != "0%" )
    {
        QString str( styleStack.attributeNS( KoXmlNS::draw, "green" ) );
        str = str.remove( '%' );
        int val = str.toInt();
        m_effect = IE_CHANNEL_INTENSITY;
        m_ie_par1 = QVariant(val);
        m_ie_par2 = QVariant( ( int )KImageEffect::Green );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "blue" ) && styleStack.attributeNS( KoXmlNS::draw, "blue" ) != "0%" )
    {
        QString str( styleStack.attributeNS( KoXmlNS::draw, "blue" ) );
        str = str.remove( '%' );
        int val = str.toInt();
        m_effect = IE_CHANNEL_INTENSITY;
        m_ie_par1 = QVariant(val);
        m_ie_par2 = QVariant( ( int )KImageEffect::Blue );
    }
    if ( styleStack.hasAttributeNS( KoXmlNS::draw, "luminance" ) )
    {
       QString str( styleStack.attributeNS( KoXmlNS::draw, "luminance" ) );
       str = str.remove( '%' );
       bright = str.toInt();
    }
}

void EmbeddedPictureObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& /*mainStyles*/ ) const
{
     //KP2DObject::fillStyle( styleObjectAuto, mainStyles );
     saveOasisPictureElement( styleObjectAuto );
}

void EmbeddedPictureObject::loadOasis(const QDomElement &element, KoOasisLoadingContext & context/*, KPRLoadingInfo *info*/)
{
    //load it into kpresenter_doc
    EmbeddedObject::loadOasis( element, context );
    loadOasisPictureEffect( context );
    QDomNode imageBox = KoDom::namedItemNS( element, KoXmlNS::draw, "image" );
    const QString href( imageBox.toElement().attributeNS( KoXmlNS::xlink, "href", QString::null) );
    kdDebug()<<" href: "<<href<<endl;
    if ( !href.isEmpty() /*&& href[0] == '#'*/ )
    {
        QString strExtension;
        const int result=href.findRev(".");
        if (result>=0)
        {
            strExtension=href.mid(result+1); // As we are using KoPicture, the extension should be without the dot.
        }
        QString filename(href/*.mid(1)*/);
        const KoPictureKey key(filename, QDateTime::currentDateTime(Qt::UTC));
        image.setKey(key);

        KoStore* store = context.store();
        if ( store->open( filename ) )
        {
            KoStoreDevice dev(store);
            if ( !image.load( &dev, strExtension ) )
                kdWarning() << "Cannot load picture: " << filename << " " << href << endl;
            store->close();
        }
        imageCollection->insertPicture( key, image );
    }
    // ### TODO: load remote file
}


// double EmbeddedPictureObject::load(const QDomElement &element)
// {
//     double offset=KP2DObject::load(element);
//     QDomElement e=element.namedItem("KEY").toElement();
//     if(!e.isNull()) {
//         KoPictureKey key;
//         key.loadAttributes( e );
//         image.clear();
//         image.setKey(key);
//     }
//     else {
//         // try to find a PIXMAP tag if the KEY is not available...
//         e=element.namedItem("PIXMAP").toElement();
//         if (e.isNull()) {
//             // try to find a FILENAME tag (old cliparts...)
//             e=element.namedItem("FILENAME").toElement();
//             if(!e.isNull()) {
//                 // Loads from the disk directly (unless it's in the collection already?)
//                 image = imageCollection->loadPicture( e.attribute("filename") );
//             }
//         } else {
//             bool openPic = true;
//             QString _data;
//             QString _fileName;
//             if(e.hasAttribute("data"))
//                 _data=e.attribute("data");
//             if ( _data.isEmpty() )
//                 openPic = true;
//             else
//                 openPic = false;
//             if(e.hasAttribute("filename"))
//                 _fileName=e.attribute("filename");
//             if ( !_fileName.isEmpty() )
//             {
//                 if ( int _envVarB = _fileName.find( '$' ) >= 0 )
//                 {
//                     int _envVarE = _fileName.find( '/', _envVarB );
//                     // ### FIXME: it should be QString::local8Bit instead of QFile::encodeName, shouldn't it?
//                     QString path = getenv( QFile::encodeName(_fileName.mid( _envVarB, _envVarE-_envVarB )) );
//                     _fileName.replace( _envVarB-1, _envVarE-_envVarB+1, path );
//                 }
//             }
//
//             if ( openPic )
//                 // !! this loads it from the disk (unless it's in the image collection already)
//                 image = imageCollection->loadPicture( _fileName );
//             else
//             {
//                 KoPictureKey key( _fileName );
//                 image.clear();
//                 image.setKey(key);
//                 QByteArray rawData=_data.utf8(); // XPM is normally ASCII, therefore UTF-8
//                 rawData[rawData.size()-1]=char(10); // Replace the NULL character by a LINE FEED
//                 QBuffer buffer(rawData); // ### TODO: open?
//                 image.loadXpm(&buffer);
//             }
//         }
//     }
//
//     e = element.namedItem( "PICTURESETTINGS" ).toElement();
//     if ( !e.isNull() ) {
//         PictureMirrorType _mirrorType = PM_NORMAL;
//         int _depth = 0;
//         bool _swapRGB = false;
//         bool _grayscal = false;
//         int _bright = 0;
//
//         if ( e.hasAttribute( "mirrorType" ) )
//             _mirrorType = static_cast<PictureMirrorType>( e.attribute( "mirrorType" ).toInt() );
//         if ( e.hasAttribute( "depth" ) )
//             _depth = e.attribute( "depth" ).toInt();
//         if ( e.hasAttribute( "swapRGB" ) )
//             _swapRGB = static_cast<bool>( e.attribute( "swapRGB" ).toInt() );
//         if ( e.hasAttribute( "grayscal" ) )
//             _grayscal = static_cast<bool>( e.attribute( "grayscal" ).toInt() );
//         if ( e.hasAttribute( "bright" ) )
//             _bright = e.attribute( "bright" ).toInt();
//
//         mirrorType = _mirrorType;
//         depth = _depth;
//         swapRGB = _swapRGB;
//         grayscal = _grayscal;
//         bright = _bright;
//     }
//     else {
//         mirrorType = PM_NORMAL;
//         depth = 0;
//         swapRGB = false;
//         grayscal = false;
//         bright = 0;
//     }
//
//     e = element.namedItem( "EFFECTS" ).toElement();
//     if (!e.isNull()) {
//         if (e.hasAttribute("type"))
//             m_effect = static_cast<ImageEffect>(e.attribute("type").toInt());
//         if (e.hasAttribute("param1"))
//             m_ie_par1 = QVariant(e.attribute("param1"));
//         else
//             m_ie_par1 = QVariant();
//         if (e.hasAttribute("param2"))
//             m_ie_par2 = QVariant(e.attribute("param2"));
//         else
//             m_ie_par2 = QVariant();
//         if (e.hasAttribute("param3"))
//             m_ie_par3 = QVariant(e.attribute("param3"));
//         else
//             m_ie_par3 = QVariant();
//     }
//     else
//         m_effect = IE_NONE;
//
//     return offset;
// }

void EmbeddedPictureObject::drawShadow( QPainter* /*_painter*/,  KoZoomHandler* /*_zoomHandler*/)
{
//     const double ox = /*orig*/m_geometry.x();
//     const double oy = /*orig*/m_geometry.y();
//     const double ow = /*ext*/m_geometry.width();
//     const double oh = /*ext*/m_geometry.height();
//
//     _painter->save();
//
//     QPen pen2 = pen.zoomedPen( _zoomHandler );
//     _painter->setPen( pen2 );
//     _painter->setBrush( getBrush() );
//
//     double sx = 0;
//     double sy = 0;
//
//     getShadowCoords( sx, sy );
//
//     _painter->translate( _zoomHandler->zoomItX( ox ), _zoomHandler->zoomItY( oy ) );
//     _painter->setPen( QPen( shadowColor ) );
//     _painter->setBrush( shadowColor );
//     if ( kAbs(angle) <= DBL_EPSILON )
//         _painter->drawRect( _zoomHandler->zoomItX( sx ), _zoomHandler->zoomItY( sy ),
//                             _zoomHandler->zoomItX( ext.width() ), _zoomHandler->zoomItY( ext.height() ) );
//     else
//     {
//         QSize bs = QSize( _zoomHandler->zoomItX( ow ), _zoomHandler->zoomItY( oh ) );
//         QRect br = QRect( 0, 0, bs.width(), bs.height() );
//         int pw = br.width();
//         int ph = br.height();
//         QRect rr = br;
//         int pixYPos = -rr.y();
//         int pixXPos = -rr.x();
//         br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
//         rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );
//
//         QWMatrix m;
//         m.translate( pw / 2, ph / 2 );
//         m.rotate( angle );
//         m.translate( rr.left() + pixXPos + _zoomHandler->zoomItX( sx ),
//                      rr.top() + pixYPos + _zoomHandler->zoomItY( sy ) );
//
//         _painter->setWorldMatrix( m, true );
//
//         _painter->drawRect( 0, 0, bs.width(), bs.height() );
//     }
//
//     _painter->restore();
}

QPixmap EmbeddedPictureObject::toPixmap( double xZoom , double yZoom )
{
 	KoZoomHandler zoomHandler;
 	zoomHandler.setZoomedResolution( xZoom /* *zoomHandler.resolutionX()*/ , yZoom /* *zoomHandler.resolutionY()*/ );
 	return generatePixmap( &zoomHandler );
}

QPixmap EmbeddedPictureObject::generatePixmap(KoZoomHandler*_zoomHandler)
{
    const double penw = _zoomHandler->zoomItX( ( ( pen.style() == Qt::NoPen ) ? 1 : pen.width() ) / 2.0 );

    QSize size( _zoomHandler->zoomSize( m_geometry.size() /*ext*/ ) );
    //kdDebug(33001) << "EmbeddedPictureObject::generatePixmap size= " << size << endl;
    QPixmap pixmap(size);
    QPainter paint;

    paint.begin( &pixmap );
    pixmap.fill( Qt::white );

    // Draw background
    paint.setPen( Qt::NoPen );
    paint.setBrush( getBrush() );

    QRect rect( (int)( penw ), (int)( penw ),
                 (int)( _zoomHandler->zoomItX( /*ext*/m_geometry.width() ) - 2.0 * penw ),
                 (int)( _zoomHandler->zoomItY( /*ext*/m_geometry.height() ) - 2.0 * penw ) );

//      if ( getFillType() == FT_BRUSH || !gradient )
         paint.drawRect( rect );
//     else {
        // ### TODO: this was also drawn for drawContour==true, but why?
//         gradient->setSize( size );
//         paint.drawPixmap( (int)( penw ), (int)( penw ),
//                           gradient->pixmap(), 0, 0,
//                           (int)( _zoomHandler->zoomItX( m_geometry/*ext*/.width() ) - 2 * penw ),
//                           (int)( _zoomHandler->zoomItY( m_geometry/*ext*/.height() ) - 2 * penw ) );
//    }


    image.draw(paint, 0, 0, size.width(), size.height(), 0, 0, -1, -1, false); // Always slow mode!
    image.clearCache(); // Release the memoy of the picture cache

//     image.setAlphaBuffer(true);
//     QBitmap tmpMask;
//     tmpMask = image.createAlphaMask().scale(size);
//     pixmap.setMask(tmpMask);

    paint.end();
    return pixmap;
}

void EmbeddedPictureObject::draw( QPainter *_painter/*, KoZoomHandler*_zoomHandler,
                           int pageNum, SelectionMode selectionMode, bool drawContour*/ )
{
    bool drawContour = false;
    KoZoomHandler*_zoomHandler = sheet()->doc();


    if ( image.isNull() ) return;

//     if ( shadowDistance > 0 && !drawContour )
//         drawShadow(_painter, _zoomHandler);

    const double ox = /*orig*/m_geometry.x();
    const double oy = /*orig*/m_geometry.y();
    const double ow = /*ext*/m_geometry.width();
    const double oh = /*ext*/m_geometry.height();
    //const double penw = _zoomHandler->zoomItX( ( ( pen.style() == Qt::NoPen ) ? 1.0 : pen.width() ) / 2.0 );

    _painter->save();

    _painter->translate( _zoomHandler->zoomItX( ox ), _zoomHandler->zoomItY( oy ) );

    if ( kAbs(angle)> DBL_EPSILON ) {
        QSize bs = QSize( _zoomHandler->zoomItX( ow ), _zoomHandler->zoomItY( oh ) );
        QRect br = QRect( 0, 0, bs.width(), bs.height() );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int pixYPos = -rr.y();
        int pixXPos = -rr.x();
        br.moveTopLeft( QPoint( -br.width() / 2, -br.height() / 2 ) );
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m;
        m.translate( pw / 2, ph / 2 );
        m.rotate( angle );
        m.translate( rr.left() + pixXPos, rr.top() + pixYPos );
        _painter->setWorldMatrix( m, true );
    }

    if ( !drawContour )
    {
        QRect rect( 0, 0, (int)( _zoomHandler->zoomItX( ow ) ),
                    (int)( _zoomHandler->zoomItY(  oh ) ) );
        // ### HACK QT seems not to be able to correctly compare QVariant
        bool variants1;
        if (m_ie_par1.isNull())
            variants1=m_cachedPar1.isNull();
        else
            variants1=(m_ie_par1 == m_cachedPar1);
        bool variants2;
        if (m_ie_par2.isNull())
            variants2=m_cachedPar2.isNull();
        else
            variants2=(m_ie_par2 == m_cachedPar2);
        bool variants3;
        if (m_ie_par3.isNull())
            variants3=m_cachedPar3.isNull();
        else
            variants3=(m_ie_par3 == m_cachedPar3);

        if (m_cachedRect == rect
            // All what EmbeddedPictureObject::changePictureSettings needs
            && m_cachedMirrorType == mirrorType && m_cachedSwapRGB == swapRGB && m_cachedGrayscal == grayscal
            && m_cachedBright == bright && m_cachedEffect == m_effect
            // Who needs it?
            && m_cachedDepth == depth
#if 0
            && m_ie_par1 == m_cachedPar1 && m_ie_par2 == m_cachedPar2 && m_ie_par3 == m_cachedPar3
#else
            && variants1 && variants2 && variants3
#endif
            )
        {
            //kdDebug(33001) << "Drawing cached pixmap " << (void*) this << " " << k_funcinfo << endl;
        }
        else
        {
            if (mirrorType != PM_NORMAL || depth != 0 || swapRGB || grayscal || bright != 0 || m_effect!=IE_NONE)
                m_cachedPixmap = changePictureSettings( generatePixmap( _zoomHandler ) );
            else
                m_cachedPixmap = generatePixmap( _zoomHandler );
            m_cachedRect = rect;
            m_cachedMirrorType = mirrorType;
            m_cachedSwapRGB = swapRGB;
            m_cachedGrayscal = grayscal;
            m_cachedBright = bright;
            m_cachedEffect = m_effect;
            m_cachedDepth = depth;
            m_cachedPar1 = m_ie_par1;
            m_cachedPar2 = m_ie_par2;
            m_cachedPar3 = m_ie_par3;
            //kdDebug(33001) <<  "Drawing non-cached pixmap " << (void*) this << " " << k_funcinfo << endl;
        }
        _painter->eraseRect( rect );
        _painter->drawPixmap( rect, m_cachedPixmap);
    }

    // Draw border
    // ### TODO port to KoBorder::drawBorders() (after writing a simplified version of it, that takes the same border on each size)
//     QPen pen2;
//     if ( drawContour ) {
//         pen2 = QPen( Qt::black, 1, Qt::DotLine );
//         _painter->setRasterOp( Qt::NotXorROP );
//     }
//     else {
//         pen2 = pen;
//         pen2.setWidth( _zoomHandler->zoomItX( ( pen.style() == Qt::NoPen ) ? 1.0 : (double)pen.width() ) );
//     }
//     _painter->setPen( pen2 );
//     _painter->setBrush( Qt::NoBrush );
//     _painter->drawRect( (int)( penw ), (int)( penw ),
//                         (int)( _zoomHandler->zoomItX( ow ) - 2.0 * penw ),
//                         (int)( _zoomHandler->zoomItY( oh ) - 2.0 * penw ) );


    _painter->restore();

    //KPObject::draw( _painter, _zoomHandler, pageNum, selectionMode, drawContour );
    EmbeddedObject::draw( _painter );
}

QPixmap EmbeddedPictureObject::getOriginalPixmap()
{
    QSize _pixSize = image.getOriginalSize();
    kdDebug(33001) << "EmbeddedPictureObject::getOriginalPixmap size= " << _pixSize << endl;
    QPixmap _pixmap = image.generatePixmap( _pixSize, true );
    image.clearCache(); // Release the memoy of the picture cache

    return _pixmap;
}

QPixmap EmbeddedPictureObject::changePictureSettings( QPixmap _tmpPixmap )
{
    QImage _tmpImage = _tmpPixmap.convertToImage();

    if (_tmpImage.isNull())
        return _tmpPixmap;

    bool _horizontal = false;
    bool _vertical = false;
    if ( mirrorType == PM_HORIZONTAL )
        _horizontal = true;
    else if ( mirrorType == PM_VERTICAL )
        _vertical = true;
    else if ( mirrorType == PM_HORIZONTALANDVERTICAL ) {
        _horizontal = true;
        _vertical = true;
    }

    _tmpImage = _tmpImage.mirror( _horizontal, _vertical );

    if ( depth != 0 ) {
        QImage tmpImg = _tmpImage.convertDepth( depth );
        if ( !tmpImg.isNull() )
            _tmpImage = tmpImg;
    }

    if ( swapRGB )
        _tmpImage = _tmpImage.swapRGB();

    if ( grayscal ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < _tmpImage.numColors(); ++i ) {
                QRgb rgb = _tmpImage.color( i );
                int gray = qGray( rgb );
                rgb = qRgb( gray, gray, gray );
                _tmpImage.setColor( i, rgb );
            }
        }
        else {
            int _width = _tmpImage.width();
            int _height = _tmpImage.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( _tmpImage.valid( _x, _y ) ) {
                        QRgb rgb = _tmpImage.pixel( _x, _y );
                        int gray = qGray( rgb );
                        rgb = qRgb( gray, gray, gray );
                        _tmpImage.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }

    if ( bright != 0 ) {
        if ( depth == 1 || depth == 8 ) {
            for ( int i = 0; i < _tmpImage.numColors(); ++i ) {
                QRgb rgb = _tmpImage.color( i );
                QColor c( rgb );

                if ( bright > 0 )
                    rgb = c.light( 100 + bright ).rgb();
                else
                    rgb = c.dark( 100 + abs( bright ) ).rgb();

                _tmpImage.setColor( i, rgb );
            }
        }
        else {
            int _width = _tmpImage.width();
            int _height = _tmpImage.height();
            int _x = 0;
            int _y = 0;

            for ( _x = 0; _x < _width; ++_x ) {
                for ( _y = 0; _y < _height; ++_y ) {
                    if ( _tmpImage.valid( _x, _y ) ) {
                        QRgb rgb = _tmpImage.pixel( _x, _y );
                        QColor c( rgb );

                        if ( bright > 0 )
                            rgb = c.light( 100 + bright ).rgb();
                        else
                            rgb = c.dark( 100 + abs( bright ) ).rgb();

                        _tmpImage.setPixel( _x, _y, rgb );
                    }
                }
            }
        }
    }

    switch (m_effect) {
    case IE_CHANNEL_INTENSITY: {
        _tmpImage = KImageEffect::channelIntensity(_tmpImage, m_ie_par1.toDouble()/100.0,
                                                   static_cast<KImageEffect::RGBComponent>(m_ie_par2.toInt()));
        break;
    }
    case IE_FADE: {
        _tmpImage = KImageEffect::fade(_tmpImage, m_ie_par1.toDouble(), m_ie_par2.toColor());
        break;
    }
    case IE_FLATTEN: {
        _tmpImage = KImageEffect::flatten(_tmpImage, m_ie_par1.toColor(), m_ie_par2.toColor());
        break;
    }
    case IE_INTENSITY: {
        _tmpImage = KImageEffect::intensity(_tmpImage, m_ie_par1.toDouble()/100.0);
        break;
    }
    case IE_DESATURATE: {
        _tmpImage = KImageEffect::desaturate(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_CONTRAST: {
        _tmpImage = KImageEffect::contrast(_tmpImage, m_ie_par1.toInt());
        break;
    }
    case IE_NORMALIZE: {
        KImageEffect::normalize(_tmpImage);
        break;
    }
    case IE_EQUALIZE: {
        KImageEffect::equalize(_tmpImage);
        break;
    }
    case IE_THRESHOLD: {
        KImageEffect::threshold(_tmpImage, m_ie_par1.toInt());
        break;
    }
    case IE_SOLARIZE: {
        KImageEffect::solarize(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_EMBOSS: {
        _tmpImage = KImageEffect::emboss(_tmpImage);
        break;
    }
    case IE_DESPECKLE: {
        _tmpImage = KImageEffect::despeckle(_tmpImage);
        break;
    }
    case IE_CHARCOAL: {
        _tmpImage = KImageEffect::charcoal(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_NOISE: {
        _tmpImage = KImageEffect::addNoise(_tmpImage, static_cast<KImageEffect::NoiseType>(m_ie_par1.toInt()));
        break;
    }
    case IE_BLUR: {
        _tmpImage = KImageEffect::blur(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_EDGE: {
        _tmpImage = KImageEffect::edge(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_IMPLODE: {
        _tmpImage = KImageEffect::implode(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_OIL_PAINT: {
        _tmpImage = KImageEffect::oilPaint(_tmpImage, m_ie_par1.toInt());
        break;
    }
    case IE_SHARPEN: {
        _tmpImage = KImageEffect::sharpen(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_SPREAD: {
        _tmpImage = KImageEffect::spread(_tmpImage, m_ie_par1.toInt());
        break;
    }
    case IE_SHADE: {
        _tmpImage = KImageEffect::shade(_tmpImage, m_ie_par1.toBool(), m_ie_par2.toDouble(), m_ie_par3.toDouble());
        break;
    }
    case IE_SWIRL: {
        _tmpImage = KImageEffect::swirl(_tmpImage, m_ie_par1.toDouble());
        break;
    }
    case IE_WAVE: {
        _tmpImage = KImageEffect::wave(_tmpImage, m_ie_par1.toDouble(), m_ie_par2.toDouble());
        break;
    }
    case IE_NONE:
    default:
        break;
    }

    _tmpPixmap.convertFromImage( _tmpImage );

    return _tmpPixmap;
}

void EmbeddedPictureObject::flip( bool /*horizontal*/ )
{
//     KP2DObject::flip( horizontal );
//     if ( horizontal )
//     {
//         switch ( mirrorType )
//         {
//             case PM_NORMAL:
//                 mirrorType = PM_HORIZONTAL;
//                 break;
//             case PM_HORIZONTAL:
//                 mirrorType = PM_NORMAL;
//                 break;
//             case PM_VERTICAL:
//                 mirrorType = PM_HORIZONTALANDVERTICAL;
//                 break;
//             case PM_HORIZONTALANDVERTICAL:
//                 mirrorType = PM_VERTICAL;
//                 break;
//         }
//     }
//     else
//     {
//         switch ( mirrorType )
//         {
//             case PM_NORMAL:
//                 mirrorType = PM_VERTICAL;
//                 break;
//             case PM_HORIZONTAL:
//                 mirrorType = PM_HORIZONTALANDVERTICAL;
//                 break;
//             case PM_VERTICAL:
//                 mirrorType = PM_NORMAL;
//                 break;
//             case PM_HORIZONTALANDVERTICAL:
//                 mirrorType = PM_HORIZONTAL;
//                 break;
//         }
//     }
}
