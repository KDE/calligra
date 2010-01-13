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
#ifndef __ORPRERENDER_H__
#define __ORPRERENDER_H__

#include <QDomDocument>
#include <QRectF>
#include <QString>
#include <QFont>
#include <QMap>
#include "koreport_export.h"

class ORPreRenderPrivate;
class ParameterList;
class ORODocument;
class KoReportData;

namespace KexiDB
{
class Connection;
}
//
// ORPreRender
// This class takes a report definition and prerenders the result to
// an ORODocument that can be used to pass to any number of renderers.
//
class KOREPORT_EXPORT ORPreRender
{
public:
//    ORPreRender(KexiDB::Connection*c = 0);
    ORPreRender(const QDomElement&);

    virtual ~ORPreRender();

    void setSourceData(KoReportData*);
    void registerScriptObject(QObject*, const QString&);

    ORODocument * generate();

    /**
    @brief Set the name of the report so that it can be used internally by the script engine
    */
    void setName(const QString &);

    bool isValid() const;

protected:

private:
    ORPreRenderPrivate* d;
    bool setDom(const QDomElement &);
    QMap<QString, QObject*> m_scriptObjects;
};

#endif // __ORPRERENDER_H__
