/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>

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

#ifndef kwview_h
#define kwview_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kprinter.h>

#include <KoBorder.h>
#include "defs.h"
#include "KWTextParag.h"

#include <KoPageLayoutDia.h>
#include <KoView.h>
#include <KoPoint.h>
#include <kshortcut.h>
#include <KoZoomMode.h>

#include <qbrush.h>

//Added by qt3to4:
#include <Q3ValueList>
#include <QResizeEvent>
#include <Q3PopupMenu>
#include <Q3PtrList>

class KWPage;
class KWViewMode;
class KWDocStruct;
class KWCanvas;
class KWDocument;
class KWGUI;
class KWFrame;
class KWFrameView;
class KWFrameViewManager;
class KWTextFrameSetEdit;
class KMacroCommand;
class KWFrameSet;
class KWFindReplace;
class KWFrameStyle;
class KWTableStyle;
class KWTableFrameSet;

class KoPicture;
class KoSearchContext;
class KoTextFormatInterface;
class KoRuler;
class TKSelectColorAction;
class KoPartSelectAction;
class KoCharSelectDia;
class KoTextFormat;
class KoFontDia;
class KoParagDia;
class KoTextIterator;

class DCOPObject;
class KStatusBarLabel;
class KoSpell;
class KUrl;
class KAction;
class KActionMenu;
class KSelectAction;
class KToggleAction;
class KFontSizeAction;
class KFontAction;
class QResizeEvent;
class QSplitter;

#include <kspell2/broker.h>
#include <kvbox.h>
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
    KWView( const QString& viewMode, QWidget *parent, const char *name, KWDocument *doc );
    virtual ~KWView();

    virtual DCOPObject* dcopObject();

    // Those methods update the UI (from the given formatting info)
    // They do NOT do anything to the text
    void showFormat( const KoTextFormat &currentFormat );
    void showAlign( int align );
    void showCounter( KoParagCounter &c );
    void showSpacing( int spacing );
    /**
     * Updates the checked state of the border buttons based on the parameters.
     * @param left represents the left border being currently visible
     * @param right represents the right border being currently visible
     * @param top represents the top border being currently visible
     * @param bottom represents the bottom border being currently visible
     */
    void updateBorderButtons( const KoBorder& left, const KoBorder& right,
                          const KoBorder& top, const KoBorder& bottom );

    void showStyle( const QString & styleName );
    void showRulerIndent( double leftMargin, double firstLine, double rightMargin, bool rtl );
    void showZoom( int zoom ); // show a zoom value in the combo
    void showZoom( const QString& zoom ); // show a zoom value in the combo
    void setZoom( int zoom, bool updateViews ); // change the zoom value

    bool viewFrameBorders() const { return m_viewFrameBorders; }
    void setViewFrameBorders(bool b);

    /**
     * Returns the KWord global KSpell2 Broker object.
     */
    KSpell2::Broker::Ptr broker() const;

    void setNoteType(NoteType nt, bool change=true);

    KWDocument *kWordDocument()const { return m_doc; }
    KWGUI *getGUI()const { return m_gui; }
    KWViewMode* viewMode() const;

    void updateStyleList();
    void updateFrameStyleList();
    void updateTableStyleList();

    void initGui();

    /// returns the current page number this view is looking at
    int currentPage() const;

    /**
     * Overloaded from View
     */
    bool doubleClickActivation() const;
    /**
     * Overloaded from View
     */
    QWidget* canvas() const;
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

    virtual QPoint applyViewTransformations( const QPoint& ) const;
    virtual QPoint reverseViewTransformations( const QPoint& ) const;

    void changeNbOfRecentFiles(int nb);

    void changeZoomMenu( int zoom=-1);

    void refreshMenuExpression();

    void updateGridButton();

    void deleteFrame(bool warning=true);

    Q3PopupMenu * popupMenu( const QString& name );

    QList<KAction*> &dataToolActionList() { return m_actionList; }
    QList<KAction*> &variableActionList() { return m_variableActionList; }
    QList<KAction*> &tableActions() { return m_tableActionList; }

    enum { // bitfield
        ProvidesImage = 1,
        ProvidesPlainText = 2,
        ProvidesOasis = 4,
        ProvidesFormula = 8
    };
    static int checkClipboard( QMimeData *data );

    bool insertInlinePicture();

    void displayFrameInlineInfo();

    void initGUIButton();

    void updateHeaderFooterButton();
    void updateFooter();
    void updateHeader();
    void switchModeView();
    void changeFootNoteMenuItem( bool b);
    void insertFile(const KUrl& url);
    void testAndCloseAllFrameSetProtectedContent();
    void updateBgSpellCheckingState();
    void updateRulerInProtectContentMode();
    void deselectAllFrames();
    void autoSpellCheck(bool b);
    void insertDirectCursor(bool b);
    void updateDirectCursorButton();

    void deleteFrameSet( KWFrameSet *);

    QList<KAction*> listOfResultOfCheckWord( const QString &word );

    int  tableSelectCell(const QString &tableName, uint row, uint col);
    void tableInsertRow(uint row, KWTableFrameSet *table = 0);
    void tableInsertCol(uint col, KWTableFrameSet *table = 0);
    int tableDeleteRow(const Q3ValueList<uint>& rows, KWTableFrameSet *table = 0);
    int tableDeleteCol(const Q3ValueList<uint>& cols, KWTableFrameSet *table = 0);

    void pasteData( const QMimeData* data, bool drop );

    KWFrameViewManager* frameViewManager() const;

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
    void updateZoom();

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
    void insertFormula( QMimeData* source=0 );
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
    void tableProtectCells(bool);
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
    void textSpacingSingle();
    void textSpacingOneAndHalf();
    void textSpacingDouble();
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

    void newPageLayout( const KoPageLayout &layout );
    void newLeftIndent( double leftIndent);
    void newFirstIndent( double firstIndent);
    void newRightIndent( double rightIndent);

    void clipboardDataChanged();
    void tabListChanged( const KoTabulatorList & tabList );

    void updatePageInfo();
    void updateFrameStatusBarItem();
    void setTemporaryStatusBarText(const QString &text);

    void slotSpecialChar(QChar , const QString &);
    void slotFrameSetEditChanged();
    void showMouseMode( int /*KWCanvas::MouseMode*/ mouseMode );
    void frameSelectedChanged();
    void docStructChanged(int type);
    void documentModified( bool );
    void changeOverwriteMode( bool );
    void slotHRulerDoubleClicked();
    void slotHRulerDoubleClicked( double );
    void slotUnitChanged(KoUnit::Unit);

    void numPagesChanged();

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

    void configureFootEndNote();
    void editFootEndNote();
    void changeFootNoteType();
    void savePicture();

    void autoSpellCheck();
    void goToFootEndNote();

    // Document Structure Area popup menu.
    void docStructEdit();
    void docStructSelect();
    void docStructDelete();
    void docStructProperties();
    void openDocStructurePopupMenu( const QPoint &p, KWFrameSet *frameset, KWTextParag *parag);

    void insertFile();

    void addBookmark();
    void selectBookmark();
    void importStyle();

    void createFrameStyle();

    void insertDirectCursor();

    void convertToTextBox();

    void slotAddIgnoreAllWord();

    void embeddedStoreInternal();

    void goToDocumentStructure();
    void goToDocument();

    void addWordToDictionary();

    void insertPicture( const KoPicture& picture, const bool makeInline, const bool keepRatio, int suggestedWidth, int suggestedHeight );

    // end of public methods
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

    /// This slot is called when the document has finished loading.
    void slotDocumentLoadingCompleted();

protected:
    void addVariableActions( int type, const QStringList & texts,
                             KActionMenu * parentMenu, const QString & menuText );

    void loadexpressionActions( KActionMenu * parentMenu);

    void createExpressionActions( KActionMenu * parentMenu,const QString& filename,int &i , bool insertSepar, const QMap<QString, KShortcut>& personalShortCut );

    void showParagraphDialog( int initialPage = -1, double initialTabPos = 0.0 );

    KWTextFrameSetEdit *currentTextEdit() const;
    /** The current text-edit if there is one, otherwise the selected text objects
     * This is what the "bold", "italic" etc. buttons apply to. */
    Q3PtrList<KoTextFormatInterface> applicableTextInterfaces() const;

    void setupActions();

    virtual void resizeEvent( QResizeEvent *e );
    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

    virtual void updateReadWrite( bool readwrite );
    /**
     * Make sure the actions related to tables have the right texts and enabled options.
     * @param selectFrames a list of selected frames
     * @see KWFrameViewManager::selectedFrames()
     */
    void updateTableActions( Q3ValueList<KWFrameView*> selectFrames);


    void startKSpell();
    void clearSpellChecker(bool cancelSpellCheck = false );

    Q3ValueList<QString> getInlineFramesets( const QDomNode &framesetElem );

    // Helper stuff for the frame adjustment;
    enum MoveFrameType  { RaiseFrame, LowerFrame, BringToFront, SendToBack };
    void adjustZOrderOfSelectedFrames(MoveFrameType moveType);
    void increaseAllZOrdersAbove(int refZOrder, int pageNum, const Q3PtrList<KWFrame>& frameSelection);
    void decreaseAllZOrdersUnder(int refZOrder, int pageNum, const Q3PtrList<KWFrame>& frameSelection);
    int raiseFrame(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame);
    int lowerFrame(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame);
    int bringToFront(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame);
    int sendToBack(const Q3PtrList<KWFrame>& frameSelection, const KWFrame *frame);
    void textStyleSelected( KoParagStyle *sty );
    void frameStyleSelected( KWFrameStyle *sty );
    void tableStyleSelected( KWTableStyle *sty );
    void changeFootEndNoteState();
    void refreshDeletePageAction();

    void spellCheckerRemoveHighlight();

    void setSpacing( KoParagLayout::SpacingType spacing, const QString &commandName );

private:  // methods
    void deleteSelectedFrames();
    void borderChanged(KoBorder::BorderType type);
    void updateZoomControls();

private: // variables
    KWDocument *m_doc;

    QActionGroup* m_paragraphStyleActionGroup;
    QActionGroup* m_frameStyleActionGroup;
    QActionGroup* m_tableStyleActionGroup;

    KAction *m_actionFileStatistics;
    KAction *m_actionEditCut;
    KAction *m_actionEditCopy;
    KAction *m_actionEditPaste;
    KAction *m_actionEditSelectAll;
    KAction *m_actionEditSelectCurrentFrame;
    KAction *m_actionEditDelFrame;
    KAction *m_actionCreateLinkedFrame;
    KAction *m_actionRaiseFrame;
    KAction *m_actionLowerFrame;
    KAction *m_actionSendBackward;
    KAction *m_actionBringToFront;

    KAction *m_actionEditCustomVars;
    KAction *m_actionEditCustomVarsEdit;
    KAction *m_actionEditFind;
    KAction *m_actionEditFindNext;
    KAction *m_actionEditFindPrevious;
    KAction *m_actionEditReplace;
    KAction *m_actionApplyAutoFormat;

    KToggleAction *m_actionViewTextMode;
    KToggleAction *m_actionViewPageMode;
    KToggleAction *m_actionViewPreviewMode;

    KToggleAction *m_actionViewFormattingChars;
    KToggleAction *m_actionViewFrameBorders;
    KToggleAction *m_actionViewHeader;
    KToggleAction *m_actionViewFooter;
    KToggleAction *m_actionViewFootNotes;
    KToggleAction *m_actionViewEndNotes;
    KToggleAction *m_actionShowDocStruct;
    KToggleAction *m_actionShowRuler;
    KToggleAction *m_actionViewShowGrid;
    KToggleAction *m_actionViewSnapToGrid;
    KToggleAction *m_actionAllowAutoFormat;

    KToggleAction *m_actionAllowBgSpellCheck;

    KSelectAction *m_actionViewZoom;

    KAction *m_actionInsertFrameBreak;
    KAction *m_actionInsertFootEndNote;
    KAction *m_actionInsertContents;
    KAction *m_actionInsertLink;
    KAction *m_actionInsertComment;
    KAction *m_actionEditComment;
    KAction *m_actionRemoveComment;
    KAction *m_actionCopyTextOfComment;
    //KAction *actionInsertPage;
    KAction *m_actionDeletePage;

    KActionMenu *actionInsertVariable;
    struct VariableDef {
        int type;
        int subtype;
    };
    typedef QMap<KAction *, VariableDef> VariableDefMap;
    KActionMenu *m_actionInsertExpression;

    KActionMenu *m_actionInsertCustom;

    VariableDefMap m_variableDefMap;
    KAction *m_actionInsertFormula;
    KAction *m_actionInsertTable;
    KAction *m_actionAutoFormat;

    KToggleAction *m_actionToolsCreateText;
    KToggleAction *m_actionToolsCreatePix;
    KoPartSelectAction *m_actionToolsCreatePart;

    KAction *m_actionFormatFont;
    KAction *m_actionFormatDefault;
    KAction *m_actionFormatFrameStylist;
    KAction *m_actionFormatStylist;
    KAction *m_actionFormatPage;

    KAction *m_actionFontSizeIncrease;
    KAction *m_actionFontSizeDecrease;

    KFontSizeAction *m_actionFormatFontSize;
    KFontAction *m_actionFormatFontFamily;
    KSelectAction *m_actionFormatStyle;
    KActionMenu *m_actionFormatStyleMenu;
    KToggleAction *m_actionFormatBold;
    KToggleAction *m_actionFormatItalic;
    KToggleAction *m_actionFormatUnderline;
    KToggleAction *m_actionFormatStrikeOut;
    TKSelectColorAction *m_actionFormatColor;
    KToggleAction *m_actionFormatAlignLeft;
    KToggleAction *m_actionFormatAlignCenter;
    KToggleAction *m_actionFormatAlignRight;
    KToggleAction *m_actionFormatAlignBlock;
    KToggleAction *m_actionFormatSpacingSingle;
    KToggleAction *m_actionFormatSpacingOneAndHalf;
    KToggleAction *m_actionFormatSpacingDouble;
    KAction *m_actionFormatParag;
    KAction *m_actionFormatFrameSet;
    KAction *m_actionFormatIncreaseIndent;
    KAction *m_actionFormatDecreaseIndent;
    KActionMenu *m_actionFormatBullet;
    KActionMenu *m_actionFormatNumber;
    KToggleAction *m_actionFormatSuper;
    KToggleAction *m_actionFormatSub;
    KAction* m_actionInsertSpecialChar;

    // Text and Frame borders.
    KSelectAction *m_actionFrameStyle;
    KActionMenu *m_actionFrameStyleMenu;
    KSelectAction *m_actionTableStyle;
    KActionMenu *m_actionTableStyleMenu;
    KToggleAction *m_actionBorderLeft;
    KToggleAction *m_actionBorderRight;
    KToggleAction *m_actionBorderTop;
    KToggleAction *m_actionBorderBottom;
    KToggleAction *m_actionBorderOutline;
    TKSelectColorAction *m_actionBorderColor;
    KSelectAction *m_actionBorderWidth;
    KSelectAction *m_actionBorderStyle;
    TKSelectColorAction *m_actionBackgroundColor;

    KAction *m_actionTableDelRow;
    KAction *m_actionTableDelCol;
    KAction *m_actionTableInsertRow;
    KAction *m_actionTableInsertCol;
    KAction *m_actionTableResizeCol;
    KAction *m_actionTableJoinCells;
    KAction *m_actionTableSplitCells;
    KAction *m_actionConvertTableToText;
    KAction *m_actionSortText;
    KAction *m_actionAddPersonalExpression;
    KToggleAction *m_actionTableProtectCells;

    KAction *m_actionTableUngroup;
    KAction *m_actionTableDelete;

    KAction *m_actionTableStylist;
    KAction *m_actionTablePropertiesMenu;
    KAction *m_actionTableProperties;

    KAction *m_actionExtraCreateTemplate;

    KAction *m_actionChangeCase;

    KAction *m_actionEditPersonnalExpr;

    KAction *m_actionConfigure;

    KAction *m_actionConfigureCompletion;

    KAction *m_actionSavePicture;

    KAction *m_actionConfigureHeaderFooter;
    KToggleAction *m_actionInlineFrame;

    KAction *m_actionOpenLink;
    KAction *m_actionChangeLink;
    KAction *m_actionCopyLink;
    KAction *m_actionAddLinkToBookmak;
    KAction *m_actionRemoveLink;

    KAction *m_actionRefreshAllVariable;
    KAction *m_actionCreateStyleFromSelection;

    KAction *m_actionConfigureFootEndNote;

    KAction *m_actionEditFootEndNote;

    KAction *m_actionChangeFootNoteType;

    KAction *m_actionGoToFootEndNote;

    // Document Structure Area popup menu.
    KAction *m_actionDocStructEdit;
    KAction *m_actionDocStructSpeak;
    KAction *m_actionDocStructSelect;
    KAction *m_actionDocStructDelete;
    KAction *m_actionDocStructProperties;

    KAction *m_actionInsertFile;

    KAction *m_actionAddBookmark;
    KAction *m_actionSelectBookmark;

    KAction *m_actionImportStyle;

    KAction *m_actionCreateFrameStyle;

    KAction *m_actionConvertToTextBox;

    KAction *m_actionSpellIgnoreAll;
    KAction *m_actionSpellCheck;

    KToggleAction *m_actionEmbeddedStoreInternal;

    KAction *m_actionAddWordToPersonalDictionary;

    KToggleAction *m_actionInsertDirectCursor;

    KAction *m_actionGoToDocumentStructure;
    KAction *m_actionGoToDocument;


    KoCharSelectDia *m_specialCharDlg;
    KoFontDia *m_fontDlg;
    KoParagDia *m_paragDlg;

    KWGUI *m_gui;

    DCOPObject *m_dcop;

    KoSearchContext *m_searchEntry, *m_replaceEntry;
    KWFindReplace *m_findReplace;

    QList<KAction*> m_actionList; // for the kodatatools
    QList<KAction*> m_variableActionList;
    QList<KAction*> m_tableActionList;

    KWPage *m_currentPage; ///< current page number

    // Statusbar items
    KStatusBarLabel* m_sbPageLabel; ///< 'Current page number and page count' label
    KStatusBarLabel* m_sbModifiedLabel;
    KStatusBarLabel* m_sbFramesLabel; ///< Info about selected frames
    KStatusBarLabel* m_sbOverwriteLabel;
    KStatusBarLabel* m_sbUnitLabel;
    KStatusBarLabel* m_sbZoomLabel;

    // Zoom values for each viewmode ( todo a viewmode enum and a qmap or so )
    struct KWZoomValueStore {
        int m_zoom;
        KoZoomMode::Mode m_zoomMode;
    };
    KWZoomValueStore m_zoomViewModeNormal;
    KWZoomValueStore m_zoomViewModePreview;

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


    KWFrameSet *m_fsInline;

    // Split cell in table
    struct {
        unsigned int columns, rows;
    } m_tableSplit;
};

/******************************************************************/
/* Class: KWGUI                                                   */
/******************************************************************/

class KWGUI : public KHBox
{
    Q_OBJECT

public:
    KWGUI( const QString& viewMode, QWidget *parent, KWView *view );

    void showGUI();

    KWView *getView()const { return m_view; }
    KWCanvas *canvasWidget()const { return m_canvas; }
    KoRuler *getVertRuler()const { return m_vertRuler; }
    KoRuler *getHorzRuler()const { return m_horRuler; }
    KoTabChooser *getTabChooser()const { return m_tabChooser; }
    KWDocStruct *getDocStruct()const { return m_docStruct; }

public slots:
    void reorganize();

protected slots:
    void unitChanged( KoUnit::Unit );

protected:
    void resizeEvent( QResizeEvent *e );

    // A pointer to the view.
    KWView          *m_view;

    // The left side
    KWDocStruct     *m_docStruct;

    // The right side
    QWidget         *m_right;	// The layout widget.

    KoTabChooser    *m_tabChooser;
    KoRuler         *m_vertRuler;
    KoRuler         *m_horRuler;
    KWCanvas        *m_canvas;

    QSplitter       *m_panner;

};

#endif
