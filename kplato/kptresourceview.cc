/* This file is part of the KDE project
  Copyright (C) 2003 - 2006 Dag Andersen kplato@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation;
  version 2 of the License.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "kptresourceview.h"

#include "kptcalendar.h"
#include "kptduration.h"
#include "kptresourceappointmentsview.h"
#include "kptview.h"
#include "kptnode.h"
#include "kptproject.h"
#include "kpttask.h"
#include "kptresource.h"
#include "kptdatetime.h"
#include "kptcontext.h"

#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QList>
#include <QHeaderView>
#include <QTreeWidget>
#include <QStringList>
#include <QVBoxLayout>

#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

ResListView::ResListView( QWidget * parent )
        : QTreeWidget( parent )
{
    setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), SLOT( slotContextMenuRequested( const QPoint& ) ) );
}

int ResListView::headerHeight() const
{
    return header() ->height();
}
void ResListView::paintToPrinter( QPainter *p, int x, int y, int w, int h )
{
#if 0
    p->save();
    QColor bgc( 193, 223, 255 );
    QBrush bg( bgc );
    p->setBackgroundMode( Qt::OpaqueMode );
    p->setBackgroundColor( bgc );
    QHeaderView *head = header();
    int offset = 0;
    QRect sr;
    // Header shall always be at top/left on page
    for ( int s = 0; s < head->count(); ++s ) {
        sr = head->sectionRect( s );
        if ( offset > sr.x() )
            offset = sr.x();
    }
    for ( int s = 0; s < head->count(); ++s ) {
        sr = head->sectionRect( s );
        if ( offset != 0 ) {
            sr = QRect( sr.x() - offset, sr.y(), sr.width(), sr.height() );
        }
        //kDebug()<<s<<": "<<head->label(s)<<" "<<sr<<endl;
        if ( sr.x() + sr.width() <= x || sr.x() >= x + w ) {
            //kDebug()<<s<<": "<<h->label(s)<<" "<<sr<<": continue"<<endl;
            continue;
        }
        QRect tr = sr;
        if ( sr.x() < x ) {
            tr.setX( x );
            //kDebug()<<s<<": "<<head->label(s)<<" "<<tr<<endl;
        }
        p->eraseRect( tr );
        p->drawText( tr, columnAlignment( s ) | Qt::AlignVCenter, head->label( s ), -1 );
    }
    p->restore();
    p->save();
    p->translate( 0, headerHeight() );
    drawAllContents( p, x, y, w, h );
    p->restore();
#endif
}
int ResListView::calculateY( int ymin, int ymax ) const
{
#if 0
    QList<ResListView::DrawableItem*> drawables;
    QTreeWidgetItem *child = firstChild();
    int level = 0;
    int ypos = 0;
    for ( ; child; child = child->nextSibling() ) {
        ypos = buildDrawables( drawables, level, ypos, child, ymin, ymax );
    }
    int y = 0;
    if ( !drawables.isEmpty() ) {
        DrawableItem * item = drawables.last();
        if ( item ) {
            y = item->y + item->i->height();
        }
    }
    //kDebug()<<k_funcinfo<<y<<" ("<<ymin<<", "<<ymax<<")"<<endl;
    while ( !drawables.isEmpty() ) {
        delete drawables.takeFirst();
    }
    return y;
#endif

    return 0;
}
int ResListView::buildDrawables( QList<ResListView::DrawableItem*> &lst, int level, int ypos, QTreeWidgetItem *item, int ymin, int ymax ) const
{
#if 0
    int y = ypos;
    int ih = item->height();
    if ( y < ymin && y + ih > ymin ) {
        y = ymin; // include partial item at top
    }
    if ( y >= ymin && y + ih < ymax ) { // exclude partial item at bottom
        ResListView::DrawableItem * dr = new ResListView::DrawableItem( level, y, item );
        lst.append( dr );
        //kDebug()<<k_funcinfo<<level<<", "<<y<<" : "<<item->text(0)<<endl;
    }
    y += ih;
    if ( item->isOpen() ) {
        QTreeWidgetItem * child = item->firstChild();
        for ( ; child; child = child->nextSibling() ) {
            y = buildDrawables( lst, level + 1, y, child, ymin, ymax );
        }
    }
    return y;
#endif

    return 0;
}

void ResListView::drawAllContents( QPainter * p, int cx, int cy, int cw, int ch )
{
#if 0
    if ( columns() == 0 ) {
        paintEmptyArea( p, QRect( cx, cy, cw, ch ) );
        return ;
    }
    //kDebug()<<k_funcinfo<<QRect(cx, cy, cw, ch)<<endl;
    QList<ResListView::DrawableItem*> drawables;
    QTreeWidgetItem *child = firstChild();
    int level = 0;
    int ypos = 0;
    for ( ; child; child = child->nextSibling() ) {
        ypos = buildDrawables( drawables, level, ypos, child, cy, cy + ch );
    }

    p->setFont( font() );

    QRect r;
    int fx = -1, x, fc = 0, lc = 0;
    int tx = -1;
    foreach ( ResListView::DrawableItem * current, drawables ) {
        int ih = current->i->height();
        int ith = current->i->totalHeight();
        int c;
        int cs;

        // need to paint current?
        if ( ih > 0 && current->y < cy + ch && current->y + ih > cy ) {
            //kDebug()<<k_funcinfo<<"Paint: "<<current->i->text(0)<<" y="<<current->y<<endl;
            if ( fx < 0 ) {
                // find first interesting column, once
                x = 0;
                c = 0;
                cs = header() ->cellSize( 0 );
                while ( x + cs <= cx && c < header() ->count() ) {
                    x += cs;
                    c++;
                    if ( c < header() ->count() )
                        cs = header() ->cellSize( c );
                }
                fx = x;
                fc = c;
                while ( x < cx + cw && c < header() ->count() ) {
                    x += cs;
                    c++;
                    if ( c < header() ->count() )
                        cs = header() ->cellSize( c );
                }
                lc = c;
            }

            x = fx;
            c = fc;
            // draw to last interesting column

            const QColorGroup &cg = ( palette().inactive() );

            while ( c < lc && !drawables.isEmpty() ) {
                int i = header() ->mapToLogical( c );
                cs = header() ->cellSize( c );
                r.setRect( x, current->y - cy, cs, ih );
                if ( i == 0 )
                    r.setLeft( r.left() + current->l * treeStepSize() );

                p->save();
                // No need to paint if the cell isn't technically visible
                if ( !( r.width() == 0 || r.height() == 0 ) ) {
                    p->translate( r.left(), r.top() );
                    int ac = header() ->mapToLogical( c );
                    // map to Left currently. This should change once we
                    // can really reverse the listview.
                    int align = columnAlignment( ac );
                    if ( align == Qt::AlignAuto )
                        align = Qt::AlignLeft;
                    bool sel = current->i->isSelected();
                    if ( sel )
                        current->i->setSelected( false );
                    current->i->paintCell( p, cg, ac, r.width(), align );
                    if ( sel )
                        current->i->setSelected( sel );
                }
                p->restore();
                x += cs;
                c++;
            }

        }

        const int cell = header() ->mapToActual( 0 );

        if ( tx < 0 )
            tx = header() ->cellPos( cell );

        // do any children of current need to be painted?
        /* FIXME: painting branches doesn't work for some reason...
            if ( ih != ith &&
                rootIsDecorated() &&
                current->y + ith > cy &&
                current->y + ih < cy + ch &&
                tx + current->l * treeStepSize() < cx + cw &&
                tx + (current->l+1) * treeStepSize() > cx ) {
            // compute the clip rectangle the safe way

            int rtop = current->y + ih;
            int rbottom = current->y + ith;
            int rleft = tx + current->l*treeStepSize();
            int rright = rleft + treeStepSize();

            int crtop = qMax( rtop, cy );
            int crbottom = qMin( rbottom, cy+ch );
            int crleft = qMax( rleft, cx );
            int crright = qMin( rright, cx+cw );

            r.setRect( crleft, crtop,
                    crright-crleft, crbottom-crtop );

            if ( r.isValid() ) {
                p->save();
                p->translate( rleft, crtop );
                //kDebug()<<k_funcinfo<<"paintBranches: "<<current->i->text(0)<<endl;

                    current->i->paintBranches( p, colorGroup(), treeStepSize(),
                                            rtop - crtop, r.height() );
                p->restore();
            }
        }*/
        while ( !drawables.isEmpty() ) {
            delete drawables.takeFirst();
        }
    }
#endif
}

void ResListView::slotContextMenuRequested( const QPoint &p )
{
    kDebug() << k_funcinfo << p << endl;
    emit contextMenuRequested( itemAt( p ), mapToGlobal( p ), 0 );
}

class ResourceItemPrivate : public QTreeWidgetItem
{
public:
    ResourceItemPrivate( Resource *r, QTreeWidgetItem *parent )
            : QTreeWidgetItem( parent ),
            resource( r )
    {
        setText( 0, r->name() );
    }

    Resource *resource;

    virtual void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
    {
        QColorGroup g = cg;
        if ( m_columns[ column ] == 1 ) {
            g.setColor( QColorGroup::Text, QColor( Qt::red ) );
            g.setColor( QColorGroup::HighlightedText, QColor( Qt::red ) );
        }

        //QTreeWidgetItem::paintCell(p, g, column, width, align);
    }
    void setColumnState( int c, int state = 1 )
    {
        m_columns[ c ] = state;
    }
private:
    QMap<int, int> m_columns;
};

class NodeItemPrivate : public QTreeWidgetItem
{
public:
    NodeItemPrivate( Task *n, QTreeWidget *parent )
            : QTreeWidgetItem( parent ),
            node( n )
    {
        setText( 0, n->name() );
        init();
    }

    NodeItemPrivate( QString name, QTreeWidget *parent )
            : QTreeWidgetItem( parent ),
            node( 0 )
    {
        setText( 0, name );
        init();
    }

    void setPriority( int col, int prio )
    {
        if ( prioColors.contains( prio ) ) {
            columnPrio.insert( col, prio );
        } else {
            columnPrio.remove( col );
        }
    }
    int priority( int col )
    {
        if ( columnPrio.contains( col ) ) {
            return columnPrio[ col ];
        }
        return 0;
    }

    virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align )
    {
        //kDebug()<<k_funcinfo<<"c="<<column<<" prio="<<(columnPrio.contains(column)?columnPrio[column]:0)<<endl;
        QColorGroup g = cg;
        if ( columnPrio.contains( column ) ) {
            g.setColor( QColorGroup::Base, prioColors[ columnPrio[ column ] ] );
        }
        //QTreeWidgetItem::paintCell(p, g, column, width, align);
    }

    Task *node;
private:
    void init()
    {
        prioColors.insert( 1, QColor( Qt::gray ) );
        prioColors.insert( 2, QColor( Qt::green ) );
        prioColors.insert( 3, QColor( Qt::yellow ) );
        prioColors.insert( 4, QColor( Qt::red ) );
    }
    QMap<int, QColor> prioColors;
    QMap<int, int> columnPrio;
};

class AppointmentItem : public QTreeWidgetItem
{
public:
    AppointmentItem( Appointment *a, QDate &d, QTreeWidgetItem *parent )
            : QTreeWidgetItem( parent ),
            appointment( a ),
    date( d ) {}

    Appointment *appointment;
    QDate date;
};

QSize ResourceView::sizeHint() const
{
    return minimumSizeHint(); // HACK: koshell splitter minimumSize problem
}

//-------------------------------------------------

ResourceView::ResourceView( View *view, QWidget *parent )
        : ViewBase(view, parent),
        m_selectedItem( 0 )
{
    QVBoxLayout *l = new QVBoxLayout( this );
    l->setMargin(0);
    m_splitter = new QSplitter( this );
    l->addWidget( m_splitter );
    m_splitter->setOrientation(Qt::Vertical);

    m_resListView = new ResListView( m_splitter );
    QStringList sl;
    sl << i18n( "Name" )
    << i18n( "Type" )
    << i18n( "Initials" )
    << i18n( "Email" )
    << i18n( "Calendar Name" )
    << i18n( "Available From" )
    << i18n( "Available Until" )
    << i18n( "%" )
    << i18n( "Normal Rate" )
    << i18n( "Overtime Rate" );
    m_resListView->setHeaderLabels( sl );

    //     m_resListView->setItemMargin(2);
    //     m_resListView->setShadeSortColumn(false);
    //     m_resListView->setRootIsDecorated(true);
    //     m_resListView->addColumn(i18n("Name"));
    //     m_resListView->setColumnAlignment(1, Qt::AlignHCenter);
    //     m_resListView->addColumn(i18n("Type"));
    //     m_resListView->setColumnAlignment(2, Qt::AlignHCenter);
    //     m_resListView->addColumn(i18n("Initials"));
    //     m_resListView->setColumnAlignment(3, Qt::AlignLeft);
    //     m_resListView->addColumn(i18n("Email"));
    //     m_resListView->setColumnAlignment(4, Qt::AlignHCenter);
    //     m_resListView->addColumn(i18n("Calendar Name"));
    //     m_resListView->setColumnAlignment(5, Qt::AlignRight);
    //     m_resListView->addColumn(i18n("Available From"));
    //     m_resListView->setColumnAlignment(6, Qt::AlignRight);
    //     m_resListView->addColumn(i18n("Available Until"));
    //     m_resListView->setColumnAlignment(7, Qt::AlignRight);
    //     m_resListView->addColumn(i18n("%"));
    //     m_resListView->setColumnAlignment(8, Qt::AlignRight);
    //     m_resListView->addColumn(i18n("Normal Rate"));
    //     m_resListView->setColumnAlignment(9, Qt::AlignRight);
    //     m_resListView->addColumn(i18n("Overtime Rate"));

    m_showAppointments = false;
    m_appview = new ResourceAppointmentsView( view, m_splitter );
    m_appview->hide();
    draw( view->getProject() );

    connect( m_resListView, SIGNAL( itemSelectionChanged() ), SLOT( resSelectionChanged() ) );
    connect( m_resListView, SIGNAL( itemActivated( QTreeWidgetItem*, int ) ), SLOT( slotItemActivated( QTreeWidgetItem* ) ) );
    //TODO
    connect( m_resListView, SIGNAL( contextMenuRequested( QTreeWidgetItem*, const QPoint&, int ) ), SLOT( popupMenuRequested( QTreeWidgetItem*, const QPoint&, int ) ) );

}

void ResourceView::zoom( double /*zoom*/ )
{}

Resource *ResourceView::currentResource() const
{
    if ( m_selectedItem )
        return m_selectedItem->resource;
    return 0;
}

void ResourceView::draw( Project &project )
{
    //kDebug()<<k_funcinfo<<endl;
    m_resListView->clear();
    m_appview->clear();
    m_selectedItem = 0;

    foreach ( ResourceGroup * gr, project.resourceGroups() ) {
        QTreeWidgetItem * item = new QTreeWidgetItem( m_resListView );
        item->setText( 0, gr->name() );
        //item->setOpen(true);
        drawResources( project, item, gr );
    }
    if ( m_selectedItem ) {
        m_selectedItem->setSelected( true );
    } else {
        resSelectionChanged( m_selectedItem );
    }
}


void ResourceView::drawResources( const Project &proj, QTreeWidgetItem *parent, ResourceGroup *group )
{
    //kDebug()<<k_funcinfo<<"group: "<<group->name()<<" ("<<group<<")"<<endl;
    foreach ( Resource * r, group->resources() ) {
        ResourceItemPrivate * item = new ResourceItemPrivate( r, parent );
        // set column colors
        item->setColumnState( 0, 0 );
        item->setColumnState( 4, 0 );
        item->setColumnState( 5, 0 );
        item->setColumnState( 6, 0 );
        item->setColumnState( 7, 0 );
        if ( r->calendar() == 0 ) {
            item->setColumnState( 0, 1 );
            item->setColumnState( 4, 1 );
        }
        if ( proj.constraint() == Node::MustFinishOn ) {
            if ( proj.mustFinishOn() <= r->availableFrom() ) {
                item->setColumnState( 0, 1 );
                item->setColumnState( 5, 1 );
            }
        } else {
            if ( proj.mustStartOn() >= r->availableUntil() ) {
                item->setColumnState( 0, 1 );
                item->setColumnState( 6, 1 );
            }
        }
        if ( r->units() == 0 ) {
            item->setColumnState( 0, 1 );
            item->setColumnState( 7, 1 );
        }
        // and the texts
        item->setText( 0, r->name() ); // refresh
        switch ( r->type() ) {
            case Resource::Type_Work:
                item->setText( 1, i18n( "Work" ) );
                break;
            case Resource::Type_Material:
                item->setText( 1, i18n( "Material" ) );
                break;
            default:
                item->setText( 1, i18n( "Undefined" ) );
                break;
        }
        item->setText( 2, r->initials() );
        item->setText( 3, r->email() );
        item->setText( 4, r->calendar() ? r->calendar() ->name() : i18n( "None" ) );
        item->setText( 5, KGlobal::locale() ->formatDateTime( r->availableFrom() ) );
        item->setText( 6, KGlobal::locale() ->formatDateTime( r->availableUntil() ) );
        item->setText( 7, QString().setNum( r->units() ) );
        item->setText( 8, KGlobal::locale() ->formatMoney( r->normalRate() ) );
        item->setText( 9, KGlobal::locale() ->formatMoney( r->overtimeRate() ) );
        if ( !m_selectedItem ) {
            m_selectedItem = item;
        }
    }
}


void ResourceView::resSelectionChanged()
{
    //kDebug()<<k_funcinfo<<endl;
    QTreeWidgetItem * i = 0;
    QList<QTreeWidgetItem*> sl = m_resListView->selectedItems();
    if ( !sl.isEmpty() )
        i = sl.first();
    resSelectionChanged( i );
}

void ResourceView::resSelectionChanged( QTreeWidgetItem *item )
{
    //kDebug()<<k_funcinfo<<item<<endl;
    ResourceItemPrivate * ritem = dynamic_cast<ResourceItemPrivate *>( item );
    if ( ritem ) {
        m_selectedItem = ritem;
        if ( m_showAppointments ) {
            m_appview->show();
            m_appview->draw( ritem->resource, m_mainview->getProject().startTime().date(), m_mainview->getProject().endTime().date() );
        } else {
            m_appview->hide();
        }
        return ;
    }
    m_selectedItem = 0;
    m_appview->clear();
}


void ResourceView::slotItemActivated( QTreeWidgetItem* )
{
    emit itemDoubleClicked();
}

void ResourceView::popupMenuRequested( QTreeWidgetItem *item, const QPoint & pos, int )
{
    kDebug()<<k_funcinfo<<pos<<endl;
    ResourceItemPrivate * ritem = dynamic_cast<ResourceItemPrivate *>( item );
    if ( ritem ) {
        if ( ritem != m_selectedItem ) {
            resSelectionChanged( ritem );
        }
        emit requestPopupMenu( "resourceview_popup", pos );
    }
}

QList<int> ResourceView::listOffsets( int pageHeight ) const
{
    QList<int> lst;
#if 0

    int hh = m_resListView->headerHeight();
    int ph = pageHeight - hh;
    int lh = m_resListView->contentsHeight() - hh; // list height ex header.
    int ly = 0;
    kDebug() << k_funcinfo << ly << ", " << lh << endl;
    while ( ly < lh ) {
        lst << ly;
        ly = m_resListView->calculateY( ly + 1, ly + ph ); // offset into the list, ex header
        //kDebug()<<k_funcinfo<<ly<<", "<<lh<<endl;
    }
#endif
    return lst;
}

void ResourceView::print( KPrinter &printer )
{
#if 0
    //kDebug()<<k_funcinfo<<endl;
    uint top, left, bottom, right;
    printer.margins( &top, &left, &bottom, &right );
    //kDebug()<<m.width()<<"x"<<m.height()<<" : "<<top<<", "<<left<<", "<<bottom<<", "<<right<<" : "<<size()<<endl;
    QPainter p;
    p.begin( &printer );
    p.setViewport( left, top, printer.width() - left - right, printer.height() - top - bottom );
    p.setClipRect( left, top, printer.width() - left - right, printer.height() - top - bottom );
    QRect preg = p.clipRegion().boundingRect();
    //kDebug()<<"p="<<preg<<endl;
    //p.drawRect(preg.x(), preg.y(), preg.width()-1, preg.height()-1);
    int ch = m_resListView->contentsHeight();
    int cw = m_resListView->contentsWidth();
    double scale = ( double ) preg.width() / ( double ) ( cw );
    //kDebug()<<"scale="<<scale<<endl;
    if ( scale < 1.0 ) {
        p.scale( scale, scale );
    }
    int ph = preg.height() - m_resListView->headerHeight();
    Q3ValueList<int> lst = listOffsets( preg.height() );
    for ( int i = 0; i < lst.count(); ++i ) {
        //kDebug()<<"Page "<<i+1<<": "<<"scale="<<scale<<" "<<lst[i]<<" : "<<cw<<"x"<<ch<<endl;
        if ( i > 0 ) {
            printer.newPage();
        }
        m_resListView->paintToPrinter( &p, 0, lst[ i ], cw, ph );
    }

    p.end();
#endif
}

bool ResourceView::setContext( Context::Resourceview & /*context*/ )
{
    //kDebug()<<k_funcinfo<<endl;
    return true;
}

void ResourceView::getContext( Context::Resourceview & /*context*/ ) const
{
    //kDebug()<<k_funcinfo<<endl;
}


}  //KPlato namespace

#include "kptresourceview.moc"
