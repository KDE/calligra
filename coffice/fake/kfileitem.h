#ifndef FAKE_KFILEITEM_H
#define FAKE_KFILEITEM_H

#include <QList>
#include <QDebug>
#include <kfile.h>
#include <kurl.h>
#include <kmimetype.h>
#include <kio/job.h>

class KFileItem
{
public:
    enum { Unknown = static_cast<mode_t>(-1) };

    enum FileTimes {
        ModificationTime = 0,
        AccessTime = 1,
        CreationTime = 2
    };

    KFileItem() {}
    KFileItem( const KIO::UDSEntry& entry, const KUrl& itemOrDirUrl, bool delayedMimeTypes = false, bool urlIsDirectory = false ) : m_url(itemOrDirUrl) {
        qWarning() << Q_FUNC_INFO << "Unhandled KIO::UDSEntry";
    }
    KFileItem( mode_t mode, mode_t permissions, const KUrl& url, bool delayedMimeTypes = false ) : m_url(url) {
        qWarning() << Q_FUNC_INFO << "Unhandled KIO::UDSEntry";
    }
    KFileItem( const KUrl &url, const QString &mimeType, mode_t mode ) : m_url(url) {}

    //void refresh() {}
    //void refreshMimeType() {}

    KUrl url() const { return m_url; }
    void setUrl( const KUrl &url ) { m_url = url; }

    QString timeString(FileTimes) const { return QString(); }

#if 0
    /**
     * Sets the item's name (i.e. the filename).
     * This is automatically done by setUrl, to set the name from the URL's fileName().
     * This method is provided for some special cases like relative paths as names (KFindPart)
     * @param name the item's name
     */
    void setName( const QString &name );
#endif

    /**
     * Returns the permissions of the file (stat.st_mode containing only permissions).
     * @return the permissions of the file
     */
    mode_t permissions() const
    {
        return Unknown;
    }

#if 0
    /**
     * Returns the access permissions for the file as a string.
     * @return the access persmission as string
     */
    QString permissionsString() const;

    /**
     * Tells if the file has extended access level information ( Posix ACL )
     * @return true if the file has extend ACL information or false if it hasn't
     */
    bool hasExtendedACL() const;

    /**
     * Returns the access control list for the file.
     * @return the access control list as a KACL
     */
    KACL ACL() const;

    /**
     * Returns the default access control list for the directory.
     * @return the default access control list as a KACL
     */
    KACL defaultACL() const;

    /**
     * Returns the file type (stat.st_mode containing only S_IFDIR, S_IFLNK, ...).
     * @return the file type
     */
    mode_t mode() const;

    /**
     * Returns the owner of the file.
     * @return the file's owner
     */
    QString user() const;

    /**
     * Returns the group of the file.
     * @return the file's group
     */
    QString group() const;

    /**
     * Returns true if this item represents a link in the UNIX sense of
     * a link.
     * @return true if the file is a link
     */
    bool isLink() const;

    /**
     * Returns true if this item represents a directory.
     * @return true if the item is a directory
     */
    bool isDir() const;

    /**
     * Returns true if this item represents a file (and not a a directory)
     * @return true if the item is a file
     */
    bool isFile() const;

    /**
     * Checks whether the file or directory is readable. In some cases
     * (remote files), we may return true even though it can't be read.
     * @return true if the file can be read - more precisely,
     *         false if we know for sure it can't
     */
    bool isReadable() const;

    /**
     * Checks whether the file or directory is writable. In some cases
     * (remote files), we may return true even though it can't be written to.
     * @return true if the file or directory can be written to - more precisely,
     *         false if we know for sure it can't
     */
#endif

    bool isWritable() const { return true; }

#if 0
    /**
     * Checks whether the file is hidden.
     * @return true if the file is hidden.
     */
    bool isHidden() const;

    /**
     * @return true if the file is a remote URL, or a local file on a network mount.
     * It will return false only for really-local file systems.
     * @since 4.7.4
     */
    bool isSlow() const;

    /**
     * Checks whether the file is a readable local .desktop file,
     * i.e. a file whose path can be given to KDesktopFile
     * @return true if the file is a desktop file.
     * @since 4.1
     */
    bool isDesktopFile() const;

    /**
     * Returns the link destination if isLink() == true.
     * @return the link destination. QString() if the item is not a link
     */
    QString linkDest() const;

    /**
     * Returns the target url of the file, which is the same as url()
     * in cases where the slave doesn't specify UDS_TARGET_URL
     * @return the target url.
     * @since 4.1
     */
    KUrl targetUrl() const;

    /**
     * Returns the resource URI to be used for Nepomuk annotations. In case
     * the slave does not specify UDS_NEPOMUK_URI an invalid url is
     * returned.
     * For local files this is the same as url().
     * @return The Nepomuk resource URI.
     * @since 4.4
     */
    KUrl nepomukUri() const;

    /**
     * Returns the local path if isLocalFile() == true or the KIO item has
     * a UDS_LOCAL_PATH atom.
     * @return the item local path, or QString() if not known
     */
    QString localPath() const;

    /**
     * Returns the size of the file, if known.
     * @return the file size, or 0 if not known
     */
    KIO::filesize_t size() const;

    /**
     * Returns true if the file is a local file.
     * @return true if the file is local, false otherwise
     */
    bool isLocalFile() const;

    /**
     * Returns the text of the file item.
     * It's not exactly the filename since some decoding happens ('%2F'->'/').
     * @return the text of the file item
     */
    QString text() const;
#endif

    /**
     * Return the name of the file item (without a path).
     * Similar to text(), but unencoded, i.e. the original name.
     * @param lowerCase if true, the name will be returned in lower case,
     * which is useful to speed up sorting by name, case insensitively.
     * @return the file's name
     */
    QString name( bool lowerCase = false ) const
    {
        QString n = m_url.fileName();
        return lowerCase ? n.toLower() : n;
    }

#if 0
    /**
     * Returns the mimetype of the file item.
     * If @p delayedMimeTypes was used in the constructor, this will determine
     * the mimetype first. Equivalent to determineMimeType()->name()
     * @return the mime type of the file
     */
    QString mimetype() const;

    /**
     * Returns the mimetype of the file item.
     * If delayedMimeTypes was used in the constructor, this will determine
     * the mimetype first.
     * @return the mime type
     */
    KMimeType::Ptr determineMimeType() const;

    /**
     * Returns the currently known mimetype of the file item.
     * This will not try to determine the mimetype if unknown.
     * @return the known mime type
     */
    KMimeType::Ptr mimeTypePtr() const;

    /**
     * @return true if we have determined the mimetype of this file already,
     * i.e. if determineMimeType() will be fast. Otherwise it will have to
     * find what the mimetype is, which is a possibly slow operation; usually
     * this is delayed until necessary.
     */
    bool isMimeTypeKnown() const;

    /**
     * Returns the user-readable string representing the type of this file,
     * like "OpenDocument Text File".
     * @return the type of this KFileItem
     */
    QString mimeComment() const;
#endif

    QString iconName() const { return QString(); }

#if 0
    /**
     * Returns a pixmap representing the file.
     * @param _size Size for the pixmap in pixels. Zero will return the
     * globally configured default size.
     * @param _state The state of the icon: KIconLoader::DefaultState,
     * KIconLoader::ActiveState or KIconLoader::DisabledState.
     * @return the pixmap
     */
    QPixmap pixmap( int _size, int _state=0 ) const;

    /**
     * Returns the overlays (bitfield of KIconLoader::*Overlay flags) that are used
     * for this item's pixmap. Overlays are used to show for example, whether
     * a file can be modified.
     * @return the overlays of the pixmap
     */
    QStringList overlays() const;

    /**
     * A comment which can contain anything - even rich text. It will
     * simply be displayed to the user as is.
     *
     * @since 4.6
     */
    QString comment() const;

    /**
     * Returns the string to be displayed in the statusbar,
     * e.g. when the mouse is over this item
     * @return the status bar information
     */
    QString getStatusBarInfo() const;

    /**
     * Let's "KRun" this file !
     * (e.g. when file is clicked or double-clicked or return is pressed)
     */
    void run( QWidget* parentWidget = 0 ) const;

    /**
     * Returns the UDS entry. Used by the tree view to access all details
     * by position.
     * @return the UDS entry
     */
    KIO::UDSEntry entry() const;

    /**
     * Used when updating a directory. marked == seen when refreshing.
     * @return true if the file item is marked
     */
    bool isMarked() const;
    /**
     * Marks the item.
     * @see isMarked()
     */
    void mark();
    /**
     * Unmarks the item.
     * @see isMarked()
     */
    void unmark();

    /**
     * Return true if this item is a regular file,
     * false otherwise (directory, link, character/block device, fifo, socket)
     * @since 4.3
     */
    bool isRegularFile() const;

    /**
     * Somewhat like a comparison operator, but more explicit,
     * and it can detect that two kfileitems are equal even when they do
     * not share the same internal pointer - e.g. when KDirLister compares
     * fileitems after listing a directory again, to detect changes.
     * @param item the item to compare
     * @return true if all values are equal
     */
    bool cmp( const KFileItem & item ) const;

    bool operator==(const KFileItem& other) const;

    bool operator!=(const KFileItem& other) const;

    /**
     * Sets the metainfo of this item to @p info.
     *
     * Made const to avoid deep copy.
     * @param info the new meta info
     */
    void setMetaInfo( const KFileMetaInfo & info ) const;

    /**
     * Returns the metainfo of this item.
     *
     * (since 4.4.3) By default it uses the KFileMetaInfo::ContentInfo | KFileMetaInfo::TechnicalInfo.
     * If you need more information, create your own KFileMetaInfo object and set it using setMetaInfo()
     * @param autoget if true, the metainfo will automatically be created
     * @param what how much metainfo you need to retrieve from the file (KFileMetaInfo::WhatFlag)
     */
    KFileMetaInfo metaInfo(bool autoget = true,
                           int what = KFileMetaInfo::ContentInfo | KFileMetaInfo::TechnicalInfo) const;

    /**
     * Tries to give a local URL for this file item if possible.
     * The given boolean indicates if the returned url is local or not.
     */
    KUrl mostLocalUrl(bool &local) const; // KDE4 TODO: bool* local = 0

    /**
     * Tries to give a local URL for this file item if possible.
     *
     * \since 4.6
     */
    KUrl mostLocalUrl() const; // KDE5: merge with above version
#endif

    bool isNull() const { return m_url.isEmpty(); }

private:
    KUrl m_url;
};

Q_DECLARE_METATYPE(KFileItem)

class KFileItemList : public QList<KFileItem>
{
public:
  //KFileItem findByName( const QString& fileName ) const;
  //KFileItem findByUrl( const KUrl& url ) const;
  //KUrl::List urlList() const;
  //KUrl::List targetUrlList() const;
};

#endif
 
 
