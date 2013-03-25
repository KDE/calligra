/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen-Wander Hiemstra <aw.hiemstra@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "CQCanvasBase.h"

class CQCanvasBase::Private
{
public:
    Private() : canvasController(0) { }
    
    CQCanvasController *canvasController;
    QString source;
};

CQCanvasBase::CQCanvasBase(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private)
{

}

CQCanvasBase::~CQCanvasBase()
{
    delete d;
}

CQCanvasController* CQCanvasBase::canvasController() const
{
    return d->canvasController;
}

QString CQCanvasBase::source() const
{
    return d->source;
}

void CQCanvasBase::setSource(const QString& source)
{
    if(source != d->source) {
        d->source = source;
        openFile(d->source);
        emit sourceChanged();
    }
}

void CQCanvasBase::setCanvasController(CQCanvasController* controller)
{
    if(d->canvasController != controller)
    {
        d->canvasController = controller;
        emit canvasControllerChanged();
    }
}
