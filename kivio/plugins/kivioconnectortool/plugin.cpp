/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "plugin.h"
#include "tool_connector.h"

#include "kivio_view.h"

#include <kinstance.h>
#include <kiconloader.h>

/***************************************************
 * Factory
 ***************************************************/
K_EXPORT_COMPONENT_FACTORY( libkivioconnectortool, ConnectorToolFactory )

KInstance* ConnectorToolFactory::s_global = 0;

ConnectorToolFactory::ConnectorToolFactory( QObject* parent, const char* name )
: KLibFactory( parent, name )
{
  s_global = new KInstance("kivio");
}

ConnectorToolFactory::~ConnectorToolFactory()
{
  delete s_global;
}

QObject* ConnectorToolFactory::createObject( QObject* parent, const char*, const char*, const QStringList& )
{
  if ( !parent->inherits("KivioView") )
    return 0;

  QObject *obj = new ConnectorTool( (KivioView*)parent );
  return obj;
}

KInstance* ConnectorToolFactory::global()
{
  return s_global;
}
#include "plugin.moc"
