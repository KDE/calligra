/*
   This file is part of the KDE libraries
   Copyright (c) 2006, 2007 Thomas Braxton <kde.braxton@gmail.com>
   Copyright (c) 1999 Preston Brown <pbrown@kde.org>
   Copyright (c) 1997 Matthias Kalle Dalheimer <kalle@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Qt5 TODO: re-enable. No point in doing it before, it breaks on QString::fromUtf8(QByteArray), which exists in qt5.
#undef QT_NO_CAST_FROM_BYTEARRAY

#include "kconfiggroup.h"
#include "kconfiggroup_p.h"

#include "kconfig.h"
#include "kconfig_p.h"
#include "ksharedconfig.h"
#include "kconfigdata.h"

#include <QtCore/QDate>
#include <QtCore/QSharedData>
#include <QtCore/QFile>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QTextStream>
#include <QtCore/QDir>
#include <QtCore/QUrl>

#include <stdlib.h>

class KConfigGroupPrivate : public QSharedData
{
 public:
    KConfigGroupPrivate(KConfig* owner, bool isImmutable, bool isConst, const QByteArray &name)
        : mOwner(owner), mName(name), bImmutable(isImmutable), bConst(isConst)
    {
    }

    KConfigGroupPrivate(const KSharedConfigPtr &owner, const QByteArray& name)
        : sOwner(owner), mOwner(sOwner.data()), mName(name),
          bImmutable(name.isEmpty()? owner->isImmutable(): owner->isGroupImmutable(name)), bConst(false)
    {
    }

    KConfigGroupPrivate(KConfigGroup* parent, bool isImmutable, bool isConst, const QByteArray& name)
        : sOwner(parent->d->sOwner), mOwner(parent->d->mOwner), mName(name),
          bImmutable(isImmutable), bConst(isConst)
    {
        if (!parent->d->mName.isEmpty())
            mParent = parent->d;
    }

    KConfigGroupPrivate(const KConfigGroupPrivate* other, bool isImmutable, const QByteArray &name)
        : sOwner(other->sOwner), mOwner(other->mOwner), mName(name),
          bImmutable(isImmutable), bConst(other->bConst)
    {
        if (!other->mName.isEmpty())
            mParent = const_cast<KConfigGroupPrivate *>(other);
    }

    KSharedConfig::Ptr sOwner;
    KConfig *mOwner;
    QExplicitlySharedDataPointer<KConfigGroupPrivate> mParent;
    QByteArray mName;

    /* bitfield */
    const bool bImmutable:1; // is this group immutable?
    const bool bConst:1; // is this group read-only?

    QByteArray fullName() const
    {
        if (!mParent) {
            return name();
        }
        return mParent->fullName(mName);
    }

    QByteArray name() const
    {
        if (mName.isEmpty())
            return "<default>";
        return mName;
    }

    QByteArray fullName(const QByteArray& aGroup) const
    {
        if (mName.isEmpty())
            return aGroup;
        return fullName() + '\x1d' + aGroup;
    }

    static QExplicitlySharedDataPointer<KConfigGroupPrivate> create(KConfigBase *master,
                                                                    const QByteArray &name,
                                                                    bool isImmutable,
                                                                    bool isConst)
    {
        QExplicitlySharedDataPointer<KConfigGroupPrivate> data;
        if (dynamic_cast<KConfigGroup*>(master))
            data = new KConfigGroupPrivate(dynamic_cast<KConfigGroup*>(master), isImmutable, isConst, name);
        else
            data = new KConfigGroupPrivate(dynamic_cast<KConfig*>(master), isImmutable, isConst, name);
        return data;
    }

    static QByteArray serializeList(const QList<QByteArray> &list);
    static QStringList deserializeList(const QString &data);
};

QByteArray KConfigGroupPrivate::serializeList(const QList<QByteArray> &list)
{
    QByteArray value = "";

    if (!list.isEmpty()) {
        QList<QByteArray>::ConstIterator it = list.constBegin();
        const QList<QByteArray>::ConstIterator end = list.constEnd();

        value = QByteArray(*it).replace('\\', "\\\\").replace(',', "\\,");

        while (++it != end) {
            // In the loop, so it is not done when there is only one element.
            // Doing it repeatedly is a pretty cheap operation.
            value.reserve(4096);

            value += ',';
            value += QByteArray(*it).replace('\\', "\\\\").replace(',', "\\,");
        }

        // To be able to distinguish an empty list from a list with one empty element.
        if (value.isEmpty())
            value = "\\0";
    }

    return value;
}

QStringList KConfigGroupPrivate::deserializeList(const QString &data)
{
    if (data.isEmpty())
        return QStringList();
    if (data == QLatin1String("\\0"))
        return QStringList(QString());
    QStringList value;
    QString val;
    val.reserve(data.size());
    bool quoted = false;
    for (int p = 0; p < data.length(); p++) {
        if (quoted) {
            val += data[p];
            quoted = false;
        } else if (data[p].unicode() == '\\') {
            quoted = true;
        } else if (data[p].unicode() == ',') {
            val.squeeze(); // release any unused memory
            value.append(val);
            val.clear();
            val.reserve(data.size() - p);
        } else {
            val += data[p];
        }
    }
    value.append(val);
    return value;
}

static QList<int> asIntList(const QByteArray& string)
{
    QList<int> list;
    Q_FOREACH(const QByteArray& s, string.split(','))
        list << s.toInt();
    return list;
}

static QList<qreal> asRealList(const QByteArray& string)
{
    QList<qreal> list;
    Q_FOREACH(const QByteArray& s, string.split(','))
        list << s.toDouble();
    return list;
}

static QString errString( const char * pKey, const QByteArray & value, const QVariant & aDefault ) {
    return QString::fromLatin1("\"%1\" - conversion of \"%3\" to %2 failed")
        .arg(QString::fromLatin1(pKey))
        .arg(QString::fromLatin1(QVariant::typeToName(aDefault.type())))
        .arg(QString::fromLatin1(value));
}

static QString formatError( int expected, int got ) {
    return QString::fromLatin1(" (wrong format: expected %1 items, got %2)").arg( expected ).arg( got );
}

QVariant KConfigGroup::convertToQVariant(const char *pKey, const QByteArray& value, const QVariant& aDefault)
{
    // if a type handler is added here you must add a QVConversions definition
    // to conversion_check.h, or ConversionCheck::to_QVariant will not allow
    // readEntry<T> to convert to QVariant.
    switch( aDefault.type() ) {
        case QVariant::Invalid:
            return QVariant();
        case QVariant::String:
            // this should return the raw string not the dollar expanded string.
            // imho if processed string is wanted should call
            // readEntry(key, QString) not readEntry(key, QVariant)
            return QString::fromUtf8(value);
        case QVariant::List:
        case QVariant::StringList:
            return KConfigGroupPrivate::deserializeList(QString::fromUtf8(value));
        case QVariant::ByteArray:
            return value;
        case QVariant::Bool: {
            const QByteArray lower(value.toLower());
            if (lower == "false" || lower == "no" || lower == "off" || lower == "0")
                return false;
            return true;
        }
        case QVariant::Double:
        case QMetaType::Float:
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::LongLong:
        case QVariant::ULongLong: {
            QVariant tmp = value;
            if ( !tmp.convert(aDefault.type()) )
                tmp = aDefault;
            return tmp;
        }
        case QVariant::Point: {
            const QList<int> list = asIntList(value);

            if ( list.count() != 2 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 2, list.count() );
                return aDefault;
            }
            return QPoint(list.at( 0 ), list.at( 1 ));
        }
        case QVariant::PointF: {
            const QList<qreal> list = asRealList(value);

            if ( list.count() != 2 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 2, list.count() );
                return aDefault;
            }
            return QPointF(list.at( 0 ), list.at( 1 ));
        }
        case QVariant::Rect: {
            const QList<int> list = asIntList(value);

            if ( list.count() != 4 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 4, list.count() );
                return aDefault;
            }
            const QRect rect(list.at( 0 ), list.at( 1 ), list.at( 2 ), list.at( 3 ));
            if ( !rect.isValid() ) {
                qWarning() << errString( pKey, value, aDefault );
                return aDefault;
            }
            return rect;
        }
        case QVariant::RectF: {
            const QList<qreal> list = asRealList(value);

            if ( list.count() != 4 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 4, list.count() );
                return aDefault;
            }
            const QRectF rect(list.at( 0 ), list.at( 1 ), list.at( 2 ), list.at( 3 ));
            if ( !rect.isValid() ) {
                qWarning() << errString( pKey, value, aDefault );
                return aDefault;
            }
            return rect;
        }
        case QVariant::Size: {
            const QList<int> list = asIntList(value);

            if ( list.count() != 2 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 2, list.count() );
                return aDefault;
            }
            const QSize size(list.at( 0 ), list.at( 1 ));
            if ( !size.isValid() ) {
                qWarning() << errString( pKey, value, aDefault );
                return aDefault;
            }
            return size;
        }
        case QVariant::SizeF: {
            const QList<qreal> list = asRealList(value);

            if ( list.count() != 2 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 2, list.count() );
                return aDefault;
            }
            const QSizeF size(list.at( 0 ), list.at( 1 ));
            if ( !size.isValid() ) {
                qWarning() << errString( pKey, value, aDefault );
                return aDefault;
            }
            return size;
        }
        case QVariant::DateTime: {
            const QList<int> list = asIntList(value);
            if ( list.count() != 6 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 6, list.count() );
                return aDefault;
            }
            const QDate date( list.at( 0 ), list.at( 1 ), list.at( 2 ) );
            const QTime time( list.at( 3 ), list.at( 4 ), list.at( 5 ) );
            const QDateTime dt( date, time );
            if ( !dt.isValid() ) {
                qWarning() << errString( pKey, value, aDefault );
                return aDefault;
            }
            return dt;
        }
        case QVariant::Date: {
            QList<int> list = asIntList(value);
            if ( list.count() == 6 )
                list = list.mid(0, 3); // don't break config files that stored QDate as QDateTime
            if ( list.count() != 3 ) {
                qWarning() << errString( pKey, value, aDefault )
                         << formatError( 3, list.count() );
                return aDefault;
            }
            const QDate date( list.at( 0 ), list.at( 1 ), list.at( 2 ) );
            if ( !date.isValid() ) {
                qWarning() << errString( pKey, value, aDefault );
                return aDefault;
            }
            return date;
        }
        case QVariant::Color:
        case QVariant::Font:
            qWarning() << "KConfigGroup::readEntry was passed GUI type '"
                    << aDefault.typeName()
                    << "' but kdeui isn't linked! If it is linked to your program, "
                    "this is a platform bug. Please inform the KDE developers";
            break;
        case QVariant::Url:
            return QUrl(QString::fromUtf8(value));

        default:
            break;
    }

    qWarning() << "unhandled type " << aDefault.typeName();
    return QVariant();
}

#ifdef Q_OS_WIN
# include <QtCore/QDir>
#endif

static bool cleanHomeDirPath( QString &path, const QString &homeDir )
{
#ifdef Q_OS_WIN //safer
   if (!QDir::convertSeparators(path).startsWith(QDir::convertSeparators(homeDir)))
        return false;
#else
   if (!path.startsWith(homeDir))
        return false;
#endif

   int len = homeDir.length();
   // replace by "$HOME" if possible
   if (len && (path.length() == len || path[len] == QLatin1Char('/'))) {
        path.replace(0, len, QString::fromLatin1("$HOME"));
        return true;
   } else
        return false;
}

static QString translatePath( QString path ) // krazy:exclude=passbyvalue
{
   if (path.isEmpty())
       return path;

   // only "our" $HOME should be interpreted
   path.replace(QLatin1Char('$'), QLatin1String("$$"));

   bool startsWithFile = path.startsWith(QLatin1String("file:"), Qt::CaseInsensitive);

   // return original path, if it refers to another type of URL (e.g. http:/), or
   // if the path is already relative to another directory
   if ((!startsWithFile && QFileInfo(path).isRelative()) ||
       (startsWithFile && QFileInfo(path.mid(5)).isRelative()))
       return path;

   if (startsWithFile)
       path.remove(0,5); // strip leading "file:/" off the string

   // keep only one single '/' at the beginning - needed for cleanHomeDirPath()
   while (path[0] == QLatin1Char('/') && path[1] == QLatin1Char('/'))
       path.remove(0,1);

   // we can not use KGlobal::dirs()->relativeLocation("home", path) here,
   // since it would not recognize paths without a trailing '/'.
   // All of the 3 following functions to return the user's home directory
   // can return different paths. We have to test all them.
   const QString homeDir0 = QFile::decodeName(qgetenv("HOME"));
   const QString homeDir1 = QDir::homePath();
   const QString homeDir2 = QDir(homeDir1).canonicalPath();
   if (cleanHomeDirPath(path, homeDir0) ||
       cleanHomeDirPath(path, homeDir1) ||
       cleanHomeDirPath(path, homeDir2) ) {
     // qDebug() << "Path was replaced\n";
   }

   if (startsWithFile)
      path.prepend(QString::fromLatin1("file://"));

   return path;
}


KConfigGroup::KConfigGroup() : d(0)
{
}

bool KConfigGroup::isValid() const
{
    return 0 != d.constData();
}

KConfigGroupGui _kde_internal_KConfigGroupGui;
static inline bool readEntryGui(const QByteArray& data, const char* key, const QVariant &input,
                                QVariant &output)
{
  if (_kde_internal_KConfigGroupGui.readEntryGui)
    return _kde_internal_KConfigGroupGui.readEntryGui(data, key, input, output);
  return false;
}

static inline bool writeEntryGui(KConfigGroup *cg, const char* key, const QVariant &input,
                                 KConfigGroup::WriteConfigFlags flags)
{
  if (_kde_internal_KConfigGroupGui.writeEntryGui)
    return _kde_internal_KConfigGroupGui.writeEntryGui(cg, key, input, flags);
  return false;
}

KConfigGroup::KConfigGroup(KConfigBase *master, const QString &_group)
    : d(KConfigGroupPrivate::create(master, _group.toUtf8(), master->isGroupImmutable(_group), false))
{
}

KConfigGroup::KConfigGroup(KConfigBase *master, const char *_group)
 : d(KConfigGroupPrivate::create(master, _group, master->isGroupImmutable(_group), false))
{
}

KConfigGroup::KConfigGroup(const KConfigBase *master, const QString &_group)
    : d(KConfigGroupPrivate::create(const_cast<KConfigBase*>(master), _group.toUtf8(), master->isGroupImmutable(_group), true))
{
}

KConfigGroup::KConfigGroup(const KConfigBase *master, const char * _group)
    : d(KConfigGroupPrivate::create(const_cast<KConfigBase*>(master), _group, master->isGroupImmutable(_group), true))
{
}

KConfigGroup::KConfigGroup(const KSharedConfigPtr &master, const QString &_group)
    : d(new KConfigGroupPrivate(master, _group.toUtf8()))
{
}

KConfigGroup::KConfigGroup(const KSharedConfigPtr &master, const char * _group)
    : d(new KConfigGroupPrivate(master, _group))
{
}

KConfigGroup &KConfigGroup::operator=(const KConfigGroup &rhs)
{
    d = rhs.d;
    return *this;
}

KConfigGroup::KConfigGroup(const KConfigGroup &rhs)
    : KConfigBase(), d(rhs.d)
{
}

KConfigGroup::~KConfigGroup()
{
    d = 0;
}

KConfigGroup KConfigGroup::groupImpl(const QByteArray& aGroup)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::groupImpl", "accessing an invalid group");
    Q_ASSERT_X(!aGroup.isEmpty(), "KConfigGroup::groupImpl", "can not have an unnamed child group");

    KConfigGroup newGroup;

    newGroup.d = new KConfigGroupPrivate(this, isGroupImmutableImpl(aGroup), d->bConst, aGroup);

    return newGroup;
}

const KConfigGroup KConfigGroup::groupImpl(const QByteArray& aGroup) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::groupImpl", "accessing an invalid group");
    Q_ASSERT_X(!aGroup.isEmpty(), "KConfigGroup::groupImpl", "can not have an unnamed child group");

    KConfigGroup newGroup;

    newGroup.d = new KConfigGroupPrivate(const_cast<KConfigGroup*>(this), isGroupImmutableImpl(aGroup),
                                         true, aGroup);

    return newGroup;
}

KConfigGroup KConfigGroup::parent() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::parent", "accessing an invalid group");

    KConfigGroup parentGroup;

    if (d->mParent) {
        parentGroup.d = d->mParent;
    } else {
        parentGroup.d = new KConfigGroupPrivate(d->mOwner, d->mOwner->isImmutable(), d->bConst, "");
        // make sure we keep the refcount up on the KConfig object
        parentGroup.d->sOwner = d->sOwner;
    }

    return parentGroup;
}

void KConfigGroup::deleteGroup(WriteConfigFlags flags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::deleteGroup", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::deleteGroup", "deleting a read-only group");

    config()->deleteGroup(d->fullName(), flags);
}

#ifndef KDE_NO_DEPRECATED
void KConfigGroup::changeGroup( const QString &group )
{
    Q_ASSERT_X(isValid(), "KConfigGroup::changeGroup", "accessing an invalid group");
    d.detach();
    d->mName = group.toUtf8();
}
#endif

#ifndef KDE_NO_DEPRECATED
void KConfigGroup::changeGroup( const char *group )
{
    Q_ASSERT_X(isValid(), "KConfigGroup::changeGroup", "accessing an invalid group");
    d.detach();
    d->mName = group;
}
#endif

QString KConfigGroup::name() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::name", "accessing an invalid group");

    return QString::fromUtf8(d->name());
}

bool KConfigGroup::exists() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::exists", "accessing an invalid group");

    return config()->hasGroup( d->fullName() );
}

bool KConfigGroup::sync()
{
    Q_ASSERT_X(isValid(), "KConfigGroup::sync", "accessing an invalid group");

    if (!d->bConst)
        return config()->sync();

    return false;
}

QMap<QString, QString> KConfigGroup::entryMap() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::entryMap", "accessing an invalid group");

    return config()->entryMap(QString::fromUtf8(d->fullName()));
}

KConfig* KConfigGroup::config()
{
    Q_ASSERT_X(isValid(), "KConfigGroup::config", "accessing an invalid group");

    return d->mOwner;
}

const KConfig* KConfigGroup::config() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::config", "accessing an invalid group");

    return d->mOwner;
}

bool KConfigGroup::isEntryImmutable(const char* key) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::isEntryImmutable", "accessing an invalid group");

    return (isImmutable() ||
        !config()->d_func()->canWriteEntry(d->fullName(), key, config()->readDefaults()));
}

bool KConfigGroup::isEntryImmutable(const QString& key) const
{
    return isEntryImmutable(key.toUtf8().constData());
}

QString KConfigGroup::readEntryUntranslated(const QString& pKey, const QString& aDefault) const
{
    return readEntryUntranslated(pKey.toUtf8().constData(), aDefault);
}

QString KConfigGroup::readEntryUntranslated(const char *key, const QString& aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readEntryUntranslated", "accessing an invalid group");

    QString result = config()->d_func()->lookupData(d->fullName(), key, KEntryMap::SearchFlags(), 0);
    if (result.isNull())
        return aDefault;
    return result;
}

QString KConfigGroup::readEntry(const char *key, const char* aDefault) const
{
    return readEntry(key, QString::fromUtf8(aDefault));
}

QString KConfigGroup::readEntry(const QString &key, const char* aDefault) const
{
    return readEntry(key.toUtf8().constData(), aDefault);
}

QString KConfigGroup::readEntry(const char* key, const QString& aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readEntry", "accessing an invalid group");

    bool expand = false;

    // read value from the entry map
    QString aValue = config()->d_func()->lookupData(d->fullName(), key, KEntryMap::SearchLocalized,
                                           &expand);
    if (aValue.isNull())
        aValue = aDefault;

    if (expand)
        return KConfigPrivate::expandString(aValue);

    return aValue;
}

QString KConfigGroup::readEntry(const QString &key, const QString& aDefault) const
{
    return readEntry(key.toUtf8().constData(), aDefault);
}

QStringList KConfigGroup::readEntry(const char* key, const QStringList& aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readEntry", "accessing an invalid group");

    const QString data = readEntry(key, QString());
    if (data.isNull())
        return aDefault;

    return KConfigGroupPrivate::deserializeList(data);
}

QStringList KConfigGroup::readEntry( const QString& key, const QStringList& aDefault) const
{
    return readEntry( key.toUtf8().constData(), aDefault );
}

QVariant KConfigGroup::readEntry( const char* key, const QVariant &aDefault ) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readEntry", "accessing an invalid group");

    const QByteArray data = config()->d_func()->lookupData(d->fullName(), key, KEntryMap::SearchLocalized);
    if (data.isNull())
        return aDefault;

    QVariant value;
    if (!readEntryGui( data, key, aDefault, value ))
        return convertToQVariant(key, data, aDefault);

    return value;
}

QVariant KConfigGroup::readEntry( const QString& key, const QVariant& aDefault) const
{
    return readEntry( key.toUtf8().constData(), aDefault );
}

QVariantList KConfigGroup::readEntry( const char* key, const QVariantList& aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readEntry", "accessing an invalid group");

    const QString data = readEntry(key, QString());
    if (data.isNull())
        return aDefault;

    QVariantList value;
    Q_FOREACH(const QString& v, KConfigGroupPrivate::deserializeList(data))
        value << v;

    return value;
}

QVariantList KConfigGroup::readEntry( const QString& key, const QVariantList& aDefault) const
{
    return readEntry( key.toUtf8().constData(), aDefault );
}

QStringList KConfigGroup::readXdgListEntry(const QString& key, const QStringList& aDefault) const
{
    return readXdgListEntry(key.toUtf8().constData(), aDefault);
}

QStringList KConfigGroup::readXdgListEntry(const char *key, const QStringList& aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readXdgListEntry", "accessing an invalid group");

    const QString data = readEntry(key, QString());
    if (data.isNull())
        return aDefault;

    QStringList value;
    QString val;
    val.reserve(data.size());
    // XXX List serialization being a separate layer from low-level parsing is
    // probably a bug. No affected entries are defined, though.
    bool quoted = false;
    for (int p = 0; p < data.length(); p++) {
        if (quoted) {
            val += data[p];
            quoted = false;
        } else if (data[p] == QLatin1Char('\\')) {
            quoted = true;
        } else if (data[p] == QLatin1Char(';')) {
            value.append(val);
            val.clear();
            val.reserve(data.size() - p);
        } else {
            val += data[p];
        }
    }
    if (!val.isEmpty()) {
        qWarning() << "List entry" << key << "in" << config()->name() << "is not compliant with XDG standard (missing trailing semicolon).";
        value.append(val);
    }
    return value;
}

QString KConfigGroup::readPathEntry(const QString& pKey, const QString & aDefault) const
{
    return readPathEntry(pKey.toUtf8().constData(), aDefault);
}

QString KConfigGroup::readPathEntry(const char *key, const QString & aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readPathEntry", "accessing an invalid group");

    bool expand = false;

    QString aValue = config()->d_func()->lookupData(d->fullName(), key, KEntryMap::SearchLocalized,
                            &expand);
    if (aValue.isNull())
        aValue = aDefault;

    return KConfigPrivate::expandString(aValue);
}

QStringList KConfigGroup::readPathEntry(const QString& pKey, const QStringList& aDefault) const
{
    return readPathEntry(pKey.toUtf8().constData(), aDefault);
}

QStringList KConfigGroup::readPathEntry(const char *key, const QStringList& aDefault) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::readPathEntry", "accessing an invalid group");

    const QString data = readPathEntry(key, QString());
    if (data.isNull())
        return aDefault;

    return KConfigGroupPrivate::deserializeList(data);
}

void KConfigGroup::writeEntry( const char* key, const QString& value, WriteConfigFlags flags )
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeEntry", "writing to a read-only group");

    writeEntry(key, value.toUtf8(), flags);
}

void KConfigGroup::writeEntry( const QString& key, const QString& value, WriteConfigFlags flags )
{
    writeEntry(key.toUtf8().constData(), value, flags);
}

void KConfigGroup::writeEntry(const QString &key, const char *value, WriteConfigFlags pFlags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeEntry", "writing to a read-only group");

    writeEntry(key.toUtf8().constData(), QVariant(QString::fromLatin1(value)), pFlags);
}

void KConfigGroup::writeEntry(const char *key, const char *value, WriteConfigFlags pFlags)
{
    writeEntry(key, QVariant(QString::fromLatin1(value)), pFlags);
}

void KConfigGroup::writeEntry( const char* key, const QByteArray& value,
                               WriteConfigFlags flags )
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeEntry", "writing to a read-only group");

    config()->d_func()->putData(d->fullName(), key, value.isNull()? QByteArray(""): value, flags);
}

void KConfigGroup::writeEntry(const QString& key, const QByteArray& value,
                              WriteConfigFlags pFlags)
{
    writeEntry(key.toUtf8().constData(), value, pFlags);
}

void KConfigGroup::writeEntry(const char* key, const QStringList &list, WriteConfigFlags flags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeEntry", "writing to a read-only group");

    QList<QByteArray> balist;

    Q_FOREACH(const QString &entry, list)
        balist.append(entry.toUtf8());

    writeEntry(key, KConfigGroupPrivate::serializeList(balist), flags);
}

void KConfigGroup::writeEntry(const QString& key, const QStringList &list, WriteConfigFlags flags)
{
    writeEntry(key.toUtf8().constData(), list, flags);
}

void KConfigGroup::writeEntry( const char* key, const QVariantList& list, WriteConfigFlags flags )
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeEntry", "writing to a read-only group");

    QList<QByteArray> data;

    Q_FOREACH(const QVariant& v, list) {
        if (v.type() == QVariant::ByteArray)
            data << v.toByteArray();
        else
            data << v.toString().toUtf8();
    }

    writeEntry(key, KConfigGroupPrivate::serializeList(data), flags);
}

void KConfigGroup::writeEntry( const char* key, const QVariant &value,
                               WriteConfigFlags flags )
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeEntry", "writing to a read-only group");

    if ( writeEntryGui( this, key, value, flags ) )
        return;                     // GUI type that was handled

    QByteArray data;
    // if a type handler is added here you must add a QVConversions definition
    // to conversion_check.h, or ConversionCheck::to_QVariant will not allow
    // writeEntry<T> to convert to QVariant.
    switch( value.type() ) {
        case QVariant::Invalid:
            data = "";
            break;
        case QVariant::ByteArray:
            data = value.toByteArray();
            break;
        case QVariant::String:
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::Double:
        case QMetaType::Float:
        case QVariant::Bool:
        case QVariant::LongLong:
        case QVariant::ULongLong:
            data = value.toString().toUtf8();
            break;
        case QVariant::List:
            if (!value.canConvert(QVariant::StringList))
                qWarning() << "not all types in \"" << key << "\" can convert to QString,"
                   " information will be lost";
        case QVariant::StringList:
            writeEntry( key, value.toList(), flags );
            return;
        case QVariant::Point: {
            QVariantList list;
            const QPoint rPoint = value.toPoint();
            list.insert( 0, rPoint.x() );
            list.insert( 1, rPoint.y() );

            writeEntry( key, list, flags );
            return;
        }
        case QVariant::PointF: {
            QVariantList list;
            const QPointF point = value.toPointF();
            list.insert( 0, point.x() );
            list.insert( 1, point.y() );

            writeEntry( key, list, flags );
            return;
        }
        case QVariant::Rect:{
            QVariantList list;
            const QRect rRect = value.toRect();
            list.insert( 0, rRect.left() );
            list.insert( 1, rRect.top() );
            list.insert( 2, rRect.width() );
            list.insert( 3, rRect.height() );

            writeEntry( key, list, flags );
            return;
        }
        case QVariant::RectF:{
            QVariantList list;
            const QRectF rRectF = value.toRectF();
            list.insert(0, rRectF.left());
            list.insert(1, rRectF.top());
            list.insert(2, rRectF.width());
            list.insert(3, rRectF.height());

            writeEntry(key, list, flags);
            return;
        }
        case QVariant::Size:{
            QVariantList list;
            const QSize rSize = value.toSize();
            list.insert( 0, rSize.width() );
            list.insert( 1, rSize.height() );

            writeEntry( key, list, flags );
            return;
        }
        case QVariant::SizeF:{
            QVariantList list;
            const QSizeF rSizeF = value.toSizeF();
            list.insert(0, rSizeF.width());
            list.insert(1, rSizeF.height());

            writeEntry(key, list, flags);
            return;
        }
        case QVariant::Date: {
            QVariantList list;
            const QDate date = value.toDate();

            list.insert( 0, date.year() );
            list.insert( 1, date.month() );
            list.insert( 2, date.day() );

            writeEntry( key, list, flags );
            return;
        }
        case QVariant::DateTime: {
            QVariantList list;
            const QDateTime rDateTime = value.toDateTime();

            const QTime time = rDateTime.time();
            const QDate date = rDateTime.date();

            list.insert( 0, date.year() );
            list.insert( 1, date.month() );
            list.insert( 2, date.day() );

            list.insert( 3, time.hour() );
            list.insert( 4, time.minute() );
            list.insert( 5, time.second() );

            writeEntry( key, list, flags );
            return;
        }

        case QVariant::Color:
        case QVariant::Font:
            qWarning() << "KConfigGroup::writeEntry was passed GUI type '"
                     << value.typeName()
                     << "' but kdeui isn't linked! If it is linked to your program, this is a platform bug. "
                        "Please inform the KDE developers";
            break;
        case QVariant::Url:
            data = QUrl(value.toUrl()).toString().toUtf8();
            break;
        default:
            qWarning() << "KConfigGroup::writeEntry - unhandled type" << value.typeName() << "in group" << name();
        }

    writeEntry(key, data, flags);
}

void KConfigGroup::writeEntry( const QString& key, const QVariant& value, WriteConfigFlags flags )
{
    writeEntry(key.toUtf8().constData(), value, flags);
}

void KConfigGroup::writeEntry(const QString& key, const QVariantList &list, WriteConfigFlags flags)
{
    writeEntry(key.toUtf8().constData(), list, flags);
}

void KConfigGroup::writeXdgListEntry(const QString& key, const QStringList &value, WriteConfigFlags pFlags)
{
    writeXdgListEntry(key.toUtf8().constData(), value, pFlags);
}

void KConfigGroup::writeXdgListEntry(const char *key, const QStringList &list, WriteConfigFlags flags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writeXdgListEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writeXdgListEntry", "writing to a read-only group");

    QString value;
    value.reserve(4096);

    // XXX List serialization being a separate layer from low-level escaping is
    // probably a bug. No affected entries are defined, though.
    QStringList::ConstIterator it = list.constBegin();
    const QStringList::ConstIterator end = list.constEnd();
    for (; it != end; ++it) {
        QString val(*it);
        val.replace(QLatin1Char('\\'), QLatin1String("\\\\")).replace(QLatin1Char(';'), QLatin1String("\\;"));
        value += val;
        value += QLatin1Char(';');
    }

    writeEntry(key, value, flags);
}

void KConfigGroup::writePathEntry(const QString& pKey, const QString & path, WriteConfigFlags pFlags)
{
    writePathEntry(pKey.toUtf8().constData(), path, pFlags);
}

void KConfigGroup::writePathEntry(const char *pKey, const QString & path, WriteConfigFlags pFlags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writePathEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writePathEntry", "writing to a read-only group");

    config()->d_func()->putData(d->fullName(), pKey, translatePath(path).toUtf8(), pFlags, true);
}

void KConfigGroup::writePathEntry(const QString& pKey, const QStringList &value, WriteConfigFlags pFlags)
{
    writePathEntry(pKey.toUtf8().constData(), value, pFlags);
}

void KConfigGroup::writePathEntry(const char *pKey, const QStringList &value, WriteConfigFlags pFlags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::writePathEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::writePathEntry", "writing to a read-only group");

    QList<QByteArray> list;
    Q_FOREACH(const QString& path, value)
        list << translatePath(path).toUtf8();

    config()->d_func()->putData(d->fullName(), pKey, KConfigGroupPrivate::serializeList(list), pFlags, true);
}

void KConfigGroup::deleteEntry( const char *key, WriteConfigFlags flags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::deleteEntry", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::deleteEntry", "deleting from a read-only group");

    config()->d_func()->putData(d->fullName(), key, QByteArray(), flags);
}

void KConfigGroup::deleteEntry( const QString& key, WriteConfigFlags flags)
{
    deleteEntry(key.toUtf8().constData(), flags);
}

void KConfigGroup::revertToDefault(const char *key)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::revertToDefault", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::revertToDefault", "writing to a read-only group");

    config()->d_func()->revertEntry(d->fullName(), key);
}

void KConfigGroup::revertToDefault(const QString &key)
{
    revertToDefault(key.toUtf8().constData());
}

bool KConfigGroup::hasDefault(const char *key) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::hasDefault", "accessing an invalid group");

    KEntryMap::SearchFlags flags = KEntryMap::SearchDefaults|KEntryMap::SearchLocalized;

    return !config()->d_func()->lookupData(d->fullName(), key, flags).isNull();
}

bool KConfigGroup::hasDefault(const QString &key) const
{
    return hasDefault(key.toUtf8().constData());
}

bool KConfigGroup::hasKey(const char *key) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::hasKey", "accessing an invalid group");

    KEntryMap::SearchFlags flags = KEntryMap::SearchLocalized;
    if ( config()->readDefaults() )
        flags |= KEntryMap::SearchDefaults;

    return !config()->d_func()->lookupData(d->fullName(), key, flags).isNull();
}

bool KConfigGroup::hasKey(const QString &key) const
{
   return hasKey(key.toUtf8().constData());
}

bool KConfigGroup::isImmutable() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::isImmutable", "accessing an invalid group");

    return d->bImmutable;
}

QStringList KConfigGroup::groupList() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::groupList", "accessing an invalid group");

    return config()->d_func()->groupList(d->fullName());
}

QStringList KConfigGroup::keyList() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::keyList", "accessing an invalid group");

    return entryMap().keys();
}

void KConfigGroup::markAsClean()
{
    Q_ASSERT_X(isValid(), "KConfigGroup::markAsClean", "accessing an invalid group");

    config()->markAsClean();
}

KConfigGroup::AccessMode KConfigGroup::accessMode() const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::accessMode", "accessing an invalid group");

    return config()->accessMode();
}

bool KConfigGroup::hasGroupImpl(const QByteArray & b) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::hasGroupImpl", "accessing an invalid group");

    return config()->hasGroup(d->fullName(b));
}

void KConfigGroup::deleteGroupImpl(const QByteArray &b, WriteConfigFlags flags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::deleteGroupImpl", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst,"KConfigGroup::deleteGroupImpl", "deleting from a read-only group");

    config()->deleteGroup(d->fullName(b), flags);
}

bool KConfigGroup::isGroupImmutableImpl(const QByteArray& b) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::isGroupImmutableImpl", "accessing an invalid group");

    if (!hasGroupImpl(b)) // group doesn't exist yet
        return d->bImmutable; // child groups are immutable if the parent is immutable.

    return config()->isGroupImmutable(d->fullName(b));
}

void KConfigGroup::copyTo(KConfigBase* other, WriteConfigFlags pFlags) const
{
    Q_ASSERT_X(isValid(), "KConfigGroup::copyTo", "accessing an invalid group");
    Q_ASSERT(other != 0);

    if (KConfigGroup *otherGroup = dynamic_cast<KConfigGroup*>(other)) {
        config()->d_func()->copyGroup(d->fullName(), otherGroup->d->fullName(), otherGroup, pFlags);
    } else if (KConfig* otherConfig = dynamic_cast<KConfig*>(other)) {
        KConfigGroup newGroup = otherConfig->group(d->fullName());
        otherConfig->d_func()->copyGroup(d->fullName(), d->fullName(), &newGroup, pFlags);
    } else {
        Q_ASSERT_X(false, "KConfigGroup::copyTo", "unknown type of KConfigBase");
    }
}

void KConfigGroup::reparent(KConfigBase* parent, WriteConfigFlags pFlags)
{
    Q_ASSERT_X(isValid(), "KConfigGroup::reparent", "accessing an invalid group");
    Q_ASSERT_X(!d->bConst, "KConfigGroup::reparent", "reparenting a read-only group");
    Q_ASSERT_X(!d->bImmutable, "KConfigGroup::reparent", "reparenting an immutable group");
    Q_ASSERT(parent != 0);

    KConfigGroup oldGroup(*this);

    d = KConfigGroupPrivate::create(parent, d->mName, false, false);
    oldGroup.copyTo(this, pFlags);
    oldGroup.deleteGroup(); // so that the entries with the old group name are deleted on sync
}
