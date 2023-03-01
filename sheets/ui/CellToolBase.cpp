// This file is part of the KDE project
// SPDX-FileCopyrightText: 2022-2023 Tomas Mecir <mecirt@gmail.com>
// SPDX-FileCopyrightText: 2006-2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-FileCopyrightText: 2005-2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
// SPDX-FileCopyrightText: 2002-2005 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 1999-2003 Laurent Montel <montel@kde.org>
// SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
// SPDX-FileCopyrightText: 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
// SPDX-FileCopyrightText: 2002-2003 John Dailey <dailey@vt.edu>
// SPDX-FileCopyrightText: 1999-2003 David Faure <faure@kde.org>
// SPDX-FileCopyrightText: 1999-2001 Simon Hausmann <hausmann@kde.org>
// SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "CellToolBase.h"
#include "CellToolBase_p.h"

// Sheets
#include "engine/CalculationSettings.h"
#include "engine/Damages.h"

#include "core/CellStorage.h"
#include "core/Database.h"
#include "core/DocBase.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "core/StyleManager.h"

#include "ActionOptionWidget.h"
#include "CellEditor.h"
#include "CellView.h"
#include "ExternalEditor.h"
#include "FilterPopup.h"
#include "inspector.h"
#include "SheetView.h"

// actions
#include "actions/Actions.h"
#include "actions/CellAction.h"

// commands
#include "commands/AutoFillCommand.h"
#include "commands/DataManipulators.h"

// dialogs
#include "dialogs/DatabaseDialog.h"
#include "dialogs/DocumentSettingsDialog.h"
#include "dialogs/ListDialog.h"

// strategies
#include "strategy/AutoFillStrategy.h"
#include "strategy/DragAndDropStrategy.h"
#include "strategy/HyperlinkStrategy.h"
#include "strategy/MergeStrategy.h"
#include "strategy/PasteStrategy.h"
#include "strategy/SelectionStrategy.h"

// Calligra
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoPointerEvent.h>
#include <KoShape.h>
#include <KoViewConverter.h>

// KF5
#include <kmessagebox.h>
#include <KSharedConfig>
#include <kstandardaction.h>

// Qt
#include <QApplication>
#include <QPainter>
#include <QDomDocument>
#ifndef QT_NO_SQL
// #include <QSqlDatabase>
#endif

#ifndef NDEBUG
#include <QTableView>
#include "core/SheetModel.h"
#endif

using namespace Calligra::Sheets;

CellToolBase::CellToolBase(KoCanvasBase* canvas)
        : KoInteractionTool(canvas)
        , d(new Private(this))
{
    d->cellEditor = 0;
    d->externalEditor = 0;
    d->formulaDialog = 0;
    d->initialized = false;
    d->lastEditorWithFocus = EmbeddedEditor;

    // Create the extra and ones with extended names for the context menu.
    d->createPopupMenuActions();
    
    // Create the actions.
    QAction* action = nullptr;

    // -- cell style actions --
    d->actions = new Actions(this);

    // -- data insert actions --

    action = new QAction(koIcon("insert-math-expression"), i18n("&Function..."), this);
    addAction("insertFormula", action);
    connect(action, &QAction::triggered, this, &CellToolBase::insertFormula);
    action->setToolTip(i18n("Insert math expression"));

#ifndef QT_NO_SQL
    action = new QAction(koIcon("network-server-database"), i18n("From &Database..."), this);
    action->setIconText(i18n("Database"));
    addAction("insertFromDatabase", action);
    connect(action, &QAction::triggered, this, &CellToolBase::insertFromDatabase);
    action->setToolTip(i18n("Insert data from a SQL database"));
#endif

    action = new QAction(i18n("Custom Lists..."), this);
    addAction("sortList", action);
    connect(action, &QAction::triggered, this, &CellToolBase::sortList);
    action->setToolTip(i18n("Create custom lists for sorting or autofill"));

    KSelectAction *selectAction = new KSelectAction(i18n("Formula Selection"), this);
    addAction("formulaSelection", selectAction);
    selectAction->setToolTip(i18n("Insert a function"));
    QStringList functionList = {"SUM", "AVERAGE", "IF", "COUNT", "MIN", "MAX", i18n("Others...")};
    selectAction->setItems(functionList);
    selectAction->setComboWidth(80);
    selectAction->setCurrentItem(0);
    connect(selectAction, QOverload<const QString &>::of(&KSelectAction::triggered), this, &CellToolBase::formulaSelection);

    // -- general editing actions --

    action = new QAction(koIcon("cell_edit"), i18n("Modify Cell"), this);
    addAction("editCell", action);
    action->setShortcuts(QList<QKeySequence>() << QKeySequence(Qt::CTRL + Qt::Key_M));
    connect(action, &QAction::triggered, this, &CellToolBase::edit);
    action->setToolTip(i18n("Edit the highlighted cell"));

    action = KStandardAction::selectAll(this, SLOT(selectAll()), this);
    action->setToolTip(i18n("Selects all cells in the current sheet"));
    addAction("selectAll", action);

    // -- misc actions --

    action = new QAction(koIconWanted("not used in UI, but devs might do, so nice to have", "inspector"), i18n("Run Inspector..."), this);
    addAction("inspector", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_I));
    connect(action, &QAction::triggered, this, &CellToolBase::inspector);

#ifndef NDEBUG
    action = new QAction(koIcon("table"), i18n("Show QTableView..."), this);
    addAction("qTableView", action);
    action->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(action, &QAction::triggered, this, &CellToolBase::qTableView);
#endif

    action = new QAction(koIcon("application-vnd.oasis.opendocument.spreadsheet"), i18n("Document Settings..."), this);
    addAction("documentSettingsDialog", action);
    connect(action, &QAction::triggered, this, &CellToolBase::documentSettingsDialog);
    action->setToolTip(i18n("Show document settings dialog"));

    // Editor actions:
    // Set up the permutation of the reference fixations action.
    action = new QAction(i18n("Permute reference fixation"), this);
    addAction("permuteFixation", action);
    action->setShortcut(Qt::Key_F4);
    // connect on creation of the embedded editor
    action->setIconText(i18n("Permute fixation"));
    action->setToolTip(i18n("Permute the fixation of the reference at the text cursor"));

    setTextMode(true);

}

CellToolBase::~CellToolBase()
{
    delete d->formulaDialog;
    delete d->cellEditor;
    delete d->actions;
    qDeleteAll(d->popupMenuActions);
    qDeleteAll(actions());
    delete d;
}

QList<QAction*> CellToolBase::popupMenuActionList() const
{
    return d->popupActionList();
}

void CellToolBase::paint(QPainter &painter, const KoViewConverter &viewConverter)
{
    KoShape::applyConversion(painter, viewConverter);
    painter.translate(offset()); // the table shape offset
    const QRectF paintRect = QRectF(QPointF(), size());

    /* paint the selection */
    d->paintReferenceSelection(painter, paintRect);
    d->paintSelection(painter, paintRect);
}

void CellToolBase::paintReferenceSelection(QPainter &painter, const QRectF &paintRect)
{
    d->paintReferenceSelection(painter, paintRect);
}

void CellToolBase::paintSelection(QPainter &painter, const QRectF &paintRect)
{
    d->paintSelection(painter, paintRect);
}

void CellToolBase::mousePressEvent(KoPointerEvent* event)
{
    KoInteractionTool::mousePressEvent(event);
}

void CellToolBase::mouseMoveEvent(KoPointerEvent* event)
{
    // Special handling for drag'n'drop.
    if (DragAndDropStrategy *const strategy = dynamic_cast<DragAndDropStrategy*>(currentStrategy())) {
        // FIXME Stefan: QDrag*Event and QDropEvent are not handled by KoInteractionTool YET:
        // Cancel the strategy after the drag has started.
        if (strategy->dragStarted()) {
            cancelCurrentStrategy();
        }
        KoInteractionTool::mouseMoveEvent(event);
        return;
    }
    // Indicators are not necessary if there's a strategy.
    if (currentStrategy()) {
        return KoInteractionTool::mouseMoveEvent(event);
    }

    Sheet *const sheet = selection()->activeSheet();

    // Get info about where the event occurred.
    QPointF position = event->point - offset(); // the shape offset, not the scrolling one.

    // Diagonal cursor, if the selection handle was hit.
    if (SelectionStrategy::hitTestReferenceSizeGrip(canvas(), selection(), position) ||
        SelectionStrategy::hitTestSelectionSizeGrip(canvas(), selection(), position)) {
        if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
            useCursor(Qt::SizeBDiagCursor);
        } else {
            useCursor(Qt::SizeFDiagCursor);
        }
        return KoInteractionTool::mouseMoveEvent(event);
    }

    // Hand cursor, if the selected area was hit.
    if (!selection()->referenceSelectionMode()) {
        const Region::ConstIterator end(selection()->constEnd());
        for (Region::ConstIterator it(selection()->constBegin()); it != end; ++it) {
            const QRect range = (*it)->rect();
            if (sheet->cellCoordinatesToDocument(range).contains(position)) {
                useCursor(Qt::PointingHandCursor);
                return KoInteractionTool::mouseMoveEvent(event);
            }
        }
    }

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    const int col = this->selection()->activeSheet()->leftColumn(position.x(), xpos);
    const int row = this->selection()->activeSheet()->topRow(position.y(), ypos);
    // Check boundaries.
    if (col < 1 || row < 1 || col > maxCol() || row > maxRow()) {
        debugSheetsUI << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        const Cell cell = Cell(selection()->activeSheet(), col, row).masterCell();
        SheetView* const sheetView = this->sheetView(selection()->activeSheet());

        QString url;
        const CellView& cellView = sheetView->cellView(col, row);
        if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
            url = cellView.testAnchor(sheetView, cell, cell.width() - position.x() + xpos, position.y() - ypos);
        } else {
            url = cellView.testAnchor(sheetView, cell, position.x() - xpos, position.y() - ypos);
        }
        if (!url.isEmpty()) {
            useCursor(Qt::PointingHandCursor);
            return KoInteractionTool::mouseMoveEvent(event);
        }
    }

    // Reset to normal cursor.
    useCursor(Qt::ArrowCursor);
    KoInteractionTool::mouseMoveEvent(event);
}

void CellToolBase::mouseReleaseEvent(KoPointerEvent* event)
{
    KoInteractionTool::mouseReleaseEvent(event);
    scrollToCell(selection()->cursor());
}

void CellToolBase::mouseDoubleClickEvent(KoPointerEvent* event)
{
    Q_UNUSED(event)
    cancelCurrentStrategy();
    scrollToCell(selection()->cursor());
    createEditor(false /* keep content */, true, true /*full editing*/);
}

void CellToolBase::keyPressEvent(QKeyEvent* event)
{
     Sheet * const sheet = selection()->activeSheet();
    if (!sheet) {
        return;
    }

    // Check for formatting key combination CTRL + ...
    // Qt::Key_Exclam, Qt::Key_At, Qt::Key_Ampersand, Qt::Key_Dollar
    // Qt::Key_Percent, Qt::Key_AsciiCircum, Qt::Key_NumberSign
    if (d->formatKeyPress(event)) {
        return;
    }

    // Don't handle the remaining special keys.
    if (event->modifiers() & (Qt::AltModifier | Qt::ControlModifier) &&
            (event->key() != Qt::Key_Down) &&
            (event->key() != Qt::Key_Up) &&
            (event->key() != Qt::Key_Right) &&
            (event->key() != Qt::Key_Left) &&
            (event->key() != Qt::Key_Home) &&
            (event->key() != Qt::Key_Enter) &&
            (event->key() != Qt::Key_Return)) {
        event->ignore(); // QKeyEvent
        return;
    }

#if 0
    // TODO move this to the contextMenuEvent of the view.
    // keyPressEvent() is not called with the contextMenuKey,
    // it's handled separately by Qt.
    if (event->key() == KGlobalSettings::contextMenuKey()) {
        int row = d->canvas->selection()->marker().y();
        int col = d->canvas->selection()->marker().x();
        QPointF p(sheet->columnPosition(col), sheet->rowPosition(row));
        d->canvas->view()->openPopupMenu(d->canvas->mapToGlobal(p.toPoint()));
    }
#endif
    switch (event->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        d->processEnterKey(event);
        return;
        break;
    case Qt::Key_Down:
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Tab: /* a tab behaves just like a right/left arrow */
    case Qt::Key_Backtab:  /* and so does Shift+Tab */
        if (event->modifiers() & Qt::ControlModifier) {
            if (!d->processControlArrowKey(event))
                return;
        } else {
            d->processArrowKey(event);
            return;
        }
        break;

    case Qt::Key_Escape:
        d->processEscapeKey(event);
        return;
        break;

    case Qt::Key_Home:
        if (!d->processHomeKey(event))
            return;
        break;

    case Qt::Key_End:
        if (!d->processEndKey(event))
            return;
        break;

    case Qt::Key_PageUp:  /* Page Up */
        if (!d->processPriorKey(event))
            return;
        break;

    case Qt::Key_PageDown:   /* Page Down */
        if (!d->processNextKey(event))
            return;
        break;

    case Qt::Key_Backspace:
    case Qt::Key_Delete:
        d->triggerAction("clearContents");
        event->accept();
    	break;

    case Qt::Key_F2:
	edit();
	break;
	
    default:
        d->processOtherKey(event);
        return;
        break;
    }
}

void CellToolBase::inputMethodEvent(QInputMethodEvent * event)
{
    // Send it to the embedded editor.
    if (editor()) {
        QApplication::sendEvent(editor()->widget(), event);
    }
}

void CellToolBase::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    Q_UNUSED(shapes);

    if (!d->initialized) {
        init();
        d->initialized = true;
    }

    useCursor(Qt::ArrowCursor);
    

    // paint the selection rectangle
    selection()->update();
    populateWordCollection();

    // Establish connections.
    connect(selection(), &Selection::changed,
            this, QOverload<const Region &>::of(&CellToolBase::selectionChanged));
    connect(selection(), &Selection::closeEditor,
            this, &CellToolBase::deleteEditor);
    connect(selection(), &Selection::modified,
            this, &CellToolBase::updateEditor);
    connect(selection(), &Selection::activeSheetChanged,
            this, &CellToolBase::activeSheetChanged);
    connect(selection(), &Selection::requestFocusEditor,
            this, &CellToolBase::focusEditorRequested);
    connect(selection(), &Selection::documentReadWriteToggled,
            this, &CellToolBase::documentReadWriteToggled);
    connect(selection(), &Selection::sheetProtectionToggled,
            this, &CellToolBase::sheetProtectionToggled);

    Map *map = selection()->activeSheet()->fullMap();
    connect(map, &MapBase::damagesFlushed, this, &CellToolBase::handleDamages);
}

void CellToolBase::deactivate()
{
    Selection *sel = selection();
    // Disconnect.
    if (sel) disconnect(sel, 0, this, 0);
    // close the cell editor
    deleteEditor(true); // save changes
    // clear the selection rectangle
    if (sel) sel->update();
}

void CellToolBase::addCellAction(CellAction *a)
{
    addAction(a->name(), a->action());
}

void CellToolBase::init()
{
    Sheet *sheet = selection()->activeSheet();
    StyleManager *manager = sheet->fullMap()->styleManager();
    connect(manager, &StyleManager::styleListChanged, selection(), &Selection::refreshSheetViews);

    d->actions->init();
}

QList<QPointer<QWidget> >  CellToolBase::createOptionWidgets()
{
    QList<QPointer<QWidget> > widgets;

    QString xmlName = QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligrasheets/CellToolOptionWidgets.xml");
    debugSheets << xmlName;
    if (xmlName.isEmpty()) {
        warnSheets << "couldn't find CellToolOptionWidgets.xml file";
        return widgets;
    }

    QFile f(xmlName);
    if (!f.open(QIODevice::ReadOnly)) {
        warnSheets << "couldn't open CellToolOptionWidgets.xml file";
        return widgets;
    }

    QDomDocument doc(QString::fromLatin1("optionWidgets"));
    QString errorMsg;
    int errorLine, errorCol;
    if (!doc.setContent(&f, &errorMsg, &errorLine, &errorCol)) {
        f.close();
        warnSheets << "couldn't parse CellToolOptionWidgets.xml file:" << errorMsg << "on line" << errorLine << "column" << errorCol;
        return widgets;
    }
    f.close();

    QDomNodeList widgetNodes = doc.elementsByTagName("optionWidget");
    for (int i = 0; i < widgetNodes.size(); ++i) {
        QDomElement e = widgetNodes.at(i).toElement();
        widgets.append(new ActionOptionWidget(this, e));
    }

    return widgets;
}

// TODO: while this event sends the offset-adjusted position, the subsequent handleMouseMove calls do not. This means that they all contain the offset adjustment themselves, which is a needless duplication. Should be improved.
KoInteractionStrategy* CellToolBase::createStrategy(KoPointerEvent* event)
{
    Sheet *const sheet = selection()->activeSheet();
    // Get info about where the event occurred.
    QPointF position = event->point - offset(); // the shape offset, not the scrolling one.

    // Autofilling or merging, if the selection handle was hit.
    if (SelectionStrategy::hitTestSelectionSizeGrip(canvas(), selection(), position)) {
        if (event->button() == Qt::LeftButton)
            return new AutoFillStrategy(this, position, event->modifiers());
        else if (event->button() == Qt::MiddleButton)
            return new MergeStrategy(this, position, event->modifiers());
    }

    // Pasting with the middle mouse button.
    if (event->button() == Qt::MiddleButton) {
        return new PasteStrategy(this, position, event->modifiers());
    }

    // Check, if the selected area was hit.
    bool hitSelection = false;
    Region::ConstIterator end = selection()->constEnd();
    for (Region::ConstIterator it = selection()->constBegin(); it != end; ++it) {
        const QRect range = (*it)->rect();
        if (sheet->cellCoordinatesToDocument(range).contains(position)) {
            // Context menu with the right mouse button.
            if (event->button() == Qt::RightButton) {
                // Setup the context menu.
                setPopupActionList(d->popupActionList());
                event->ignore();
                return nullptr; // Act directly; no further strategy needed.
            }
            hitSelection = true;
            break;
        }
    }

    // In which cell did the user click?
    qreal xpos;
    qreal ypos;
    const int col = sheet->leftColumn(position.x(), xpos);
    const int row = sheet->topRow(position.y(), ypos);
    // Check boundaries.
    if (col > maxCol() || row > maxRow()) {
        debugSheetsUI << "col or row is out of range:" << "col:" << col << " row:" << row;
    } else {
        // Context menu with the right mouse button.
        if (event->button() == Qt::RightButton) {
            selection()->initialize(QPoint(col, row), sheet);
            // Setup the context menu.
            setPopupActionList(d->popupActionList());
            event->ignore();
            return nullptr; // Act directly; no further strategy needed.
        } else {
            const Cell cell = Cell(sheet, col, row).masterCell();
            SheetView* const sheetView = this->sheetView(sheet);

            // Filter button hit.
            const double offsetX = canvas()->canvasController()->canvasOffsetX();
            const double offsetY = canvas()->canvasController()->canvasOffsetY();
            const QPointF p1 = QPointF(xpos, ypos) - offset(); // the shape offset, not the scrolling one.
            const QSizeF s1(cell.width(), cell.height());
            const QRectF cellRect = canvas()->viewConverter()->documentToView(QRectF(p1, s1));
            const QRect cellViewRect = cellRect.translated(offsetX, offsetY).toRect();
            if (sheetView->cellView(col, row).hitTestFilterButton(cell, cellViewRect, event->pos())) {
                Database database = cell.database();
                FilterPopup::showPopup(canvas()->canvasWidget(), cell, cellViewRect, &database);
                return nullptr; // Act directly; no further strategy needed.
            }

            // Hyperlink hit.
            QString url;
            const CellView& cellView = sheetView->cellView(col, row);
            if (selection()->activeSheet()->layoutDirection() == Qt::RightToLeft) {
                url = cellView.testAnchor(sheetView, cell, cell.width() - position.x() + xpos, position.y() - ypos);
            } else {
                url = cellView.testAnchor(sheetView, cell, position.x() - xpos, position.y() - ypos);
            }
            if (!url.isEmpty()) {
                return new HyperlinkStrategy(this, position,
                                             event->modifiers(), url, cellView.textRect());
            }
        }
    }

    // Do we want to drag and drop the selection?
    bool wantDragDrop = hitSelection;
    if (event->modifiers() & Qt::ControlModifier) wantDragDrop = false;   // no drag&drop if Ctrl is pressed
    QRect selRect = selection()->boundingRect();
    // no drag&drop if it's a single cell, assume they want a selection
    if ((selRect.width() <= 1) && (selRect.height() <= 1)) wantDragDrop = false;
    if (selection()->referenceSelectionMode()) wantDragDrop = false;
    if (wantDragDrop)
        return new DragAndDropStrategy(this, position, event->modifiers());

    return new SelectionStrategy(this, position, event->modifiers());
}

// This makes sure that the action buttons stay updated whenever we change anything.
void CellToolBase::handleDamages()
{
    updateActions();
}

void CellToolBase::selectionChanged(const Region& region)
{
    Q_UNUSED(region);
    if (!d->externalEditor) {
        return;
    }
    // Update the editor, if the reference selection is enabled.
    if (editor() && selection()->referenceSelectionMode()) {
        // First, update the formula expression. This will send a signal with
        // the new expression to the external editor, which does not have focus
        // yet (the canvas has). If it would have, it would also send a signal
        // to inform the embedded editor about a changed text.
        editor()->selectionChanged();
        // Focus the embedded or external editor after updating the expression.
        focusEditorRequested();
        return;
    }

    const Cell cell = Cell(selection()->activeSheet(), selection()->cursor());
    if (!cell) {
        return;
    }

    if (selection()->activeSheet()->isProtected()) {
        const Style style = cell.style();
        if (style.notProtected()) {
            if (selection()->isSingular()) {
                d->setProtectedActionsEnabled(true);
            } else { // more than one cell
                d->setProtectedActionsEnabled(false);
            }
        } else {
            d->setProtectedActionsEnabled(false);
        }
    }

    updateActions();
}

void CellToolBase::scrollToCell(const QPoint &location)
{
    Sheet *const sheet = selection()->activeSheet();

    // Adjust the maximum accessed column and row for the scrollbars.
    sheetView(sheet)->updateAccessedCellRange(location);

    // The cell geometry expanded by some pixels in each direction.
    const Cell cell = Cell(sheet, location).masterCell();
    const double xpos = sheet->columnPosition(cell.cellPosition().x());
    const double ypos = sheet->rowPosition(cell.cellPosition().y());
    const double pixelWidth = canvas()->viewConverter()->viewToDocumentX(1);
    const double pixelHeight = canvas()->viewConverter()->viewToDocumentY(1);
    QRectF rect(xpos, ypos, cell.width(), cell.height());
    rect.adjust(-2*pixelWidth, -2*pixelHeight, +2*pixelWidth, +2*pixelHeight);
    rect = rect & QRectF(QPointF(0.0, 0.0), sheet->documentSize());

    // Scroll to cell.
    canvas()->canvasController()->ensureVisible(canvas()->viewConverter()->documentToView(rect), true);
}

CellEditorBase* CellToolBase::editor() const
{
    return d->cellEditor;
}

void CellToolBase::setLastEditorWithFocus(Editor editor)
{
    d->lastEditorWithFocus = editor;
}

double CellToolBase::canvasOffsetX() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->canvasController()->canvasOffsetX());
}

double CellToolBase::canvasOffsetY() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->canvasController()->canvasOffsetY());
}

double CellToolBase::canvasWidth() const
{
    return canvas()->viewConverter()->viewToDocumentX(canvas()->canvasWidget()->width());
}

bool CellToolBase::createEditor(bool clear, bool focus, bool captureArrows)
{
    const Cell cell(selection()->activeSheet(), selection()->marker());
    if (selection()->activeSheet()->isProtected() && !cell.style().notProtected())
        return false;

    if (!editor()) {
        d->cellEditor = new CellEditor(this, d->wordCollection,canvas()->canvasWidget());
        d->cellEditor->setEditorFont(cell.style().font(), true, canvas()->viewConverter());
        connect(action("permuteFixation"), &QAction::triggered,
                d->cellEditor, &CellEditor::permuteFixation);

        if(d->externalEditor) {
            connect(d->cellEditor, &CellEditor::textModified,
                    d->externalEditor, &ExternalEditor::setText);
            connect(d->externalEditor, &ExternalEditor::textModified,
                    d->cellEditor, [this] (const QString &text) {
                        d->cellEditor->setText(text);
                    });
            d->externalEditor->applyAction()->setEnabled(true);
            d->externalEditor->cancelAction()->setEnabled(true);
        }
        
        double w = cell.width();
        double h = cell.height();
        double min_w = cell.width();
        double min_h = cell.height();

        double xpos = selection()->activeSheet()->columnPosition(selection()->marker().x());
        xpos += canvasOffsetX();
        Qt::LayoutDirection sheetDir = selection()->activeSheet()->layoutDirection();
        bool rtlText = cell.displayText().isRightToLeft();

        // if sheet and cell direction don't match, then the editor's location
        // needs to be shifted backwards so that it's right above the cell's text
        if (w > 0 && ((sheetDir == Qt::RightToLeft && !rtlText) ||
                      (sheetDir == Qt::LeftToRight && rtlText)))
            xpos -= w - min_w;

        // paint editor above correct cell if sheet direction is RTL
        if (sheetDir == Qt::RightToLeft) {
            double dwidth = canvasWidth();
            double w2 = qMax(w, min_w);
            xpos = dwidth - w2 - xpos;
        }
        double ypos = selection()->activeSheet()->rowPosition(selection()->marker().y());
        ypos += canvasOffsetY();

        // Setup the editor's palette.
        const Style style = cell.effectiveStyle();
        QPalette editorPalette(d->cellEditor->palette());
        QColor color = style.fontColor();
        if (!color.isValid())
            color = canvas()->canvasWidget()->palette().text().color();
        editorPalette.setColor(QPalette::Text, color);
        color = style.backgroundColor();
        if (!color.isValid())
            color = editorPalette.base().color();
        editorPalette.setColor(QPalette::Background, color);
        d->cellEditor->setPalette(editorPalette);

        // apply (table shape) offset
        xpos += offset().x();
        ypos += offset().y();

        const QRectF rect(xpos + 0.5, ypos + 0.5, w - 0.5, h - 0.5); //needed to circumvent rounding issue with height/width
        const QRectF zoomedRect = canvas()->viewConverter()->documentToView(rect);
        d->cellEditor->setGeometry(zoomedRect.toRect().adjusted(1, 1, -1, -1));
        d->cellEditor->setMinimumSize(QSize((int)canvas()->viewConverter()->documentToViewX(min_w) - 1,
                                       (int)canvas()->viewConverter()->documentToViewY(min_h) - 1));
        d->cellEditor->show();

        // Laurent 2001-12-05
        // Don't add focus when we create a new editor and
        // we select text in edit widget otherwise we don't delete
        // selected text.
        if (focus)
            d->cellEditor->setFocus();

        // clear the selection rectangle
        selection()->update();
    }

    d->cellEditor->setCaptureArrowKeys(captureArrows);

    if (!clear && !cell.isNull()) {
        d->cellEditor->setText(cell.userInput());
        // place cursor at the end
        int pos = d->cellEditor->toPlainText().length();
        d->cellEditor->setCursorPosition(pos);
        if (d->externalEditor) d->externalEditor->setCursorPosition(pos);
    }
    return true;
}

void CellToolBase::populateWordCollection()
{
  const CellStorage* cellstore=selection()->activeSheet()->fullCellStorage();
  int lastrow=cellstore->rows();
  int lastcolumn=cellstore->columns();
  if( lastrow < 2000 && lastcolumn < 20) {
  for (int j=1 ; j <= lastcolumn ; j++) {
    for (int i=1; i<=lastrow ; i++) {
      Value val=Cell( selection()->activeSheet(), j, i).value();
      if(val.isString()) {
          QString value = val.asString();
	if(!d->wordCollection.values(j).contains(value)){
	    d->wordCollection.insertMulti(j, value);
	  }
      }	 
    }
  }
  }
}

void CellToolBase::deleteEditor(bool saveChanges, bool expandMatrix)
{
    if (!d->cellEditor) {
        return;
    }
    const QString userInput = d->cellEditor->toPlainText();
    d->cellEditor->hide();
    // Delete the cell editor first and after that update the document.
    // That means we get a synchronous repaint after the cell editor
    // widget is gone. Otherwise we may get painting errors.
    delete d->cellEditor;
    d->cellEditor = 0;

    delete d->formulaDialog;
    d->formulaDialog = 0;

    if (saveChanges) {
        applyUserInput(userInput, expandMatrix);
    } else {
        selection()->update();
    }
    if (d->externalEditor) {
        d->externalEditor->applyAction()->setEnabled(false);
        d->externalEditor->cancelAction()->setEnabled(false);
    }
    canvas()->canvasWidget()->setFocus();
}

void CellToolBase::activeSheetChanged(Sheet* sheet)
{
#ifdef NDEBUG
    Q_UNUSED(sheet);
#else
    Q_ASSERT(selection()->activeSheet() == sheet);
#endif
    populateWordCollection();
    if (!selection()->referenceSelectionMode()) {
        return;
    }
    if (editor()) {
        if (selection()->originSheet() != selection()->activeSheet()) {
            editor()->widget()->hide();
        } else {
            editor()->widget()->show();
        }
    }
    focusEditorRequested();
}

void CellToolBase::updateEditor()
{
    if (!d->externalEditor) {
        return;
    }
    const Cell cell = Cell(selection()->activeSheet(), selection()->cursor());
    if (!cell) {
        return;
    }
    d->updateEditor(cell);
}

void CellToolBase::focusEditorRequested()
{
    // Nothing to do, if not in editing mode.
    if (!editor()) {
        return;
    }
    // If we are in editing mode, we redirect the focus to the CellEditor or ExternalEditor.
    // This screws up <Tab> though (David)
    if (selection()->originSheet() != selection()->activeSheet()) {
        // Always focus the external editor, if not on the origin sheet.
        if (d->externalEditor) d->externalEditor->setFocus();
    } else {
        // Focus the last active editor, if on the origin sheet.
        if (d->lastEditorWithFocus == EmbeddedEditor) {
            editor()->widget()->setFocus();
        } else {
            if (d->externalEditor) d->externalEditor->setFocus();
        }
    }
}

void CellToolBase::applyUserInput(const QString &userInput, bool expandMatrix)
{
    QString text = userInput;
    if (!text.isEmpty() && text.at(0) == '=') {
        //a formula
        int openParenthese = text.count('(');
        int closeParenthese = text.count(')');
        int diff = qAbs(openParenthese - closeParenthese);
        if (openParenthese > closeParenthese) {
            for (int i = 0; i < diff; ++i) {
                text += ')';
            }
        }
    }
    DataManipulator* command = new DataManipulator();
    command->setSheet(selection()->activeSheet());
    command->setValue(Value(text));
    command->setParsing(true);
    command->setExpandMatrix(expandMatrix);
    command->add(expandMatrix ? *selection() : Region(selection()->cursor(), selection()->activeSheet()));
    command->execute(canvas());

    if (expandMatrix && selection()->isSingular())
        selection()->initialize(*command);
}

void CellToolBase::documentReadWriteToggled(bool readWrite)
{
    d->setProtectedActionsEnabled(readWrite);

    updateActions();
}

void CellToolBase::sheetProtectionToggled(bool protect)
{
    d->setProtectedActionsEnabled(!protect);

    updateActions();
}

void CellToolBase::updateActions()
{
    Sheet *sheet = selection()->activeSheet();
    bool readWrite = sheet->fullMap()->doc()->isReadWrite();
    const Cell cell = Cell(sheet, selection()->cursor());
    d->actions->updateOnChange(readWrite, selection(), cell);
}

void CellToolBase::triggerAction(const QString &name)
{
    CellAction *a = d->actions->cellAction(name);
    if (a)
        a->trigger();
    else
        KMessageBox::sorry(canvas()->canvasWidget(), i18n("Unable to locate action %1", name));
}

void CellToolBase::insertFormula()
{
    if (! d->formulaDialog) {
        if (! createEditor())
            return;
        d->formulaDialog = new FormulaDialog(canvas()->canvasWidget(), selection(), editor());
    }
    d->formulaDialog->show(); // dialog deletes itself later
}

void CellToolBase::insertFromDatabase()
{
#ifndef QT_NO_SQL
    selection()->emitCloseEditor(true);

    QStringList str = QSqlDatabase::drivers();
    if (str.isEmpty()) {
        KMessageBox::error(canvas()->canvasWidget(), i18n("No database drivers available. To use this feature you need "
                           "to install the necessary Qt database drivers."));
        return;
    }

    QPointer<DatabaseDialog> dialog = new DatabaseDialog(canvas()->canvasWidget(), selection());
    dialog->exec();
    delete dialog;
#endif
}

void CellToolBase::sortList()
{
    QPointer<ListDialog> dialog = new ListDialog(canvas()->canvasWidget());
    Map *map = selection()->activeSheet()->fullMap();
    ApplicationSettings *sett = map->applicationSettings();
    Localization *locale = map->calculationSettings()->locale();
    dialog->setCustomLists(sett->sortingList(), locale);
    if (dialog->exec() && dialog->changed()) {
        QStringList result = dialog->customLists();
        auto config = KSharedConfig::openConfig();
        config->group("Parameters").writeEntry("Other list", result);

        // TODO do this better
        delete(AutoFillCommand::other);
        AutoFillCommand::other = nullptr;
    }
    sett->setSortingList(dialog->customLists());
    delete dialog;
}

void CellToolBase::formulaSelection(const QString& expression)
{
    if (expression == i18n("Others...")) {
        insertFormula();
        return;
    }

    createEditor();
    FormulaDialog* dialog = new FormulaDialog(canvas()->canvasWidget(), selection(), editor(), expression);
    dialog->show(); // dialog deletes itself later
}

void CellToolBase::edit()
{
    // Not yet in edit mode?
    if (!editor()) {
        createEditor(false /* keep content */, true, true /*capture arrow keys*/);
        return;
    }

    // If the editor doesn't allow cursor movement, enable it now (enters real editing mode)
    if (!editor()->captureArrowKeys()) {
        editor()->setCaptureArrowKeys(true);
        return;
    }

    // Switch focus.
    if (editor()->widget()->hasFocus()) {
        if (d->externalEditor) d->externalEditor->setFocus();
    } else {
        editor()->widget()->setFocus();
    }
}

void CellToolBase::deleteSelection()
{
    d->triggerAction("clearContents");
}

void CellToolBase::selectAll()
{
    selection()->selectAll();
}

void CellToolBase::inspector()
{
    // useful to inspect objects
    Cell cell(selection()->activeSheet(), selection()->marker());
    QPointer<Calligra::Sheets::Inspector> ins = new Calligra::Sheets::Inspector(cell);
    ins->exec();
    delete ins;
}

void CellToolBase::qTableView()
{
#ifndef NDEBUG
    QPointer<KoDialog> dialog = new KoDialog(canvas()->canvasWidget());
    QTableView* const view = new QTableView(dialog);
    SheetModel* const model = new SheetModel(selection()->activeSheet());
    view->setModel(model);
    dialog->setCaption("Read{Only,Write}TableModel Test");
    dialog->setMainWidget(view);
    dialog->exec();
    delete dialog;
    delete model;
#endif
}

void CellToolBase::documentSettingsDialog()
{
    QPointer<DocumentSettingsDialog> dialog = new DocumentSettingsDialog(selection(), canvas()->canvasWidget());
    dialog->exec();
    delete dialog;
}

void CellToolBase::setExternalEditor(Calligra::Sheets::ExternalEditor *editor)
{
    d->externalEditor = editor;
}
