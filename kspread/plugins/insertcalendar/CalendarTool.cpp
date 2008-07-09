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

// Local
#include "CalendarTool.h"

#include "ui_CalendarToolWidget.h"

#include <KGenericFactory>

using namespace KSpread;

class CalendarTool::Private
{
public:
    Ui::CalendarToolWidget options;
};


CalendarTool::CalendarTool(KoCanvasBase* canvas)
    : DefaultTool(canvas)
    , d(new Private)
{
    setObjectName("CalendarTool");
/*
    KAction* importAction = new KAction(KIcon("document-import"), i18n("Import OpenDocument Spreadsheet File"), this);
    importAction->setIconText(i18n("Import"));
    addAction("import", importAction);
    connect(importAction, SIGNAL(triggered()), this, SLOT(importDocument()));*/
}

CalendarTool::~CalendarTool()
{
    delete d;
}

void CalendarTool::activate(bool temporary)
{
    DefaultTool::activate(temporary);
}

void CalendarTool::deactivate()
{
    DefaultTool::deactivate();
}

QWidget* CalendarTool::createOptionWidget()
{
    QWidget* optionWidget = new QWidget(m_canvas->canvasWidget());
    d->options.setupUi(optionWidget);
    return optionWidget;
}

#include "CalendarTool.moc"
