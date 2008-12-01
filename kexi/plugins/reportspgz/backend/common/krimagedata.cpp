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
#include "krimagedata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kglobalsettings.h>
#include <QBuffer>
#include <kcodecs.h>

KRImageData::KRImageData(QDomNode & element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;

    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        if (n == "data") {
            // see "string" just below for comments on String vs. Data
            QDomNodeList dnl = node.childNodes();
            for (int di = 0; di < dnl.count(); di++) {
                node = dnl.item(di);
                n = node.nodeName();
                if (n == "controlsource") {
                    _controlSource->setValue(node.firstChild().nodeValue());
                } else {
                    kDebug() << "while parsing field data, encountered unknown element: " << n;
                }
            }
        } else if (n == "name") {
            _name->setValue(node.firstChild().nodeValue());
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "mode") {
            setMode(node.firstChild().nodeValue());
        } else if (n == "map") {
            // should read the format in but it will just be reset by the setImageData
            // method
            kDebug() << "Loading Image Data";
            setInlineImageData(node.firstChild().nodeValue().toLatin1());
        } else if (n == "rect") {
            QDomNodeList rnl = node.childNodes();
            qreal x, y, w, h = 0.0;
            for (int ri = 0; ri < rnl.count(); ri++) {
                node = rnl.item(ri);
                n = node.nodeName();
                if (n == "x") {
                    x = node.firstChild().nodeValue().toFloat();
                    x = ((x - (int) x) < 0.5 ? (int) x : (int) x + 1);
                } else if (n == "y") {
                    y = node.firstChild().nodeValue().toFloat();
                    y = ((y - (int) y) < 0.5 ? (int) y : (int) y + 1);
                } else if (n == "width") {
                    w = node.firstChild().nodeValue().toFloat();
                    w = ((w - (int) w) < 0.5 ? (int) w : (int) w + 1);
                } else if (n == "height") {
                    h = node.firstChild().nodeValue().toFloat();
                    h = ((h - (int) h) < 0.5 ? (int) h : (int) h + 1);
                } else {
                    kDebug() << "While parsing rect encountered unknown element: " << n;
                }
            }
            _pos.setPointPos(QPointF(x, y));
            _size.setPointSize(QSizeF(w, h));
        } else {
            kDebug() << "while parsing image element encountered unknown element: " << n;
        }
    }
}

bool KRImageData::isInline()
{
    return !(inlineImageData().isEmpty());
}

QString KRImageData::inlineImageData()
{
    QPixmap pixmap = _staticImage->value().value<QPixmap>();
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(IO_ReadWrite);
    pixmap.save(&buffer, "PNG");   // writes pixmap into ba in PNG format,
    //TODO should i remember the format used, or save as PNG as its lossless?

    QByteArray imageEncoded(KCodecs::base64Encode(buffer.buffer(), true));
    return imageEncoded;
}

void KRImageData::setInlineImageData(QByteArray dat, const QString &fn)
{


    if (!fn.isEmpty()) {
        QPixmap pix(fn);
        if (!pix.isNull())
            _staticImage->setValue(pix);
        else {
            QPixmap blank(1,1);
            blank.fill();
            _staticImage->setValue(blank);
        }
    } else {

        QByteArray binaryStream;

        binaryStream = KCodecs::base64Decode(dat);
        QImage img(binaryStream);
        QPixmap pix;
        pix.convertFromImage(img, QPixmap::Color);

        _staticImage->setValue(pix);

    }

}

QString KRImageData::mode()
{
    return _resizeMode->value().toString();
}
void KRImageData::setMode(QString m)
{
    if (mode() != m) {
        _resizeMode->setValue(m);
    }
}

void KRImageData::createProperties()
{
    _set = new KoProperty::Set(0, "Image");

    _controlSource = new KoProperty::Property("ControlSource", QStringList(), QStringList(), "", "Control Source");

    QStringList keys, strings;
    keys << "Clip" << "Stretch";
    strings << i18n("Clip") << i18n("Stretch");
    _resizeMode = new KoProperty::Property("Mode", keys, strings, "Clip", "Resize Mode");

    _staticImage = new KoProperty::Property("StaticImage", QPixmap(), "Static Image", "Static Image");

    _set->addProperty(_name);
    _set->addProperty(_controlSource);
    _set->addProperty(_resizeMode);
    _set->addProperty(_pos.property());
    _set->addProperty(_size.property());
    _set->addProperty(_staticImage);
}


void KRImageData::setColumn(QString c)
{
    _controlSource->setValue(c);
}

QString KRImageData::column()
{
    return _controlSource->value().toString();
}

int KRImageData::type() const
{
    return RTTI;
}
int KRImageData::RTTI = KRObjectData::EntityImage;
KRImageData* KRImageData::toImage()
{
    return this;
}
