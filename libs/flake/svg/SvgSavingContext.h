/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SVGSAVINGCONTEXT_H
#define SVGSAVINGCONTEXT_H

class KoXmlWriter;
class KoShape;
class KoImageData;
class QIODevice;
class QString;
class QTransform;
class QImage;

#include "flake_export.h"

/// Context for saving svg files
class FLAKE_EXPORT SvgSavingContext
{
public:
    /// Creates a new svg saving context on the specified output device
    explicit SvgSavingContext(QIODevice &outputDevice, bool saveInlineImages = true);

    /// Virtual destructor
    virtual ~SvgSavingContext();

    /// Provides access to the style writer
    KoXmlWriter &styleWriter();

    /// Provides access to the shape writer
    KoXmlWriter &shapeWriter();

    /// Create a unuque id from the specified base text
    QString createUID(const QString &base);

    /// Returns the unique id for the given shape
    QString getID(const KoShape *obj);

    /// Returns the transformation used to transform into usre space
    QTransform userSpaceTransform() const;

    /// Returns if image should be saved inline
    bool isSavingInlineImages() const;

    /// Create a filename suitable for saving external data
    QString createFileName(const QString &extension);

    /// Saves given image and returns the href used
    QString saveImage(const QImage &image);

    /// Saves given image and returns the href used
    QString saveImage(KoImageData *image);

private:
    class Private;
    Private *const d;
};

#endif // SVGSAVINGCONTEXT_H
