/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: clipart collection (header)                            */
/******************************************************************/

#ifndef kpclipartcollection_h
#define kpclipartcollection_h

#include <qmap.h>
#include <qdatetime.h>
#include <qcstring.h>
#include <qpicture.h>

#include "global.h"

#include <iostream.h>

/******************************************************************/
/* Class: KPClipartCollection                                     */
/******************************************************************/

class KPClipartCollection
{
public:
    struct Key
    {
        Key()
            : filename(), lastModified()
        {}

        Key( const QString &fn, const QDateTime &mod )
            : filename( fn ), lastModified( mod )
        {}
        Key( const Key &key )
            : filename( key.filename ), lastModified( key.lastModified )
        {}

        Key &operator=( const Key &key ) {
            filename = key.filename;
            lastModified = key.lastModified;
            return *this;
        }

        bool operator==( const Key &key ) const {
            return ( key.filename == filename &&
                     key.lastModified == lastModified );
        }

        bool operator<( const Key &key ) const {
            QString s1( key.toString() );
            QString s2( toString() );
            return ( s1 < s2 );
        }

        QString toString() const {
            QString s = QString( "%1_%2" ).arg( filename ).arg( lastModified.toString() );
            return QString( s );
        }

        QString filename;
        QDateTime lastModified;
    };

    KPClipartCollection()
        : allowChangeRef( false )
    { date = QDate::currentDate(); time = QTime::currentTime(); }
    ~KPClipartCollection();

    QPicture *findClipart( const Key &key );
    QPicture *insertClipart( const Key &key, const QPicture &pic );

    void addRef( const Key &key );
    void removeRef( const Key &key );

    QMap< Key, QPicture >::Iterator begin() { return data.begin(); }
    QMap< Key, QPicture >::Iterator end() { return data.end(); }

    int references( const Key &key ) { return refs.contains( key ) ? refs.find( key ).data() : -1; }

    QDate tmpDate() { return date; }
    QTime tmpTime() { return time; }

    void setAllowChangeRef( bool b )
    { allowChangeRef = b ; }

protected:
    QMap< Key, QPicture > data;
    QMap< Key, int > refs;

    QDate date;
    QTime time;

    bool allowChangeRef;

};

ostream& operator<<( ostream &out, KPClipartCollection::Key &key );

#endif
