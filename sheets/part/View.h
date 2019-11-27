/* This file is part of the KDE project
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef CALLIGRA_SHEETS_VIEW
#define CALLIGRA_SHEETS_VIEW

#include <QList>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QPrinter>

#include <QPointF>
#include <KoView.h>
#include <KoZoomMode.h>
#include <KoPart.h>
#include "sheets_common_export.h"

class QKeyEvent;
class QScrollBar;

class KoCanvasController;
class KoZoomHandler;

namespace Calligra
{
namespace Sheets
{
class Cell;
class ColumnHeaderWidget;
class Damage;
class Sheet;
class Canvas;
class Doc;
class Region;
class Selection;
class SheetView;
class RowHeaderWidget;
class View;
class TabBar;

/**
 * @class View
 * @brief The View class displays a Calligra Sheets document.
 * View is used to display a spreadsheet document and provide
 * the interface for the user to perform editing and data analysis.
 *
 * A view consists of several parts:
 *  \li canvas to display cells in a sheet
 *  \li column header to show columns
 *  \li row header to show rows
 *  \li corner button to select all cells
 *  \li horizontal and vertical scrollbars for navigation
 *  \li tab bar to select active worksheet
 *  \li status bar to show status messages
 *
 *
 * KPART_TODO: when we have mdi again, we need to have an equivalent of guiActivateEvent
 *   that sets the initalPosition() and calls calcStatusBarOp().
 */
class CALLIGRA_SHEETS_COMMON_EXPORT View : public KoView
{
    Q_OBJECT

public:
    /** Creates a new view displaying \p document and with \p parent as parent. */
    View(KoPart *part, QWidget *parent, Doc *document);

    /** Destroys the view */
    ~View() override;

    /** \return the document this view displays. */
    Doc* doc() const;

    /** \return the canvas of the view */
    Canvas* canvasWidget() const;

    /** \return the canvas controller of the view */
    KoCanvasController* canvasController() const;

    /** \return the column header */
    ColumnHeaderWidget* columnHeader() const;

    /** \return the row header */
    RowHeaderWidget* rowHeader() const;

    /** \return the horizontal scrollbar */
    QScrollBar* horzScrollBar() const;

    /** \return the vertical scrollbar */
    QScrollBar* vertScrollBar() const;

    /** \return the tab bar */
    TabBar* tabBar() const;

    /** \return the zoom handler */
    KoZoomHandler* zoomHandler() const;

    /** \return the sheet, that is currently displayed */
    Sheet* activeSheet() const;

    /**
     * \return the SheetView for \p sheet
     */
    SheetView* sheetView(const Sheet* sheet) const;

    /** Loads the view settings. */
    void initConfig();

    /** Initializes the on-the-fly calculation method. */
    void initCalcMenu();

    /** Changes the maximum number of entries in the recent files menu. */
    void changeNbOfRecentFiles(int _nb);

    /** Updates the state of the 'Show Page Borders' action (for DBus). */
    void updateBorderButton();

    /** \return the color for cell borders \deprecated */
    QColor borderColor() const;

    /**
     * \return the current cell selection.
     */
    Selection* selection() const;

    /**
     * Updates the action, that unhides a hidden sheet.
     * If there are no hidden sheets or the map is protected,
     * the show sheet action will be disabled.
     */
    void updateShowSheetMenu();

    /**
     * @return marker for @p sheet
     */
    QPoint markerFromSheet(Sheet* sheet) const;

    /**
     * @return scroll offset for @p sheet
     */
    QPointF offsetFromSheet(Sheet* sheet) const;

    /**
     * Save current sheet selection.
     * Call when we change sheet, or before save in OpenDocument format.
     */
    void saveCurrentSheetSelection();

    /**
     * The state of the 'View loading' process.
     * Useful to suppress the painting of the canvas and setting the initial
     * scrolling and selection positions, which need a fully built View.
     * \return \c true if the view is not being fully created yet.
     */
    bool isLoading() const;

public Q_SLOTS:
    /** Clears all visual cached data. */
    void refreshSheetViews();

    /** Invalidates all visual cached data for the cells in \p region. */
    void refreshSelection(const Region& region);

    void finishLoading();
    /**
     * Prepares this view for a modification of \p region, e.g. closes the cell
     * editor.
     */
    void aboutToModify(const Region& region);

    /** Sets the initial scrolling offset and selection after loading. */
    void initialPosition();

    //BEGIN Actions
    void createTemplate();
    void recalcWorkBook();
    void recalcWorkSheet();
    void paperLayoutDlg();
    void resetPrintRange();
    void togglePageOutline(bool);
    void toggleProtectSheet(bool);
    void toggleProtectDoc(bool);
    void viewZoom(KoZoomMode::Mode mode, qreal zoom);

    void insertSheet();
    void duplicateSheet();
    void deleteSheet();
    void hideSheet();
    void showSheet();

    void optionsNotifications();
    void preference();

    void copyAsText(); // DBus related

    void moveSheet(unsigned sheet, unsigned target);

    /**
     * Shows the sheet properties dialog.
     */
    void sheetProperties();
    //END Actions

    /**
     * Sets the currently displayed \p sheet.
     * \param sheet the sheet
     * \param updateTabBar if \c true, updates the tab bar
     */
    void setActiveSheet(Sheet* sheet, bool updateTabBar = true);

    /**
     * Switch the active sheet to the name. This slot is connected to the tab bar
     * and activated when the user selects a new sheet in the tab bar.
     */
    void changeSheet(const QString& _name);

    /**
     * Switch the active sheet to the next visible sheet. Does nothing if the current
     * active sheet is the last visible sheet in the workbook.
     */
    void nextSheet();

    /**
     * Switch the active sheet to the previous visible sheet. Does nothing if the current
     * active sheet is the first visible sheet in the workbook.
     */
    void previousSheet();

    /**
     * Switch the active sheet to the first visible sheet in the workbook. Does nothing
     * if the current active sheet is already the first one.
     */
    void firstSheet();

    /**
     * Switch the active sheet to the last visible sheet in the workbook. Does nothing
     * if the current active sheet is already the last one.
     */
    void lastSheet();

    /**
     * Switches the shape anchoring.
     * Shapes can either be anchored to cells or to the sheet/page.
     * \param mode string indicating cell or sheet anchoring
     */
    void setShapeAnchoring(const QString& mode);

    /** Shows the context menu for the on-the-fly calculation method. */
    void statusBarClicked(const QPoint& pos);

    /** Updates the on-the-fly calculation method menu. */
    void menuCalc(bool);

    /**
     * Shows the column header, if \p enable is \c true, otherwise it will be hidden.
     */
    void showColumnHeader(bool enable);

    /**
     * Shows the column header, if \p enable is \c true, otherwise it will be hidden.
     */
    void showRowHeader(bool enable);

    /**
     * Shows the horizontal scrollbar, if \p enable is \c true, otherwise it will be hidden.
     */
    void showHorizontalScrollBar(bool enable);

    /**
     * Shows the vertical scrollbar, if \p enable is \c true, otherwise it will be hidden.
     */
    void showVerticalScrollBar(bool enable);

    /**
     * Shows the status bar if b is true, otherwise the status bar will be hidden.
     */
    void showStatusBar(bool b);

    /**
     * Shows the tab bar if b is true, otherwise the tab bar will be hidden.
     */
    void showTabBar(bool b);

    /**
     * Shows context menu when tabbar is double-clicked.
     */
    void popupTabBarMenu(const QPoint&);

    /**
     * \ingroup Damages
     * Handles damages that need visual updates.
     */
    void handleDamages(const QList<Damage*>& damages);

    /**
     * write in statusBar result of calc (Min, or Max, average, sum, count)
     */
    void calcStatusBarOp();

protected Q_SLOTS:
    /** Renames the active sheet. */
    void slotRename();

public Q_SLOTS:
    /** Reacts on selection changes. */
    void slotChangeSelection(const Region&);

    /** Reacts on reference selection changes. */
    void slotScrollChoice(const Region&);

    /** Updates the shape anchoring action. */
    void shapeSelectionChanged();

    /** Calls KoToolProxy::deleteSelection(). */
    void editDeleteSelection();

    void updateAccessedCellRange(Sheet* sheet, const QPoint& location);

public: // reimplementations
    // KoView interface
    virtual QWidget *canvas() const;

public:
    /**
     * Retrieves the left border width that is displayed around the content if
     * the view is active.
     *
     * In a spread sheet this border is for example used to display the
     * rows, while a top border is used to display the names of the cells
     * and a right and bottom border is used to display scrollbars. If the view
     * becomes inactive, then this stuff is not displayed anymore.
     */
    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

    /**
     * Sets up so that autoScroll signals are emitted when the mouse pointer is outside the view
     */
    void enableAutoScroll();

    /**
     * Stops the emitting of autoScroll signals
     */
    void disableAutoScroll();


protected: // reimplementations
    // QWidget interface
    void keyPressEvent(QKeyEvent * _ev) override;
    // KoView interface
    void updateReadWrite(bool readwrite) override;
    KoPrintJob * createPrintJob() override;
public:
    KoZoomController *zoomController() const override;

Q_SIGNALS:
    /** Indicates that the document's read/write state has changed. */
    void documentReadWriteToggled(bool readwrite);

    /** Indicates that the sheet's protection state has changed. */
    void sheetProtectionToggled(bool protect);

    void autoScroll(const QPoint &scrollDistance);

private Q_SLOTS:
    /** Adds \p sheet to the displayed sheets. */
    void addSheet(Sheet *sheet);

    /** Removes \p sheet from the displayed sheets. */
    void removeSheet(Sheet* sheet);

    /** Called if a Sheet-instance is deleted to proper clean-up internal pointers. */
    void sheetDestroyed(QObject* obj);

    void slotAutoScroll();

private:
    Q_DISABLE_COPY(View)


    int autoScrollAcceleration(int offset) const;

    class Private;
    Private * const d;

    /** Creates and initializes the canvas and other child widgets. */
    void initView();

    /** Sets the column/row headers minima according to the zoom level. */
    void setHeaderMinima();
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VIEW
