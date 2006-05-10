/* This file is part of the KDE project
  Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#include "view.h"

#include <math.h>

#include <qmatrix.h>
#include <qevent.h>
#include <QPoint>
#include <q3canvas.h>
#include <qaction.h>
#include <qcursor.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qimage.h>

#include <qprinter.h>
//Added by qt3to4:
#include <QDragMoveEvent>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QMouseEvent>
#include <kdebug.h>

#include <koproperty/property.h>

#include "canvas.h"
#include "box.h"
#include "commdefs.h"
#include "reportitem.h"
#include "label.h"
#include "field.h"
#include "specialfield.h"
#include "calcfield.h"
#include "line.h"
#include "kugartemplate.h"
#include "detailbase.h"
#include "band.h"
#include "command.h"

#include "plugin.h"

namespace Kudesigner
{

void SelectionRect::draw( QPainter & painter )
{
    //    painter.setPen(Qt::NoPen);

    /*  QPrinter pr;
      if ( pr.setup() ) {
        QPainter p(&pr);
        m_canvas->drawArea( m_canvas->rect(), &p );
      } */

    /*  kDebug(31000) << "creating pixmap" << endl;
        QPixmap mp(rect().size());
        kDebug(31000) << "creating painter" << endl;
        QPainter p(&mp);
        kDebug(31000) << "filling pixmap" << endl;
        m_canvas->drawArea(m_canvas->rect(), &p);
        kDebug(31000) << "converting to image" << endl;
        QImage im = mp.convertToImage();
        if (!im.isNull())
        {
            kDebug(31000) << "do dither" << endl;
            mp.convertFromImage(im,  Qt::OrderedAlphaDither);

            kDebug(31000) << "creating brush" << endl;
            QBrush br(KGlobalSettings::highlightColor(),Qt::TexturePattern);
            br.setPixmap(mp);
            painter.setBrush(br);
        }
        kDebug(31000) << "drawing" << endl;*/
    //    painter.drawRect(rect());
    QPen pen( QColor( 0, 0, 0 ), 0, Qt::DotLine );
    painter.setPen( pen );
    painter.setBrush( QBrush( Qt::NoBrush ) );
    painter.drawRect( rect() );
    //  QCanvasRectangle::draw(painter);
}



View::View( Canvas *canvas, QWidget *parent, const char *name, Qt::WFlags f ) :
        Q3CanvasView( canvas, parent, name, f ), selectionBuf( 0 ), m_plugin( 0 ), m_canvas( canvas )
{
    itemToInsert = 0;
    moving = 0;
    resizing = 0;
    selectionStarted = 0;
    request = RequestNone;

    selectionRect = new SelectionRect( 0, 0, 0, 0, canvas );

    connect( m_canvas, SIGNAL( itemSelected() ), this, SLOT( selectItem() ) );
}

void View::deleteItem( Q3CanvasItemList &l )
{
    for ( Q3CanvasItemList::Iterator it = l.begin(); it != l.end(); ++it )
    {
        m_canvas->unselectItem( static_cast<Kudesigner::Box*>( *it ) );
        if ( m_canvas->kugarTemplate() ->removeReportItem( *it ) )
            break;
    }
}

void View::editItem( Q3CanvasItemList & /* l */ )
{
    //display editor for report items or sections
    /*  for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
        {
        if ((*it)->rtti() >= 1800) //for my own report items
        {
            CanvasKudesigner::Box *l = (CanvasKudesigner::Box*)(*it);
            dlgItemOptions *dlgOpts = new dlgItemOptions(&(l->props), this);
            dlgOpts->exec();
            delete dlgOpts;
            if ((*it)->rtti() == KugarTemplate)
            ((CanvasKugarTemplate*)(*it))->updatePaperProps();
            (*it)->hide();
            (*it)->show();
            if ((*it)->rtti() < 2000)
            ((MyCanvas *)(m_canvas))->templ->arrangeSections();
            m_canvas->update();
            emit modificationPerformed();
            break;
        }
        }*/
}

void View::selectItemFromList( Q3CanvasItemList &l )
{
    //display editor for report items or sections
    for ( Q3CanvasItemList::Iterator it = l.begin(); it != l.end(); ++it )
    {
        if ( ( *it ) ->rtti() >= 1800 )  //include bands and the template itself
        {
            Kudesigner::Box * b = static_cast<Kudesigner::Box*>( *it );
            if ( !m_canvas->selected.contains( b ) )
            {
                m_canvas->unselectAll();
                m_canvas->selectItem( b, false );
                m_canvas->update();
                //                qWarning("selected item set");
                //                selected->drawHolders();
                return ;
            }
            if ( m_canvas->selected.contains( b ) )
            {
                if ( m_canvas->selected.count() > 1 )
                {
                    m_canvas->unselectAll();
                    m_canvas->selectItem( b, false );
                    m_canvas->update();
                }
                return ;
            }
        }
    }
    m_canvas->unselectAll();
    //    qWarning("unselect");
}


void View::placeItem( Q3CanvasItemList &l, QMouseEvent *e )
{
    for ( Q3CanvasItemList::Iterator it = l.begin(); it != l.end(); ++it )
    {
        if ( ( ( *it ) ->rtti() > 1800 ) && ( ( ( *it ) ->rtti() < 2000 ) ) )
        {
            int band = ( *it ) ->rtti();
            int bandLevel = -1;
            if ( ( band == Rtti_DetailHeader ) ||
                    ( band == Rtti_Detail ) ||
                    ( band == Rtti_DetailFooter ) )
                bandLevel = static_cast<DetailBase*>( *it ) ->level();
            emit itemPlaced( e->x(), e->y(), band, bandLevel );

            //            emit modificationPerformed();
        }
    }
    itemToInsert = 0;
    emit selectedActionProcessed();
}


bool View::startResizing( QMouseEvent * /*e*/, QPoint &p )
{
    if ( m_canvas->selected.count() == 0 )
        return false;
    for ( BoxList::iterator it = m_canvas->selected.begin();
            it != m_canvas->selected.end(); ++it )
    {
        Kudesigner::Box *cbx = *it;
        resizing_type = cbx->isInHolder( p );
        /*        qWarning("POINT: %d %d", p.x(), p.y());
                qWarning("RESIZE: %d", resizing_type);*/
        if ( resizing_type )
        {
            m_canvas->selectItem( cbx, false );
            //kDebug()<<"A Widget should be resized"<<endl;
            moving = 0;
            resizing = cbx;
            moving_start = p;
            moving_offsetX = 0;
            moving_offsetY = 0;

            if ( cbx->rtti() > 2001 )
            {
                ReportItem * item = static_cast<ReportItem*>( cbx );
                resizing_constraint.setX( ( int ) item->section() ->x() );
                resizing_constraint.setY( ( int ) item->section() ->y() );
                resizing_constraint.setWidth( item->section() ->width() );
                resizing_constraint.setHeight(
                    item->section() ->height() );
                if ( cbx->rtti() != Rtti_Line )
                {
                    resizing_minSize.setWidth( 10 );
                    resizing_minSize.setHeight( 10 );
                }
                else
                {
                    resizing_minSize.setWidth( 0 );
                    resizing_minSize.setHeight( 0 );
                }
            }
            else
                if ( cbx->rtti() >= Rtti_ReportHeader )
                {
                    resizing_constraint = QRect( 0, 0, 1000, 1000 );
                    resizing_minSize.setWidth( 0 );
                    resizing_minSize.setHeight( static_cast<Band*>( cbx ) ->minHeight() );
                }
                else
                {
                    resizing_constraint = QRect( 0, 0, 1000, 1000 );
                    resizing_minSize.setWidth( 0 );
                    resizing_minSize.setHeight( 10 );
                }
            return true;
        }
    }
    return false;
}

void View::startMoveOrResizeOrSelectItem( Q3CanvasItemList &l,
        QMouseEvent * /*e*/, QPoint &p )
{
    //allow user to move any item except for page rectangle
    for ( Q3CanvasItemList::Iterator it = l.begin(); it != l.end(); ++it )
    {
        Kudesigner::Box *cb = static_cast<Kudesigner::Box*>( *it );
        if ( cb->rtti() >= 1700 )  //> 2001)
        {
            moving_start = p;
            moving_offsetX = 0;
            moving_offsetY = 0;
            resizing_type = cb->isInHolder( p );
            if ( ( *it ) ->rtti() > 2001 )
            {
                ReportItem * item = static_cast<ReportItem*>( *it );
                moving = item;
                resizing = 0;
                return ;
            }
        }
    }
    moving = 0;
    resizing = 0;
    //         qWarning("1");
    selectionStarted = 1;
    selectionRect->setX( p.x() );
    selectionRect->setY( p.y() );
    selectionRect->setZ( 50 );
    //         qWarning("2");
    selectionRect->show();
    //         qWarning("3");
}

void View::contentsMousePressEvent( QMouseEvent* e )
{
    QPoint p = inverseWorldMatrix().QMatrix::map( e->pos() );
    Q3CanvasItemList l = m_canvas->collisions( p );

    //if there is a request for properties or for delete operation
    //perform that and do not take care about mouse buttons

    //    qWarning("mouse press");

    switch ( request )
    {
    case RequestProps:
        clearRequest();
        editItem( l );
        return ;
    case RequestDelete:
        deleteItem( l );
        clearRequest();
        return ;
    case RequestNone:
        break;
    }

    moving = 0;
    resizing = 0;
    selectionStarted = 0;



    /*    Kudesigner::Box *b;
        qWarning("Selected items:");
        for (b = selected.first(); b; b = selected.next())
            qWarning("%s", b->props["Text"].first.latin1());

      */
    switch ( e->button() )
    {
			case Qt::LeftButton:
        if ( itemToInsert )
        {
            //                qWarning("placing item");
            m_canvas->unselectAll();
            placeItem( l, e );
        }
        else
        {
            //                qWarning("starting move or resize");
            if ( !startResizing( e, p ) )
            {
                selectItemFromList( l );
                startMoveOrResizeOrSelectItem( l, e, p );
            }
        }
        break;
    default:
        break;
    }
}

void View::contentsMouseReleaseEvent( QMouseEvent* e )
{
    selectionRect->setSize( 0, 0 );
    selectionRect->setX( 0 );
    selectionRect->setY( 0 );
    selectionRect->hide();

    QPoint p = inverseWorldMatrix().QMatrix::map( e->pos() );
    Q3CanvasItemList l = m_canvas->collisions( p );

    switch ( e->button() )
    {
			case Qt::LeftButton:
        if ( selectionStarted )
            finishSelection();
        break;
        /*        case MidButton:
                    deleteItem(l);
                    break;
                case Qt::RightButton:
                    editItem(l);
                    break;*/
    default:
        break;
    }
}


void View::fixMinValues( double &pos, double minv, double &offset )
{
    if ( pos < minv )
    {
        offset = offset + pos - minv;
        pos = minv;
    }
    else
    {
        if ( offset < 0 )
        {
            offset = offset + pos - minv;
            if ( offset < 0 )
                pos = minv;
            else
            {
                pos = offset + minv;
                offset = 0;
            }
        }
    }
}

void View::fixMaxValues( double &pos, double size, double maxv, double &offset )
{
    double tmpMax = pos + size;
    if ( tmpMax > maxv )
    {
        offset = offset + tmpMax - maxv;
        pos = maxv - size;
    }
    else
    {
        if ( offset > 0 )
        {
            offset = offset + tmpMax - maxv;
            if ( offset > 0 )
                pos = maxv - size;
            else
            {
                pos = offset + maxv - size;
                offset = 0;
            }
        }
    }
}

#ifdef Q_WS_WIN
double rint( double x )
{
    if ( fabs( x - floor( x ) ) < fabs( x - ceil( x ) ) )
        return floor( x );
    else
        return ceil( x );
}
#endif

void View::stickToGrid( double &x, double &y )
{
    int cx = int( rint( x / Config::gridSize() ) );
    int cy = int( rint( y / Config::gridSize() ) );
    x = cx * Config::gridSize();
    y = cy * Config::gridSize();
}

void View::stickDimToGrid( double x, double y, double &w, double &h )
{
    int rightX = int( x + w );
    int bottomY = int( y + h );
    int nx = int( rint( rightX /Config::gridSize() ) * Config::gridSize() );
    int ny = int( rint( bottomY / Config::gridSize() ) * Config::gridSize() );
    w = nx - x;
    h = ny - y;
}

void View::contentsMouseMoveEvent( QMouseEvent* e )
{
    QPoint p = inverseWorldMatrix().map( e->pos() );

    /*    QCanvasItemList l=m_canvas->collisions(p);
      setCursor(QCursor(Qt::ArrowCursor));
        unsetCursor();
        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
        {
            if ((*it)->rtti() > 2000)
            {
                CanvasReportItem *item = (CanvasReportItem*)(*it);
                if (item->bottomRightResizableRect().contains(e->pos()))
                    setCursor(QCursor(Qt::SizeFDiagCursor));
            }
        }*/

    if ( moving )
    {
        double newXPos = moving->x() + p.x() - moving_start.x();
        double newYPos = moving->y() + p.y() - moving_start.y();
        fixMinValues( newYPos, moving->parentSection->y(), moving_offsetY );
        fixMinValues( newXPos, moving->parentSection->x(), moving_offsetX );
        fixMaxValues( newYPos, moving->height(), moving->parentSection->y() + moving->parentSection->height(), moving_offsetY );
        fixMaxValues( newXPos, moving->width(), moving->parentSection->x() + moving->parentSection->width(), moving_offsetX );

        double sx = newXPos;
        double sy = newYPos;
        if ( Config::gridSize() > 1 )
            stickToGrid( newXPos, newYPos );

        moving->move( newXPos, newYPos );

        /*        attempt to prevent item collisions
                QCanvasItemList l=m_canvas->collisions(moving->rect());
                if (l.count() > 2)
                {
                    moving->moveBy(-(p.x() - moving_start.x()),
                                    -(p.y() - moving_start.y()));
                    m_canvas->update();
                    return;
                }*/
        //         moving_start = p;
        moving_start = QPoint( p.x() + ( int ) ( newXPos - sx ), p.y() + ( int ) ( newYPos - sy ) );
        moving->updateGeomProps();
        m_canvas->update();
        emit modificationPerformed();
    }
    if ( resizing )
    {
        Q3CanvasRectangle * r = ( Q3CanvasRectangle * ) resizing;
        double newXPos = r->x();
        double newYPos = r->y();
        double h = r->height();
        double w = r->width();
        //        kDebug()<<"resizing"<<endl;

        //vertical resizing
        if ( resizing_type & Kudesigner::Box::ResizeBottom )
        {
            //            kDebug()<<"Resize bottom"<<endl;
            h = h + p.y() - moving_start.y();
            fixMaxValues( h, r->y(), resizing_constraint.bottom(), moving_offsetY );
            if ( resizing->rtti() != Rtti_Line )
                fixMinValues( h, resizing_minSize.height(), moving_offsetY );
        }
        else
            if ( resizing_type & Kudesigner::Box::ResizeTop )
            {
                //                kDebug()<<"Resize top"<<endl;
                newYPos = r->y() + p.y() - moving_start.y();
                fixMinValues( newYPos, resizing_constraint.top(), moving_offsetY );
                if ( resizing->rtti() != Rtti_Line )
                    fixMaxValues( newYPos, resizing_minSize.height(), r->y() + r->height(), moving_offsetY );
                h = h + ( r->y() - newYPos );
            }


        //horizontal resizing
        if ( resizing_type & Kudesigner::Box::ResizeRight )
        {
            //kDebug()<<"Resize right"<<endl;
            w = w + p.x() - moving_start.x();
            fixMaxValues( w, r->x(), resizing_constraint.right(), moving_offsetX );
            if ( resizing->rtti() != Rtti_Line )
                fixMinValues( w, resizing_minSize.width(), moving_offsetX );
        }
        else
            if ( resizing_type & Kudesigner::Box::ResizeLeft )
            {
                //    kDebug()<<"Resize left"<<endl;
                newXPos = r->x() + p.x() - moving_start.x();
                fixMinValues( newXPos, resizing_constraint.left(), moving_offsetX );
                if ( resizing->rtti() != Rtti_Line )
                    fixMaxValues( newXPos, resizing_minSize.width(), r->x() + r->width(), moving_offsetX );
                w = w + ( r->x() - newXPos );
            }

        //sticky stuff
        double sx = newXPos;
        double sy = newYPos;
        if ( Config::gridSize() > 1 )
            stickToGrid( newXPos, newYPos );

        r->move( newXPos, newYPos );

        int dx = ( int ) ( newXPos - sx );
        int dy = ( int ) ( newYPos - sy );
        //        moving_start = QPoint(p.x() + dx, p.y() + dy);
        w -= dx;
        h -= dy;
        //         moving_start = p;

        double sw = w;
        double sh = h;
        stickDimToGrid( newXPos, newYPos, w, h );
        int dw = ( int ) ( w - sw );
        int dh = ( int ) ( h - sh );

        moving_start = QPoint( p.x() + dx + dw, p.y() + dy + dh );

        r->setSize( ( int ) w, ( int ) h );
        resizing->updateGeomProps();
        m_canvas->update();
        emit modificationPerformed();
    }
    if ( selectionStarted )
    {
        selectionRect->setSize( ( int ) ( e->pos().x() - selectionRect->x() ),
                                ( int ) ( e->pos().y() - selectionRect->y() ) );
        m_canvas->unselectAll();
        Q3CanvasItemList l = m_canvas->collisions( selectionRect->rect() );
        for ( Q3CanvasItemList::Iterator it = l.begin(); it != l.end(); ++it )
        {
            QRect r;
            int left = selectionRect->rect().left();
            int right = selectionRect->rect().right();
            int top = selectionRect->rect().top();
            int bottom = selectionRect->rect().bottom();
            r.setLeft( left < right ? left : right );
            r.setRight( left < right ? right : left );
            r.setTop( top < bottom ? top : bottom );
            r.setBottom( top < bottom ? bottom : top );

            if ( ( ( *it ) ->rtti() > 2001 ) &&
                    ( r.contains( static_cast<Kudesigner::Box*>( *it ) ->rect() ) ) )
            {
                m_canvas->selectItem( static_cast<Kudesigner::Box*>( *it ) );
                m_canvas->update();
            }
        }


        /*        selectionRect->setSize(e->pos().x() - selectionRect->x(),
                    e->pos().y() - selectionRect->y());
                unselectAll();
                QCanvasItemList l = m_canvas->collisions(selectionRect->rect());
                for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
                {
                    if ( ((*it)->rtti() > 2001) &&
                        (selectionRect->rect().contains(((CanvasKudesigner::Box*)(*it))->rect())) )
                    {
                        selectItem((CanvasKudesigner::Box*)(*it));
                        m_canvas->update();
                    }
                }*/
    }
}


void View::contentsMouseDoubleClickEvent( QMouseEvent *e )
{
    ReportItem * item = 0L;
    if ( e->button() == Qt::LeftButton && m_canvas->selected.count() == 1 )
        item = dynamic_cast<ReportItem*>( m_canvas->selected.first() );
    if ( item )
    {
        item->fastProperty();
        emit changed();
        item->hide();
        item->show();
    }
}

void View::setRequest( RequestType r )
{
    switch ( r )
    {
    case RequestProps:
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor( Qt::PointingHandCursor );
        break;
    case RequestDelete:
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor( Qt::ForbiddenCursor );
        break;
    case RequestNone:
        QApplication::restoreOverrideCursor();
        break;
    }
    request = r;
}

void View::clearRequest()
{
    QApplication::restoreOverrideCursor();
    request = RequestNone;
    emit selectedEditActionProcessed();
}

bool View::requested()
{
    if ( request == RequestNone )
        return false;
    else
        return true;
}

void View::updateProperty()
{
    for ( Kudesigner::BoxList::iterator it = m_canvas->selected.begin();
            it != m_canvas->selected.end(); ++it )
    {
        Kudesigner::Box *b = *it;
        //         b->props[name]->setValue(value);
        b->hide();
        b->show();
        if ( ( b->rtti() >= 1800 ) && ( b->rtti() < 2000 ) )
            m_canvas->kugarTemplate() ->arrangeSections();
    }
}


void View::finishSelection()
{
    selectionStarted = false;

    if ( !m_canvas->selected.isEmpty() )
    {
        Kudesigner::BoxList::const_iterator it = m_canvas->selected.begin();
        Kudesigner::Box *b = *it;
        Buffer *buf = new Buffer( &( b->props ) );
        ++it;
        //         qWarning("== %d", m_canvas->selected.count());
        //         Kudesigner::BoxList::const_iterator it2 = m_canvas->selected.end();
        //         qWarning("41: %d", it != it2);
        for ( ; it != m_canvas->selected.end(); ++it )
        {
            b = *it;
            buf->intersect( &( b->props ) );
        }
        emit selectionClear();
        //        delete selectionBuf;
        selectionBuf = buf;
        emit selectionMade( selectionBuf );
    }
}

void View::setPlugin( KuDesignerPlugin *plugin )
{
    m_plugin = plugin;
}

void View::contentsDragMoveEvent( QDragMoveEvent * event )
{
    //perhaps this could be optimized a little bit
    if ( !m_plugin )
        return ;
    Q3CanvasItemList l = m_canvas->collisions( event->pos() );
    /*    kDebug()<<l.count()<<endl;*/
    if ( l.count() < 2 )
    {
        event->ignore();
        return ;
    }
    Kudesigner::Box *b = static_cast<Kudesigner::Box*>( *( l.begin() ) );
    event->accept();
    if ( m_plugin->dragMove( event, b ) )
        event->accept();
    else
        event->ignore();
}

void View::contentsDragEnterEvent ( QDragEnterEvent * /*event*/ )
{
    //    event->accept();
}

void View::keyPressEvent( QKeyEvent *e )
{
    kDebug(31000) << k_funcinfo << endl;

    if ( m_canvas->selected.count() == 1 )
    {
        ReportItem * item = static_cast<ReportItem *>( m_canvas->selected.first() );

        switch ( e->key() )
        {
        case Qt::Key_Delete:
            kDebug(31000) <<  "Deleting selection" << endl;
            /*                unselectItem(item);
                            ( (MyCanvas*) m_canvas )->templ->removeReportItem( item );
                            clearRequest();*/
            //                deleteSelected();
            //FIXME: this disregards undo/redo
            if ( m_canvas->selected.count() > 0 )
            {
                emit selectionClear();
                DeleteReportItemsCommand *cmd = new DeleteReportItemsCommand( m_canvas,
                                                m_canvas->selected );
                cmd->execute();
                delete cmd;
            }
            return ;

            /* Adjust height with - and +  */
        case Qt::Key_Minus:
        case Qt::Key_Plus:
            {
                int size = item->props[ "FontSize" ].value().toInt();

                if ( e->key() == Qt::Key_Minus )
                    size--;
                else
                    size++;

                if ( size < 5 )
                    size = 5;

                if ( size > 50 )
                    size = 50;

                item->props[ "FontSize" ].setValue( size );
                item->hide();
                item->show();
                return ;
            }

        default:
            e->ignore();
        }

    }
}

void View::selectItem( )
{
    if ( !selectionStarted )
        finishSelection();
}

/*void View::deleteSelected( )
{
    CanvasKudesigner::Box *b;

    QPtrList<CanvasKudesigner::Box> list = m_canvas->selected;
    unselectAll();

    for (b = list.first(); b; b = list.next())
    {
        ( (MyCanvas*) m_canvas )->templ->removeReportItem( b );
    }
}
*/

void View::setCanvas( Canvas *canvas )
{
    if ( selectionRect )
        delete selectionRect;
    Q3CanvasView::setCanvas( ( Q3Canvas* ) canvas );
    m_canvas = canvas;
    selectionRect = new SelectionRect( 0, 0, 0, 0, m_canvas );
    connect( m_canvas, SIGNAL( itemSelected() ), this, SLOT( selectItem() ) );
    clearRequest();
}

void View::setGridSize( int size )
{
    Config::setGridSize( size );
    m_canvas->setChanged( m_canvas->rect() );
    m_canvas->update();
}

}

#include "view.moc"
