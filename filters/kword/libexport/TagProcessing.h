/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#ifndef EXPORTTAGPROCESSING_H
#define EXPORTTAGPROCESSING_H

#include <qdom.h>
#include <qvaluelist.h>

#include <kdemacros.h>
#include <kdebug.h>

class KWEFKWordLeader;

/**
 * The class TagProcessing and the two functions ProcessSubtags () and
 * AllowNoSubtags () allow for easing parsing of subtags in the
 * current tag. If don't expect any subtags you call AllowNoSubtags ().
 * Otherwise you create a list of TagProcessing elements and pass that
 * to ProcessSubtags () which will go through all subtags it can find,
 * call the corresponding processing function, and do all the
 * necessary error handling.
 */

class TagProcessing
{
    public:
        TagProcessing ()
        {}

        TagProcessing (const QString& n,
                       void (*p)(QDomNode, void *, KWEFKWordLeader*),
                       void *d) : name (n), processor (p), data (d)
        {}

        /**
         * @brief No-op constructor
         *
         * Avoids a warning that a tag is unknown.
         */
        TagProcessing (const QString& _name) : name(_name), processor(0), data(0)
        {}

        QString  name;
        void     (*processor)(QDomNode, void *, KWEFKWordLeader*);
        void    *data;
};

void ProcessSubtags     ( const QDomNode             &parentNode,
                          QValueList<TagProcessing>  &tagProcessingList,
                          KWEFKWordLeader            *leader);

void AllowNoSubtags ( const QDomNode& myNode, KWEFKWordLeader *leader );


/**
 * The class AttrProcessing and the two functions ProcessAttributes ()
 * and AllowNoSubtags () allow for easing parsing of the current tag's
 * attributes. If don't expect any attributes you call AllowNoAttributes ().
 * Otherwise you create a list of AttrProcessing elements and pass
 * that to ProcessAttributes () which will go through all attributes
 * it can find, retrieve the value in the datatype defined, and do all
 * the necessary error handling.
 */

class AttrProcessing
{
public:

    enum AttrType
        { AttrNull = 0, AttrInt, AttrDouble, AttrBool, AttrQString };

    /**
     * @brief Default constructor
     */
    AttrProcessing () : type( AttrNull ), data(0) {}

    /**
     * @brief Old constructor (without type checking)
     * @deprecated do not use for new code
     */
    AttrProcessing ( const QString& n, const QString& t, void *d ) KDE_DEPRECATED;

    /**
     * @brief No-op constructor
     *
     * This just avoids that a warning is triggered that an attribute is not known
     */
    AttrProcessing ( const QString& _name )
        : name( _name ), type( AttrNull ), data( 0 ) {}

    /**
     * @brief Integer constructor
     *
     * Get the attribute value as an integer
     */
    AttrProcessing ( const QString& _name, int& i )
        : name( _name ), type( AttrInt ), data( &i ) {}

    /**
     * @brief Double constructor
     *
     * Get the attribute value as a double
     */
    AttrProcessing ( const QString& _name, double& d )
        : name( _name ), type( AttrDouble ), data( &d ) {}

    /**
     * @brief Boolean constructor
     *
     * Get the attribute value as a bool
     * (The values "yes,", "no", "true", "false", "0" and "1" are supported)
     */
    AttrProcessing ( const QString& _name, bool& flag )
        : name( _name ), type( AttrBool ), data( &flag ) {}

    /**
     * @brief String constructor
     *
     * Get the attribute value as a QString
     */
    AttrProcessing ( const QString& _name, QString& str )
        : name( _name ), type( AttrQString ), data( &str ) {}


public:
    QString   name;
    AttrType  type;
    void     *data;
};

void ProcessAttributes ( const QDomNode              &myNode,
                         QValueList<AttrProcessing>  &attrProcessingList );

void AllowNoAttributes ( const QDomNode& myNode );

#endif
