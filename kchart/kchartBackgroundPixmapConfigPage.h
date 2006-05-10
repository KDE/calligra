/* This file is part of the KDE project
   Copyright (C) 2000 Matthias Kalle Dalheimer <kalle@kde.org>

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

#ifndef __KCHARTBACKGROUNDPIXMAPCONFIGPAGE_H__
#define __KCHARTBACKGROUNDPIXMAPCONFIGPAGE_H__

#include <QWidget>
#include <qpixmap.h>

class QSpinBox;
class QRadioButton;
class QComboBox;
class QPixmap;
class Q3ListBox;
class KColorButton;
class Q3GroupBox;

namespace KChart
{

class KChartParams;

class KChartBackgroundPixmapConfigPage : public QWidget
{
    Q_OBJECT

public:
    KChartBackgroundPixmapConfigPage( KChartParams* params,
                                      QWidget* parent );
    void init();
    void apply();

private slots:
    void showSettings( const QString& ); 
    void slotBrowse();
    void slotWallPaperChanged( int );

private:
    bool loadWallPaper();

    KChartParams* _params;
    QComboBox* wallCB;
    QWidget* wallWidget;
    QSpinBox* intensitySB;
    QRadioButton* stretchedRB;
    QRadioButton* scaledRB;
    QRadioButton* centeredRB;
    QRadioButton* tiledRB;
    QString wallFile;
    QPixmap wallPixmap;
#if 0
    Q3ListBox *regionList;
#endif
    KColorButton* _backgroundCB;
    Q3GroupBox* right;
};

}  //KChart namespace

#endif
