/*
 * Kexi Report Plugin /
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

#include "krdetailsectiondata.h"
#include "krsectiondata.h"
#include "parsexmlutils.h"
#include <kdebug.h>

KRDetailSectionData::KRDetailSectionData()
{
    m_pageBreak = BreakNone;
    m_detailSection = 0;
}

KRDetailSectionData::KRDetailSectionData(const QDomElement &elemSource)
{
    m_valid = false;
    if (elemSource.tagName() != "section") {
        return;
    }

    bool have_detail = false;

    KRSectionData * old_head = 0;
    KRSectionData * old_foot = 0;

    QDomNodeList section = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++) {
        QDomElement elemThis = section.item(nodeCounter).toElement();
        if (elemThis.tagName() == "group") {
            QDomNodeList nl = elemThis.childNodes();
            QDomNode node;
            ORDetailGroupSectionData * dgsd = new ORDetailGroupSectionData();
            for (int i = 0; i < nl.count(); i++) {
                node = nl.item(i);
                if (node.nodeName() == "column") {
                    dgsd->column = node.firstChild().nodeValue();
                    if (dgsd->head) {
                        dgsd->head->setExtra(dgsd->column);
                    }
                    if (dgsd->foot) {
                        dgsd->foot->setExtra(dgsd->column);
                    }
                } else if (node.nodeName() == "pagebreak") {
                    QDomElement elemThis = node.toElement();
                    QString n = elemThis.attribute("when");
                    if ("after foot" == n)
                        dgsd->pagebreak = ORDetailGroupSectionData::BreakAfterGroupFoot;
                } else if (node.nodeName() == "head") {
                    KRSectionData * sd = new KRSectionData(node.toElement());
                    if (sd->isValid()) {
                        dgsd->head = sd;
                        dgsd->head->setExtra(dgsd->column);
                        //TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
                        //for ( QList<ORDataData>::iterator it = sd->trackTotal.begin(); it != sd->trackTotal.end(); ++it )
                        // dgsd->_subtotCheckPoints[*it] = 0.0;
                    } else
                        delete sd;
                } else if (node.nodeName() == "foot") {
                    KRSectionData * sd = new KRSectionData(node.toElement());
                    if (sd->isValid()) {
                        dgsd->foot = sd;
                        dgsd->foot->setExtra(dgsd->column);
                        //TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
                        //for ( QList<ORDataData>::iterator it = sd->trackTotal.begin(); it != sd->trackTotal.end(); ++it )
                        // dgsd->_subtotCheckPoints[*it] = 0.0;
                    } else
                        delete sd;
                }
                //else
                //TODO qDebug("While parsing group section encountered an unknown element: %s", node.nodeName().toLatin1());
            }
            m_groupList.append(dgsd);
        } else if (elemThis.tagName() == "detail") {
            KRSectionData * sd = new KRSectionData(elemThis);
            if (sd->isValid()) {
                m_detailSection = sd;
                //TODO Track Totals?sectionTarget.trackTotal += sd->trackTotal;
                have_detail = true;
            } else
                delete sd;
        }
        kDebug() << "While parsing detail section encountered an unknown element: " << elemThis.tagName();
    }

    m_valid = true;
}

KRDetailSectionData::~KRDetailSectionData()
{
}


