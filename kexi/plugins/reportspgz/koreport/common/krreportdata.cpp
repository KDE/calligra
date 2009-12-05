/*
 * Kexi Report Plugin
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
#include "krreportdata.h"
#include <kdebug.h>
#include <KoUnit.h>
#include <KoDpi.h>
#include "krdetailsectiondata.h"
#include "krobjectdata.h"
#include "parsexmlutils.h"

void KRReportData::init()
{
    m_pgheadFirst = m_pgheadOdd = m_pgheadEven = m_pgheadLast = m_pgheadAny = NULL;
    m_pgfootFirst = m_pgfootOdd = m_pgfootEven = m_pgfootLast = m_pgfootAny = NULL;
    m_rpthead = m_rptfoot = NULL;
}

KRReportData::KRReportData()
 : m_detailsection(0)
{
    init();
    m_valid = true;
}

KRReportData::KRReportData(const QDomElement & elemSource)
 : m_detailsection(0)
{
    m_valid = false;
    init();
    bool valid; //used for local unit conversions

    if (elemSource.tagName() != "koreport") {
        kDebug() << "QDomElement passed to parseReport() was not <koreport> tag";
        kDebug() << elemSource.text();
        return;
    }

    qreal d = 0.0;

    QDomNodeList section = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++) {
        QDomElement elemThis = section.item(nodeCounter).toElement();
        if (elemThis.tagName() == "title")
            m_title = elemThis.text();
        else if (elemThis.tagName() == "datasource") {
            m_query = elemThis.text();
	    m_externalData = elemThis.attribute("external").toInt();
	} else if (elemThis.tagName() == "script") {
            m_script = elemThis.text();
            m_interpreter = elemThis.attribute("interpreter");
        } else if (elemThis.tagName() == "size") {
            if (elemThis.firstChild().isText())
                page.setPageSize(elemThis.firstChild().nodeValue());
            else {
                //bad code! bad code!
                // this code doesn't check the elemts and assums they are what
                // they should be.
                QDomNode n1 = elemThis.firstChild();
                QDomNode n2 = n1.nextSibling();
                if (n1.nodeName() == "width") {
                    page.setCustomWidth(n1.firstChild().nodeValue().toDouble() / 100.0);
                    page.setCustomHeight(n2.firstChild().nodeValue().toDouble() / 100.0);
                } else {
                    page.setCustomWidth(n2.firstChild().nodeValue().toDouble() / 100.0);
                    page.setCustomHeight(n1.firstChild().nodeValue().toDouble() / 100.0);
                }
                page.setPageSize("Custom");
            }
        } else if (elemThis.tagName() == "labeltype")
            page.setLabelType(elemThis.firstChild().nodeValue());
        else if (elemThis.tagName() == "portrait")
            page.setPortrait(true);
        else if (elemThis.tagName() == "landscape")
            page.setPortrait(false);
        else if (elemThis.tagName() == "topmargin") {
            d = elemThis.text().toDouble(&valid);
            if (!m_valid || d < 0.0) {
                //TODO qDebug("Error converting topmargin value: %s",(const char*)elemThis.text());
                d = 50.0;
            }
            page.setMarginTop(POINT_TO_INCH(d) * KoDpi::dpiY());
        } else if (elemThis.tagName() == "bottommargin") {
            d = elemThis.text().toDouble(&valid);
            if (!valid || d < 0.0) {
                //TODO qDebug("Error converting bottommargin value: %s",(const char*)elemThis.text());
                d = 50.0;
            }
            page.setMarginBottom(POINT_TO_INCH(d) * KoDpi::dpiY());
        } else if (elemThis.tagName() == "leftmargin") {
            d = elemThis.text().toDouble(&valid);
            if (!valid || d < 0.0) {
                //TODO qDebug("Error converting leftmargin value: %s",(const char*)elemThis.text());
                d = 50.0;
            }
            page.setMarginLeft(POINT_TO_INCH(d) * KoDpi::dpiX());
        } else if (elemThis.tagName() == "rightmargin") {
            d = elemThis.text().toDouble(&valid);
            if (!valid || d < 0.0) {
                //TODO qDebug("Error converting rightmargin value: %s",(const char*)elemThis.text());
                d = 50.0;
            }
            page.setMarginRight(POINT_TO_INCH(d) * KoDpi::dpiX());
        } else if (elemThis.tagName() == "rpthead") {
            KRSectionData * sd = new KRSectionData(elemThis);
            if (sd->isValid()) {
                m_rpthead = sd;
//TODO Track Totals?    reportTarget.trackTotal += sd->trackTotal;
            } else
                delete sd;
        } else if (elemThis.tagName() == "rptfoot") {
            KRSectionData * sd = new KRSectionData(elemThis);
            if (sd->isValid()) {
                m_rptfoot = sd;
//TODO Track Totals?    reportTarget.trackTotal += sd->trackTotal;
            } else
                delete sd;
        } else if (elemThis.tagName() == "pghead") {
            KRSectionData * sd = new KRSectionData(elemThis);
            if (sd->isValid()) {
                if (sd->extra() == "firstpage")
                    m_pgheadFirst = sd;
                else if (sd->extra() == "odd")
                    m_pgheadOdd = sd;
                else if (sd->extra() == "even")
                    m_pgheadEven = sd;
                else if (sd->extra() == "lastpage")
                    m_pgheadLast = sd;
                else if (sd->extra().isEmpty())
                    m_pgheadAny = sd;
                else {
                    //TODO qDebug("don't know which page this page header is for: %s",(const char*)sd->extra);
                    delete sd;
                }
//TODO Track Totals?    reportTarget.trackTotal += sd->trackTotal;
            } else
                delete sd;
        } else if (elemThis.tagName() == "pgfoot") {
            KRSectionData * sd = new KRSectionData(elemThis);
            if (sd->isValid()) {
                if (sd->extra() == "firstpage")
                    m_pgfootFirst = sd;
                else if (sd->extra() == "odd")
                    m_pgfootOdd = sd;
                else if (sd->extra() == "even")
                    m_pgfootEven = sd;
                else if (sd->extra() == "lastpage")
                    m_pgfootLast = sd;
                else if (sd->extra().isEmpty())
                    m_pgfootAny = sd;
                else {
                    //TODO qDebug("don't know which page this page footer is for: %s",(const char*)sd->extra);
                    delete sd;
                }
//TODO Track Totals?    reportTarget.trackTotal += sd->trackTotal;
            } else
                delete sd;
        } else if (elemThis.tagName() == "section") {
            KRDetailSectionData * dsd = new KRDetailSectionData(elemThis);

            if (dsd->isValid()) {
                m_detailsection = dsd;
                //reportTarget.trackTotal += dsd->trackTotal;
            } else {
                delete dsd;
            }
        }

        //else
        //TODO qDebug("While parsing report encountered an unknown element: %s",(const char*)elemThis.tagName());
    }

    m_valid = true;
}


KRReportData::~KRReportData()
{
}

QList<KRObjectData*> KRReportData::objects() const
{
    QList<KRObjectData*> obs;
    KRSectionData *sec;

    for (int i = 1; i <= KRSectionData::PageFootAny; i++) {
        sec = section((KRSectionData::Section)i);
        if (sec) {
            obs << sec->objects();
        }
    }

    if (m_detailsection) {
        kDebug() << "Number of groups: " << m_detailsection->m_groupList.count();
        foreach(ORDetailGroupSectionData* g, m_detailsection->m_groupList) {
            if (g->head) {
                obs << g->head->objects();
            }
            if (g->foot) {
                obs << g->foot->objects();
            }
        }
        if (m_detailsection->m_detailSection)
            obs << m_detailsection->m_detailSection->objects();
    }

    kDebug() << "Object List:";
    foreach(KRObjectData* o, obs) {
        kDebug() << o->entityName();
    }
    return obs;
}

KRObjectData* KRReportData::object(const QString& n) const
{
    QList<KRObjectData*> obs = objects();

    foreach(KRObjectData* o, obs) {
        if (o->entityName() == n) {
            return o;
        }
    }
    return 0;
}

QList<KRSectionData*> KRReportData::sections() const
{
    QList<KRSectionData*> secs;
    KRSectionData *sec;
    for (int i = 0; i < 12 ; ++i) {
        sec = section((KRSectionData::Section)(i + 1));
        if (sec) {
            secs << sec;
        }
    }

    if (m_detailsection) {
        kDebug() << "Number of groups: " << m_detailsection->m_groupList.count();
        foreach(ORDetailGroupSectionData* g, m_detailsection->m_groupList) {
            if (g->head) {
                secs << g->head;
            }
            if (g->foot) {
                secs << g->foot;
            }
        }
        if (m_detailsection->m_detailSection)
            secs << m_detailsection->m_detailSection;
    }

    return secs;
}

KRSectionData* KRReportData::section(const QString& sn) const
{
    QList<KRSectionData*> secs = sections();

    foreach(KRSectionData *sec, secs) {
        if (sec->name() == sn) {
            return sec;
        }
    }
    return 0;
}

KRSectionData* KRReportData::section(KRSectionData::Section s) const
{
    KRSectionData *sec;
    switch (s) {
    case KRSectionData::PageHeadAny:
        sec = m_pgheadAny;
        break;
    case KRSectionData::PageHeadEven:
        sec = m_pgheadEven;
        break;
    case KRSectionData::PageHeadOdd:
        sec = m_pgheadOdd;
        break;
    case KRSectionData::PageHeadFirst:
        sec = m_pgheadFirst;
        break;
    case KRSectionData::PageHeadLast:
        sec = m_pgheadLast;
        break;
    case KRSectionData::PageFootAny:
        sec = m_pgfootAny;
        break;
    case KRSectionData::PageFootEven:
        sec = m_pgfootEven;
        break;
    case KRSectionData::PageFootOdd:
        sec = m_pgfootOdd;
        break;
    case KRSectionData::PageFootFirst:
        sec = m_pgfootFirst;
        break;
    case KRSectionData::PageFootLast:
        sec = m_pgfootLast;
        break;
    case KRSectionData::ReportHead:
        sec = m_rpthead;
        break;
    case KRSectionData::ReportFoot:
        sec = m_rptfoot;
        break;
    default:
        sec = NULL;
    }
    return sec;
}

