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

#include "latexparser.h"

#include <fstream.h>
#include <iostream.h>
#include <QList>

//extern void yylex();
extern QList<Element*>* _root;
extern bool texparse(QString);

LatexParser::LatexParser()
{
}

LatexParser::LatexParser(const QString& fileIn)
{
    _filename = fileIn;
}

LatexParser::~LatexParser()
{
}

QList<Element*>* LatexParser::parse()
{
    texparse(_filename);
    return _root;
}
