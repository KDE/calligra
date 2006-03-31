/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef kpresenter_view_h
#define kpresenter_view_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qguardedptr.h>
#include <qtimer.h>
#include <qdatetime.h>

#include <KoView.h>
#include "global.h"
#include <KoBrush.h>
#include <KoPen.h>
#include <KoRuler.h>

class QPopupMenu;
class QSplitter;
class QLabel;
class DCOPObject;
class KPrView;
class KPrBackDialog;
class KoRuler;
class QScrollBar;
class AFChoose;
class KPrPropertyEditor;
class KPrPgConfDia;
class KPrSlideTransitionDia;
class KPrRotationDialogImpl;
class KPrShadowDialogImpl;
class KPrImageEffectDia;
class ConfPieDia;
class ConfRectDia;
class ConfPolygonDia;
class ConfPictureDia;
class KPrPresDurationDia;
class QToolButton;
class KPrSideBar;
class KPrNoteBar;

class KAction;
class KActionMenu;
class KToggleAction;
class TKSelectColorAction;
class KoPartSelectAction;
class KoPicture;
class KoParagStyle;
class KoLineStyleAction;
class KoLineWidthAction;
class KoRect;

class KoCharSelectDia;
class KoTextFormat;
class KoTextObject;
class KoTextZoomHandler;

class KCommand;
class KMacroCommand;
class KFontSizeAction;
class KSelectAction;
class KFontAction;
class KoParagCounter;
class KActionMenu;
class KoSearchContext;
class KPrFindReplace;
class KPrCanvas;
class KoFontDia;
class KoParagDia;
class KPrObject;
class KPrPixmapObject;
class KPrDocument;
class KPrPage;
class KPrTextObject;
class KoTextIterator;
class KStatusBarLabel;

class KoSpell;
#include <kspell2/broker.h>
namespace KSpell2 {
    class Dialog;
}

class PageBase : public QWidget
{
public:
    PageBase( QWidget *parent, KPrView *v ) : QWidget( parent ), view( v ) {}
    void resizeEvent( QResizeEvent *e );

private:
    KPrView *view;

};


class KPrView : public KoView
{
    friend class PageBase;
    Q_OBJECT

public:
    KPrView( KPrDocument* _doc, QWidget *_parent = 0, const char *_name = 0 );
    ~KPrView();

    void initGui();
    virtual DCOPObject* dcopObject();

    // for dcop interface
    int getCurrentPresPage() const;
    int getCurrentPresStep() const;
    int getPresStepsOfPage() const;
    int getNumPresPages() const;
    bool gotoPresPage( int pg );

    virtual void setupPrinter( KPrinter &printer );
    virtual void print( KPrinter &printer );

    void showFormat( const KoTextFormat &format );

    void showZoom( int zoom ); // show a zoom value in the combo
    void setZoom( int zoom, bool updateViews ); // change the zoom value
    void changeZoomMenu( int zoom=-1);
    void showStyle( const QString & styleName );
    void updateStyleList();

    KoTextZoomHandler *zoomHandler() const;

    //used this function when we when to print/create web presentation etc...
    //=>we unzoom it.
    void unZoomDocument(int &dpiX,int &dpiY);
    void zoomDocument(int zoom);

    //refresh footer/header button when we make undo/redo show/hide header/footer
    void updateHeaderFooterButton();
    void updateDisplayObjectMasterPageButton();

    void updateDisplayBackgroundButton();
    virtual int leftBorder() const { return canvas()->mapTo(const_cast<KPrView *>(this), QPoint(0,0)).x(); };
    virtual int rightBorder() const { return width() - canvas()->mapTo(const_cast<KPrView *>(this), QPoint(canvas()->width(), 0)).x(); };
    virtual int topBorder() const { return canvas()->mapTo(const_cast<KPrView *>(this), QPoint(0,0)).y(); };
    virtual int bottomBorder() const { return height() - canvas()->mapTo(const_cast<KPrView *>(this), QPoint(0, canvas()->height())).y(); };

    void updateGuideLineButton();

    void updateGridButton();
    void savePicture( const QString& oldName, KoPicture& picture);
    void savePicture( KPrPixmapObject* obj );

    void insertFile(const QString &path);
    void testAndCloseAllTextObjectProtectedContent();
    void updateRulerInProtectContentMode();
    QPtrList<KAction> listOfResultOfCheckWord( const QString &word );

    /**
     * Returns the KPresenter global KSpell2 Broker object.
     */
    KSpell2::Broker *broker() const;
    bool editMaster() const { return m_editMaster;}

signals:
    void currentPageChanged( int );
    void presentationFinished();

public slots:
    // edit menu
    void editCut();
    void editCopy();
    void editPaste();
    void editDelete();
    void editSelectAll();
    void editDeSelectAll();
    void editCopyPage();
    void editDuplicatePage();
    void editDelPage();
    void editFind();
    void editReplace();

    // view menu
    void viewShowSideBar();
    void viewShowNoteBar();
    void viewSlideMaster();
    void viewZoom( const QString &s );

    void viewFooter();
    void viewHeader();
    void insertComment();
    void editComment();

    void viewGuideLines();

    void viewGrid();

    void viewSnapToGrid();

    // insert menu
    void insertPage();
    void insertPicture();
    void insertPicture(const QString &file);
    void insertSpecialChar();

    // tools menu
    void toolsMouse();
    void toolsRotate();
    void toolsZoom();
    void toolsLine();
    void toolsLinePopup();
    void toolsShapePopup();
    void toolsRectangle();
    void toolsCircleOrEllipse();
    void toolsPie();
    void toolsText();
    void toolsAutoform();
    void toolsDiagramm();
    void toolsTable();
    void toolsFormula();
    void toolsObject();
    void toolsFreehand();
    void toolsPolyline();
    void toolsQuadricBezierCurve();
    void toolsCubicBezierCurve();
    void toolsConvexOrConcavePolygon();
    void toolsClosedLinePopup();
    void toolsClosedFreehand();
    void toolsClosedPolyline();
    void toolsClosedQuadricBezierCurve();
    void toolsClosedCubicBezierCurve();

    // extra menu
    void extraProperties();
    void extraRaise();
    void extraLower();
    void extraRotate();
    void extraSendBackward();
    void extraBringForward();
    void extraArrangePopup();

    void extraShadow();
    void extraBackground();
    void extraLayout();
    void extraConfigure();
    void extraLineBegin();
    void extraLineEnd();
    void extraWebPres();
    void extraMSPres();
    void extraCreateTemplate();
    void extraDefaultTemplate();
    void extraGroup();
    void extraUnGroup();
    void extraPenStyle( int newStyle );
    void extraPenWidth( double newWidth );

    void configureCompletion();

    void extraAlignObjLeft();
    void extraAlignObjCenterH();
    void extraAlignObjRight();
    void extraAlignObjTop();
    void extraAlignObjCenterV();
    void extraAlignObjBottom();

    // screen menu
    void screenConfigPages();
    void screenAssignEffect();
    void screenTransEffect();
    void screenStart();
    void screenStartFromFirst();
    void screenStop();
    void screenPause();
    void screenFirst();
    /**
     * Go to previous step of the presentation.
     * gotoPreviousPage: if is set to true go to beginning of the
     *                   previous page.
     */
    void screenPrev( bool gotoPreviousPage = false );
    /**
     * Go to the next step of the presentation.
     * gotoNextPage: if is set to true go to beginning of the
     *               next page.
     */
    void screenNext( bool gotoNextPage = false );
    void screenLast();
    void screenSkip();

    // text toolbar
    void sizeSelected( int size );
    void fontSelected( const QString &fontFamily );
    void textBold();
    void textItalic();
    void textUnderline();
    void textStrikeOut();
    void textColor();
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textAlignBlock();
    void mtextFont();

    void slotCounterStyleSelected();

    void textDepthPlus();
    void textDepthMinus();
    void textContentsToHeight();
    void textObjectToContents();
    void textInsertPageNum();
    void textDefaultFormat();

    // color bar
    void penChosen();
    void brushChosen();

    void skipToPage( int _num );
    void nextPage();
    void prevPage();

    /**
     * Update a given item in the sidebar
     */
    void updateSideBarItem( KPrPage * page );
    void addSideBarItem( int pos );
    void moveSideBarItem( int oldPos, int newPos );
    void removeSideBarItem( int pos );

    //statusbar updates
    void updatePageInfo();
    void updateObjectStatusBarItem();
    void pageNumChanged();
    void updateSideBarMenu();

    void objectSelectedChanged();

    void renamePageTitle();

    void picViewOriginalSize();
    void picViewOrig640x480();
    void picViewOrig800x600();
    void picViewOrig1024x768();
    void picViewOrig1280x1024();
    void picViewOrig1600x1200();

    void chPic();
    void imageEffect();

    void textSubScript();
    void textSuperScript();

    void slotSpecialChar(QChar, const QString &);
    void slotSpecialCharDlgClosed();

    void insertLink();
    void changeLink();
    void copyLink();
    void removeLink();
    void addToBookmark();
    void slotSoftHyphen();
    void slotNonbreakingSpace();
    void slotNonbreakingHyphen();
    void slotLineBreak();
    void slotIncreaseNumberingLevel();
    void slotDecreaseNumberingLevel();

    void extraAutoFormat();
    void slotSpellCheck();


    void spellCheckerDone( const QString & );
    void spellCheckerCancel();
    void startKSpell();
    void spellAddAutoCorrect (const QString & originalword, const QString & newword);
    void spellCheckerMisspelling( const QString &, int );
    void spellCheckerCorrected( const QString &, int, const QString & );
    void alignChanged( int );

    void formatParagraph();
    void changeNbOfRecentFiles(int _nb);

    void insertVariable();

    void insertCustomVariable();
    void insertNewCustomVariable();
    void refreshCustomMenu();

    void editCustomVars ();
    void openLink();

    void increaseFontSize();
    void decreaseFontSize();

    void tabListChanged( const KoTabulatorList & tabList );

    void newLeftIndent( double _leftIndent);
    void newFirstIndent( double _firstIndent);
    void newRightIndent( double _rightIndent);
    void slotUpdateRuler();

    void slotHRulerDoubleClicked( double ptpos );
    void slotHRulerDoubleClicked();

    void changeCaseOfText();
    void slotStyleSelected();
    void textStyleSelected( int );
    void extraStylist();

    void slotAllowAutoFormat();

    void slotCompletion();

    void removeComment();
    void copyTextOfComment();

    //zoom menu
    void zoomMinus();
    void zoomPageWidth();
    void zoomEntirePage();
    void zoomPlus();
    void zoomSelectedObject();
    void zoomPageHeight();
    void zoomAllObject();

    void flipHorizontal();
    void flipVertical();
    void closeObject();

    void duplicateObj();
    void applyAutoFormat();
    void createStyleFromSelection();

    void alignVerticalTop();
    void alignVerticalBottom();
    void alignVerticalCenter();


    void savePicture();
    void autoSpellCheck();

    void insertFile();
    void editCustomVariable();
    void importStyle();

    void backgroundPicture();
    void updateBgSpellCheckingState();
    void updatePresentationButton(bool);
    void refreshGroupButton();
    void slotAddIgnoreAllWord();
    void addWordToDictionary();
    void customSlideShow();

    void displayObjectFromMasterPage();
    void displayBackground();
    void slotUnitChanged(KoUnit::Unit);
    void documentModified( bool );

public:
    // create GUI
    virtual void createGUI();

    // get current pagenum, 1-based
    unsigned int getCurrPgNum() const;
    /**
     * Recalculate the currPgNum from the activePage of the canvas
     * This is necessary after a page has inserted.
     */
    void recalcCurrentPageNum();

    // return pointer to document
    KPrDocument *kPresenterDoc() const {return m_pKPresenterDoc; }

    // properties
    void changePicture( const QString & );

    KPrCanvas* getCanvas() const { return m_canvas;}

    void setRulerMouseShow( bool _show );
    void setRulerMousePos( int mx, int my );

    // set scrollbar ranges
    void setRanges();

    KoRuler *getHRuler() const { return h_ruler; }
    KoRuler *getVRuler() const { return v_ruler; }
    KoTabChooser *getTabChooser() const { return tabChooser; }
    QScrollBar *getHScrollBar() const { return horz; }
    QScrollBar *getVScrollBar() const { return vert; }

    /**
     * @brief Show the rect where object lies
     *
     * It will move the screen to the top right corner of object
     * when it is not allready totaly visible on the screen.
     *
     * @param object which should be shown
     */
    void showObjectRect( const KPrObject * object );

    PieType getPieType() const { return pieType; }
    int getPieAngle() const { return pieAngle; }
    int getPieLength() const { return pieLength; }
    KoPen getPen() const { return pen; }
    QBrush getBrush() const { return brush; }
    LineEnd getLineBegin() const { return lineBegin; }
    LineEnd getLineEnd() const{ return lineEnd; }
    QColor getGColor1() const { return gColor1; }
    QColor getGColor2() const { return gColor2; }
    BCType getGType() const { return gType; }
    FillType getFillType() const { return fillType; }
    bool getGUnbalanced() const { return gUnbalanced; }
    int getGXFactor() const { return gXFactor; }
    int getGYFactor() const { return gYFactor; }
    TKSelectColorAction* getActionBrushColor() const { return actionBrushColor; }
    TKSelectColorAction* getActionPenColor() const { return actionPenColor; }

    void setPieType(PieType _pieType) { pieType = _pieType; }
    void setPieAngle(int _pieAngle) { pieAngle = _pieAngle; }
    void setPieLength(int _pieLength) { pieLength = _pieLength; }
    void setPen(KoPen _pen) { pen = _pen; }
    void setBrush(QBrush _brush) { brush = _brush; }
    void setLineBegin(LineEnd _lineBegin) { lineBegin = _lineBegin; }
    void setLineEnd(LineEnd _lineEnd){ lineEnd = _lineEnd; }
    void setGColor1(QColor _gColor1) { gColor1 = _gColor1; }
    void setGColor2(QColor _gColor2) { gColor2 = _gColor2; }
    void setGType(BCType _gType) { gType = _gType; }
    void setFillType(FillType _fillType) { fillType = _fillType; }
    void setGUnbalanced(bool _gUnbalanced) { gUnbalanced = _gUnbalanced; }
    void setGXFactor(int _gXFactor) { gXFactor = _gXFactor; }
    void setGYFactor(int _gYFactor) { gYFactor = _gYFactor; }

    void setTool( ToolEditMode toolEditMode );

    int getRndX() const { return rndX; }
    int getRndY() const { return rndY; }

    void setRndX(int _rndX) { rndX = _rndX; }
    void setRndY(int _rndY) { rndY = _rndY; }

//     QFont &currFont() { return tbFont; }
//     QColor &currColor() { return tbColor; }

    void enableWebPres();
    void enableMSPres();

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
     * Rebuild the whole sidebar
     */
    void updateSideBar();

    void refreshPageButton();

    void setCanvasXOffset( int _x );
    void setCanvasYOffset( int _y );

    void openPopupMenuObject( const QString & name , const QPoint & _point );
    void openPopupMenuMenuPage( const QPoint & _point );
    void openPopupMenuSideBar(const QPoint & _point);

    void openPopupMenuZoom( const QPoint & _point );

    void penColorChanged( const KoPen & _pen );
    void brushColorChanged( const QBrush & _brush );

    /**
     * Restart the timer for going to the next page.
     * This is used in automatic presentation mode.
     */
    void restartAutoPresTimer();

    /**
     * Continue the stopped timer for going to the next page.
     * This is used in automatic presentation mode.
     */
    void continueAutoPresTimer();

    /**
     * Stop the timer for going to the next page.
     * This is used in automatic presentation mode.
     */
    void stopAutoPresTimer();

    /**
     * Set the timer for going to next step to sec seconds.
     * This is used in automatic presentation mode.
     */
    void setAutoPresTimer( int sec );

    void showCounter( KoParagCounter &c );

    QPopupMenu * popupMenu( const QString& name );

    void showRulerIndent( double _leftMargin, double _firstLine, double _rightMargin, bool rtl );

    void reorganize();

    // For KPrNoteBar
    KPrNoteBar *getNoteBar() const { return notebar; }

    // Used by Page to plug/unplug the datatool actions
    QPtrList<KAction>& actionList() { return m_actionList; }
    // Used by Page to plug/unplug the variable actions
    QPtrList<KAction> &variableActionList() { return m_variableActionList; }

    // for Polygon object
    bool getCheckConcavePolygon() const { return checkConcavePolygon; }
    int getCornersValue() const { return cornersValue; }
    int getSharpnessValue() const { return sharpnessValue; }

    void setCheckConcavePolygon(bool _concavePolygon) { checkConcavePolygon = _concavePolygon; }
    void setCornersValue(int _cornersValue) { cornersValue = _cornersValue; }
    void setSharpnessValue(int _sharpnessValue) { sharpnessValue = _sharpnessValue; }

    // for Picture Object
    PictureMirrorType getPictureMirrorType() const { return mirrorType; }
    int getPictureDepth() const { return depth; }
    bool getPictureSwapRGB() const { return swapRGB; }
    bool getPictureGrayscal() const { return grayscal; }
    int getPictureBright() const { return bright; }

    /**
     * Set the duration of the given page ( zero based ).
     * This reads out m_duration and adds it to the given page.
     * m_duration is restarted.
     */
    void setPageDuration( int _pgNum );

    /**
     * @brief Zoom the rect
     *
     * @param rect The rect which should be zoomed to.
     */
    void setZoomRect( const KoRect & rect );
    void changeVerticalAlignmentStatus(VerticalAlignmentType _type );

    void closeTextObject();
    void deSelectAllObjects();

    void insertDirectCursor(bool b);
    void updateDirectCursorButton();

    void setEditMaster( bool editMaster );

protected slots:
    // dialog slots
    void backOk( KPrBackDialog*, bool );
    void afChooseOk( const QString & );
    void slotAfchooseCanceled();
    void propertiesOk();
    void pgConfOk();
    void rotateOk();
    void shadowOk();
    void pddClosed();

    // scrolling
    void scrollH( int );
    void scrollV( int );

    // textobject
    void fontChanged( const QFont & );
    void colorChanged( const QColor &color );

    void extraLineBeginNormal();
    void extraLineBeginArrow();
    void extraLineBeginRect();
    void extraLineBeginCircle();
    void extraLineBeginLineArrow();
    void extraLineBeginDimensionLine();
    void extraLineBeginDoubleArrow();
    void extraLineBeginDoubleLineArrow();
    void setExtraLineBegin(LineEnd lb);
    void extraLineEndNormal();
    void extraLineEndArrow();
    void extraLineEndRect();
    void extraLineEndCircle();
    void extraLineEndLineArrow();
    void extraLineEndDimensionLine();
    void extraLineEndDoubleArrow();
    void extraLineEndDoubleLineArrow();
    void setExtraLineEnd(LineEnd le);

    void setExtraPenStyle( Qt::PenStyle style );
    void setExtraPenWidth( double width );

    /**
     * Restart the presenation from the first shown slide.
     * This only works in automatic presentation and infinite loop mode.
     */
    void restartPresentation() { m_autoPresRestart = true; }
    /**
     * Finish the automatic presentation mode.
     * This only works in automatic presentation mode.
     */
    void stopAutomaticPresentation() { m_autoPresStop = true; }

    void newPageLayout( const KoPageLayout &_layout );
    void openPageLayoutDia() { extraLayout(); }
    void unitChanged( KoUnit::Unit );

    void doAutomaticScreenPres();

    void getPageMouseWheelEvent( QWheelEvent *e );
    void updateRuler();

    void refreshAllVariable();
    void slotViewFormattingChars();
    void slotUpdateScrollBarRanges();

    void addGuideLine();

    void refreshRuler( bool state );
    void slotApplyFont();
    void slotApplyParag();
    void slotObjectEditChanged();
    void slotChangeCutState(bool );

    void insertDirectCursor();
    void slotCorrectWord();
    void editFindNext();
    void editFindPrevious();

    void initialLayoutOfSplitter();

    virtual void slotChildActivated(bool a);

    void loadingFinished();

protected:
    virtual void resizeEvent( QResizeEvent* );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

    virtual void keyPressEvent( QKeyEvent* );

    virtual void guiActivateEvent( KParts::GUIActivateEvent *ev );

// GUI
    void setupActions();
    void setupPopupMenus();
    void setupScrollbars();
    void setupRulers();

    void startScreenPres( int pgNum = -1 );

    virtual void updateReadWrite( bool readwrite );

    void addVariableActions( int type, const QStringList & texts,
                             KActionMenu * parentMenu, const QString & menuText );

    void showParagraphDialog( int initialPage = -1, double initialTabPos = 0.0 );

    QValueList<KoTextObject *> spellAddTextObject() const;

    bool switchInOtherPage( const QString & text );

    void openThePresentationDurationDialog();
    QString presentationDurationDataFormatChange( int _time );

    int getZoomEntirePage() const;

    KCommand * applyAutoFormatToCurrentPage( const QPtrList<KoTextObject> & lst);
    void textStyleSelected( KoParagStyle *_sty );

    /*
     * create a command which sets the pen according to the flags
     * for the selected objects on the active and sticky page
     */
    KCommand * getPenCmd( const QString &name, KoPen pen, LineEnd lb, LineEnd le, int flags );

    void spellCheckerRemoveHighlight();

    void updateNoteBarText();
private:
    void clearSpellChecker(bool cancelSpellCheck = false);

// ********** variables **********

    // document
    KPrDocument *m_pKPresenterDoc;

    // flags
    /**
     * Set to true if the presentation sould be restarted.
     * This only works in automatic presentation and infinite loop mode.
     */
    bool m_autoPresRestart;

    /**
     * Set to true if the presentation sould be stoped.
     * This only works in automatic presentation mode.
     */
    bool m_autoPresStop;
    bool m_screenSaverWasEnabled;

    // right button popup menus
    QPopupMenu *rb_lbegin, *rb_lend;
    QPopupMenu *m_arrangeObjectsPopup;
    QPtrList<KAction> m_actionList; // for the kodatatools
    QPtrList<KAction> m_variableActionList;

    // scrollbars
    QScrollBar *vert, *horz;

    // dialogs
    AFChoose *afChoose;
    KPrPropertyEditor *m_propertyEditor;
    KPrPgConfDia *pgConfDia;
    KPrRotationDialogImpl *rotateDia;
    KPrShadowDialogImpl *shadowDia;
    KPrImageEffectDia *imageEffectDia;
    KPrPresDurationDia *presDurationDia;

    // default pen and brush
    KoPen pen;
    QBrush brush;
    LineEnd lineBegin;
    LineEnd lineEnd;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    PieType pieType;
    bool gUnbalanced;
    int gXFactor, gYFactor;
    int pieLength, pieAngle;
    int rndX, rndY;
    // for Convex/Concave Polygon
    bool checkConcavePolygon;
    int cornersValue;
    int sharpnessValue;
    /// used to save mouse pos
    QPoint m_mousePos;

    // for Picture Object
    PictureMirrorType mirrorType;
    int depth;
    bool swapRGB;
    bool grayscal;
    int bright;

    // the page
    KPrCanvas *m_canvas;
    KoRuler *h_ruler, *v_ruler;
    KoTabChooser *tabChooser;
    // text toolbar values
    QFont tbFont;
    int tbAlign;
    QColor tbColor;

    bool m_bShowGUI;
    bool presStarted;
    /**
     * Indicates if the master page is edited
     */
    bool m_editMaster;
    bool allowWebPres;
    bool allowMSPres;
    int currPg; // 0-based

    QSize oldSize;

    int screensaver_pid;

    // Statusbar items
    QLabel * m_sbPageLabel; // 'Current page number and page count' label
    QLabel * m_sbObjectLabel; // Info about selected object
    QLabel *m_sbSavingLabel; // use when saving file
    KStatusBarLabel* m_sbModifiedLabel;
    KStatusBarLabel* m_sbUnitLabel;
    KStatusBarLabel* m_sbZoomLabel;

    // actions
    KAction *actionEditCut;
    KAction *actionEditCustomVars;
    KAction *actionEditCopy;
    KAction *actionEditPaste;
    KAction *actionEditDelete;
    KAction *actionEditSelectAll;
    KAction *actionEditDeSelectAll;
    KAction *actionEditCopyPage;
    KAction *actionEditDuplicatePage;
    KAction *actionEditDelPage;

    KToggleAction *actionViewShowSideBar;
    KToggleAction *actionViewShowNoteBar;
    KToggleAction *actionViewSlideMaster;

    KToggleAction *actionViewShowGuideLine;

    KToggleAction *actionViewFormattingChars;
    KToggleAction *actionViewShowGrid;
    KToggleAction *actionViewSnapToGrid;

    KAction *actionInsertPage;
    KAction *actionInsertPicture;

    KToggleAction *actionToolsMouse;
    KToggleAction *actionToolsRotate;
    KToggleAction *actionToolsZoom;
    KToggleAction *actionToolsLine;
    KToggleAction *actionToolsRectangle;
    KToggleAction *actionToolsCircleOrEllipse;
    KToggleAction *actionToolsPie;
    KToggleAction *actionToolsText;
    KToggleAction *actionToolsAutoform;
    KToggleAction *actionToolsDiagramm;
    KToggleAction *actionToolsTable;
    KToggleAction *actionToolsFormula;
    KToggleAction *actionToolsFreehand;
    KToggleAction *actionToolsPolyline;
    KToggleAction *actionToolsQuadricBezierCurve;
    KToggleAction *actionToolsCubicBezierCurve;
    KToggleAction *actionToolsConvexOrConcavePolygon;
    KToggleAction *actionToolsClosedFreehand;
    KToggleAction *actionToolsClosedPolyline;
    KToggleAction *actionToolsClosedQuadricBezierCurve;
    KToggleAction *actionToolsClosedCubicBezierCurve;
    KoPartSelectAction *actionToolsObject;
    KActionMenu *actionToolsLinePopup;
    KActionMenu *actionToolsShapePopup;
    KActionMenu *actionToolsClosedLinePopup;

    KAction *actionTextFont;
    KFontSizeAction *actionTextFontSize;
    KFontAction *actionTextFontFamily;
    KToggleAction *actionTextAlignLeft;
    KToggleAction *actionTextAlignCenter;
    KToggleAction *actionTextAlignRight;
    KToggleAction *actionTextAlignBlock;

    KActionMenu *actionFormatBullet;
    KActionMenu *actionFormatNumber;


    KAction *actionTextDepthPlus;
    KAction *actionTextDepthMinus;
    KAction *actionTextExtentCont2Height;
    KAction *actionTextExtendObj2Cont;
    KToggleAction *actionTextBold;
    KToggleAction *actionTextItalic;
    KToggleAction *actionTextUnderline;
    KToggleAction *actionFormatStrikeOut;
    KAction *actionTextInsertPageNum;

    KAction *actionExtraProperties;
    KAction *actionExtraRaise;
    KAction *actionExtraLower;

    KAction *actionExtraBringForward;
    KAction *actionExtraSendBackward;
    KActionMenu *actionExtraArrangePopup;

    KAction *actionExtraRotate;
    KAction *actionExtraShadow;
    KActionMenu *actionExtraAlignObjsPopup;
    KAction *actionExtraAlignObjLeft;
    KAction *actionExtraAlignObjCenterH;
    KAction *actionExtraAlignObjRight;
    KAction *actionExtraAlignObjTop;
    KAction *actionExtraAlignObjCenterV;
    KAction *actionExtraAlignObjBottom;
    KAction *actionExtraBackground;
    KAction *actionExtraLayout;
    KAction *actionExtraConfigure;
    KAction *actionExtraWebPres;
    KAction *actionExtraMSPres;
    KAction *actionExtraCreateTemplate;
    KAction *actionExtraLineBegin;
    KAction *actionExtraLineEnd;
    KAction *actionExtraGroup;
    KAction *actionExtraUnGroup;
    KoLineStyleAction *actionExtraPenStyle;
    KoLineWidthAction *actionExtraPenWidth;

    KAction *actionScreenConfigPages;
    KAction *actionScreenAssignEffect;
    KAction *actionScreenTransEffect;
    KAction *actionScreenStart;
    KAction *actionScreenStartFromFirst;
    KAction *actionScreenStop;
    KAction *actionScreenPause;
    KAction *actionScreenFirst;
    KAction *actionScreenPrev;
    KAction *actionScreenNext;
    KAction *actionScreenLast;
    KAction *actionScreenSkip;

    KAction *actionEditFind;
    KAction *actionEditFindNext;
    KAction *actionEditFindPrevious;
    KAction *actionEditReplace;

    KAction *actionCustomSlideShow;

    KAction *actionColorBar;
    KAction *actionExtraDefaultTemplate;

    TKSelectColorAction* actionBrushColor;
    TKSelectColorAction* actionPenColor;
    TKSelectColorAction* actionTextColor;


    KAction *actionResizeTextObject;
    KAction *actionExtendObjectHeight;
    KAction *actionObjectProperties;

    KAction *actionRenamePage;

    KAction *actionPicOriginalSize;
    KAction *actionPic640x480;
    KAction *actionPic800x600;
    KAction *actionPic1024x768;
    KAction *actionPic1280x1024;
    KAction *actionPic1600x1200;
    KAction *actionChangePic;
    KAction *actionExtraSpellCheck;
    KAction *actionFormatDefault;

    KAction *actionImageEffect;

    KAction *actionInsertComment;
    KAction *actionEditComment;
    KAction *actionRemoveComment;
    KAction *actionCopyTextOfComment;

    KAction *actionImportStyle;

    DCOPObject *dcop;

    QToolButton *pgNext, *pgPrev;
    KPrSideBar *sidebar;
    KPrNoteBar *notebar;
    QSplitter *splitter;
    PageBase *pageBase;

    KToggleAction *actionFormatSuper;
    KToggleAction *actionFormatSub;

    KToggleAction *m_actionExtraHeader;
    KToggleAction *m_actionExtraFooter;

    KAction* actionInsertSpecialChar;
    KAction *actionInsertLink;

    KAction * actionChangeLink;
    KAction *actionCopyLink;
    KAction *actionRemoveLink;

    KAction *actionAddLinkToBookmak;
    KAction *actionFormatParag;
    KAction *actionOpenLink;
    KAction *actionIncreaseFontSize;
    KAction *actionDecreaseFontSize;
    KAction *actionChangeCase;
    KAction *actionRefreshAllVariable;

    KSelectAction *actionViewZoom;

    KSelectAction *actionFormatStyle;

    KToggleAction *actionAllowAutoFormat;

    KToggleAction *actionDisplayObjectFromMasterPage;

    KToggleAction *actionDisplayBackgroundPage;

    KAction *actionFormatStylist;

    KAction *actionAddGuideLine;

    KAction *actionConfigureCompletion;

    KAction *actionZoomMinus;
    KAction *actionZoomPageWidth;
    KAction *actionZoomEntirePage;
    KAction *actionZoomPlus;
    KAction *actionZoomSelectedObject;
    KAction *actionZoomPageHeight;
    KAction *actionZoomAllObject;

    KAction *actionFlipHorizontal;
    KAction *actionFlipVertical;
    KAction *actionCloseObject;

    KAction *actionDuplicateObj;
    KAction *actionApplyAutoFormat;

    KAction *actionCreateStyleFromSelection;

    KAction *actionSavePicture;

    KAction *actionSaveBackgroundPicture;


    KAction *actionInsertFile;

    KAction *actionSpellIgnoreAll;

    KToggleAction *actionAlignVerticalTop;
    KToggleAction *actionAlignVerticalBottom;
    KToggleAction *actionAlignVerticalCenter;

    KToggleAction *actionAllowBgSpellCheck;
    KAction *actionEditCustomVarsEdit;
    KActionMenu *actionFormatStyleMenu;
    KToggleAction *actionInsertDirectCursor;

    KAction *actionAddWordToPersonalDictionary;

    /// timer for automatic presentation mode
    QTimer m_autoPresTimer;
    /// time for messuring the elapsed time of the timer
    QTime m_autoPresTime;
    /// the elapsed time if the timer gets stopped in milliseconds
    int m_autoPresElapsedTime;
    /// the value of m_autoPresTimer in milliseconds
    int m_autoPresTimerValue;
    /// true if the timer is connected to doAutomaticScreenPres
    bool m_autoPresTimerConnected;

    /// timer for duration of a page
    QTime m_duration;
    /// list for saving the duration of the pages
    QValueList<int> m_presentationDurationList;

    KoCharSelectDia *m_specialCharDlg;

    // store the currently selected line-tool
    enum CurrentLineTool {
        LtLine = 1,
        LtFreehand = 2,
        LtPolyline = 4,
        LtQuadricBezier = 8,
        LtCubicBezier = 16
    };

    CurrentLineTool m_currentLineTool;

    // store the currently selected shape-tool
    enum CurrentShapeTool {
        StRectangle = 1,
        StCircle = 2,
        StPie = 4,
        StPolygon = 8
    };

    CurrentShapeTool m_currentShapeTool;

    // store the currently selected closed-line-tool
    enum CurrentClosedLineTool {
        CltFreehand = 1,
        CltPolyline = 2,
        CltQuadricBezier = 4,
        CltCubicBezier = 8
    };

    CurrentClosedLineTool m_currentClosedLineTool;

    // Spell-checking
    struct {
        KoSpell *kospell;
        KMacroCommand * macroCmdSpellCheck;
        QStringList replaceAll;
        KoTextIterator * textIterator;
        KSpell2::Dialog *dlg;
    } m_spell;

    KSpell2::Broker::Ptr m_broker;



    KActionMenu *actionInsertVariable;
    KActionMenu *actionInsertCustom;

    struct VariableDef {
        int type;
        int subtype;
    };
    typedef QMap<KAction *, VariableDef> VariableDefMap;
    VariableDefMap m_variableDefMap;

    KoSearchContext *m_searchEntry, *m_replaceEntry;
    KPrFindReplace *m_findReplace;
    KoFontDia *m_fontDlg;
    KoParagDia *m_paragDlg;
    int m_switchPage;
    int m_initSwitchPage;

    int xOffsetSaved, yOffsetSaved; // saved when going fullscreen
    bool m_bDisplayFieldCode; //save state before to go to presentation mode
};

#endif
