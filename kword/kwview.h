/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef kwview_h
#define kwview_h

#include <koprinter.h>
#include <qbrush.h>

#include "border.h"
#include "defs.h"
#include "kwtextparag.h"

#include <koPageLayoutDia.h>
#include <koView.h>

class KWDocStruct;
class KoRuler;
class KWCanvas;
class KWChild;
class KWDocument;
class KWGUI;
class KWFrame;
class KWPartFrameSet;
class KWSearchContext;
class KWStyleManager;
class KWTableDia;
class KWView;
class QResizeEvent;
class KSpell;
class QScrollView;
class QSplitter;
class KAction;
class KActionMenu;
class KSelectAction;
class KToggleAction;
class KFontSizeAction;
class KFontAction;
class KWTextFrameSetEdit;
class TKSelectColorAction;
class KoPartSelectAction;
class KCharSelectDia;
class KWTextFrameset;
class KMacroCommand;

namespace Qt3 {
class QTextFormat;
class QTextParag;
};

/******************************************************************/
/* Class: KWView						  */
/******************************************************************/

class KWView : public KoView
{
    Q_OBJECT

public:
    KWView( QWidget *_parent, const char *_name, KWDocument *_doc );
    virtual ~KWView();

    // Those methods update the UI (from the given formatting info)
    // They do NOT do anything to the text
    void showFormat( const QTextFormat &currentFormat );
    void showAlign( int align );
    void showCounter( KoParagCounter &c );
    void showParagBorders( Border _left, Border _right,
                          Border _top, Border _bottom );
    void showFrameBorders( Border _left, Border _right,
                          Border _top, Border _bottom );

    void showStyle( const QString & styleName );
    void showRulerIndent( double _leftMargin, double _firstLine );
    void showZoom( int zoom ); // show a zoom value in the combo
    void setZoom( int zoom, bool updateViews ); // change the zoom value

    bool viewFrameBorders() const { return m_viewFrameBorders; }
    void setViewFrameBorders(bool b);

    // Currently unused
    //bool viewTableGrid() const { return m_viewTableGrid; }
    //void setViewTableGrid(bool _b) { m_viewTableGrid = _b;}

//    virtual void setNoteType(KWFootNoteManager::NoteType nt, bool change=true);

    KWDocument *kWordDocument() { return m_doc; }
    KWGUI *getGUI() { return m_gui; }
    void updateStyleList();

    void initGui();

    int currentPage() const { return m_currentPage; }

    /**
     * Overloaded from View
     */
    bool doubleClickActivation() const;
    /**
     * Overloaded from View
     */
    QWidget* canvas();
    /**
     * Overloaded from View
     */
    int canvasXOffset() const;
    /**
     * Overloaded from View
     */
    int canvasYOffset() const;
    /**
     * Overloaded vrom View
     */
    void canvasAddChild( KoViewChild *child );

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );
    void openPopupMenuInsideFrame(KWFrame *, const QPoint &);
    void openPopupMenuEditFrame(const QPoint &);
    void openPopupMenuChangeAction( const QPoint & );

    void initConfig();

    void updatePopupMenuChangeAction();
    void changeNbOfRecentFiles(int _nb);

    void changeZoomMenu( int zoom=-1);

    void refreshMenuExpression();

    void deleteFrame(bool _warning=true);

public slots:
    void fileStatistics();
    void editCut();
    void editCopy();
    void editPaste();
    void editSelectAll();
    void editFind();
    void editReplace();
    void editDeleteFrame();
    void editCustomVars();
    void editSerialLetterDataBase();

    void viewTextMode();
    void viewPageMode();
    void viewPreviewMode();
    void slotViewFormattingChars();
    void slotViewFrameBorders();
    void viewHeader();
    void viewFooter();
    void viewFootNotes();
    void viewEndNotes();
    void viewZoom( const QString &s );

    void insertTable();
    void insertPicture();
    void insertSpecialChar();
    void insertFrameBreak();
    void insertVariable();
    void insertFootNoteEndNote();
    void insertContents();

    void formatFont();
    void formatParagraph();
    void formatPage();
    void formatFrameSet();

    void extraSpelling();
    void extraAutoFormat();
    void extraStylist();
    void extraCreateTemplate();

    void toolsCreateText();
    void insertFormula();
    void toolsPart();

    void tableInsertRow();
    void tableInsertCol();
    void tableDeleteRow();
    void tableDeleteCol();
    void tableJoinCells();
    void tableSplitCells();
    //void tableSplitCellsVerticaly();
    //void tableSplitCellsHorizontaly();
    void tableUngroupTable();
    void tableDelete();

    void textStyleSelected( int );
    void textSizeSelected( int );
    void textFontSelected( const QString & );
    void textBold();
    void textItalic();
    void textUnderline();
    void textStrikeOut();
    void textColor();
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textAlignBlock();
    void textList();
    void textSuperScript();
    void textSubScript();
    void textIncreaseIndent();
    void textDecreaseIndent();
    void textDefaultFormat();

    // Text and Frame borders.
    void borderOutline();
    void borderLeft();
    void borderRight();
    void borderTop();
    void borderBottom();
    void borderColor();
    void borderWidth( const QString &width );
    void borderStyle( const QString &style );
    void backgroundColor();

    void showFormulaToolbar( bool show );

    void configure();

    void newPageLayout( KoPageLayout _layout );
    void newLeftIndent( double _leftIndent);
    void newFirstIndent( double _firstIndent);

    void spellCheckerReady();
    void spellCheckerMisspelling( QString, QStringList*, unsigned );
    void spellCheckerCorrected( QString, QString, unsigned );
    void spellCheckerDone( const QString & );
    void spellCheckerFinished( );

    void clipboardDataChanged();
    void tabListChanged( const KoTabulatorList & tabList );

    void updatePageInfo();
    void updateFrameStatusBarItem();

    void slotSpecialChar(QChar , const QString &);
    void slotFrameSetEditChanged();
    void setTool( int /*KWCanvas::MouseMode*/ _mouseMode );
    void frameSelectedChanged();
    void docStructChanged(int _type);

    void pageNumChanged();

    void insertExpression();

    void renameButtonTOC(bool b);

    void changeCaseOfText();

    void editPersonalExpr();

    void slotUpdateRuler();
    void slotEmbedImage( const QString &filename );

    void insertCustomVariable();
    void insertNewCustomVariable();
    void slotSpecialCharDlgClosed();

    void refreshCustomMenu();
    void setFrameStartEnd();
protected:
    void addVariableActions( int type, const QStringList & texts,
                             KActionMenu * parentMenu, const QString & menuText );

    void loadexpressionActions( KActionMenu * parentMenu);

    void createExpressionActions( KActionMenu * parentMenu,const QString& filename );

    void insertPicture( const QString &filename, bool isClipart, bool makeInline, QSize pixmapSize, bool _keepRatio );

    KWTextFrameSetEdit *currentTextEdit();

    void setupActions();
    void createKWGUI();

    virtual void resizeEvent( QResizeEvent *e );
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    virtual void updateReadWrite( bool readwrite );

    void borderShowValues();
    void borderSet();

    void tableSplitCells(int col, int row);

    void startKSpell();

private:
    KWDocument *m_doc;

    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditSelectAll;
    KAction *actionEditDelFrame;
    KAction *actionEditCustomVars;

    KToggleAction *actionViewTextMode;
    KToggleAction *actionViewPageMode;
    KToggleAction *actionViewPreviewMode;

    KToggleAction *actionViewFormattingChars;
    KToggleAction *actionViewFrameBorders;
    KToggleAction *actionViewHeader;
    KToggleAction *actionViewFooter;
    KToggleAction *actionViewFootNotes;
    KToggleAction *actionViewEndNotes;
    KSelectAction *actionViewZoom;

    KAction *actionInsertFrameBreak;
    KAction *actionInsertFootEndNote;
    KAction *actionInsertContents;
    KActionMenu *actionInsertVariable;
    struct VariableDef {
        int type;
        int subtype;
    };
    typedef QMap<KAction *, VariableDef> VariableDefMap;
    KActionMenu *actionInsertExpression;

    KActionMenu *actionInsertCustom;

    VariableDefMap m_variableDefMap;
    KAction *actionInsertFormula;

    //KToggleAction *actionToolsEdit;
    //KToggleAction *actionToolsEditFrames;
    KToggleAction *actionToolsCreateText;
    KToggleAction *actionToolsCreatePix;
    KToggleAction *actionToolsCreateClip;
    KoPartSelectAction *actionToolsCreatePart;

    KAction *actionFormatFont;
    KAction *actionFormatDefault;

    KFontSizeAction *actionFormatFontSize;
    KFontAction *actionFormatFontFamily;
    KSelectAction *actionFormatStyle;
    KToggleAction *actionFormatBold;
    KToggleAction *actionFormatItalic;
    KToggleAction *actionFormatUnderline;
    KToggleAction *actionFormatStrikeOut;
    TKSelectColorAction *actionFormatColor;
    KToggleAction *actionFormatAlignLeft;
    KToggleAction *actionFormatAlignCenter;
    KToggleAction *actionFormatAlignRight;
    KToggleAction *actionFormatAlignBlock;
    KAction *actionFormatParag;
    KAction *actionFormatFrameSet;
    KAction *actionFormatIncreaseIndent;
    KAction *actionFormatDecreaseIndent;
    KToggleAction *actionFormatList;
    KToggleAction *actionFormatSuper;
    KToggleAction *actionFormatSub;
    KAction* actionInsertSpecialChar;

    // Text and Frame borders.
    KToggleAction *actionBorderLeft;
    KToggleAction *actionBorderRight;
    KToggleAction *actionBorderTop;
    KToggleAction *actionBorderBottom;
    KToggleAction *actionBorderOutline;
    TKSelectColorAction *actionBorderColor;
    KSelectAction *actionBorderWidth;
    KSelectAction *actionBorderStyle;
    TKSelectColorAction *actionBackgroundColor;
    struct
    {
        Border left;    // Values specific to left border.
        Border right;   // right.
        Border top;     // top.
        Border bottom;  // bottom.
        Border common;  // Value common to left, right top and bottom borders.
    } m_border;

    KAction *actionTableDelRow;
    KAction *actionTableDelCol;
    KAction *actionTableInsertRow;
    KAction *actionTableInsertCol;
    KAction *actionTableJoinCells;
    KAction *actionTableSplitCells;

    KAction *actionTableUngroup;
    KAction *actionTableDelete;

    KAction *actionExtraSpellCheck;
    KAction *actionExtraCreateTemplate;

    KAction *actionChangeCase;

    KAction *actionEditPersonnalExpr;

    KAction *actionConfigure;

    QList<KAction> m_actionList; // for the kodatatools

    KCharSelectDia *m_specialCharDlg;
    KWGUI *m_gui;

    KWSearchContext *searchEntry, *replaceEntry;

    int m_currentPage; // 0-based current page number

    // Statusbar items
    QLabel * m_sbPageLabel; // 'Current page number and page count' label
    QLabel * m_sbFramesLabel; // Info about selected frames

    // Zoom values for each viewmode ( todo a viewmode enum and a qmap or so )
    int m_zoomViewModeNormal;
    int m_zoomViewModePreview;

    bool m_viewFrameBorders /*, m_viewTableGrid*/;
    QStringList m_ignoreWord;

    // Spell-checking
    struct {
	KSpell *kspell;
	int spellCurrFrameSetNum;
	QList<KWTextFrameSet> textFramesets;
	QStringList ignoreWord;
	KMacroCommand * macroCmdSpellCheck;
     } m_spell;

};

/******************************************************************/
/* Class: KWGUI						  */
/******************************************************************/

class KWGUI;
class KWLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    KWLayoutWidget( QWidget *parent, KWGUI *g );

protected:
    void resizeEvent( QResizeEvent *e );
    KWGUI *gui;

};

class KWGUI : public QWidget
{
    friend class KWLayoutWidget;

    Q_OBJECT

public:
    KWGUI( QWidget *parent, KWView *_view );

    void showGUI();

    KWView *getView() { return view; }
    KWCanvas *canvasWidget() { return canvas; }
    KoRuler *getVertRuler() { return r_vert; }
    KoRuler *getHorzRuler() { return r_horz; }
    KoTabChooser *getTabChooser() { return tabChooser; }
    KWDocStruct *getDocStruct() { return docStruct; }

public slots:
    void reorganize();

protected slots:
    void unitChanged( QString );


protected:
    void resizeEvent( QResizeEvent *e );

    KoRuler *r_vert, *r_horz;
    KWCanvas *canvas;
    KWView *view;
    KoTabChooser *tabChooser;
    KWDocStruct *docStruct;
    QSplitter *panner;
    KWLayoutWidget *left;

};

#endif
