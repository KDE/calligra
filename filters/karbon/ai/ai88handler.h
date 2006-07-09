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

#ifndef AI88HANDLER_H
#define AI88HANDLER_H

#include "aiparserbase.h"

/**
  *@author Dirk Schoenberger
  */

class AI88Handler {
private:
  AIParserBase *m_delegate;

  void _handleSetDash();
  void _handleSetStrokeColorCMYK();
  void _handleSetFillColorCMYK();
  void _handleSetStrokeColorGray();
  void _handleSetFillColorGray();
  void _handleSetStrokeColorCustom();
  void _handleSetFillColorCustom();
  void _handleSetFillPattern();
  void _handleSetStrokePattern();
  void _handlePatternDefinition();
  void _handleGsaveIncludeDocument();
  void _handleSetCurrentText();
  void _handleTextBlock (TextOperation to);
  void _handleTextOutput ();
  void _handleFontEncoding();

public:
	AI88Handler(AIParserBase *delegate);
	~AI88Handler();

  bool handleAIOperation (AIOperation op);
};

#endif
