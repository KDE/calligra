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
#ifndef KRFIELDDATA_H
#define KRFIELDDATA_H
#include "krobjectdata.h"
#include <QRect>
#include <qdom.h>
#include "krsize.h"
#include <parsexmlutils.h>
/**
 @author
*/

namespace Scripting
{
class Field;
}

class KRFieldData : public KRObjectData
{
public:
    KRFieldData() {
        createProperties();
    };
    KRFieldData(QDomNode & element);
    ~KRFieldData();
    virtual KRFieldData * toField();
    virtual int type() const;

    Qt::Alignment textFlags() const;
    void setTextFlags(Qt::Alignment);
    QFont font() const {
        return _font->value().value<QFont>();
    }

    ORDataData data() {
        return ORDataData("Data Source", _controlSource->value().toString());
    }

    void setColumn(const QString&);
    void setTrackTotal(bool);
    void setTrackTotalFormat(const QString &, bool = FALSE);
    void setUseSubTotal(bool);

    QString controlSource() const;

    ORLineStyleData lineStyle();
    ORTextStyleData textStyle();
protected:

    QRect _rect;
    KRSize _size;
    KoProperty::Property * _controlSource;
    KoProperty::Property * _hAlignment;
    KoProperty::Property * _vAlignment;
    KoProperty::Property * _font;
    KoProperty::Property * _trackTotal;
    KoProperty::Property * _trackBuiltinFormat;
    KoProperty::Property * _useSubTotal;
    KoProperty::Property * _trackTotalFormat;
    KoProperty::Property * _fgColor;
    KoProperty::Property * _bgColor;
    KoProperty::Property* _bgOpacity;
    KoProperty::Property* _lnColor;
    KoProperty::Property* _lnWeight;
    KoProperty::Property* _lnStyle;


    //QFont font;
    //int align;
    //ORDataData data;
    //ORTextStyleData textStyle;

    //bool trackTotal;
    //bool sub_total;
    //bool builtinFormat;
    //QString format;

    QStringList fieldNames(const QString &);

private:
    virtual void createProperties();
    static int RTTI;

    friend class ORPreRenderPrivate;
    friend class Scripting::Field;
};

#endif
