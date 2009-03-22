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
#ifndef KRLINEDATA_H
#define KRLINEDATA_H
#include "krobjectdata.h"
#include <QRect>
#include <QPainter>
#include <qdom.h>
#include "krpos.h"
#include "krsize.h"
#include "parsexmlutils.h"

namespace Scripting
{
class Line;
}

/**
 @author
*/
class KRLineData : public KRObjectData
{
public:
    KRLineData() {
        createProperties();
    }
    KRLineData(QDomNode & element);
    ~KRLineData() {};
    virtual int type() const;
    virtual KRLineData * toLine();
    ORLineStyleData lineStyle();
    unsigned int weight() const;
    void setWeight(int w);

protected:
    KRPos m_start;
    KRPos m_end;
    KoProperty::Property *m_lineColor;
    KoProperty::Property *m_lineWeight;
    KoProperty::Property *m_lineStyle;
    virtual void createProperties();
private:

    static int RTTI;

    friend class ORPreRenderPrivate;
    friend class Scripting::Line;
};

#endif
