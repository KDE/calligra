/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2003 Peter Simonsson
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
#include "kivio_pluginmanager.h"

#include <kdebug.h>
#include <kparts/plugin.h>

#include "kivio_plugin.h"

namespace Kivio {
  PluginManager::PluginManager(KivioView* parent, const char* name) : QObject(parent, name)
  {
    m_activeTool = 0;
    m_defaultTool = 0;
    m_delegateEvents = true;
  }
  
  PluginManager::~PluginManager()
  {
  }
  
  bool PluginManager::delegateEvent(QEvent* e)
  {
    if(activeTool() && m_delegateEvents) {
      return activeTool()->processEvent(e);
    }
    
    return false;
  }
  
  Kivio::MouseTool* PluginManager::activeTool()
  {
    return m_activeTool;
  }
  
  Kivio::MouseTool* PluginManager::defaultTool()
  {
    return m_defaultTool;
  }

  void PluginManager::activateDefaultTool()
  {
    if(defaultTool()) {
      kdDebug(43000) << "Default tool activated! " << defaultTool()->name() << endl;
      defaultTool()->setActivated(true);
    }
  }
  
  void PluginManager::activate(Kivio::MouseTool* tool)
  {
    if(tool != m_activeTool) {
      if(m_activeTool) {
        kdDebug(43000) << "Deactivate tool: " << m_activeTool->name() << endl;
        m_activeTool->setActivated(false);
      }
      
      kdDebug(43000) << "Activate new tool: " << tool->name() << endl;
      m_activeTool = tool;
    }
  }

  void PluginManager::setDefaultTool(Kivio::MouseTool* tool)
  {
    m_defaultTool = tool;
    kdDebug(43000) << "New default tool: " << tool->name() << endl;
  }

  Kivio::Plugin* PluginManager::findPlugin(const QString& name)
  {
    QPtrList<KParts::Plugin> plugins = KParts::Plugin::pluginObjects(parent());
    KParts::Plugin* p = plugins.first();
    bool found = false;
    
    while(p && !found) {
      if(p->name() == name) {
        found = true;
      } else {
        p = plugins.next();
      }
    }
    
    return static_cast<Kivio::Plugin*>(p);
  }
}

#include "kivio_pluginmanager.moc"
