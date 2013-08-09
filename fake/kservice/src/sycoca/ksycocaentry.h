/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
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
 **/

#ifndef KSYCOCAENTRY_H
#define KSYCOCAENTRY_H

#include <ksycocatype.h>
#include <ksharedptr.h>
#include <kservice_export.h>

#include <QtCore/QDataStream>
#include <QtCore/QStringList>
#include <QtCore/QVariant>

class KSycocaEntryPrivate;

/**
 * Base class for all Sycoca entries.
 *
 * You can't create an instance of KSycocaEntry, but it provides
 * the common functionality for servicetypes and services.
 *
 * @internal
 * @see http://techbase.kde.org/Development/Architecture/KDE3/System_Configuration_Cache
 */
class KSERVICE_EXPORT KSycocaEntry : public KShared
{

public:
   /*
    * constructs a invalid KSycocaEntry object
    */
   KSycocaEntry();

   virtual ~KSycocaEntry();

   /**
    * internal
    */
   bool isType(KSycocaType t) const;
   /**
    * internal
    */
   KSycocaType sycocaType() const;

   typedef KSharedPtr<KSycocaEntry> Ptr;
   typedef QList<Ptr> List;

   /**
    * Default constructor
    */
//   explicit KSycocaEntry(const QString &path);

   /**
    * Safe demarshalling functions.
    */
   static void read( QDataStream &s, QString &str );
   static void read( QDataStream &s, QStringList &list );


   /**
    * @return the name of this entry
    */
   QString name() const;

   /**
    * @return the path of this entry
    * The path can be absolute or relative.
    * The corresponding factory should know relative to what.
    */
   QString entryPath() const;

   /**
    * @return the unique ID for this entry
    * In practice, this is storageId() for KService and name() for everything else.
    * \since 4.2.1
    */
   QString storageId() const;

    /**
    * @return true if valid
    */
   bool isValid() const;

   /**
    * @return true if deleted
    */
   bool isDeleted() const;

    /**
     * Returns the requested property. Some often used properties
     * have convenience access functions like exec(),
     * serviceTypes etc.
     *
     * @param name the name of the property
     * @return the property, or invalid if not found
     */
   QVariant property(const QString &name) const;

    /**
     * Returns the list of all properties that this service can have.
     * That means, that some of these properties may be empty.
     * @return the list of supported properties
     */
    QStringList propertyNames() const;

   /**
    * Sets whether or not this service is deleted
    */
   void setDeleted( bool deleted );


   /*
    * @returns true, if this is a separator
    */
   bool isSeparator() const;

   /**
    * @internal
    * @return the position of the entry in the sycoca file
    */
   int offset() const;

   /**
    * @internal
    * Save ourselves to the database.
    */
   void save(QDataStream &s);

//   KSycocaEntry(const KSycocaEntry &copy);
//   KSycocaEntry &operator=(const KSycocaEntry &right);
protected:
   KSycocaEntry(KSycocaEntryPrivate &d);
   KSycocaEntryPrivate *d_ptr;

private:
    Q_DISABLE_COPY(KSycocaEntry)

    Q_DECLARE_PRIVATE(KSycocaEntry)
};

#endif
