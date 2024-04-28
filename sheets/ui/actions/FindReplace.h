/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_FIND_REPLACE
#define CALLIGRA_SHEETS_ACTION_FIND_REPLACE

#include "CellAction.h"

#include "core/Cell.h"
#include "dialogs/FindDialog.h"

class KFind;
class KReplace;
class KUndo2Command;

namespace Calligra
{
namespace Sheets
{

class FindReplaceAction : public CellAction
{
    Q_OBJECT
public:
    FindReplaceAction(Actions *actions);
    virtual ~FindReplaceAction();

    void executeReplace(Selection *selection, Sheet *sheet, QWidget *canvasWidget);
    void executeFindNext(Selection *selection, Sheet *sheet, QWidget *canvasWidget);
    void executeFindPrevious(Selection *selection, Sheet *sheet, QWidget *canvasWidget);
protected Q_SLOTS:
    void findNext();
    /**
     * Called by find/replace (findNext) when it found a match
     */
    void slotHighlight(const QString &text, int matchingIndex, int matchedLength);
    /**
     * Called when replacing text in a cell
     */
    void slotReplace(const QString &newText, int, int, int);

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    virtual bool enabledIfReadOnly() const override
    {
        return true;
    }
    virtual bool enabledIfProtected() const override
    {
        return true;
    }

    void initFindReplace();
    Cell findNextCell();
    Cell nextFindValidCell(int col, int row);
    void findPrevious();

    Selection *m_selection;

    // Find and Replace context. We remember the options and
    // the strings used previously.
    long m_findOptions;
    QStringList m_findStrings;
    QStringList m_replaceStrings;
    FindOption::searchTypeValue m_typeValue;
    FindOption::searchDirectionValue m_directionValue;
    // Current "find" operation
    KFind *m_find;
    KReplace *m_replace;
    KUndo2Command *m_replaceCommand;
    int m_findLeftColumn;
    int m_findRightColumn;
    int m_findTopRow;
    int m_findBottomRow;
    QPoint m_findStart;
    QPoint m_findPos;
    QPoint m_findEnd;

    Sheet *m_currentSheet;
    Sheet *m_firstSheet;
};

/**
 * The next three actions rely on a shared find object. We store all the relevant information on the FindReplaceAction action, that the others access.
 * This base class provides the necessary access methods.
 */

class FindAction : public CellAction
{
public:
    FindAction(Actions *actions, const QString &actionName);
    virtual ~FindAction();
    FindReplaceAction *findAction();

protected:
    FindReplaceAction *m_findAction;
};

class FindNext : public FindAction
{
    Q_OBJECT
public:
    FindNext(Actions *actions);
    virtual ~FindNext();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    virtual bool enabledIfReadOnly() const override
    {
        return true;
    }
    virtual bool enabledIfProtected() const override
    {
        return true;
    }
};

class FindPrevious : public FindAction
{
    Q_OBJECT
public:
    FindPrevious(Actions *actions);
    virtual ~FindPrevious();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;

    virtual bool enabledIfReadOnly() const override
    {
        return true;
    }
    virtual bool enabledIfProtected() const override
    {
        return true;
    }
};

class Replace : public FindAction
{
    Q_OBJECT
public:
    Replace(Actions *actions);
    virtual ~Replace();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;
    QAction *createAction() override;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_FIND_REPLACE
