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
#ifndef KIVIOMOUSETOOLACTION_H
#define KIVIOMOUSETOOLACTION_H

#include <kactionclasses.h>
#include <koffice_export.h>

namespace Kivio {

class KIVIO_EXPORT MouseToolAction : public KRadioAction
{
  Q_OBJECT
  public:
    /**
    * Constructs a mouse tool action with text and potential keyboard
    * accelerator but nothing else. Use this only if you really
    * know what you are doing.
    *
    * @param text The text that will be displayed.
    * @param cut The corresponding keyboard accelerator (shortcut).
    * @param parent This action's parent.
    * @param name An internal name for this action.
      */
    MouseToolAction(const QString& text, const KShortcut& cut = KShortcut(), QObject* parent = 0, const char* name = 0);
  
    /**
      *  @param text The text that will be displayed.
      *  @param cut The corresponding keyboard accelerator (shortcut).
      *  @param receiver The SLOT's parent.
      *  @param slot The SLOT to invoke to execute this action.
      *  @param parent This action's parent.
      *  @param name An internal name for this action.
      */
    MouseToolAction(const QString& text, const KShortcut& cut,
                  const QObject* receiver, const char* slot, QObject* parent, const char* name = 0);
  
    /**
      *  @param text The text that will be displayed.
      *  @param pix The icons that go with this action.
      *  @param cut The corresponding keyboard accelerator (shortcut).
      *  @param parent This action's parent.
      *  @param name An internal name for this action.
      */
    MouseToolAction(const QString& text, const QIconSet& pix, const KShortcut& cut = KShortcut(),
                  QObject* parent = 0, const char* name = 0);
  
    /**
      *  @param text The text that will be displayed.
      *  @param pix The dynamically loaded icon that goes with this action.
      *  @param cut The corresponding keyboard accelerator (shortcut).
      *  @param parent This action's parent.
      *  @param name An internal name for this action.
      */
    MouseToolAction(const QString& text, const QString& pix, const KShortcut& cut = KShortcut(),
                  QObject* parent = 0, const char* name = 0);
  
    /**
      *  @param text The text that will be displayed.
      *  @param pix The icons that go with this action.
      *  @param cut The corresponding keyboard accelerator (shortcut).
      *  @param receiver The SLOT's parent.
      *  @param slot The SLOT to invoke to execute this action.
      *  @param parent This action's parent.
      *  @param name An internal name for this action.
      */
    MouseToolAction(const QString& text, const QIconSet& pix, const KShortcut& cut,
                  const QObject* receiver, const char* slot, QObject* parent, const char* name = 0);
  
    /**
      *  @param text The text that will be displayed.
      *  @param pix The dynamically loaded icon that goes with this action.
      *  @param cut The corresponding keyboard accelerator (shortcut).
      *  @param receiver The SLOT's parent.
      *  @param slot The SLOT to invoke to execute this action.
      *  @param parent This action's parent.
      *  @param name An internal name for this action.
      */
    MouseToolAction(const QString& text, const QString& pix, const KShortcut& cut,
                  const QObject* receiver, const char* slot,
                  QObject* parent, const char* name = 0);
  
    /**
      *  @param parent This action's parent.
      *  @param name An internal name for this action.
      */
    MouseToolAction(QObject* parent = 0, const char* name = 0);

    ~MouseToolAction();

    /**
     *  "Plug" or insert this action into a given widget.
     *
     *  This will typically be a menu or a toolbar.  From this point
     *  on, you will never need to directly manipulate the item in the
     *  menu or toolbar.  You do all enabling/disabling/manipulation
     *  directly with your MouseToolAction object.
     *
     *  @param widget The GUI element to display this action.
     *  @param index  The index of the item.
     */
    virtual int plug(QWidget* widget, int index = -1);
  
  signals:
    void doubleClicked();
};

}

#endif
