/* This file is part of the KOffice libraries
   Copyright (C) 2001 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __koffice_filter_h__
#define __koffice_filter_h__

#include <qobject.h>

class KoFilterChain;

// Note: This class has to be derived from QObject, because we open the
// filter libs dynamically (KLibLoader)!
class KoFilter : public QObject
{
    Q_OBJECT

    friend class KoFilterEntry;  // needed for the filter chain pointer :(

public:
    enum ConversionStatus { OK, StupidError, UsageError, CreationError, FileNotFound,
                            StorageCreationError, BadMimeType, BadConversionGraph,
                            EmbeddedDocError, WrongFormat, NotImplemented };
    virtual ~KoFilter();

    // convert the file from a "from" mimetype to the desitnation mimetype ("to")
    virtual ConversionStatus convert( const QCString& from, const QCString& to ) = 0;

    // Returns the current embedded file(name), QString::null if
    // invalid (i.e. no current embedded file).
    virtual QString currentEmbeddedFile();

signals:
    /**
     * Emit this signal with a value in the range of 1...100 to have some
     * progress feedback for the user.
     */
    void sigProgress( int value );

protected:
    KoFilter();  // only for classes inheriting this one

    // This pointer will be set *after* returning from the constructor,
    // so don't try to use it in your constructor!
    KoFilterChain* m_chain;

private:
    KoFilter( const KoFilter& rhs );
    KoFilter &operator=( const KoFilter& rhs );
};

#endif
