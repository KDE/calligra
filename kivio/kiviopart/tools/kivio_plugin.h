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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef KIVIO_PLUGIN_H
#define KIVIO_PLUGIN_H

#include <kparts/plugin.h>
#include <kivio_view.h>

namespace Kivio {
  /**
  This class provides the base for all plugins in Kivio
  */
  class Plugin : public KParts::Plugin
  {
    Q_OBJECT
    public:
      Plugin(KivioView* parent = 0, const char* name = 0);
      ~Plugin();
      
      KivioView* view() { return static_cast<KivioView*>(parent()); }
  };
}

#endif
