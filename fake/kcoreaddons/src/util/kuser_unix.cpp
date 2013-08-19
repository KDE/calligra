/*
 *  KUser - represent a user/account
 *  Copyright (C) 2002 Tim Jansen <tim@tjansen.de>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
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
 */

#include <kuser.h>

#include <QtCore/QMutableStringListIterator>
#include <QtCore/QDir>

#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>
#include <grp.h>

class KUser::Private : public QSharedData
{
public:
    uid_t uid;
    gid_t gid;
    QString loginName;
    QString homeDir, shell;
    QMap<UserProperty, QVariant> properties;

    Private() : uid(uid_t(-1)), gid(gid_t(-1)) {}
    Private(const char *name) : uid(uid_t(-1)), gid(gid_t(-1))
    {
        fillPasswd(name ? ::getpwnam( name ) : 0);
    }
    Private(const passwd *p) : uid(uid_t(-1)), gid(gid_t(-1))
    {
        fillPasswd(p);
    }

    void fillPasswd(const passwd *p)
    {
        if (p) {
            QString gecos = QString::fromLocal8Bit(p->pw_gecos);
            QStringList gecosList = gecos.split(QLatin1Char(','));
            // fill up the list, should be at least 4 entries
            while (gecosList.size() < 4)
                gecosList << QString();

            uid = p->pw_uid;
            gid = p->pw_gid;
            loginName = QString::fromLocal8Bit(p->pw_name);
            properties[KUser::FullName] = QVariant(gecosList[0]);
            properties[KUser::RoomNumber] = QVariant(gecosList[1]);
            properties[KUser::WorkPhone] = QVariant(gecosList[2]);
            properties[KUser::HomePhone] = QVariant(gecosList[3]);
            if (uid == ::getuid() && uid == ::geteuid())
                homeDir = QFile::decodeName(qgetenv("HOME"));
            if (homeDir.isEmpty())
                homeDir = QFile::decodeName(p->pw_dir);
            shell = QString::fromLocal8Bit(p->pw_shell);
        }
    }
};


KUser::KUser(UIDMode mode)
{
	uid_t _uid = ::getuid(), _euid;
	if (mode == UseEffectiveUID && (_euid = ::geteuid()) != _uid )
		d = new Private( ::getpwuid( _euid ) );
	else {
		d = new Private( qgetenv( "LOGNAME" ).constData() );
		if (uid() != _uid) {
			d = new Private( qgetenv( "USER" ).constData() );
			if (uid() != _uid)
				d = new Private( ::getpwuid( _uid ) );
		}
	}
}

KUser::KUser(K_UID _uid)
    : d(new Private( ::getpwuid( _uid ) ))
{
}

KUser::KUser(const QString& name)
    : d(new Private( name.toLocal8Bit().data() ))
{
}

KUser::KUser(const char *name)
    : d(new Private( name ))
{
}

KUser::KUser(const passwd *p)
    : d(new Private( p ))
{
}

KUser::KUser(const KUser & user)
  : d(user.d)
{
}

KUser& KUser::operator =(const KUser& user)
{
  d = user.d;
  return *this;
}

bool KUser::operator ==(const KUser& user) const {
	return (uid() == user.uid()) && (uid() != uid_t(-1));
}

bool KUser::operator !=(const KUser& user) const {
	return (uid() != user.uid()) || (uid() == uid_t(-1));
}

bool KUser::isValid() const {
	return uid() != uid_t(-1);
}

K_UID KUser::uid() const {
	return d->uid;
}

K_GID KUser::gid() const {
	return d->gid;
}

bool KUser::isSuperUser() const {
	return uid() == 0;
}

QString KUser::loginName() const {
	return d->loginName;
}

#ifndef KDE_NO_DEPRECATED
QString KUser::fullName() const {
	return d->properties[FullName].toString();
}
#endif

QString KUser::homeDir() const {
	return d->homeDir;
}

QString KUser::faceIconPath() const
{
    QString pathToFaceIcon(homeDir() + QDir::separator() + QLatin1String(".face.icon"));

    if (QFile::exists(pathToFaceIcon)) {
        return pathToFaceIcon;
    }

    return QString();
}

QString KUser::shell() const {
	return d->shell;
}

QList<KUserGroup> KUser::groups() const {
  QList<KUserGroup> result;
  const QList<KUserGroup> allGroups = KUserGroup::allGroups();
  QList<KUserGroup>::const_iterator it;
  for ( it = allGroups.begin(); it != allGroups.end(); ++it ) {
    const QList<KUser> users = (*it).users();
    if ( users.contains(*this) ) {
       result.append(*it);
    }
  }
  return result;
}

QStringList KUser::groupNames() const {
  QStringList result;
  const QList<KUserGroup> allGroups = KUserGroup::allGroups();
  QList<KUserGroup>::const_iterator it;
  for ( it = allGroups.begin(); it != allGroups.end(); ++it ) {
    const QList<KUser> users = (*it).users();
    if ( users.contains(*this) ) {
       result.append((*it).name());
    }
  }
  return result;
}

QVariant KUser::property(UserProperty which) const
{
    return d->properties.value(which);
}

QList<KUser> KUser::allUsers() {
  QList<KUser> result;

  passwd* p;

  while ((p = getpwent()))  {
    result.append(KUser(p));
  }

  endpwent();

  return result;
}

QStringList KUser::allUserNames() {
  QStringList result;

  passwd* p;

  while ((p = getpwent()))  {
    result.append(QString::fromLocal8Bit(p->pw_name));
  }

  endpwent();
  return result;
}

KUser::~KUser() {
}

class KUserGroup::Private : public QSharedData
{
public:
    gid_t gid;
    QString name;
    QList<KUser> users;

    Private() : gid(gid_t(-1)) {}
    Private(const char *_name) : gid(gid_t(-1))
    {
        fillGroup(_name ? ::getgrnam( _name ) : 0);
    }
    Private(const ::group *p) : gid(gid_t(-1))
    {
        fillGroup(p);
    }

    void fillGroup(const ::group *p) {
        if (p) {
            gid = p->gr_gid;
            name = QString::fromLocal8Bit(p->gr_name);
            for (char **user = p->gr_mem; *user; user++)
                users.append(KUser(*user));
        }
    }
};

KUserGroup::KUserGroup(KUser::UIDMode mode)
{
    d = new Private(getgrgid(KUser(mode).gid()));
}

KUserGroup::KUserGroup(K_GID _gid)
    : d(new Private(getgrgid(_gid)))
{
}

KUserGroup::KUserGroup(const QString& _name)
    : d(new Private(_name.toLocal8Bit().data()))
{
}

KUserGroup::KUserGroup(const char *_name)
    : d(new Private(_name))
{
}

KUserGroup::KUserGroup(const ::group *g)
    : d(new Private(g))
{
}

KUserGroup::KUserGroup(const KUserGroup & group)
  : d(group.d)
{
}

KUserGroup& KUserGroup::operator =(const KUserGroup& group) {
  d = group.d;
  return *this;
}

bool KUserGroup::operator ==(const KUserGroup& group) const {
	return (gid() == group.gid()) && (gid() != gid_t(-1));
}

bool KUserGroup::operator !=(const KUserGroup& user) const {
	return (gid() != user.gid()) || (gid() == gid_t(-1));
}

bool KUserGroup::isValid() const {
	return gid() != gid_t(-1);
}

K_GID KUserGroup::gid() const {
	return d->gid;
}

QString KUserGroup::name() const {
	return d->name;
}

QList<KUser> KUserGroup::users() const {
	return d->users;
}

QStringList KUserGroup::userNames() const {
  QStringList result;
  QList<KUser>::const_iterator it;
  for ( it = d->users.begin(); it != d->users.end(); ++it ) {
    result.append((*it).loginName());
  }
  return result;
}

QList<KUserGroup> KUserGroup::allGroups() {
  QList<KUserGroup> result;

  ::group* g;
  while ((g = getgrent()))  {
     result.append(KUserGroup(g));
  }

  endgrent();

  return result;
}

QStringList KUserGroup::allGroupNames() {
  QStringList result;

  ::group* g;
  while ((g = getgrent()))  {
     result.append(QString::fromLocal8Bit(g->gr_name));
  }

  endgrent();

  return result;
}

KUserGroup::~KUserGroup() {
}
