/*
 *  KUser - represent a user/account
 *  Copyright (C) 2002-2003 Tim Jansen <tim@tjansen.de>
 *  Copyright (C) 2003 Oswald Buddenhagen <ossi@kde.org>
 *  Copyright (C) 2004 Jan Schaefer <j_schaef@informatik.uni-kl.de>
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
#ifndef KUSER_H
#define KUSER_H

#include <kcoreaddons_export.h>

#include <QSharedDataPointer>
#include <QtCore/QVariant>

class KUserGroup;
class QString;
class QStringList;
template <class T> class QList;

#ifdef Q_OS_WIN
typedef void *K_UID;
typedef void *K_GID;
#else
#include <sys/types.h>
typedef uid_t K_UID;
typedef gid_t K_GID;
struct passwd;
struct group;
#endif


/**
 * \class KUser kuser.h <KUser>
 *
 * @short Represents a user on your system
 *
 * This class represents a user on your system. You can either get
 * information about the current user, of fetch information about
 * a user on the system. Instances of this class will be explicitly shared,
 * so copying objects is very cheap and you can safely pass objects by value.
 *
 * @author Tim Jansen <tim@tjansen.de>
 */
class KCOREADDONS_EXPORT KUser {

public:

  enum UIDMode {
  	UseEffectiveUID, ///< Use the effective user id.
  	UseRealUserID    ///< Use the real user id.
  };

  /**
   * Creates an object that contains information about the current user.
   * (as returned by getuid(2) or geteuid(2), taking $LOGNAME/$USER into
   * account).
   * @param mode if #UseEffectiveUID is passed the effective
   *             user is returned.
   *        If #UseRealUserID is passed the real user will be
   *        returned.
   *        The real UID will be different than the effective UID in setuid
   *        programs; in
   *        such a case use the effective UID for checking permissions, and
   *        the real UID for displaying information about the user.
   */
  explicit KUser(UIDMode mode = UseEffectiveUID);

  /**
   * Creates an object for the user with the given user id.
   * If the user does not exist isValid() will return false.
   * @param uid the user id
   */
  explicit KUser(K_UID uid);

  /**
   * Creates an object that contains information about the user with the given
   * name. If the user does not exist isValid() will return false.
   *
   * @param name the name of the user
   */
  explicit KUser(const QString& name);

  /**
   * Creates an object that contains information about the user with the given
   * name. If the user does not exist isValid() will return false.
   *
   * @param name the name of the user
   */
  explicit KUser(const char* name);

#ifndef Q_OS_WIN
  /**
   * Creates an object from a passwd structure.
   * If the pointer is null isValid() will return false.
   *
   * @param p the passwd structure to create the user from
   */
  explicit KUser(const passwd *p);
#endif

  /**
   * Creates an object from another KUser object
   * @param user the user to create the new object from
   */
  KUser(const KUser & user);

  /**
   * Copies a user
   * @param user the user to copy
   * @return this object
   */
  KUser& operator =(const KUser& user);

  /**
   * Two KUser objects are equal if the uid() are identical.
   * Invalid users never compare equal.
   */
  bool operator ==(const KUser& user) const;

  /**
   * Two KUser objects are not equal if uid() are not identical.
   * Invalid users always compare unequal.
   */
  bool operator !=(const KUser &user) const;

  /**
   * Returns true if the user is valid. A KUser object can be invalid if
   * you created it with an non-existing uid or name.
   * @return true if the user is valid
   */
  bool isValid() const;

  /**
   * Returns the user id of the user.
   * @return the id of the user or -1 if user is invalid
   */
  K_UID uid() const;

#ifndef Q_OS_WIN
  /**
   * Returns the group id of the user.
   * @return the id of the group or -1 if user is invalid
   */
  K_GID gid() const;
#endif

  /**
   * Checks whether the user is the super user (root).
   * @return true if the user is root
   */
  bool isSuperUser() const;

  /**
   * The login name of the user.
   * @return the login name of the user or QString() if user is invalid
   */
  QString loginName() const;

  /**
   * The full name of the user.
   * @return the full name of the user or QString() if user is invalid
   * @deprecated use property(KUser::FullName) instead
   */
#ifndef KDE_NO_DEPRECATED
  KCOREADDONS_DEPRECATED QString fullName() const;
#endif

  /**
   * The path to the user's home directory.
   * @return the home directory of the user or QString() if the
   *         user is invalid
   */
  QString homeDir() const;

  /**
   * The path to the user's face file.
   * @return the path to the user's face file or QString() if no
   *         face has been set
   */
  QString faceIconPath() const;

  /**
   * The path to the user's login shell.
   * @return the login shell of the user or QString() if the
   *         user is invalid
   */
  QString shell() const;

  /**
   * Returns all groups of the user
   * @return all groups of the user
   */
  QList<KUserGroup> groups() const;

  /**
   * Returns all group names of the user
   * @return all group names of the user
   */
  QStringList groupNames() const;

  enum UserProperty { FullName, RoomNumber, WorkPhone, HomePhone };

  /**
   * Returns an extended property.
   *
   * Under Windows, @p RoomNumber, @p WorkPhone and @p HomePhone are unsupported.
   *
   * @return a QVariant with the value of the property or an invalid QVariant,
   *         if the property is not set
   */
  QVariant property(UserProperty which) const;

  /**
   * Destructor.
   */
  ~KUser();

  /**
   * Returns all users of the system.
   * @return all users of the system.
   */
  static QList<KUser> allUsers();

  /**
   * Returns all user names of the system.
   * @return all user names of the system.
   */
  static QStringList allUserNames();

private:
  class Private;
  QSharedDataPointer<Private> d;
};

/**
 * \class KUserGroup kuser.h <KUserGroup>
 *
 * @short Represents a group on your system
 *
 * This class represents a group on your system. You can either get
 * information about the group of the current user, of fetch information about
 * a group on the system. Instances of this class will be explicitly shared,
 * so copying objects is very cheap and you can safely pass objects by value.
 *
 * @author Jan Schaefer <j_schaef@informatik.uni-kl.de>
 */
class KCOREADDONS_EXPORT KUserGroup {

public:

  /**
   * Create an object from a group name.
   * If the group does not exist, isValid() will return false.
   * @param name the name of the group
   */
  explicit KUserGroup(const QString& name);

  /**
   * Create an object from a group name.
   * If the group does not exist, isValid() will return false.
   * @param name the name of the group
   */
  explicit KUserGroup(const char *name);

#ifndef Q_OS_WIN
  /**
   * Create an object from the group of the current user.
   * @param mode if #KUser::UseEffectiveUID is passed the effective user
   *        will be used. If #KUser::UseRealUserID is passed the real user
   *        will be used.
   *        The real UID will be different than the effective UID in setuid
   *        programs; in  such a case use the effective UID for checking
   *        permissions, and the real UID for displaying information about
   *        the group associated with the user.
   */
  explicit KUserGroup(KUser::UIDMode mode = KUser::UseEffectiveUID);

  /**
   * Create an object from a group id.
   * If the group does not exist, isValid() will return false.
   * @param gid the group id
   */
  explicit KUserGroup(K_GID gid);

  /**
   * Creates an object from a group structure.
   * If the pointer is null, isValid() will return false.
   * @param g the group structure to create the group from.
   */
  explicit KUserGroup(const group *g);
#endif

  /**
   * Creates a new KUserGroup instance from another KUserGroup object
   * @param group the KUserGroup to copy
   */
  KUserGroup(const KUserGroup & group);

  /**
   * Copies a group
   * @param group the group that should be copied
   * @return this group
   */
  KUserGroup& operator =(const KUserGroup& group);

  /**
   * Two KUserGroup objects are equal if their gid()s are identical.
   * Invalid groups never compare equal.
   * @return true if the groups are identical
   */
  bool operator ==(const KUserGroup& group) const;

  /**
   * Two KUserGroup objects are not equal if their gid()s are not identical.
   * Invalid groups always compare unequal.
   * @return true if the groups are not identical
   */
  bool operator !=(const KUserGroup& group) const;

  /**
   * Returns whether the group is valid.
   * A KUserGroup object can be invalid if it is
   * created with a non-existing gid or name.
   * @return true if the group is valid
   */
  bool isValid() const;

#ifndef Q_OS_WIN
  /**
   * Returns the group id of the group.
   * @return the group id of the group or -1 if the group is invalid
   */
  K_GID gid() const;
#endif

  /**
   * The name of the group.
   * @return the name of the group
   */
  QString name() const;

  /**
   * Returns a list of all users of the group.
   * @return a list of all users of the group
   */
  QList<KUser> users() const;

  /**
   * Returns a list of all user login names of the group.
   * @return a list of all user login names of the group
   */
  QStringList userNames() const;

  /**
   * Destructor.
   */
  ~KUserGroup();

  /**
   * Returns a list of all groups on this system
   */
  static QList<KUserGroup> allGroups();

  /**
   * Returns a list of all group names on this system
   */
  static QStringList allGroupNames();

private:
  class Private;
  QSharedDataPointer<Private> d;
};

#endif
