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

#ifndef kword_view_h
#define kword_view_h

#include <qwidget.h>
#include <qlist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qbrush.h>
#include <qstringlist.h>

#include "format.h"
#include "paraglayout.h"
#include "searchdia.h"
#include "footnote.h"

#include <koPageLayoutDia.h>

#include <koView.h>

class KWPartFrameSet;
class KWordView;
class KWordDocument;
class KWordChild;
class KWordGUI;
class KWPaintWindow;
class KWordShell;
class QResizeEvent;
class QMouseEvent;
class QKeyEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QCloseEvent;
class QFocusEvent;
class QShowEvent;
class KWParag;
class KWParagDia;
class KWStyleManager;
class KWTableDia;
class KWDocStruct;
class KSpell;
class QScrollView;
class QSplitter;
class KAction;

// /******************************************************************/
// /* Class: KWordFrame						  */
// /******************************************************************/

// class KWordFrame : public KoFrame
// {
//     Q_OBJECT

// public:
//     KWordFrame( KWordView*, KWordChild* );

//     KWordChild* child()
//     { return m_pKWordChild; }
//     KWordView* wordView()
//     { return m_pKWordView; }

// protected:
//     KWordChild *m_pKWordChild;
//     KWordView *m_pKWordView;

// };

/******************************************************************/
/* Class: KWordView						  */
/******************************************************************/

class KWordView : public KoView
{
    Q_OBJECT

public:
    // C++
    KWordView( QWidget *_parent, const char *_name, KWordDocument *_doc );
    virtual ~KWordView();
    virtual void createKWordGUI();
    virtual void construct();
    virtual void setFormat( const KWFormat &_format, bool _check = true,
			    bool _update_page = true, bool _redraw = true );
    virtual void setFlow( KWParagLayout::Flow _flow );
    virtual void setLineSpacing( int _spc );
    virtual void setParagBorders( KWParagLayout::Border _left, KWParagLayout::Border _right,
				  KWParagLayout::Border _top, KWParagLayout::Border _bottom );

    virtual void setNoteType(KWFootNoteManager::NoteType nt, bool change=true);

    KWordGUI *getGUI() { return gui; }
    void setTool( MouseMode _mouseMode );
    void updateStyle( QString _styleName, bool _updateFormat = true );
    void updateStyleList();

    bool getViewFormattingChars() { return _viewFormattingChars; }
    bool getViewFrameBorders() { return _viewFrameBorders; }
    bool getViewTableGrid() { return _viewTableGrid; }

    void setSearchEntry( KWSearchDia::KWSearchEntry *e ) { searchEntry = e; }
    void setReplaceEntry( KWSearchDia::KWSearchEntry *e ) { replaceEntry = e; }

    void initGui();

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

    virtual void setupPrinter( QPrinter &printer );
    virtual void print( QPrinter &printer );

public slots:
    // TEMP function. Pure for debugging.
    void printDebug();
    // IDL
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editSelectAll();
    virtual void editFind();
    virtual void editDeleteFrame();
    virtual void editReconnectFrame();
    virtual void editCustomVars();
    virtual void editSerialLetterDataBase();

    virtual void viewFormattingChars();
    virtual void viewFrameBorders();
    virtual void viewTableGrid();
    virtual void viewHeader();
    virtual void viewFooter();
    virtual void viewFootNotes();
    virtual void viewEndNotes();
    virtual void viewZoom( const QString &s );

    virtual void insertPicture();
    virtual void insertClipart();
    virtual void insertSpecialChar();
    virtual void insertFrameBreak();
    virtual void insertVariableDateFix();
    virtual void insertVariableDateVar();
    virtual void insertVariableTimeFix();
    virtual void insertVariableTimeVar();
    virtual void insertVariablePageNum();
    virtual void insertVariableCustom();
    virtual void insertVariableSerialLetter();
    virtual void insertFootNoteEndNote();
    virtual void insertContents();

    virtual void formatFont();
    virtual void formatParagraph();
    virtual void formatPage();
    virtual void formatFrameSet();

    virtual void extraSpelling();
    virtual void extraAutoFormat();
    virtual void extraStylist();
    virtual void extraOptions();
    virtual void extraCreateTemplate();

    virtual void toolsEdit();
    virtual void toolsEditFrame();
    virtual void toolsCreateText();
    virtual void toolsCreatePix();
    virtual void toolsClipart();
    virtual void toolsTable();
    virtual void toolsKSpreadTable();
    virtual void toolsFormula();
    virtual void toolsPart();

    virtual void tableInsertRow();
    virtual void tableInsertCol();
    virtual void tableDeleteRow();
    virtual void tableDeleteCol();
    virtual void tableJoinCells();
    virtual void tableSplitCells();
    virtual void tableUngroupTable();
    virtual void tableDelete();

    virtual void textStyleSelected( const QString & );
    virtual void textSizeSelected( const QString & );
    virtual void textFontSelected( const QString & );
    virtual void textBold();
    virtual void textItalic();
    virtual void textUnderline();
    virtual void textColor();
    virtual void textAlignLeft();
    virtual void textAlignCenter();
    virtual void textAlignRight();
    virtual void textAlignBlock();
    virtual void textLineSpacing( const QString & );
    virtual void textEnumList();
    virtual void textUnsortList();
    virtual void textSuperScript();
    virtual void textSubScript();
    virtual void textBorderLeft();
    virtual void textBorderRight();
    virtual void textBorderTop();
    virtual void textBorderBottom();
    virtual void textBorderColor();
    virtual void textBorderWidth( const QString & );
    virtual void textBorderStyle( const QString & );
    virtual void frameBorderLeft();
    virtual void frameBorderRight();
    virtual void frameBorderTop();
    virtual void frameBorderBottom();
    virtual void frameBorderColor();
    virtual void frameBorderWidth( const QString &width );
    virtual void frameBorderStyle( const QString &style );
    virtual void frameBackColor();

    virtual void formulaPower();
    virtual void formulaSubscript();
    virtual void formulaParentheses();
    virtual void formulaAbsValue();
    virtual void formulaBrackets();
    virtual void formulaFraction();
    virtual void formulaRoot();
    virtual void formulaIntegral();
    virtual void formulaMatrix();
    virtual void formulaLeftSuper();
    virtual void formulaLeftSub();
    virtual void formulaProduct();
    virtual void formulaSum();

    void changeUndo( QString, bool );
    void changeRedo( QString, bool );

    void showFormulaToolbar( bool show );

public slots:
    void slotInsertObject( KWordChild *_child, KWPartFrameSet *_kwpf );
    void slotUpdateChildGeometry( KWordChild *_child );
    void paragDiaOk();
    void styleManagerOk();
    void openPageLayoutDia()  { formatPage(); }
    void newPageLayout( KoPageLayout _layout );
    void spellCheckerReady();
    void spellCheckerMisspelling( QString, QStringList*, unsigned );
    void spellCheckerCorrected( QString, QString, unsigned );
    void spellCheckerDone( const QString & );
    void spellCheckerFinished( );
    void searchDiaClosed();

    void clipboardDataChanged();

protected:
    void setupActions();
    void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void keyReleaseEvent( QKeyEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void focusInEvent( QFocusEvent *e );
    void showEvent( QShowEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    void setParagBorderValues();

    virtual void updateReadWrite( bool readwrite );

    KWordDocument *m_pKWordDoc;

    bool m_bUnderConstruction, m_bKWordModified;

    KAction *actionPrintDebug;
    KAction *actionEditUndo;
    KAction *actionEditRedo;
    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditFind;
    KAction *actionEditSelectAll;
    KAction *actionEditDelFrame;
    KAction *actionEditReconnectFrame;
    KAction *actionEditCustomVars;
    KAction *actionEditSlDataBase;

    KAction *actionViewFormattingChars;
    KAction *actionViewFrameBorders;
    KAction *actionViewTableGrid;
    KAction *actionViewHeader;
    KAction *actionViewFooter;
    KAction *actionViewFootNotes;
    KAction *actionViewEndNotes;
    KAction *actionViewZoom;

    KAction *actionInsertPicture;
    KAction *actionInsertClipart;
    KAction *actionInsertSpecialChar;
    KAction *actionInsertFrameBreak;
    KAction *actionInsertFootEndNote;
    KAction *actionInsertContents;
    KAction *actionInsertVarDateFix;
    KAction *actionInsertVarDate;
    KAction *actionInsertVarTimeFix;
    KAction *actionInsertVarTime;
    KAction *actionInsertVarPgNum;
    KAction *actionInsertVarCustom;
    KAction *actionInsertVarSerialLetter;

    KAction *actionToolsEdit;
    KAction *actionToolsEditFrames;
    KAction *actionToolsCreateText;
    KAction *actionToolsCreatePix;
    KAction *actionToolsCreateClip;
    KAction *actionToolsCreateTable;
    KAction *actionToolsCreateKSpreadTable;
    KAction *actionToolsCreateFormula;
    KAction *actionToolsCreatePart;

    KAction *actionFormatFont;
    KAction *actionFormatFontSize;
    KAction *actionFormatFontFamily;
    KAction *actionFormatStyle;
    KAction *actionFormatBold;
    KAction *actionFormatItalic;
    KAction *actionFormatUnderline;
    KAction *actionFormatColor;
    KAction *actionFormatAlignLeft;
    KAction *actionFormatAlignCenter;
    KAction *actionFormatAlignRight;
    KAction *actionFormatAlignBlock;
    KAction *actionFormatParag;
    KAction *actionFormatFrameSet;
    KAction *actionFormatPage;
    KAction *actionFormatLineSpacing;
    KAction *actionFormatEnumList;
    KAction *actionFormatUnsortList;
    KAction *actionFormatSuper;
    KAction *actionFormatSub;
    KAction *actionFormatBrdLeft;
    KAction *actionFormatBrdRight;
    KAction *actionFormatBrdTop;
    KAction *actionFormatBrdBottom;
    KAction *actionFormatBrdColor;
    KAction *actionFormatBrdWidth;
    KAction *actionFormatBrdStyle;

    KAction *actionFrameBrdLeft;
    KAction *actionFrameBrdRight;
    KAction *actionFrameBrdTop;
    KAction *actionFrameBrdBottom;
    KAction *actionFrameBrdColor;
    KAction *actionFrameBrdWidth;
    KAction *actionFrameBrdStyle;
    KAction *actionFrameBackColor;

    KAction *actionFormulaPower;
    KAction *actionFormulaSubscript;
    KAction *actionFormulaParentheses;
    KAction *actionFormulaAbs;
    KAction *actionFormulaBrackets;
    KAction *actionFormulaFraction;
    KAction *actionFormulaRoot;
    KAction *actionFormulaIntegral;
    KAction *actionFormulaMatrix;
    KAction *actionFormulaLeftSuper;
    KAction *actionFormulaLeftSub;
    KAction *actionFormulaProduct;
    KAction *actionFormulaSum;

    KAction *actionTableDelRow;
    KAction *actionTableDelCol;
    KAction *actionTableInsertRow;
    KAction *actionTableInsertCol;
    KAction *actionTableJoinCells;
    KAction *actionTableSplitCells;
    KAction *actionTableUngroup;
    KAction *actionTableDelete;

    KAction *actionExtraSpellCheck;
    KAction *actionExtraAutocorrection;
    KAction *actionExtraStylist;
    KAction *actionExtraOptions;
    KAction *actionExtraCreateTemplate;

    // text toolbar values
    QFont tbFont;
    QColor tbColor;
    QStringList fontList;
    QStringList styleList;

    KWordGUI *gui;
    bool m_bShowGUI;
    bool _viewFormattingChars, _viewFrameBorders, _viewTableGrid;

    KWFormat format;
    KWParagLayout::Flow flow;
    KWFormat::VertAlign vertAlign;
    KWParagLayout::Border left, right, top, bottom, tmpBrd, frmBrd;
    KWSearchDia::KWSearchEntry *searchEntry, *replaceEntry;
    QBrush backColor;
    int spc;

    KWParagDia *paragDia;
    KWStyleManager *styleManager;
    KSpell *kspell;
    KWSearchDia *searchDia;
    KWTableDia *tableDia;

    KWParag *currParag;
    int currFrameSetNum;
    int lastTextPos;

};

/******************************************************************/
/* Class: KWordGUI						  */
/******************************************************************/

class KWordGUI;
class KWLayoutWidget : public QWidget
{
    Q_OBJECT

public:
    KWLayoutWidget( QWidget *parent, KWordGUI *g );

protected:
    void resizeEvent( QResizeEvent *e );
    KWordGUI *gui;

};

class KWordGUI : public QWidget
{
    friend class KWLayoutWidget;

    Q_OBJECT

public:
    KWordGUI( QWidget *parent, bool __show, KWordDocument *_doc, KWordView *_view );

    KWordDocument *getDocument()
    { return doc; }

    void showGUI( bool __show );

    void setDocument( KWordDocument *_doc );

    KWordView *getView()
    { return view; }
    KWPage *getPaperWidget()
    { return paperWidget; }
    KoRuler *getVertRuler()
    { return r_vert; }
    KoRuler *getHorzRuler()
    { return r_horz; }
    KoTabChooser *getTabChooser()
    { return tabChooser; }
    KWDocStruct *getDocStruct()
    { return docStruct; }

    void keyEvent( QKeyEvent *e )
    { keyPressEvent( e ); }

    void scrollTo( int _x, int _y );

protected slots:
    void unitChanged( QString );
    void reorganize();

protected:
    void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

    bool _show;
    KoRuler *r_vert, *r_horz;
    KWPage *paperWidget;
    KWordDocument *doc;
    KWordView *view;
    KoTabChooser *tabChooser;
    KWDocStruct *docStruct;
    QSplitter *panner;
    KWLayoutWidget *left;
    bool _showStruct;

};

#endif
