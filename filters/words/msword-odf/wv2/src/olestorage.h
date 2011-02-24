/* This file is part of the wvWare 2 project
   Copyright (C) 2001-2003 Werner Trobin <trobin@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02111-1307, USA.
*/

#ifndef OLESTORAGE_H
#define OLESTORAGE_H

#include <string>
#include <list>
#include <deque>

#include <gsf/gsf.h>
#include "wv2_export.h"

namespace wvWare
{

class OLEStream;
class OLEStreamReader;
class OLEStreamWriter;

class WV2_EXPORT OLEStorage
{
    friend class OLEStream;
public:
    /**
     * The mode of the storage. libgsf doesn't support storages opened
     * for reading and writing like libole2 did.
     */
    enum Mode { ReadOnly, WriteOnly };

    /**
     * Create an "empty" storage
     */
    OLEStorage();
    /**
     * Specify a name for the storage. Note: It will *not* be opened
     * right now, call @see open() to do that
     */
    OLEStorage( const std::string& fileName );
    /**
     * Specify a buffer for the storage. The OLE storage will be read
     * from that buffer. We don't take ownership of the buffer.
     * Note: Only for ReadOnly mode!
     */
    OLEStorage( const unsigned char* buffer, size_t buflen );
    /**
     * Destroy the current storage. Open streams on it will
     * be synced and written back.
     */
    ~OLEStorage();

    /**
     * Specify a name for the storage (file)
     * This is a no-op if a storage is already open!
     */
    void setName( const std::string& fileName );
    /**
     * Get the name of the OLE storage file
     */
    std::string name() const { return m_fileName; }

    /**
     * Specify a buffer for the storage. The OLE storage will be read
     * from that buffer. We don't take ownership of the buffer.
     * Note: Only for ReadOnly mode!
     */
    void setBuffer( const unsigned char* buffer, size_t buflen );

    /**
     * Open the specified storage for reading or writing.
     * Opening a storage twice won't do any harm.
     * @return true if opening was successful
     */
    bool open( Mode mode );

    /**
     * Closes the current storage (no-op if we don't have one ;)
     * This method also tries to close/sync all the open streams.
     * Closing a storage twice won't do any harm.
     */
    void close();

    /**
     * Any problems with the current storage?
     */
    bool isValid() const;

    /**
     * Returns a list containing all the files and dirs at the
     * current position in the VFS. The std::strings are UTF-8
     * encoded.
     * Only works in ReadOnly mode, gsf doesn't support reading
     * the directory structure of WriteOnly storages.
     */
    std::list<std::string> listDirectory();

    /**
     * Enters a specific subdirectory. If the specified directory
     * doesn't exist this method returns false. The passed string
     * has to be UTF-8 encoded.
     */
    bool enterDirectory( const std::string& directory );
    /**
     * One level up (cd ..)
     */
    void leaveDirectory();

    /**
     * Manipulate the path directly. If the operation fails at some
     * point the path will be set back to the previous location.
     */
    bool setPath( const std::string& path );
    /**
     * Get the current path (UTF-8 encoded)
     */
    std::string path() const;

    /**
     * Opens the specified stream for reading and passes the
     * opened stream reader back. Returns 0 if it didn't work,
     * e.g. if the storage is opened in WriteOnly mode.
     * Note: The ownership is transferred to you!
     */
    OLEStreamReader* createStreamReader( const std::string& stream );

    /**
     * Opens a stream for writing (you get 0 if it failed, e.g. if
     * the storage is in ReadOnly mode).
     * Note: The ownership is transferred to you!
     * Note2: Don't try to pass names with a '/' in it :)
     */
    OLEStreamWriter* createStreamWriter( const std::string& stream );

private:
    /**
     * we don't want to allow copying
     */
    OLEStorage( const OLEStorage& rhs );
    /**
     * we don't want to allow assigning
     */
    OLEStorage& operator=( const OLEStorage& rhs );

    /**
     * This method is called from the stream's DTOR
     */
    void streamDestroyed( OLEStream* stream );

    // One of those members will be 0, we only use one of them,
    // due to the libgsf design...
    GsfInfile* m_inputFile;
    GsfOutfile* m_outputFile;

    std::string m_fileName;

    const unsigned char* m_buffer;
    size_t m_buflen;

    union Directory
    {
        Directory( GsfInfile* in ) : infile( in ) {}
        Directory( GsfOutfile* out ) : outfile( out ) {}

        GsfInfile* infile;
        GsfOutfile* outfile;
    };
    std::deque<Directory> m_path;

    /**
     * We're not the owner, but we still keep track of all
     * the streams for bookkeeping issues. If the user forgets
     * to delete the OLEStreams we do it on closing.
     */
    std::list<OLEStream*> m_streams;
};

} // namespace wvWare

#endif // OLESTORAGE_H
