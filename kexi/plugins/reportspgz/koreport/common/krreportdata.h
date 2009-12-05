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

#ifndef KRREPORTDATA_H
#define KRREPORTDATA_H

#include <QObject>
#include <qdom.h>
#include "krsectiondata.h"
#include "reportpageoptions.h"
#include "parsexmlutils.h"

class KRDetailSectionData;

namespace Scripting
{
class Report;
}
/**
 @author Adam Pigg <adam@piggz.co.uk>
*/
class KRReportData : public QObject
{
    Q_OBJECT

public:
    explicit KRReportData(const QDomElement & elemSource);
    KRReportData();
    ~KRReportData();

    bool isValid() const { return m_valid; }
    
    /**
    \return a list of all objects in the report
    */
    QList<KRObjectData*> objects() const;

    /**
    \return a report object given its name
    */
    KRObjectData* object(const QString&) const;

    /**
    \return all the sections, including groups and detail
    */
    QList<KRSectionData*> sections() const;

    /**
    \return a sectiondata given a section enum
    */
    KRSectionData* section(KRSectionData::Section) const;

    /**
    \return a sectiondata given its name
    */
    KRSectionData* section(const QString&) const;

    QString query() const {
        return m_query;
    }
    QString script() const {
        return m_script;
    };
    QString interpreter() const {
        return m_interpreter;
    }
    
    bool externalData() const {
      return m_externalData;
    }
    
    KRDetailSectionData* detail() const {
        return detailsection;
    }
    
    void setName(const QString&n){m_name = n;}
    QString name() const {return m_name;}
    
protected:
    QString m_title;
    QString m_name;
    QString m_query;
    QString m_script;
    QString m_interpreter;
    bool m_externalData;

    ReportPageOptions page;

    KRSectionData * pghead_first;
    KRSectionData * pghead_odd;
    KRSectionData * pghead_even;
    KRSectionData * pghead_last;
    KRSectionData * pghead_any;

    KRSectionData * rpthead;
    KRSectionData * rptfoot;

    KRSectionData * pgfoot_first;
    KRSectionData * pgfoot_odd;
    KRSectionData * pgfoot_even;
    KRSectionData * pgfoot_last;
    KRSectionData * pgfoot_any;

    KRDetailSectionData* detailsection;
private:
    bool m_valid;
    void init();

    friend class ORPreRenderPrivate;
    friend class ORPreRender;
    friend class KRScriptHandler;
    friend class Scripting::Report;
//    QList<ORDataData> trackTotal;
};

#endif
