/* This file is part of the KDE project
   Copyright (C) 2002, Dirk Schönberger <dirk.schoenberger@sz-online.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "ai3handler.h"

AI3Handler::AI3Handler(AIParserBase *delegate){
  m_delegate = delegate;
}
AI3Handler::~AI3Handler(){
}

bool AI3Handler::handleAIOperation (AIOperation op) {
//  PathElement pathElement;
//  double fval;
  int ival;

  switch (op) {
    case AIO_SetWindingOrder :
      ival = m_delegate->getIntValue();
      if (m_delegate->m_gstateHandler) m_delegate->m_gstateHandler->gotWindingOrder (ival);
      return true;
      break;
    case AIO_BeginGroupNoClip :
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotBeginGroup (false);
      return true;
      break;
    case AIO_EndGroupNoClip :
      if (m_delegate->m_debug) qDebug ("got end group noclip");
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotEndGroup (false);
      if (m_delegate->m_debug) qDebug ("/got end group noclip");
      return true;
      break;
    case AIO_BeginCombination :
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotBeginCombination ();
      return true;
      break;
    case AIO_EndCombination :
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotEndCombination ();
      return true;
      break;
    case AIO_BeginGroupClip :
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotBeginGroup (true);
      return true;
      break;
    case AIO_EndGroupClip :
      if (m_delegate->m_structureHandler) m_delegate->m_structureHandler->gotEndGroup (true);
      return true;
      break;
    case AIO_SetFillMode :
      if (m_delegate->m_pathHandler) m_delegate->m_pathHandler->gotFillMode((FillMode) m_delegate->getIntValue());
      return true;
      break;
    default :
      return false;
  }
  return false;
}

