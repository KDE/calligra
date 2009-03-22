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
#ifndef KRIMAGEDATA_H
#define KRIMAGEDATA_H
#include "krobjectdata.h"
#include <QRect>
#include <QPainter>
#include <qdom.h>
#include "krpos.h"
#include "krsize.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>
#include <parsexmlutils.h>

namespace Scripting
{
class Image;
}

/**
 @author
*/
class KRImageData : public KRObjectData
{
public:
    KRImageData() {
        createProperties();
    }
    KRImageData(QDomNode & element);
    ~KRImageData() {};
    virtual KRImageData * toImage();
    virtual int type() const;

    void setMode(QString);

    void setInlineImageData(QByteArray, const QString& = QString());
    void setColumn(QString);
    QString mode();
    bool isInline();
    QString inlineImageData();
    QString column();

    ORDataData data() {
        return ORDataData("Data Source", m_controlSource->value().toString());
    }
protected:
    QRect _rect();
    KRSize m_size;
    KoProperty::Property * m_controlSource;
    KoProperty::Property* m_resizeMode;
    KoProperty::Property* m_staticImage;

private:
    static int RTTI;
    virtual void createProperties();
    friend class ORPreRenderPrivate;
    friend class Scripting::Image;
};

#endif
