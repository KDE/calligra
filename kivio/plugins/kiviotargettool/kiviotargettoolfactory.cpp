/* This file is part of the KDE project
   Copyright (C) 2004 Peter Simonsson <psn@linux.se>,

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
#include "kiviotargettoolfactory.h"

#include "kiviotargettool.h"

K_EXPORT_COMPONENT_FACTORY( libkiviotargettool, KivioTargetToolFactory )

KInstance* KivioTargetToolFactory::s_global = 0;

KivioTargetToolFactory::KivioTargetToolFactory(QObject *parent, const char *name)
 : KLibFactory(parent, name)
{
  s_global = new KInstance("kivio");
}

KivioTargetToolFactory::~KivioTargetToolFactory()
{
  delete s_global;
}

QObject* KivioTargetToolFactory::createObject( QObject* parent, const char*, const char*, const QStringList& )
{
  if ( !parent->inherits("KivioView") )
    return 0;

  QObject *obj = new Kivio::TargetTool(static_cast<KivioView*>(parent));
  return obj;
}

KInstance* KivioTargetToolFactory::global()
{
  return s_global;
}

#include "kiviotargettoolfactory.moc"
