/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002, 2003, 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "PictureEps.h"

#include <unistd.h>
#include <stdio.h>

#include <QBuffer>
#include <QPainter>
#include <QPrinter>
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QImage>
#include <QPixmap>
#include <QApplication>
#include <QMimeData>

#include <kglobal.h>
#include <kdebug.h>

#include <ktemporaryfile.h>
#include <kshell.h>

#include "PictureKey.h"
#include "PictureBase.h"


PictureEps::PictureEps(void) : m_psStreamStart(0), m_psStreamLength(0), m_cacheIsInFastMode(true)
{
}

PictureEps::~PictureEps(void)
{
}

PictureBase* PictureEps::newCopy(void) const
{
    return new PictureEps(*this);
}

PictureType::Type PictureEps::getType(void) const
{
    return PictureType::TypeEps;
}

bool PictureEps::isNull(void) const
{
    return m_rawData.isNull();
}

QImage PictureEps::scaleWithGhostScript(const QSize& size, const int resolutionx, const int resolutiony)
{
    if (!m_boundingBox.width() || !m_boundingBox.height()) {
        kDebug(30508) << "EPS image has a null size! (in PictureEps::scaleWithGhostScript)";
        return QImage();
    }

    // ### TODO: do not call GhostScript up to three times for each re-scaling (one call of GhostScript should be enough to know which device is available: gs --help)
    // png16m is better, but not always available -> fallback to bmp16m, then fallback to ppm (256 colors)
    // ### TODO: pcx24b is also a true color format
    // ### TODO: support alpha (other gs devices needed)

    const char* deviceTable[] = { "png16m", "bmp16m", "ppm", 0 };

    QImage img;

    for (int i = 0; deviceTable[i]; ++i) {
        if (tryScaleWithGhostScript(img, size, resolutionx, resolutiony, deviceTable[i]) != -1) {
            return img;
        }

    }

    kError(30508) << "Image from GhostScript cannot be loaded (in PictureEps::scaleWithGhostScript)" << endl;
    return img;
}

// Helper method for scaleWithGhostScript. Returns 1 on success, 0 on error, -1 if nothing generated
// (in which case another 'output device' can be tried)
int PictureEps::tryScaleWithGhostScript(QImage &image, const QSize& size, const int resolutionx, const int resolutiony, const char* device)
// Based on the code of the file kdelibs/kimgio/eps.cpp
{
    kDebug(30508) << "Sampling with GhostScript, using device \"" << device << "\" (in PictureEps::tryScaleWithGhostScript)";

    KTemporaryFile tmpFile;
    if (!tmpFile.open()) {
        kError(30508) << "No KTemporaryFile! (in PictureEps::tryScaleWithGhostScript)" << endl;
        return 0; // error
    }

    const int wantedWidth = size.width();
    const int wantedHeight = size.height();
    const qreal xScale = qreal(size.width()) / qreal(m_boundingBox.width());
    const qreal yScale = qreal(size.height()) / qreal(m_boundingBox.height());

    // create GS command line

    QString cmdBuf("gs -sOutputFile=");
    cmdBuf += KShell::quoteArg(tmpFile.fileName());
    cmdBuf += " -q -g";
    cmdBuf += QString::number(wantedWidth);
    cmdBuf += 'x';
    cmdBuf += QString::number(wantedHeight);

    if ((resolutionx > 0) && (resolutiony > 0)) {
#if 0
        // Do not play with resolution for now.
        // It brings more problems at print than solutions
        cmdBuf += " -r";
        cmdBuf += QString::number(resolutionx);
        cmdBuf += 'x';
        cmdBuf += QString::number(resolutiony);
#endif
    }

    cmdBuf += " -dSAFER -dPARANOIDSAFER -dNOPAUSE -sDEVICE=";
    cmdBuf += device;
    //cmdBuf += " -c 255 255 255 setrgbcolor fill 0 0 0 setrgbcolor";
    cmdBuf += " -";
    cmdBuf += " -c showpage quit";

    // run ghostview

    FILE* ghostfd = popen(QFile::encodeName(cmdBuf), "w");

    if (ghostfd == 0) {
        kError(30508) << "No connection to GhostScript (in PictureEps::tryScaleWithGhostScript)" << endl;
        return 0; // error
    }

    // The translation is needed as GhostScript (7.07) cannot handle negative values in the bounding box otherwise.
    fprintf(ghostfd, "\n%d %d translate\n", -qRound(m_boundingBox.left()*xScale), -qRound(m_boundingBox.top()*yScale));
    fprintf(ghostfd, "%g %g scale\n", xScale, yScale);

    // write image to gs

    fwrite(m_rawData.data() + m_psStreamStart, sizeof(char), m_psStreamLength, ghostfd);

    pclose(ghostfd);

    // load image
    if (!image.load(tmpFile.fileName())) {
        // It failed - maybe the device isn't supported by gs
        return -1;
    }
    if (image.size() != size) { // this can happen due to rounding problems
        //kDebug(30508) <<"fixing size to" << size.width() <<"x" << size.height()
        //          << " (was " << image.width() << "x" << image.height() << ")" << endl;
        image = image.scaled(size);   // hmm, smoothScale instead?
    }
    kDebug(30508) << "Image parameters:" << image.width() << "x" << image.height() << "x" << image.depth();
    return 1; // success
}

void PictureEps::scaleAndCreatePixmap(const QSize& size, bool fastMode, const int resolutionx, const int resolutiony)
{
    kDebug(30508) << "PictureEps::scaleAndCreatePixmap" << size << "" << (fastMode ? QString("fast") : QString("slow"))
    << " resolutionx: " << resolutionx << " resolutiony: " << resolutiony << endl;
    if ((size == m_cachedSize)
            && ((fastMode) || (!m_cacheIsInFastMode))) {
        // The cached pixmap has already the right size
        // and:
        // - we are in fast mode (We do not care if the re-size was done slowly previously)
        // - the re-size was already done in slow mode
        kDebug(30508) << "Already cached!";
        return;
    }

    // Slow mode can be very slow, especially at high zoom levels -> configurable
    if (!isSlowResizeModeAllowed()) {
        kDebug(30508) << "User has disallowed slow mode!";
        fastMode = true;
    }

    // We cannot use fast mode, if nothing was ever cached.
    if (fastMode && !m_cachedSize.isEmpty()) {
        kDebug(30508) << "Fast scaling!";
        // Slower than caching a QImage, but faster than re-sampling!
        QImage image(m_cachedPixmap.toImage());
        m_cachedPixmap = QPixmap::fromImage(image.scaled(size));
        m_cacheIsInFastMode = true;
        m_cachedSize = size;
    } else {
        QTime time;
        time.start();

        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_cachedPixmap = QPixmap::fromImage(scaleWithGhostScript(size, resolutionx, resolutiony));
        QApplication::restoreOverrideCursor();
        m_cacheIsInFastMode = false;
        m_cachedSize = size;

        kDebug(30508) << "Time:" << (time.elapsed() / 1000.0) << " s";
    }
    kDebug(30508) << "New size:" << size;
}

void PictureEps::draw(QPainter& painter, int x, int y, int width, int height, int sx, int sy, int sw, int sh, bool fastMode)
{
    if (!width || !height)
        return;

    QSize screenSize(width, height);
    //kDebug( 30508 ) <<"PictureEps::draw screenSize=" << screenSize.width() <<"x" << screenSize.height();

    if (dynamic_cast<QPrinter*>(painter.device())) { // Is it an external device (i.e. printer)
        kDebug(30508) << "Drawing for a printer (in PictureEps::draw)";
        // For printing, always re-sample the image, as a printer has never the same resolution than a display.
        QImage image(scaleWithGhostScript(screenSize, painter.device()->logicalDpiX(), painter.device()->logicalDpiY()));
        // sx,sy,sw,sh is meant to be used as a cliprect on the pixmap, but drawImage
        // translates it to the (x,y) point -> we need (x+sx, y+sy).
        painter.drawImage(x + sx, y + sy, image, sx, sy, sw, sh);
    } else { // No, it is simply a display
        scaleAndCreatePixmap(screenSize, fastMode, painter.device()->logicalDpiX(), painter.device()->logicalDpiY());

        // sx,sy,sw,sh is meant to be used as a cliprect on the pixmap, but drawPixmap
        // translates it to the (x,y) point -> we need (x+sx, y+sy).
        painter.drawPixmap(x + sx, y + sy, m_cachedPixmap, sx, sy, sw, sh);
    }
}

bool PictureEps::extractPostScriptStream(void)
{
    kDebug(30508) << "PictureEps::extractPostScriptStream";
    QDataStream data(&m_rawData, QIODevice::ReadOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    qint32 magic, offset, length;
    data >> magic;
    data >> offset;
    data >> length;
    if (!length) {
        kError(30508) << "Length of PS stream is zero!" << endl;
        return false;
    }
    if (offset + length > m_rawData.size()) {
        kError(30508) << "Data stream of the EPSF file is longer than file: " << offset << "+" << length << ">" << m_rawData.size() << endl;
        return false;
    }
    m_psStreamStart = offset;
    m_psStreamLength = length;
    return true;
}

QString PictureEps::readLine(const QByteArray& array, const uint start, const uint length, uint& pos, bool& lastCharWasCr)
{
    QString strLine;
    const uint finish = qMin(start + length, (uint) array.size());
    for (; pos < finish; ++pos) { // We are starting at pos
        const char ch = array[ pos ]; // Read one character
        if (ch == '\n') {
            if (lastCharWasCr) {
                // We have a line feed following a Carriage Return
                // As the Carriage Return has already ended the previous line,
                // discard this Line Feed.
                lastCharWasCr = false;
            } else {
                // We have a normal Line Feed, therefore we end the line
                break;
            }
        } else if (ch == '\r') {
            // We have a Carriage Return, therefore we end the line
            lastCharWasCr = true;
            break;
        } else if (ch == char(12))  // Form Feed
        { // ### TODO: can a FF happen in PostScript?
            // Ignore the form feed
            continue;
        } else {
            strLine += ch;
            lastCharWasCr = false;
        }
    }
    return strLine;
}


bool PictureEps::loadData(const QByteArray& array, const QString& /* extension */)
{

    kDebug(30508) << "PictureEps::load";
    // First, read the raw data
    m_rawData = array;

    if (m_rawData.isNull()) {
        kError(30508) << "No data was loaded!" << endl;
        return false;
    }

    if ((m_rawData[0] == char(0xc5)) && (m_rawData[1] == char(0xd0))
            && (m_rawData[2] == char(0xd3)) && (m_rawData[3] == char(0xc6))) {
        // We have a so-called "MS-DOS EPS file", we have to extract the PostScript stream
        if (!extractPostScriptStream()) // Changes m_rawData
            return false;
    } else {
        m_psStreamStart = 0;
        m_psStreamLength = m_rawData.size();
    }

    QString lineBox; // Line with the bounding box
    bool lastWasCr = false; // Was the last character of the line a carriage return?
    uint pos = m_psStreamStart; // We start to search the bounding box at the start of the PostScript stream
    QString line(readLine(m_rawData, m_psStreamStart, m_psStreamLength, pos, lastWasCr));
    kDebug(30508) << "Header:" << line;
    if (!line.startsWith("%!")) {
        kError(30508) << "Not a PostScript file!" << endl;
        return false;
    }
    QRect rect;
    bool lineIsBoundingBox = false; // Does "line" has a %%BoundingBox line?
    for (;;) {
        ++pos; // Get over the previous line end (CR or LF)
        line = readLine(m_rawData,  m_psStreamStart, m_psStreamLength, pos, lastWasCr);
        kDebug(30508) << "Checking line:" << line;
        // ### TODO: it seems that the bounding box can be delayed with "(atend)" in the trailer (GhostScript 7.07 does not support it either.)
        if (line.startsWith("%%BoundingBox:")) {
            lineIsBoundingBox = true;
            break;
        }
        // ### TODO: also abort on %%EndComments
        // ### TODO: %? , where ? is non-white-space printable, does not end the comment!
        else if (!line.startsWith("%%"))
            break; // Not a EPS comment anymore, so abort as we are not in the EPS header anymore
    }
    if (!lineIsBoundingBox) {
        kError(30508) << "PictureEps::load: could not find a bounding box!" << endl;
        return false;
    }
    // Floating point values are not allowed in a Bounding Box, but ther are many such files out there...
    QRegExp exp("(\\-?[0-9]+\\.?[0-9]*)\\s(\\-?[0-9]+\\.?[0-9]*)\\s(\\-?[0-9]+\\.?[0-9]*)\\s(\\-?[0-9]+\\.?[0-9]*)");
    if (!line.contains(exp)) {
        // ### TODO: it might be an "(atend)" and the bounding box is in the trailer
        // (but GhostScript 7.07 does not support a bounding box in the trailer.)
        // Note: in Trailer, it is the last BoundingBox that counts not the first!
        kError(30508) << "Not standard bounding box: " << line << endl;
        return false;
    }
    kDebug(30508) << "Reg. Exp. Found:" << exp.capturedTexts();
    rect.setLeft((int)exp.cap(1).toDouble());
    rect.setTop((int)exp.cap(2).toDouble());
    rect.setRight((int)exp.cap(3).toDouble());
    rect.setBottom((int)exp.cap(4).toDouble());
    m_boundingBox = rect;
    m_originalSize = rect.size();
    kDebug(30508) << "Rect:" << rect << " Size:"  << m_originalSize;
    return true;
}

bool PictureEps::save(QIODevice* io) const
{
    // We save the raw data, to avoid damaging the file by many load/save cycles
    qint64 size = io->write(m_rawData); // WARNING: writeBlock returns Q_LONG but size() Q_ULONG!
    return (size == m_rawData.size());
}

QSize PictureEps::getOriginalSize(void) const
{
    return m_originalSize;
}

QPixmap PictureEps::generatePixmap(const QSize& size, bool smoothScale)
{
    scaleAndCreatePixmap(size, !smoothScale, 0, 0);
    return m_cachedPixmap;
}

QString PictureEps::getMimeType(const QString&) const
{
    return "image/x-eps";
}

QImage PictureEps::generateImage(const QSize& size)
{
    // 0, 0 == resolution unknown
    return scaleWithGhostScript(size, 0, 0);
}

void PictureEps::clearCache(void)
{
    m_cachedPixmap = QPixmap();
    m_cacheIsInFastMode = true;
    m_cachedSize = QSize();
}
