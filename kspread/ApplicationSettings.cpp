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

using namespace KSpread;

class ApplicationSettings::Private
{
public:
    QColor gridColor;
    QColor pageBorderColor;
    KGlobalSettings::Completion completionMode;
    KSpread::MoveTo moveTo;
    MethodOfCalc calcMethod;
    double indentValue;
    bool verticalScrollBar      : 1;
    bool horizontalScrollBar    : 1;
    bool columnHeader           : 1;
    bool rowHeader              : 1;
    bool showStatusBar          : 1;
    bool showTabBar             : 1;
    bool captureAllArrowKeys    : 1;
};

ApplicationSettings::ApplicationSettings()
        : d(new Private)
{
    d->gridColor = Qt::lightGray;
    d->pageBorderColor = Qt::red;
    d->completionMode = KGlobalSettings::CompletionAuto;
    d->moveTo = Bottom;
    d->calcMethod = SumOfNumber;
    d->indentValue = 10.0;
    d->verticalScrollBar = true;
    d->horizontalScrollBar = true;
    d->columnHeader = true;
    d->rowHeader = true;
    d->showStatusBar = true;
    d->showTabBar = true;
    d->captureAllArrowKeys = true;
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

KGlobalSettings::Completion ApplicationSettings::completionMode() const
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

void ApplicationSettings::setCompletionMode(KGlobalSettings::Completion complMode)
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

KSpread::MoveTo ApplicationSettings::moveToValue() const
{
    return d->moveTo;
}

void ApplicationSettings::setMoveToValue(KSpread::MoveTo moveTo)
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

QColor ApplicationSettings::pageBorderColor() const
{
    return d->pageBorderColor;
}

void ApplicationSettings::changePageBorderColor(const QColor& color)
{
    d->pageBorderColor = color;
}

void ApplicationSettings::setCaptureAllArrowKeys(bool capture)
{
    d->captureAllArrowKeys = capture;
}

bool ApplicationSettings::captureAllArrowKeys() const
{
    return d->captureAllArrowKeys;
}
