
/*
** Header file for inclusion with kspread_xml2latex.c
**
** SPDX-FileCopyrightText: 2003 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
**
*/

#ifndef __CALLIGRA_SHEETS_LATEX_PEN_H__
#define __CALLIGRA_SHEETS_LATEX_PEN_H__

#include "xmlparser.h"
#include <QColor>
#include <QString>
#include <QTextStream>

/***********************************************************************/
/* Class: Pen                                                          */
/***********************************************************************/

/**
 * This class describe a pen which is used to draw borders.
 */
class Pen : public XmlParser
{
    double _width;
    int _style;
    QColor _color;

public:
    /**
     * Constructors
     *
     * Creates a new instance of Format.
     */
    Pen();

    /*
     * Destructor
     *
     * Nothing to do
     */
    ~Pen() override = default;

    /**
     * getters
     */
    double getWidth() const
    {
        return _width;
    }
    int getStyle() const
    {
        return _style;
    }
    QColor getColor() const
    {
        return _color;
    }

    /**
     * setters
     */
    void setWidth(double w)
    {
        _width = w;
    }
    void setStyle(int s)
    {
        _style = s;
    }
    void setColor(QAnyStringView color)
    {
        _color = QColor::fromString(color);
    }

    /**
     * Helpful functions
     */

    /**
     * Get information from a markup tree (only param of a format).
     */
    virtual void analyze(const QDomNode);

    virtual void generate(QTextStream &);
};

#endif /* __CALLIGRA_SHEETS_LATEX_PEN_H__ */
