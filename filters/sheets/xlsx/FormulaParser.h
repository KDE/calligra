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
#ifndef FORMULAPARSER_H
#define FORMULAPARSER_H

#include <QString>

class Cell;

namespace MSOOXML
{

/**
 * Generate and return the ODF formula for \p thisCell based on the formula in the
 * defined \p referencedCell . This is used for formula groups.
 */
QString convertFormulaReference(Cell *referencedCell, Cell *thisCell);
}

#endif // FORMULAPARSER_H
