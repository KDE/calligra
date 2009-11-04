/* This file is part of the KDE project
  Copyright (C) 2007 Dag Andersen <danders@get2net.dk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
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

#include "kptdependencyeditor.h"

#include "kptglobal.h"
#include "kptitemmodelbase.h"
#include "kptcommand.h"
#include "kptproject.h"
#include "kptrelation.h"
#include "kptschedule.h"

#include <QGraphicsSceneMouseEvent>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QPainterPath>
#include <QPalette>
#include <QStyleOptionViewItem>
#include <QVBoxLayout>
#include <QWidget>
#include <QTimer>
#include <QKeyEvent>

#include <kmenu.h>
#include <kicon.h>
#include <kaction.h>
#include <kglobal.h>
#include <klocale.h>
#include <ktoggleaction.h>
#include <kactionmenu.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kaccelgen.h>
#include <kactioncollection.h>

namespace KPlato
{

//----------------------
DependecyViewPrintingDialog::DependecyViewPrintingDialog( ViewBase *parent, DependencyView *view )
    : PrintingDialog( parent ),
    m_depview( view )
{
}

int DependecyViewPrintingDialog::documentLastPage() const
{
    //TODO
    return documentFirstPage();
}

void DependecyViewPrintingDialog::printPage( int page, QPainter &painter )
{
    painter.save();

    QRect hRect = headerRect();
    QRect fRect = footerRect();
    QRect pageRect = printer().pageRect();

    kDebug()<<pageRect<<hRect<<fRect;

    painter.translate( pageRect.topLeft() );

    painter.setClipping( true );

    paintHeaderFooter( painter, printingOptions(), page, *(m_depview->project()) );

    int gap = 8;
    int pageHeight = pageRect.height();
    if ( hRect.isValid() ) {
        pageHeight -= ( hRect.height() + gap );
    }
    if ( fRect.isValid() ) {
        pageHeight -= ( fRect.height() + gap );
    }
    painter.translate( 0, hRect.height() + gap );
    
    QRect r( 0, 0, pageRect.width(), pageHeight );
    m_depview->itemScene()->render( &painter, r );
    
    painter.restore();
}


DependencyLinkItemBase::DependencyLinkItemBase( QGraphicsItem *parent )
    : QGraphicsPathItem( parent ),
    m_editable( false ),
    predItem( 0 ),
    succItem( 0 ),
    relation( 0 ),
    m_arrow( new QGraphicsPathItem( this ) )
{
}

DependencyLinkItemBase::DependencyLinkItemBase( DependencyNodeItem *predecessor, DependencyNodeItem *successor, Relation *rel, QGraphicsItem *parent )
    : QGraphicsPathItem( parent ),
    m_editable( false ),
    predItem( predecessor ),
    succItem( successor ),
    relation( rel ),
    m_arrow( new QGraphicsPathItem( this ) )
{
}

DependencyLinkItemBase::~DependencyLinkItemBase()
{
}

DependencyScene *DependencyLinkItemBase::itemScene() const
{
    return static_cast<DependencyScene*>( scene() );
}

void DependencyLinkItemBase::createPath( const QPointF &sp, int starttype, const QPointF &ep, int endtype )
{
    //if ( predItem && succItem ) kDebug()<<predItem->text()<<" ->"<<succItem->text()<<" visible="<<isVisible();
    if ( ! isVisible() ) {
        return;
    }
    qreal hgap = itemScene()->horizontalGap();
    
    bool up = sp.y() > ep.y();
    bool right = sp.x() < ep.x();
    bool same = sp.x() == ep.x();

    QPainterPath link( sp );
    qreal x = sp.x();
    qreal y = sp.y();
    if ( right && starttype == DependencyNodeItem::Finish) {
        x = ep.x();
        x += endtype == DependencyNodeItem::Start ? - hgap/2 - 6 : hgap/2 - 6;
        link.lineTo( x, y );
        x += 6;
        QPointF cp( x, y );
        y += up ? -6 : +6;
        link.quadTo( cp, QPointF( x, y ) );
        y = up ? ep.y() + 6 : ep.y() - 6;
        link.lineTo( x, y );
        y = ep.y();
        cp = QPointF( x, y );
        x += endtype == DependencyNodeItem::Start ? 6 : -6;
        link.quadTo(  cp, QPointF( x, y ) );
    } else if ( right && starttype == DependencyNodeItem::Start ) {
        x = sp.x() - hgap/2 + 6;
        link.lineTo( x, y );
        x -= 6;
        QPointF cp( x, y );
        y += up ? -6 : +6;
        link.quadTo( cp, QPointF( x, y ) );
        y = up ? ep.y() + 6 : ep.y() - 6;
        link.lineTo( x, y );
        y = ep.y();
        cp = QPointF( x, y );
        x += endtype == DependencyNodeItem::Start ? 6 : -6;
        link.quadTo(  cp, QPointF( x, y ) );
    } else if ( same ) {
        x = ep.x();
        x += endtype == DependencyNodeItem::Start ? - hgap/2 + 6 : hgap/2 - 6;
        link.lineTo( x, y );
        x += endtype == DependencyNodeItem::Start ? -6 : +6;
        QPointF cp( x, y );
        y += up ? -6 : 6;
        link.quadTo( cp, QPointF( x, y ) );
        y = up ? ep.y() + 6 : ep.y() - 6;
        link.lineTo( x, y );
        y = ep.y();
        cp = QPointF( x, y );
        if ( endtype == DependencyNodeItem::Start ) {
            x += 6; 
        } else {
            x -= 6;
        }
        link.quadTo( cp, QPointF( x, y ) );
    } else {
        x = ep.x();
        x += endtype == DependencyNodeItem::Start ? - hgap/2 + 6 : hgap/2 + 6;
        link.lineTo( x, y );
        x -= 6;
        QPointF cp( x, y );
        y += up ? -6 : 6;
        link.quadTo( cp, QPointF( x, y ) );
        y = up ? ep.y() + 6 : ep.y() - 6;
        link.lineTo( x, y );
        y = ep.y();
        cp = QPointF( x, y );
        x += endtype == DependencyNodeItem::Start ? 6 : -6;
        link.quadTo( cp, QPointF( x, y ) );
    }
    link.lineTo( ep );

    setPath( link );
    
    QPainterPath arrow;
    x = endtype == DependencyNodeItem::Start ? -6 : 6;
    arrow.moveTo( ep );
    arrow.lineTo( ep.x() + x, ep.y() - 3 );
    arrow.lineTo( ep.x() + x, ep.y() + 3 );
    arrow.lineTo( ep );
    m_arrow->setPath( arrow );
    m_arrow->show();
}

//--------------------------------
DependencyLinkItem::DependencyLinkItem( DependencyNodeItem *predecessor, DependencyNodeItem *successor, Relation *rel, QGraphicsItem *parent )
    : DependencyLinkItemBase( predecessor, successor, rel, parent )
{
    setZValue( 100.0 );
    setAcceptsHoverEvents( true );
    //kDebug()<<predecessor->text()<<"("<<predecessor->column()<<") -"<<successor->text();
    predItem->addChildRelation( this );
    succItem->addParentRelation( this );
    succItem->setColumn();

    m_arrow->setBrush( Qt::black );

    m_pen = pen();
}

DependencyLinkItem::~DependencyLinkItem()
{
    if ( predItem ) {
        predItem->takeChildRelation( this );
    }
    if ( succItem ) {
        succItem->takeParentRelation( this );
    }
}

int DependencyLinkItem::newChildColumn() const
{
    int col = predItem->column();
    if ( relation->type() == Relation::FinishStart ) {
        ++col;
    }
    //kDebug()<<"new col="<<col;
    return col;
}

void DependencyLinkItem::setItemVisible( bool show )
{
    setVisible( show && predItem->isVisible() && succItem->isVisible() );
}

void DependencyLinkItem::createPath()
{
    setVisible( predItem->isVisible() && succItem->isVisible() );
    if ( ! isVisible() ) {
        //kDebug()<<"Visible="<<isVisible()<<":"<<predItem->node()->name()<<" -"<<succItem->node()->name();
        return;
    }
    QPointF sp = startPoint();
    QPointF ep = endPoint();
    int stype = 0, etype = 0;
    switch ( relation->type() ) {
        case Relation::StartStart:
            stype = DependencyNodeItem::Start; etype = DependencyNodeItem::Start;
            break;
        case Relation::FinishStart:
            stype = DependencyNodeItem::Finish; etype = DependencyNodeItem::Start;
            break;
        case Relation::FinishFinish:
            stype = DependencyNodeItem::Finish; etype = DependencyNodeItem::Finish;
            break;
        default:
            break;
    }
    DependencyLinkItemBase::createPath( sp, stype, ep, etype );
}

QPointF DependencyLinkItem::startPoint() const
{
    if ( relation->type() == Relation::StartStart ) {
        return predItem->connectorPoint( DependencyNodeItem::Start );
    }
    return predItem->connectorPoint( DependencyNodeItem::Finish );
}

QPointF DependencyLinkItem::endPoint() const
{
    if ( relation->type() == Relation::FinishFinish ) {
        return succItem->connectorPoint( DependencyNodeItem::Finish );
    }
    return succItem->connectorPoint( DependencyNodeItem::Start );
}

void DependencyLinkItem::hoverEnterEvent( QGraphicsSceneHoverEvent * /*event*/ )
{
    setZValue( zValue() + 1 );
    setPen( QPen( Qt::black, 2 ) );
    m_arrow->setPen( pen() );
    update();
}

void DependencyLinkItem::hoverLeaveEvent( QGraphicsSceneHoverEvent * /*event*/ )
{
    resetHooverIndication();
}

void DependencyLinkItem::resetHooverIndication()
{
    setZValue( zValue() - 1 );
    setPen( m_pen );
    m_arrow->setPen( m_pen );
    update();
}

void DependencyLinkItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //kDebug();
    QGraphicsItem::GraphicsItemFlags f = flags();
    if ( isEditable() && itemScene()->connectionMode() ) {
        itemScene()->clearConnection();
        setFlags( f & ~QGraphicsItem::ItemIsSelectable );
    }
    QGraphicsPathItem::mousePressEvent( event );
    if ( f != flags() ) {
        setFlags( f );
    }
}

//--------------------
DependencyCreatorItem::DependencyCreatorItem( QGraphicsItem *parent )
    : DependencyLinkItemBase( parent ),
    predConnector( 0 ),
    succConnector( 0 ),
    m_editable( false )
{
    setZValue( 1000.0 );
    clear();
    setPen( QPen( Qt::blue, 2 ) );
    m_arrow->setBrush( Qt::blue );
    m_arrow->setPen( QPen( Qt::blue, 2 ) );
}

void DependencyCreatorItem::clear()
{
    hide();
    if ( predConnector && predConnector->parentItem() ) {
        static_cast<DependencyNodeItem*>( predConnector->parentItem() )->setConnectorHoverMode( true );
    } else if ( succConnector && succConnector->parentItem() ) {
        static_cast<DependencyNodeItem*>( succConnector->parentItem() )->setConnectorHoverMode( true );
    }
    predConnector = 0;
    succConnector = 0;
    setPath( QPainterPath() );
    m_arrow->setPath( QPainterPath() );
}

void DependencyCreatorItem::setPredConnector( DependencyConnectorItem *item )
{
    predConnector = item;
    //static_cast<DependencyNodeItem*>( item->parentItem() )->setConnectorHoverMode( false );
}

void DependencyCreatorItem::setSuccConnector( DependencyConnectorItem *item )
{
    succConnector = item;
}

void DependencyCreatorItem::createPath()
{
    if ( predConnector == 0 ) {
        return;
    }
    if ( succConnector == 0 ) {
        return;
    }
    QPointF sp = predConnector->connectorPoint();
    QPointF ep = succConnector->connectorPoint();
    DependencyLinkItemBase::createPath( sp, predConnector->ctype(), ep, succConnector->ctype() );
}

void DependencyCreatorItem::createPath( const QPointF &ep )
{
    m_arrow->hide();
    if ( succConnector ) {
        return createPath();
    }
    if ( predConnector == 0 ) {
        return;
    }
    QPointF sp = predConnector->connectorPoint();
    
    QPainterPath link( sp );
    link.lineTo( ep );
    setPath( link );
    
}

QPointF DependencyCreatorItem::startPoint() const
{
    return predConnector == 0 ? QPointF() : predConnector->connectorPoint();
}

QPointF DependencyCreatorItem::endPoint() const
{
    return succConnector == 0 ? QPointF() : succConnector->connectorPoint();
}

//--------------------
DependencyConnectorItem::DependencyConnectorItem( DependencyNodeItem::ConnectorType type, DependencyNodeItem *parent )
    : QGraphicsRectItem( parent ),
    m_ctype( type ),
    m_editable( false )
{
    setCursor( Qt::UpArrowCursor );
    setAcceptsHoverEvents( true );
    setZValue( 500.0 );

    setFlag( QGraphicsItem::ItemIsFocusable );
}

DependencyScene *DependencyConnectorItem::itemScene() const
{
    return static_cast<DependencyScene*>( scene() );
}

DependencyNodeItem *DependencyConnectorItem::nodeItem() const
{
    return static_cast<DependencyNodeItem*>( parentItem() );
}

Node *DependencyConnectorItem::node() const
{
    return static_cast<DependencyNodeItem*>( parentItem() )->node();
}

QPointF DependencyConnectorItem::connectorPoint() const
{
    QRectF r = rect();
    return QPointF( r.x()+r.width(), r.y() + r.height()/2 );
}

void DependencyConnectorItem::hoverEnterEvent( QGraphicsSceneHoverEvent * /*event*/ )
{
    //qDebug()<<"DependencyConnectorItem::hoverEnterEvent:"<<node()->name();
    itemScene()->connectorEntered( this, true );
}

void DependencyConnectorItem::hoverLeaveEvent( QGraphicsSceneHoverEvent * /*event*/ )
{
    //qDebug()<<"DependencyConnectorItem::hoverLeaveEvent:"<<node()->name();
    itemScene()->connectorEntered( this, false );
}

void DependencyConnectorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"DependencyConnectorItem::mousePressEvent:"<<node()->name()<<isEditable();
    if ( ! isEditable() ) {
        event->ignore();
        return;
    }
    if (event->button() == Qt::LeftButton ) {
        m_mousePressPos = event->pos();
    } else {
        event->ignore();
    }
}

void DependencyConnectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug()<<"DependencyConnectorItem::mouseReleaseEvent:"<<node()->name()<<m_mousePressPos;
    m_mousePressPos = QPointF();
    if (event->button() != Qt::LeftButton ) { 
        event->ignore();
        return;
    }
    if ( rect().contains( event->scenePos() ) ) {
        // user clicked on this item
        //qDebug()<<"DependencyConnectorItem::mouseReleaseEvent:"<<node()->name()<<"click";
        bool multiSelect = (event->modifiers() & Qt::ControlModifier) != 0;
        if (multiSelect) {
            itemScene()->multiConnectorClicked( this );
        } else {
            itemScene()->singleConnectorClicked( this );
        }
        return;
    }
    QGraphicsItem *item = 0;
    foreach ( QGraphicsItem *i, itemScene()->items( event->scenePos() ) ) {
        if ( i->type() == DependencyConnectorItem::Type ) {
            item = i;
            break;
        }
    }
    if ( item == 0 || item == itemScene()->fromItem() ) {
        itemScene()->setFromItem( 0 );
        return;
    }
    itemScene()->singleConnectorClicked( static_cast<DependencyConnectorItem*>( item ) );
}

void DependencyConnectorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton ) {
        //qDebug()<<"DependencyConnectorItem::mouseMoveEvent:"<<node()->name()<<event->pos();
        if ( ! m_mousePressPos.isNull() ) {
            itemScene()->setFromItem( this );
            m_mousePressPos = QPointF();
        }
        QGraphicsItem *item = 0;
        foreach ( QGraphicsItem *i, itemScene()->items( event->scenePos() ) ) {
            if ( i->type() == DependencyConnectorItem::Type ) {
                item = i;
                break;
            }
        }
        if ( item != this ) {
            itemScene()->connectorEntered( this, false );
        }
        if ( item != 0 ) {
            itemScene()->connectorEntered( static_cast<DependencyConnectorItem*>( item ), true );
        }
    } else {
        event->ignore();
    }
}

void DependencyConnectorItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    //kDebug();
    QStyleOptionGraphicsItem opt( *option );
    if ( itemScene()->fromItem() == this ) {
        opt.state |= QStyle::State_Selected;
    }
    if ( opt.state & ( QStyle::State_Selected | QStyle::State_HasFocus ) ) {
        painter->save();
        QBrush b = brush();
        QPen p = pen();
        if (opt.state & QStyle::State_Selected) {
            QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled
                    ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
                cg = QPalette::Inactive;
    
            p.setStyle( Qt::NoPen );
            QColor col = option->palette.brush(cg, QPalette::Highlight).color();
            QLinearGradient g( 0.0, rect().top(), 0.0, rect().bottom() );
            g.setColorAt( 0.0, col.lighter( 125 ) );
            g.setColorAt( 1.0, col.lighter( 60 ) );
            b = QBrush( g );
        }
        if (opt.state & QStyle::State_HasFocus) {
            QPalette::ColorGroup cg = option->state & QStyle::State_Enabled
                    ? QPalette::Active : QPalette::Disabled;
            if (cg == QPalette::Active && !(option->state & QStyle::State_Active))
                cg = QPalette::Inactive;

            p.setWidth( 2 );
            p.setStyle( Qt::DotLine );
            if ( option->state & QStyle::State_Selected ) {
                p.setColor( option->palette.color( cg, QPalette::HighlightedText ) );
            } else {
                p.setColor( option->palette.color( cg, QPalette::Highlight ) );
            }
        }
        painter->setPen( p );
        painter->setBrush( b );
        painter->drawRect( rect() );
        painter->restore();
    }
    QRectF r = rect();
    if ( ctype() == DependencyNodeItem::Start ) {
        r.setRect( r.right() - (r.width()/2.0) + 1.0, r.y() + ( r.height() * 0.33 ), r.width() / 2.0, r.height() * 0.33 );
    } else {
        r.setRect( r.right() - (r.width()/2.0) - 1.0, r.y() + ( r.height() * 0.33 ), r.width() / 2.0, r.height() * 0.33 );
    }
    painter->fillRect( r, Qt::black );
}

QList<DependencyLinkItem*> DependencyConnectorItem::predecessorItems() const
{
    return nodeItem()->predecessorItems( m_ctype );
}

QList<DependencyLinkItem*> DependencyConnectorItem::successorItems() const
{
    return nodeItem()->successorItems( m_ctype );
}

//--------------------
DependencyNodeItem::DependencyNodeItem( Node *node, DependencyNodeItem *parent )
    : QGraphicsRectItem( parent ),
    m_node( node ),
    m_parent( 0 ),
    m_editable( false )
{
    setAcceptsHoverEvents( true );
    setZValue( 400.0 );
    setParentItem( parent );
    m_start = new DependencyConnectorItem( DependencyNodeItem::Start, this );
    m_finish = new DependencyConnectorItem( DependencyNodeItem::Finish, this );
    
    m_text = new QGraphicsTextItem( this );
    m_textFont = m_text->font();
    m_textFont.setPointSize( 10 );
    m_text->setFont( m_textFont );
    setText();
    
    setFlags( QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable );
    
    m_symbol = new DependencyNodeSymbolItem( this );
    m_symbol->setZValue( zValue() + 10.0 );
    setSymbol();
}

DependencyNodeItem::~DependencyNodeItem()
{
    qDeleteAll( m_childrelations );
    qDeleteAll( m_parentrelations );
    //qDeleteAll( m_children );
}

void DependencyNodeItem::setText()
{
    m_text->setPlainText( m_node == 0 ? QString() : QString( "%1  %2").arg( m_node->wbsCode() ).arg(m_node->name() ) ); 
}

DependencyScene *DependencyNodeItem::itemScene() const
{
    return static_cast<DependencyScene*>( scene() );
}
void DependencyNodeItem::setSymbol()
{
    m_symbol->setSymbol( m_node->type(), itemScene()->symbolRect() );
}

QPointF DependencyNodeItem::connectorPoint( DependencyNodeItem::ConnectorType type ) const
{
    QRectF r;
    if ( type == Start ) {
        return m_start->connectorPoint();
    }
    return m_finish->connectorPoint();
}

void DependencyNodeItem::setConnectorHoverMode( bool mode )
{
    m_start->setAcceptsHoverEvents( mode );
    m_finish->setAcceptsHoverEvents( mode );
}

void DependencyNodeItem::setParentItem( DependencyNodeItem *parent )
{
    if ( m_parent ) {
        m_parent->takeChild( this );
    }
    m_parent = parent;
    if ( m_parent ) {
        m_parent->addChild( this );
    }
}

void DependencyNodeItem::setExpanded( bool mode )
{
    foreach ( DependencyNodeItem *ch, m_children ) {
        itemScene()->setItemVisible( ch, mode );
        ch->setExpanded( mode );
    }
}

void DependencyNodeItem::setItemVisible( bool show )
{
    setVisible( show );
    //kDebug()<<isVisible()<<","<<node()->name();
    foreach ( DependencyLinkItem *i, m_parentrelations ) {
        i->setItemVisible( show );
    }
    foreach ( DependencyLinkItem *i, m_childrelations ) {
        i->setItemVisible( show );
    }
}

DependencyNodeItem *DependencyNodeItem::takeChild( DependencyNodeItem *ch )
{
    int i = m_children.indexOf( ch );
    if ( i == -1 ) {
        return 0;
    }
    return m_children.takeAt( i );
}

void DependencyNodeItem::setRectangle( const QRectF &rect )
{
    //kDebug()<<text()<<":"<<rect;
    setRect( rect );

    qreal connection = static_cast<DependencyScene*>( scene() )->connectorWidth();
    m_start->setRect( rect.x() + connection, rect.y(), -connection, rect.height() );
    m_finish->setRect( rect.right() - connection, rect.y(), connection, rect.height() );
    
    m_text->setPos( m_finish->rect().right() + 2.0, itemScene()->gridY(  row() ) );
    
    m_symbol->setPos( rect.topLeft() + QPointF( connection, 0 ) + QPointF( 2.0, 2.0 ) );
}

void DependencyNodeItem::moveToY( qreal y )
{
    QRectF r = rect();
    r. moveTop( y );
    setRectangle( r );
    //kDebug()<<text()<<" move to="<<y<<" new pos:"<<rect();
    foreach ( DependencyLinkItem *i, m_parentrelations ) {
        i->createPath();
    }
    foreach ( DependencyLinkItem *i, m_childrelations ) {
        i->createPath();
    }
}

void DependencyNodeItem::setRow( int row )
{
    moveToY( itemScene()->itemY( row ) );
}

int DependencyNodeItem::row() const
{
    return itemScene()->row( rect().y() );
}

void DependencyNodeItem::moveToX( qreal x )
{
    QRectF r = rect();
    r. moveLeft( x );
    setRectangle( r );
    //kDebug()<<m_text->toPlainText()<<" to="<<x<<" new pos:"<<rect();
    foreach ( DependencyLinkItem *i, m_parentrelations ) {
        i->createPath();
    }
    foreach ( DependencyLinkItem *i, m_childrelations ) {
        i->createPath();
    }
}

void DependencyNodeItem::setColumn()
{
    int col = m_parent == 0 ? 0 : m_parent->column() + 1;
    //kDebug()<<this<<text();
    foreach ( DependencyLinkItem *i, m_parentrelations ) {
        col = qMax( col, i->newChildColumn() );
    }
    if ( col != column() ) {
        setColumn( col );
        foreach ( DependencyLinkItem *i, m_childrelations ) {
            i->succItem->setColumn();
        }
        //kDebug()<<m_children.count()<<"Column="<<column()<<","<<text();
        foreach ( DependencyNodeItem *i, m_children ) {
            i->setColumn();
        }
    }
}

void DependencyNodeItem::setColumn( int col )
{
    moveToX( itemScene()->itemX( col ) );
}

int DependencyNodeItem::column() const
{
    return itemScene()->column( rect().x() );
}

DependencyLinkItem *DependencyNodeItem::takeParentRelation( DependencyLinkItem *r )
{
    int i = m_parentrelations.indexOf( r );
    if ( i == -1 ) {
        return 0;
    }
    DependencyLinkItem *dep = m_parentrelations.takeAt( i );
    setColumn();
    return dep;
}

DependencyLinkItem *DependencyNodeItem::takeChildRelation( DependencyLinkItem *r )
{
    int i = m_childrelations.indexOf( r );
    if ( i == -1 ) {
        return 0;
    }
    return m_childrelations.takeAt( i );
}

void DependencyNodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    kDebug();
    QGraphicsItem::GraphicsItemFlags f = flags();
    if ( itemScene()->connectionMode() ) {
        itemScene()->clearConnection();
        setFlags( f & ~QGraphicsItem::ItemIsSelectable );
    }
    QGraphicsRectItem::mousePressEvent( event );
    if ( f != flags() ) {
        setFlags( f );
    }
}

void DependencyNodeItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget * )
{
    QLinearGradient g( 0.0, rect().top(), 0.0, rect().bottom() );
    g.setColorAt( 0.0, option->palette.color( QPalette::Midlight ) );
    g.setColorAt( 1.0, option->palette.color( QPalette::Dark ) );
    QBrush b( g );
    painter->setBrush( b );
    painter->setPen( QPen( Qt::NoPen ) );
    painter->drawRect( rect() );

    QRectF r = rect().adjusted( -m_start->rect().width(), 0.0, -m_finish->rect().width(), 0.0 );
    if (option->state & ( QStyle::State_Selected | QStyle::State_HasFocus ) ) {
        if (option->state & QStyle::State_Selected) {
            QPalette::ColorGroup cg = option->state & QStyle::State_Enabled
                    ? QPalette::Normal : QPalette::Disabled;
            if (cg == QPalette::Normal && !(option->state & QStyle::State_Active))
                cg = QPalette::Inactive;
    
            QColor col = option->palette.brush(cg, QPalette::Highlight).color();
            g.setColorAt( 0.0, col.lighter( 125 ) );
            g.setColorAt( 1.0, col.lighter( 60 ) );
            b = QBrush( g );
        } else b = QBrush( Qt::NoBrush );
        if (option->state & QStyle::State_HasFocus) {
            QPalette::ColorGroup cg = option->state & QStyle::State_Enabled
                    ? QPalette::Active : QPalette::Disabled;
            if (cg == QPalette::Active && !(option->state & QStyle::State_Active))
                cg = QPalette::Inactive;
            
            QPen p( Qt::DotLine );
            p.setWidth( 2 );
            if ( option->state & QStyle::State_Selected ) {
                p.setColor( option->palette.color( cg, QPalette::HighlightedText ) );
            } else {
                p.setColor( option->palette.color( cg, QPalette::Highlight ) );
            }
            painter->setPen( p );
        }
        painter->setBrush( b );
        painter->drawRect( r );
    }
}

DependencyConnectorItem *DependencyNodeItem::connectorItem( ConnectorType ctype ) const
{
    switch ( ctype ) {
        case Start: return m_start;
        case Finish: return m_finish;
        default: break;
    }
    return 0;
}

QList<DependencyLinkItem*> DependencyNodeItem::predecessorItems( ConnectorType ctype ) const
{
    QList<DependencyLinkItem*> lst;
    foreach ( DependencyLinkItem *i, m_parentrelations ) {
        if ( ctype == Start && ( i->relation->type() == Relation::StartStart || i->relation->type() == Relation::FinishStart ) ) {
            lst << i;
        }
        if ( ctype == Finish && i->relation->type() == Relation::FinishFinish ) {
            lst << i;
        }
    }
    return lst;
}

QList<DependencyLinkItem*> DependencyNodeItem::successorItems( ConnectorType ctype ) const
{
    QList<DependencyLinkItem*> lst;
    foreach ( DependencyLinkItem *i, m_childrelations ) {
        if ( ctype == Start && i->relation->type() == Relation::StartStart ) {
            lst << i;
        }
        if ( ctype == Finish && ( i->relation->type() == Relation::FinishFinish || i->relation->type() == Relation::FinishStart ) ) {
            lst << i;
        }
    }
    return lst;
}

//--------------------
void DependencyNodeSymbolItem::setSymbol( int type, const QRectF &rect )
{
    KDGantt::ItemType itemtype = KDGantt::TypeNone;
    QPainterPath p;
    switch ( type ) {
        case Node::Type_Summarytask:
            itemtype = KDGantt::TypeSummary;
            p.moveTo( rect.topLeft() );
            p.lineTo( rect.topRight() );
            p.lineTo( rect.left() + rect.width() / 2.0, rect.bottom() );
            p.closeSubpath();
            break;
        case Node::Type_Task:
            itemtype = KDGantt::TypeTask;
            p.moveTo( rect.topLeft() );
            p.lineTo( rect.topRight() );
            p.lineTo( rect.bottomRight() );
            p.lineTo( rect.bottomLeft() );
            p.closeSubpath();
            break;
        case Node::Type_Milestone:
            itemtype = KDGantt::TypeEvent;
            p.moveTo( rect.left() + ( rect.width() / 2.0 ), rect.top() );
            p.lineTo( rect.right(), rect.top() + ( rect.height() / 2.0 ) );
            p.lineTo( rect.left() + ( rect.width() / 2.0 ), rect.bottom() );
            p.lineTo( rect.left(), rect.top() + ( rect.height() / 2.0 ) );
            p.closeSubpath();
            break;
        default: 
            break;
    }
    setPath( p );
    setBrush( m_delegate.defaultBrush( itemtype ) );
}

//--------------------
DependencyScene::DependencyScene( QWidget *parent )
    : QGraphicsScene( parent ),
    m_model( 0 ),
    m_readwrite( false )
{
    m_connectionitem = new DependencyCreatorItem();
    addItem( m_connectionitem );
    //kDebug();
    m_connectionitem->hide();
}

DependencyScene::~DependencyScene()
{
    //kDebug()<<" DELETED";
    clearScene();
}

void DependencyScene::setFromItem( DependencyConnectorItem *item )
{
    DependencyConnectorItem *old = fromItem();
    m_connectionitem->clear();
    if ( old ) {
        old->update();
    }
    if ( item ) {
        foreach ( QGraphicsItem *i, items() ) {
            if ( i != m_connectionitem && i->type() != DependencyConnectorItem::Type ) {
                i->setAcceptsHoverEvents( false );
                if ( i->type() == DependencyLinkItem::Type ) {
                    static_cast<DependencyLinkItem*>( i )->resetHooverIndication();
                }
            }
        }
        item->setCursor( Qt::UpArrowCursor );
        m_connectionitem->setPredConnector( item );
        m_connectionitem->show();
        item->update();
    } else {
        foreach ( QGraphicsItem *i, items() ) {
            if ( i != m_connectionitem && i->type() != DependencyConnectorItem::Type )
                i->setAcceptsHoverEvents( true );
        }
    }
}

bool DependencyScene::connectionIsValid( DependencyConnectorItem *pred, DependencyConnectorItem *succ )
{
    if ( pred->ctype() == DependencyNodeItem::Start && succ->ctype() == DependencyNodeItem::Finish ) {
        return false;
    }
    Node *par = static_cast<DependencyNodeItem*>( pred->parentItem() )->node();
    Node *ch = static_cast<DependencyNodeItem*>( succ->parentItem() )->node();
    return m_project->linkExists( par, ch ) || m_project->legalToLink( par, ch );
}

void DependencyScene::connectorEntered( DependencyConnectorItem *item, bool entered )
{
    //kDebug()<<entered;
    //TODO special cursor
    item->setCursor( Qt::UpArrowCursor );
    if ( ! entered ) {
        // when we leave a connector we don't have a successor
        m_connectionitem->setSuccConnector( 0 );
        return;
    }
    if ( m_connectionitem->predConnector == item ) {
        // when inside the predecessor, clicking is allowed (deselects connector)
        item->setCursor( Qt::UpArrowCursor );
        return;
    }
    if ( ! m_connectionitem->isVisible() ) {
        // we are not in connection mode
        return;
    }
    if ( m_connectionitem->predConnector == 0 ) {
        // nothing we can do if we don't have a predecessor (shouldn't happen)
        return;
    }
    if ( item->parentItem() == m_connectionitem->predConnector->parentItem() ) {
        // not allowed to connect to the same node
        item->setCursor( Qt::ForbiddenCursor );
        return;
    }
    if ( ! ( connectionIsValid( m_connectionitem->predConnector, item ) ) ) {
        // invalid connection (circular dependency, connecting to parent node, etc)
        item->setCursor( Qt::ForbiddenCursor );
        return;
    }
    m_connectionitem->setSuccConnector( item );
    m_connectionitem->createPath();
}

void DependencyScene::drawBackground ( QPainter *painter, const QRectF &rect )
{
    QGraphicsScene::drawBackground( painter, rect );
    QStyleOptionViewItemV3 opt;
    QBrush br( opt.palette.brush( QPalette::AlternateBase ) );
    int first = row( rect.y() );
    int last = row( rect.bottom() );
    for ( int r = first; r <= last; ++r ) {
        if ( r % 2 == 1 ) {
            qreal oy = gridY( r );
            QRectF rct( rect.x(), oy, rect.width(), gridHeight() );
            painter->fillRect( rct, br );
            //kDebug()<<r<<": oy="<<oy<<""<<rct;
        }
    }
}

QList<QGraphicsItem*> DependencyScene::itemList( int type ) const
{
    QList<QGraphicsItem*> lst;
    foreach ( QGraphicsItem *i, items() ) {
        if ( i->type() == type ) {
            lst << i;
        }
    }
    return lst;
}

void DependencyScene::clearScene()
{
    QList<QGraphicsItem*> its, deps;
    foreach ( QGraphicsItem *i, items() ) {
        if ( i->type() == DependencyNodeItem::Type && i->parentItem() == 0 ) {
            its << i;
        } else if ( i->type() == DependencyLinkItem::Type ) {
            deps << i;
        }
    }
    qDeleteAll( deps );
    qDeleteAll( its );
    removeItem( m_connectionitem );
    qDeleteAll( items() );
    m_connectionitem->clear();
    addItem( m_connectionitem );
    //kDebug();
}

QList<DependencyNodeItem*> DependencyScene::removeChildItems( DependencyNodeItem *item )
{
    QList<DependencyNodeItem*> lst;
    foreach ( DependencyNodeItem *i, item->children() ) {
        m_allItems.removeAt( m_allItems.indexOf( i ) );
        lst << i;
        lst += removeChildItems( i );
    }
    return lst;
}

void DependencyScene::moveItem( DependencyNodeItem *item, const QList<Node*> &lst )
{
    //kDebug()<<item->text();
    int idx = m_allItems.indexOf( item );
    int ndx = lst.indexOf( item->node() );
    Q_ASSERT( idx != -1 && ndx != -1 );
    Node *oldParent = item->parentItem() == 0 ? 0 : item->parentItem()->node();
    Node *newParent = item->node()->parentNode();
    if ( newParent == m_project ) {
        newParent = 0;
    } else kDebug()<<newParent->name()<<newParent->level();
    if ( idx != ndx || oldParent != newParent ) {
        // If I have childeren, these must be moved too.
        QList<DependencyNodeItem*> items = removeChildItems( item );
        
        m_allItems.removeAt( idx );
        m_allItems.insert( ndx, item );
        item->setParentItem( m_allItems.value( lst.indexOf( newParent ) ) );
        item->setColumn();
        //kDebug()<<item->text()<<":"<<idx<<"->"<<ndx<<", "<<item->column()<<r;
        if ( ! items.isEmpty() ) {
            foreach ( DependencyNodeItem *i, items ) {
                m_allItems.insert( ++ndx, i );
                i->setColumn();
                //kDebug()<<i->text()<<": ->"<<ndx<<", "<<i->column()<<r;
            }
        }
    }
}

void DependencyScene::setItemVisible( DependencyNodeItem *item, bool show )
{
    //kDebug()<<"Visible count="<<m_visibleItems.count()<<" total="<<m_allItems.count();
    item->setItemVisible( show );
    int row = m_allItems.indexOf( item );
    if ( row == -1 ) {
        kDebug()<<"Unknown item!!";
        return;
    }
    if ( show && m_hiddenItems.values().contains( item ) ) {
        moveItem( item, m_project->flatNodeList() ); // might have been moved
    }
    m_hiddenItems.clear();
    m_visibleItems.clear();
    int viewrow = 0;
    for ( int i = 0; i < m_allItems.count(); ++i ) {
        DependencyNodeItem *itm = m_allItems[ i ];
        if ( itm->isVisible() ) {
            m_visibleItems.insert( i, itm );
            //kDebug()<<itm->text()<<":"<<i<<viewrow;
            itm->setRow( viewrow );
            ++viewrow;
        } else {
            m_hiddenItems.insert( i, itm );
        }
    }
}

DependencyNodeItem *DependencyScene::findPrevItem( Node *node )  const
{
    if ( node->numChildren() == 0 ) {
        return findItem( node );
    }
    return findPrevItem( node->childNodeIterator().last() );
}

DependencyNodeItem *DependencyScene::itemBefore( DependencyNodeItem *parent, Node *node )  const
{
    Node *sib = node->siblingBefore();
    DependencyNodeItem *bef = parent;
    if ( sib ) {
        bef = findPrevItem( sib );
    }
    return bef;
}

DependencyNodeItem *DependencyScene::createItem( Node *node )
{
    DependencyNodeItem *parent = findItem( node->parentNode() );
    DependencyNodeItem *after = itemBefore( parent, node );
    int i = m_allItems.count()-1;
    if ( after ) {
        i = m_allItems.indexOf( after );
        //kDebug()<<"after="<<after->node()->name()<<" pos="<<i;
    }
    DependencyNodeItem *item = new DependencyNodeItem( node, parent );
    if ( item->scene() != this ) {
        addItem( item );
    }
    item->setEditable( m_readwrite );
    item->startConnector()->setEditable( m_readwrite );
    item->finishConnector()->setEditable( m_readwrite );
    //kDebug()<<item->text()<<item;
    int col = item->nodeLevel();
    if ( parent ) {
        col += parent->column();
    }
    item->setRectangle( QRectF( itemX( col ), itemY(), itemWidth(), itemHeight() ) );
    m_allItems.insert( i+1, item );
    setItemVisible( item, true );
    return item;
}

DependencyLinkItem *DependencyScene::findItem( const Relation* rel ) const
{
    foreach ( QGraphicsItem *i, itemList( DependencyLinkItem::Type ) ) {
        if ( static_cast<DependencyLinkItem*>( i )->relation == rel ) {
            return static_cast<DependencyLinkItem*>( i );
        }
    }
    return 0;
}

DependencyLinkItem *DependencyScene::findItem( const DependencyConnectorItem *c1, const DependencyConnectorItem *c2, bool exact ) const
{
    DependencyNodeItem *n1 = c1->nodeItem();
    DependencyNodeItem *n2 = c2->nodeItem();
    foreach ( QGraphicsItem *i, itemList( DependencyLinkItem::Type ) ) {
        DependencyLinkItem *link = static_cast<DependencyLinkItem*>( i );
        if ( link->predItem == n1 && link->succItem == n2 ) {
            switch ( link->relation->type() ) {
                case Relation::StartStart:
                    if ( c1->ctype() == DependencyNodeItem::Start && c2->ctype() == DependencyNodeItem::Start ) {
                        return link;
                    }
                    break;
                case Relation::FinishStart:
                    if ( c1->ctype() == DependencyNodeItem::Finish && c2->ctype() == DependencyNodeItem::Start ) {
                        return link;
                    }
                    break;
                case Relation::FinishFinish:
                    if ( c1->ctype() == DependencyNodeItem::Finish && c2->ctype() == DependencyNodeItem::Finish ) {
                        return link;
                    }
                    break;
                default:
                    break;
            }
            return 0;
        }
        if ( link->predItem == n2 && link->succItem == n1 ) {
            if ( exact ) {
                return 0;
            }
            switch ( link->relation->type() ) {
                case Relation::StartStart:
                    if ( c2->ctype() == DependencyNodeItem::Start && c1->ctype() == DependencyNodeItem::Start ) {
                        return link;
                    }
                    break;
                case Relation::FinishStart:
                    if ( c1->ctype() == DependencyNodeItem::Finish && c1->ctype() == DependencyNodeItem::Start ) {
                        return link;
                    }
                    break;
                case Relation::FinishFinish:
                    if ( c1->ctype() == DependencyNodeItem::Finish && c1->ctype() == DependencyNodeItem::Finish ) {
                        return link;
                    }
                    break;
                default:
                    break;
            }
            return 0;
        }
    }
    return 0;
}

DependencyNodeItem *DependencyScene::findItem( const Node *node ) const
{
    foreach ( QGraphicsItem *i, itemList( DependencyNodeItem::Type ) ) {
        if ( static_cast<DependencyNodeItem*>( i )->node() == node ) {
            return static_cast<DependencyNodeItem*>( i );
        }
    }
    return 0;
}

void DependencyScene::createLinks()
{
    foreach ( DependencyNodeItem *i, m_allItems ) {
        createLinks( i );
    }
}
void DependencyScene::createLinks( DependencyNodeItem *item )
{
    foreach ( Relation *rel, item->node()->dependChildNodes() ) {
        createLink( item, rel );
    }
}
void DependencyScene::createLink( DependencyNodeItem *parent, Relation *rel )
{
    DependencyNodeItem *child = findItem( rel->child() );
    if ( parent == 0 || child == 0 ) {
        return;
    }
    DependencyLinkItem *dep = new DependencyLinkItem( parent, child, rel );
    dep->setEditable( m_readwrite );
    addItem( dep );
    //kDebug();
    dep->createPath();
}

void DependencyScene::mouseMoveEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    if ( m_connectionitem->isVisible() ) {
        int x = qMin( qMax( sceneRect().left() + 2, mouseEvent->scenePos().x() ), sceneRect().right() - 4 );
        int y = qMin( qMax( sceneRect().top() + 2, mouseEvent->scenePos().y() ), sceneRect().bottom() - 4 );
        m_connectionitem->createPath( QPoint( x, y ) );
    }
    QGraphicsScene::mouseMoveEvent( mouseEvent );
    //kDebug()<<mouseEvent->scenePos()<<","<<mouseEvent->isAccepted();
    
}

void DependencyScene::keyPressEvent( QKeyEvent *keyEvent )
{
    //kDebug()<<focusItem();
    if ( m_visibleItems.isEmpty() ) {
        return QGraphicsScene::keyPressEvent( keyEvent );
    }
    if ( focusItem() == 0 ) {
        setFocusItem( m_visibleItems.values().first() );
        if ( focusItem() ) {
            update( focusItem()->boundingRect() );
        }
        emit focusItemChanged( focusItem() );
        return;
    }
    switch ( keyEvent->key() ) {
        case Qt::Key_Left: {
            QGraphicsItem *fitem = focusItem();
            if ( fitem->type() == DependencyNodeItem::Type ) {
                DependencyConnectorItem *item = static_cast<DependencyNodeItem*>( fitem )->startConnector();
                if ( item ) {
                    setFocusItem( item );
                    update( static_cast<DependencyNodeItem*>( fitem )->rect() );
                    update( item->rect() );
                    emit focusItemChanged( focusItem() );
                }
                return;
            }
            if ( fitem->type() == DependencyConnectorItem::Type ) {
                DependencyConnectorItem *citem = static_cast<DependencyConnectorItem*>( fitem );;
                if ( citem->ctype() == DependencyNodeItem::Start ) {
                    //Goto prev nodes finishConnector
                    DependencyNodeItem *nitem = static_cast<DependencyNodeItem*>( citem->parentItem() );
                    DependencyNodeItem *item = nodeItem( nitem->row() - 1 );
                    if ( item ) {
                        setFocusItem( item->finishConnector() );
                        update( citem->rect() );
                        update( item->finishConnector()->rect() );
                        emit focusItemChanged( focusItem() );
                    }
                } else {
                    // Goto node item (parent)
                    setFocusItem( citem->parentItem() );
                    update( citem->rect() );
                    update( static_cast<DependencyNodeItem*>( citem->parentItem() )->rect() );
                    emit focusItemChanged( focusItem() );
                }
                return;
            }
            return;
        }
        case Qt::Key_Right: {
            QGraphicsItem *fitem = focusItem();
            if ( fitem->type() == DependencyNodeItem::Type ) {
                DependencyConnectorItem *item = static_cast<DependencyNodeItem*>( fitem )->finishConnector();
                if ( item ) {
                    setFocusItem( item );
                    update( item->rect() );
                    update( static_cast<DependencyNodeItem*>( item->parentItem() )->rect() );
                    emit focusItemChanged( focusItem() );
                }
                return;
            }
            if ( fitem->type() == DependencyConnectorItem::Type ) {
                DependencyConnectorItem *citem = static_cast<DependencyConnectorItem*>( fitem );;
                if ( citem->ctype() == DependencyNodeItem::Finish ) {
                    //Goto prev nodes startConnector
                    DependencyNodeItem *nitem = static_cast<DependencyNodeItem*>( citem->parentItem() );
                    DependencyNodeItem *item = nodeItem( nitem->row() + 1 );
                    if ( item ) {
                        setFocusItem( item->startConnector() );
                        update( citem->rect() );
                        update( item->startConnector()->rect() );
                        emit focusItemChanged( focusItem() );
                    }
                } else {
                    // Goto node item (parent)
                    setFocusItem( citem->parentItem() );
                    update( citem->rect() );
                    update( static_cast<DependencyNodeItem*>( citem->parentItem() )->rect() );
                    emit focusItemChanged( focusItem() );
                }
                return;
            }
            return;
        }
        case Qt::Key_Up: {
            QGraphicsItem *fitem = focusItem();
            if ( fitem->type() == DependencyNodeItem::Type ) {
                DependencyNodeItem *item = nodeItem( static_cast<DependencyNodeItem*>( fitem )->row() - 1 );
                if ( item ) {
                    setFocusItem( item );
                    update( static_cast<DependencyNodeItem*>( fitem )->rect() );
                    update( item->rect() );
                    emit focusItemChanged( focusItem() );
                }
                return;
            }
            if ( fitem->type() == DependencyConnectorItem::Type ) {
                DependencyConnectorItem *citem = static_cast<DependencyConnectorItem*>( fitem );
                DependencyNodeItem *nitem = static_cast<DependencyNodeItem*>( citem->parentItem() );
                if ( citem->ctype() == DependencyNodeItem::Finish ) {
                    DependencyNodeItem *item = nodeItem( nitem->row() - 1 );
                    if ( item ) {
                        setFocusItem( item->finishConnector() );
                        update( citem->rect() );
                        update( item->finishConnector()->rect() );
                        emit focusItemChanged( focusItem() );
                    }
                } else {
                    DependencyNodeItem *item = nodeItem( static_cast<DependencyNodeItem*>( fitem )->row() - 1 );
                    if ( item ) {
                        setFocusItem( item->startConnector() );
                        update( citem->rect() );
                        update( item->startConnector()->rect() );
                        emit focusItemChanged( focusItem() );
                    }
                }
                return;
            }
            return;
        }
        case Qt::Key_Down: {
            QGraphicsItem *fitem = focusItem();
            if ( fitem->type() == DependencyNodeItem::Type ) {
                DependencyNodeItem *item = nodeItem( static_cast<DependencyNodeItem*>( fitem )->row() + 1 );
                if ( item ) {
                    setFocusItem( item );
                    update( static_cast<DependencyNodeItem*>( fitem )->rect() );
                    update( item->rect() );
                    emit focusItemChanged( focusItem() );
                }
            }
            if ( fitem->type() == DependencyConnectorItem::Type ) {
                DependencyConnectorItem *citem = static_cast<DependencyConnectorItem*>( fitem );
                DependencyNodeItem *nitem = static_cast<DependencyNodeItem*>( citem->parentItem() );
                if ( citem->ctype() == DependencyNodeItem::Finish ) {
                    DependencyNodeItem *item = nodeItem( nitem->row() + 1 );
                    if ( item ) {
                        setFocusItem( item->finishConnector() );
                        update( citem->rect() );
                        update( item->finishConnector()->rect() );
                        emit focusItemChanged( focusItem() );
                    }
                } else {
                    DependencyNodeItem *item = nodeItem( static_cast<DependencyNodeItem*>( fitem )->row() + 1 );
                    if ( item ) {
                        setFocusItem( item->startConnector() );
                        update( citem->rect() );
                        update( item->startConnector()->rect() );
                        emit focusItemChanged( focusItem() );
                    }
                }
                return;
            }
            return;
        }
        case Qt::Key_Space:
        case Qt::Key_Select: {
            QGraphicsItem *item = focusItem();
            if ( item->type() == DependencyConnectorItem::Type ) {
                singleConnectorClicked( static_cast<DependencyConnectorItem*>( item ) );
                return;
            }
            if ( item->type() == DependencyNodeItem::Type ) {
                singleConnectorClicked( 0 );
                foreach ( QGraphicsItem *i, selectedItems() ) {
                    i->setSelected( false );
                }
                item->setSelected( true );
                return;
            }
            return;
        }
    }
    QGraphicsScene::keyPressEvent( keyEvent );
}

DependencyNodeItem *DependencyScene::nodeItem( int row ) const
{
    if ( row < 0 || m_visibleItems.isEmpty() ) {
        return 0;
    }
    foreach ( DependencyNodeItem *i, m_visibleItems ) {
        if ( i->row() == row ) {
            return i;
        }
    }
    return 0;
}

void DependencyScene::singleConnectorClicked( DependencyConnectorItem *item )
{
    //kDebug();
    clearSelection();
    QList<DependencyConnectorItem*> lst;
    if ( item == 0 || item == fromItem() ) {
        setFromItem( 0 );
        m_clickedItems = lst;
    } else if ( fromItem() == 0 ) {
        setFromItem( item );
    } else if ( connectionIsValid( fromItem(), item ) ) {
        emit connectItems( fromItem(), item );
        setFromItem( 0 );
    } else {
        setFromItem( 0 );
    }
    emit connectorClicked( item );
}

void DependencyScene::multiConnectorClicked( DependencyConnectorItem *item )
{
    //kDebug();
    singleConnectorClicked( item );
}

void DependencyScene::clearConnection()
{
    setFromItem( 0 );
    m_clickedItems.clear();
}

void DependencyScene::mousePressEvent( QGraphicsSceneMouseEvent *mouseEvent )
{
    //kDebug();
    QGraphicsScene::mousePressEvent( mouseEvent );
    if ( ! mouseEvent->isAccepted() ) {
        clearConnection();
    }
}

void DependencyScene::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent *event )
{
    //kDebug()<<event->pos()<<event->scenePos()<<event->screenPos();
    QGraphicsScene::mouseDoubleClickEvent( event );
    emit itemDoubleClicked( itemAt( event->scenePos() ) );
}

void DependencyScene::contextMenuEvent ( QGraphicsSceneContextMenuEvent *event )
{
    if ( event->reason() == QGraphicsSceneContextMenuEvent::Mouse ) {
        kDebug()<<"Mouse:"<<itemAt( event->scenePos())<<event->pos()<<event->scenePos()<<event->screenPos();
        emit contextMenuRequested( itemAt( event->scenePos() ), event->screenPos() );
        return;
    }
    if ( focusItem() ) {
        if ( focusItem()->type() == DependencyConnectorItem::Type ) {
            DependencyConnectorItem *to = static_cast<DependencyConnectorItem*>( focusItem() );
            DependencyConnectorItem *from = fromItem();
            kDebug()<<"DependencyConnectorItem:"<<from<<to;
            if ( from ) {
                DependencyLinkItem *link = findItem( from, to );
                if ( link ) {
                    emit dependencyContextMenuRequested( link, to );
                    setFromItem( 0 ); // avoid showing spurious DependencyCreatorItem
                    return;
                } else kDebug()<<"No link";
            }
        } else kDebug()<<"Not connector type"<<focusItem();
    } else kDebug()<<"No focusItem";
    emit contextMenuRequested( focusItem() );
}

void DependencyScene::setReadWrite( bool on )
{
    m_readwrite = on;
    foreach ( QGraphicsItem *i, items() ) {
        if ( i->type() == DependencyConnectorItem::Type ) {
            static_cast<DependencyConnectorItem*>( i )->setEditable( on );
        } else if ( i->type() == DependencyLinkItem::Type ) {
            static_cast<DependencyLinkItem*>( i )->setEditable( on );
        }
    }
}

//--------------------

DependencyView::DependencyView( QWidget *parent )
    : QGraphicsView( parent ),
    m_project( 0 )
{
    setItemScene( new DependencyScene( this ) );
    setAlignment( Qt::AlignLeft | Qt::AlignTop );
    
    connect( scene(), SIGNAL( selectionChanged() ), this, SLOT( slotSelectionChanged() ) );
    connect( scene(), SIGNAL( connectItems( DependencyConnectorItem*, DependencyConnectorItem* ) ), this, SIGNAL( makeConnection( DependencyConnectorItem*, DependencyConnectorItem* ) ) );

    connect( scene(), SIGNAL( contextMenuRequested( QGraphicsItem* ) ), this, SLOT( slotContextMenuRequested( QGraphicsItem* ) ) );

    connect( scene(), SIGNAL( dependencyContextMenuRequested( DependencyLinkItem*, DependencyConnectorItem* ) ), this, SLOT( slotDependencyContextMenuRequested( DependencyLinkItem*, DependencyConnectorItem* ) ) );

    connect( scene(), SIGNAL( contextMenuRequested( QGraphicsItem*, const QPoint& ) ), this, SIGNAL( contextMenuRequested( QGraphicsItem*, const QPoint& ) ) );

    connect( itemScene(), SIGNAL( focusItemChanged( QGraphicsItem* ) ), this, SLOT( slotFocusItemChanged( QGraphicsItem* ) ) );

    m_autoScrollTimer.start( 100 );
    connect( &m_autoScrollTimer, SIGNAL( timeout() ), SLOT( slotAutoScroll() ) );
}

void DependencyView::slotContextMenuRequested( QGraphicsItem *item )
{
    if ( item ) {
        kDebug()<<item<<item->boundingRect()<<(item->mapToScene( item->pos() ).toPoint())<<(mapToGlobal( item->mapToParent( item->pos() ).toPoint()));
        emit contextMenuRequested( item, mapToGlobal( item->mapToScene( item->boundingRect().topRight() ).toPoint() ) );
    }
}

void DependencyView::slotDependencyContextMenuRequested( DependencyLinkItem *item, DependencyConnectorItem *connector )
{
    if ( item ) {
        kDebug()<<item<<item->boundingRect()<<(item->mapToScene( item->pos() ).toPoint())<<(mapToGlobal( item->mapToParent( item->pos() ).toPoint()));
        emit contextMenuRequested( item, mapToGlobal( item->mapToScene( item->boundingRect().topRight() ).toPoint() ) );
    }
}

void DependencyView::slotConnectorClicked( DependencyConnectorItem *item )
{
    if ( itemScene()->fromItem() == 0 ) {
        itemScene()->setFromItem( item );
    } else {
        //kDebug()<<"emit makeConnection:"<<static_cast<DependencyNodeItem*>( item->parentItem() )->text();
        emit makeConnection( itemScene()->fromItem(), item );
    }
}

void DependencyView::slotSelectionChanged()
{
    //HACK because of tt bug 160653
    QTimer::singleShot(0, this, SLOT(slotSelectedItems()));
}

void DependencyView::slotSelectedItems()
{
    emit selectionChanged( itemScene()->selectedItems() );
}

void DependencyView::slotFocusItemChanged( QGraphicsItem *item )
{
    ensureVisible( item, 10, 10 );
}

void DependencyView::setItemScene( DependencyScene *scene )
{
    setScene( scene );
    //slotResizeScene( m_treeview->viewport()->size() );
    if ( m_project ) {
        createItems();
    }
}
void DependencyView::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( slotRelationAdded( Relation* ) ) );
        disconnect( m_project, SIGNAL( relationRemoved( Relation* ) ), this, SLOT( slotRelationRemoved( Relation* ) ) );
        disconnect( m_project, SIGNAL( relationModified( Relation* ) ), this, SLOT( slotRelationModified( Relation* ) ) );
    
        disconnect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeAdded( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        disconnect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotNodeMoved( Node* ) ) );
    }
    m_project = project;
    if ( project ) {
        connect( m_project, SIGNAL( relationAdded( Relation* ) ), this, SLOT( slotRelationAdded( Relation* ) ) );
        connect( m_project, SIGNAL( relationRemoved( Relation* ) ), this, SLOT( slotRelationRemoved( Relation* ) ) );
        connect( m_project, SIGNAL( relationModified( Relation* ) ), this, SLOT( slotRelationModified( Relation* ) ) );
        
        connect( m_project, SIGNAL( nodeAdded( Node* ) ), this, SLOT( slotNodeAdded( Node* ) ) );
        connect( m_project, SIGNAL( nodeRemoved( Node* ) ), this, SLOT( slotNodeRemoved( Node* ) ) );
        connect( m_project, SIGNAL( nodeChanged( Node* ) ), this, SLOT( slotNodeChanged( Node* ) ) );
        connect( m_project, SIGNAL( nodeMoved( Node* ) ), this, SLOT( slotNodeMoved( Node* ) ) );

        connect( m_project, SIGNAL( wbsDefinitionChanged() ), this, SLOT( slotWbsCodeChanged() ) );
    }
    if ( itemScene() ) {
        itemScene()->setProject( project );
        createItems();
    }

}

DependencyLinkItem *DependencyView::findItem( const Relation* rel ) const
{
    return itemScene()->findItem( rel );
}

DependencyNodeItem *DependencyView::findItem( const Node *node ) const
{
    return itemScene()->findItem( node );
}

void DependencyView::slotRelationAdded( Relation* rel )
{
    DependencyLinkItem *item = findItem( rel );
    if ( item == 0 ) {
        DependencyNodeItem *p = findItem( rel->parent() );
        DependencyNodeItem *c = findItem( rel->child() );
        DependencyLinkItem *r = new DependencyLinkItem( p, c, rel );
        scene()->addItem( r );
        //kDebug();
        r->createPath();
        r->setVisible( c->isVisible() && p->isVisible() );
    } else kDebug()<<"Relation already exists!";
}

void DependencyView::slotRelationRemoved( Relation* rel )
{
    DependencyLinkItem *item = findItem( rel );
    if ( item ) {
        scene()->removeItem( item );
        delete item;
    } else kDebug()<<"Relation does not exist!";
}

void DependencyView::slotRelationModified( Relation* rel )
{
    //kDebug();
    slotRelationRemoved( rel );
    slotRelationAdded( rel );
}

void DependencyView::slotNodeAdded( Node *node )
{
    //kDebug();
    DependencyNodeItem *item = findItem( node );
    if ( item == 0 ) {
        item = createItem( node );
    } else {
        //kDebug()<<node->name();
        itemScene()->setItemVisible( item, true );
    }
    ensureVisible( item );
    slotWbsCodeChanged();
}

void DependencyView::slotNodeRemoved( Node *node )
{
    DependencyNodeItem *item = findItem( node );
    if ( item ) {
        //kDebug()<<node->name();
        itemScene()->setItemVisible( item, false );
    } else kDebug()<<"Node does not exist!";
    slotWbsCodeChanged();
}

void DependencyView::slotNodeChanged( Node *node )
{
    DependencyNodeItem *item = findItem( node );
    if ( item && item->isVisible() ) {
        item->setText();
        item->setSymbol();
    } else kDebug()<<"Node does not exist!";
}

void DependencyView::slotWbsCodeChanged()
{
    foreach( DependencyNodeItem *i, itemScene()->nodeItems() ) {
        if ( i->isVisible() ) {
            i->setText();
        }
    }
}

void DependencyView::slotNodeMoved( Node *node )
{
    slotNodeRemoved( node );
    slotNodeAdded( node );
}

void DependencyView::setItemExpanded( int , bool )
{
}

void DependencyView::createItems()
{
    itemScene()->clearScene();
    if ( m_project == 0 ) {
        return;
    }
    scene()->addLine( 0.0, 0.0, 1.0, 0.0 );
    createItems( m_project );

    createLinks();
}

DependencyNodeItem *DependencyView::createItem( Node *node )
{
    return itemScene()->createItem( node );
}

void DependencyView::createItems( Node *node )
{
    if ( node != m_project ) {
        //kDebug()<<node->name()<<" ("<<node->numChildren()<<")";
        DependencyNodeItem *i = createItem( node );
        if ( i == 0 ) {
            return;
        }
    }
    foreach ( Node *n, node->childNodeIterator() ) {
        createItems( n );
    }
}

void DependencyView::createLinks()
{
    //kDebug();
    itemScene()->createLinks();
}

void DependencyView::keyPressEvent(QKeyEvent *event)
{
    if ( event->modifiers() & Qt::ControlModifier ) {
        switch ( event->key() ) {
            case Qt::Key_Plus:
                return scale( 1.1, 1.1 );
            case Qt::Key_Minus:
                return scale( 0.9, 0.9 );
            default: break;
        }
    }
    QGraphicsView::keyPressEvent(event);
}

void DependencyView::mouseMoveEvent( QMouseEvent *mouseEvent )
{
    //qDebug()<<"DependencyView::mouseMoveEvent:"<<viewport()->cursor().shape();
    m_cursorPos = mouseEvent->pos();
    if ( itemScene()->connectionMode() && itemScene()->mouseGrabberItem() ) {
        QPointF spos = mapToScene( m_cursorPos );
        Qt::CursorShape c = Qt::ArrowCursor;
        foreach ( QGraphicsItem *i, itemScene()->items( spos ) ) {
            if ( i->type() == DependencyConnectorItem::Type ) {
                if ( i == itemScene()->fromItem() ) {
                    c = Qt::UpArrowCursor;
                } else {
                    if ( itemScene()->connectionIsValid( itemScene()->fromItem(), static_cast<DependencyConnectorItem*>( i ) ) ) {
                        c = Qt::UpArrowCursor;
                    } else {
                        c = Qt::ForbiddenCursor;
                    }
                }
            }
        }
        viewport()->setCursor( c );
    }
    QGraphicsView::mouseMoveEvent( mouseEvent );
    //kDebug()<<mouseEvent->scenePos()<<","<<mouseEvent->isAccepted();
    
}

void DependencyView::slotAutoScroll()
{
    if ( itemScene()->connectionMode() ) {
        ensureVisible( QRectF( mapToScene( m_cursorPos ), QSizeF( 1, 1 ) ), 2, 2 );
    }
}

//-----------------------------------
DependencyEditor::DependencyEditor( KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
    m_currentnode( 0 ),
    m_manager( 0 )
{
    setupGui();

    QVBoxLayout * l = new QVBoxLayout( this );
    l->setMargin( 0 );
    m_view = new DependencyView( this );
    l->addWidget( m_view );
    
    connect( m_view, SIGNAL( makeConnection( DependencyConnectorItem*, DependencyConnectorItem* ) ), this, SLOT( slotCreateRelation( DependencyConnectorItem*, DependencyConnectorItem* ) ) );
    connect( m_view, SIGNAL( selectionChanged( QList<QGraphicsItem*> ) ), this, SLOT( slotSelectionChanged( QList<QGraphicsItem*> ) ) );

    connect( m_view->itemScene(), SIGNAL( itemDoubleClicked( QGraphicsItem* ) ), this, SLOT( slotItemDoubleClicked( QGraphicsItem* ) ) );
    
    connect( m_view, SIGNAL( contextMenuRequested( QGraphicsItem*, const QPoint& ) ), this, SLOT( slotContextMenuRequested( QGraphicsItem*, const QPoint& ) ) );

}

void DependencyEditor::updateReadWrite( bool on )
{
    qDebug()<<"DependencyEditor::updateReadWrite:"<<on;
    m_view->itemScene()->setReadWrite( on );
    ViewBase::updateReadWrite( on );
}

void DependencyEditor::slotItemDoubleClicked( QGraphicsItem *item )
{
    //kDebug();
    if ( ! isReadWrite() ) {
        return;
    }
    if ( item && item->type() == DependencyLinkItem::Type ) {
        emit modifyRelation( static_cast<DependencyLinkItem*>( item )->relation );
        return;
    }
    if ( item && item->type() == DependencyNodeItem::Type ) {
        emit editNode( static_cast<DependencyNodeItem*>( item )->node() );
        return;
    }
    if ( item && item->type() == DependencyNodeSymbolItem::Type ) {
        emit editNode( static_cast<DependencyNodeItem*>( item->parentItem() )->node() );
        return;
    }
}

void DependencyEditor::slotCreateRelation( DependencyConnectorItem *pred, DependencyConnectorItem *succ )
{
    //kDebug();
    if ( ! isReadWrite() ) {
        return;
    }
    Node *par = pred->node();
    Node *ch = succ->node();
    Relation::Type type = Relation::FinishStart;
    if ( pred->ctype() == DependencyNodeItem::Start ) {
        if ( succ->ctype() == DependencyNodeItem::Start ) {
            type = Relation::StartStart;
        }
    } else {
        if ( succ->ctype() == DependencyNodeItem::Start ) {
            type = Relation::FinishStart;
        } else {
            type = Relation::FinishFinish;
        }
    }
    Relation *rel = ch->findRelation( par );
    if ( rel == 0 ) {
        //kDebug()<<"New:"<<par->name()<<" ->"<<ch->name()<<","<<type;
        emit addRelation( par, ch, type );
    } else if ( rel->type() != type ) {
        //kDebug()<<"Mod:"<<par->name()<<" ->"<<ch->name()<<","<<type;
        emit modifyRelation( rel, type );
    }
}

void DependencyEditor::draw( Project &project )
{
    m_view->setProject( &project );
}

void DependencyEditor::draw()
{
}

void DependencyEditor::setGuiActive( bool activate )
{
    //kDebug()<<activate;
    updateActionsEnabled( true );
    ViewBase::setGuiActive( activate );
/*    if ( activate && !m_view->selectionModel()->currentIndex().isValid() ) {
        m_view->selectionModel()->setCurrentIndex(m_view->model()->index( 0, 0 ), QItemSelectionModel::NoUpdate);
    }*/
}

void DependencyEditor::slotCurrentChanged(  const QModelIndex &, const QModelIndex & )
{
    //kDebug()<<curr.row()<<","<<curr.column();
    slotEnableActions();
}

void DependencyEditor::slotSelectionChanged(  QList<QGraphicsItem*> )
{
    //kDebug()<<lst.count();
    slotEnableActions();
}

int DependencyEditor::selectedNodeCount() const
{
    return selectedNodes().count();
}

QList<Node*> DependencyEditor::selectedNodes() const {
    QList<Node*> lst;
    foreach ( QGraphicsItem *i, m_view->itemScene()->selectedItems() ) {
        if ( i->type() == DependencyNodeItem::Type ) {
            lst << static_cast<DependencyNodeItem*>( i )->node();
        }
    }
    return lst;
}

Node *DependencyEditor::selectedNode() const
{
    QList<Node*> lst = selectedNodes();
    if ( lst.count() != 1 ) {
        return 0;
    }
    return lst.first();
}

Node *DependencyEditor::currentNode() const {
    return m_currentnode;
/*    Node * n = 0;
    QGraphicsItem *i = m_view->itemScene()->focusItem();
    if ( i && i->type() == DependencyNodeItem::Type ) {
        n = static_cast<DependencyNodeItem*>( i )->node();
    }
    if ( n == 0 || n->type() == Node::Type_Project ) {
        return 0;
    }
    return n;*/
}

Relation *DependencyEditor::currentRelation() const {
    return m_currentrelation;
}

void DependencyEditor::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
}

void DependencyEditor::slotContextMenuRequested( QGraphicsItem *item, const QPoint& pos )
{
    //kDebug()<<item<<","<<pos;
    if ( ! isReadWrite() ) {
        return;
    }
    QString name;
    if ( item && item->type() == DependencyNodeSymbolItem::Type ) {
        item = item->parentItem();
    }
    if ( item ) {
        if ( item->type() == DependencyNodeItem::Type ) {
            m_currentnode = static_cast<DependencyNodeItem*>( item )->node();
            if ( m_currentnode == 0 ) {
                //kDebug()<<"No node";
                return;
            }
            bool scheduled = m_manager != 0 && m_currentnode->isScheduled( m_manager->id() );
            switch ( m_currentnode->type() ) {
                case Node::Type_Task:
                    name = scheduled ? "task_popup" : "task_edit_popup";
                    break;
                case Node::Type_Milestone:
                    name = scheduled ? "taskeditor_milestone_popup" : "task_edit_popup";
                    break;
                case Node::Type_Summarytask:
                    name = "summarytask_popup";
                    break;
                default:
                    break;
            }
            //kDebug()<<m_currentnode->name()<<" :"<<pos;
        } else if ( item->type() == DependencyLinkItem::Type ) {
            m_currentrelation = static_cast<DependencyLinkItem*>( item )->relation;
            if ( m_currentrelation ) {
                name = "relation_popup";
            }
        } else if ( item->type() == DependencyConnectorItem::Type ) {
            DependencyConnectorItem *c = static_cast<DependencyConnectorItem*>( item );
            QList<DependencyLinkItem*> items;
            QList<QAction*> actions;
            KMenu menu;;
            foreach ( DependencyLinkItem *i, c->predecessorItems() ) {
                items << i;
                actions << menu.addAction( KIcon( "document-properties" ), i->predItem->text() );
            }
            menu.addSeparator();
            foreach ( DependencyLinkItem *i, c->successorItems() ) {
                items << i;
                actions << menu.addAction( KIcon( "document-properties" ), i->succItem->text() );
            }
            if ( ! actions.isEmpty() ) {
                QAction *action = menu.exec( pos );
                if ( action && actions.contains( action ) ) {
                    emit modifyRelation( items[ actions.indexOf( action ) ]->relation );
                    return;
                }
            }
        }
    }
    //kDebug()<<name;
    if ( ! name.isEmpty() ) {
        emit requestPopupMenu( name, pos );
    }
    m_currentnode = 0;
    m_currentrelation = 0;
}

void DependencyEditor::slotEnableActions()
{
    updateActionsEnabled( true );
}

void DependencyEditor::updateActionsEnabled( bool on )
{
    Project *p = m_view->project();
    
    bool o = ( on && p && selectedNodeCount() <= 1 );
    menuAddTask->setEnabled( o );
    actionAddTask->setEnabled( o );
    actionAddMilestone->setEnabled( o );
    
    actionDeleteTask->setEnabled( on && p && selectedNodeCount() > 0 );
    
    o = ( on && p && selectedNodeCount() == 1 );
    
    menuAddSubTask->setEnabled( o );
    actionAddSubtask->setEnabled( o );
}

void DependencyEditor::setupGui()
{
    KActionCollection *coll = actionCollection();
    
    QString name = "taskeditor_add_list";

    menuAddTask = new KActionMenu( KIcon( "view-task-add" ), "Add Task...", this );
    coll->addAction("add_task", menuAddTask );
    connect( menuAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    addAction( name, menuAddTask );

    actionAddTask  = new KAction( i18n("Add Task..."), this);
    actionAddTask->setShortcut( KShortcut( Qt::CTRL + Qt::Key_I ) );
    connect( actionAddTask, SIGNAL( triggered( bool ) ), SLOT( slotAddTask() ) );
    menuAddTask->addAction( actionAddTask );

    actionAddMilestone  = new KAction( i18n("Add Milestone..."), this );
    actionAddMilestone->setShortcut( KShortcut( Qt::CTRL + Qt::ALT + Qt::Key_I ) );
    connect( actionAddMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddMilestone() ) );
    menuAddTask->addAction( actionAddMilestone );
    

    menuAddSubTask = new KActionMenu( KIcon( "view-task-child-add" ), "Add Sub-Task...", this );
    coll->addAction("add_subtask", menuAddTask );
    connect( menuAddSubTask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    addAction( name, menuAddSubTask );

    actionAddSubtask  = new KAction( i18n("Add Sub-Task..."), this );
    actionAddSubtask->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::Key_I ) );
    connect( actionAddSubtask, SIGNAL( triggered( bool ) ), SLOT( slotAddSubtask() ) );
    menuAddSubTask->addAction( actionAddSubtask );

    actionAddSubMilestone = new KAction( i18n("Add Sub-Milestone..."), this );
    actionAddSubMilestone->setShortcut( KShortcut( Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_I ) );
    connect( actionAddSubMilestone, SIGNAL( triggered( bool ) ), SLOT( slotAddSubMilestone() ) );
    menuAddSubTask->addAction( actionAddSubMilestone );

    actionDeleteTask  = new KAction(KIcon( "edit-delete" ), i18n("Delete Task"), this);
    actionDeleteTask->setShortcut( KShortcut( Qt::Key_Delete ) );
    coll->addAction("delete_task", actionDeleteTask );
    connect( actionDeleteTask, SIGNAL( triggered( bool ) ), SLOT( slotDeleteTask() ) );
    addAction( name, actionDeleteTask );
}

void DependencyEditor::slotAddTask()
{
    //kDebug();
    m_currentnode = selectedNode();
    emit addTask();
    m_currentnode = 0;
}

void DependencyEditor::slotAddMilestone()
{
    //kDebug();
    m_currentnode = selectedNode(); // sibling
    emit addMilestone();
    m_currentnode = 0;
}

void DependencyEditor::slotAddSubtask()
{
    //kDebug();
    m_currentnode = selectedNode();
    if ( m_currentnode == 0 ) {
        return;
    }
    emit addSubtask();
    m_currentnode = 0;
}

void DependencyEditor::slotAddSubMilestone()
{
    kDebug();
    m_currentnode = selectedNode();
    if ( m_currentnode == 0 ) {
        return;
    }
    emit addSubMilestone();
    m_currentnode = 0;
}

void DependencyEditor::edit( QModelIndex i )
{
    if ( i.isValid() ) {
/*        QModelIndex p = m_view->itemModel()->parent( i );
        m_view->treeView()->setExpanded( p, true );
        m_view->treeView()->selectionModel()->setCurrentIndex( i, QItemSelectionModel::NoUpdate );
        m_view->treeView()->edit( i );*/
    }
}

void DependencyEditor::slotDeleteTask()
{
    //kDebug();
    QList<Node*> lst = selectedNodes();
    while ( true ) {
        // remove children of selected tasks, as parents delete their children
        Node *ch = 0;
        foreach ( Node *n1, lst ) {
            foreach ( Node *n2, lst ) {
                if ( n2->isChildOf( n1 ) ) {
                    ch = n2;
                    break;
                }
            }
            if ( ch != 0 ) {
                break;
            }
        }
        if ( ch == 0 ) {
            break;
        }
        lst.removeAt( lst.indexOf( ch ) );
    }
    foreach ( Node* n, lst ) { kDebug()<<n->name(); }
    emit deleteTaskList( lst );
}

KoPrintJob *DependencyEditor::createPrintJob()
{
    DependecyViewPrintingDialog *dia = new DependecyViewPrintingDialog( this, m_view );
    dia->printer().setCreator("KPlato 0.7");
//    dia->printer().setFullPage(true); // ignore printer margins
    return dia;
}

} // namespace KPlato

#include "kptdependencyeditor.moc"
