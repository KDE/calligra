/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FIXEDELEMENT_H
#define FIXEDELEMENT_H

#include "BasicElement.h"
#include "koformula_export.h"
#include <KoXmlReader.h>

class FormulaCursor;
class QPainterPath;
class RowElement;
/**
 * @short Abstract Base Class for MathML elements with fixed number of children
 *
 */

class KOFORMULA_EXPORT FixedElement : public BasicElement
{
public:
    /// The standard constructor
    explicit FixedElement(BasicElement *parent = nullptr);

    /// The standard destructor
    ~FixedElement() override;

    bool acceptCursor(const FormulaCursor &cursor) override;

    BasicElement *elementBefore(int position) const override;

    BasicElement *elementAfter(int position) const override;

    virtual BasicElement *elementNext(int position) const;

    QLineF cursorLine(int position) const override;

    bool loadElement(KoXmlElement &tmp, RowElement **child);

    QPainterPath selectionRegion(const int pos1, const int pos2) const override;

    /// inherited from BasicElement
    int positionOfChild(BasicElement *child) const override;

    int endPosition() const override;

protected:
    bool moveHorSituation(FormulaCursor &newcursor, FormulaCursor &oldcursor, int pos1, int pos2);
    bool moveVertSituation(FormulaCursor &newcursor, FormulaCursor &oldcursor, int pos1, int pos2);
    bool moveSingleSituation(FormulaCursor &newcursor, FormulaCursor &oldcursor, int pos);
};

#endif // ROWELEMENT_H
