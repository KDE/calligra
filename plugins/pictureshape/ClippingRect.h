/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2011 Silvio Heinrich <plassy@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef H_CLIPPING_RECT_H
#define H_CLIPPING_RECT_H

#include <QRectF>
#include <QSizeF>

/**
 * This is a helper class. It helps converting clipping information
 * from standard cartesian coordinates to the format required to
 * save the information to an ODF file (using the fo::clip attribute)
 */
struct ClippingRect {
    ClippingRect();
    ClippingRect(const ClippingRect &rect);
    ClippingRect &operator=(const ClippingRect &rect);
    explicit ClippingRect(const QRectF &rect, bool isUniform = false);

    void scale(const QSizeF &size, bool isUniform = false);
    void normalize(const QSizeF &size);
    void setRect(const QRectF &rect, bool isUniform = false);

    qreal width() const;
    qreal height() const;
    QRectF toRect() const;

    qreal top;
    qreal right;
    qreal bottom;
    qreal left;
    bool uniform;
    bool inverted;
};

#endif // H_CLIPPING_RECT_H
