/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
 *  Copyright (C) 2006-2007 David Faure <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#ifndef KMIMETYPEFACTORY_H
#define KMIMETYPEFACTORY_H

#include <assert.h>

#include <QtCore/QStringList>

#include "ksycocafactory.h"
#include "ksycocaentry_p.h"

class KSycoca;

/**
 * @internal  - this header is not installed
 *
 * A sycoca factory for mimetype entries
 * Since KDE 4.6, this is only used to point to the "service offers" in ksycoca for each mimetype.
 * @see KMimeType
 */
class KSERVICE_EXPORT KMimeTypeFactory : public KSycocaFactory
{
    K_SYCOCAFACTORY( KST_KMimeTypeFactory )
public:
    /**
     * Create factory
     */
    KMimeTypeFactory();

    virtual ~KMimeTypeFactory();

    /**
     * Not meant to be called at this level
     */
     virtual KSycocaEntry *createEntry(const QString &) const
    { assert(0); return 0; }

    /**
     * Returns the possible offset for a given mimetype entry.
     */
    int entryOffset(const QString& mimeTypeName);

    /**
     * Returns the offset into the service offers for a given mimetype.
     */
    int serviceOffersOffset(const QString& mimeTypeName);

public:
    /**
     * @return all mimetypes
     * Slow and memory consuming, avoid using
     */
    QStringList allMimeTypes();

    /**
     * @return the unique mimetype factory, creating it if necessary
     */
    static KMimeTypeFactory * self();

public: // public for KBuildServiceFactory
    // A small entry for each mimetype with name and offset into the services-offer-list.
    class MimeTypeEntryPrivate;
    class KSERVICE_EXPORT MimeTypeEntry : public KSycocaEntry
    {
        Q_DECLARE_PRIVATE( MimeTypeEntry )
    public:
        typedef KSharedPtr<MimeTypeEntry> Ptr;

        MimeTypeEntry(const QString& file, const QString& name);
        MimeTypeEntry(QDataStream& s, int offset);

        int serviceOffersOffset() const;
        void setServiceOffersOffset(int off);
    };

protected:
    virtual MimeTypeEntry *createEntry(int offset) const;
private:
    // d pointer: useless since this header is not installed
    //class KMimeTypeFactoryPrivate* d;
};

#endif
