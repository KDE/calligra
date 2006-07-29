/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2004 Peter Simonsson
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

#ifndef KIVIOKIVIO_CONFIG_H
#define KIVIOKIVIO_CONFIG_H

#include <KoPageLayout.h>

#include "kivio_settings.h"
#include "kivio_grid_data.h"
#include <koffice_export.h>

namespace Kivio {
  /**
  * This class handles all settings
  */
  class KIVIO_EXPORT Config : public Kivio::Settings
  {
    public:
      /**
      * Set the default page layout
      */
      static void setDefaultPageLayout(const KoPageLayout& layout);

      /**
      * Get the default page layout
      */
      static KoPageLayout defaultPageLayout();

      /**
       * Returns the grid settings
       */
      static void setGrid(const KivioGridData& grid);

      /**
       * Returns the grid settings
       */
      static KivioGridData grid();

    protected:
      Config();
  };
}

#endif
