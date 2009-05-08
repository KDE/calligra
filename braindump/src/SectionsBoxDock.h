/*
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SECTIONS_BOX_DOCKER_H_
#define _SECTIONS_BOX_DOCKER_H_

#include "ui_wdgsectionsbox.h"

#include <QDockWidget>
#include <KoDockFactory.h>

class SectionsBoxDock : public QDockWidget {
  public:
    SectionsBoxDock();
    virtual ~SectionsBoxDock();
  private:
    Ui::WdgSectionsBox m_wdgSectionsBox;
};

#endif
