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
#ifndef KIVIO_PLUGINMANAGER_H
#define KIVIO_PLUGINMANAGER_H

#include <qobject.h>
#include "kivio_mousetool.h"
#include <koffice_export.h>

namespace Kivio {
  class Plugin;
  
  /**
  This class manages the plugins*/
  class KIVIO_EXPORT PluginManager : public QObject {
    Q_OBJECT
    public:
      PluginManager(KivioView* parent, const char* name = 0);
      ~PluginManager();

      bool delegateEvent(QEvent* e);
      
      void setEventDelegationEnabled(bool enabled) { m_delegateEvents = enabled; }
      bool eventDelegationEnabled() const { return m_delegateEvents; }
            
      /** Returns the tool that is in use. */
      Kivio::MouseTool* activeTool();
      /** Returns the default tool. */
      Kivio::MouseTool* defaultTool();
      
      Kivio::Plugin* findPlugin(const QString& name);
    
    public slots:
      /** Makes the default tool active. */
      void activateDefaultTool();
      /** Makes @param tool active. */
      void activate(Kivio::MouseTool* tool);
      /** Make @param tool the default. */
      void setDefaultTool(Kivio::MouseTool* tool);
    
    private:
      Kivio::MouseTool* m_activeTool;
      Kivio::MouseTool* m_defaultTool;

      bool m_delegateEvents;
  };
}

#endif
