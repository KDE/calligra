/* This file is part of the KDE project
   Copyright 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2002-2004 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2003 Laurent Montel <montel@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 John Dailey <dailey@vt.edu>
   Copyright 1999-2003 David Faure <faure@kde.org>
   Copyright 1999-2001 Simon Hausmann <hausmann@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef CALLIGRA_SHEETS_CELL_TOOL_BASE
#define CALLIGRA_SHEETS_CELL_TOOL_BASE

#include <KoInteractionTool.h>

#include <sheets/Cell.h>
#include "sheets_common_export.h"
#include "Selection.h"

class KoColor;

namespace Calligra
{
namespace Sheets
{
class ExternalEditor;
class SheetView;
class CellEditorBase;

/**
 * Abstract tool providing actions acting on cell ranges.
 */
class CALLIGRA_SHEETS_COMMON_EXPORT CellToolBase : public KoInteractionTool
{
    Q_OBJECT

public:
    /**
     * The editor type.
     */
    enum Editor {
        EmbeddedEditor,  ///< the embedded editor appearing in a cell
        ExternalEditor   ///< the external editor located in the tool options
    };

    /**
     * Constructor.
     */
    explicit CellToolBase(KoCanvasBase *canvas);

    /**
     * Destructor.
     */
    ~CellToolBase() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void paintReferenceSelection(QPainter &painter, const QRectF &paintRect);
    void paintSelection(QPainter &painter, const QRectF &paintRect);

    void mousePressEvent(KoPointerEvent* event) override;
    void mouseMoveEvent(KoPointerEvent* event) override;
    void mouseReleaseEvent(KoPointerEvent* event) override;
    void mouseDoubleClickEvent(KoPointerEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void inputMethodEvent(QInputMethodEvent * event) override;

    Selection* selection() override = 0;

    void deleteSelection() override;

    virtual bool createEditor(bool clear = true, bool focus = true, bool captureArrows = false);
    virtual CellEditorBase* editor() const;

    /**
     * Sets the editor \p type, which had the focus at last.
     */
    void setLastEditorWithFocus(Editor type);

    /**
     * Sets an external editor to be associated with the internal editor of this tool.
     */
    void setExternalEditor(Calligra::Sheets::ExternalEditor* editor);

    /**
     * The shape offset in document coordinates.
     */
    virtual QPointF offset() const = 0;
    virtual QSizeF size() const = 0;

public Q_SLOTS:
    /**
     * Scrolls to the cell located at \p location.
     */
    virtual void scrollToCell(const QPoint &location);

    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    void deactivate() override;
    virtual void deleteEditor(bool saveChanges, bool expandMatrix = false);
    void populateWordCollection();

protected:
    void init();
    QList<QPointer<QWidget> > createOptionWidgets() override;
    void applyUserInput(const QString &userInput, bool expandMatrix = false);
    KoInteractionStrategy* createStrategy(KoPointerEvent* event) override;

    /**
     * The canvas scrolling offset in document coordinates.
     */
    virtual QPointF canvasOffset() const = 0;
    virtual double canvasOffsetX() const;
    virtual double canvasOffsetY() const;
    virtual double canvasWidth() const;

    virtual int maxCol() const = 0;
    virtual int maxRow() const = 0;
    virtual SheetView* sheetView(const Sheet* sheet) const = 0;

    QList<QAction*> popupMenuActionList() const;


protected Q_SLOTS:
    void selectionChanged(const Region&);
    void activeSheetChanged(Sheet*);
    void updateEditor();
    void focusEditorRequested();
    void documentReadWriteToggled(bool enable);
    void sheetProtectionToggled(bool enable);

    // -- cell style actions --
    void cellStyle();
    void setDefaultStyle();
    void styleDialog();
    void setStyle(const QString& name);
    void createStyleFromCell();
    // -- font actions --
    void bold(bool enable);
    void italic(bool enable);
    void underline(bool enable);
    void strikeOut(bool enable);
    void font(const QString& font);
    void fontSize(int size);
    void increaseFontSize();
    void decreaseFontSize();
    void changeTextColor(const KoColor &);
    // -- horizontal alignment actions --
    void alignLeft(bool enable);
    void alignRight(bool enable);
    void alignCenter(bool enable);
    // -- vertical alignment actions --
    void alignTop(bool enable);
    void alignMiddle(bool enable);
    void alignBottom(bool enable);
    // -- border actions --
    void borderLeft();
    void borderRight();
    void borderTop();
    void borderBottom();
    void borderAll();
    void borderRemove();
    void borderOutline();
    void borderColor(const KoColor &);
    // -- text layout actions --
    void wrapText(bool enable);
    void verticalText(bool enable);
    void increaseIndentation();
    void decreaseIndentation();
    void changeAngle();
    // -- value format actions --
    void percent(bool enable);
    void currency(bool enable);
    void increasePrecision();
    void decreasePrecision();
    // -- misc style attribute actions --
    void toUpperCase();
    void toLowerCase();
    void firstLetterToUpperCase();
    void changeBackgroundColor(const KoColor &);
    // -- cell merging actions --
    void mergeCells();
    void mergeCellsHorizontal();
    void mergeCellsVertical();
    void dissociateCells();
    // -- column & row actions --
    void resizeColumn();
    void insertColumn();
    void deleteColumn();
    void hideColumn();
    void showColumn();
    void slotShowColumnDialog();
    void equalizeColumn();
    void adjustColumn();
    void resizeRow();
    void insertRow();
    void deleteRow();
    void hideRow();
    void showRow();
    void slotShowRowDialog();
    void equalizeRow();
    void adjustRow();
    void adjust();
    // -- cell insert/remove actions --
    void insertCells();
    void deleteCells();
    // -- cell content actions --
    void clearAll();
    void clearContents();
    void comment();
    void clearComment();
    void conditional();
    void clearConditionalStyles();
    void insertHyperlink();
    void clearHyperlink();
    void validity();
    void clearValidity();
    // -- sorting/filtering action --
    void sort();
    void sortInc();
    void sortDec();
    void autoFilter();
    // -- fill actions --
    void fillLeft();
    void fillRight();
    void fillUp();
    void fillDown();
    void autoSum();
    // -- data insert actions --
    void insertSeries();
    void insertFormula();
    void insertSpecialChar();
    void specialChar(QChar character, const QString& fontName);
    void specialCharDialogClosed();
    void insertFromDatabase();
    void insertFromTextfile();
    void insertFromClipboard();
    void textToColumns();
    void sortList();
    void consolidate();
    void goalSeek();
    void subtotals();
    void pivot();
    void setAreaName();
    void namedAreaDialog();
    void formulaSelection(const QString& expression);
    // -- general editing actions --
    void edit();
    void cut() override;
    void copy() const override;
    bool paste() override;
    void specialPaste();
    void pasteWithInsertion();
    void selectAll();
    void find();
    void findNext();
    void findPrevious();
    void replace();
    void initFindReplace();
    Cell findNextCell();
    /**
     * Called by find/replace (findNext) when it found a match
     */
    void slotHighlight(const QString &text, int matchingIndex, int matchedLength);
    /**
     * Called when replacing text in a cell
     */
    void slotReplace(const QString &newText, int, int, int);
    Cell nextFindValidCell(int col, int row);
    // -- misc actions --
    void gotoCell();
    void spellCheck();
    void inspector();
    void qTableView();
    void sheetFormat();
    void listChoosePopupMenu();
    void listChooseItemSelected(QAction*);
    void documentSettingsDialog();
    void breakBeforeColumn(bool);
    void breakBeforeRow(bool);

private:
    Q_DISABLE_COPY(CellToolBase)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_TOOL_BASE
