/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULADATA_H
#define FORMULADATA_H

#include "FormulaElement.h"
#include "KoFormulaShape.h"
#include "KoShapeSavingContext.h"
#include "koformula_export.h"
#include <QObject>
#include <kundo2command.h>

class FormulaCommand;

/**
 * This is a QObject wrapper around a formulaElement, which allows to communicate
 * between tool, cursor and shape
 */
class KOFORMULA_EXPORT FormulaData : public QObject
{
    Q_OBJECT
public:
    explicit FormulaData(FormulaElement *element);

    ~FormulaData() override;

    /// @return formulaElement that represents the data
    FormulaElement *formulaElement() const;

    /// emit a dataChanged signal
    void notifyDataChange(FormulaCommand *command, bool undo);
    void setFormulaElement(FormulaElement *element);

Q_SIGNALS:
    void dataChanged(FormulaCommand *element, bool undo);

public Q_SLOTS:
    /// only for debugging
    void writeElementTree();

    void saveMathML(KoShapeSavingContext &context);

private:
    FormulaElement *m_element;
};

#endif // FORMULADATA_H
