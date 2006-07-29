/*
 * Copyright (c) 2002 Nicolas HADACEK (hadacek@kde.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "FilterDevice.h"

#include <math.h>
#include <KoFilterChain.h>
#include <kdebug.h>

#include "GfxState.h"
#include "Link.h"

#include "FilterPage.h"
#include "data.h"
#include "dialog.h"


namespace PDFImport
{

Device::Device(Data &data)
    : _data(data), _fillColor(Qt::white), _strokeColor(Qt::black)
{
    _pages.setAutoDelete(true);
}

Device::~Device()
{
    clear();
}

void Device::clear()
{
    _images.clear();
}

void Device::init()
{
    // get some global infos on frames size
    const double maxH = _data.pageRect().bottom();
    const double maxR = _data.pageRect().right();

    double maxHeaderBottom = 0;
    double minBodyTop = maxH;
    double minHeaderBodyDelta = maxH;
    double maxBodyBottom = 0;
    double minFooterTop = maxH;
    double minBodyFooterDelta = maxH;
    double minLeft = maxR;
    double maxRight = 0;

    for (Page *page = _pages.first(); page; page = _pages.next()) {
        const DRect &hr = page->rects()[Header];
        const DRect &br = page->rects()[Body];
        const DRect &fr = page->rects()[Footer];
        if ( hr.isValid() ) {
            maxHeaderBottom = kMax(maxHeaderBottom, hr.bottom());
            if ( br.isValid() )
                minHeaderBodyDelta =
                    kMin(minHeaderBodyDelta, br.top() - hr.bottom());
            minLeft = kMin(minLeft, hr.left());
            maxRight = kMax(maxRight, hr.right());
        }
        if ( fr.isValid() ) {
            minFooterTop = kMin(minFooterTop, fr.top());
            if ( br.isValid() )
                minBodyFooterDelta =
                    kMin(minBodyFooterDelta, fr.top() - br.bottom());
            minLeft = kMin(minLeft, fr.left());
            maxRight = kMax(maxRight, fr.right());
        }
        if ( br.isValid() ) {
            minBodyTop = kMin(minBodyTop, br.top());
            maxBodyBottom = kMax(maxBodyBottom, br.bottom());
            minLeft = kMin(minLeft, br.left());
            maxRight = kMax(maxRight, br.right());
        }
    }

    // set minimal top and maximal bottom to body frame
    double minTop = kMax(maxHeaderBottom+minHeaderBodyDelta, minBodyTop);
    double maxBottom = kMin(minFooterTop-minBodyFooterDelta, maxBodyBottom);
    for (Page *page = _pages.first(); page; page = _pages.next()) {
        DRect &r = page->rects()[Body];
        if ( r.top()>minTop ) r.setTop(minTop);
        if ( r.bottom()<maxBottom ) r.setBottom(maxBottom);
    }

    // set minimal left and maximal right for header and for footer
    for (Page *page = _pages.first(); page; page = _pages.next()) {
        DRect &hr = page->rects()[Header];
        if ( hr.isValid() ) {
            if ( hr.left()>minLeft ) hr.setLeft(minLeft);
            if ( hr.right()<maxRight ) hr.setRight(maxRight);
        }
        DRect &fr = page->rects()[Footer];
        if ( fr.isValid() ) {
            if ( fr.left()>minLeft ) fr.setLeft(minLeft);
            if ( fr.right()<maxRight ) fr.setRight(maxRight);
        }
    }
}


void Device::dumpPage(uint i)
{
    Page *page = _pages.at(i);
    _data.initPage(page->rects(), page->pictures);
    page->dump();
}

void Device::startPage(int, GfxState *)
{
    _pages.append( new Page(_data) );
}

void Device::endPage()
{
    if ( !_currentImage.image.isNull() ) addImage();
    current()->endPage();
    clear();
    kdDebug(30516) << "-- end page --------------------------" << endl;
}

void Device::updateFont(GfxState *state)
{
    current()->updateFont(state);
}

void Device::beginString(GfxState *state, GString *)
{
    current()->beginString(state, state->getCurX(), state->getCurY());
}

void Device::endString(GfxState *)
{
    current()->endString();
}

void Device::drawChar(GfxState *state, double x, double y,
                            double dx, double dy, double, double,
                            CharCode, Unicode *u, int uLen)
{
    current()->addChar(state, x, y, dx, dy, u, uLen);
}

void Device::drawLink(::Link* link, Catalog *cat)
{
    double x1, y1, x2, y2, w;
    link->getBorder(&x1, &y1, &x2, &y2, &w);

    int ux1, uy1, ux2, uy2;
    cvtUserToDev(x1, y1, &ux1, &uy1);
    cvtUserToDev(x2, y2, &ux2, &uy2);

    DRect r(kMin(ux1, ux2), kMax(ux1, ux2), kMin(uy1, uy2), kMax(uy1, uy2));
    Link *l = new Link(r, *link->getAction(), *cat);
    current()->addLink(l);
}

void Device::addImage()
{
//    kdDebug(30516) << "-> add image" << endl;
    if ( _currentImage.image.width()==0 || _currentImage.image.height()==0 ) {
        kdDebug(30516) << "image has null width or height !" << endl;
        _currentImage = Image();
        return;
    }

    // check if same image already put at same place (don't know why it
    // appends sometimes : related to KWord printing to pdf ?)
    ImageList::iterator it;
    for (it=_images.begin(); it!=_images.end(); ++it) {
        if ( (*it).rect==_currentImage.rect
             && (*it).image==_currentImage.image ) {
            kdDebug(30516) << "image already there !\n";
            _currentImage = Image();
            return;
        }
    }

    // add image
    QString name = QString("pictures/picture%1.png").arg(_data.imageIndex());
    QDomElement frameset = _data.pictureFrameset(_currentImage.rect);
    current()->pictures.append(frameset);
    QDomElement picture = _data.createElement("PICTURE");
    picture.setAttribute("keepAspectRatio", "false");
    frameset.appendChild(picture);

    QDomElement key = _data.createElement("KEY");
    key.setAttribute("msec", 0);
    key.setAttribute("second", 0);
    key.setAttribute("minute", 0);
    key.setAttribute("hour", 0);
    key.setAttribute("day", 1);
    key.setAttribute("month", 1);
    key.setAttribute("year", 1970);
    key.setAttribute("filename", name);
    picture.appendChild(key);

    key = _data.createElement("KEY");
    key.setAttribute("msec", 0);
    key.setAttribute("second", 0);
    key.setAttribute("minute", 0);
    key.setAttribute("hour", 0);
    key.setAttribute("day", 1);
    key.setAttribute("month", 1);
    key.setAttribute("year", 1970);
    key.setAttribute("filename", name);
    key.setAttribute("name", name);
    _data.pictures().appendChild(key);

    KoStoreDevice *sd = _data.chain()->storageFile(name, KoStore::Write);
    QImageIO io(sd, "PNG");
    io.setImage(_currentImage.image);
    bool ok = io.write();
    Q_ASSERT(ok);
    sd->close();

    _images.append(_currentImage);
    _currentImage = Image();
}

void Device::computeGeometry(GfxState *state, Image &image)
{
    double xt, yt, wt, ht;
    state->transform(0, 0, &xt, &yt);
    state->transformDelta(1, 1, &wt, &ht);
    image.rect.setLeft(xt + (wt>0 ? 0 : wt));
    image.rect.setRight(image.rect.left() + fabs(wt));
    image.rect.setTop(yt + (ht>0 ? 0 : ht));
    image.rect.setBottom(image.rect.top() + fabs(ht));

    // #### TODO : take care of image transform (rotation,...)
}

uint Device::initImage(GfxState *state, int width, int height,
                       bool withMask)
{
    // get image geometry
    Image image;
    image.mask = withMask;
    computeGeometry(state, image);

    // check if new image
//    kdDebug(30516) << "current image " << _currentImage.image.width()
//                   << " " << _currentImage.rect.left()
//                   << " " << _currentImage.rect.right()
//                   << " " << _currentImage.rect.bottom()
//                   << " " << _currentImage.mask << endl;
//    kdDebug(30516) << "new image " << width
//                   << " " << image.rect.left() << " " << image.rect.right()
//                   << " " << image.rect.top()
//                   << " " << image.mask << endl;
    if ( !_currentImage.image.isNull() &&
         (_currentImage.image.width()!=width
          || !equal(image.rect.left(), _currentImage.rect.left())
          || !equal(image.rect.right(), _currentImage.rect.right())
          || !equal(image.rect.top(), _currentImage.rect.bottom())
          || image.mask!=_currentImage.mask) )
        addImage();

    uint offset =
        (_currentImage.image.isNull() ? 0 : _currentImage.image.height());
    image.image = QImage(width, offset + height, 32);
    image.image.setAlphaBuffer(withMask);
    if ( !_currentImage.image.isNull() ) { // copy previous
//        kdDebug(30516) << "image continued..." << endl;
        for (int j=0; j<_currentImage.image.height(); j++) {
            QRgb *pix = (QRgb *)_currentImage.image.scanLine(j);
            QRgb *newPix = (QRgb *)image.image.scanLine(j);
            for (int i=0; i<width; i++) newPix[i] = pix[i];
        }
        _currentImage.image = image.image;
        _currentImage.rect.setBottom( image.rect.bottom() );
    } else _currentImage = image;
    return offset;
}

void Device::drawImage(GfxState *state, Object *, Stream *str,
                             int width, int height, GfxImageColorMap *colorMap,
                             int *maskColors, GBool inlineImg)
{
    kdDebug(30516) << "image kind=" << str->getKind()
                   << " inline=" << inlineImg
                   << " maskColors=" << (maskColors!=0) << endl;
    if ( !_data.options().importImages ) return;

    uint offset = initImage(state, width, height, maskColors!=0);

    // read pixels
    int nbComps = colorMap->getNumPixelComps();
    int nbBits = colorMap->getBits();
    ImageStream *istr = new ImageStream(str, width, nbComps, nbBits);
    istr->reset();
    for (int j=0; j<height; j++) {
        Guchar *p = istr->getLine();
        QRgb *pix = (QRgb *)_currentImage.image.scanLine(offset + j);
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

void Device::drawImageMask(GfxState *state, Object *, Stream *str,
                                 int width, int height, GBool invert,
                                 GBool inlineImg)
{
    kdDebug(30516) << "image mask ! kind=" << str->getKind()
                   << "inline=" << inlineImg << endl;
    if ( !_data.options().importImages ) return;

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
        QRgb *pix = (QRgb *)_currentImage.image.scanLine(offset + j);
        for (int i=0; i<width; i++)
            pix[i] = qRgba(red, green, blue, 255 * p[i]);
    }
    delete istr;

    if (invert) _currentImage.image.invertPixels();
}

void Device::updateAll(GfxState *state)
{
    updateFillColor(state);
    updateStrokeColor(state);
    updateFont(state);
}

void Device::updateFillColor(GfxState *state)
{
    GfxRGB rgb;
    state->getFillRGB(&rgb);
    _fillColor = toColor(rgb);
}

void Device::updateStrokeColor(GfxState *state)
{
    GfxRGB rgb;
    state->getStrokeRGB(&rgb);
    _strokeColor = toColor(rgb);
}

void Device::stroke(GfxState */*state*/)
{
//    kdDebug(30516) << "stroke" << endl;
//    DPathVector path = convertPath(state);
//    for (uint i=0; i<path.size(); i++) {
//        if ( path[i].isHorizontalSegment() ) {
//            kdDebug(30516) << "  horizontal segment" << endl;
            // #### FIXME correctly draw the line
//            if ( !_currentImage.image.isNull() ) addImage();
//            _currentImage.rect = path[i].boundingRect();
//            _currentImage.rect.bottom+=1;
//            _currentImage.image =
//                QImage(qRound(_currentImage.rect.width()),
//                       qRound(_currentImage.rect.height()), 32);
//            _currentImage.image.fill(_fillColor.pixel());
//            addImage();
//        } else if ( path[i].isVerticalSegment() ) {
//            kdDebug(30516) << "  vertical segment" << endl;
//        }
//    }
}

void Device::fill(GfxState */*state*/)
{
//    kdDebug(30516) << "fill" << endl;
//    doFill(state);
}

void Device::eoFill(GfxState */*state*/)
{
//    kdDebug(30516) << "eoFill" << endl;
//    convertPath(state);
//    doFill(state);
}

void Device::doFill(const DPathVector &path)
{
    for (uint i=0; i<path.size(); i++) {
        if ( path[i].isSegment() ) continue;
        if ( path[i].isRectangle() ) {
            kdDebug(30516) << "fill rectangle" << endl;
            if ( !_currentImage.image.isNull() ) addImage();
            _currentImage.rect = path[i].boundingRect();
            _currentImage.image =
                QImage(qRound(_currentImage.rect.width()),
                       qRound(_currentImage.rect.height()), 32);
            _currentImage.image.fill(_fillColor.pixel());
            addImage();
        }
    }
}

DPathVector Device::convertPath(GfxState *state)
{
    GfxPath *path = state->getPath();
    uint nbPaths = path->getNumSubpaths();
    DPathVector vector;
    for (uint i=0; i<nbPaths; i++) {
        GfxSubpath *spath = path->getSubpath(i);
        uint nbPoints = spath->getNumPoints();
        DPath dpath;
        for (uint k=0; k<nbPoints; k++) {
            if ( k>=1 && spath->getCurve(k) ) {
                kdDebug(30516) << "    bezier curve : ignore !" << endl;
                dpath = DPath();
                break;
            } else {
                DPoint dpoint;
                state->transform(spath->getX(k), spath->getY(k),
                                 &dpoint.x, &dpoint.y);
                dpath.push_back(dpoint);
            }
        }
        if ( dpath.size()!=0 ) vector.push_back(dpath);
    }
    return vector;
}

} // namespace
