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
#include "layerlisttooltip.h"

#include <qheader.h>

#include <klocale.h>

#include "kivio_layer_panel.h"
#include "kivio_layer.h"

namespace Kivio {

LayerListToolTip::LayerListToolTip(QWidget* parent, QListView* listView)
 : QToolTip(parent)
{
  m_listView = listView;
}

LayerListToolTip::~LayerListToolTip()
{
}

void LayerListToolTip::maybeTip(const QPoint& pos)
{
  if(!m_listView) {
    return;
  }

  KivioLayerItem* item = dynamic_cast<KivioLayerItem*>(m_listView->itemAt(pos));

  if(!item) {
    return;
  }

  QRect itemRect = m_listView->itemRect(item);
  QString toolTip = "<b>" + item->layer()->name() + "</b><br>";
  toolTip += i18n("Visible: ") + (item->layer()->visible() ? i18n("Yes") : i18n("No")) + "<br>";
  toolTip += i18n("Printable: ") + (item->layer()->printable() ? i18n("Yes") : i18n("No")) + "<br>";
  toolTip += i18n("Editable: ") + (item->layer()->editable() ? i18n("Yes") : i18n("No")) + "<br>";
  toolTip += i18n("Connectable: ") + (item->layer()->connectable() ? i18n("Yes") : i18n("No"));
  tip(itemRect, toolTip);
}

}
