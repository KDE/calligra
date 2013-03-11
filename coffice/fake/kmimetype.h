#ifndef FAKE_KMIMETYPE_H
#define FAKE_KMIMETYPE_H

#include <kurl.h>
#include <kservicetype.h>

#include <QStringList>
#include <QDebug>

class KMimeType : public KServiceType
{
public:
    typedef KSharedPtr<KMimeType> Ptr;
    typedef QList<Ptr> List;

    enum FindByNameOption { DontResolveAlias, ResolveAliases = 1 };

    QString name() const
    {
        qWarning() << Q_FUNC_INFO << "TODO";
        return QString();
    }

    bool isEmpty() const
    {
        qWarning() << Q_FUNC_INFO << "TODO";
        return true;
    }

    static Ptr findByContent( const QByteArray &data, int *accuracy=0 )
    {
        qWarning() << Q_FUNC_INFO << "TODO";
        return Ptr();
    }
    
    static Ptr mimeType( const QString& name, FindByNameOption options = ResolveAliases )
    {
        qWarning() << Q_FUNC_INFO << "TODO" << name;
        return Ptr();
    }

    bool is( const QString& mimeTypeName ) const
    {
        qWarning() << Q_FUNC_INFO << "TODO" << mimeTypeName;
        return false;
    }

    QString iconName( const KUrl &url = KUrl()) const
    {
        qWarning() << Q_FUNC_INFO << "TODO" << url;
        return QString();
    }

#if 0
    /**
     * Return the filename of the icon associated with the mimetype, for a given url.
     * Use KIconLoader::loadMimeTypeIcon to load the icon.
     * @param url URL for the file
     * @param mode the mode of the file. The mode may modify the icon
     *              with overlays that show special properties of the
     *              icon. Use 0 for default
     * @return the name of the icon. The name of a default icon if there is no icon
     *         for the mime type
     */
    static QString iconNameForUrl( const KUrl & url, mode_t mode = 0 );

    /**
     * Return the "favicon" (see http://www.favicon.com) for the given @p url,
     * if available. Does NOT attempt to download the favicon, it only returns
     * one that is already available.
     *
     * If unavailable, returns QString().
     * @param url the URL of the favicon
     * @return the name of the favicon, or QString()
     */
    static QString favIconForUrl( const KUrl& url );

#endif

    QString comment( const KUrl& url = KUrl() ) const
    {
        return QString();
    }

    QStringList patterns() const
    {
        return QStringList();
    }

    static Ptr findByUrl( const KUrl& url, mode_t mode = 0, bool is_local_file = false, bool fast_mode = false, int *accuracy = 0 )
    {
        qWarning() << Q_FUNC_INFO << "TODO" << url;
        return Ptr();
    }

    static Ptr findByPath( const QString& path, mode_t mode = 0, bool fast_mode = false, int* accuracy = 0 )
    {
        qWarning() << Q_FUNC_INFO << "TODO" << path;
        return Ptr();
    }

#if 0
    /**
     * Tries to find out the MIME type of a data chunk by looking for
     * certain magic numbers and characteristic strings in it.
     *
     * @param data the data to examine
     * @param accuracy If not a null pointer, *accuracy is set to the
     *          accuracy of the match (which is in the range 0..100)
     * @return a pointer to the KMimeType. "application/octet-stream" is
     *          returned if the type can not be found this way.
     */
    static Ptr findByContent( const QByteArray &data, int *accuracy=0 );

    /**
     * Tries to find out the MIME type of filename/url and a data chunk.
     * Whether to trust the extension or the data depends on the results of both approaches,
     * and is determined automatically.
     *
     * This method is useful for instance in the get() method of kioslaves, and anywhere else
     * where a filename is associated with some data which is available immediately.
     *
     * @param name the filename or url representing this data.
     * Only used for the extension, not used as a local filename.
     * @param data the data to examine when the extension isn't conclusive in itself
     * @param mode the mode of the file (used, for example, to identify executables)
     * @param accuracy If not a null pointer, *accuracy is set to the
     *          accuracy of the match (which is in the range 0..100)
     */
    static Ptr findByNameAndContent( const QString& name, const QByteArray& data,
                                     mode_t mode = 0, int *accuracy=0 );

    /**
     * Tries to find out the MIME type of a data chunk by looking for
     * certain magic numbers and characteristic strings in it.
     *
     * @param device the IO device providing the data to examine
     * @param accuracy If not a null pointer, *accuracy is set to the
     *          accuracy of the match (which is in the range 0..100)
     * @return a pointer to the KMimeType. "application/octet-stream" is
     *          returned if the type can not be found this way.
     * @since 4.4
     */
    static Ptr findByContent( QIODevice* device, int* accuracy = 0 );

    /**
     * Tries to find out the MIME type of filename/url and a data chunk.
     * Whether to trust the extension or the data depends on the results of both approaches,
     * and is determined automatically.
     *
     * This method is useful for instance in the get() method of kioslaves, and anywhere else
     * where a filename is associated with some data which is available immediately.
     *
     * @param name the filename or url representing this data.
     * Only used for the extension, not used as a local filename.
     * @param device the IO device providing the data to examine when the extension isn't conclusive in itself
     * @param mode the mode of the file (used, for example, to identify executables)
     * @param accuracy If not a null pointer, *accuracy is set to the
     *          accuracy of the match (which is in the range 0..100)
     * @return a pointer to the KMimeType. "application/octet-stream" is
     *          returned if the type can not be found this way.
     * @since 4.4
     */
    static Ptr findByNameAndContent( const QString& name, QIODevice* device,
                                     mode_t mode = 0, int* accuracy = 0 );
#endif

    static Ptr findByFileContent( const QString &fileName, int *accuracy=0 )
    {
        qWarning() << Q_FUNC_INFO << "TODO" << fileName;
        return Ptr();
    }

#if 0
    /**
     * Returns whether a file has an internal format that is not human readable.
     * This is much more generic than "not mime->is(text/plain)".
     * Many application file formats (like rtf and postscript) are based on text,
     * but text that the user should rarely ever see.
     */
    static bool isBinaryData( const QString &fileName );

    /**
     * Returns whether a buffer has an internal format that is not human readable.
     * This is much more generic than "not mime->is(text/plain)".
     * Many application file formats (like rtf and postscript) are based on text,
     * but text that the user should rarely ever see.
     */
    static bool isBufferBinaryData( const QByteArray &data );

    /**
     * Get all the mimetypes.
     *
     * Useful for showing the list of
     * available mimetypes.
     * More memory consuming than the ones above, don't use unless
     * really necessary.
     * @return the list of all existing KMimeTypes
     */
    static List allMimeTypes();
#endif

    static QString defaultMimeType()
    {
        static QString defaultmimetype = "application/octet-stream";
        return defaultmimetype;
    }

    /**
     * Returns the default mimetype.
     * Always application/octet-stream.
     * This can be used to check the result of mimeType(name).
     * @return the "application/octet-stream" mimetype pointer.
     */
    static KMimeType::Ptr defaultMimeTypePtr()
    {
        //static KMimeType::Ptr defaultmimetype = KMimeType::Ptr(new KMimeType(defaultMimeType()));
        static KMimeType::Ptr defaultmimetype;
        return defaultmimetype;
    }

#if 0
    /// Return true if this mimetype is the default mimetype
    bool isDefault() const;

    /**
     * If this mimetype is a subclass of another mimetype,
     * return the name of the parent.
     *
     * @return the parent mime type, or QString() if not set
     *
     * @deprecated this method does not support multiple inheritance,
     * which is actually part of the shared-mime-info standard.
     * Use is(), parentMimeTypes(), or allParentMimeTypes() instead of parentMimeType()
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED QString parentMimeType() const;
#endif

    /**
     * If this mimetype is a subclass of one or more other mimetypes,
     * return the list of those mimetypes.
     *
     * For instance a application/javascript is a special kind of text/plain,
     * so the definition of application/javascript says
     *      sub-class-of type="text/plain"
     *
     * Another example: application/x-shellscript is a subclass of two other mimetypes,
     * application/x-executable and text/plain.
     *
     * (Note that this notion doesn't map to the servicetype inheritance mechanism,
     * since an application that handles the specific type doesn't necessarily handle
     * the base type. The opposite is true though.)
     *
     * @return the list of parent mimetypes
     * @since 4.1
     */
    QStringList parentMimeTypes() const;

    /**
     * Return all parent mimetypes of this mimetype, direct or indirect.
     * This includes the parent(s) of its parent(s), etc.
     * If this mimetype is an alias, the list also contains the canonical
     * name for this mimetype.
     *
     * The usual reason to use this method is to look for a setting which
     * is stored per mimetype (like PreviewJob does).
     * @since 4.1
     */
    QStringList allParentMimeTypes() const;

    /**
     * Returns the user-specified icon for this mimetype. This is empty most of the time,
     * you probably want to use iconName() instead. This method is for the mimetype editor.
     * @since 4.3
     */
    QString userSpecifiedIconName() const;
#endif

    /**
     * Return the primary extension associated with this mimetype, if any.
     * If patterns() returns (*.jpg, *.jpeg) then mainExtension will return ".jpg".
     * Note that the dot is included.
     * If none of the patterns is in *.foo format (for instance
     *   <code>*.jp? or *.* or callgrind.out* </code>)
     * then mainExtension() returns an empty string.
     */
    QString mainExtension() const { return QString(); }

#if 0
    /**
     * Determines the extension from a filename (or full path) using the mimetype database.
     * This allows to extract "tar.bz2" for foo.tar.bz2
     * but still return "txt" for my.doc.with.dots.txt
     */
    static QString extractKnownExtension( const QString &fileName );

    /**
     * Returns true if the given filename matches the given pattern.
     * @since 4.6.1
     */
    static bool matchFileName( const QString &filename, const QString &pattern );

    /**
     * Returns the version of the installed update-mime-database program
     * (from freedesktop.org shared-mime-info). This is used by unit tests
     * and by the code that writes out icon definitions.
     * @since 4.3
     * @return -1 on error, otherwise a version number to use like this:
     * @code
     * if (version >= KDE_MAKE_VERSION(0, 40, 0)) { ... }
     * @endcode
     */
    static int sharedMimeInfoVersion();
#endif

protected:

#if 0
    friend class KMimeTypeRepository; // for KMimeType(QString,QString,QString)

    /**
     * @internal Construct a service from a stream.
     *
     * The stream must already be positionned at the correct offset
     */
    KMimeType( QDataStream& str, int offset );

    /**
     * Construct a mimetype and take all information from an XML file.
     * @param fullpath the path to the xml that describes the mime type
     * @param name the name of the mimetype (usually the end of the path)
     * @param comment the comment associated with the mimetype
     */
    KMimeType( const QString& fullpath, const QString& name, const QString& comment );

    /**
     * Construct a mimetype from another mimetype's private object
     *
     * @param dd the private object
     */
    KMimeType( KMimeTypePrivate &dd);

    /**
     * Construct a mimetype based on another mimetype's private object
     *
     * Allows the name and comment to be overridden.
     *
     * @param dd the private object
     * @param name the name of the mimetype
     * @param comment the comment associated with the mimetype
     */
    KMimeType( KMimeTypePrivate &dd, const QString& name, const QString& comment );
#else
    //KMimeType() {}
#endif

};

#endif
