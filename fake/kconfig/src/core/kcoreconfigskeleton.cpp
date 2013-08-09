/*
    This file is part of KOrganizer.
    Copyright (c) 2000,2001 Cornelius Schumacher <schumacher@kde.org>
    Copyright (c) 2003 Waldo Bastian <bastian@kde.org>

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

#include "kcoreconfigskeleton.h"
#include "kcoreconfigskeleton_p.h"

#include <QUrl>

Q_DECLARE_METATYPE(QList<QUrl>)

static QString obscuredString(const QString &str)
{
    QString result;
    const QChar *unicode = str.unicode();
    for ( int i = 0; i < str.length(); ++i )
        // yes, no typo. can't encode ' ' or '!' because
        // they're the unicode BOM. stupid scrambling. stupid.
        result += ( unicode[ i ].unicode() <= 0x21 ) ? unicode[ i ]
                                                     : QChar( 0x1001F - unicode[ i ].unicode() );

  return result;
}

KConfigSkeletonItem::KConfigSkeletonItem(const QString & _group,
                                         const QString & _key)
    : mGroup(_group)
    , mKey(_key)
    , d( new KConfigSkeletonItemPrivate )
{
}

KConfigSkeletonItem::~KConfigSkeletonItem()
{
    delete d;
}

void KConfigSkeletonItem::setGroup( const QString &_group )
{
    mGroup = _group;
}

QString KConfigSkeletonItem::group() const
{
    return mGroup;
}

void KConfigSkeletonItem::setKey( const QString &_key )
{
    mKey = _key;
}

QString KConfigSkeletonItem::key() const
{
    return mKey;
}

void KConfigSkeletonItem::setName(const QString &_name)
{
    mName = _name;
}

QString KConfigSkeletonItem::name() const
{
    return mName;
}

void KConfigSkeletonItem::setLabel( const QString &l )
{
    d->mLabel = l;
}

QString KConfigSkeletonItem::label() const
{
    return d->mLabel;
}

void KConfigSkeletonItem::setToolTip( const QString &t )
{
    d->mToolTip = t;
}

QString KConfigSkeletonItem::toolTip() const
{
    return d->mToolTip;
}

void KConfigSkeletonItem::setWhatsThis( const QString &w )
{
    d->mWhatsThis = w;
}

QString KConfigSkeletonItem::whatsThis() const
{
    return d->mWhatsThis;
}

QVariant KConfigSkeletonItem::minValue() const
{
    return QVariant();
}

QVariant KConfigSkeletonItem::maxValue() const
{
    return QVariant();
}

bool KConfigSkeletonItem::isImmutable() const
{
    return d->mIsImmutable;
}

void KConfigSkeletonItem::readImmutability( const KConfigGroup &group )
{
  d->mIsImmutable = group.isEntryImmutable( mKey );
}


KCoreConfigSkeleton::ItemString::ItemString( const QString &_group, const QString &_key,
                                    QString &reference,
                                    const QString &defaultValue,
                                    Type type )
  : KConfigSkeletonGenericItem<QString>( _group, _key, reference, defaultValue ),
    mType( type )
{
}

void KCoreConfigSkeleton::ItemString::writeConfig( KConfig *config )
{
  if ( mReference != mLoadedValue ) // WABA: Is this test needed?
  {
    KConfigGroup cg(config, mGroup );
    if ((mDefault == mReference) && !cg.hasDefault( mKey))
      cg.revertToDefault( mKey );
    else if ( mType == Path )
      cg.writePathEntry( mKey, mReference );
    else if ( mType == Password )
      cg.writeEntry( mKey, obscuredString( mReference ) );
    else
      cg.writeEntry( mKey, mReference );
  }
}


void KCoreConfigSkeleton::ItemString::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );

  if ( mType == Path )
  {
    mReference = cg.readPathEntry( mKey, mDefault );
  }
  else if ( mType == Password )
  {
    QString val = cg.readEntry( mKey, obscuredString( mDefault ) );
    mReference = obscuredString( val );
  }
  else
  {
    mReference = cg.readEntry( mKey, mDefault );
  }

  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemString::setProperty(const QVariant & p)
{
  mReference = p.toString();
}

bool KCoreConfigSkeleton::ItemString::isEqual(const QVariant &v) const
{
    return mReference == v.toString();
}

QVariant KCoreConfigSkeleton::ItemString::property() const
{
  return QVariant(mReference);
}

KCoreConfigSkeleton::ItemPassword::ItemPassword( const QString &_group, const QString &_key,
                                    QString &reference,
                                    const QString &defaultValue)
  : ItemString( _group, _key, reference, defaultValue, Password )
{
}

KCoreConfigSkeleton::ItemPath::ItemPath( const QString &_group, const QString &_key,
                                    QString &reference,
                                    const QString &defaultValue)
  : ItemString( _group, _key, reference, defaultValue, Path )
{
}

KCoreConfigSkeleton::ItemUrl::ItemUrl( const QString &_group, const QString &_key,
                                    QUrl &reference,
                                    const QUrl &defaultValue )
  : KConfigSkeletonGenericItem<QUrl>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemUrl::writeConfig( KConfig *config )
{
    if ( mReference != mLoadedValue ) // WABA: Is this test needed?
    {
        KConfigGroup cg(config, mGroup );
        if ((mDefault == mReference) && !cg.hasDefault( mKey))
            cg.revertToDefault( mKey );
        else
            cg.writeEntry<QString>( mKey, mReference.toString() );
    }
}

void KCoreConfigSkeleton::ItemUrl::readConfig( KConfig *config )
{
    KConfigGroup cg(config, mGroup );

    mReference = QUrl( cg.readEntry<QString>( mKey, mDefault.toString() ) );
    mLoadedValue = mReference;

    readImmutability( cg );
}

void KCoreConfigSkeleton::ItemUrl::setProperty(const QVariant & p)
{
    mReference = qvariant_cast<QUrl>(p);
}

bool KCoreConfigSkeleton::ItemUrl::isEqual(const QVariant &v) const
{
    return mReference == qvariant_cast<QUrl>(v);
}

QVariant KCoreConfigSkeleton::ItemUrl::property() const
{
    return qVariantFromValue<QUrl>(mReference);
}

KCoreConfigSkeleton::ItemProperty::ItemProperty( const QString &_group,
                                        const QString &_key,
                                        QVariant &reference,
                                        const QVariant &defaultValue )
  : KConfigSkeletonGenericItem<QVariant>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemProperty::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemProperty::setProperty(const QVariant & p)
{
  mReference = p;
}

bool KCoreConfigSkeleton::ItemProperty::isEqual(const QVariant &v) const
{
    //this might cause problems if the QVariants are not of default types
    return mReference == v;
}

QVariant KCoreConfigSkeleton::ItemProperty::property() const
{
  return mReference;
}

KCoreConfigSkeleton::ItemBool::ItemBool( const QString &_group, const QString &_key,
                                bool &reference, bool defaultValue )
  : KConfigSkeletonGenericItem<bool>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemBool::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemBool::setProperty(const QVariant & p)
{
  mReference = p.toBool();
}

bool KCoreConfigSkeleton::ItemBool::isEqual(const QVariant &v) const
{
    return mReference == v.toBool();
}

QVariant KCoreConfigSkeleton::ItemBool::property() const
{
  return QVariant( mReference );
}


KCoreConfigSkeleton::ItemInt::ItemInt( const QString &_group, const QString &_key,
                              qint32 &reference, qint32 defaultValue )
  : KConfigSkeletonGenericItem<qint32>( _group, _key, reference, defaultValue )
  ,mHasMin(false), mHasMax(false)
{
}

void KCoreConfigSkeleton::ItemInt::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  if (mHasMin)
    mReference = qMax(mReference, mMin);
  if (mHasMax)
    mReference = qMin(mReference, mMax);
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemInt::setProperty(const QVariant & p)
{
  mReference = p.toInt();
}

bool KCoreConfigSkeleton::ItemInt::isEqual(const QVariant &v) const
{
    return mReference == v.toInt();
}

QVariant KCoreConfigSkeleton::ItemInt::property() const
{
  return QVariant(mReference);
}

QVariant KCoreConfigSkeleton::ItemInt::minValue() const
{
  if (mHasMin)
    return QVariant(mMin);
  return QVariant();
}

QVariant KCoreConfigSkeleton::ItemInt::maxValue() const
{
  if (mHasMax)
    return QVariant(mMax);
  return QVariant();
}

void KCoreConfigSkeleton::ItemInt::setMinValue(qint32 v)
{
  mHasMin = true;
  mMin = v;
}

void KCoreConfigSkeleton::ItemInt::setMaxValue(qint32 v)
{
  mHasMax = true;
  mMax = v;
}


KCoreConfigSkeleton::ItemLongLong::ItemLongLong( const QString &_group, const QString &_key,
                              qint64 &reference, qint64 defaultValue )
  : KConfigSkeletonGenericItem<qint64>( _group, _key, reference, defaultValue )
  ,mHasMin(false), mHasMax(false)
{
}

void KCoreConfigSkeleton::ItemLongLong::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  if (mHasMin)
    mReference = qMax(mReference, mMin);
  if (mHasMax)
    mReference = qMin(mReference, mMax);
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemLongLong::setProperty(const QVariant & p)
{
  mReference = p.toLongLong();
}

bool KCoreConfigSkeleton::ItemLongLong::isEqual(const QVariant &v) const
{
    return mReference == v.toLongLong();
}

QVariant KCoreConfigSkeleton::ItemLongLong::property() const
{
  return QVariant(mReference);
}

QVariant KCoreConfigSkeleton::ItemLongLong::minValue() const
{
  if (mHasMin)
    return QVariant(mMin);
  return QVariant();
}

QVariant KCoreConfigSkeleton::ItemLongLong::maxValue() const
{
  if (mHasMax)
    return QVariant(mMax);
  return QVariant();
}

void KCoreConfigSkeleton::ItemLongLong::setMinValue(qint64 v)
{
  mHasMin = true;
  mMin = v;
}

void KCoreConfigSkeleton::ItemLongLong::setMaxValue(qint64 v)
{
  mHasMax = true;
  mMax = v;
}

KCoreConfigSkeleton::ItemEnum::ItemEnum( const QString &_group, const QString &_key,
                                     qint32 &reference,
                                     const QList<Choice> &choices,
                                     qint32 defaultValue )
  : ItemInt( _group, _key, reference, defaultValue ), mChoices(choices)
{
}

void KCoreConfigSkeleton::ItemEnum::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  if (!cg.hasKey(mKey))
  {
    mReference = mDefault;
  }
  else
  {
    int i = 0;
    mReference = -1;
    QString tmp = cg.readEntry( mKey, QString() ).toLower();
    for(QList<Choice>::ConstIterator it = mChoices.constBegin();
        it != mChoices.constEnd(); ++it, ++i)
    {
      if ((*it).name.toLower() == tmp)
      {
        mReference = i;
        break;
      }
    }
    if (mReference == -1)
       mReference = cg.readEntry( mKey, mDefault );
  }
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemEnum::writeConfig( KConfig *config )
{
  if ( mReference != mLoadedValue ) // WABA: Is this test needed?
  {
    KConfigGroup cg(config, mGroup );
    if ((mDefault == mReference) && !cg.hasDefault( mKey))
      cg.revertToDefault( mKey );
    else if ((mReference >= 0) && (mReference < (int) mChoices.count()))
      cg.writeEntry( mKey, mChoices[mReference].name );
    else
      cg.writeEntry( mKey, mReference );
  }
}

QList<KCoreConfigSkeleton::ItemEnum::Choice> KCoreConfigSkeleton::ItemEnum::choices() const
{
    return mChoices;
}

QList<KCoreConfigSkeleton::ItemEnum::Choice> KCoreConfigSkeleton::ItemEnum::choices2() const
{
    return mChoices;
}

KCoreConfigSkeleton::ItemUInt::ItemUInt( const QString &_group, const QString &_key,
                                quint32 &reference,
                                quint32 defaultValue )
  : KConfigSkeletonGenericItem<quint32>( _group, _key, reference, defaultValue )
  ,mHasMin(false), mHasMax(false)
{
}

void KCoreConfigSkeleton::ItemUInt::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  if (mHasMin)
    mReference = qMax(mReference, mMin);
  if (mHasMax)
    mReference = qMin(mReference, mMax);
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemUInt::setProperty(const QVariant & p)
{
  mReference = p.toUInt();
}

bool KCoreConfigSkeleton::ItemUInt::isEqual(const QVariant &v) const
{
    return mReference == v.toUInt();
}

QVariant KCoreConfigSkeleton::ItemUInt::property() const
{
  return QVariant(mReference);
}

QVariant KCoreConfigSkeleton::ItemUInt::minValue() const
{
  if (mHasMin)
    return QVariant(mMin);
  return QVariant();
}

QVariant KCoreConfigSkeleton::ItemUInt::maxValue() const
{
  if (mHasMax)
    return QVariant(mMax);
  return QVariant();
}

void KCoreConfigSkeleton::ItemUInt::setMinValue(quint32 v)
{
  mHasMin = true;
  mMin = v;
}

void KCoreConfigSkeleton::ItemUInt::setMaxValue(quint32 v)
{
  mHasMax = true;
  mMax = v;
}


KCoreConfigSkeleton::ItemULongLong::ItemULongLong( const QString &_group, const QString &_key,
                              quint64 &reference, quint64 defaultValue )
  : KConfigSkeletonGenericItem<quint64>( _group, _key, reference, defaultValue )
  ,mHasMin(false), mHasMax(false)
{
}

void KCoreConfigSkeleton::ItemULongLong::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  if (mHasMin)
    mReference = qMax(mReference, mMin);
  if (mHasMax)
    mReference = qMin(mReference, mMax);
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemULongLong::setProperty(const QVariant & p)
{
  mReference = p.toULongLong();
}

bool KCoreConfigSkeleton::ItemULongLong::isEqual(const QVariant &v) const
{
    return mReference == v.toULongLong();
}

QVariant KCoreConfigSkeleton::ItemULongLong::property() const
{
  return QVariant(mReference);
}

QVariant KCoreConfigSkeleton::ItemULongLong::minValue() const
{
  if (mHasMin)
    return QVariant(mMin);
  return QVariant();
}

QVariant KCoreConfigSkeleton::ItemULongLong::maxValue() const
{
  if (mHasMax)
    return QVariant(mMax);
  return QVariant();
}

void KCoreConfigSkeleton::ItemULongLong::setMinValue(quint64 v)
{
  mHasMin = true;
  mMin = v;
}

void KCoreConfigSkeleton::ItemULongLong::setMaxValue(quint64 v)
{
  mHasMax = true;
  mMax = v;
}

KCoreConfigSkeleton::ItemDouble::ItemDouble( const QString &_group, const QString &_key,
                                    double &reference, double defaultValue )
  : KConfigSkeletonGenericItem<double>( _group, _key, reference, defaultValue )
  ,mHasMin(false), mHasMax(false)
{
}

void KCoreConfigSkeleton::ItemDouble::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  if (mHasMin)
    mReference = qMax(mReference, mMin);
  if (mHasMax)
    mReference = qMin(mReference, mMax);
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemDouble::setProperty(const QVariant & p)
{
  mReference = p.toDouble();
}

bool KCoreConfigSkeleton::ItemDouble::isEqual(const QVariant &v) const
{
    return mReference == v.toDouble();
}

QVariant KCoreConfigSkeleton::ItemDouble::property() const
{
  return QVariant(mReference);
}

QVariant KCoreConfigSkeleton::ItemDouble::minValue() const
{
  if (mHasMin)
    return QVariant(mMin);
  return QVariant();
}

QVariant KCoreConfigSkeleton::ItemDouble::maxValue() const
{
  if (mHasMax)
    return QVariant(mMax);
  return QVariant();
}

void KCoreConfigSkeleton::ItemDouble::setMinValue(double v)
{
  mHasMin = true;
  mMin = v;
}

void KCoreConfigSkeleton::ItemDouble::setMaxValue(double v)
{
  mHasMax = true;
  mMax = v;
}


KCoreConfigSkeleton::ItemRect::ItemRect( const QString &_group, const QString &_key,
                                QRect &reference,
                                const QRect &defaultValue )
  : KConfigSkeletonGenericItem<QRect>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemRect::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemRect::setProperty(const QVariant & p)
{
  mReference = p.toRect();
}

bool KCoreConfigSkeleton::ItemRect::isEqual(const QVariant &v) const
{
    return mReference == v.toRect();
}

QVariant KCoreConfigSkeleton::ItemRect::property() const
{
  return QVariant(mReference);
}


KCoreConfigSkeleton::ItemPoint::ItemPoint( const QString &_group, const QString &_key,
                                  QPoint &reference,
                                  const QPoint &defaultValue )
  : KConfigSkeletonGenericItem<QPoint>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemPoint::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemPoint::setProperty(const QVariant & p)
{
  mReference = p.toPoint();
}

bool KCoreConfigSkeleton::ItemPoint::isEqual(const QVariant &v) const
{
    return mReference == v.toPoint();
}

QVariant KCoreConfigSkeleton::ItemPoint::property() const
{
  return QVariant(mReference);
}


KCoreConfigSkeleton::ItemSize::ItemSize( const QString &_group, const QString &_key,
                                QSize &reference,
                                const QSize &defaultValue )
  : KConfigSkeletonGenericItem<QSize>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemSize::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemSize::setProperty(const QVariant & p)
{
  mReference = p.toSize();
}

bool KCoreConfigSkeleton::ItemSize::isEqual(const QVariant &v) const
{
    return mReference == v.toSize();
}

QVariant KCoreConfigSkeleton::ItemSize::property() const
{
  return QVariant(mReference);
}


KCoreConfigSkeleton::ItemDateTime::ItemDateTime( const QString &_group, const QString &_key,
                                        QDateTime &reference,
                                        const QDateTime &defaultValue )
  : KConfigSkeletonGenericItem<QDateTime>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemDateTime::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemDateTime::setProperty(const QVariant & p)
{
  mReference = p.toDateTime();
}

bool KCoreConfigSkeleton::ItemDateTime::isEqual(const QVariant &v) const
{
    return mReference == v.toDateTime();
}

QVariant KCoreConfigSkeleton::ItemDateTime::property() const
{
  return QVariant(mReference);
}


KCoreConfigSkeleton::ItemStringList::ItemStringList( const QString &_group, const QString &_key,
                                            QStringList &reference,
                                            const QStringList &defaultValue )
  : KConfigSkeletonGenericItem<QStringList>( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemStringList::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  if ( !cg.hasKey( mKey ) )
    mReference = mDefault;
  else
    mReference = cg.readEntry( mKey, mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemStringList::setProperty(const QVariant & p)
{
  mReference = p.toStringList();
}

bool KCoreConfigSkeleton::ItemStringList::isEqual(const QVariant &v) const
{
    return mReference == v.toStringList();
}

QVariant KCoreConfigSkeleton::ItemStringList::property() const
{
  return QVariant(mReference);
}


KCoreConfigSkeleton::ItemPathList::ItemPathList( const QString &_group, const QString &_key,
                                            QStringList &reference,
                                            const QStringList &defaultValue )
  : ItemStringList( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemPathList::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  if ( !cg.hasKey( mKey ) )
    mReference = mDefault;
  else
    mReference = cg.readPathEntry( mKey, QStringList() );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemPathList::writeConfig( KConfig *config )
{
  if ( mReference != mLoadedValue ) // WABA: Is this test needed?
  {
    KConfigGroup cg(config, mGroup );
    if ((mDefault == mReference) && !cg.hasDefault( mKey))
      cg.revertToDefault( mKey );
    else {
      QStringList sl = mReference;
      cg.writePathEntry( mKey, sl );
    }
  }
}

KCoreConfigSkeleton::ItemUrlList::ItemUrlList( const QString &_group, const QString &_key,
                                            QList<QUrl> &reference,
                                            const QList<QUrl> &defaultValue )
  : KConfigSkeletonGenericItem<QList<QUrl> >( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemUrlList::readConfig( KConfig *config )
{
    KConfigGroup cg(config, mGroup );
    if ( !cg.hasKey( mKey ) )
        mReference = mDefault;
    else {
        QStringList strList;
        Q_FOREACH (const QUrl& url, mDefault) {
            strList.append(url.toString());
        }
        mReference.clear();
        const QStringList readList = cg.readEntry<QStringList>(mKey, strList);
        Q_FOREACH (const QString& str, readList) {
            mReference.append(QUrl(str));
        }
    }
    mLoadedValue = mReference;

    readImmutability( cg );
}

void KCoreConfigSkeleton::ItemUrlList::writeConfig( KConfig *config )
{
    if ( mReference != mLoadedValue ) // WABA: Is this test needed?
    {
        KConfigGroup cg(config, mGroup );
        if ((mDefault == mReference) && !cg.hasDefault( mKey))
            cg.revertToDefault( mKey );
        else {
            QStringList strList;
            Q_FOREACH (const QUrl& url, mReference) {
                strList.append(url.toString());
            }
            cg.writeEntry<QStringList>(mKey, strList);
        }
    }
}

void KCoreConfigSkeleton::ItemUrlList::setProperty(const QVariant & p)
{
    mReference = qvariant_cast<QList<QUrl> >(p);
}

bool KCoreConfigSkeleton::ItemUrlList::isEqual(const QVariant &v) const
{
    return mReference == qvariant_cast<QList<QUrl> >(v);
}

QVariant KCoreConfigSkeleton::ItemUrlList::property() const
{
    return qVariantFromValue<QList<QUrl> >(mReference);
}

Q_DECLARE_METATYPE( QList<int> )

KCoreConfigSkeleton::ItemIntList::ItemIntList( const QString &_group, const QString &_key,
                                      QList<int> &reference,
                                      const QList<int> &defaultValue )
  : KConfigSkeletonGenericItem<QList<int> >( _group, _key, reference, defaultValue )
{
}

void KCoreConfigSkeleton::ItemIntList::readConfig( KConfig *config )
{
  KConfigGroup cg(config, mGroup );
  if ( !cg.hasKey( mKey ) )
    mReference = mDefault;
  else
    mReference = cg.readEntry( mKey , mDefault );
  mLoadedValue = mReference;

  readImmutability( cg );
}

void KCoreConfigSkeleton::ItemIntList::setProperty(const QVariant &p)
{
    mReference = qvariant_cast< QList<int> >(p);
}

bool KCoreConfigSkeleton::ItemIntList::isEqual(const QVariant &v) const
{
    return mReference == qvariant_cast< QList<int> >(v);
}

QVariant KCoreConfigSkeleton::ItemIntList::property() const
{
    return qVariantFromValue< QList<int> >(mReference);
}

//static int kCoreConfigSkeletionDebugArea() { static int s_area = KDebug::registerArea("kdecore (KConfigSkeleton)"); return s_area; }

KCoreConfigSkeleton::KCoreConfigSkeleton(const QString &configname, QObject* parent)
  : QObject(parent),
    d( new Private )
{
    //qDebug() << "Creating KCoreConfigSkeleton (" << (void *)this << ")";

    d->mConfig = KSharedConfig::openConfig( configname );
}

KCoreConfigSkeleton::KCoreConfigSkeleton(KSharedConfig::Ptr pConfig, QObject* parent)
  : QObject(parent),
    d( new Private )
{
    //qDebug() << "Creating KCoreConfigSkeleton (" << (void *)this << ")";
    d->mConfig = pConfig;
}


KCoreConfigSkeleton::~KCoreConfigSkeleton()
{
  delete d;
}

void KCoreConfigSkeleton::setCurrentGroup( const QString &group )
{
  d->mCurrentGroup = group;
}

QString KCoreConfigSkeleton::currentGroup() const
{
    return d->mCurrentGroup;
}

KConfig *KCoreConfigSkeleton::config()
{
  return d->mConfig.data();
}

const KConfig *KCoreConfigSkeleton::config() const
{
  return d->mConfig.data();
}

void KCoreConfigSkeleton::setSharedConfig(KSharedConfig::Ptr pConfig)
{
    d->mConfig = pConfig;
}

KConfigSkeletonItem::List KCoreConfigSkeleton::items() const
{
    return d->mItems;
}

bool KCoreConfigSkeleton::useDefaults(bool b)
{
  if (b == d->mUseDefaults)
    return d->mUseDefaults;

  d->mUseDefaults = b;
  KConfigSkeletonItem::List::ConstIterator it;
  for( it = d->mItems.constBegin(); it != d->mItems.constEnd(); ++it )
  {
    (*it)->swapDefault();
  }
  usrUseDefaults(b);
  return !d->mUseDefaults;
}

void KCoreConfigSkeleton::setDefaults()
{
  KConfigSkeletonItem::List::ConstIterator it;
  for( it = d->mItems.constBegin(); it != d->mItems.constEnd(); ++it ) {
    (*it)->setDefault();
  }
  usrSetDefaults();
}

void KCoreConfigSkeleton::readConfig()
{
    // qDebug();
  d->mConfig->reparseConfiguration();
  KConfigSkeletonItem::List::ConstIterator it;
  for( it = d->mItems.constBegin(); it != d->mItems.constEnd(); ++it )
  {
    (*it)->readConfig( d->mConfig.data() );
  }
  usrReadConfig();
}

bool KCoreConfigSkeleton::writeConfig()
{
    //qDebug();
  KConfigSkeletonItem::List::ConstIterator it;
  for( it = d->mItems.constBegin(); it != d->mItems.constEnd(); ++it )
  {
    (*it)->writeConfig( d->mConfig.data() );
  }
  if (!usrWriteConfig())
    return false;

  if (!d->mConfig->sync())
    return false;

  readConfig();

  emit configChanged();

  return true;
}

bool KCoreConfigSkeleton::usrUseDefaults(bool)
{
  return false;
}

void KCoreConfigSkeleton::usrSetDefaults()
{
}

void KCoreConfigSkeleton::usrReadConfig()
{
}

bool KCoreConfigSkeleton::usrWriteConfig()
{
  return true;
}

void KCoreConfigSkeleton::addItem( KConfigSkeletonItem *item, const QString &name )
{
    if (d->mItems.contains(item)) {
        if (item->name() == name ||
            (name.isEmpty() && item->name() == item->key())) {
            // nothing to do -> it is already in our collection
            // and the name isn't changing
            return;
        }

        d->mItemDict.remove(item->name());
    } else {
        d->mItems.append( item );
    }

    item->setName(name.isEmpty() ? item->key() : name);
    d->mItemDict.insert(item->name(), item);
    item->readDefault(d->mConfig.data());
    item->readConfig(d->mConfig.data());
}

void KCoreConfigSkeleton::removeItem(const QString &name)
{
    KConfigSkeletonItem *item = d->mItemDict.value(name);
    if (item) {
        d->mItems.removeAll(item);
        d->mItemDict.remove(item->name());
        delete item;
    }
}

void KCoreConfigSkeleton::clearItems()
{
    KConfigSkeletonItem::List items = d->mItems;
    d->mItems.clear();
    d->mItemDict.clear();
    qDeleteAll(items);
}

KCoreConfigSkeleton::ItemString *KCoreConfigSkeleton::addItemString( const QString &name, QString &reference,
                                     const QString &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemString *item;
  item = new KCoreConfigSkeleton::ItemString( d->mCurrentGroup, key.isEmpty() ? name : key,
                                          reference, defaultValue,
                                          KCoreConfigSkeleton::ItemString::Normal );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemPassword *KCoreConfigSkeleton::addItemPassword( const QString &name, QString &reference,
                                       const QString &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemPassword *item;
  item = new KCoreConfigSkeleton::ItemPassword( d->mCurrentGroup, key.isNull() ? name : key,
                                          reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemPath *KCoreConfigSkeleton::addItemPath( const QString &name, QString &reference,
                                   const QString &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemPath *item;
  item = new KCoreConfigSkeleton::ItemPath( d->mCurrentGroup, key.isNull() ? name : key,
                                        reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemProperty *KCoreConfigSkeleton::addItemProperty( const QString &name, QVariant &reference,
                                       const QVariant &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemProperty *item;
  item = new KCoreConfigSkeleton::ItemProperty( d->mCurrentGroup, key.isNull() ? name : key,
                                            reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemBool *KCoreConfigSkeleton::addItemBool( const QString &name, bool &reference,
                                   bool defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemBool *item;
  item = new KCoreConfigSkeleton::ItemBool( d->mCurrentGroup, key.isNull() ? name : key,
                                        reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemInt *KCoreConfigSkeleton::addItemInt( const QString &name, qint32 &reference,
                                  qint32 defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemInt *item;
  item = new KCoreConfigSkeleton::ItemInt( d->mCurrentGroup, key.isNull() ? name : key,
                                       reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemUInt *KCoreConfigSkeleton::addItemUInt( const QString &name, quint32 &reference,
                                   quint32 defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemUInt *item;
  item = new KCoreConfigSkeleton::ItemUInt( d->mCurrentGroup, key.isNull() ? name : key,
                                        reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemLongLong *KCoreConfigSkeleton::addItemLongLong( const QString &name, qint64 &reference,
                                    qint64 defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemLongLong *item;
  item = new KCoreConfigSkeleton::ItemLongLong( d->mCurrentGroup, key.isNull() ? name : key,
                                         reference, defaultValue );
  addItem( item, name );
  return item;
}

#ifndef KDE_NO_DEPRECATED
KCoreConfigSkeleton::ItemLongLong *KCoreConfigSkeleton::addItemInt64(
        const QString& name,
        qint64 &reference,
        qint64 defaultValue,
        const QString & key)
{
    return addItemLongLong(name, reference, defaultValue, key);
}
#endif

KCoreConfigSkeleton::ItemULongLong *KCoreConfigSkeleton::addItemULongLong( const QString &name, quint64 &reference,
                                     quint64 defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemULongLong *item;
  item = new KCoreConfigSkeleton::ItemULongLong( d->mCurrentGroup, key.isNull() ? name : key,
                                          reference, defaultValue );
  addItem( item, name );
  return item;
}

#ifndef KDE_NO_DEPRECATED
KCoreConfigSkeleton::ItemULongLong *KCoreConfigSkeleton::addItemUInt64(
        const QString & name,
        quint64 &reference,
        quint64 defaultValue,
        const QString & key)
{
    return addItemULongLong(name, reference, defaultValue, key);
}
#endif

KCoreConfigSkeleton::ItemDouble *KCoreConfigSkeleton::addItemDouble( const QString &name, double &reference,
                                     double defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemDouble *item;
  item = new KCoreConfigSkeleton::ItemDouble( d->mCurrentGroup, key.isNull() ? name : key,
                                          reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemRect *KCoreConfigSkeleton::addItemRect( const QString &name, QRect &reference,
                                   const QRect &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemRect *item;
  item = new KCoreConfigSkeleton::ItemRect( d->mCurrentGroup, key.isNull() ? name : key,
                                        reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemPoint *KCoreConfigSkeleton::addItemPoint( const QString &name, QPoint &reference,
                                    const QPoint &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemPoint *item;
  item = new KCoreConfigSkeleton::ItemPoint( d->mCurrentGroup, key.isNull() ? name : key,
                                         reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemSize *KCoreConfigSkeleton::addItemSize( const QString &name, QSize &reference,
                                   const QSize &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemSize *item;
  item = new KCoreConfigSkeleton::ItemSize( d->mCurrentGroup, key.isNull() ? name : key,
                                        reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemDateTime *KCoreConfigSkeleton::addItemDateTime( const QString &name, QDateTime &reference,
                                       const QDateTime &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemDateTime *item;
  item = new KCoreConfigSkeleton::ItemDateTime( d->mCurrentGroup, key.isNull() ? name : key,
                                            reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemStringList *KCoreConfigSkeleton::addItemStringList( const QString &name, QStringList &reference,
                                         const QStringList &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemStringList *item;
  item = new KCoreConfigSkeleton::ItemStringList( d->mCurrentGroup, key.isNull() ? name : key,
                                              reference, defaultValue );
  addItem( item, name );
  return item;
}

KCoreConfigSkeleton::ItemIntList *KCoreConfigSkeleton::addItemIntList( const QString &name, QList<int> &reference,
                                      const QList<int> &defaultValue, const QString &key )
{
  KCoreConfigSkeleton::ItemIntList *item;
  item = new KCoreConfigSkeleton::ItemIntList( d->mCurrentGroup, key.isNull() ? name : key,
                                           reference, defaultValue );
  addItem( item, name );
  return item;
}

bool KCoreConfigSkeleton::isImmutable(const QString &name) const
{
  KConfigSkeletonItem *item = findItem(name);
  return !item || item->isImmutable();
}

KConfigSkeletonItem *KCoreConfigSkeleton::findItem(const QString &name) const
{
  return d->mItemDict.value(name);
}

#include "kcoreconfigskeleton.moc"
