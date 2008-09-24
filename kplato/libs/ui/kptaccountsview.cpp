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

#include <kcalendarsystem.h>
#include <kglobal.h>
#include <klocale.h>

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
    
    m_rightview->header()->setResizeMode ( QHeaderView::ResizeToContents );
            
    hideColumns( m_rightview, QList<int>() << 0 << 1 << 2 );
    slotModelReset();
    
    connect( m, SIGNAL( modelReset() ), SLOT( slotModelReset() ) );
}

void AccountsTreeView::slotModelReset()
{
    hideColumns( m_leftview, QList<int>() << 3 << -1 );
    QHeaderView *v = m_leftview->header();
    kDebug()<<v->sectionSize(2)<<v->sectionSizeHint(2)<<v->defaultSectionSize()<<v->minimumSectionSize();
}

//------------------------
AccountsView::AccountsView( Project *project, KoDocument *part, QWidget *parent )
    : ViewBase( part, parent ),
        m_project(project),
        m_manager( 0 )
{
    init();
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
    model()->setPeriodType( 0 );
    setProject( m_project );
}

void AccountsView::setProject( Project *project )
{
    model()->setProject( project );
    m_project = project;
}

void AccountsView::setScheduleManager( ScheduleManager *sm )
{
    m_manager = sm;
    if ( sm ) {
        model()->setPeriod( m_project->startTime( sm->id() ).date(), m_project->endTime( sm->id() ).date() );
    }
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
    //kDebug()<<"---->"<<endl;
/*    const Context::Accountsview &context = c.accountsview;
    
    QList<int> list;
    list << context.accountsviewsize << context.periodviewsize;
    //m_dlv->setSizes(list); //NOTE: Doesn't always work!
    m_date = context.date;
    if ( !m_date.isValid() )
    m_date = QDate::currentDate();
    m_period = context.period;
    m_cumulative = context.cumulative;
    setContextClosedItems( context );*/
    //kDebug()<<"<----";
    return true;
}

// void AccountsView::setContextClosedItems( const Context::Accountsview &context )
// {
//     for ( QStringList::ConstIterator it = context.closedItems.begin(); it != context.closedItems.end(); ++it ) {
//                 if (m_accounts.findAccount(*it)) {
//                     QTreeWidgetItemIterator lit(m_dlv->masterListView());
//                     for (; lit.current(); ++lit) {
//                         if (lit.current()->text(0) == (*it)) {
//                             m_dlv->setOpen(lit.current(), false);
//                             break;
//                         }
//                     }
//                 }
//     }
// }

void AccountsView::saveContext( QDomElement &context ) const
{
    //kDebug()<<endl;
/*    Context::Accountsview &context = c.accountsview;
    
    context.accountsviewsize = m_dlv->sizes() [ 0 ];
    context.periodviewsize = m_dlv->sizes() [ 1 ];
    context.date = m_date;
    context.period = m_period;
    context.cumulative = m_cumulative;*/
    //kDebug()<<"sizes="<<sizes()[0]<<","<<sizes()[1]<<endl;

    //getContextClosedItems( context, m_dlv->masterListView() ->topLevelItem( 0 ) );
}

// void AccountsView::getContextClosedItems( Context::Accountsview &context, QTreeWidgetItem *item ) const
// {
//     if ( item == 0 )
//         return ;
//         for (QTreeWidgetItem *i = item; i; i = i->nextSibling()) {
//             if (!i->isOpen()) {
//                 context.closedItems.append(i->text(0));
//                 kDebug()<<"add closed"<<i->text(0);
//             }
//             getContextClosedItems(context, i->firstChild());
//         }
// }

void AccountsView::slotConfigure()
{
    //kDebug();
}


}  //KPlato namespace

#include "kptaccountsview.moc"
