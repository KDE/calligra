/* This file is part of the KDE project
   Copyright (c) 2001 David Faure <faure@kde.org>

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

#ifndef koClipart_h
#define koClipart_h

#include <koImage.h>

/**
 * KoClipartKey is the structure describing a clipart in a unique way.
 * It currently includes the original path to the image and the modification
 * date.
 * At the moment there is no difference between KoImageKey and KoClipartKey.
 */
typedef KoImageKey KoClipartKey;

class KoClipartPrivate;

/**
 * A 'clipart' is a QPicture (for instance WMF or [later] SVG files)
 * Since QPictures aren't shared, KoClipart is an implicitly shared wrapper around a QPicture.
 */
class KoClipart
{
public:
    /**
     * Default constructor. Creates a null image.
     */
    KoClipart();

    /**
     * Constructs a KoClipart object from the given key and
     * QPicture. Note that KoClipart will create a copy of the
     * provided QPicture.
     */
    KoClipart( const KoClipartKey &key, const QPicture &pic );

    /**
     * Copy constructor.
     */
    KoClipart( const KoClipart &other );

    /**
     * Destructor.
     */
    ~KoClipart();

    /**
     * Assignment operator.
     */
    KoClipart &operator=( const KoClipart &other );

    /**
     * Retrieve the stored QPicture object.
     * Returns 0L if the clipart is null.
     */
    QPicture * picture() const;

    /**
     * Retrieve the key structure describing the image in a unique way.
     */
    KoClipartKey key() const;

    /**
     * Returns true if the clipart is null. A null clipart is created using the
     * default constructor.
     */
    bool isNull() const;

private:
    KoClipartPrivate *d;
};

#endif
