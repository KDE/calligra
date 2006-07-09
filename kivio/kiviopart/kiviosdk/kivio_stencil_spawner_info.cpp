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
#include "kivio_stencil_spawner_info.h"
#include <qdom.h>

#include <kglobal.h>
#include <klocale.h>

KivioStencilSpawnerInfo::KivioStencilSpawnerInfo()
{
    m_author = "Joe Bob";
    m_title  = "Untitled";
    m_id = "";
    m_desc   = "No desc";
    m_version = "1.0";
    m_web    = "http://kivio.sourceforge.net";
    m_email  = "landshark@ameritech.net";
    m_autoUpdate = "";
}

KivioStencilSpawnerInfo::~KivioStencilSpawnerInfo()
{
}

KivioStencilSpawnerInfo::KivioStencilSpawnerInfo( const QString &auth, const QString &tit, const QString &_id, const QString &des, const QString &ver, const QString &wb,
                                            const QString &em, const QString &au )
{
    m_author = auth;
    m_title = tit;
    m_id = _id;
    m_desc = des;
    m_version = ver;
    m_web = wb;
    m_email = em;
    m_autoUpdate = au;
}

bool KivioStencilSpawnerInfo::loadXML( const QDomElement &e )
{
  QDomNode node;
  QDomElement nodeElement;
  QString nodeName, origTitle, origDesc;
  m_title = "";
  m_desc = "";

  node = e.firstChild();
  while(!node.isNull())
  {
    nodeName = node.nodeName();

    nodeElement = node.toElement();

    if( nodeName.compare("Author")==0 )
    {
      m_author = nodeElement.attribute("data");
    }
    else if((nodeName.compare("Title")==0) && nodeElement.hasAttribute("lang"))
    {
      if(nodeElement.attribute("lang") == KGlobal::locale()->language()) {
        m_title = nodeElement.attribute("data");
      }
    }
    else if((nodeName.compare("Title")==0) && !nodeElement.hasAttribute("lang"))
    {
      origTitle = nodeElement.attribute("data");
    }
    else if( nodeName.compare("Id")==0 )
    {
      m_id = nodeElement.attribute("data");
    }
    else if((nodeName.compare("Description")==0) && nodeElement.hasAttribute("lang"))
    {
      if(nodeElement.attribute("lang") == KGlobal::locale()->language()) {
        m_desc = nodeElement.attribute("data");
      }
    }
    else if((nodeName.compare("Description")==0) && !nodeElement.hasAttribute("lang"))
    {
      origDesc = nodeElement.attribute("data");
    }
    else if( nodeName.compare("Version")==0 )
    {
      m_version = nodeElement.attribute("data");
    }
    else if( nodeName.compare("Web")==0 )
    {
      m_web = nodeElement.attribute("data");
    }
    else if( nodeName.compare("Email")==0 )
    {
      m_email = nodeElement.attribute("data");
    }
    else if( nodeName.compare("AutoUpdate")==0 )
    {
      m_autoUpdate = nodeElement.attribute("data");
    }
    
    if(m_title.isEmpty()) {
      m_title = i18n( "Stencils", origTitle.utf8());
    }
    
    if(m_desc.isEmpty()) {
      m_desc = i18n( "Stencils", origDesc.utf8());
    }

    node = node.nextSibling();
  }

  return true;
}
