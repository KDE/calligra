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
#include "polylineconnectorspawner.h"

#include "kiviopolylineconnector.h"

namespace Kivio {

PolyLineConnectorSpawner::PolyLineConnectorSpawner(KivioStencilSpawnerSet* spawnerSet)
 : KivioStencilSpawner(spawnerSet)
{
  m_info = KivioStencilSpawnerInfo("Kivio Team", "PolyLine Connector", "Internal - PolyLine Connector",
                                   "PolyLine Connector", "0.1", "http://localhost/", "", "off");
}

PolyLineConnectorSpawner::~PolyLineConnectorSpawner()
{
}

bool PolyLineConnectorSpawner::load(const QString&)
{
  return true;
}

QDomElement PolyLineConnectorSpawner::saveXML(QDomDocument& /*doc*/)
{
  return QDomElement();
}

KivioStencil* PolyLineConnectorSpawner::newStencil()
{
  PolyLineConnector* connector = new PolyLineConnector();
  connector->setSpawner(this);

  return connector;
}

KivioStencil* PolyLineConnectorSpawner::newStencil(const QString& /*arg*/)
{
  PolyLineConnector* connector = new PolyLineConnector();
  connector->setSpawner(this);

  return connector;
}

}
