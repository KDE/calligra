/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "reportsectiondetail.h"
#include "reportsectiondetailgroup.h"
#include "reportsection.h"
#include <qdom.h>
#include <kdebug.h>

//
// ReportSectionDetail
//
ReportSectionDetail::ReportSectionDetail(ReportDesigner * rptdes, const char * name)
        : QWidget(rptdes)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _pagebreak = BreakNone;
    vboxlayout = new QVBoxLayout(this);
    vboxlayout->setSpacing(0);
    vboxlayout->setMargin(0);
    _rd = rptdes;
    _detail = new ReportSection(rptdes /*, this*/);
    vboxlayout->addWidget(_detail);

    this->setLayout(vboxlayout);
}
ReportSectionDetail::~ReportSectionDetail()
{
    // Qt should be handling everything for us
    _rd = 0;
}

int ReportSectionDetail::pageBreak() const
{
    return _pagebreak;
}
void ReportSectionDetail::setPageBreak(int pb)
{
    _pagebreak = pb;
}

ReportSection * ReportSectionDetail::getDetail()
{
    return _detail;
}

void ReportSectionDetail::buildXML(QDomDocument & doc, QDomElement & section)
{
    if (pageBreak() != ReportSectionDetail::BreakNone) {
        QDomElement spagebreak = doc.createElement("pagebreak");
        if (pageBreak() == ReportSectionDetail::BreakAtEnd)
            spagebreak.setAttribute("when", "at end");
        section.appendChild(spagebreak);
    }

    for (unsigned int i = 0; i < groupList.count(); i++) {
        ReportSectionDetailGroup * rsdg = groupList.at(i);
        QDomNode grp = doc.createElement("group");

        QDomNode gcol = doc.createElement("column");
        gcol.appendChild(doc.createTextNode(rsdg->column()));
        grp.appendChild(gcol);

        if (rsdg->pageBreak() != ReportSectionDetailGroup::BreakNone) {
            QDomElement pagebreak = doc.createElement("pagebreak");
            if (rsdg->pageBreak() == ReportSectionDetailGroup::BreakAfterGroupFooter)
                pagebreak.setAttribute("when", "after foot");
            grp.appendChild(pagebreak);
        }

        //group head
        if (rsdg->isGroupHeadShowing()) {
            QDomElement ghead = doc.createElement("head");
            rsdg->getGroupHead()->buildXML(doc, ghead);
            grp.appendChild(ghead);
        }
        // group foot
        if (rsdg->isGroupFootShowing()) {
            QDomElement gfoot = doc.createElement("foot");
            rsdg->getGroupFoot()->buildXML(doc, gfoot);
            grp.appendChild(gfoot);
        }

        section.appendChild(grp);
    }

    // detail section
    QDomElement gdetail = doc.createElement("detail");
    _detail->buildXML(doc, gdetail);
    section.appendChild(gdetail);
}

void ReportSectionDetail::initFromXML(QDomNode & section)
{
    QDomNodeList nl = section.childNodes();
    QDomNode node;
    QString n;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "pagebreak") {
            QDomElement eThis = node.toElement();
            if (eThis.attribute("when") == "at end")
                setPageBreak(BreakAtEnd);
        } else if (n == "group") {
            ReportSectionDetailGroup * rsdg = new ReportSectionDetailGroup("unnamed", this, this);
            QDomNodeList gnl = node.childNodes();
            QDomNode gnode;
            bool show_head = false;
            bool show_foot = false;
            for (int gi = 0; gi < gnl.count(); gi++) {
                gnode = gnl.item(gi);

                if (gnode.nodeName() == "column") {
                    rsdg->setColumn(gnode.firstChild().nodeValue());
                } else if (gnode.nodeName() == "pagebreak") {
                    QDomElement elemThis = gnode.toElement();
                    QString n = elemThis.attribute("when");
                    if ("after foot" == n)
                        rsdg->setPageBreak(ReportSectionDetailGroup::BreakAfterGroupFooter);
                } else if (gnode.nodeName() == "head") {
                    rsdg->getGroupHead()->initFromXML(gnode);
                    rsdg->showGroupHead(true);
                    show_head = true;
                } else if (gnode.nodeName() == "foot") {
                    rsdg->getGroupFoot()->initFromXML(gnode);
                    rsdg->showGroupFoot(true);
                    show_foot = true;
                } else {
                    kDebug() << "encountered unknown element while parsing group element: " << gnode.nodeName();
                }
            }
            insertSection(groupSectionCount(), rsdg);
            rsdg->showGroupHead(show_head);
            rsdg->showGroupFoot(show_foot);
        } else if (n == "detail") {
            _detail->initFromXML(node);
        } else {
            // unknown element
            kDebug() << "while parsing section encountered and unknown element: " <<  n;
        }
    }

}

ReportDesigner * ReportSectionDetail::reportDesigner()
{
    return _rd;
}

int ReportSectionDetail::groupSectionCount()
{
    return groupList.count();
}

ReportSectionDetailGroup * ReportSectionDetail::getSection(int i)
{
    return groupList.at(i);
}

void ReportSectionDetail::insertSection(int idx, ReportSectionDetailGroup * rsd)
{
    groupList.insert(idx, rsd);

    rsd->getGroupHead()->setParent(this);
    rsd->getGroupFoot()->setParent(this);

    idx = 0;
    int gi = 0;
    for (gi = 0; gi < (int) groupList.count(); gi++) {
        rsd = groupList.at(gi);
        vboxlayout->removeWidget(rsd->getGroupHead());
        vboxlayout->insertWidget(idx, rsd->getGroupHead());
        idx++;
    }
    vboxlayout->removeWidget(_detail);
    vboxlayout->insertWidget(idx, _detail);
    idx++;
    for (gi = ((int) groupList.count() - 1); gi >= 0; gi--) {
        rsd = groupList.at(gi);
        vboxlayout->removeWidget(rsd->getGroupFoot());
        vboxlayout->insertWidget(idx, rsd->getGroupFoot());
        idx++;
    }

    if (_rd) _rd->setModified(true);
    adjustSize();
}

int ReportSectionDetail::findSection(const QString & name)
{
    // find the item by its name
    ReportSectionDetailGroup * rsd = 0;
    for (unsigned int i = 0; i < groupList.count(); i++) {
        rsd = groupList.at(i);
        if (name == rsd->column()) return i;
    }
    return -1;
}

void ReportSectionDetail::removeSection(int idx, bool del)
{
    ReportSectionDetailGroup * rsd = groupList.at(idx);

    vboxlayout->removeWidget(rsd->getGroupHead());
    vboxlayout->removeWidget(rsd->getGroupFoot());

    groupList.removeAt(idx);

    if (_rd) _rd->setModified(TRUE);
    if (del) delete rsd;
    adjustSize();
}

QSize ReportSectionDetail::sizeHint() const
{
    QSize s;
    ReportSectionDetailGroup * rsdg = 0;
    for (int gi = 0; gi < (int) groupList.count(); gi++) {
        rsdg = groupList.at(gi);
        if (rsdg->isGroupHeadShowing()) s += rsdg->getGroupHead()->size();
        if (rsdg->isGroupFootShowing()) s += rsdg->getGroupFoot()->size();
    }
    return s += _detail->size();
}
