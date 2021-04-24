/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_CELL_STORAGE_P
#define KSPREAD_CELL_STORAGE_P

// Sheets
#include "Binding.h"
#include "Condition.h"
#include "Formula.h"
#include "Style.h"
#include "Validity.h"
#include "Value.h"

namespace Calligra
{
namespace Sheets
{

class CellStorageUndoData
{
public:
    bool isEmpty() const {
        if (!bindings.isEmpty())
            return false;
        if (!comments.isEmpty())
            return false;
        if (!conditions.isEmpty())
            return false;
        if (!databases.isEmpty())
            return false;
        if (!formulas.isEmpty())
            return false;
        if (!fusions.isEmpty())
            return false;
        if (!links.isEmpty())
            return false;
        if (!matrices.isEmpty())
            return false;
        if (!namedAreas.isEmpty())
            return false;
        if (!styles.isEmpty())
            return false;
        if (!userInputs.isEmpty())
            return false;
        if (!validities.isEmpty())
            return false;
        if (!values.isEmpty())
            return false;
        if (!richTexts.isEmpty())
            return false;
        return true;
    }

    QList< QPair<QRectF, Binding> >          bindings;
    QList< QPair<QRectF, QString> >          comments;
    QList< QPair<QRectF, Conditions> >       conditions;
    QList< QPair<QRectF, Database> >         databases;
    QVector< QPair<QPoint, Formula> >        formulas;
    QList< QPair<QRectF, bool> >             fusions;
    QVector< QPair<QPoint, QString> >        links;
    QList< QPair<QRectF, bool> >             matrices;
    QList< QPair<QRectF, QString> >          namedAreas;
    QList< QPair<QRectF, SharedSubStyle> >   styles;
    QVector< QPair<QPoint, QString> >        userInputs;
    QList< QPair<QRectF, Validity> >         validities;
    QVector< QPair<QPoint, Value> >          values;
    QVector< QPair<QPoint, QSharedPointer<QTextDocument> > > richTexts;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_CELL_STORAGE_P
