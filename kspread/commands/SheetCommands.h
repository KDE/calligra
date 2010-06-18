/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_SHEET_COMMANDS
#define KSPREAD_SHEET_COMMANDS

#include <QString>
#include <QUndoCommand>

/**
 * The KSpread namespace.
 */
namespace KSpread
{
class Map;
class Sheet;

/**
 * \ingroup Commands
 * \brief Renames a sheet.
 */
class RenameSheetCommand : public QUndoCommand
{
public:
    RenameSheetCommand(Sheet* sheet, const QString &name);

    virtual void redo();
    virtual void undo();

protected:
    Sheet* sheet;
    QString oldName;
    QString newName;
};


/**
 * \ingroup Commands
 * \brief Hides a sheet.
 */
class HideSheetCommand : public QUndoCommand
{
public:
    explicit HideSheetCommand(Sheet* sheet);

    virtual void redo();
    virtual void undo();

protected:
    Map* map;
    QString sheetName;
};


/**
 * \ingroup Commands
 * \brief Shows a hidden sheet.
 */
class ShowSheetCommand : public QUndoCommand
{
public:
    explicit ShowSheetCommand(Sheet* sheet, QUndoCommand* parent = 0);

    virtual void redo();
    virtual void undo();

protected:
    Map* map;
    QString sheetName;
};


/**
 * \ingroup Commands
 * \brief Adds a sheet.
 */
class AddSheetCommand : public QUndoCommand
{
public:
    explicit AddSheetCommand(Sheet* sheet);

    virtual void redo();
    virtual void undo();

protected:
    Sheet*  m_sheet;
    bool    m_firstrun;
};


/**
 * \ingroup Commands
 * \brief Duplicates a sheet.
 */
class DuplicateSheetCommand : public QUndoCommand
{
public:
    explicit DuplicateSheetCommand();

    void setSheet(Sheet* sheet);

    virtual void redo();
    virtual void undo();

protected:
    Sheet* m_oldSheet;
    Sheet* m_newSheet;
    bool m_firstrun;
};


/**
 * \ingroup Commands
 * \brief Removes a sheet.
 */
class RemoveSheetCommand : public QUndoCommand
{
public:
    explicit RemoveSheetCommand(Sheet* sheet);

    virtual void redo();
    virtual void undo();

protected:
    Sheet* sheet;
    Map* map;
};


/**
 * \ingroup Commands
 * \brief Changes sheet properties.
 */
class SheetPropertiesCommand : public QUndoCommand
{
public:
    SheetPropertiesCommand(Sheet* sheet);
    void setLayoutDirection(Qt::LayoutDirection direction);
    void setAutoCalculationEnabled(bool b);
    void setShowGrid(bool b);
    void setShowPageBorders(bool b);
    void setShowFormula(bool b);
    void setHideZero(bool b);
    void setShowFormulaIndicator(bool b);
    void setShowCommentIndicator(bool b);
    void setColumnAsNumber(bool b);
    void setLcMode(bool b);
    void setCapitalizeFirstLetter(bool b);

    virtual void redo();
    virtual void undo();

protected:
    Sheet* sheet;
    Map* map;
    Qt::LayoutDirection oldDirection, newDirection;
    bool oldAutoCalc, newAutoCalc;
    bool oldShowGrid, newShowGrid;
    bool oldShowPageBorders, newShowPageBorders;
    bool oldShowFormula, newShowFormula;
    bool oldHideZero, newHideZero;
    bool oldShowFormulaIndicator, newShowFormulaIndicator;
    bool oldShowCommentIndicator, newShowCommentIndicator;
    bool oldColumnAsNumber, newColumnAsNumber;
    bool oldLcMode, newLcMode;
    bool oldCapitalizeFirstLetter, newCapitalizeFirstLetter;
};

} // namespace KSpread

#endif // KSPREAD_SHEET_COMMANDS
