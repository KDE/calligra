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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef _KIVIO_PROTECTION_PANEL_H
#define _KIVIO_PROTECTION_PANEL_H

class KAction;
class KivioView;
class KivioLayer;

#include "kivio_protection_panel_base.h"

class KivioProtectionPanel : public KivioProtectionPanelBase
{
Q_OBJECT

public:
    KivioProtectionPanel( KivioView *view, QWidget *parent=0, const char *name=0 );
    virtual ~KivioProtectionPanel();

public slots:
   void updateCheckBoxes();
   void togWidth(bool);
   void togHeight(bool);
   void togAspect(bool);
   void togDelete(bool);
   void togX(bool);
   void togY(bool);

protected:
   KivioView *m_pView;
};

#endif
