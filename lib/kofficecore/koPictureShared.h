/* This file is part of the KDE project
   Copyright (c) 2001 Simon Hausmann <hausmann@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __koPictureShared_h__
#define __koPictureShared_h__

#include <qshared.h>
#include <qstring.h>
#include <qiodevice.h>
#include <qpixmap.h>

#include "koPictureKey.h"

class QPainter;
class QSize;

class KoPictureBase;

/**
 * KoPictureShared is the class that has the shared part of KoPicture
 */
class KoPictureShared : public QShared
{
public:
    /**
     * Default constructor.
     */
    KoPictureShared(void);

    /**
     * Destructor.
     */
    ~KoPictureShared(void);

    /**
     * Copy constructor
     */
    KoPictureShared(const KoPictureShared &other);

    /**
     * Assignment operator
     */
    KoPictureShared& operator=(const KoPictureShared& other);

    KoPictureType::Type getType(void) const;

    /**
     * Returns true if the picture is null.
     */
    bool isNull(void) const;

    /**
     * Draw the image in a painter.
     * No, this isn't as simple as painter.drawPixmap().
     * This method ensures that the best quality is used when printing, scaling the painter.
     *
     * The parameters @p width, @p height define the desired size for the image
     * Note that the image is being scaled to that size using scale() - except when printing.
     * This avoids scaling the image at each paint event.
     *
     * The other parameters are very similar to QPainter::drawPixmap :
     * (@p x, @p y) define the position in the painter,
     * (@p sx, @p sy) specify the top-left point in pixmap that is to be drawn. The default is (0, 0).
     * (@p sw, @p sh) specify the size of the pixmap that is to be drawn. The default, (-1, -1), means all the way to the bottom
     * right of the pixmap.
     */
    void draw(QPainter& painter, int x, int y, int width, int height, int sx = 0, int sy = 0, int sw = -1, int sh = -1);

    bool load(QIODevice* io, const QString& extension);

    bool save(QIODevice* io);

    void setExtension(const QString& extension);

    QString getExtension(void) const;

    QSize getOriginalSize(void) const;

    QSize getSize(void) const;

    void setSize(const QSize& size);

    /**
     * Clear and set the mode of this KoPictureShared
     *
     * @p newMode is a file extension (like "png") giing the wanted mode
     */
    void clearAndSetMode(const QString& newMode);

    /**
     * Reset the KoPictureShared (but not the key!)
     */
    void clear(void);

    bool loadFromFile(const QString& fileName);

    /**
     * Load a potentially broken XPM file (for KPresenter)
     */
    bool loadXpm(QIODevice* io);

    /**
     * @deprecated
     * Returns a QPixmap from an image
     * Returns an empty QPixmap if the KoPictureShared is not an image.
     */
    QPixmap generatePixmap(const QSize& size);

protected:
    /**
     * @internal
     * Load a WMF file (a .wmf file could be a QPicture file)
     */
    bool loadWmf(QIODevice* io);
    /**
     * @internal
     * Do a normal load
     */
    bool load(QIODevice* io);

protected:
    KoPictureBase* m_base;
    QSize m_size;
    QString m_extension;
};

#endif /* __koPictureShared_h__ */
