/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Nokia

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MSO_WRITER_H
#define MSO_WRITER_H

#include <QRectF>

class KoGenStyles;
class KoGenStyle;
class KoXmlWriter;

/**
 * Helper class that for writing xml.
 *
 * Besides containing KoXmlWriter, this class keeps track of the coordinate
 * system. It has convenience functions for writing lengths in physical
 * dimensions (currently only mm).
 */
class Writer
{
public:
    qreal xOffset;
    qreal yOffset;
    qreal scaleX;
    qreal scaleY;

    /**
     * Rotation of the group shape.
     */
    qreal g_rotation;

    /**
     * Flip the group shape horizontally.
     */
    bool g_flipH;

    /**
     * Flip the group shape vertically.
     */
    bool g_flipV;

    /**
     * Xml writer that writes into content.xml.
     */
    KoXmlWriter &xml;
    /**
     * Styles for the document that is being created.
     **/
    KoGenStyles &styles;
    /**
     * Tells if the current output is for styles.xml or content.xml
     **/
    bool stylesxml;

    /**
     * Construct a new Writer.
     *
     * @param xmlWriter The xml writer that writes content.xml
     * @param kostyles The styles for the writer.
     * @param stylexml Defines whether the style is an XML style.
     */
    Writer(KoXmlWriter &xmlWriter, KoGenStyles &kostyles, bool stylexml = false);
    /**
     * Create a new writer with a new coordinate system.
     *
     * In different contexts in drawings in PPT files, different coordinate
     * systems are used.  These are defined by specifying a rectangle in the
     * old coordinate system and the equivalent in the new coordinate system.
     */
    Writer transform(const QRectF &oldCoords, const QRectF &newCoords) const;
    /**
     * Convert local length to global length.
     *
     * A length without unit in the local coordinate system is converted to a
     * global length without a unit.
     *
     * @param length a local length.
     * @return the global length.
     */
    qreal vLength(qreal length) const;
    /**
     * @see vLength
     */
    qreal hLength(qreal length) const;
    /**
     * @see vLength
     */
    qreal vOffset(qreal offset) const;
    /**
     * @see vLength
     */
    qreal hOffset(qreal offset) const;
};

#endif // MSO_WRITER_H
