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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "toolbarseparator.h"

#include <ktoolbar.h>
#include <qframe.h>

ToolBarSeparator::ToolBarSeparator( QObject* parent, const char* name )
: KAction(QString::null,0,parent,name)
{
}

ToolBarSeparator::~ToolBarSeparator()
{
}

int ToolBarSeparator::plug( QWidget* widget, int index )
{
  if ( widget->inherits("KToolBar") ) {
    KToolBar *toolBar = static_cast<KToolBar*>(widget);
    QFrame* frame = new QFrame(toolBar);
    frame->setFrameStyle(QFrame::VLine | QFrame::Sunken);
    frame->setLineWidth(1);
    frame->setFixedWidth(6);
    int id = toolBar->insertWidget(-1,6,frame,index);

    addContainer(toolBar,id);
    connect( toolBar, SIGNAL(destroyed()), this, SLOT(slotDestroyed()) );
    return containerCount() - 1;
  }
  return -1;
}

void ToolBarSeparator::unplug( QWidget* widget )
{
}
#include "toolbarseparator.moc"
