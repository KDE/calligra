#ifndef FAKE_KMIMETYPE_H
#define FAKE_KMIMETYPE_H

#include <kurl.h>
#include <kservicetype.h>
#include <koservicetype.h>

#include <QStringList>
#include <QRegExp>
#include <QDebug>

class KMimeType : public KoServiceType
{
public:
    typedef KSharedPtr<KMimeType> Ptr;
    typedef QList<Ptr> List;

    static List allMimeTypes()
    {
        static List m_allMimeTypes;
        if (m_allMimeTypes.isEmpty()) {
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.text"), QStringList() << "*.odt"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.spreadsheet"), QStringList() << "*.ods"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.presentation"), QStringList() << "*.odp"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.graphics"), QStringList() << "*.odg"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.chart"), QStringList() << "*.odc"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.formula"), QStringList() << "*.odf"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.image"), QStringList() << "*.odi"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.oasis.opendocument.text-master"), QStringList() << "*.odm"));

            m_allMimeTypes << Ptr(new KMimeType(QString("application/msword"), QStringList() << "*.doc" << "*.dot"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.wordprocessingml.document"), QStringList() << "*.docx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.wordprocessingml.template"), QStringList() << "*.dotx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-word.document.macroEnabled.12"), QStringList() << "*.docm"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-word.template.macroEnabled.12"), QStringList() << "*.dotm"));

            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-excel"), QStringList() << "*.xls" << "*.xlt" << "*.xla"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"), QStringList() << "*.xlsx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.spreadsheetml.template"), QStringList() << "*.xltx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-excel.sheet.macroEnabled.12"), QStringList() << "*.xlsm"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-excel.template.macroEnabled.12"), QStringList() << "*.xltm"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-excel.addin.macroEnabled.12"), QStringList() << "*.xlam"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-excel.sheet.binary.macroEnabled.12"), QStringList() << "*.xlsb"));

            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-powerpoint"), QStringList() << "*.ppt" << "*.pot" << "*.pps" << "*.ppa"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.presentationml.presentation"), QStringList() << "*.pptx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.presentationml.template"), QStringList() << "*.potx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.openxmlformats-officedocument.presentationml.slideshow"), QStringList() << "*.ppsx"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-powerpoint.addin.macroEnabled.12"), QStringList() << "*.ppam"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-powerpoint.presentation.macroEnabled.12"), QStringList() << "*.pptm"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-powerpoint.template.macroEnabled.12"), QStringList() << "*.potm"));
            m_allMimeTypes << Ptr(new KMimeType(QString("application/vnd.ms-powerpoint.slideshow.macroEnabled.12"), QStringList() << "*.ppsm"));

            m_allMimeTypes << Ptr(new KMimeType(QString("application/pdf"), QStringList() << "*.pdf"));
        }
        return m_allMimeTypes;
    }

    enum FindByNameOption { DontResolveAlias, ResolveAliases = 1 };

    QString name() const
    {
        return m_name;
    }

    bool isEmpty() const
    {
        return m_name.isEmpty();
    }

    static Ptr findByContent( const QByteArray &data, int *accuracy=0 )
    {
        qWarning() << Q_FUNC_INFO << "TODO";
        return defaultMimeTypePtr();
    }
    
    static Ptr mimeType( const QString& name, FindByNameOption options = ResolveAliases )
    {
        Q_FOREACH(const Ptr &m, allMimeTypes())
            if (m->name() == name)
                return m;
        qWarning()<<Q_FUNC_INFO<<"No mimetype for name="<<name;
        return defaultMimeTypePtr();
    }

    bool is( const QString& mimeTypeName ) const
    {
        qDebug()<<Q_FUNC_INFO<<mimeTypeName<<m_name;
        return mimeTypeName == m_name;
    }

    QString iconName( const KUrl &url = KUrl() ) const
    {
        return QString();
    }

    static QString iconNameForUrl( const KUrl & url, mode_t mode = 0 )
    {
        return QString();
    }

    static QString favIconForUrl( const KUrl& url )
    {
        return QString();
    }

    QString comment( const KUrl& url = KUrl() ) const
    {
        return QString();
    }

    QStringList patterns() const
    {
        qDebug()<<Q_FUNC_INFO<<m_patterns;
        return m_patterns;
    }

    static Ptr findByUrl( const KUrl& url, mode_t mode = 0, bool is_local_file = false, bool fast_mode = false, int *accuracy = 0 )
    {
        QString file = url.toLocalFile();
        Q_FOREACH(const Ptr &m, allMimeTypes()) {
            Q_FOREACH(const QString &p, m->patterns()) {
                QRegExp rx(p, Qt::CaseInsensitive, QRegExp::Wildcard);
                if (rx.exactMatch(file))
                    return m;
            }
        }
        qWarning() << Q_FUNC_INFO << "No mimetype for url=" << url << "file=" << file;
        return defaultMimeTypePtr();
    }

    static Ptr findByPath( const QString& path, mode_t mode = 0, bool fast_mode = false, int* accuracy = 0 )
    {
        return findByUrl(KUrl(path), mode, true, fast_mode, accuracy);
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
        qDebug() << Q_FUNC_INFO << fileName;
        return findByUrl(KUrl(fileName), 0, false, false, accuracy);
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
#endif

    static QString defaultMimeType()
    {
        static QString defaultmimetype = "application/octet-stream";
        return defaultmimetype;
    }

    static KMimeType::Ptr defaultMimeTypePtr()
    {
        //static KMimeType::Ptr defaultmimetype = KMimeType::Ptr(new KMimeType(defaultMimeType()));
        static KMimeType::Ptr defaultmimetype;
        if (!defaultmimetype)
            defaultmimetype = Ptr(new KMimeType());
        return defaultmimetype;
    }

    bool isDefault() const { return m_name.isEmpty(); }

#if 0

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

    QString mainExtension() const
    {
        QString result;
        if (!m_patterns.isEmpty()) {
            result = m_patterns.first();
            int delimiter = result.lastIndexOf(".");
            int wildcard = result.lastIndexOf("*");
            if (delimiter > 0 && wildcard < delimiter) {
                result = result.mid(delimiter, result.length() - delimiter);
            }
        }
        qDebug() << Q_FUNC_INFO << "mainExtension=" << result;
        return result;
    }

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
    KMimeType(const QString& name = QString(), const QStringList &patterns = QStringList()) : KoServiceType(0), m_name(name), m_patterns(patterns) {}
#endif

private:
    QString m_name;
    QStringList m_patterns;
};

#endif
