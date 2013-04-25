/*
 *  Copyright (C) 2012 C. Boemann <cbo@boemann.dk>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef SIMPLEPAGESTYLEWIDGET_H
#define SIMPLEPAGESTYLEWIDGET_H

#include <QWidget>

class KWView;
class KWPageTool;
namespace Ui {
class SimplePageStyleWidget;
}

class SimplePageStyleWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimplePageStyleWidget(KWView* view, KWPageTool* pageTool, QWidget *parent = 0);
    ~SimplePageStyleWidget();
    
private slots:
    void refreshInformations();

    void on_spinTo_valueChanged(int arg1);

    void on_spinFrom_valueChanged(int arg1);

    void on_buttonApply_clicked();

    void on_list_clicked(const int &index);

private:
    KWPageTool* m_pageTool;
    KWView* m_view;
    Ui::SimplePageStyleWidget *ui;
};

#endif // SIMPLEPAGESTYLEWIDGET_H
