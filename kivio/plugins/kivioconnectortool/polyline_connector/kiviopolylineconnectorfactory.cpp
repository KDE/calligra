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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kiviopolylineconnectorfactory.h"

#include <qpixmap.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "kivio_stencil_spawner_info.h"
#include "kivio_stencil.h"
#include "kiviopolylineconnector.h"

KivioStencilSpawnerInfo KivioPolyLineConnectorFactory::s_ssinfo = KivioStencilSpawnerInfo("Kivio Internal",
  "Polyline Connector", "Kivio Internal - Polyline Connector", "Polyline Connector", "0.1", "http://localhost/", "", "off");

KivioPolyLineConnectorFactory::KivioPolyLineConnectorFactory(QObject *parent, const char* name, const QStringList& args)
 : KivioStencilFactory(parent, name, args)
{
  kdDebug() << "new poly line connector factory: " << endl;
}

KivioPolyLineConnectorFactory::~KivioPolyLineConnectorFactory()
{
}

KivioStencil* KivioPolyLineConnectorFactory::NewStencil(const QString& /*name*/)
{
  return new Kivio::PolyLineConnector();
}

KivioStencil* KivioPolyLineConnectorFactory::NewStencil()
{
  return new Kivio::PolyLineConnector();
}

KivioStencilSpawnerInfo* KivioPolyLineConnectorFactory::GetSpawnerInfo()
{
  return &s_ssinfo;
}

QPixmap* KivioPolyLineConnectorFactory::GetIcon()
{
  return 0;
}
