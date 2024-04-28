/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Ulrich Kuettler <ulrich.kuettler@gmx.de>
   SPDX-FileCopyrightText: 2006 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2006 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include "koformula_export.h"
#include <QString>

class BasicElement;

enum ElementType {
    Basic,
    Formula,
    Row,
    Identifier,
    Number,
    Operator,
    Space,
    Fraction,
    Table,
    TableRow,
    TableData,
    Under,
    Over,
    UnderOver,
    MultiScript,
    SupScript,
    SubScript,
    SubSupScript,
    Root,
    SquareRoot,
    Text,
    Style,
    Padded,
    Error,
    Fenced,
    Glyph,
    String,
    Enclose,
    Phantom,
    Action,
    Annotation,
    Unknown,
    Empty
};

/**
 * @short An implementation of the factory pattern to create element instances
 *
 * The creation of new BasicElement derived classes is an often done task. While
 * loading ElementFactory provides a very simple way to achieve an element by
 * passing its MathML name. Just use the static createElement() method.
 * While saving the elementName() method is used to map the ElementType's to the
 * according MathML names.
 *
 * @author Martin Pfeiffer
 */
class KOFORMULA_EXPORT ElementFactory
{
public:
    /// The default constructor
    ElementFactory();

    /**
     * Obtain new instances of elements by passing the MathML tag name
     * @param tagName The MathML tag name of the new element
     * @param parent The parent element of the newly created element
     * @return A pointer to the new BasicElement derived element
     */
    static BasicElement *createElement(const QString &tagName, BasicElement *parent);

    /**
     * Obtain the MathML name of a ElementType.
     * @param type The given ElementType to get the MathML name from
     * @return The MathML name as QString
     */
    static QString elementName(ElementType type);
};

#endif // ELEMENTFACTORY_H
