/* This file is part of the KDE libraries
   Copyright (C) 2010 Teo Mrnjavac <teo@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KABOUT_APPLICATION_PERSON_MODEL_H
#define KABOUT_APPLICATION_PERSON_MODEL_H

#include "kaboutapplicationconfigattica_p.h"

#if HAVE_ATTICA
#include <attica/providermanager.h>
#include <attica/provider.h>
#endif //HAVE_ATTICA

#include <kaboutdata.h>

#include <QIcon>
#include <QtCore/QAbstractListModel>
#include <QPixmap>
#include <QtNetwork/QNetworkReply>
#include <QUrl>

// Forward declarations to make Attica-related members work
namespace Attica {
class BaseJob;
}

namespace KDEPrivate
{

class KAboutApplicationPersonProfile;
class KAboutApplicationPersonIconsJob;

class KAboutApplicationPersonModel : public QAbstractListModel
{
    Q_OBJECT
public:
    KAboutApplicationPersonModel( const QList< KAboutPerson > &personList,
                                  const QString &providerUrl = QString(),
                                  QObject *parent = 0 );

    int rowCount( const QModelIndex &parent = QModelIndex() ) const;
    int columnCount(const QModelIndex &parent = QModelIndex() ) const { Q_UNUSED( parent ) return 1; }
    QVariant data( const QModelIndex &index, int role ) const;

    Qt::ItemFlags flags( const QModelIndex &index ) const;

    bool hasAvatarPixmaps() const { return m_hasAvatarPixmaps; }

    const QString &providerName() const { return m_providerName; }

private Q_SLOTS:
    void onProvidersLoaded();
    void onPersonJobFinished( Attica::BaseJob *job );
    void onAvatarJobFinished( QNetworkReply *reply );
    void onOcsLinksJobFinished( KAboutApplicationPersonIconsJob *job );

private:
    void fetchOcsLinkIcons( int personProfileListIndex );

    QList< KAboutPerson > m_personList;
    QList< KAboutApplicationPersonProfile > m_profileList;

    QMap< int, QString > m_ocsLinkIconUrls;
    QMap< int, QPixmap > m_ocsLinkIcons;

    bool m_hasAvatarPixmaps;

#if HAVE_ATTICA
    Attica::ProviderManager m_providerManager;
    Attica::Provider m_provider;
#endif //HAVE_ATTICA
    QString m_providerUrl;
    QString m_providerName;

    friend class KAboutApplicationPersonIconsJob;
};

//This list must be in sync with the one in KAboutApplicationPersonProfileOcsLink::prettyType()
static const char s_personOcsLinkAtticaTypes[][16] = {
    { "other" },
    { "Blog" },
    { "delicious" },
    { "Digg" },
    { "Facebook" },
    { "Homepage" },
    { "identi.ca" },
    { "libre.fm" },
    { "LinkedIn" },
    { "MySpace" },
    { "Reddit" },
    { "StackOverflow" },
    { "Twitter" },
    { "Wikipedia" },
    { "Xing" },
    { "YouTube" } };

class KAboutApplicationPersonProfileOcsLink
{
public:
    enum Type
    {
        Other = 0,
        Blog,
        Delicious,
        Digg,
        Facebook,
        Homepage,
        Identica,
        LibreFm,
        LinkedIn,
        MySpace,
        Reddit,
        StackOverflow,
        Twitter,
        Wikipedia,
        Xing,
        YouTube,
        NUM_ATTICA_LINK_TYPES
    };

    static Type typeFromAttica( const QString &atticaType );

    KAboutApplicationPersonProfileOcsLink( Type type, const QUrl &url )
        : m_type( type )
        , m_url( url )
    {}

    Type type() const { return m_type; }
    QString prettyType() const;
    void setIcon( const QIcon &icon ) { m_icon = icon; }
    const QIcon &icon() const { return m_icon; }
    const QUrl & url() const { return m_url; }

private:
    Type m_type;
    QUrl m_url;
    QIcon m_icon;
};

class KAboutApplicationPersonProfile
{
public:
    KAboutApplicationPersonProfile()
        : m_name()
        , m_task()
        , m_email()
        , m_ocsUsername() {} //needed for QVariant

    KAboutApplicationPersonProfile( const QString &name,
                                    const QString &task,
                                    const QString &email,
                                    const QString &ocsUsername = QString() )
        : m_name( name )
        , m_task( task )
        , m_email( email )
        , m_ocsUsername( ocsUsername )
    {}

    void setHomepage( const QUrl &url ) { m_homepage = url; }
    void setAvatar( const QPixmap &pixmap ) { m_avatar = pixmap; }
    void setLocation( const QString &location ) { m_location = location; }
    void setOcsProfileUrl( const QString &url ) { m_ocsProfileUrl = url; }
    void addAdditionalString( const QString &string ) { m_additionalStrings << string; }
    void setOcsLinks( const QList< KAboutApplicationPersonProfileOcsLink > &ocsLinks )
        { m_ocsLinks = ocsLinks; }

    const QString & name() const { return m_name; }
    const QString & task() const { return m_task; }
    const QString & email() const { return m_email; }
    const QString & ocsUsername() const { return m_ocsUsername; }
    const QString & ocsProfileUrl() const { return m_ocsProfileUrl; }
    const QUrl & homepage() const { return m_homepage; }
    const QPixmap & avatar() const { return m_avatar; }
    const QString & location() const { return m_location; }
    const QStringList & additionalStrings() const { return m_additionalStrings; }
    const QList< KAboutApplicationPersonProfileOcsLink > & ocsLinks() const { return m_ocsLinks; }

private:
    QString m_name;
    QString m_task;
    QString m_email;
    QString m_ocsUsername;
    QString m_ocsProfileUrl;
    QUrl m_homepage;
    QPixmap m_avatar;
    QString m_location;
    QStringList m_additionalStrings;
    QList< KAboutApplicationPersonProfileOcsLink > m_ocsLinks;
};

class KAboutApplicationPersonIconsJob : public QObject
{
    Q_OBJECT
public:
    KAboutApplicationPersonIconsJob( KAboutApplicationPersonModel *model,
                                    int personProfileListIndex );

    void start();

    int personProfileListIndex() const { return m_personProfileListIndex; }

    const QList< KAboutApplicationPersonProfileOcsLink > & ocsLinks() const { return m_ocsLinks; }

Q_SIGNALS:
    void finished( KAboutApplicationPersonIconsJob *job);

private Q_SLOTS:
    void onJobFinished( QNetworkReply *reply );

private:
    void getIcons( int i );

    int m_personProfileListIndex;
    KAboutApplicationPersonModel *m_model;
    QList< KAboutApplicationPersonProfileOcsLink > m_ocsLinks;

    QNetworkAccessManager *m_manager;
};

} //namespace KDEPrivate

Q_DECLARE_METATYPE( KDEPrivate::KAboutApplicationPersonProfile )
#endif // KABOUT_APPLICATION_PERSON_MODEL_H
