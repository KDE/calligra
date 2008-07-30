/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIBLOBBUFFER_H
#define KEXIBLOBBUFFER_H

#include <qobject.h>
#include <q3intdict.h>
#include <q3dict.h>
#include <qpixmap.h>

#include <kurl.h>

#include <kexi_export.h>

namespace KexiDB
{
  class Connection;
}

//! Application-wide buffer for local BLOB data like pixmaps.
/*! For now only pixmaps are supported 
 @todo support any KPart-compatible objects and more...

 Use this class by acessing to its singleton: KexiBLOBBuffer::self().

 This class is used for buffering BLOB data, 
 to avoid duplicating object's data in memory and a need for loading (decoding) 
 the same object many times. 
 The data is always local, what means database storage is not employed here.
 
 Each BLOB instance is identified by an unsigned integer number (Id_t type), 
 uniquely generated on BLOB loading. Each BLOB can have assigned source url 
 it has been loaded from (the url can be empty though, e.g. for data coming from clipboard). 

 References to KexiBLOBBuffer are counted, so when last reference is lost, data is freed
 and the integer identifier is no longer pointing any valid data.
 KexiBLOBBuffer::Handle is value-based class that describes handle based in an identifier.
 Objects of this class are obtained e.g. from insertPixmap() method.

 There are two kinds of identifiers:
 - integers assigned for BLOBs already saved to a db backend, 
    when KexiBLOBBuffer::Handle::stored() is true
 - temporary integers assigned for new BLOBs not yet saved to a db backend, 
    when KexiBLOBBuffer::Handle::stored() is false
 KexiBLOBBuffer::Handle::setStoredWidthID() can be used to switch from unstored to stored state.
 Among others, the state has effect on saving forms: only unstored BLOBs will be saved back 
 to the database; when a BLOB needs to be removed, only it will be physically removed only if it was stored.

 KexiBLOBBuffer is also useful for two more reasons:
 - Property editor's item for "image" property displays a preview of pixmap contents.
   Without buffering, it would be needed to load pixmap data again: what if the file
   it is loaded from is located remote and connection is slow? Memory would be also unnecessary doubled.
 - Undo/Redo framework requires to store previous property values. Having a reference defined 
   by a single interger, memory can be handled more effectively. 

 Example use cases:
 A large pixmap file "abc.jpg" is loaded as QByteArray <b>once</b> and buffered:
 integer identifier is returned.
 Then, multiple image widgets are using "abc.jpg" for displaying.
 Duplicating an image widget means only duplicating it's properties 
 like position and BLOB's id: BLOB itself (data of "abc.jpg") is not duplicated.
 Creating a new image widget and assiging the same "abc.jpg" pixmap, means only 
 referencing KexiBLOBBuffer using the same identifier.
*/
class KEXICORE_EXPORT KexiBLOBBuffer : public QObject
{
  Q_OBJECT

  private:
    class Item;
  public:
    //! long integer for unique identifying blobs
//! @todo Qt4: will be changed
    typedef long Id_t;

    //! Access to KexiBLOBBuffer singleton
    static KexiBLOBBuffer* self();

    static void setConnection(KexiDB::Connection *conn);

    //! Object handle used by KexiBLOBBuffer
    class KEXICORE_EXPORT Handle {
      public:
        //! Constructs a null handle. 
        //! Null handles have empty pixap and data members, id == 0 and cast to boolean false.
        Handle();

        //! Constructs a copy of \a handle.
        Handle(const Handle& handle);

        ~Handle();

        Id_t id() const { return m_item ? m_item->id : 0; }

        /*! \return true if this BLOB data pointed by this handle is stored at the db backend
         or false if it is kept in memory. Null handles return false. */
        bool stored() const { return m_item ? m_item->stored : false; }

        //! \return true if this is null handle (i.e. one not pointing to any data)
        operator bool() const { return m_item; }

        Handle& operator=(const Handle& handle);

        QByteArray data() const { return m_item ? m_item->data() : QByteArray(); }

        QPixmap pixmap() const { return m_item ? m_item->pixmap() : QPixmap(); }

        /*! Sets "stored" flag to true by setting non-temporary identifier.
         Only call this method for unstored (in memory) BLOBs */
        void setStoredWidthID(Id_t id);

        QString originalFileName() const { return m_item ? m_item->name: QString(); }

        QString mimeType() const { return m_item ? m_item->mimeType : QString(); }

        Id_t folderId() const { return m_item ? m_item->folderId : 0; }

      protected:
        //! Constructs a handle based on \a item. Null handle is constructed for null \a item.
        Handle(Item* item);
      private:
        Item* m_item;
      friend class KexiBLOBBuffer;
    };

    //! @internal
    KexiBLOBBuffer();

    ~KexiBLOBBuffer();

    /*! Inserts a new pixmap loaded from a file at \a url. 
     If the same file has already been loaded before, it can be found in cache 
     and returned instantly. It is assumed that the BLOB is unstored, because it is loaded from 
     external source, so stored() will be equal to false for returned handle.
     \return handle to the pixmap data or a null handle if such pixmap could not be loaded. */
    Handle insertPixmap(const KUrl& url);

    /*! Inserts a new BLOB data. 
     @param data The data for BLOB object.
     @param name The name for the object, usually a file name or empty
     @param caption The more friendly than name, can be based on file name or empty or
            defined by a user (this case is not yet used)
     @param mimeType The mimeType for the object for easier and mor accurate decoding.
     @param identifier Object's identifier. If positive, the "stored" flag for the data 
     will be set to true with \a identifer, otherwise (the default) the BLOB data will 
     have "stored" flag set to false, and a new temporary identifier will be assigned. */
    Handle insertObject(const QByteArray& data, const QString& name, 
      const QString& caption, const QString& mimeType, Id_t identifier = 0);

    /*! Inserts a new pixmap available in memory, e.g. coming from clipboard. */
    Handle insertPixmap(const QPixmap& pixmap);

    /*! \return an object for a given \a id. If \a stored is true, stored BLOBs buffer 
     is browsed, otherwise unstored (in memory) BLOBs buffer is browsed.
     If no object is cached for this id, null handle is returned. */
    Handle objectForId(Id_t id, bool stored);

    /*! \return an object for a given \a id. First, unstored object is checked, then unstored, 
     if stored was not found. */
    Handle objectForId(Id_t id);

  protected:
    /*! Removes an object for a given \a id. If \a stored is true, stored BLOB is removed,
     otherwise unstored (in memory) BLOB is removed. */
    void removeItem(Id_t id, bool stored);

    /*! Takes an object for a \a item out of the buffer. */
    void takeItem(Item* item);

    /*! Inserts an object for a given \a id into the buffer. */
    void insertItem(Item* item);

  private:
    class KEXICORE_EXPORT Item {
      public:
        Item(const QByteArray& data, Id_t ident,
          bool stored,
          const QString& name = QString(),
          const QString& caption = QString(),
          const QString& mimeType = QString(),
          Id_t folderId = 0,
          const QPixmap& pixmap = QPixmap());
        ~Item();
        QPixmap pixmap() const;
        QByteArray data() const;
//				KexiBLOBBuffer* buf;
//				KUrl url;
        QString name;
        QString caption; //!< @todo for future use within image gallery
        QString mimeType;
        uint refs;
        Id_t id;
        Id_t folderId;
        bool stored : 1;
        QString prettyURL; //!< helper
      private:
        QByteArray *m_data;
        QPixmap *m_pixmap;
        bool *m_pixmapLoaded; //!< *m_pixmapLoaded will be set in Info::pixmap(), 
                              //!< to avoid multiple pixmap decoding when it previously failed
      friend class KexiBLOBBuffer;
    };
    class Private;
    Private *d;
    friend class Handle;
};

#endif
