/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson
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
#include "mousetoolaction.h"

#include <ktoolbar.h>
#include <ktoolbarbutton.h>
#include <kdebug.h>

namespace Kivio {

MouseToolAction::MouseToolAction(const QString& text, const KShortcut& cut,
                            QObject* parent, const char* name)
  : KRadioAction(text, cut, parent, name)
{
}

MouseToolAction::MouseToolAction(const QString& text, const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name)
  : KRadioAction(text, cut, receiver, slot, parent, name)
{
}

MouseToolAction::MouseToolAction(const QString& text, const QIconSet& pix,
                            const KShortcut& cut,
                            QObject* parent, const char* name)
  : KRadioAction(text, pix, cut, parent, name)
{
}

MouseToolAction::MouseToolAction(const QString& text, const QString& pix,
                            const KShortcut& cut,
                            QObject* parent, const char* name)
  : KRadioAction(text, pix, cut, parent, name)
{
}

MouseToolAction::MouseToolAction(const QString& text, const QIconSet& pix,
                            const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name)
  : KRadioAction(text, pix, cut, receiver, slot, parent, name)
{
}

MouseToolAction::MouseToolAction(const QString& text, const QString& pix,
                            const KShortcut& cut,
                            const QObject* receiver, const char* slot,
                            QObject* parent, const char* name)
  : KRadioAction(text, pix, cut, receiver, slot, parent, name)
{
}

MouseToolAction::MouseToolAction(QObject* parent, const char* name)
  : KRadioAction(parent, name)
{
}

MouseToolAction::~MouseToolAction()
{
}

int MouseToolAction::plug(QWidget* widget, int index)
{
  int usedIndex = KRadioAction::plug(widget, index);
  
  if(usedIndex == -1) {
    return usedIndex;
  }
  
  if(::qt_cast<KToolBar*>(widget)) {
    KToolBar* toolBar = static_cast<KToolBar*>(widget);
    KToolBarButton* button = toolBar->getButton(itemId(usedIndex));
    connect(button, SIGNAL(doubleClicked(int)), this, SIGNAL(doubleClicked()));
  }
  
  return usedIndex;
}

}

#include "mousetoolaction.moc"
