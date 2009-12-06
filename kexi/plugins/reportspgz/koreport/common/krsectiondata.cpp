/*
 * Kexi report writer and rendering engine
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


#include "krsectiondata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <QColor>

#include "krlinedata.h"
#include "krfielddata.h"
#include "krtextdata.h"
#include "krbarcodedata.h"
#include "krimagedata.h"
#include "krlabeldata.h"
#include "krchartdata.h"
#include "krcheckdata.h"

KRSectionData::KRSectionData()
{
    createProperties();
}

KRSectionData::KRSectionData(const QDomElement & elemSource)
{
    //TODO surely this could be wriiten in a more sane way!!!!
    
    createProperties();
    m_name = elemSource.tagName();
    setObjectName(m_name);

    if (m_name != "reportheader" && m_name != "reportfooter" &&
            m_name != "pageheader" && m_name != "pagefooter" &&
            m_name != "groupheader" && m_name != "groupfooter" &&
            m_name != "detail") {
        m_valid = false;
        return;
    }
    m_height->setValue(1);

    m_type = None;

    if (m_name == "reportheader")
        m_type = ReportHeader;

    if (m_name == "reportfooter")
        m_type = ReportFooter;

    if (m_name == "pageheader")
        m_type = PageHeaderAny;

    if (m_name == "pagefooter")
        m_type = PageFooterAny;

    if (m_name == "groupheader")
        m_type = GroupHeader;

    if (m_name == "groupfooter")
        m_type = GroupFooter;

    if (m_name == "detail")
        m_type = Detail;

    QDomNodeList section = elemSource.childNodes();
    for (int nodeCounter = 0; nodeCounter < section.count(); nodeCounter++) {
        QDomElement elemThis = section.item(nodeCounter).toElement();
        if (elemThis.tagName() == "height") {
            bool valid;
            qreal height = elemThis.text().toDouble(&valid);
            if (valid)
                m_height->setValue(height);
        } else if (elemThis.tagName() == "bgcolor") {
            m_backgroundColor->setValue(QColor(elemThis.text()));
        } else if (elemThis.tagName() == "firstpage") {
            if (m_name == "pageheader" || m_name == "pagefooter")
                m_extra = elemThis.tagName();

            if (m_name == "pageheader")
                m_type = PageHeaderFirst;
            if (m_name == "pagefooter")
                m_type = PageFooterFirst;
        } else if (elemThis.tagName() == "odd") {
            if (m_name == "pageheader" || m_name == "pagefooter")
                m_extra = elemThis.tagName();

            if (m_name == "pageheader")
                m_type = PageHeaderOdd;
            if (m_name == "pagefooter")
                m_type = PageFooterOdd;
        } else if (elemThis.tagName() == "even") {
            if (m_name == "pageheader" || m_name == "pagefooter")
                m_extra = elemThis.tagName();

            if (m_name == "pageheader")
                m_type = PageHeaderEven;
            if (m_name == "pagefooter")
                m_type = PageFooterEven;
        } else if (elemThis.tagName() == "lastpage") {
            if (m_name == "pageheader" || m_name == "pagefooter")
                m_extra = elemThis.tagName();

            if (m_name == "pageheader")
                m_type = PageHeaderLast;
            if (m_name == "pagefooter")
                m_type = PageFooterLast;
        } else if (elemThis.tagName() == "label") {
            KRLabelData * label = new KRLabelData(elemThis);
            m_objects.append(label);
            //else
            //  delete label;
        } else if (elemThis.tagName() == "field") {
            KRFieldData * field = new KRFieldData(elemThis);
            //if(parseReportField(elemThis, *field) == TRUE)
            //{
            m_objects.append(field);
            //TODO Totals
            //  if(field->trackTotal)
//          sectionTarget.trackTotal.append(field->data);
            //}
            //else
            //  delete field;
        } else if (elemThis.tagName() == "text") {
            KRTextData * text = new KRTextData(elemThis);
            m_objects.append(text);
        } else if (elemThis.tagName() == "line") {
            KRLineData * line = new KRLineData(elemThis);
            m_objects.append(line);
        } else if (elemThis.tagName() == "barcode") {
            KRBarcodeData * bc = new KRBarcodeData(elemThis);
            m_objects.append(bc);

        } else if (elemThis.tagName() == "image") {
            KRImageData * img = new KRImageData(elemThis);
            m_objects.append(img);
        } else if (elemThis.tagName() == "chart") {
            KRChartData * chart = new KRChartData(elemThis);
            m_objects.append(chart);
        } else if (elemThis.tagName() == "check") {
            KRCheckData * check = new KRCheckData(elemThis);
            m_objects.append(check);

        } else
            kDebug() << "While parsing section encountered an unknown element: " << elemThis.tagName();
    }
    qSort(m_objects.begin(), m_objects.end(), zLessThan);
    m_valid = true;
}

KRSectionData::~KRSectionData()
{

}

bool KRSectionData::zLessThan(KRObjectData* s1, KRObjectData* s2)
{
    return s1->Z < s2->Z;
}

bool KRSectionData::xLessThan(KRObjectData* s1, KRObjectData* s2)
{
    return s1->position().toPoint().x() < s2->position().toPoint().x();
}

void KRSectionData::createProperties()
{
    m_set = new KoProperty::Set(0, "Section");

    m_height = new KoProperty::Property("Height", 1.0, "Height", "Height");
    m_backgroundColor = new KoProperty::Property("BackgroundColor", Qt::white, "Background Color", "Background Color");

    m_set->addProperty(m_height);
    m_set->addProperty(m_backgroundColor);
}

QString KRSectionData::name() const
{
    return (m_extra.isEmpty() ? m_name : m_name + "_" + m_extra);
}
