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
#ifndef KIVIOMOUSETOOL_H
#define KIVIOMOUSETOOL_H

#include "kivio_plugin.h"

#include <qptrlist.h>
#include <koffice_export.h>
class KivioStencil;
class KoPoint;

namespace Kivio {
  /**
  Baseclass for mouse tools (like eg the select tool)*/
  class KIVIO_EXPORT MouseTool : public Kivio::Plugin
  {
    Q_OBJECT
    public:
      MouseTool(KivioView* parent = 0, const char* name = 0);
      ~MouseTool();
    
      virtual bool processEvent(QEvent* e) = 0;
      
    public slots:
      virtual void setActivated(bool) = 0;
      /** This function should apply a change to @p stencils
       * done by the tool.
       */
      virtual void applyToolAction(QPtrList<KivioStencil>* stencils) { Q_UNUSED(stencils); };
      virtual void applyToolAction(KivioStencil* /*stencil*/, const KoPoint& /*pos*/) {};
    
    signals:
      /** this should be emited when the tool wants to be active */
      void activated(Kivio::MouseTool*);
  };
}

#endif
