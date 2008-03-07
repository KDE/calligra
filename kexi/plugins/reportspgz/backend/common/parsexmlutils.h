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

/*
 *     This is a collection of various functions used to parse the
 * different xml structures used in the Report definitions.  Most
 * of these are structures that are common to several or the complex
 * structures.
 */

#ifndef __PARSEXMLUTILS_H__
#define __PARSEXMLUTILS_H__

// forward declarations
class QDomElement;
class KRObjectData;
class KRLineData;
class KRLabelData;
class KRFieldData;
class KRTextData;
class KRBarcodeData;
class KRImageData;

#include "reportpageoptions.h"
			  
#include <qstring.h>
#include <QRectF>
#include <qfont.h>
#include <qcolor.h>
#include <qlist.h>
#include <qmap.h>

#include <QList>
#include <QPair>
#include "../../pgzkexireportpart2_export.h"

//
// Data Structures
//
class PGZKEXIREPORTPART2_LIB_EXPORT ORDataData
{
  public:
    ORDataData()
    {
      query = QString::null;
      column = QString::null;
    }
    ORDataData(const QString & q, const QString & c)
    {
      query = q;
      column = c;
    }
    ORDataData(const ORDataData & d)
    {
      query = d.query;
      column = d.column;
    }

    ORDataData & operator=(const ORDataData & d)
    {
      query = d.query;
      column = d.column;
      return *this;
    }

    bool operator==(const ORDataData & d) const
    {
      return ((query == d.query) && (column == d.column));
    }

    bool operator< (const ORDataData & d) const
    {
      if((query < d.query) || (query == d.query && column < d.column))
        return true;
      return false;
    }

    QString query;
    QString column;
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORTitleData
{
  public:
    QString string;
    QFont font;
    bool font_defined;
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORTextStyleData
{
        public:
		QFont font;
		Qt::Alignment textFlags;
                QColor bgColor;
                QColor fgColor;
		int bgOpacity;
                
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORLineStyleData
{
	public:
		int weight;
		QColor lnColor;
		Qt::PenStyle style;  
};

//
// ORSectionData is used to store the information about a specific
// section. A section has a name and optionally extra data. `name'
// rpthead, rptfoot, pghead, pgfoot, grphead, grpfoot or detail.
// In the case of pghead and pgfoot extra would contain the page
// designation (firstpage, odd, even or lastpage).
//
class PGZKEXIREPORTPART2_LIB_EXPORT ORSectionData
{
  public:
    QString name;
    QString extra; // extra info about the section
    qreal height;
    QColor bgColor;
    
    QList<KRObjectData*> objects;
    QList<ORDataData> trackTotal;
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORDetailGroupSectionData
{
  public:
    ORDetailGroupSectionData();

    enum PageBreak {
      BreakNone = 0,
      BreakAfterGroupFoot = 1
    };

    QString name;
    QString column;
    int pagebreak;

    QMap<ORDataData,qreal> _subtotCheckPoints;

    ORSectionData * head;
    ORSectionData * foot;
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORDetailSectionData
{
  public:
    ORDetailSectionData();

    enum PageBreak {
      BreakNone = 0,
      BreakAtEnd = 1
    };

    QString name;
    int pagebreak;

    ORSectionData * detail;

    QList<ORDetailGroupSectionData*> groupList;
    QList<ORDataData> trackTotal;
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORParameter
{
  public:
    ORParameter() : active(false) {};
    ORParameter(const QString & n) : name(n), active(false) {}
    ORParameter(const ORParameter & d)
    {
      name = d.name;
      type = d.type;
      defaultValue = d.defaultValue;
      description = d.description;
      listtype = d.listtype;
      query = d.query;
      values = d.values;
      active = d.active;
    }

    ORParameter & operator=(const ORParameter & d)
    {
      name = d.name;
      type = d.type;
      defaultValue = d.defaultValue;
      description = d.description;
      listtype = d.listtype;
      query = d.query;
      values = d.values;
      active = d.active;
      return *this;
    }

    QString name;
    QString type;
    QString defaultValue;
    QString description;
    QString listtype;
    QString query;
    QList<QPair<QString,QString> > values;
    bool active;
};

class PGZKEXIREPORTPART2_LIB_EXPORT ORReportData
{
  public:
    ORReportData();

    QString title;
//    QString description;

    QMap<QString,ORParameter> definedParams;

    ReportPageOptions page;
    QString query;

    ORSectionData * pghead_first;
    ORSectionData * pghead_odd;
    ORSectionData * pghead_even;
    ORSectionData * pghead_last;
    ORSectionData * pghead_any;

    ORSectionData * rpthead;
    ORSectionData * rptfoot;

    ORSectionData * pgfoot_first;
    ORSectionData * pgfoot_odd;
    ORSectionData * pgfoot_even;
    ORSectionData * pgfoot_last;
    ORSectionData * pgfoot_any;

    QList<ORDetailSectionData*> sections;
//    QMap<QString, QColor> color_map;
    QList<ORDataData> trackTotal;
};


bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportRect(const QDomElement &, QRectF &);
bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportFont(const QDomElement &, QFont &);

bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportData(const QDomElement &, ORDataData &);

bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportTextStyleData(const QDomElement &, ORTextStyleData &);
bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportLineStyleData(const QDomElement &, ORLineStyleData &);

bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportSection(const QDomElement &, ORSectionData &);
bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportDetailSection(const QDomElement &, ORDetailSectionData &);
bool PGZKEXIREPORTPART2_LIB_EXPORT parseReport(const QDomElement &, ORReportData &);
bool PGZKEXIREPORTPART2_LIB_EXPORT parseReportParameter(const QDomElement &, ORReportData &);

#endif
