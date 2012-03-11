/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Sebastian Sauer <sebsauer@kdab.com>
 * Copyright (C) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * Contact: Suresh Chande suresh.chande@nokia.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */
#include "FormulaParser.h"
#include "XlsxXmlWorksheetReader_p.h"

#include <sheets/Util.h>

QString MSOOXML::convertFormulaReference(Cell* referencedCell, Cell* thisCell)
{
    if (!referencedCell->formula)
        return QString();
    Q_ASSERT(!referencedCell->formula->isShared());
    QString formula = static_cast<FormulaImpl*>(referencedCell->formula)->m_formula;
    return Calligra::Sheets::Util::adjustFormulaReference(formula, referencedCell->row, referencedCell->column, thisCell->row, thisCell->column);
}
