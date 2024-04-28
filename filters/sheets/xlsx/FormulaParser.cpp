/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Sebastian Sauer <sebsauer@kdab.com>
 * SPDX-FileCopyrightText: 2009-2010 Nokia Corporation and /or its subsidiary(-ies).
 * SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#include "FormulaParser.h"
#include "XlsxXmlWorksheetReader_p.h"

#include <sheets/engine/Util.h>

QString MSOOXML::convertFormulaReference(Cell *referencedCell, Cell *thisCell)
{
    if (!referencedCell->formula)
        return QString();
    Q_ASSERT(!referencedCell->formula->isShared());
    QString formula = static_cast<FormulaImpl *>(referencedCell->formula)->m_formula;
    return Calligra::Sheets::Util::adjustFormulaReference(formula, referencedCell->row, referencedCell->column, thisCell->row, thisCell->column);
}
