/* This file is part of the KDE project
   Copyright (C) 1999 Matthias Kalle Dalheimer <kalle@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __KOCHARTBARCONFIGDIALOG_H__
#define __KOCHARTBARCONFIGDIALOG_H__

#include "kchartConfigDialog.h"
#include "kchartDataEditor.h"

namespace KChart
{

class KChartParams;
class KChartBarConfigPage;

class KChartBarConfigDialog : public KChartConfigDialog
{
    Q_OBJECT

public:
    KChartBarConfigDialog( KChartParams* params, 
                           QWidget* parent, int flag, KoChart::Data *dat );

public slots:
    virtual void apply();
    virtual void defaults();

private:
    KChartBarConfigPage* _barpage;
};

}  //KChart namespace

#endif
