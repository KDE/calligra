/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FilterDevice.h"

#include <math.h>
#include <qimage.h>
#include <koFilterChain.h>

#include "GfxState.h"
#include "Link.h"

#include "FilterPage.h"
#include "misc.h"


FilterDevice::FilterDevice(FilterData &data)
    : _data(data), _page(0)
{
    _image.ptr = 0;
}

FilterDevice::~FilterDevice()
{
    delete _page;
    for (uint i=0; i<_images.size(); i++)
        delete _images[i].ptr;
}

void FilterDevice::startPage(int, GfxState *)
{
    _page = new FilterPage(_data);
}

void FilterDevice::endPage()
{
    _page->coalesce();
    _page->prepare();
    _page->dump();
    delete _page;
    _page = 0;

    if (_image.ptr) addImage();
    _data.pageIndex++;
}

void FilterDevice::updateFont(GfxState *state)
{
    _page->updateFont(state);
}

void FilterDevice::beginString(GfxState *state, GString *)
{
    _page->beginString(state, state->getCurX(), state->getCurY());
}

void FilterDevice::endString(GfxState *)
{
    _page->endString();
}

void FilterDevice::drawChar(GfxState *state, double x, double y,
                            double dx, double dy, double, double,
                            CharCode, Unicode *u, int uLen)
{
    _page->addChar(state, x, y, dx, dy, u, uLen);
}

void FilterDevice::drawLink(Link* link, Catalog *cat)
{
    double x1, y1, x2, y2, w;
    link->getBorder(&x1, &y1, &x2, &y2, &w);

    int ux1, uy1, ux2, uy2;
    cvtUserToDev(x1, y1, &ux1, &uy1);
    cvtUserToDev(x2, y2, &ux2, &uy2);

    LinkAction *action = link->getAction();
    FilterLink *flink = new FilterLink(ux1 ,ux2, uy1, uy2, *action, *cat);
    _page->addLink(flink);
}

void FilterDevice::addImage()
{
    // check if same image already put at same place (don't know why this
    // append sometimes : related to KWord printing to pdf ?)
    for (uint i=0; i<_images.size(); i++) {
        if ( !equal(_images[i].right, _image.right) ) continue;
        if ( !equal(_images[i].left, _image.left) ) continue;
        if ( !equal(_images[i].top, _image.top) ) continue;
        if ( !equal(_images[i].bottom, _image.bottom) ) continue;
        if ( (*_images[i].ptr)==(*_image.ptr) ) {
            qDebug("image already there !");
            delete _image.ptr;
            _image.ptr = 0;
            return;
        }
    }

    // add image
    QString name = QString("pictures/picture%1.png").arg(_data.imageIndex);
    QDomElement frameset = _data.document.createElement("FRAMESET");
    frameset.setAttribute("frameType", 2);
    frameset.setAttribute("name", QString("Picture %1").arg(_data.imageIndex));
    frameset.setAttribute("frameInfo", 0);
    _data.framesets.appendChild(frameset);

    QDomElement frame = _data.document.createElement("FRAME");
    frame.setAttribute("newFrameBehavior", 1);
    frame.setAttribute("runaround", 0);
    frame.setAttribute("left", _image.left);
    frame.setAttribute("right", _image.right);
    double offset = (_data.pageIndex-1) * _data.pageHeight;
    frame.setAttribute("top", _image.top + offset);
    frame.setAttribute("bottom", _image.bottom + offset);
    frameset.appendChild(frame);

    QDomElement picture = _data.document.createElement("PICTURE");
    picture.setAttribute("keepAspectRatio", "false");
    frameset.appendChild(picture);

    QDomElement key = _data.document.createElement("KEY");
    key.setAttribute("msec", 0);
    key.setAttribute("second", 0);
    key.setAttribute("minute", 0);
    key.setAttribute("hour", 0);
    key.setAttribute("day", 1);
    key.setAttribute("month", 1);
    key.setAttribute("year", 1970);
    key.setAttribute("filename", name);
    picture.appendChild(key);

    key = _data.document.createElement("KEY");
    key.setAttribute("msec", 0);
    key.setAttribute("second", 0);
    key.setAttribute("minute", 0);
    key.setAttribute("hour", 0);
    key.setAttribute("day", 1);
    key.setAttribute("month", 1);
    key.setAttribute("year", 1970);
    key.setAttribute("filename", name);
    key.setAttribute("name", name);
    _data.pictures.appendChild(key);

    KoStoreDevice *sd = _data.chain->storageFile(name, KoStore::Write);
    QImageIO io(sd, "PNG");
    io.setImage(*_image.ptr);
    bool ok = io.write();
    Q_ASSERT(ok);
    sd->close();

    _images.append(_image);
    _data.imageIndex++;
    _image.ptr = 0;
}

void FilterDevice::computeGeometry(GfxState *state, Image &image)
{
    double xt, yt, wt, ht;
    state->transform(0, 0, &xt, &yt);
    state->transformDelta(1, 1, &wt, &ht);
    image.left = xt + (wt>0 ? 0 : wt);
    image.right = image.left + fabs(wt);
    image.top = yt + (ht>0 ? 0 : ht);
    image.bottom = image.top + fabs(ht);

    // #### TODO : take care of image transform (rotation,...)
}

uint FilterDevice::initImage(GfxState *state, int width, int height,
                             bool withMask)
{
    // get image geometry
    Image image;
    image.mask = withMask;
    computeGeometry(state, image);

    // check if new image
    if ( _image.ptr &&
         (_image.ptr->width()!=width || !equal(image.left, _image.left) ||
          !equal(image.right, _image.right) ||
          !equal(image.top, _image.bottom) ||
          !equal(image.mask, _image.mask)) ) addImage();

    qDebug("  same=%i w=%i h=%i left=%f top=%f right=%f bottom=%f",
           _image.ptr!=0, width, height, image.left,
           image.top, image.right, image.bottom);

    uint offset = (_image.ptr ? _image.ptr->height() : 0);
    image.ptr = new QImage(width, offset + height, 32);
    image.ptr->setAlphaBuffer(withMask);
    if (_image.ptr) { // copy previous
        for (int j=0; j<_image.ptr->height(); j++) {
            QRgb *pix = (QRgb *)_image.ptr->scanLine(j);
            QRgb *newPix = (QRgb *)image.ptr->scanLine(j);
            for (int i=0; i<width; i++) newPix[i] = pix[i];
        }
        delete _image.ptr;
        _image.ptr = image.ptr;
        _image.bottom = image.bottom;
    } else _image = image;
    return offset;
}

void FilterDevice::drawImage(GfxState *state, Object *, Stream *str,
                             int width, int height, GfxImageColorMap *colorMap,
                             int *maskColors, GBool inlineImg)
{
    qDebug("image kind=%i inline=%i maskColors=%i", str->getKind(), inlineImg,
           maskColors!=0);
    uint offset = initImage(state, width, height, maskColors!=0);

    // read pixels
    int nbComps = colorMap->getNumPixelComps();
    int nbBits = colorMap->getBits();
    ImageStream *istr = new ImageStream(str, width, nbComps, nbBits);
    istr->reset();
    for (int j=0; j<height; j++) {
        Guchar *p = istr->getLine();
        QRgb *pix = (QRgb *)_image.ptr->scanLine(offset + j);
        for (int i=0; i<width; i++) {
            GfxRGB rgb;
            colorMap->getRGB(p, &rgb);
            int alpha = 255;
            if (maskColors) {
                for (int k=0; k<nbComps; k++)
                    if ( p[k]<maskColors[2*k] || p[k]>maskColors[2*k+1] ) {
                        alpha = 0;
                        break;
                    }
            }
            pix[i] = qRgba(qRound(rgb.r*255), qRound(rgb.g*255),
                           qRound(rgb.b*255), alpha);
            p += nbComps;
        }
    }
    delete istr;
}

void FilterDevice::drawImageMask(GfxState *state, Object *, Stream *str,
                                 int width, int height, GBool invert,
                                 GBool inlineImg)
{
    qDebug("image mask ! kind=%i inline=%i", str->getKind(), inlineImg);
    uint offset = initImage(state, width, height, true);

    // read pixels
    GfxRGB rgb;
    state->getFillRGB(&rgb);
    int red = qRound(rgb.r * 255);
    int green = qRound(rgb.g * 255);
    int blue = qRound(rgb.b * 255);

    ImageStream *istr = new ImageStream(str, width, 1, 1);
    str->reset();
    for (int j=0; j<height; j++) {
        Guchar *p = istr->getLine();
        QRgb *pix = (QRgb *)_image.ptr->scanLine(offset + j);
        for (int i=0; i<width; i++)
            pix[i] = qRgba(red, green, blue, 255 * p[i]);
    }
    delete istr;

    if (invert) _image.ptr->invertPixels();
}
