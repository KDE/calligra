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


#ifndef __KCHARTCONFIGDIALOG_H__
#define __KCHARTCONFIGDIALOG_H__


#include <qtabdialog.h>

#include "koChart.h"


class QWidget;

class KDChartParams;


namespace KChart
{

class KChartParams;
class KChartAuxiliary;

class KChartSubTypeChartPage;
class KChartHeaderFooterConfigPage;

class KChartColorConfigPage;
class KChartParameterConfigPage;
class KChartParameter3dConfigPage;
class KChartParameterPieConfigPage;
class KChartFontConfigPage;
class KChartDataConfigPage;
class KChartPieConfigPage;
class KChartComboPage;
class KChartBackgroundPixmapConfigPage;
class KChartLegendConfigPage;
class KChartLine3dConfigPage;
class KChartParameterPolarConfigPage;

class KChartConfigDialog : public QTabDialog
{
    Q_OBJECT

public:
    enum { KC_FONT = 1, KC_COLORS = 2, KC_BACK = 4, KC_LEGEND=8, KC_SUBTYPE=16,KC_HEADERFOOTER=32,KC_ALL=256 };
    KChartConfigDialog( KChartParams* params,
                        QWidget* parent, int flags,
			KoChart::Data *dat, KChartAuxiliary *aux );

    void subtypePage();

signals:
    void dataChanged();

protected:
    KChartParams                 *m_params;
    KChartAuxiliary              *m_aux;

    KChartDataConfigPage         *m_datapage;
    KChartSubTypeChartPage       *m_subTypePage;

    KChartParameter3dConfigPage      *_parameter3dpage;
    //KChartLine3dConfigPage         *_linepage3d;
    KChartParameterPieConfigPage     *_parameterpiepage;
    KChartParameterPolarConfigPage   *_polarpage;

    KChartHeaderFooterConfigPage     *m_headerfooterpage;
    KChartLegendConfigPage           *m_legendPage;
    KChartParameterConfigPage        *m_axespage;
    KChartColorConfigPage            *_colorpage;
    KChartFontConfigPage             *_parameterfontpage;
    KChartBackgroundPixmapConfigPage *_backgroundpixpage;

    //KChartGeometryConfigPage* _geompage;
    //KChartPieConfigPage*_piepage;
    //KChartComboPage *_hlcChart;

protected slots:
    void         init();
    virtual void apply();
    virtual void defaults();
};

}  //KChart namespace

#endif
