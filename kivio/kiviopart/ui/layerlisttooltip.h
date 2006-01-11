/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2006 Peter Simonsson <psn@linux.se>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIVIOLAYERLISTTOOLTIP_H
#define KIVIOLAYERLISTTOOLTIP_H

#include <qtooltip.h>
#include <qlistview.h>

namespace Kivio {

/**
 * Shows the tooltips in the layer list
 */
class LayerListToolTip : public QToolTip
{
  public:
    LayerListToolTip(QWidget* parent, QListView* listView);
    ~LayerListToolTip();

    void maybeTip(const QPoint& pos);

  protected:
    QListView* m_listView;
};

}

#endif
