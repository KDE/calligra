/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_gui_h__
#define __kspread_gui_h__

class KSpreadView;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadScripts;
class KSpreadTable;
class KSpreadDoc;
class KSpreadPaperLayout;
class KSpreadChildPicture;
class KSpreadChildFrame;
class KSpreadShell;
class KSpreadTabBar;
class KSpreadEditWidget;
class KSpreadCanvas;
class KSpreadHBorder;
class KSpreadVBorder;
class KSpreadChild;
class KSpreadCell;

class KoDocumentEntry;
class KoToolEntry;

#include <koFrame.h>
#include <koView.h>
#include <opMenu.h>
#include <opToolBar.h>
#include <koFrame.h>
#include <koScanPlugins.h>
#include <openparts_ui.h>
#include <datatools.h>

#include <qlist.h>
#include <qscrbar.h>
#include <qlabel.h>
#include <qbutton.h>
#include <qpoint.h>

#include "kspread.h"

#ifdef USE_PICTURES
class KSpreadChildPicture : public KoDocumentChildPicture
{
public:
  KSpreadChildPicture( KSpreadView*, KSpreadChild* );
  virtual ~KSpreadChildPicture();

protected:
  KSpreadView *m_pView;
};
#endif

class KSpreadChildFrame : public KoFrame
{
  Q_OBJECT
public:
  KSpreadChildFrame( KSpreadView*, KSpreadChild* );

  KSpreadChild* child() { return m_pChild; }
  /**
   * @return the view owning this frame.
   */
  KSpreadView* spreadView() { return m_pView; }

protected:
  KSpreadChild *m_pChild;
  KSpreadView *m_pView;
};


/**
 */
class KSpreadView : public QWidget,
		    virtual public KoViewIf,
		    virtual public KSpread::View_skel
{
    Q_OBJECT
public:
    KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc *_doc );
    ~KSpreadView();

    // IDL
    virtual KSpread::Book_ptr book();

    // C++
    KSpreadCanvas* canvasWidget() { return m_pCanvas; }
    KSpreadHBorder* hBorderWidget() { return m_pHBorderWidget; }
    KSpreadVBorder* vBorderWidget() { return m_pVBorderWidget; }
    QScrollBar* horzScrollBar() { return m_pHorzScrollBar; }
    QScrollBar* vertScrollBar() { return m_pVertScrollBar; }
    KSpreadEditWidget* editWidget() { return m_pEditWidget; }
    QLabel* posWidget() { return m_pPosWidget; }

    KSpreadDoc* doc() { return m_pDoc; }

    void addTable( KSpreadTable *_t );
    void removeTable( KSpreadTable *_t );
    void removeAllTables();
    void changeTable( const QString& _name );
    enum PType {TXT_COLOR, FRAME_COLOR, BACK_COLOR};
    QString colorToPixString( QColor c, PType _type );
    void setTextColor(QColor c );
    void setbgColor(QColor c );
    void setActiveTable( KSpreadTable *_t );

    KSpreadTable* activeTable() { return m_pTable; }
    KSpreadTabBar* tabBar() { return  m_pTabBar;}

    void openPopupMenu( const QPoint &_global );
    void set_bg_color(QColor _bgcolor) { bgColor=_bgcolor;}
    void set_text_color(QColor _tbcolor) { tbColor=_tbcolor;}
    void PopupMenuRow(const QPoint & _point ) ;
    void PopupMenuColumn( const QPoint & _point);
    /**
     * Used by @ref KSpreadEditWidget. Sets the text of the active cell.
     */
    void setText( const QString& _text );

    void enableUndo( bool _b );
    void enableRedo( bool _b );

    /**
     * Called by @ref KSpreadCanvas if its action is @ref KSpreadCanvas::InsertChart.
     *
     * @param _geometry is the zoomed geometry of the new child.
     */
    void insertChart( const QRect& _geometry );
    /**
     * Called by @ref KSpreadCanvas if its action is @ref KSpreadCanvas::InsertObject.
     *
     * @param _geometry is the zoomed geometry of the new child.
     */
    void insertChild( const QRect& _geometry, KoDocumentEntry& _entry );

    void setFocus( CORBA::Boolean mode );

#ifdef USE_PICTURE
    QListIterator<KSpreadChildPicture> pictures() { return QListIterator<KSpreadChildPicture>( m_lstPictures ); }

    void markChildPicture( KSpreadChildPicture *_pic );
#endif
    // IDL
    virtual CORBA::Boolean printDlg();

    // IDL Slots
    /**
     * Menu Edit->Copy
     */
    void copySelection();
    /**
     * Menu Edit->Cut
     */
    void cutSelection();
    /**
     * Menu Edit->Patse
     */
    void paste();
    /**
     * Menu Edit->Special Paste
     */
     void Specialpaste();

    /**
     * Menu Edit->Cell
     */
    void editCell();
    /**
     * Menu Edit->Undo
     */
    void undo();
    /**
     * Menu Edit->Redo
     */
    void redo();
    /**
     * Menu Edit->Page Layout
     */
    void paperLayoutDlg();
    /**
     * Menu Edit->Insert->Table
     */
    void insertTable();
    /**
     * Menu Edit->Insert->Object
     */
    void insertObject();
    /**
     * Menu Edit->Insert->Image
     */
    void insertImage();
     /**
     * Menu Edit->Remove->Table
     */
     void RemoveTable();


    /**
     * Menu Scripts->Edit Global Scripts
     */
    void editGlobalScripts();
    /**
     * Menu Scripts->Edit Local Script
     */
    void editLocalScripts();
    /**
     * Menu Scripts->Reload Scripts
     */
    void reloadScripts();
    /**
     * Menu Scripts->Run Local Script
     */
    void runLocalScript();

    /**
     * Menu View->New View
     */
    void newView();
    /**
     * Menu View->Show Page Borders
     */
    void togglePageBorders();
    /**
     * Menu Data
     */
     void gotocell();
    /**
     * Menu Data
     */
     void replace();
    /**
     * Menu Data
     */
    void consolidate();

    /**
     * Menu Folder
     */
    void insertNewTable();

    /**
     * Menu Auto Fill
     */
    void autoFill();

    /**
     * Menu for help menu
     */
    void helpUsing();

    /**
     * ToolBar
     */
    // void print();
    /**
     * ToolBar
     */
    void insertChart();

    /**
     * ToolBar
     */
  // void zoomMinus();
    /**
     * ToolBar
     */
  // void zoomPlus();

    /**
     * ToolBar
     */
    void moneyFormat();

    /**
     * ToolBar
     */
    void alignLeft();
    /**
     * ToolBar
     */
    void alignRight();
    /**
     * ToolBar
     */
    void alignCenter();
    /**
     * ToolBar
     */
    void multiRow();

    /**
     * ToolBar
     */
    void precisionMinus();
    /**
     * ToolBar
     */
    void precisionPlus();

    /**
     * ToolBar
     */
    void percent();

    /**
     * ToolBar
     */
    void fontSelected( const CORBA::WChar *_font );
    /**
     * ToolBar
     */
    void fontSizeSelected( const CORBA::WChar *_size );
    /**
     * ToolBar
     */
    void bold();
    /**
     * ToolBar
     */
    void italic();

    /**
     * ToolBar
     */
    void deleteColumn();
    /**
     * ToolBar
     */
    void deleteRow();
    /**
     * ToolBar
     */
    void insertColumn();
    /**
     * ToolBar
     */
    void insertRow();
    /**
     *Toolbar
     */

    void formulaselection( const CORBA::WChar *_math );

    void TextColor();
    void BackgroundColor();

    virtual void cleanUp();

protected slots:
    // C++
    /**
     * Popup menu
     */
    void slotActivateTool( int _id );
    /**
     * Popup menu
     */
    void slotCopy();
    /**
     * Popup menu
     */
    void slotCut();
    /**
     * Popup menu
     */
    void slotPaste();
    /**
     * Popup menu
     */
     void slotSpecialPaste();
    /**
     * Popup menu
     */
    void slotDelete();
    /**
     * Popup menu
     */
    void slotLayoutDlg();

    /**
     * @ref #tabBar is connected to this slot.
     * When the user selects a new table using the @ref #tabBar this slot
     * is signaled.
     */
    void slotChangeTable( const QString& _name );
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToFirstTable();
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToLeftTable();
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToRightTable();
    /**
     * Scroll @ref #tabBar.
     */
    void slotScrollToLastTable();

    void slotInsertRow();

    void slotRemoveRow();

    void slotInsertColumn();

    void slotRemoveColumn();


public slots:
    // Document signals
    void slotUnselect( KSpreadTable *_table, const QRect& _old );
    void slotUpdateView( KSpreadTable *_table );
    void slotUpdateView( KSpreadTable *_table, const QRect& );
    void slotUpdateCell( KSpreadTable *_table, KSpreadCell* _cell, int _col, int _row );
    void slotUpdateHBorder( KSpreadTable *_table );
    void slotUpdateVBorder( KSpreadTable *_table );
    void slotChangeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new );
    void slotAddTable( KSpreadTable *_table );
    void slotInsertChild( KSpreadChild *_child );
    void slotRemoveChild( KSpreadChild *_child );
    void slotUpdateChildGeometry( KSpreadChild *_child );

    // KSpreadChildFrame signals
    void slotChildGeometryEnd( KoFrame* );
    void slotChildMoveEnd( KoFrame* );

    // IDL
    virtual CORBA::ULong leftGUISize();
    virtual CORBA::ULong rightGUISize();
    virtual CORBA::ULong topGUISize();
    virtual CORBA::ULong bottomGUISize();

signals:
    void sig_selectionChanged( KSpreadTable* _table, const QRect& _selection );

protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const char* _event, const CORBA::Any& _value );
    // C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );
    virtual bool mappingToolDone( DataTools::Answer& _answer );
    virtual bool mappingEventSetText( KSpread::EventSetText& _event );
    virtual bool mappingEventKeyPressed( KSpread::EventKeyPressed& _event );
    virtual bool mappingEventChartInserted( KSpread::EventChartInserted& _event );

    virtual void keyPressEvent ( QKeyEvent * _ev );
    virtual void resizeEvent( QResizeEvent *_ev );

    // GUI stuff
    QButton* newIconButton( const char *_file, bool _kbutton = false, QWidget *_parent = 0L );

    QScrollBar *m_pHorzScrollBar;
    QScrollBar *m_pVertScrollBar;
    KSpreadCanvas *m_pCanvas;
    KSpreadVBorder *m_pVBorderWidget;
    KSpreadHBorder *m_pHBorderWidget;
    KSpreadEditWidget *m_pEditWidget;
    QWidget *m_pFrame;
    QFrame *m_pToolWidget;
    QButton *m_pTabBarFirst;
    QButton *m_pTabBarLeft;
    QButton *m_pTabBarRight;
    QButton *m_pTabBarLast;
    QButton *m_pOkButton;
    QButton *m_pCancelButton;
    KSpreadTabBar *m_pTabBar;
    QLabel *m_pPosWidget;


    OpenPartsUI::ToolBar_var m_vToolBarEdit;
    CORBA::Long m_idButtonEdit_Copy;
    CORBA::Long m_idButtonEdit_Paste;
    CORBA::Long m_idButtonEdit_Cut;
    CORBA::Long m_idButtonEdit_DelRow;
    CORBA::Long m_idButtonEdit_DelCol;
    CORBA::Long m_idButtonEdit_InsRow;
    CORBA::Long m_idButtonEdit_InsCol;
    OpenPartsUI::ToolBar_var m_vToolBarLayout;
    CORBA::Long m_idComboLayout_Font;
    CORBA::Long m_idComboLayout_FontSize;
    CORBA::Long m_idButtonLayout_Bold;
    CORBA::Long m_idButtonLayout_Italic;
    CORBA::Long m_idButtonLayout_Money;
    CORBA::Long m_idButtonLayout_Percent;
    CORBA::Long m_idButtonLayout_Left;
    CORBA::Long m_idButtonLayout_Center;
    CORBA::Long m_idButtonLayout_Right;
    CORBA::Long m_idButtonLayout_MultiRows;
    CORBA::Long m_idButtonLayout_PrecMinus;
    CORBA::Long m_idButtonLayout_PrecPlus;
    CORBA::Long m_idButtonLayout_Chart;
    CORBA::Long m_idButtonLayout_Text_Color;
    CORBA::Long m_idButtonLayout_bg_Color;

    OpenPartsUI::Menu_var m_vMenuEdit;
    CORBA::Long m_idMenuEdit_Undo;
    CORBA::Long m_idMenuEdit_Redo;
    CORBA::Long m_idMenuEdit_Cut;
    CORBA::Long m_idMenuEdit_Copy;
    CORBA::Long m_idMenuEdit_Paste;
    CORBA::Long m_idMenuEdit_Cell;
    CORBA::Long m_idMenuEdit_Layout;
    CORBA::Long m_idMenuEdit_Special;

    OpenPartsUI::Menu_var m_vMenuEdit_Insert;
    CORBA::Long m_idMenuEdit_Insert_Table;
    CORBA::Long m_idMenuEdit_Insert_Chart;
    CORBA::Long m_idMenuEdit_Insert_Image;
    CORBA::Long m_idMenuEdit_Insert_Object;

    OpenPartsUI::Menu_var m_vMenuEdit_Remove;
    CORBA::Long m_idMenuEdit_Remove_Table;

    OpenPartsUI::Menu_var m_vMenuView;
    CORBA::Long m_idMenuView_NewView;
    CORBA::Long m_idMenuView_ShowPageBorders;
    OpenPartsUI::Menu_var m_vMenuData;
    CORBA::Long m_idMenuData_Consolidate;
    CORBA::Long m_idMenuData_replace;
    CORBA::Long m_idMenuData_goto;
    OpenPartsUI::Menu_var m_vMenuFolder;
    CORBA::Long m_idMenuFolder_NewTable;
    OpenPartsUI::Menu_var m_vMenuFormat;
    CORBA::Long m_idMenuFormat_AutoFill;
    OpenPartsUI::Menu_var m_vMenuScripts;
    CORBA::Long m_idMenuScripts_EditGlobal;
    CORBA::Long m_idMenuScripts_EditLocal;
    CORBA::Long m_idMenuScripts_Reload;
    CORBA::Long m_idMenuScripts_Run;
    OpenPartsUI::Menu_var m_vMenuHelp;
    CORBA::Long m_idMenuHelp_About;
    CORBA::Long m_idMenuHelp_Using;
    OpenPartsUI::ToolBar_var m_vToolBarMath ;
    CORBA::Long m_idComboMath;

    /**
     * Pointer to the last popup menu.
     * Since only one popup menu can be opened at once, its pointer is stored here.
     * Delete the old one before you store a pointer to anotheron here.
     * May be 0L.
     */
    QPopupMenu *m_pPopupMenu;
    int m_popupMenuFirstToolId;

    QPopupMenu *m_pPopupRow;
    QPopupMenu *m_pPopupColumn;

     QColor tbColor;
     QColor bgColor;
    /**
     * Tells wether the user modfied the current cell.
     * Some key events are passed to the @ref EditWindow. When this flag is set and you
     * want to leave the cell with the marker then you must first save the new text
     * in the cell before moving the marker.
     */
    bool m_bEditDirtyFlag;

    /**
     * The active KSpreadTable. This table has the input focus. It may be 0L, too.
     */
    KSpreadTable* m_pTable;

    KSpreadDoc *m_pDoc;

    /**
     * Tells wether undo is possible right now or not.
     *
     * @see #enableUndo
     */
    bool m_bUndo;
    /**
     * Tells wether redo is possible right now or not.
     *
     * @see #enableRedo
     */
    bool m_bRedo;

    /**
     * Flags that indicate wether we should display additional
     * GUI stuff like rulers and scrollbars.
     *
     * @see #showGUI
     */
    bool m_bShowGUI;

    QList<KSpreadChildFrame> m_lstFrames;
#ifdef USE_PICTURES
    QList<KSpreadChildPicture> m_lstPictures;
#endif

  /**
   * Set to true if the function @ref #init is entered. The start value if false.
   */
   bool m_bInitialized;

   struct ToolEntry
   {
     QString command;
     KoToolEntry* entry;
   };
   QList<ToolEntry> m_lstTools;

   KoPluginManager *m_pluginManager;

   static KSpreadScripts *m_pGlobalScriptsDialog;
};

#endif
