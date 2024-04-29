/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_CLEAR
#define CALLIGRA_SHEETS_ACTION_CLEAR

#include "CellAction.h"

#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "ui/commands/DataManipulators.h"

#include <QMap>

namespace Calligra
{
namespace Sheets
{

class ClearAll : public CellAction
{
    Q_OBJECT
public:
    ClearAll(Actions *actions);
    virtual ~ClearAll();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

class ClearContents : public CellAction
{
    Q_OBJECT
public:
    ClearContents(Actions *actions);
    virtual ~ClearContents();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
};

/**
 * \ingroup Commands
 * \brief Deletes cell contents and column/row styles.
 */
class CALLIGRA_SHEETS_UI_EXPORT DeleteCommand : public AbstractDataManipulator
{
public:
    /**
     * Constructor.
     */
    explicit DeleteCommand(KUndo2Command *parent = nullptr);

    /**
     * Destructor.
     */
    ~DeleteCommand() override;

    enum Mode {
        Everything, ///< Delete also column and row formats.
        OnlyCells ///< Delete only cell contents, styles, etc.
    };
    void setMode(Mode mode);

protected:
    /**
     * Processes \p element , a Region::Point or a Region::Range .
     * Invoked by performCommands() .
     */
    bool process(Element *element) override;

    // dummy
    Value newValue(Element *, int, int, bool *, Format::Type *) override
    {
        return Value();
    }

    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

protected:
    QMap<int, ColFormat> m_columnFormats;
    QMap<int, RowFormat> m_rowFormats;
    Mode m_mode;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_CLEAR
