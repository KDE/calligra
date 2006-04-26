/* This file is part of the KDE project
   Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
             (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 1999-2003 Laurent Montel <montel@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002-2003 John Dailey <dailey@vt.edu>
             (C) 1999-2003 David Faure <faure@kde.org>
             (C) 1999-2001 Simon Hausmann <hausmann@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_VIEW
#define KSPREAD_VIEW


#include <qpoint.h>
#include <q3ptrlist.h>
#include <qstringlist.h>
#include <q3valuelist.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QResizeEvent>
#include <QList>

#include <kprinter.h>
#include <kdeprint/kprintdialogpage.h>

#include <KoPoint.h>
#include <KoView.h>


class QScrollBar;

class KoDocumentEntry;
class KoTabBar;

namespace KSpread
{
class Cell;
class Damage;
class Sheet;
class Canvas;
class Child;
class Doc;
class EditWidget;
class HBorder;
class LocationEditWidget;
class Region;
class Selection;
class VBorder;
class View;
class ComboboxLocationEditWidget;
class EmbeddedKOfficeObject;
class EmbeddedObject;


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
    friend class Canvas;

    Q_OBJECT
public:

    /** Creates a new view */
    View( QWidget *_parent, const char *_name,
        Doc *doc );

    /** Destroys the view */
    ~View();

    Doc* doc() const;

    /** Returns the canvas of the view */
    Canvas* canvasWidget() const;

    /** Returns the column header */
    HBorder* hBorderWidget() const;

    /** Returns the row header */
    VBorder* vBorderWidget() const;

    /** Returns the horizontal scrollbar */
    QScrollBar* horzScrollBar() const;

    /** Returns the vertical scrollbar */
    QScrollBar* vertScrollBar() const;

    /** Returns the editor widget */
    KSpread::EditWidget* editWidget() const;

    /** Returns the location widget */
    ComboboxLocationEditWidget* posWidget() const;

    /** Returns the tab bar */
    KoTabBar* tabBar() const;

    void setZoom( int zoom, bool updateViews ); // change the zoom value

    void addSheet( Sheet *_t );
    //void removesheet( Sheet *_t );
    void removeAllSheets();
    void setActiveSheet( Sheet *_t,bool updatesheet=true );

    const Sheet* activeSheet() const;
    Sheet* activeSheet();

    void openPopupMenu( const QPoint &_global );
    void popupRowMenu(const QPoint & _point ) ;
    void popupColumnMenu( const QPoint & _point);

    // void showFormulaToolBar( bool show );

    /**
     * Used by @ref EditWidget. Sets the text of the active cell(s).
     */
    void setText( const QString& _text, bool array = false );

    void enableUndo( bool _b );
    void enableRedo( bool _b );
    void enableInsertColumn( bool _b );
    void enableInsertRow( bool _b );

    /**
     *
     * @param _geometry is the zoomed geometry of the new child.
     * @param _entry is the entry to insert.
     *
     * @todo check validity of @p _entry docs.
     */
    void insertChart( const QRect& _geometry, KoDocumentEntry& _entry );
    /**
     *
     * @param _geometry is the geometry of the new child.
     * @param _entry is the entry to insert.
     *
     * @todo check validity of @p _entry docs.
     */
    void insertChild( const QRect& _geometry, KoDocumentEntry& _entry );

   // void insertPicture( const QRect& _geometry, KUrl& _file );

    virtual void print( KPrinter &printer );
    virtual void setupPrinter( KPrinter &printer );

    /**
     * Fills the @ref EditWidget with the current cells
     * content. This function is usually called after the
     * cursor moved.
     */
    void updateEditWidget();
    /**
     * Same as updateEditEidget() but no update of menus and toolbars
     */
    void updateEditWidgetOnPress();


    /**
     * Called before saving, to finish the current edition (if any)
     */
    void deleteEditor( bool saveChanges = true );
    void closeEditor();
    virtual DCOPObject* dcopObject();

    virtual QWidget *canvas() const;
    virtual int canvasXOffset() const;
    virtual int canvasYOffset() const;

    /**
     * @reimp
     */
    KoDocument *hitTest( const QPoint &pos );


    void initConfig();


    void initCalcMenu();

    void changeNbOfRecentFiles(int _nb);

    void updateBorderButton();

    void removeSheet( Sheet *_t );
    void insertSheet( Sheet* sheet );
    QColor borderColor() const;

    Selection* selectionInfo() const;
    Selection* choice() const;

    void updateShowSheetMenu();

    /**
     * Mark all selected cells / regions of cells as 'dirty' (ie. requiring a repaint)
     * They will be repainted on the next call to paintUpdates()
     */
    void markSelectionAsDirty();

    /**
     * Repaint any cell with the paintDirty flag that is visible in this view
     */
    void paintUpdates();

    /**
     * Resets the internal handle pointer, called from InsertHandler destructor
     */
    void resetInsertHandle();

    bool isInsertingObject();

    bool showSheet(const QString& sheetName);

    /**
     * @return marker for @p sheet
     */
    QPoint markerFromSheet( Sheet* sheet ) const;

    /**
     * @return scroll offset for @p sheet
     */
    KoPoint offsetFromSheet( Sheet* sheet ) const;

    /**
     * Save current sheet selection.
     * Call when we change sheet, or before save in OpenDocument format.
     */
    void saveCurrentSheetSelection();

    void deleteSelectedObjects();

public slots:
    /**
    * refresh view when you hide/show vertical scrollbar
    */
    void refreshView();
    void initialPosition();
    /**
     * Actions
     */
    void createTemplate();
    void transformPart();
    void copySelection();
    void cutSelection();
    void deleteSelection();
    void clearTextSelection();
    void clearCommentSelection();
    void clearValiditySelection();
    void clearConditionalSelection();
    void fillRight();
    void fillLeft();
    void fillUp();
    void fillDown();
    void recalcWorkBook();
    void recalcWorkSheet();
    void paste();
    void specialPaste();
    void editCell();
    void setAreaName();
    void showAreaName();
    void adjust();
    void defaultSelection();
    void paperLayoutDlg();
    void styleDialog();
    void definePrintRange();
    void resetPrintRange();
    void insertObject();
    void insertFromDatabase();
    void insertFromTextfile();
    void insertFromClipboard();
    void insertSpecialChar();
    void togglePageBorders( bool );
    void toggleProtectSheet( bool );
    void toggleProtectDoc( bool );
    void viewZoom( const QString & );
    void find();
    void findNext();
    void findPrevious();
    void replace();
    void conditional();
    void validity();
    void insertSeries();
    void sort();
    void insertHyperlink();
    void removeHyperlink();
    void goalSeek();
    void multipleOperations();
    void subtotals();
    void textToColumns();
    void consolidate();
    void insertSheet();
    void removeSheet();
    void hideSheet();
    void showSheet();
    void helpUsing();
    void insertChart();
    void insertPicture();
    void moneyFormat(bool b);
    void alignLeft( bool b );
    void alignRight( bool b );
    void alignCenter( bool b );
    void alignTop( bool b );
    void alignMiddle( bool b );
    void alignBottom( bool b );
    void wrapText( bool b );
    void precisionMinus();
    void precisionPlus();
    void createStyleFromCell();
    void styleSelected( const QString & );
    void setSelectionPrecision(int delta);
    void percent(bool b);
    void fontSelected( const QString &_font );
    void fontSizeSelected( int size );
    void bold( bool b );
    void italic( bool b );
    void underline( bool b );
    void strikeOut( bool b );
    void deleteColumn();
    void insertColumn();
    void deleteRow();
    void insertRow();
    void hideRow();
    void showRow();
    void showSelRows();
    void hideColumn();
    void showColumn();
    void showSelColumns();
    void insertMathExpr();
    void formulaSelection( const QString &_math );
    void changeTextColor();
    void changeBackgroundColor();
    void sortInc();
    void sortDec();
    void layoutDlg();
    void extraProperties();
    void borderBottom();
    void borderRight();
    void borderLeft();
    void borderTop();
    void borderOutline();
    void borderAll();
    void borderRemove();
    void changeBorderColor();
    void sheetFormat();
    void autoSum();
    void resizeRow();
    void resizeColumn();
    void increaseFontSize();
    void decreaseFontSize();
    void setSelectionFontSize(int size);

    void setSelectionTextColor(const QColor &txtColor);
    void setSelectionBackgroundColor(const QColor &bgColor);
    void setSelectionBorderColor(const QColor &bdColor);

    void setSelectionLeftBorderColor(const QColor &color);
    void setSelectionRightBorderColor(const QColor &color);
    void setSelectionTopBorderColor(const QColor &color);
    void setSelectionBottomBorderColor(const QColor &color);
    void setSelectionAllBorderColor(const QColor &color);
    void setSelectionOutlineBorderColor(const QColor &color);

    void upper();
    void lower();
    void equalizeColumn();
    void equalizeRow();
    void preference();
    void firstLetterUpper();
    void verticalText(bool );
    void addModifyComment();
    void setSelectionComment(QString comment);
    void removeComment();
    void changeAngle();
    void setSelectionAngle(int angle);

    /**
     * Merges selected cells into one cell. This will not work if only one
     * cell is selected. An entire row or column can't be merged as well.
     *
     * \sa dissociateCell
     */
    void mergeCell();
    void mergeCellHorizontal();
    void mergeCellVertical();


    /**
     * Breaks merged cell. Obviously this can be done only on merged cells.
     *
     * \sa mergeCell
     */
    void dissociateCell();

    void gotoCell();
    void increaseIndent();
    void decreaseIndent();
    void copyAsText();

    void moveSheet( unsigned sheet, unsigned target );

    /**
     * Shows the sheet properties dialog.
     */
    void sheetProperties();

    /**
     * Switch the active sheet to the name. This slot is connected to the tab bar
     * and activated when the user selects a new sheet in the tab bar.
     */
    void changeSheet( const QString& _name );

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

    void sortList();

    void statusBarClicked(int _id);
    void menuCalc(bool);

    /**
     * Shows the status bar if b is true, otherwise the status bar will be hidden.
     */
    void showStatusBar( bool b );

    /**
     * Shows the tab bar if b is true, otherwise the tab bar will be hidden.
     */
    void showTabBar( bool b );

    /**
     * Shows the formula bar if b is true, otherwise the formula bar will be hidden.
     */
    void showFormulaBar( bool b );

    /**
     * Shows context menu when tabbar is double-clicked.
     */
    void popupTabBarMenu( const QPoint& );

    void handleDamages( const QList<Damage*>& damages );

    void runInternalTests();
    void runInspector();

    void initialiseMarkerFromSheet( Sheet *_sheet, const QPoint &point );

    /**
     * write in statusBar result of calc (Min, or Max, average, sum, count)
     */
    void calcStatusBarOp();

protected slots:
    /**
     * Popup menu
     */
    void slotActivateTool( int _id );
    void slotInsert();
    void slotInsertCellCopy();
    void slotRemove();
    void slotRename();

    /**
     * Invoked if the popup menu for an embedded document should be opened.
     */
    void popupChildMenu( KoChild*, const QPoint& global_pos );

    /**
     * Invoked when the "Delete Embedded Document" option from an embedded document's
     * popup menu is selected.
     */
    void slotPopupDeleteChild();

    /**
     * Border popup menu
     */
    void slotPopupAdjustColumn();
    void slotPopupAdjustRow();

    /**
     * list from list choose
     */
    void slotItemSelected( int );
    void slotListChoosePopupMenu( );

    /**
     * Called by find/replace (findNext) when it found a match
     */
    void slotHighlight( const QString &text, int matchingIndex, int matchedLength );
    /**
     * Called when replacing text in a cell
     */
    void slotReplace( const QString &newText, int, int, int );

    void slotSpecialChar( QChar c, const QString & _font );
    void slotSpecialCharDlgClosed();

    void propertiesOk();

    void objectSelectedChanged();

protected slots:
    void slotChildSelected( KoDocumentChild* ch );
    void slotChildUnselected( KoDocumentChild* );

public slots:
    // Document signals
    void slotRefreshView();
    void slotUpdateView( Sheet *_sheet );
    void slotUpdateView( Sheet *_sheet, const Region& );
    void slotUpdateView( EmbeddedObject* obj );
    void slotUpdateHBorder( Sheet *_sheet );
    void slotUpdateVBorder( Sheet *_sheet );
    void slotChangeSelection(const Region&);
    void slotChangeChoice(const Region&);
    void slotAddSheet( Sheet *_sheet );
    void slotUpdateChildGeometry( EmbeddedKOfficeObject *_child );
    void slotSheetRenamed( Sheet* sheet, const QString& old_name );
    void slotSheetHidden( Sheet*_sheet );
    void slotSheetShown( Sheet*_sheet );
    void slotSheetRemoved( Sheet*_sheet );
    void refreshLocale();
    void extraSpelling();

    void spellCheckerReady();
    void spellCheckerMisspelling( const QString &, const QStringList &, unsigned int);
    void spellCheckerCorrected( const QString &, const QString &, unsigned int);
    void spellCheckerDone( const QString & );
    void spellCheckerFinished( );
    void spellCheckerIgnoreAll( const QString & word);
    void spellCheckerReplaceAll( const QString &,  const QString &);
    void startKSpell();

    /**
     * Updates the view and the action. This is typically connected
     * to KoCommandHistory::commandExecuted() signal.
     */
    void commandExecuted();

    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

protected:

    virtual void keyPressEvent ( QKeyEvent * _ev );
    virtual void resizeEvent( QResizeEvent *_ev );

    virtual QMatrix matrix() const;

    /**
     * Returns the position of the top-left point of the currently selected cell in document coordinates.
     * This is used when inserting some types of objects or pasting images into the document (so that the newly
     * pasted object's top-left point will be aligned with the top-left point of the currently selected cell)
     *
     */
    KoPoint markerDocumentPosition();

    /**
     * Activates the formula editor for the current cell.
     * This function is usually called if the user presses
     * a button in the formula toolbar.
     */
    void activateFormulaEditor();

    virtual void updateReadWrite( bool readwrite );

    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    void initFindReplace();
    Cell* findNextCell();

private:
    class Private;
    Private* d;

    // don't allow copy or assignment
    View( const View& );
    View& operator=( const View& );

    void initView();

    bool spellSwitchToOtherSheet();
    void spellCleanup();

    Cell* nextFindValidCell( int col, int row );

    /**
     * @return @c true if document is being loaded. It is useful to supress scrolling
     * while the "View loading" process.
     */
    bool isLoading() const;

    friend class Private;
};

} // namespace KSpread

#endif // KSPREAD_VIEW
