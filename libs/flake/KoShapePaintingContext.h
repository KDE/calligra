/* This file is part of the KDE project
   Copyright (C) 2004-2006 David Faure <faure@kde.org>
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2011 Inge Wallin <inge@lysator.liu.se>

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

#ifndef KOSHAPEPAINTINGCONTEXT_H
#define KOSHAPEPAINTINGCONTEXT_H

#include "flake_export.h"

#include <QImage>
#include <QTransform>
#include <QTextBlockUserData>


class KoShape;
class KoViewConverter;
class KoShapePaintingContextPrivate;

/**
 * The set of data for the ODF file format used during painting of a shape.
 */
class FLAKE_EXPORT KoShapePaintingContext
{
public:
    /// The Style used for painting the shape
    enum ShapePaintingOption {
        /**
         * If set, paint the shape itself.
         */
        PaintShape = 0x01,
        /**
         * If set, paint the decorations, provided the shape has any..
         */
        PaintDecorations = 0x02
    };
    Q_DECLARE_FLAGS(ShapePaintingOptions, ShapePaintingOption)

    /**
     * @brief Constructor
     * @param converter The ViewConverter used to set the transformation in the painter
     */
    KoShapePaintingContext(KoViewConverter &converter);
    virtual ~KoShapePaintingContext();

    /**
     * @brief Get the view converter.
     *
     * @return viewConverter
     */
    KoViewConverter &viewConverter() const;

    /**
     * @brief Set the view converter
     *
     * Change the view converter that is used for painting the shape.
     *
     * @param viewConverter view converter to use
     */
    void setViewConverter(KoViewConverter &viewConverter);

    /**
     * @brief Check if an option is set
     *
     * @return true if the option is set, false otherwise
     */
    bool isSet(ShapePaintingOption option) const;

    /**
     * @brief Set the options to use
     *
     * @param options to use
     */
    void setOptions(ShapePaintingOptions options);

    /// Add an option to the set of options stored on this context, will leave the other options intact.
    void addOption(ShapePaintingOption option);

    /// Remove an option, will leave the other options intact.
    void removeOption(ShapePaintingOption option);

    /**
     * @brief Get the options used
     *
     * @return options used
     */
    ShapePaintingOptions options() const;


private:
    KoShapePaintingContextPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KoShapePaintingContext::ShapePaintingOptions)

#endif // KOSHAPEPAINTINGCONTEXT_H
