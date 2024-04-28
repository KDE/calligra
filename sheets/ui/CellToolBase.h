// This file is part of the KDE project
// SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>
// SPDX-FileCopyrightText: 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
// SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 1999-2003 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
// SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
// SPDX-FileCopyrightText: 2002-2003 John Dailey <dailey@vt.edu>
// SPDX-FileCopyrightText: 1999-2003 David Faure <faure@kde.org>
// SPDX-FileCopyrightText: 1999-2001 Simon Hausmann <hausmann@kde.org>
// SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef CALLIGRA_SHEETS_CELL_TOOL_BASE
#define CALLIGRA_SHEETS_CELL_TOOL_BASE

#include <KoInteractionTool.h>

#include "Selection.h"
#include "core/ApplicationSettings.h"
#include "core/Cell.h"
#include "sheets_ui_export.h"

namespace Calligra
{
namespace Sheets
{

class Cell;
class Region;
class Sheet;
class ExternalEditor;
class Selection;
class SheetView;
class CellEditorBase;
class CellAction;

/**
 * Abstract tool providing actions acting on cell ranges.
 */
class CALLIGRA_SHEETS_UI_EXPORT CellToolBase : public KoInteractionTool
{
    Q_OBJECT

public:
    /**
     * The editor type.
     */
    enum Editor {
        EmbeddedEditor, ///< the embedded editor appearing in a cell
        ExternalEditor ///< the external editor located in the tool options
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

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void inputMethodEvent(QInputMethodEvent *event) override;

    Selection *selection() override = 0;

    void deleteSelection() override;

    virtual bool createEditor(bool clear = true, bool focus = true, bool captureArrows = false);
    virtual CellEditorBase *editor() const;

    /**
     * Sets the editor \p type, which had the focus at last.
     */
    void setLastEditorWithFocus(Editor type);

    /**
     * Sets an external editor to be associated with the internal editor of this tool.
     */
    void setExternalEditor(Calligra::Sheets::ExternalEditor *editor);

    /** Wrapper around addAction, which is protected (in parent). */

    void addCellAction(CellAction *a);

    virtual SheetView *sheetView(Sheet *sheet) const = 0;

    /** Update all the actions' enabled/disabled status. */
    void updateActions();

    /** Trigger an action. */
    void triggerAction(const QString &name);

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

    void edit();
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;
    virtual void deleteEditor(bool saveChanges, bool expandMatrix = false);
    void populateWordCollection();

protected:
    void init();
    QList<QPointer<QWidget>> createOptionWidgets() override;
    void applyUserInput(const QString &userInput, bool expandMatrix = false);
    KoInteractionStrategy *createStrategy(KoPointerEvent *event) override;

    /**
     * The canvas scrolling offset in document coordinates.
     */
    virtual QPointF canvasOffset() const = 0;
    virtual double canvasOffsetX() const;
    virtual double canvasOffsetY() const;
    virtual double canvasWidth() const;

    virtual int maxCol() const = 0;
    virtual int maxRow() const = 0;

    QList<QAction *> popupMenuActionList() const;

protected Q_SLOTS:
    void selectionChanged(const Region &);
    void activeSheetChanged(Sheet *);
    void updateEditor();
    void focusEditorRequested();
    void documentReadWriteToggled(bool enable);
    void sheetProtectionToggled(bool enable);
    void handleDamages();

private:
    Q_DISABLE_COPY(CellToolBase)

    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CELL_TOOL_BASE
