/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

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

#ifndef __KCHARTDATACONFIGPAGE_H__
#define __KCHARTDATACONFIGPAGE_H__

#include <qwidget.h>
#include <qbutton.h>
#include <qlistbox.h>
#include <qpainter.h>

#include "kchartcolorarray.h"
#include "kchartDataEditor.h"

class QRadioButton;

namespace KChart
{

class KChartParams;

class KChartDataConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartDataConfigPage( KChartParams* params, QWidget* parent,
			  KoChart::Data *dat);
    void init();
    void apply();

public slots:

private:
    KChartParams *m_params;

    QRadioButton* m_rowMajor;
    QRadioButton* m_colMajor;

    KoChart::Data *data;
};

}  //KChart namespace

#endif
