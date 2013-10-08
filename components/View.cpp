/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "View.h"

using namespace Calligra::Components;

class View::Private
{
public:
    Private() : document{nullptr}
    { }

    Document* document;
};

View::View(QQuickItem* parent)
    : QQuickItem{parent}, d{new Private}
{

}

View::~View()
{
    delete d;
}

Document* View::document() const
{
    return d->document;
}

void View::setDocument(Document* newDocument)
{
    if(newDocument != d->document) {
        d->document = newDocument;
        emit documentChanged();
    }
}
