#ifndef __kpimage_h__
#define __kpimage_h__

#include <qtextstream.h>

#include <koImageCollection.h>

struct KPImageKey
{
    KPImageKey()
        : filename(), lastModified()
        {}

    KPImageKey( const QString &fn, const QDateTime &mod )
        : filename( fn ), lastModified( mod )
        {}
    KPImageKey( const KPImageKey &key )
        : filename( key.filename ), lastModified( key.lastModified )
        {}

    KPImageKey &operator=( const KPImageKey &key ) {
        filename = key.filename;
        lastModified = key.lastModified;
        return *this;
    }

    bool operator==( const KPImageKey &key ) const {
        return ( key.filename == filename &&
                 key.lastModified == lastModified );
    }

    bool operator<( const KPImageKey &key ) const {
        return key.toString() < toString();
    }

    QString toString() const {
        return QString::fromLatin1( "%1_%2" ).arg( filename ).arg( lastModified.toString() );
    }

    QString filename;
    QDateTime lastModified;
};

class KPImageCollection : public KoImageCollection<KPImageKey>
{
public:
    KPImageCollection()
        { m_tmpDate = QDate::currentDate(); m_tmpTime = QTime::currentTime(); }

    KoImage<KPImageKey> loadImage( const KPImageKey &key );

    KoImage<KPImageKey> loadImage( const KPImageKey &key, const QString &rawData );

    // this is ugly, but it was in KPPixmapCollection
    QDate tmpDate() const { return m_tmpDate; }
    QTime tmpTime() const { return m_tmpTime; }

private:
    QDate m_tmpDate;
    QTime m_tmpTime;
};

typedef KPImageCollection::Image KPImage;

QTextStream &operator<<( QTextStream &out, KPImageKey &key );

QTextStream &operator<<( QTextStream &out, KPImage &img );

#endif
