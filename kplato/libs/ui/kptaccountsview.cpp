/* This file is part of the KDE project
  Copyright (C) 2005 - 2006 Dag Andersen kplato@kde.org>

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
  Boston, MA 02110-1301, USA.
*/

#include "kptaccountsview.h"

#include "kptaccountsviewconfigdialog.h"
#include "kptdatetime.h"
#include "kptproject.h"
#include "kpteffortcostmap.h"
#include "kptaccountsmodel.h"

#include <KoDocument.h>

#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <qpalette.h>
#include <QPushButton>
#include <QSizePolicy>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMenu>

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>
#include <kaction.h>

#include <kdebug.h>

namespace KPlato
{

AccountsTreeView::AccountsTreeView( QWidget *parent )
    : DoubleTreeViewBase( parent )
{
    kDebug()<<"---------------"<<this<<"------------------";
    setSelectionMode( QAbstractItemView::ExtendedSelection );

    CostBreakdownItemModel *m = new CostBreakdownItemModel( this );
    setModel( m );
    
    QHeaderView *v = m_leftview->header();
    v->setStretchLastSection( false );
    v->setResizeMode( 1, QHeaderView::Stretch );
    v->setResizeMode ( 2, QHeaderView::ResizeToContents );
    
    v = m_rightview->header();
    v->setResizeMode ( QHeaderView::ResizeToContents );
    v->setStretchLastSection( false );
            
    hideColumns( m_rightview, QList<int>() << 0 << 1 << 2 );
    slotModelReset();
    
    connect( m, SIGNAL( modelReset() ), SLOT( slotModelReset() ) );
}

void AccountsTreeView::slotModelReset()
{
    hideColumns( m_leftview, QList<int>() << 3 << -1 );
    QHeaderView *v = m_leftview->header();
    kDebug()<<v->sectionSize(2)<<v->sectionSizeHint(2)<<v->defaultSectionSize()<<v->minimumSectionSize();

    hideColumns( m_rightview, QList<int>() << 0 << 1 << 2 );
}

CostBreakdownItemModel *AccountsTreeView::model() const
{
    return static_cast<CostBreakdownItemModel*>( DoubleTreeViewBase::model() );
}

bool AccountsTreeView::cumulative() const
{
    return model()->cumulative();
}

void AccountsTreeView::setCumulative( bool on )
{
    model()->setCumulative( on );
}

int AccountsTreeView::periodType() const
{
    return model()->periodType();
}
    
void AccountsTreeView::setPeriodType( int period )
{
    model()->setPeriodType( period );
}

int AccountsTreeView::startMode() const
{
    return model()->startMode();
}

void AccountsTreeView::setStartMode( int mode )
{
    model()->setStartMode( mode );
}

int AccountsTreeView::endMode() const
{
    return model()->endMode();
}

void AccountsTreeView::setEndMode( int mode )
{
    model()->setEndMode( mode );
}

QDate AccountsTreeView::startDate() const
{
    return model()->startDate();
}

void AccountsTreeView::setStartDate( const QDate &date )
{
    model()->setStartDate( date );
}

QDate AccountsTreeView::endDate() const
{
    return model()->endDate();
}

void AccountsTreeView::setEndDate( const QDate &date )
{
    model()->setEndDate( date );
}

int AccountsTreeView::showMode() const
{
    return model()->showMode();
}
void AccountsTreeView::setShowMode( int show )
{
    model()->setShowMode( show );
}

//------------------------
AccountsView::AccountsView( Project *project, KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
        m_project(project),
        m_manager( 0 )
{
    init();

    setupGui();
    
    connect( m_view, SIGNAL( contextMenuRequested( QModelIndex, const QPoint& ) ), SLOT( slotContextMenuRequested( QModelIndex, const QPoint& ) ) );
    
    connect( m_view, SIGNAL( headerContextMenuRequested( const QPoint& ) ), SLOT( slotHeaderContextMenuRequested( const QPoint& ) ) );
}

void AccountsView::setZoom( double zoom )
{
    Q_UNUSED( zoom );
}

void AccountsView::init()
{
    QVBoxLayout *l = new QVBoxLayout( this );
    m_view = new AccountsTreeView( this );
    l->addWidget( m_view );
    setProject( m_project );
}

void AccountsView::setupGui()
{
    createOptionAction();
}

void AccountsView::slotContextMenuRequested( QModelIndex, const QPoint &pos )
{
    kDebug();
    slotHeaderContextMenuRequested( pos );
}

void AccountsView::slotHeaderContextMenuRequested( const QPoint &pos )
{
    kDebug();
    QList<QAction*> lst = contextActionList();
    if ( ! lst.isEmpty() ) {
        QMenu::exec( lst, pos,  lst.first() );
    }
}

void AccountsView::slotOptions()
{
    kDebug();
    AccountsviewConfigDialog dlg( m_view, this );
    if ( dlg.exec() == QDialog::Accepted ) {
    }
}

void AccountsView::setProject( Project *project )
{
    model()->setProject( project );
    m_project = project;
}

void AccountsView::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
    model()->setScheduleManager( sm );
}

CostBreakdownItemModel *AccountsView::model() const
{
    return static_cast<CostBreakdownItemModel*>( m_view->model() );
}


#if 0
void AccountsView::print( QPrinter &printer, QPrintDialog &printDialog )
{
    //kDebug();
    uint top, left, bottom, right;
    printer.margins( &top, &left, &bottom, &right );
    //kDebug()<<m.width()<<"x"<<m.height()<<" :"<<top<<","<<left<<","<<bottom<<","<<right<<" :"<<size();
    QPainter p;
    p.begin( &printer );
    p.setViewport( left, top, printer.width() - left - right, printer.height() - top - bottom );
    p.setClipRect( left, top, printer.width() - left - right, printer.height() - top - bottom );
    QRect preg = p.clipRegion().boundingRect();
    //kDebug()<<"p="<<preg;
    //p.drawRect(preg.x(), preg.y(), preg.width()-1, preg.height()-1);
    double scale = qMin( ( double ) preg.width() / ( double ) size().width(), ( double ) preg.height() / ( double ) ( size().height() ) );
    //kDebug()<<"scale="<<scale;
    if ( scale < 1.0 ) {
        p.scale( scale, scale );
}
    QPixmap labelPixmap = QPixmap::grabWidget( m_label );
    p.drawPixmap( m_label->pos(), labelPixmap );
    p.translate( 0, m_label->size().height() );
    m_dlv->paintContents( &p );
    p.end();
}
#endif

bool AccountsView::loadContext( const KoXmlElement &context )
{
    //kDebug();
    m_view->setShowMode( context.attribute( "show-mode" ).toInt() );
    m_view->setCumulative( (bool)( context.attribute( "cumulative" ).toInt() ) );
    m_view->setPeriodType( context.attribute( "period-type", "0" ).toInt() );
    m_view->setStartDate( QDate::fromString( context.attribute( "start-date", "" ), Qt::ISODate ) );
    m_view->setStartMode( context.attribute( "start-mode", "0" ).toInt() );
    m_view->setEndDate( QDate::fromString( context.attribute( "end-date", "" ), Qt::ISODate ) );
    m_view->setEndMode( context.attribute( "end-mode", "0" ).toInt() );
    
    //kDebug()<<m_view->startMode()<<m_view->startDate()<<m_view->endMode()<<m_view->endDate();
    return true;
}

void AccountsView::saveContext( QDomElement &context ) const
{
    //kDebug();
    context.setAttribute( "show-mode", m_view->showMode() );
    context.setAttribute( "cumulative", m_view->cumulative() );
    context.setAttribute( "period-type", m_view->periodType() );
    context.setAttribute( "start-mode", m_view->startMode() );
    context.setAttribute( "start-date", m_view->startDate().toString( Qt::ISODate ) );
    context.setAttribute( "end-mode", m_view->endMode() );
    context.setAttribute( "end-date", m_view->endDate().toString( Qt::ISODate ) );
    
}

KoPrintJob *AccountsView::createPrintJob()
{
    return m_view->createPrintJob( this );
}


}  //KPlato namespace

#include "kptaccountsview.moc"
