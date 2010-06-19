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

#ifndef KSPREAD_VIEW
#define KSPREAD_VIEW

#include <QList>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QtGui/QPrinter>

#include <QPointF>
#include <KoView.h>
#include <KoZoomMode.h>
#include "kspread_export.h"

class QAction;
class QKeyEvent;
class QResizeEvent;
class QScrollBar;

class KoCanvasController;
class KoDocumentEntry;
class KoZoomHandler;

namespace KSpread
{
class Cell;
class ColumnHeader;
class Damage;
class Sheet;
class Canvas;
class Doc;
class Region;
class Selection;
class SheetView;
class RowHeader;
class View;
class TabBar;

/** @class View
 *
 * @brief The View class displays a KSpread document.
 *
 *
 * View is used to display a spreadsheet document and provide
 * the interface for the user to perform editing and data analysis.
 *
 * A view consists of several parts:
 *  \li canvas to display cells in a sheet
 *  \li line editor to display and edit cell contents
 *  \li location editor to show marker and/or selection
 *  \li column header to show columns
 *  \li row header to show rows
 *  \li horizontal and vertical scrollbars for navigation
 *  \li tab bar to select active worksheet
 *
 */
class KSPREAD_EXPORT View : public KoView
{
    Q_OBJECT

public:

    /** Creates a new view */
    View(QWidget *_parent, Doc *doc);

    /** Destroys the view */
    ~View();

    Doc* doc() const;

    /** Returns the canvas of the view */
    Canvas* canvasWidget() const;

    // KoView interface
    virtual QWidget *canvas() const;

    /** Returns the canvas controller of the view */
    KoCanvasController* canvasController() const;

    /** Returns the column header */
    ColumnHeader* columnHeader() const;

    /** Returns the row header */
    RowHeader* rowHeader() const;

    /** Returns the horizontal scrollbar */
    QScrollBar* horzScrollBar() const;

    /** Returns the vertical scrollbar */
    QScrollBar* vertScrollBar() const;

    /** Returns the tab bar */
    TabBar* tabBar() const;

    /** \return the zoom handler */
    KoZoomHandler* zoomHandler() const;

    /** \return the sheet, that is currently displayed */
    Sheet* activeSheet() const;

    /**
     * \return the SheetView for \p sheet
     */
    SheetView* sheetView(const Sheet* sheet) const;

    void initConfig();


    void initCalcMenu();

    void changeNbOfRecentFiles(int _nb);

    void updateBorderButton();

    QColor borderColor() const;

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
     * @return @c true if document is being loaded. It is useful to suppress scrolling
     * while the "View loading" process.
     */
    bool isLoading() const;

    virtual KoZoomController *zoomController() const;


public Q_SLOTS:
    /**
    * refresh view when you hide/show vertical scrollbar
    */
    void refreshView();
    void refreshSheetViews();
    void refreshSelection(const Region& region);
    void aboutToModify(const Region& region);
    void initialPosition();
    void finishLoading();
    /**
     * Actions
     */
    void createTemplate();
    void recalcWorkBook();
    void recalcWorkSheet();
    void paperLayoutDlg();
    void resetPrintRange();
    void togglePageBorders(bool);
    void toggleProtectSheet(bool);
    void toggleProtectDoc(bool);
    void viewZoom(KoZoomMode::Mode mode, qreal zoom);

    void insertSheet();
    void duplicateSheet();
    void deleteSheet();
    void hideSheet();
    void showSheet();
    void helpUsing();

    void optionsNotifications();
    void preference();

    void copyAsText();

    void moveSheet(unsigned sheet, unsigned target);

    /**
     * Shows the sheet properties dialog.
     */
    void sheetProperties();

    void setActiveSheet(Sheet* sheet, bool updatesheet = true);

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

    void statusBarClicked(const QPoint& pos);
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

    void initialiseMarkerFromSheet(Sheet *_sheet, const QPoint &point);

    /**
     * write in statusBar result of calc (Min, or Max, average, sum, count)
     */
    void calcStatusBarOp();

protected slots:
    void slotRename();

public slots:
    // Document signals
    void slotRefreshView();
    void slotUpdateView(Sheet *_sheet);
    void slotUpdateView(Sheet *_sheet, const Region&);
    void slotUpdateColumnHeader(Sheet *_sheet);
    void slotUpdateRowHeader(Sheet *_sheet);
    void slotChangeSelection(const Region&);
    void slotScrollChoice(const Region&);
    void slotSheetRenamed(Sheet* sheet, const QString& old_name);
    void slotSheetHidden(Sheet*_sheet);
    void slotSheetShown(Sheet*_sheet);
    void shapeSelectionChanged();
    void editDeleteSelection();

public:
    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

protected:

    virtual void keyPressEvent(QKeyEvent * _ev);
    virtual void updateReadWrite(bool readwrite);

    virtual void guiActivateEvent(KParts::GUIActivateEvent *ev);

    virtual KoPrintJob * createPrintJob();

Q_SIGNALS:
    void documentReadWriteToggled(bool readwrite);
    void sheetProtectionToggled(bool protect);

private Q_SLOTS:
    void removeSheet(Sheet* sheet);
    void reviveSheet(Sheet* sheet);

private:
    Q_DISABLE_COPY(View)

    class Private;
    Private * const d;

    void initView();

    friend class Private;
};

} // namespace KSpread

#endif // KSPREAD_VIEW
