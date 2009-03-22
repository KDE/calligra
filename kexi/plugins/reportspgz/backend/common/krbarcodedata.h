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
#ifndef KRBARCODEDATA_H
#define KRBARCODEDATA_H
#include "krobjectdata.h"
#include <QRect>
#include <QPainter>
#include <qdom.h>
#include "krpos.h"
#include "krsize.h"
#include <parsexmlutils.h>

namespace Scripting
{
class Barcode;
}
/**
 @author
*/
class KRBarcodeData : public KRObjectData
{
public:
    KRBarcodeData() {
        createProperties();
    }
    KRBarcodeData(QDomNode & element);
    ~KRBarcodeData();
    virtual KRBarcodeData * toBarcode();
    virtual int type() const;

    QString column();
    int alignment();
    void setAlignment(int);
    int maxLength();
    void setMaxLength(int i);
    QString format();
    void setFormat(const QString&);

    ORDataData data() {
        return ORDataData("Data Source", m_controlSource->value().toString());
    }

protected:
    QRect _rect();
    KRSize m_size;

    KoProperty::Property * m_controlSource;
    KoProperty::Property * m_horizontalAlignment;
    KoProperty::Property * m_format;
    KoProperty::Property * m_maxLength;

    // all these values are in inches and
    // are for internal use only
    qreal min_width_data;
    qreal min_width_total;
    qreal min_height;

private:
    virtual void createProperties();
    static int RTTI;

    friend class ORPreRenderPrivate;
    friend class Scripting::Barcode;
};

#endif
