/* This file is part of the KDE project
 * Copyright (C) 2003 Robert JACOLIN <rjacolin@ifrance.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "env.h"
#include <iostream.h>
#include <QList>

Env::Env()
{
    setType(Element::LATEX_ENV);
}

Env::Env(const char* command)
{
    setType(Element::LATEX_ENV);
    /* Parse the command name */
    QString pattern = QString(command);
    int pos = pattern.find("{");
    /* Remove \begin{ at the beginning and the } at the end. */
    if (pos != -1)
        _name = pattern.mid(pos + 1, pattern.length() - pos - 2);
    _name = _name.trimmed();
}

Env::Env(const QString& name) : _name(name)
{
    setType(Element::LATEX_ENV);
    _name = _name.trimmed();
}

Env::~Env()
{
}

void Env::setChildren(QList<Element*>* children)
{
    if (children != NULL)
        _children = *children;
}

void Env::print(int tab)
{
    cout << "ENV " << qPrintable(getName()) << "{" << endl;

    Element* elt;
    for (elt = _children.first(); elt; elt = _children.next()) {
        elt->print(tab);
    }
    cout << "}" << endl;
}
