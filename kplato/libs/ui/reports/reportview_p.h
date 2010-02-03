/* This file is part of the KDE project
  Copyright (C) 2010 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#ifndef KPLATOREPORTVIEW_P_H
#define KPLATOREPORTVIEW_P_H


#include "kplatoui_export.h"

#include "reportdesigner.h"

class KoDocument;

class KoReportData;
class ORPreRender;
class ORODocument;
class ReportViewPageSelect;
class RecordNavigator;
class ScriptAdaptor;
class ReportDesigner;

class QScrollArea;
class QDomElement;

namespace KPlato
{

//-----------------
class KPlato_ReportDesigner : public ReportDesigner
{
    Q_OBJECT
public:
    KPlato_ReportDesigner( QWidget *parent = 0);

};


} // namespace KPlato

#endif
