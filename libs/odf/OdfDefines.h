/* This file is part of the KDE project
 * Copyright (C)  2006, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C)  2008 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C)  2011 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#ifndef ODFDEFINES_H
#define ODFDEFINES_H

#include "koodf_export.h"

/**
 * Generic namespace of the Calligra ODF library for helper methods and data.
 */
namespace KoOdf
{

/// Text in the objects will be positioned according to the direction.
enum TextDirection {
    AutoDirection,      ///< Take the direction from the text.
    LeftRightTopBottom, ///< Text layout for most western languages
    RightLeftTopBottom, ///< Text layout for languages like Hebrew
    TopBottomRightLeft,  ///< Vertical text layout.
    TopBottomLeftRight,  ///< Vertical text layout. ?
    InheritDirection    ///< Direction is unspecified and should come from the container
};

}


#endif
