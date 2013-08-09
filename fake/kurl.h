#ifndef FAKE_KURL_H
#define FAKE_KURL_H

#include <QUrl>
#include <QList>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>

class KUrl : public QUrl
{
public:

  class List : public QList<KUrl>
  {
  public:

#if 0
    /**
     * Creates an empty List.
     */
      List() { }
      /**
       * Creates a list that contains the given URL as only
       * item.
       * @param url the url to add.
       */
      List(const KUrl &url);
      /**
       * Creates a list that contains the URLs from the given
       * list of strings.
       * @param list the list containing the URLs as strings
       */
      List(const QStringList &list);
      /**
       * Creates a list that contains the URLs from the given QList<KUrl>.
       * @param list the list containing the URLs
       */
      List(const QList<KUrl> &list);
      /**
       * Creates a list that contains the URLs from the given QList<KUrl>.
       * @param list the list containing the URLs
       * @since 4.7
       */
      List(const QList<QUrl> &list);
      /**
       * Converts the URLs of this list to a list of strings.
       * @return the list of strings
       */
      QStringList toStringList() const;

      /**
       * Converts the URLs of this list to a list of strings.
       *
       * @param trailing use to add or remove a trailing slash to/from the path.
       *
       * @return the list of strings
       *
       * @since 4.6
       */
      QStringList toStringList(KUrl::AdjustPathOption trailing) const;

      /**
       * Converts this KUrl::List to a QVariant, this allows to use KUrl::List
       * in QVariant() constructor
       */
      operator QVariant() const;

      /**
       * Converts this KUrl::List into a list of QUrl instances.
       * @since 4.7
       */
      operator QList<QUrl>() const;

      /**
       * Adds URLs data into the given QMimeData.
       *
       * By default, populateMimeData also exports the URLs as plain text, for e.g. dropping
       * onto a text editor.
       * But in some cases this might not be wanted, e.g. if adding other mime data
       * which provides better plain text data.
       *
       * WARNING: do not call this method multiple times on the same mimedata object,
       * you can add urls only once. But you can add other things, e.g. images, XML...
       *
       * @param mimeData the QMimeData instance used to drag or copy this URL
       * @param metaData KIO metadata shipped in the mime data, which is used for instance to
       * set a correct HTTP referrer (some websites require it for downloading e.g. an image)
       * @param flags set NoTextExport to prevent setting plain/text data into @p mimeData
       * In such a case, setExportAsText( false ) should be called.
       */
      void populateMimeData( QMimeData* mimeData,
                             const KUrl::MetaDataMap& metaData = MetaDataMap(),
                             MimeDataFlags flags = DefaultMimeDataFlags ) const;

      /**
       * Adds URLs into the given QMimeData.
       *
       * This should add both the KDE-style URLs (eg: desktop:/foo) and
       * the "most local" version of the URLs (eg:
       * file:///home/jbloggs/Desktop/foo) to the mimedata.
       *
       * This method should be called on the KDE-style URLs.
       *
       * @code
       * QMimeData* mimeData = new QMimeData();
       *
       * KUrl::List kdeUrls;
       * kdeUrls << "desktop:/foo";
       * kdeUrls << "desktop:/bar";
       *
       * KUrl::List normalUrls;
       * normalUrls << "file:///home/jbloggs/Desktop/foo";
       * normalUrls << "file:///home/jbloggs/Desktop/bar";
       *
       * kdeUrls.populateMimeData(normalUrls, mimeData);
       * @endcode
       *
       * @param mostLocalUrls the "most local" urls
       * @param mimeData      the mime data object to populate
       * @param metaData      KIO metadata shipped in the mime data, which is
       *                      used for instance to set a correct HTTP referrer
       *                      (some websites require it for downloading e.g. an
       *                      image)
       * @param flags         set NoTextExport to prevent setting plain/text
       *                      data into @p mimeData. In such a case,
       *                      <code>setExportAsText(false)</code> should be called.
       * @since 4.2
       */
      void populateMimeData(const KUrl::List& mostLocalUrls,
                            QMimeData* mimeData,
                            const KUrl::MetaDataMap& metaData = MetaDataMap(),
                            MimeDataFlags flags = DefaultMimeDataFlags) const;

      /**
       * Return true if @p mimeData contains URI data
       */
      static bool canDecode( const QMimeData *mimeData );

      /**
        * Return the list of mimeTypes that can be decoded by fromMimeData
        */
      static QStringList mimeDataTypes();

      /**
       * Extract a list of KUrls from the contents of @p mimeData.
       * Decoding will fail if @p mimeData does not contain any URLs, or if at
       * least one extracted URL is not valid.
       * @param mimeData the mime data to extract from; cannot be 0
       * @param metaData optional pointer to a map holding the metadata
       * @return the list of urls
       */
      static KUrl::List fromMimeData( const QMimeData *mimeData, KUrl::MetaDataMap* metaData = 0 );

      /**
       * Flags to be used in fromMimeData.
       * @since 4.2.3
       */
      enum DecodeOptions {
        /**
         * When the mimedata contains both KDE-style URLs (eg: desktop:/foo) and
         * the "most local" version of the URLs (eg: file:///home/dfaure/Desktop/foo),
         * decode it as local urls. Useful in paste/drop operations that end up calling KIO,
         * so that urls from other users work as well.
         */
        PreferLocalUrls,
        /**
         * When the mimedata contains both KDE-style URLs (eg: desktop:/foo) and
         * the "most local" version of the URLs (eg: file:///home/dfaure/Desktop/foo),
         * decode it as the KDE-style URL. Useful in DnD code e.g. when moving icons,
         * and the kde-style url is used as identifier for the icons.
         */
        PreferKdeUrls
      };

      /**
       * Extract a list of KUrls from the contents of @p mimeData.
       * Decoding will fail if @p mimeData does not contain any URLs, or if at
       * least one extracted URL is not valid.
       * @param mimeData the mime data to extract from; cannot be 0
       * @param decodeOptions options for decoding
       * @param metaData optional pointer to a map holding the metadata
       * @return the list of urls
       * @since 4.2.3
       */
      static KUrl::List fromMimeData( const QMimeData *mimeData,
                                      DecodeOptions decodeOptions, // TODO KDE5: = PreferKdeUrls, and merge with above
                                      KUrl::MetaDataMap* metaData = 0 );
#endif
    };

    enum DirectoryOption { ObeyTrailingSlash = 0x02, AppendTrailingSlash = 0x04, IgnoreTrailingSlash = 0x01 };
    Q_DECLARE_FLAGS(DirectoryOptions, DirectoryOption)

    enum AdjustPathOption { RemoveTrailingSlash, LeaveTrailingSlash, AddTrailingSlash };

    KUrl() : QUrl() {}
    KUrl(const QString &urlOrPath) : QUrl(QUrl::fromLocalFile(urlOrPath)) {}
    KUrl(const QByteArray& urlOrPath) : QUrl(QUrl::fromLocalFile(urlOrPath)) {}
    KUrl(const char* urlOrPath) : QUrl(urlOrPath) {}
    KUrl(const QUrl &u) : QUrl(u) {}
    KUrl(const KUrl &u) : QUrl(u.toString()) {}

    QByteArray protocol() const
    {
        return scheme().toUtf8();
    }

    void setProtocol(const QByteArray &p)
    {
        setScheme(QString::fromUtf8(p));
    }

//     QUrl url() const
//     {
//         return *this;
//     }

    QString url(AdjustPathOption = RemoveTrailingSlash) const
    {
        qDebug() << Q_FUNC_INFO << "url=" << toString();
        //return toEncoded();
        return toString();
    }

    QString toLocalFile( AdjustPathOption trailing = LeaveTrailingSlash ) const
    {
        if (!host().isEmpty() && isLocalFile()) {
            KUrl urlWithoutHost(*this);
            urlWithoutHost.setHost(QString());
            return trailingSlash(trailing, urlWithoutHost.toLocalFile());
        }

        return trailingSlash(trailing, QUrl::toLocalFile());
    }

    static QString toPrettyPercentEncoding(const QString &input, bool forFragment)
    {
        QString result;
        result.reserve(input.length());
        for (int i = 0; i < input.length(); ++i) {
            const QChar c = input.at(i);
            register ushort u = c.unicode();
            if (u < 0x20
                || (!forFragment && u == '?') // don't escape '?' in fragments, not needed and wrong (#173101)
                || u == '#' || u == '%'
                || (u == ' ' && (i+1 == input.length() || input.at(i+1).unicode() == ' '))) {
                static const char hexdigits[] = "0123456789ABCDEF";
                result += QLatin1Char('%');
                result += QLatin1Char(hexdigits[(u & 0xf0) >> 4]);
                result += QLatin1Char(hexdigits[u & 0xf]);
            } else {
                result += c;
            }
        }
        return result;
    }

    QString prettyUrl( AdjustPathOption trailing = LeaveTrailingSlash ) const
    {
        QString result = scheme();
        if (!result.isEmpty()) {
            if (!authority().isEmpty() || result == QLatin1String("file") || path().isEmpty())
                result += QLatin1String("://");
            else
                result += QLatin1Char(':');
        }

        QString tmp = userName();
        if (!tmp.isEmpty()) {
            result += QString::fromLatin1(QUrl::toPercentEncoding(tmp));
            result += QLatin1Char('@');
        }

        tmp = host();
        if (tmp.contains(QLatin1Char(':')))
            result += QLatin1Char('[') + tmp + QLatin1Char(']');
        else
            result += tmp;

        if (port() != -1) {
            result += QLatin1Char(':');
            result += QString::number(port());
        }

        tmp = path();
#ifdef Q_WS_WIN
        if (isLocalFile())
            tmp.prepend(QLatin1Char('/'));
#endif
        result += toPrettyPercentEncoding(tmp, false);

        if (trailing == AddTrailingSlash && !tmp.endsWith(QLatin1Char('/')))
            result += QLatin1Char('/');
        else if (trailing == RemoveTrailingSlash && tmp.length() > 1 && tmp.endsWith(QLatin1Char('/')))
            result.chop(1);

        if (hasQuery()) {
            result += QLatin1Char('?');
            result += QString::fromLatin1(encodedQuery());
        }
        if (hasFragment()) {
            result += QLatin1Char('#');
            result += toPrettyPercentEncoding(fragment(), true);
        }
        return result;
    }

    QString pathOrUrl(AdjustPathOption trailing = LeaveTrailingSlash) const
    {
        if ( isLocalFile() && fragment().isNull() && encodedQuery().isNull() ) {
            return toLocalFile(trailing);
        } else {
            return prettyUrl(trailing);
        }
    }

    QString fileName(DirectoryOptions trailing = IgnoreTrailingSlash) const
    {
        Q_UNUSED(trailing);
        return QFileInfo(toLocalFile()).fileName();
    }

    void setDirectory(const QString &dir)
    {
        setPath(QFileInfo(dir, fileName()).absoluteFilePath());
    }

    QString directory(const DirectoryOptions& options = IgnoreTrailingSlash) const
    {
        Q_UNUSED(options);
        return QFileInfo(toLocalFile()).absolutePath();
    }

    void setFileName(const QString &file)
    {
        QFileInfo fi(QDir(directory()), file);
        setPath(fi.filePath());
    }

    void addPath(QString s)
    {
        setPath(QFileInfo(path(), s).absoluteFilePath());
    }

    static KUrl fromPath(const QString &path)
    {
        KUrl u;
        u.setPath(path);
        return u;
    }

    void adjustPath(AdjustPathOption trailing) {Q_UNUSED(trailing);}

    static bool isRelativeUrl(const QString &url) { return QUrl(url).isRelative(); }

private:

    static QString trailingSlash( KUrl::AdjustPathOption trailing, const QString &path )
    {
        if ( trailing == KUrl::LeaveTrailingSlash )
            return path;
        QString result = path;
        if ( trailing == KUrl::AddTrailingSlash ) {
            int len = result.length();
            if ( (len == 0) || (result[ len - 1 ] != QLatin1Char('/')) )
                result += QLatin1Char('/');
            return result;
        } else if ( trailing == KUrl::RemoveTrailingSlash ) {
            if ( result == QLatin1String("/") )
                return result;
            int len = result.length();
            while (len > 1 && result[ len - 1 ] == QLatin1Char('/'))
                len--;
            result.truncate( len );
            return result;
        } else {
            return result;
        }
    }

};

Q_DECLARE_METATYPE(KUrl)

#endif
