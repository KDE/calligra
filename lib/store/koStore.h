/* This file is part of the KDE project
   Copyright (C) 1998, 1999 David Faure <faure@kde.org>

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

#ifndef __koStore_h_
#define __koStore_h_

#include <qstring.h>
#include <qcstring.h>

/**
 * Base class for @ref KoTarStore and @ref KoBinaryStore
 * We call a "store" the file on the hard disk (the one the users sees)
 * and call a "file" a file inside the store.
 */
class KoStore
{
protected:
    /**
     * This is an abstract class.
     * See KoTarStore and KoBinaryStore for how to construct a store
     */
    KoStore() {}

public:
    enum Mode { Read, Write };

    /**
     * Destroys the store (i.e. closes the file on the hard disk)
     */
    virtual ~KoStore() {}

    /**
     * Open a new file inside the store
     * @param name the filename, internal representation ("root", "tar:0"... )
     * @mimetype deprecated, don't use
     */
    virtual bool open( const QString & name, const QCString & mimetype = "" ) = 0;
    /**
     * Close the file inside the store
     */
    virtual void close() = 0;

    /**
     * @return true if an error occured
     */
    virtual bool bad() = 0;

    /**
     * Write data into the currently opened file. You can also use the streams
     * for this.
     */
    virtual bool write( const char* _data, unsigned long _len ) = 0;
    /**
     * Read data from the currently opened file. You can also use the streams
     * for this.
     */
    virtual long read( char *_buffer, unsigned long _len ) = 0;

    /**
     * Write data into the currently opened file. You can also use the streams
     * for this.
     */
    virtual bool write( const QByteArray& data ) = 0;
    /**
     * Read data from the currently opened file. You can also use the streams
     * for this.
     */
    virtual QByteArray read( unsigned long max ) = 0;
};

#endif
