/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
 *                     David Faure   <faure@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
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
 **/

#include "kservicetype.h"
#include "kservicetype_p.h"
#include "ksycoca.h"
#include "kservice.h"
#include "kservicetypefactory.h"
#include "kservicefactory.h"
#include "kservicetypeprofile.h"
#include <assert.h>
#include <kdesktopfile.h>
#include <kconfiggroup.h>
#include <QDebug>

extern int servicesDebugArea();

template QDataStream& operator>> <QString, QVariant>(QDataStream&, QMap<QString, QVariant>&);
template QDataStream& operator<< <QString, QVariant>(QDataStream&, const QMap<QString, QVariant>&);

KServiceType::KServiceType( KServiceTypePrivate &dd, const QString& _name,
                            const QString& _comment )
    : KSycocaEntry(dd)
{
    Q_D(KServiceType);
    d->m_strName = _name;
    d->m_strComment = _comment;
}

KServiceType::KServiceType( KDesktopFile *config )
    : KSycocaEntry(*new KServiceTypePrivate(config->fileName()))
{
    Q_D(KServiceType);
    d->init(config);
}

void
KServiceTypePrivate::init( KDesktopFile *config )
{
//    Q_Q(KServiceType);

    KConfigGroup desktopGroup = config->desktopGroup();
    m_strName = desktopGroup.readEntry( "X-KDE-ServiceType" );
    m_strComment = desktopGroup.readEntry("Comment");
    deleted = desktopGroup.readEntry("Hidden", false);

    // We store this as property to preserve BC, we can't change that
    // because KSycoca needs to remain BC between KDE 2.x and KDE 3.x
    QString sDerived = desktopGroup.readEntry( "X-KDE-Derived" );
    m_bDerived = !sDerived.isEmpty();
    if ( m_bDerived )
        m_mapProps.insert( QString::fromLatin1("X-KDE-Derived"), sDerived );

    const QStringList tmpList = config->groupList();
    QStringList::const_iterator gIt = tmpList.begin();

    for( ; gIt != tmpList.end(); ++gIt ) {
        if ( (*gIt).startsWith( QLatin1String("Property::") ) ) {
            KConfigGroup cg(config, *gIt );
            QVariant v = QVariant::nameToType( cg.readEntry( "Type" ).toLatin1().constData() );
            v = cg.readEntry( "Value", v );

            if ( v.isValid() )
                m_mapProps.insert( (*gIt).mid( 10 ), v );
        }
    }

    gIt = tmpList.begin();
    for( ; gIt != tmpList.end(); ++gIt ) {
        if( (*gIt).startsWith( QLatin1String("PropertyDef::") ) ) {
            KConfigGroup cg(config, *gIt);
            m_mapPropDefs.insert( (*gIt).mid( 13 ),
                                  QVariant::nameToType( cg.readEntry( "Type" ).toLatin1().constData() ) );
        }
    }
}

KServiceType::KServiceType( QDataStream& _str, int offset )
    : KSycocaEntry(*new KServiceTypePrivate(_str, offset))
{
    Q_D(KServiceType);
    d->load(_str);
}

KServiceType::KServiceType( KServiceTypePrivate &dd)
    : KSycocaEntry(dd)
{
}

void
KServiceTypePrivate::load( QDataStream& _str )
{
    qint8 b;
    QString dummy;
    _str >> m_strName >> dummy >> m_strComment >> m_mapProps >> m_mapPropDefs
         >> b >> m_serviceOffersOffset;
    m_bDerived = m_mapProps.contains(QString::fromLatin1("X-KDE-Derived"));
}

void
KServiceTypePrivate::save( QDataStream& _str )
{
  KSycocaEntryPrivate::save( _str );
  // !! This data structure should remain binary compatible at all times !!
  // You may add new fields at the end. Make sure to update the version
  // number in ksycoca.h
  _str << m_strName << QString() /*was icon*/ << m_strComment << m_mapProps << m_mapPropDefs
       << (qint8) 1 << m_serviceOffersOffset;
}

KServiceType::~KServiceType()
{
}

QString KServiceType::parentServiceType() const
{
    const QVariant v = property(QString::fromLatin1("X-KDE-Derived"));
    return v.toString();
}

bool KServiceType::inherits( const QString& servTypeName ) const
{
    if ( name() == servTypeName )
        return true;
    QString st = parentServiceType();
    while ( !st.isEmpty() )
    {
        KServiceType::Ptr ptr = KServiceType::serviceType( st );
        if (!ptr) return false; //error
        if ( ptr->name() == servTypeName )
            return true;
        st = ptr->parentServiceType();
    }
    return false;
}

QVariant
KServiceTypePrivate::property( const QString& _name ) const
{
    QVariant v;

    if ( _name == QLatin1String("Name") )
        v = QVariant( m_strName );
    else if ( _name == QLatin1String("Comment") )
        v = QVariant( m_strComment );
    else
        v = m_mapProps.value( _name );

    return v;
}

QStringList
KServiceTypePrivate::propertyNames() const
{
    QStringList res = m_mapProps.keys();
    res.append( QString::fromLatin1("Name") );
    res.append( QString::fromLatin1("Comment") );
    return res;
}

QVariant::Type
KServiceType::propertyDef( const QString& _name ) const
{
    Q_D(const KServiceType);
    return static_cast<QVariant::Type>( d->m_mapPropDefs.value( _name, QVariant::Invalid ) );
}

QStringList
KServiceType::propertyDefNames() const
{
    Q_D(const KServiceType);
    return d->m_mapPropDefs.keys();
}

KServiceType::Ptr KServiceType::serviceType( const QString& _name )
{
    return KServiceTypeFactory::self()->findServiceTypeByName( _name );
}

KServiceType::List KServiceType::allServiceTypes()
{
    return KServiceTypeFactory::self()->allServiceTypes();
}

KServiceType::Ptr KServiceType::parentType()
{
    Q_D(KServiceType);
    if (d->m_parentTypeLoaded)
        return d->parentType;

    d->m_parentTypeLoaded = true;

    const QString parentSt = parentServiceType();
    if (parentSt.isEmpty())
        return KServiceType::Ptr();

    d->parentType = KServiceTypeFactory::self()->findServiceTypeByName( parentSt );
    if (!d->parentType)
        qWarning() << entryPath() << "specifies undefined mimetype/servicetype" << parentSt;
    return d->parentType;
}

void KServiceType::setServiceOffersOffset( int offset )
{
    Q_D(KServiceType);
    Q_ASSERT( offset != -1 );
    d->m_serviceOffersOffset = offset;
}

int KServiceType::serviceOffersOffset() const
{
    Q_D(const KServiceType);
    return d->serviceOffersOffset();
}

QString KServiceType::comment() const
{
    Q_D(const KServiceType);
    return d->comment();
}

// ## KDE4: remove?
#ifndef KDE_NO_DEPRECATED
QString KServiceType::desktopEntryPath() const
{
    return entryPath();
}
#endif

bool KServiceType::isDerived() const
{
    Q_D(const KServiceType);
    return d->m_bDerived;
}

QMap<QString,QVariant::Type> KServiceType::propertyDefs() const
{
    Q_D(const KServiceType);
    return d->m_mapPropDefs;
}
