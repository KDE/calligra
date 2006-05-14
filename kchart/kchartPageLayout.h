/* This file is part of the KDE project
   Copyright (C) 2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __KCHARTPAGELAYOUT__
#define __KCHARTPAGELAYOUT__

#include <kdialogbase.h>

class QLineEdit;

namespace KChart
{

class KChartView;
class KChartParams;
class KChartPageLayout : public KDialog
{
    Q_OBJECT
public:
    KChartPageLayout( KChartParams* _params, QWidget* parent);
public slots:
    void slotOk();
    void slotApply();
    void slotReset();
protected:
    void init();
private:
    QLineEdit *leftBorder;
    QLineEdit *rightBorder;
    QLineEdit *topBorder;
    QLineEdit *bottomBorder;
    KChartParams* params;
    int oldGlobalLeadingRight;
    int oldGlobalLeadingLeft;
    int oldGlobalLeadingTop;
    int oldGlobalLeadingBottom;
signals:
    void dataChanged();
};

}  //KChart namespace

#endif
