// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#ifndef __kpresenter_doc_h__
#define __kpresenter_doc_h__


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class KoGenStyles;
class KPresenterView;
class DCOPObject;
class KCommand;
class KMacroCommand;
class KoCommandHistory;
class KoZoomHandler;
class KoAutoFormat;
class KoUnit;
class KoVariable;
class KoVariableFormatCollection;
class KPrVariableCollection;
class KTempFile;
class KoParagStyle;
#ifdef HAVE_LIBKSPELL2
class KPrBgSpellCheck;
#endif
class KoTextParag;
class KoTextObject;
class KPRLoadingInfo;
class KPGroupObject;

class KoOasisContext;
class KoXmlWriter;
class QFile;

#include <koDocument.h>
#include <koDocumentChild.h>

#include <qptrlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qdatetime.h>

#include <koPageLayoutDia.h>

#include "global.h"
#include <koStyleStack.h>
#include <koPictureCollection.h>
#include "kpgradientcollection.h"
#include <koUnit.h>
#include <kozoomhandler.h>
#include <kostyle.h> // for KoStyleChangeDefMap
#include <kocommandhistory.h>

class KoDocumentEntry;
class KPTextObject;
class StyleDia;
class KPPartObject;
class KPrPage;
class KPObject;
class KPresenterDoc;

class KoParagStyle;
class KoStyleCollection;

class KPresenterChild : public KoDocumentChild
{
public:

    // constructor - destructor
    KPresenterChild( KPresenterDoc *_kpr, KoDocument* _doc, const QRect& _rect );
    KPresenterChild( KPresenterDoc *_kpr );
    ~KPresenterChild();

    // get parent
    KPresenterDoc* parent()const { return m_parent; }

    virtual KoDocument *hitTest( const QPoint &, const QWMatrix & );

protected:
    KPresenterDoc *m_parent;
};

class KPresenterDoc : public KoDocument
{
    Q_OBJECT
    Q_PROPERTY( int maxRecentFiles READ maxRecentFiles )
    Q_PROPERTY( int undoRedoLimit READ undoRedoLimit WRITE setUndoRedoLimit )
    Q_PROPERTY( double indentValue READ getIndentValue WRITE setIndentValue )
    Q_PROPERTY( int getLeftBorder READ getLeftBorder )
    Q_PROPERTY( int getTopBorder READ getTopBorder )
    Q_PROPERTY( int getBottomBorder READ getBottomBorder )
    Q_PROPERTY( int getRightBorder READ getRightBorder )
    Q_PROPERTY( bool cursorInProtectedArea READ cursorInProtectedArea WRITE setCursorInProtectedArea )
    Q_PROPERTY( bool insertDirectCursor READ insertDirectCursor WRITE setInsertDirectCursor )
    Q_PROPERTY( QString picturePath READ picturePath WRITE setPicturePath )
    Q_PROPERTY( QString globalLanguage READ globalLanguage WRITE setGlobalLanguage )
    Q_PROPERTY( bool globalHyphenation READ globalHyphenation WRITE setGlobalHyphenation )
    Q_PROPERTY( double tabStopValue READ tabStopValue WRITE setTabStopValue )
    Q_PROPERTY( bool snapToGrid READ snapToGrid WRITE setSnapToGrid )
    Q_PROPERTY( bool backgroundSpellCheckEnabled READ backgroundSpellCheckEnabled WRITE enableBackgroundSpellCheck )
    Q_PROPERTY( bool spInfiniteLoop READ spInfiniteLoop WRITE setInfiniteLoop )
    Q_PROPERTY( bool spManualSwitch READ spManualSwitch WRITE setManualSwitch )
    Q_PROPERTY( bool presentationDuration READ presentationDuration WRITE setPresentationDuration )
    Q_PROPERTY( QColor gridColor READ gridColor WRITE setGridColor )
    Q_PROPERTY( QColor txtBackCol READ txtBackCol WRITE setTxtBackCol )


  public:
    // constructor - destructor
    KPresenterDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* doc = 0,
                   const char* name = 0, bool singleViewMode = false );
    ~KPresenterDoc();

    // Drawing
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false,
                               double zoomX = 1.0, double zoomY = 1.0 );

    // save
    virtual QDomDocument saveXML();
    virtual bool completeSaving( KoStore* _store );
    virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

    void saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles, QFile* tmpStyckyFile ) const;
    enum { STYLE_BACKGROUNDPAGE = 20, STYLE_BACKGROUNDPAGEAUTO,STYLE_GRAPHICAUTO, STYLE_HATCH, STYLE_GRADIENT,STYLE_OBJECTANIMATION, STYLE_STROKE, STYLE_MARKER, STYLE_PICTURE, STYLE_PRESENTATIONSTICKYOBJECT };

    // load
    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& styles, const QDomDocument&, KoStore* store );
    /*
     * Load oasis object
     * @param pos: we use -1 for sticky page.
     */
    void loadOasisObject( int pos, KPrPage * page, QDomNode & drawPage, KoOasisContext & context, KPGroupObject *groupObject = 0L);

    virtual bool loadXML( const QDomDocument& doc );
    virtual bool loadXML( QIODevice *, const QDomDocument & doc );
    virtual bool loadChildren( KoStore* _store );

    virtual bool initDoc();
    void initEmpty();
    virtual void setEmpty();

    virtual void addView( KoView *_view );
    virtual void removeView( KoView *_view );

    // page layout
    void setPageLayout( const KoPageLayout &);

    virtual QPixmap generatePreview( const QSize &size );

    //header-footer
    void createHeaderFooter();
    void updateHeaderFooterPosition();

    const QPtrList<KPrPage> & getPageList() const {return m_pageList;}

    KPrPage * stickyPage() const;
    KPrPage * activePage()const;
    // We need one that's not const, due to QPtrList::at() not being const
    QPtrList<KPrPage>& pageList() { return m_pageList;}

    void insertPixmapKey( KoPictureKey key );

    void insertObjectInPage(double offset, KPObject *_obj);

    void setGridValue( double rx, double ry, bool _replace = true );

    // get - set options for editmodi
    QColor txtBackCol() const { return _txtBackCol; }
    void setTxtBackCol( const QColor &c ) { _otxtBackCol = _txtBackCol; _txtBackCol = c; }

    // get - set roundedness
    unsigned int getRndX() const { return _xRnd; }
    unsigned int getRndY() const { return _yRnd; }

    // get values for screenpresentations
    bool spInfiniteLoop() const { return _spInfiniteLoop; }
    bool spManualSwitch() const { return _spManualSwitch; }
    bool presentationDuration() const { return _showPresentationDuration; }
    void setInfiniteLoop( bool il ) { _spInfiniteLoop = il; }
    void setManualSwitch( bool ms ) { _spManualSwitch = ms; }
    void setPresentationDuration( bool pd ) { _showPresentationDuration = pd; }


    // size of page
    QRect getPageRect( bool decBorders = true ) const;

    void insertObject(KPresenterChild* ch ){ insertChild(ch);}

    void savePage( const QString &file, int pgnum, bool ignore = false );
    void pastePage( const QMimeSource * data, int pgnum );

    QString templateFileName(bool chooseTemplate, const QString &theFile );

    int insertNewPage( const QString &cmdName,int _page, InsertPos _insPos, bool chooseTemplate, const QString &theFile );

    void insertPage( KPrPage *_page, int position);
    void takePage(KPrPage *_page);
    void deletePage( int _page );
    void copyPageToClipboard( int pgnum );

    // repaint all views
    void repaint( bool );

    void repaint( const QRect& );
    void repaint( KPObject* );

    // stuff for screen-presentations
    /**
     * return the list of steps for the selected page
     * where objects appear/disappear.
     */
    QValueList<int> getPageEffectSteps( unsigned int );

    QPen presPen() const { return _presPen; }
    void setPresPen( QPen p ) {_presPen = p; }

    void restoreBackground( KPrPage * );
    KCommand * loadPastedObjs( const QString &in, KPrPage* _page );

    void deSelectAllObj();
    void deSelectObj(KPObject *obj);
    void updateHeaderFooterButton();

    KoPictureCollection *pictureCollection(){ return &m_pictureCollection; }
    KPGradientCollection *gradientCollection() { return &_gradientCollection; }

    KoAutoFormat * getAutoFormat()const { return m_autoFormat; }

    void replaceObjs( bool createUndoRedo = true );

    int getLeftBorder() const;
    int getTopBorder() const;
    int getBottomBorder() const;
    int getRightBorder() const;

    void enableEmbeddedParts( bool f );

    KPTextObject *header()const { return _header; }
    KPTextObject *footer()const { return _footer; }
    bool hasHeader()const { return _hasHeader; }
    bool hasFooter()const { return _hasFooter; }
    void setHeader( bool b );
    void setFooter( bool b );

    bool isHeader(const KPObject *obj)const;
    bool isFooter(const KPObject *obj)const;
    bool isHeaderFooter(const KPObject *obj)const;

    // Returns true if the slide pgNum (0 based)
    bool isSlideSelected( int pgNum) ;
    // Returns the list of selected slides. Slide numbers are 0-based.
    QValueList<int> selectedSlides();
    QString selectedForPrinting();

    virtual DCOPObject* dcopObject();

    void initConfig();
    void saveConfig();

    bool raiseAndLowerObject;

    KoStyleCollection * styleCollection()const { return m_styleColl;}


    void updateAllStyleLists();
    void applyStyleChange( KoStyleChangeDefMap changed );
    void updateStyleListOrder( const QStringList &list );

    void addCommand( KCommand * cmd );

    KoZoomHandler* zoomHandler() const { return m_zoomHandler; }
    QFont defaultFont() const { return m_defaultFont; }
    void setDefaultFont( const QFont & newFont) {
        m_defaultFont = newFont;
    }

    /**
     * get custom kspell config
     */

    bool showStatusBar() const { return m_bShowStatusBar;}
    void setShowStatusBar( bool _status ) { m_bShowStatusBar = _status;}

    bool showGrid() const { return m_bShowGrid; }
    void setShowGrid ( bool _grid ) { m_bShowGrid = _grid; }

    double tabStopValue() const { return m_tabStop; }
    void setTabStopValue ( double _tabStop );

    // The user-chosen global unit
    QString getUnitName()const { return KoUnit::unitName( m_unit ); }
    KoUnit::Unit getUnit()const { return m_unit; }
    void setUnit( KoUnit::Unit _unit );

    // in pt
    double getIndentValue()const { return m_indent; }
    void setIndentValue(double _ind) { m_indent=_ind; }

    int maxRecentFiles() const { return m_maxRecentFiles; }

    KoVariableFormatCollection *variableFormatCollection()const { return m_varFormatCollection; }

    void recalcVariables( int type );

    void recalcPageNum();

    KPrVariableCollection *getVariableCollection()const {return m_varColl;}

    void refreshMenuCustomVariable();

    void setShowRuler(bool _ruler){ m_bShowRuler=_ruler; }
    bool showRuler() const { return m_bShowRuler; }

    void reorganizeGUI();

    int undoRedoLimit() const;
    void setUndoRedoLimit(int _val);


    void updateRuler();
    void updateRulerPageLayout();

    unsigned int getPageNums() const { return m_pageList.count(); }

    // Tell all views to stop editing this frameset, if they were doing so
    void terminateEditing( KPTextObject * textObj )
        { emit sig_terminateEditing( textObj ); }
    KPrPage * initialActivePage() const;

    KPresenterView *firstView() const;

    void displayActivePage(KPrPage * _page);

    void updateZoomRuler();
    void newZoomAndResolution( bool updateViews, bool forPrint );

    void addRemovePage( int pos, bool addPage );
    void movePageTo( int oldPos, int newPos );

    void updateSideBarItem( int pgNum, bool sticky = false );
    KPrPage * findSideBarPage(KPObject *object);
    KPrPage * findSideBarPage(QPtrList<KPObject> &objects);

    void refreshAllNoteBar(int page, const QString &text, KPresenterView *exceptView);


    bool backgroundSpellCheckEnabled() const;
    void enableBackgroundSpellCheck( bool b );

    void startBackgroundSpellCheck();

    //refresh obj when we active or disactive
    void reactivateBgSpellChecking(bool refreshTextObj=false);
    QPtrList<KoTextObject> allTextObjects() const;
    QValueList<KoTextObject *> visibleTextObjects( ) const;

    bool allowAutoFormat() const { return m_bAllowAutoFormat; }
    void setAllowAutoFormat(bool _b){ m_bAllowAutoFormat=_b; }

    // This setting has to be here [instead of the view] because we need to
    // format paragraphs slightly differently (to add room for the CR char)
    bool viewFormattingChars() const { return m_bViewFormattingChars; }
    void setViewFormattingChars(bool _b) { m_bViewFormattingChars=_b; }


    bool showHelplines() const {return m_bShowHelplines; }
    void setShowHelplines(bool b);

    QValueList<double> &horizHelplines() {return m_horizHelplines; }
    QValueList<double> &vertHelplines() {return m_vertHelplines; }
    QValueList<KoPoint> &helpPoints() {return m_helpPoints; }

    void horizHelplines(const QValueList<double> &lines);
    void vertHelplines(const QValueList<double> &lines);

    int indexOfHorizHelpline(double pos);
    int indexOfVertHelpline(double pos);

    int indexOfHelpPoint( const KoPoint &pos );

    void updateHorizHelpline(int idx, double pos);
    void updateVertHelpline(int idx, double pos);

    void updateHelpPoint( int idx, const KoPoint &pos );


    void addHorizHelpline(double pos);
    void addVertHelpline(double pos);

    void addHelpPoint( const KoPoint &pos );

    void removeHorizHelpline(int index);
    void removeVertHelpline( int index );

    void removeHelpPoint( int index );


    void updateHelpLineButton();
    void updateGridButton();

    double getGridX()const { return m_gridX; }
    void setGridX(double _x) { m_gridX=_x; }

    double getGridY()const { return m_gridY; }
    void setGridY(double _y) { m_gridY=_y; }


    bool gridToFront() const { return m_bGridToFont; }
    void setGridToFront( bool _front ) { m_bGridToFont = _front; }

    bool snapToGrid() const { return m_bSnapToGrid; }
    void setSnapToGrid( bool _b ) { m_bSnapToGrid = _b; }

    bool helpLineToFront() const { return m_bHelplinesToFront; }
    void setHelpLineToFront( bool _front ) { m_bHelplinesToFront = _front; }

    QColor gridColor() const { return m_gridColor; }
    void setGridColor( const QColor & _col ) { m_gridColor = _col; }

    QStringList spellListIgnoreAll() const { return m_spellListIgnoreAll;}
    void addIgnoreWordAll( const QString & );
    void addIgnoreWordAllList( const QStringList & _lst)
        { m_spellListIgnoreAll = _lst;}

    void clearIgnoreWordAll( );
    KoCommandHistory * commandHistory()const { return m_commandHistory; }
    void updateObjectStatusBarItem();
    void updateObjectSelected();
    void layout(KPObject *kpobject);
    void layout();
    void changeBgSpellCheckingState( bool b );

    bool cursorInProtectedArea()const;
    void setCursorInProtectedArea( bool b );

    void insertFile(const QString & file );

    void spellCheckParagraphDeleted( KoTextParag *_parag,  KPTextObject *frm);

    void loadPictureMap ( const QDomElement& domElement );
    void updateRulerInProtectContentMode();
    void updatePresentationButton();
    void refreshGroupButton();

    QString picturePath()const { return m_picturePath; }
    void setPicturePath( const QString & _path ) { m_picturePath = _path ; }

    bool insertDirectCursor() const { return m_bInsertDirectCursor; }
    void setInsertDirectCursor(bool _b);
    void updateDirectCursorButton();

    QString globalLanguage()const { return m_globalLanguage; }
    void setGlobalLanguage( const QString & _lang ){m_globalLanguage = _lang;}

    bool globalHyphenation() const { return m_bGlobalHyphenation; }
    void setGlobalHyphenation ( bool _hyphen ) { m_bGlobalHyphenation = _hyphen; }

    void addWordToDictionary( const QString & word);

    void loadImagesFromStore( KoStore *_store );
    void saveEmbeddedObject(KPrPage *page, const QPtrList<KoDocumentChild>& childList ,QDomDocument &doc,QDomElement &presenter );
    void insertEmbedded( KoStore *store, QDomElement elem, KMacroCommand * macroCmd, KPrPage *page );
#ifdef HAVE_LIBKSPELL2
    KPrBgSpellCheck* backSpeller() const { return m_bgSpellCheck; }
#endif
public slots:
    void movePage( int from, int to );
    void copyPage( int from, int to );
    void selectPage( int pgNum, bool select );
    void clipboardDataChanged();

    void slotRepaintChanged( KPTextObject * );

    void slotRepaintVariable();

signals:
    void enablePaste( bool );
    void sig_refreshMenuCustomVariable();
    void pageNumChanged();
    void sig_updateRuler();
    void sig_terminateEditing( KPTextObject * );
    void sig_updateMenuBar();

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotDocumentInfoModifed();

protected:
    KoView* createViewInstance( QWidget* parent, const char* name );
    void testAndCloseAllTextObjectProtectedContent();
    // ************ functions ************
    /**
     * Overloaded function from @ref Document_impl. Saves all children.
     */
    virtual bool saveChildren( KoStore* _store );

    void loadStyleTemplates( const QDomElement &styles );
    void saveStyle( KoParagStyle *sty, QDomElement parentElem );

    QDomDocumentFragment saveBackground( QDomDocument& );
    QDomElement saveObjects( QDomDocument &doc );
    QDomElement saveTitle( QDomDocument &doc );
    QDomElement saveNote( QDomDocument &doc );
    QDomElement saveAttribute( QDomDocument &doc );
    QDomElement saveUsedSoundFileToXML( QDomDocument &_doc, QStringList _list );
    void loadTextStyle( const QDomElement& domElement );
    void saveEmbeddedObject(KPrPage *page, KoDocumentChild *chl,QDomDocument &doc,QDomElement &presenter, double offset );
    void saveHelpLines( QDomDocument &doc, QDomElement& element );

    void loadBackground( const QDomElement &element );
    void loadHelpLines( const QDomElement &element );
    KCommand * loadObjects( const QDomElement &element,bool paste=false );
    void loadTitle( const QDomElement &element );
    void loadNote( const QDomElement &element );
    void loadUsedSoundFileFromXML( const QDomElement &element );

    virtual bool completeLoading( KoStore* /* _store */ );
    void makeUsedPixmapList();
    void makeUsedSoundFileList();

    void saveUsedSoundFileToStore( KoStore *_store, QStringList _list );
    void loadUsedSoundFileFromStore( KoStore *_store, QStringList _list );
    void addStyles( const QDomElement* style, KoOasisContext & context );
    void fillStyleStack( const QDomElement& object, KoOasisContext & context );
    /*
     * increaseOrder = true by default, put to false when we have group of animation
     */
    int createPresentationAnimation(const QDomElement& element, int order = 0 ,bool increaseOrder = true);

    void saveOasisPresentationSettings( KoXmlWriter &contentTmpWriter );
    void saveOasisPresentationCustionSlideShow( KoXmlWriter &contentTmpWriter );

    void saveOasisSettings( KoXmlWriter &contentTmpWriter );
    void loadOasisSettings( const QDomDocument&settingsDoc );


    //we move presSpeed to each table => compatibility with old file format
    void compatibilityPresSpeed();

    void parseOasisHelpLine( const QString &str );

    // ************ variables ************

    // screenpresentations
    bool _spInfiniteLoop, _spManualSwitch, _showPresentationDuration;

    // options
    int _xRnd, _yRnd;

    // options for editmode
    QColor _txtBackCol;
    QColor _otxtBackCol;

    bool _clean;
    int objStartY, objStartNum;

    QPen _presPen;

    KoPictureCollection m_pictureCollection;
    KPGradientCollection _gradientCollection;

    KPTextObject *_header, *_footer;
    bool _hasHeader, _hasFooter;

    QMap<KoPictureKey, QString> m_pictureMap;

    KoPageLayout __pgLayout;
    int lastObj;

    QString urlIntern;

    QValueList<KoPictureKey> usedPictures;
    QStringList usedSoundFile, haveNotOwnDiskSoundFile;
    QPtrList<KTempFile> tmpSoundFileList;
    DCOPObject *dcop;

    int saveOnlyPage;
    QString m_tempFileInClipboard;
    bool ignoreSticky;

    KoCommandHistory * m_commandHistory;
    KoZoomHandler* m_zoomHandler;
    QFont m_defaultFont;
    KoAutoFormat * m_autoFormat;

    bool m_bShowRuler;
    bool m_bShowStatusBar;
    bool m_bAllowAutoFormat;
    bool m_bShowHelplines;
    bool m_bViewFormattingChars;

    bool m_bHelplinesToFront;

    bool m_bShowGrid;

    bool m_bGridToFont;
    bool m_bSnapToGrid;
    double m_indent; // in pt

    double m_tabStop;
    KoUnit::Unit m_unit;
    int m_maxRecentFiles;
    bool m_cursorInProtectectedArea;
    KoVariableFormatCollection *m_varFormatCollection;
    KPrVariableCollection *m_varColl;
    QColor m_gridColor;
    bool _duplicatePage;
private:
    QValueList<double> m_horizHelplines;
    QValueList<double> m_vertHelplines;

    QValueList<KoPoint> m_helpPoints;

    QPtrList<KPrPage> m_pageList;
    QPtrList<KPrPage> m_deletedPageList;

    QStringList m_spellListIgnoreAll;

    double m_gridX;
    double m_gridY;
    double oldGridX;
    double oldGridY;
    int m_insertFilePage;
    KPrPage *m_initialActivePage;
    KPrPage *m_pageWhereLoadObject;
    KPrPage *m_stickyPage;
 #ifdef HAVE_LIBKSPELL2
    KPrBgSpellCheck *m_bgSpellCheck;
#endif
    KoStyleCollection *m_styleColl;
    KPObject *bgObjSpellChecked;
    QString m_picturePath;
    bool  m_bInsertDirectCursor;
    QString m_globalLanguage;
    bool m_bGlobalHyphenation;
    KPRLoadingInfo *m_loadingInfo;
};

#endif
