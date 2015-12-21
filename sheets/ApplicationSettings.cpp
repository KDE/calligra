/* This file is part of the KDE project
   Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include "ApplicationSettings.h"

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN ApplicationSettings::Private
{
public:
    QColor gridColor;
    QColor pageOutlineColor;
    KCompletion::CompletionMode completionMode;
    Calligra::Sheets::MoveTo moveTo;
    MethodOfCalc calcMethod;
    double indentValue;
    bool verticalScrollBar      : 1;
    bool horizontalScrollBar    : 1;
    bool columnHeader           : 1;
    bool rowHeader              : 1;
    bool showStatusBar          : 1;
    bool showTabBar             : 1;
};

ApplicationSettings::ApplicationSettings()
        : d(new Private)
{
    d->gridColor = Qt::lightGray;
    d->pageOutlineColor = Qt::red;
    d->completionMode = KCompletion::CompletionAuto;
    d->moveTo = Bottom;
    d->calcMethod = SumOfNumber;
    d->indentValue = 10.0;
    d->verticalScrollBar = true;
    d->horizontalScrollBar = true;
    d->columnHeader = true;
    d->rowHeader = true;
    d->showStatusBar = true;
    d->showTabBar = true;
}

ApplicationSettings::~ApplicationSettings()
{
    delete d;
}

void ApplicationSettings::load()
{
}

void ApplicationSettings::save() const
{
}

void ApplicationSettings::setShowVerticalScrollBar(bool show)
{
    d->verticalScrollBar = show;
}

bool ApplicationSettings::showVerticalScrollBar()const
{
    return d->verticalScrollBar;
}

void ApplicationSettings::setShowHorizontalScrollBar(bool show)
{
    d->horizontalScrollBar = show;
}

bool ApplicationSettings::showHorizontalScrollBar()const
{
    return d->horizontalScrollBar;
}

KCompletion::CompletionMode ApplicationSettings::completionMode() const
{
    return d->completionMode;
}

void ApplicationSettings::setShowColumnHeader(bool show)
{
    d->columnHeader = show;
}

bool ApplicationSettings::showColumnHeader() const
{
    return d->columnHeader;
}

void ApplicationSettings::setShowRowHeader(bool show)
{
    d->rowHeader = show;
}

bool ApplicationSettings::showRowHeader() const
{
    return d->rowHeader;
}

void ApplicationSettings::setGridColor(const QColor& color)
{
    d->gridColor = color;
}

QColor ApplicationSettings::gridColor() const
{
    return d->gridColor;
}

void ApplicationSettings::setCompletionMode(KCompletion::CompletionMode complMode)
{
    d->completionMode = complMode;
}

double ApplicationSettings::indentValue() const
{
    return d->indentValue;
}

void ApplicationSettings::setIndentValue(double val)
{
    d->indentValue = val;
}

void ApplicationSettings::setShowStatusBar(bool statusBar)
{
    d->showStatusBar = statusBar;
}

bool ApplicationSettings::showStatusBar() const
{
    return d->showStatusBar;
}

void ApplicationSettings::setShowTabBar(bool tabbar)
{
    d->showTabBar = tabbar;
}

bool ApplicationSettings::showTabBar()const
{
    return d->showTabBar;
}

Calligra::Sheets::MoveTo ApplicationSettings::moveToValue() const
{
    return d->moveTo;
}

void ApplicationSettings::setMoveToValue(Calligra::Sheets::MoveTo moveTo)
{
    d->moveTo = moveTo;
}

void ApplicationSettings::setTypeOfCalc(MethodOfCalc calc)
{
    d->calcMethod = calc;
}

MethodOfCalc ApplicationSettings::getTypeOfCalc() const
{
    return d->calcMethod;
}

QColor ApplicationSettings::pageOutlineColor() const
{
    return d->pageOutlineColor;
}

void ApplicationSettings::changePageOutlineColor(const QColor& color)
{
    d->pageOutlineColor = color;
}

