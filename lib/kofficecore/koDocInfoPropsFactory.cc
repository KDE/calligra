/* This file is part of the KDE project
   Copyright (c) 2000 Simon Hausmann <hausmann@kde.org>

   $Id$

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <koDocInfoPropsFactory.h>
#include <koDocumentInfoDlg.h>

#include <kpropsdlg.h>
#include <kdebug.h>
#include <klocale.h>

#include <assert.h>

extern "C"
{
  void *init_libkodocinfopropspage()
  {
    return new KoDocInfoPropsFactory();
  }
};

KoDocInfoPropsFactory::KoDocInfoPropsFactory( QObject *parent, const char *name )
: KLibFactory( parent, name )
{
  // Install the libkoffice* translations
  KGlobal::locale()->insertCatalogue("koffice");
}

KoDocInfoPropsFactory::~KoDocInfoPropsFactory()
{
}

QObject* KoDocInfoPropsFactory::createObject( QObject* parent, const char*, const char *classname,
			                const QStringList & )
{
  if ( strcmp( classname, "KPropsDlgPlugin" ) == 0 )
  {
    assert( parent );
    if ( !parent->inherits( "KPropertiesDialog" ) )
      return 0L;

    QObject *obj = new KoDocumentInfoPropsPage( static_cast<KPropertiesDialog *>( parent ) );
    return obj;
  }

  return 0L;
}

#include <koDocInfoPropsFactory.moc>
