/*  This file is part of the KDE libraries
 *  Copyright (C) 1999 Waldo Bastian <bastian@kde.org>
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

#include "ksycocaentry.h"
#include "ksycocaentry_p.h"

#include <ksycoca.h>

KSycocaEntryPrivate::KSycocaEntryPrivate(QDataStream &_str, int iOffset)
    : offset(iOffset), deleted(false)
{
    KSycocaEntry::read( _str, path );
}

KSycocaEntry::KSycocaEntry()
    : d_ptr(0)
{
}

KSycocaEntry::KSycocaEntry(KSycocaEntryPrivate &d)
    : d_ptr(&d)
{
}

KSycocaEntry::~KSycocaEntry()
{
    delete d_ptr;
}

void KSycocaEntry::read( QDataStream &s, QString &str )
{
  quint32 bytes;
  s >> bytes;                          // read size of string
  if ( bytes > 8192 ) {                // null string or too big
      if (bytes != 0xffffffff)
         KSycoca::flagError();
      str.clear();
  }
  else if ( bytes > 0 ) {              // not empty
      int bt = bytes/2;
      str.resize( bt );
      QChar* ch = (QChar *) str.unicode();
      char t[8192];
      char *b = t;
      s.readRawData( b, bytes );
      while ( bt-- ) {
          *ch++ = (ushort) (((ushort)b[0])<<8) | (uchar)b[1];
          b += 2;
      }
  } else {
      str.clear();
  }
}

void KSycocaEntry::read( QDataStream &s, QStringList &list )
{
  list.clear();
  quint32 count;
  s >> count;                          // read size of list
  if (count >= 1024)
  {
     KSycoca::flagError();
     return;
  }
  for(quint32 i = 0; i < count; i++)
  {
     QString str;
     read(s, str);
     list.append( str );
     if (s.atEnd())
     {
        KSycoca::flagError();
        return;
     }
  }
}

bool KSycocaEntry::isType(KSycocaType t) const
{
    return d_ptr->isType(t);
}

KSycocaType KSycocaEntry::sycocaType() const
{
    return d_ptr->sycocaType();
}

QString KSycocaEntry::entryPath() const
{
    Q_D(const KSycocaEntry);
    return d->path;
}

QString KSycocaEntry::storageId() const
{
    Q_D(const KSycocaEntry);
    return d->storageId();
}

bool KSycocaEntry::isDeleted() const
{
    Q_D(const KSycocaEntry);
    return d->deleted;
}

void KSycocaEntry::setDeleted( bool deleted )
{
    Q_D(KSycocaEntry);
    d->deleted = deleted;
}

bool KSycocaEntry::isSeparator() const
{
    return d_ptr == 0 || isType(KST_KServiceSeparator);
}

int KSycocaEntry::offset() const
{
    Q_D(const KSycocaEntry);
    return d->offset;
}

void KSycocaEntryPrivate::save(QDataStream &s)
{
    offset = s.device()->pos(); // store position in member variable
    s << qint32(sycocaType()) << path;
}

void KSycocaEntry::save(QDataStream &s)
{
    Q_D(KSycocaEntry);
    d->save(s);
}

bool KSycocaEntry::isValid() const
{
    Q_D(const KSycocaEntry);
    return d && d->isValid();
}

QString KSycocaEntry::name() const
{
    Q_D(const KSycocaEntry);
    return d->name();
}

QStringList KSycocaEntry::propertyNames() const
{
    Q_D(const KSycocaEntry);
    return d->propertyNames();
}

QVariant KSycocaEntry::property(const QString &name) const
{
    Q_D(const KSycocaEntry);
    return d->property(name);
}
