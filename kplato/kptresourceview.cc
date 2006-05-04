/* This file is part of the KDE project
   Copyright (C) 2003 - 2004 Dag Andersen kplato@kde.org>

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

#include <q3header.h>
#include <q3popupmenu.h>
#include <qpainter.h>
#include <q3paintdevicemetrics.h>
#include <qstyle.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <Q3ValueList>

#include <k3listview.h>
#include <klocale.h>
#include <kglobal.h>
#include <kprinter.h>

#include <kdebug.h>

namespace KPlato
{

class ResListView : public K3ListView {
public:
    ResListView(QWidget * parent = 0)
    : K3ListView(parent)
    {}

    int headerHeight() const {
        return header()->count() > 0 ? header()->sectionRect(0).height() : 0;
    }
    virtual void paintToPrinter(QPainter *p, int x, int y, int w, int h) {
        p->save();
        QColor bgc(193, 223, 255);
        QBrush bg(bgc);
        p->setBackgroundMode(Qt::OpaqueMode);
        p->setBackgroundColor(bgc);
        Q3Header *head = header();
        int offset = 0;
        QRect sr;
        // Header shall always be at top/left on page
        for (int s = 0; s < head->count(); ++s) {
            sr = head->sectionRect(s);
            if (offset > sr.x())
                offset = sr.x();
        }
        for (int s = 0; s < head->count(); ++s) {
            sr = head->sectionRect(s);
            if (offset != 0) {
                sr = QRect(sr.x()-offset, sr.y(), sr.width(), sr.height());
            }
            //kDebug()<<s<<": "<<head->label(s)<<" "<<sr<<endl;
            if (sr.x()+sr.width() <= x || sr.x() >= x+w) {
                //kDebug()<<s<<": "<<h->label(s)<<" "<<sr<<": continue"<<endl;
                continue;
            }
            QRect tr = sr;
            if (sr.x() < x) {
                tr.setX(x);
                //kDebug()<<s<<": "<<head->label(s)<<" "<<tr<<endl;
            }
            p->eraseRect(tr);
            p->drawText(tr, columnAlignment(s)|Qt::AlignVCenter, head->label(s), -1);
        }
        p->restore();
        p->save();
        p->translate(0, headerHeight());
        drawAllContents(p, x, y, w, h);
        p->restore();
    }
    int calculateY(int ymin, int ymax) const {
        Q3PtrList<ResListView::DrawableItem> drawables;
        drawables.setAutoDelete(true);
        Q3ListViewItem *child = firstChild();
        int level = 0;
        int ypos = 0;
        for (; child; child = child->nextSibling()) {
            ypos = buildDrawables(drawables, level, ypos, child, ymin, ymax);
        }
        int y = 0;
        DrawableItem *item = drawables.getLast();
        if (item) {
            y = item->y + item->i->height();
        }
        //kDebug()<<k_funcinfo<<y<<" ("<<ymin<<", "<<ymax<<")"<<endl;
        return y;
    }
    class DrawableItem {
    public:
        DrawableItem(int level, int ypos, Q3ListViewItem *item ) { y = ypos; l = level; i = item; };
        int y;
        int l;
        Q3ListViewItem * i;
    };
protected:
    int buildDrawables(Q3PtrList<ResListView::DrawableItem> &lst, int level, int ypos, Q3ListViewItem *item, int ymin, int ymax) const {
        int y = ypos;
        int ih = item->height();
        if (y < ymin && y+ih > ymin) {
            y = ymin; // include partial item at top
        }
        if (y >= ymin && y+ih < ymax) { // exclude partial item at bottom
            ResListView::DrawableItem *dr = new ResListView::DrawableItem(level, y, item);
            lst.append(dr);
            //kDebug()<<k_funcinfo<<level<<", "<<y<<" : "<<item->text(0)<<endl;
        }
        y += ih;
        if (item->isOpen()) {
            Q3ListViewItem *child = item->firstChild();
            for (; child; child = child->nextSibling()) {
                y = buildDrawables(lst, level+1, y, child, ymin, ymax);
            }
        }
        return y;
    }
    // This is a copy of QListView::drawContentsOffset(), with a few changes
    // because drawContentsOffset() only draws *visible* items,
    // we want to draw *all* items.
    // FIXME: Haven't got paintBraches() to work, atm live without it.
    virtual void drawAllContents(QPainter * p, int cx, int cy, int cw, int ch) {
        if ( columns() == 0 ) {
            paintEmptyArea( p, QRect( cx, cy, cw, ch ) );
            return;
        }
        //kDebug()<<k_funcinfo<<QRect(cx, cy, cw, ch)<<endl;
        Q3PtrList<ResListView::DrawableItem> drawables;
        drawables.setAutoDelete(true);
        Q3ListViewItem *child = firstChild();
        int level = 0;
        int ypos = 0;
        for (; child; child = child->nextSibling()) {
            ypos = buildDrawables(drawables, level, ypos, child, cy, cy+ch);
        }

        p->setFont( font() );

        Q3PtrListIterator<ResListView::DrawableItem> it(drawables);

        QRect r;
        int fx = -1, x, fc = 0, lc = 0;
        int tx = -1;
        ResListView::DrawableItem * current;

        while ( (current = it.current()) != 0 ) {
            ++it;
            int ih = current->i->height();
            int ith = current->i->totalHeight();
            int c;
            int cs;

            // need to paint current?
            if ( ih > 0 && current->y < cy+ch && current->y+ih > cy ) {
                //kDebug()<<k_funcinfo<<"Paint: "<<current->i->text(0)<<" y="<<current->y<<endl;
                if ( fx < 0 ) {
                    // find first interesting column, once
                    x = 0;
                    c = 0;
                    cs = header()->cellSize( 0 );
                    while ( x + cs <= cx && c < header()->count() ) {
                        x += cs;
                        c++;
                        if ( c < header()->count() )
                            cs = header()->cellSize( c );
                    }
                    fx = x;
                    fc = c;
                    while( x < cx + cw && c < header()->count() ) {
                        x += cs;
                        c++;
                        if ( c < header()->count() )
                            cs = header()->cellSize( c );
                    }
                    lc = c;
                }

                x = fx;
                c = fc;
                // draw to last interesting column

                const QColorGroup &cg = ( palette().inactive() );

                while ( c < lc && !drawables.isEmpty() ) {
                    int i = header()->mapToLogical( c );
                    cs = header()->cellSize( c );
                    r.setRect( x, current->y-cy, cs, ih );
                    if ( i == 0 )
                        r.setLeft( r.left() + current->l * treeStepSize() );

                    p->save();
                    // No need to paint if the cell isn't technically visible
                    if ( !( r.width() == 0 || r.height() == 0 ) ) {
                        p->translate( r.left(), r.top() );
                        int ac = header()->mapToLogical( c );
                        // map to Left currently. This should change once we
                        // can really reverse the listview.
                        int align = columnAlignment( ac );
                        if ( align == Qt::AlignAuto ) align = Qt::AlignLeft;
                        bool sel = current->i->isSelected();
                        if (sel)
                            current->i->setSelected(false);
                        current->i->paintCell( p, cg, ac, r.width(), align );
                        if (sel)
                            current->i->setSelected(sel);
                    }
                    p->restore();
                    x += cs;
                    c++;
                }

            }

            const int cell = header()->mapToActual( 0 );

            if ( tx < 0 )
                tx = header()->cellPos( cell );

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
        }
    }

};

class ResourceItemPrivate : public K3ListViewItem {
public:
    ResourceItemPrivate(Resource *r, Q3ListViewItem *parent)
        : K3ListViewItem(parent, r->name()),
        resource(r) {}

    Resource *resource;

    virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
        QColorGroup g = cg;
        if (m_columns[column] == 1) {
            g.setColor(QColorGroup::Text, QColor(Qt::red));
            g.setColor(QColorGroup::HighlightedText, QColor(Qt::red));
        }

        K3ListViewItem::paintCell(p, g, column, width, align);
    }
    void setColumnState(int c, int state=1) {
        m_columns[c] = state;
    }
private:
    QMap<int, int> m_columns;
};

class NodeItemPrivate : public K3ListViewItem {
public:
    NodeItemPrivate(Task *n, Q3ListView *parent)
    : K3ListViewItem(parent, n->name()),
      node(n) {
        init();
    }

    NodeItemPrivate(QString name, Q3ListView *parent)
    : K3ListViewItem(parent, name),
      node(0) {
        init();
    }

    void setPriority(int col, int prio) {
        if (prioColors.contains(prio)) {
            columnPrio.insert(col, prio);
        } else {
            columnPrio.remove(col);
        }
    }
    int priority(int col) {
        if (columnPrio.contains(col)) {
            return columnPrio[col];
        }
        return 0;
    }

    virtual void paintCell ( QPainter * p, const QColorGroup & cg, int column, int width, int align ) {
        //kDebug()<<k_funcinfo<<"c="<<column<<" prio="<<(columnPrio.contains(column)?columnPrio[column]:0)<<endl;
        QColorGroup g = cg;
        if (columnPrio.contains(column)) {
            g.setColor(QColorGroup::Base, prioColors[columnPrio[column]]);
        }
        K3ListViewItem::paintCell(p, g, column, width, align);
    }

    Task *node;
private:
    void init() {
        prioColors.insert(1, QColor(Qt::gray));
        prioColors.insert(2, QColor(Qt::green));
        prioColors.insert(3, QColor(Qt::yellow));
        prioColors.insert(4, QColor(Qt::red));
    }
    QMap<int, QColor> prioColors;
    QMap<int, int> columnPrio;
};

class AppointmentItem : public K3ListViewItem {
public:
    AppointmentItem(Appointment *a, QDate &d, Q3ListViewItem *parent)
        : K3ListViewItem(parent),
        appointment(a),
        date(d) {}

    Appointment *appointment;
    QDate date;
};

QSize ResourceView::sizeHint() const {
    return minimumSizeHint(); // HACK: koshell splitter minimumSize problem
}

ResourceView::ResourceView(View *view, QWidget *parent)
    : QSplitter(parent, "Resource view"),
    m_mainview(view),
    m_selectedItem(0),
    m_currentNode(0)
{
    setOrientation(Qt::Vertical);

    resList = new ResListView(this, "Resource list");
    resList->setItemMargin(2);
    resList->setShadeSortColumn(false);
    resList->setRootIsDecorated(true);
    resList->addColumn(i18n("Name"));
    resList->setColumnAlignment(1, Qt::AlignHCenter);
    resList->addColumn(i18n("Type"));
    resList->setColumnAlignment(2, Qt::AlignHCenter);
    resList->addColumn(i18n("Initials"));
    resList->setColumnAlignment(3, Qt::AlignLeft);
    resList->addColumn(i18n("Email"));
    resList->setColumnAlignment(4, Qt::AlignHCenter);
    resList->addColumn(i18n("Calendar Name"));
    resList->setColumnAlignment(5, Qt::AlignRight);
    resList->addColumn(i18n("Available From"));
    resList->setColumnAlignment(6, Qt::AlignRight);
    resList->addColumn(i18n("Available Until"));
    resList->setColumnAlignment(7, Qt::AlignRight);
    resList->addColumn(i18n("%"));
    resList->setColumnAlignment(8, Qt::AlignRight);
    resList->addColumn(i18n("Normal Rate"));
    resList->setColumnAlignment(9, Qt::AlignRight);
    resList->addColumn(i18n("Overtime Rate"));

    m_showAppointments = false;
    m_appview = new ResourceAppointmentsView(view, this);
    m_appview->hide();
    draw(view->getProject());

    //connect(resList, SIGNAL(selectionChanged(QListViewItem*)), SLOT(resSelectionChanged(QListViewItem*)));
    connect(resList, SIGNAL(selectionChanged()), SLOT(resSelectionChanged()));
    connect(resList, SIGNAL( contextMenuRequested(Q3ListViewItem*, const QPoint&, int)), SLOT(popupMenuRequested(Q3ListViewItem*, const QPoint&, int)));
    //NOTE: using doubleClicked, not executed() to be consistent with ganttview
    connect(resList, SIGNAL(doubleClicked(Q3ListViewItem*, const QPoint&, int)), SLOT(slotItemDoubleClicked(Q3ListViewItem*)));

}

void ResourceView::zoom(double /*zoom*/)
{
}

Resource *ResourceView::currentResource() {
    if (m_selectedItem)
        return m_selectedItem->resource;
    return 0;
}

void ResourceView::draw(Project &project)
{
    //kDebug()<<k_funcinfo<<endl;
    resList->clear();
    m_appview->clear();
    m_selectedItem = 0;

    Q3PtrListIterator<ResourceGroup> it(project.resourceGroups());
    for (; it.current(); ++it) {
        K3ListViewItem *item = new K3ListViewItem(resList, it.current()->name());
        item->setOpen(true);
        drawResources(project, item, it.current());
    }
    if (m_selectedItem) {
        resList->setSelected(m_selectedItem, true);
    } else {
        resSelectionChanged(m_selectedItem);
    }
}


void ResourceView::drawResources(const Project &proj, Q3ListViewItem *parent, ResourceGroup *group)
{
    //kDebug()<<k_funcinfo<<"group: "<<group->name()<<" ("<<group<<")"<<endl;
    Q3PtrListIterator<Resource> it(group->resources());
    for (; it.current(); ++it) {
        Resource *r = it.current();
        ResourceItemPrivate *item = new ResourceItemPrivate(r, parent);
        // set column colors
        item->setColumnState(0, 0);
        item->setColumnState(4, 0);
        item->setColumnState(5, 0);
        item->setColumnState(6, 0);
        item->setColumnState(7, 0);
        if (r->calendar() == 0) {
            item->setColumnState(0, 1);
            item->setColumnState(4, 1);
        }
        if (proj.constraint() == Node::MustFinishOn) {
            if (proj.mustFinishOn() <= r->availableFrom()) {
                item->setColumnState(0, 1);
                item->setColumnState(5, 1);
            }
        } else {
            if (proj.mustStartOn() >= r->availableUntil()) {
                item->setColumnState(0, 1);
                item->setColumnState(6, 1);
            }
        }
        if (r->units() == 0) {
            item->setColumnState(0, 1);
            item->setColumnState(7, 1);
        }
        // and the texts
        item->setText(0, r->name()); // refresh
        switch (r->type()) {
            case Resource::Type_Work:
                item->setText(1, i18n("Work"));
                break;
            case Resource::Type_Material:
                item->setText(1, i18n("Material"));
                break;
            default:
                item->setText(1, i18n("Undefined"));
                break;
        }
        item->setText(2, r->initials());
        item->setText(3, r->email());
        item->setText(4, r->calendar() ? r->calendar()->name() : i18n("None"));
        item->setText(5, KGlobal::locale()->formatDateTime(r->availableFrom()));
        item->setText(6, KGlobal::locale()->formatDateTime(r->availableUntil()));
        item->setText(7, QString().setNum(r->units()));
        item->setText(8, KGlobal::locale()->formatMoney(r->normalRate()));
        item->setText(9, KGlobal::locale()->formatMoney(r->overtimeRate()));
        if (!m_selectedItem) {
            m_selectedItem = item;
        }
    }
}


void ResourceView::resSelectionChanged() {
    //kDebug()<<k_funcinfo<<endl;
    resSelectionChanged(resList->selectedItem());
}

void ResourceView::resSelectionChanged(Q3ListViewItem *item) {
    //kDebug()<<k_funcinfo<<item<<endl;
    ResourceItemPrivate *ritem = dynamic_cast<ResourceItemPrivate *>(item);
    if (ritem) {
        m_selectedItem = ritem;
        if (m_showAppointments) {
            m_appview->show();
            m_appview->draw(ritem->resource, m_mainview->getProject().startTime().date(), m_mainview->getProject().endTime().date());
        } else {
            m_appview->hide();
        }
        return;
    }
    m_selectedItem = 0;
    m_appview->clear();
}


void ResourceView::slotItemDoubleClicked(Q3ListViewItem*) {
    emit itemDoubleClicked();
}

void ResourceView::popupMenuRequested(Q3ListViewItem* item, const QPoint & pos, int)
{
    ResourceItemPrivate *ritem = dynamic_cast<ResourceItemPrivate *>(item);
    if (ritem) {
        if (ritem != m_selectedItem)
            resSelectionChanged(ritem);
        Q3PopupMenu *menu = m_mainview->popupMenu("resource_popup");
        if (menu)
        {
            menu->exec(pos);
            //kDebug()<<k_funcinfo<<"id="<<id<<endl;
        }
        else
            kDebug()<<k_funcinfo<<"No menu!"<<endl;
    }
}

Q3ValueList<int> ResourceView::listOffsets(int pageHeight) const {
    Q3ValueList<int> lst;
    int hh = resList->headerHeight();
    int ph = pageHeight-hh;
    int lh = resList->contentsHeight() - hh; // list height ex header.
    int ly = 0;
    kDebug()<<k_funcinfo<<ly<<", "<<lh<<endl;
    while (ly < lh) {
        lst << ly;
        ly = resList->calculateY(ly+1, ly+ph); // offset into the list, ex header
        //kDebug()<<k_funcinfo<<ly<<", "<<lh<<endl;
    }
    return lst;
}

void ResourceView::print(KPrinter &printer) {
    //kDebug()<<k_funcinfo<<endl;
    Q3PaintDeviceMetrics m = Q3PaintDeviceMetrics ( &printer );
    uint top, left, bottom, right;
    printer.margins(&top, &left, &bottom, &right);
    //kDebug()<<m.width()<<"x"<<m.height()<<" : "<<top<<", "<<left<<", "<<bottom<<", "<<right<<" : "<<size()<<endl;
    QPainter p;
    p.begin(&printer);
    p.setViewport(left, top, m.width()-left-right, m.height()-top-bottom);
    p.setClipRect(left, top, m.width()-left-right, m.height()-top-bottom);
    QRect preg = p.clipRegion().boundingRect();
    //kDebug()<<"p="<<preg<<endl;
    //p.drawRect(preg.x(), preg.y(), preg.width()-1, preg.height()-1);
    int ch = resList->contentsHeight();
    int cw = resList->contentsWidth();
    double scale = (double)preg.width()/(double)(cw);
    //kDebug()<<"scale="<<scale<<endl;
    if (scale < 1.0) {
        p.scale(scale, scale);
    }
    int ph = preg.height()-resList->headerHeight();
    Q3ValueList<int> lst = listOffsets(preg.height());
    for (int i=0; i < lst.count(); ++i) {
        //kDebug()<<"Page "<<i+1<<": "<<"scale="<<scale<<" "<<lst[i]<<" : "<<cw<<"x"<<ch<<endl;
        if (i > 0) {
            printer.newPage();
        }
        resList->paintToPrinter(&p, 0, lst[i], cw, ph);
    }

    p.end();
}

bool ResourceView::setContext(Context::Resourceview &/*context*/) {
    //kDebug()<<k_funcinfo<<endl;
    return true;
}

void ResourceView::getContext(Context::Resourceview &/*context*/) const {
    //kDebug()<<k_funcinfo<<endl;
}


}  //KPlato namespace

#include "kptresourceview.moc"
