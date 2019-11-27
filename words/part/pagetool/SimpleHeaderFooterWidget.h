/*
 *  Copyright (C) 2012 C. Boemann <cbo@boemann.dk>
 *  
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef SIMPLEHEADERFOOTERWIDGET_H
#define SIMPLEHEADERFOOTERWIDGET_H

#include <QWidget>

#include "ui_SimpleHeaderFooterWidget.h"

class KWView;

class SimpleHeaderFooterWidget : public QWidget
{
    Q_OBJECT
public:

    explicit SimpleHeaderFooterWidget(KWView *view, QWidget *parent = 0);
    ~SimpleHeaderFooterWidget() override;

private:
    Ui::SimpleHeaderFooterWidget widget;
};

#endif // SIMPLEHEADERFOOTERWIDGET_H
