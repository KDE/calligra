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
#include <qwidget.h>
#include <qlist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qbrush.h>
#include <qstringlist.h>
#include <qstylesheet.h>

#include "border.h"
#include "defs.h"
#include "kwtextparag.h"
//#include "footnote.h"

#include <koPageLayoutDia.h>
#include <koRuler.h>
#include <koView.h>

class KWCanvas;
class KWChild;
class KWDocument;
class KWDocStruct;
class KWGUI;
class KWParag;
class KWParagDia;
class KWPartFrameSet;
class KWSearchContext;
class KWStyleManager;
class KWTableDia;
class KWView;
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
class KSpell;
class QScrollView;
class QSplitter;
class KAction;
class KSelectAction;
class KToggleAction;
class KSelectColorAction;
class KFontSizeAction;
class KFontAction;
class KColorAction;
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
    void showCounter( Counter &c );
    void showParagBorders( Border _left, Border _right,
                          Border _top, Border _bottom );
    void showFrameBorders( Border _left, Border _right,
                          Border _top, Border _bottom );

    void showStyle( const QString & styleName );
    void showRulerIndent( KWUnit _leftMargin, KWUnit _firstLine );

//    virtual void setNoteType(KWFootNoteManager::NoteType nt, bool change=true);

    KWGUI *getGUI() { return gui; }
    void setTool( MouseMode _mouseMode );
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
    void openPopupMenuEditText(const QPoint &);
    void openPopupMenuEditFrame(const QPoint &);
    void openPopupMenuChangeAction( const QPoint & );

    void initConfig();

    void updatePopupMenuChangeAction();

public slots:
    // TEMP functions. Purely for debugging.
    void printDebug();

    void editCut();
    void editCopy();
    void editPaste();
    void editSelectAll();
    void editFind();
    void editReplace();
    void editDeleteFrame();
    void editReconnectFrame();
    void editCustomVars();
    void editSerialLetterDataBase();

    void viewFormattingChars();
    void viewFrameBorders();
    void viewTableGrid();
    void viewHeader();
    void viewFooter();
    void viewFootNotes();
    void viewEndNotes();
    void viewZoom( const QString &s );

    void insertPicture();
    void insertPicture(const QString &filename);
    void insertSpecialChar();
    void insertFrameBreak();
    void insertVariableDateFix();
    void insertVariableDateVar();
    void insertVariableTimeFix();
    void insertVariableTimeVar();
    void insertVariablePageNum();
    void insertVariableCustom();
    void insertVariableSerialLetter();
    void insertFootNoteEndNote();
    void insertContents();

    void formatFont();
    void formatParagraph();
    void formatPage();
    void formatFrameSet();

    void extraSpelling();
    void extraAutoFormat();
    void extraStylist();
    void extraOptions();
    void extraCreateTemplate();

    void toolsEdit();
    void toolsEditFrame();
    void toolsCreateText();
    void toolsCreatePix();
    void toolsTable();
    void toolsFormula();
    void toolsPart();

    void tableInsertRow();
    void tableInsertCol();
    void tableDeleteRow();
    void tableDeleteCol();
    void tableJoinCells();
    void tableSplitCells();
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
    void textEnumList();
    void textUnsortList();
    void textSuperScript();
    void textSubScript();
    void textBorderLeft();
    void textBorderRight();
    void textBorderTop();
    void textBorderBottom();
    void textBorderOutline();
    void textBorderColor();
    void textBorderWidth( const QString & );
    void textBorderStyle( const QString & );
    void frameBorderLeft();
    void frameBorderRight();
    void frameBorderTop();
    void frameBorderBottom();
    void frameBorderOutline();
    void frameBorderColor();
    void frameBorderWidth( const QString &width );
    void frameBorderStyle( const QString &style );
    void frameBackColor();

    void formulaPower();
    void formulaSubscript();
    void formulaParentheses();
    void formulaAbsValue();
    void formulaBrackets();
    void formulaFraction();
    void formulaRoot();
    void formulaIntegral();
    void formulaMatrix();
    void formulaLeftSuper();
    void formulaLeftSub();
    void formulaProduct();
    void formulaSum();

    void showFormulaToolbar( bool show );

    void configure();

    void slotInsertObject( KWChild *_child, KWPartFrameSet *_kwpf );
    void slotUpdateChildGeometry( KWChild *_child );
    void paragDiaOk();
    void styleManagerOk();
    void fontDiaOk();
    void openPageLayoutDia()  { formatPage(); }
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

    void slotSpecialChar(QChar , const QString &);

protected:
    void setupActions();
    void createKWGUI();

    virtual void resizeEvent( QResizeEvent *e );
/*    virtual void keyPressEvent( QKeyEvent *e );
    virtual void keyReleaseEvent( QKeyEvent *e );
    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void focusInEvent( QFocusEvent *e );
    virtual void showEvent( QShowEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );
  */
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    void showParagBorderValues();
    void verifyFrameOutline();
    virtual void updateReadWrite( bool readwrite );

    KWDocument *doc;

    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditSelectAll;
    KAction *actionEditDelFrame;
    KAction *actionEditReconnectFrame;
    KAction *actionEditCustomVars;
    KAction *actionEditSlDataBase;

    KToggleAction *actionViewFormattingChars;
    KToggleAction *actionViewFrameBorders;
    KToggleAction *actionViewTableGrid;
    KToggleAction *actionViewHeader;
    KToggleAction *actionViewFooter;
    KToggleAction *actionViewFootNotes;
    KToggleAction *actionViewEndNotes;
    KSelectAction *actionViewZoom;

    KAction *actionInsertPicture;
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

    KToggleAction *actionToolsEdit;
    KToggleAction *actionToolsEditFrames;
    KToggleAction *actionToolsCreateText;
    KToggleAction *actionToolsCreatePix;
    KToggleAction *actionToolsCreateClip;
    KToggleAction *actionToolsCreateTable;
    KToggleAction *actionToolsCreateFormula;
    KToggleAction *actionToolsCreatePart;

    KAction *actionFormatFont;
    KFontSizeAction *actionFormatFontSize;
    KFontAction *actionFormatFontFamily;
    KSelectAction *actionFormatStyle;
    KToggleAction *actionFormatBold;
    KToggleAction *actionFormatItalic;
    KToggleAction *actionFormatUnderline;
    KToggleAction *actionFormatStrikeOut;
    KColorAction *actionFormatColor;
    KToggleAction *actionFormatAlignLeft;
    KToggleAction *actionFormatAlignCenter;
    KToggleAction *actionFormatAlignRight;
    KToggleAction *actionFormatAlignBlock;
    KAction *actionFormatParag;
    KAction *actionFormatFrameSet;
    KAction *actionFormatPage;
    KAction *actionFormatStylist;
    KToggleAction *actionFormatEnumList;
    KToggleAction *actionFormatUnsortList;
    KToggleAction *actionFormatSuper;
    KToggleAction *actionFormatSub;
    KToggleAction *actionFormatBrdLeft;
    KToggleAction *actionFormatBrdRight;
    KToggleAction *actionFormatBrdTop;
    KToggleAction *actionFormatBrdBottom;
    KToggleAction *actionFormatBrdOutline;
    KSelectColorAction *actionFormatBrdColor;
    KSelectAction *actionFormatBrdWidth;
    KSelectAction *actionFormatBrdStyle;

    KToggleAction *actionFrameBrdLeft;
    KToggleAction *actionFrameBrdRight;
    KToggleAction *actionFrameBrdTop;
    KToggleAction *actionFrameBrdBottom;
    KToggleAction *actionFrameBrdOutline;
    KSelectColorAction *actionFrameBrdColor;
    KSelectAction *actionFrameBrdWidth;
    KSelectAction *actionFrameBrdStyle;
    KSelectColorAction *actionFrameBackColor;

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
    KAction *actionExtraOptions;
    KAction *actionExtraCreateTemplate;

    KAction *actionConfigure;

    // text toolbar values
    QStringList fontList;

    KWGUI *gui;

    Border left, right, top, bottom, tmpBrd, frmBrd;
    KWSearchContext *searchEntry, *replaceEntry;
    QBrush backColor;

    // Spell-checking
    KSpell *kspell;
    KWTextParag *currParag;
    int currFrameSetNum;
    int m_currentPage; // 0-based current page number
    int lastTextPos;

    // Statusbar items
    enum StatusBarIds {
        statusPage = 1
        // More to be added
    };
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
    KWGUI( QWidget *parent, bool __show, KWDocument *_doc, KWView *_view );

    KWDocument *getDocument() { return doc; }

    void showGUI();

    KWView *getView() { return view; }
    KWCanvas *canvasWidget() { return canvas; }
    KoRuler *getVertRuler() { return r_vert; }
    KoRuler *getHorzRuler() { return r_horz; }
    KoTabChooser *getTabChooser() { return tabChooser; }
    KWDocStruct *getDocStruct() { return docStruct; }

protected slots:
    void unitChanged( QString );
    void reorganize();

protected:
    void resizeEvent( QResizeEvent *e );

    KoRuler *r_vert, *r_horz;
    KWCanvas *canvas;
    KWDocument *doc;
    KWView *view;
    KoTabChooser *tabChooser;
    KWDocStruct *docStruct;
    QSplitter *panner;
    KWLayoutWidget *left;

};

#endif
