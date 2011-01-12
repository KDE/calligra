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

#ifndef __LATEXPARSER_ENV_H__
#define __LATEXPARSER_ENV_H__

#include "element.h"

#include <QString>
#include <QList>

class Env: public Element
{
    QList<Element*> _children;
    QString _name;

public:
    Env();

    explicit Env(const char*);

    explicit Env(const QString&);

    ~Env();

    QString getName() const {
        return _name;
    }

    void setName(QString name) {
        _name = name;
    }
    void setChildren(QList<Element*>*);

    void print(int tab = 0);
};

#endif /* __LATEXPARSER_ENV_H__ */
