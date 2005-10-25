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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwview_h
#define kwview_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kprinter.h>
#include <qbrush.h>

#include "koborder.h"
#include "defs.h"
#include "KWTextParag.h"

#include <koPageLayoutDia.h>
#include <koView.h>
#include <kshortcut.h>
class DCOPObject;
class KURL;

class KoPicture;
class KWDocStruct;
class KoRuler;
class KWCanvas;
class KWChild;
class KWDocument;
class KWGUI;
class KWFrame;
class KWPartFrameSet;
class KoSearchContext;
class KWStyleManager;
class KWTableDia;
class KWView;
class QResizeEvent;
//class KSpell;
class QScrollView;
class QSplitter;
class KAction;
class KActionMenu;
class KSelectAction;
class KToggleAction;
class KFontSizeAction;
class KFontAction;
class KWTextFrameSetEdit;
class KoTextFormatInterface;
class TKSelectColorAction;
class KoPartSelectAction;
class KoCharSelectDia;
class KWTextFrameset;
class KMacroCommand;
class KWFrameSet;
class KoFindReplace;
class KWFindReplace;
class KoTextFormat;
class KoTextParag;
class KoFontDia;
class KoParagDia;
class KWViewMode;
class KWFrameStyle;
class KWTableStyle;
class KoTextIterator;
class KWTableFrameSet;
class KStatusBarLabel;

class KoSpell;
#include <kspell2/broker.h>
namespace KSpell2 {
    class Dialog;
}


/******************************************************************/
/* Class: KWView                                                  */
/******************************************************************/

class KWView : public KoView
{
    Q_OBJECT

public:
    KWView( KWViewMode* viewMode, QWidget *_parent, const char *_name, KWDocument *_doc );
    virtual ~KWView();

    virtual DCOPObject* dcopObject();

    // Those methods update the UI (from the given formatting info)
    // They do NOT do anything to the text
    void showFormat( const KoTextFormat &currentFormat );
    void showAlign( int align );
    void showCounter( KoParagCounter &c );
    void showParagBorders( const KoBorder& _left, const KoBorder& _right,
                          const KoBorder& _top, const KoBorder& _bottom );
    void showFrameBorders( const KoBorder& _left, const KoBorder& _right,
                          const KoBorder& _top, const KoBorder& _bottom );

    void showStyle( const QString & styleName );
    void showRulerIndent( double _leftMargin, double _firstLine, double _rightMargin, bool rtl );
    void showZoom( int zoom ); // show a zoom value in the combo
    void setZoom( int zoom, bool updateViews ); // change the zoom value

    bool viewFrameBorders() const { return m_viewFrameBorders; }
    void setViewFrameBorders(bool b);

    /**
     * Returns the KWord global KSpell2 Broker object.
     */
    KSpell2::Broker *broker() const;

    // Currently unused
    //bool viewTableGrid() const { return m_viewTableGrid; }
    //void setViewTableGrid(bool _b) { m_viewTableGrid = _b;}

    void setNoteType(NoteType nt, bool change=true);

    KWDocument *kWordDocument()const { return m_doc; }
    KWGUI *getGUI()const { return m_gui; }
    void updateStyleList();
    void updateFrameStyleList();
    void updateTableStyleList();

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

    void changeNbOfRecentFiles(int _nb);

    void changeZoomMenu( int zoom=-1);

    void refreshMenuExpression();

    void updateGridButton();

    void deleteFrame(bool _warning=true);

    QPopupMenu * popupMenu( const QString& name );

    QPtrList<KAction> &dataToolActionList() { return m_actionList; }
    QPtrList<KAction> &variableActionList() { return m_variableActionList; }
    QPtrList<KAction> &tableActions() { return m_tableActionList; }

    enum { // bitfield
        ProvidesImage = 1,
        ProvidesPlainText = 2,
        ProvidesOasis = 4,
        ProvidesFormula = 8
    };
    static int checkClipboard( QMimeSource *data );

    bool insertInlinePicture();

    void displayFrameInlineInfo();

    void initGUIButton();

    void updateHeaderFooterButton();
    void updateFooter();
    void updateHeader();
    void switchModeView();
    void changeFootNoteMenuItem( bool b);
    void insertFile(const KURL& _url);
    void testAndCloseAllFrameSetProtectedContent();
    void updateBgSpellCheckingState();
    void updateRulerInProtectContentMode();
    void deselectAllFrames();
    void autoSpellCheck(bool b);
    void insertDirectCursor(bool b);
    void updateDirectCursorButton();

    void deleteFrameSet( KWFrameSet *);

    QPtrList<KAction> listOfResultOfCheckWord( const QString &word );

    int  tableSelectCell(const QString &tableName, uint row, uint col);
    void tableInsertRow(uint row, KWTableFrameSet *table = 0);
    void tableInsertCol(uint col, KWTableFrameSet *table = 0);
    int tableDeleteRow(const QValueList<uint>& rows, KWTableFrameSet *table = 0);
    int tableDeleteCol(const QValueList<uint>& cols, KWTableFrameSet *table = 0);

    void pasteData( QMimeSource* data );

public slots:
    void fileStatistics();
    void editCut();
    void editCopy();
    void editPaste();
    void editSelectAll();
    void editSelectAllFrames();
    void editSelectCurrentFrame();
    void editFind();
    void editFindNext();
    void editFindPrevious();
    void editReplace();
    void editDeleteFrame();
    void editCustomVariable();
    void editCustomVars();
    void editMailMergeDataBase();
    void createLinkedFrame();

    void viewTextMode();
    void viewPageMode();
    void viewPreviewMode();
    void slotViewFormattingChars();
    void slotViewFrameBorders();
    void viewHeader();
    void viewFooter();
    void viewZoom( const QString &s );

    void insertTable();
    void insertPicture();
    void insertSpecialChar();
    void insertFrameBreak();
    void insertVariable();
    void insertFootNote();
    void insertContents();
    void insertLink();
    void insertComment();
    void removeComment();
    void copyTextOfComment();

    void insertPage();
    void deletePage();

    void formatFont();
    void formatParagraph();
    void formatPage();
    void formatFrameSet();

    void slotSpellCheck();
    void extraAutoFormat();
    void extraFrameStylist();
    void extraStylist();
    void extraCreateTemplate();

    void toolsCreateText();
    void insertFormula( QMimeSource* source=0 );
    void toolsPart();

    void tableProperties();
    void tableInsertRow();
    void tableInsertCol();
    void tableResizeCol();
    void tableDeleteRow();
    void tableDeleteCol();
    void tableJoinCells();
    void tableSplitCells();
    void tableSplitCells(int col, int row);
    void tableProtectCells();
    void tableUngroupTable();
    void tableDelete();
    void tableStylist();
    void convertTableToText();
    void sortText();
    void addPersonalExpression();

    void slotStyleSelected();
    void slotFrameStyleSelected();
    void slotTableStyleSelected();
    void textStyleSelected( int );
    void frameStyleSelected( int );
    void tableStyleSelected( int );
    void textSizeSelected( int );
    void increaseFontSize();
    void decreaseFontSize();
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
    void textSuperScript();
    void textSubScript();
    void textIncreaseIndent();
    void textDecreaseIndent();
    void textDefaultFormat();
    void slotCounterStyleSelected();

    // Text and Frame borders.
    void borderOutline();
    void borderLeft();
    void borderRight();
    void borderTop();
    void borderBottom();
    void backgroundColor();

    void showFormulaToolbar( bool show );

    void configure();
    void configureCompletion();

    void newPageLayout( const KoPageLayout &_layout );
    void newLeftIndent( double _leftIndent);
    void newFirstIndent( double _firstIndent);
    void newRightIndent( double _rightIndent);

    void clipboardDataChanged();
    void tabListChanged( const KoTabulatorList & tabList );

    void updatePageInfo();
    void updateFrameStatusBarItem();
    void setTemporaryStatusBarText(const QString &text);

    void slotSpecialChar(QChar , const QString &);
    void slotFrameSetEditChanged();
    void showMouseMode( int /*KWCanvas::MouseMode*/ _mouseMode );
    void frameSelectedChanged();
    void docStructChanged(int _type);
    void slotHRulerDoubleClicked();
    void slotHRulerDoubleClicked( double );
    void slotUnitChanged(KoUnit::Unit);

    void pageNumChanged();

    void insertExpression();

    void updateTocActionText(bool hasToc);

    void changeCaseOfText();

    void editPersonalExpr();

    void slotUpdateRuler();
    void slotEmbedImage( const QString &filename );

    void insertCustomVariable();
    void insertNewCustomVariable();
    void slotSpecialCharDlgClosed();

    void refreshCustomMenu();

    void changePicture();

    void configureHeaderFooter();

    void inlineFrame();

    /** Move the selected frame above maximum 1 frame that is in front of it. */
    void raiseFrame() { adjustZOrderOfSelectedFrames(RaiseFrame); };
    /** Move the selected frame behind maximum 1 frame that is behind it */
    void lowerFrame() { adjustZOrderOfSelectedFrames(LowerFrame); };
    /** Move the selected frame(s) to be in the front most position. */
    void bringToFront() { adjustZOrderOfSelectedFrames(BringToFront); };
    /** Move the selected frame(s) to be behind all other frames */
    void sendToBack() { adjustZOrderOfSelectedFrames(SendToBack); };

    void openLink();
    void changeLink();
    void copyLink();
    void removeLink();
    void addToBookmark();
    void editComment();
    void showDocStructure();
    void showRuler();
    void viewGrid();
    void viewSnapToGrid();

    void slotSoftHyphen();
    void slotLineBreak();
    void slotNonbreakingSpace();
    void slotNonbreakingHyphen();

    void slotIncreaseNumberingLevel();
    void slotDecreaseNumberingLevel();

    void refreshAllVariable();

    void slotAllowAutoFormat();

    void slotCompletion();

    void applyAutoFormat();
    void createStyleFromSelection();
    // end of public slots
    void configureFootEndNote();
    void editFootEndNote();
    void changeFootNoteType();
    void savePicture();

    void autoSpellCheck();
    void goToFootEndNote();

    void selectFrameSet();
    void editFrameSet();
    void openDocStructurePopupMenu( const QPoint &p, KWFrameSet *frameset);

    void insertFile();

    void addBookmark();
    void selectBookmark();
    void importStyle();

    void createFrameStyle();

    void insertDirectCursor();

    void convertToTextBox();

    void slotAddIgnoreAllWord();

#if 0 // KWORD_HORIZONTAL_LINE
    // MOC_SKIP_BEGIN
    void insertHorizontalLine();
    void changeHorizontalLine();
    // MOC_SKIP_END
#endif

    void embeddedStoreInternal();

    void goToDocumentStructure();
    void goToDocument();

    void addWordToDictionary();

    void deleteFrameSet();
    void editFrameSetProperties();

protected slots:
    virtual void slotChildActivated( bool a ); ///< from KoView
    void slotSetInitialPosition();

    void spellCheckerMisspelling( const QString &, int );
    void spellCheckerCorrected( const QString &, int, const QString & );
    void spellCheckerDone( const QString & );
    void spellCheckerCancel();

    void spellAddAutoCorrect (const QString & originalword, const QString & newword);
    void slotApplyFont();
    void slotApplyParag();
    void slotPageLayoutChanged( const KoPageLayout& layout );
    void slotChangeCaseState(bool b);
    void slotChangeCutState(bool b);
    void slotCorrectWord();

protected:
    void addVariableActions( int type, const QStringList & texts,
                             KActionMenu * parentMenu, const QString & menuText );

    void loadexpressionActions( KActionMenu * parentMenu);

    void createExpressionActions( KActionMenu * parentMenu,const QString& filename,int &i , bool insertSepar, const QMap<QString, KShortcut>& personalShortCut );

    void insertPicture( const KoPicture& picture, const bool makeInline, const bool _keepRatio );

    void showParagraphDialog( int initialPage = -1, double initialTabPos = 0.0 );

    KWTextFrameSetEdit *currentTextEdit() const;
    /** The current text-edit if there is one, otherwise the selected text objects
     * This is what the "bold", "italic" etc. buttons apply to. */
    QPtrList<KoTextFormatInterface> applicableTextInterfaces() const;

    void setupActions();

    virtual void resizeEvent( QResizeEvent *e );
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    virtual void updateReadWrite( bool readwrite );
    void updateTableActions( int nbFramesSelected );

    enum borderChanged { BorderOutline, BorderLeft, BorderRight, BorderTop, BorderBottom };
    void borderShowValues();
    void borderSet( borderChanged type);


    void startKSpell();
    void clearSpellChecker(bool cancelSpellCheck = false );

    QValueList<QString> getInlineFramesets( const QDomNode &framesetElem );

    // Helper stuff for the frame adjustment;
    enum moveFrameType  { RaiseFrame, LowerFrame, BringToFront, SendToBack };
    void adjustZOrderOfSelectedFrames(moveFrameType moveType);
    void increaseAllZOrdersAbove(int refZOrder, int pageNum, const QPtrList<KWFrame> frameSelection);
    void decreaseAllZOrdersUnder(int refZOrder, int pageNum, const QPtrList<KWFrame> frameSelection);
    int raiseFrame(const QPtrList<KWFrame> frameSelection, const KWFrame *frame);
    int lowerFrame(const QPtrList<KWFrame> frameSelection, const KWFrame *frame);
    int bringToFront(const QPtrList<KWFrame> frameSelection, const KWFrame *frame);
    int sendToBack(const QPtrList<KWFrame> frameSelection, const KWFrame *frame);
    void textStyleSelected( KoParagStyle *_sty );
    void frameStyleSelected( KWFrameStyle *_sty );
    void tableStyleSelected( KWTableStyle *_sty );
    void changeFootEndNoteState();
    void refreshDeletePageAction();

    void spellCheckerRemoveHighlight();

private:
    KWDocument *m_doc;

    // TODO use m_ notation
    KAction *actionFileStatistics;
    KAction *actionEditCut;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditSelectAll;
    KAction *actionEditSelectCurrentFrame;
    KAction *actionEditDelFrame;
    KAction *actionCreateLinkedFrame;
    KAction *actionRaiseFrame;
    KAction *actionLowerFrame;
    KAction *actionSendBackward;
    KAction *actionBringToFront;

    KAction *actionEditCustomVars;
    KAction *actionEditCustomVarsEdit;
    KAction *actionEditFind;
    KAction *actionEditFindNext;
    KAction *actionEditFindPrevious;
    KAction *actionEditReplace;
    KAction *actionApplyAutoFormat;

    KToggleAction *actionViewTextMode;
    KToggleAction *actionViewPageMode;
    KToggleAction *actionViewPreviewMode;

    KToggleAction *actionViewFormattingChars;
    KToggleAction *actionViewFrameBorders;
    KToggleAction *actionViewHeader;
    KToggleAction *actionViewFooter;
    KToggleAction *actionViewFootNotes;
    KToggleAction *actionViewEndNotes;
    KToggleAction *actionShowDocStruct;
    KToggleAction *actionShowRuler;
    KToggleAction *actionViewShowGrid;
    KToggleAction *actionViewSnapToGrid;
    KToggleAction *actionAllowAutoFormat;

    KToggleAction *actionAllowBgSpellCheck;

    KSelectAction *actionViewZoom;

    KAction *actionInsertFrameBreak;
    KAction *actionInsertFootEndNote;
    KAction *actionInsertContents;
    KAction *actionInsertLink;
    KAction *actionInsertComment;
    KAction *actionEditComment;
    KAction *actionRemoveComment;
    KAction *actionCopyTextOfComment;
    //KAction *actionInsertPage;
    KAction *actionDeletePage;

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
    KAction *actionInsertTable;
    KAction *actionAutoFormat;

    KToggleAction *actionToolsCreateText;
    KToggleAction *actionToolsCreatePix;
    KoPartSelectAction *actionToolsCreatePart;

    KAction *actionFormatFont;
    KAction *actionFormatDefault;
    KAction *actionFormatFrameStylist;
    KAction *actionFormatStylist;
    KAction *actionFormatPage;

    KAction *actionFontSizeIncrease;
    KAction *actionFontSizeDecrease;

    KFontSizeAction *actionFormatFontSize;
    KFontAction *actionFormatFontFamily;
    KSelectAction *actionFormatStyle;
    KActionMenu *actionFormatStyleMenu;
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
    KActionMenu *actionFormatBullet;
    KActionMenu *actionFormatNumber;
    KToggleAction *actionFormatSuper;
    KToggleAction *actionFormatSub;
    KAction* actionInsertSpecialChar;

    // Text and Frame borders.
    KSelectAction *actionFrameStyle;
    KActionMenu *actionFrameStyleMenu;
    KSelectAction *actionTableStyle;
    KActionMenu *actionTableStyleMenu;
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
        KoBorder left;    ///< Values specific to left border.
        KoBorder right;   ///< right.
        KoBorder top;     ///< top.
        KoBorder bottom;  ///< bottom.
    } m_border;

    KAction *actionTableDelRow;
    KAction *actionTableDelCol;
    KAction *actionTableInsertRow;
    KAction *actionTableInsertCol;
    KAction *actionTableResizeCol;
    KAction *actionTableJoinCells;
    KAction *actionTableSplitCells;
    KAction *actionConvertTableToText;
    KAction *actionSortText;
    KAction *actionAddPersonalExpression;
    KToggleAction *actionTableProtectCells;

    KAction *actionTableUngroup;
    KAction *actionTableDelete;

    KAction *actionTableStylist;
    KAction *actionTablePropertiesMenu;
    KAction *actionTableProperties;

    KAction *actionExtraCreateTemplate;

    KAction *actionChangeCase;

    KAction *actionEditPersonnalExpr;

    KAction *actionConfigure;

    KAction *actionConfigureCompletion;


    KAction *actionChangePicture;

    KAction *actionSavePicture;

    KAction *actionConfigureHeaderFooter;
    KToggleAction *actionInlineFrame;

    KAction *actionOpenLink;
    KAction *actionChangeLink;
    KAction *actionCopyLink;
    KAction *actionAddLinkToBookmak;
    KAction *actionRemoveLink;

    KAction *actionRefreshAllVariable;
    KAction *actionCreateStyleFromSelection;

    KAction *actionConfigureFootEndNote;

    KAction *actionEditFootEndNote;

    KAction *actionChangeFootNoteType;

    KAction *actionGoToFootEndNote;


    KAction *actionEditFrameSet;
    KAction *actionDeleteFrameSet;
    KAction *actionSelectedFrameSet;
    KAction *actionInsertFile;

    KAction *actionAddBookmark;
    KAction *actionSelectBookmark;

    KAction *actionImportStyle;

    KAction *actionCreateFrameStyle;

    KAction *actionConvertToTextBox;

    KAction *actionSpellIgnoreAll;
    KAction *actionSpellCheck;

#if 0 // KWORD_HORIZONTAL_LINE
    // MOC_SKIP_BEGIN
    KAction *actionInsertHorizontalLine;
    KAction *actionChangeHorizontalLine;
    // MOC_SKIP_END
#endif
    KAction *actionEditFrameSetProperties;
    KToggleAction *actionEmbeddedStoreInternal;

    KAction *actionAddWordToPersonalDictionary;

    KToggleAction *actionInsertDirectCursor;

    KAction *actionGoToDocumentStructure;
    KAction *actionGoToDocument;


    KoCharSelectDia *m_specialCharDlg;
    KoFontDia *m_fontDlg;
    KoParagDia *m_paragDlg;

    KWGUI *m_gui;

    DCOPObject *dcop;

    KoSearchContext *m_searchEntry, *m_replaceEntry;
    KWFindReplace *m_findReplace;

    QPtrList<KAction> m_actionList; // for the kodatatools
    QPtrList<KAction> m_variableActionList;
    QPtrList<KAction> m_tableActionList;

    int m_currentPage; ///< 0-based current page number

    // Statusbar items
    KStatusBarLabel * m_sbPageLabel; ///< 'Current page number and page count' label
    KStatusBarLabel * m_sbFramesLabel; ///< Info about selected frames

    // Zoom values for each viewmode ( todo a viewmode enum and a qmap or so )
    int m_zoomViewModeNormal;
    int m_zoomViewModePreview;

    bool m_viewFrameBorders /*, m_viewTableGrid*/;

    /// Spell-checking
    struct {
        KoSpell *kospell;
        KMacroCommand * macroCmdSpellCheck;
        QStringList replaceAll;
        KoTextIterator * textIterator;
        KSpell2::Dialog *dlg;
     } m_spell;
    KSpell2::Broker::Ptr m_broker;


    KWFrameSet *fsInline;

};

/******************************************************************/
/* Class: KWGUI                                                   */
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
    KWGUI( KWViewMode* viewMode, QWidget *parent, KWView *_view );

    void showGUI();

    KWView *getView()const { return view; }
    KWCanvas *canvasWidget()const { return canvas; }
    KoRuler *getVertRuler()const { return r_vert; }
    KoRuler *getHorzRuler()const { return r_horz; }
    KoTabChooser *getTabChooser()const { return tabChooser; }
    KWDocStruct *getDocStruct()const { return docStruct; }

public slots:
    void reorganize();

protected slots:
    void unitChanged( KoUnit::Unit );

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

/******************************************************************/
/* Class: KWStatisticsDialog                                      */
/******************************************************************/
/**
* Is not intended to inherit from
*/

class KWStatisticsDialog : public KDialogBase
{
    Q_OBJECT

public:
    KWStatisticsDialog( QWidget *_parent, KWDocument *_doc );
    bool wasCanceled()const { return m_canceled; }

private:
    KWDocument *m_doc;
    QWidget *m_parent;
    bool m_canceled;
    QLabel *resultLabelAll[7];
    QLabel *resultLabelSelected[7];
    QLabel *resultGeneralLabel[6];
    void addBox( QFrame *page, QLabel **resultLabel, bool calcWithFootNoteCheckbox );
    void addBoxGeneral( QFrame *page, QLabel **resultLabel );

    bool calcStats( QLabel **resultLabel, bool selection, bool useFootEndNote );
    void calcGeneral( QLabel **resultLabel );
    bool docHasSelection()const;
    double calcFlesch(ulong sentences, ulong words, ulong syllables);
private slots:
    void slotRefreshValue(bool);
};

#endif
