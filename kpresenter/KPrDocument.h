//Added by qt3to4:
#include <QPixmap>
// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#ifndef __kpresenter_doc_h__
#define __kpresenter_doc_h__


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class KoGenStyles;
class KPrView;
class DCOPObject;
class KCommand;
class KMacroCommand;
class KoCommandHistory;
class KoTextZoomHandler;
class KoAutoFormat;
class KoVariable;
class KoVariableFormatCollection;
class KPrVariableCollection;
class KTempFile;
class KoParagStyle;
class KPrBgSpellCheck;
class KoTextParag;
class KoTextObject;
class KPrLoadingInfo;
class KPrGroupObject;

class KoOasisContext;
class KoOasisSettings;
class KoXmlWriter;
class QFile;
class KoSavingContext;
class KCommandHistory;

#include <KoDocument.h>
#include <KoDocumentChild.h>

#include <q3ptrlist.h>
#include <qobject.h>
#include <QString>
#include <q3valuelist.h>
#include <qdatetime.h>
#include <q3dict.h>
#include <KoPageLayoutDia.h>

#include "global.h"
#include <KoStyleStack.h>
#include <KoPictureCollection.h>
#include "KPrGradientCollection.h"
#include <KoTextZoomHandler.h>
#include <KoStyleCollection.h> // for KoStyleChangeDefMap

class KoDocumentEntry;
class KPrTextObject;
class KPrPartObject;
class KPrPage;
class KPrObject;
class KPrDocument;

class KoParagStyle;
class KoStyleCollection;

class KPrChild : public KoDocumentChild
{
public:

    // constructor - destructor
    KPrChild( KPrDocument *_kpr, KoDocument* _doc, const QRect& _rect );
    KPrChild( KPrDocument *_kpr );
    ~KPrChild();

    // get parent
    KPrDocument* parent()const { return m_parent; }

    virtual KoDocument *hitTest( const QPoint &, const QMatrix & );

protected:
    KPrDocument *m_parent;
};

class KPrDocument : public KoDocument
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
    KPrDocument( QWidget *parentWidget = 0, QObject* doc = 0,
                   bool singleViewMode = false );
    ~KPrDocument();

    // Drawing
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false,
                               double zoomX = 1.0, double zoomY = 1.0 );

    // save
    virtual QDomDocument saveXML();
    virtual bool completeSaving( KoStore* _store );
    virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

    enum SaveFlag { SaveAll, SaveSelected, SavePage };
    void saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles, QFile* masterStyles, 
                                  KoSavingContext & savingContext, SaveFlag saveFlag = SaveAll ) const;
    enum { STYLE_BACKGROUNDPAGE = 20, STYLE_BACKGROUNDPAGEAUTO, STYLE_GRADIENT,STYLE_OBJECTANIMATION, STYLE_STROKE, STYLE_MARKER, STYLE_PICTURE, STYLE_PRESENTATIONSTICKYOBJECT };

    // load
    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& styles, const QDomDocument&, KoStore* store );
    /*
     * Load oasis object
     * @param pos: we use -1 for sticky page.
     */
    void loadOasisObject( KPrPage * page, QDomNode & drawPage, KoOasisContext & context, KPrGroupObject *groupObject = 0L);

    virtual bool loadXML( const QDomDocument& doc );
    virtual bool loadXML( QIODevice *, const QDomDocument & doc );
    virtual bool loadChildren( KoStore* _store );

    virtual int supportedSpecialFormats() const;

    virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget=0);
    virtual void setEmpty();

    virtual void addView( KoView *_view );
    virtual void removeView( KoView *_view );

    // page layout
    void setPageLayout( const KoPageLayout &);

    virtual QPixmap generatePreview( const QSize &size );

    virtual void addShell( KoMainWindow *shell );

    //header-footer
    void createHeaderFooter();
    void updateHeaderFooterPosition();

    const Q3PtrList<KPrPage> & getPageList() const {return m_pageList;}

    // @return the master page
    KPrPage * masterPage() const { return m_masterPage; }
    KPrPage * activePage()const;
    // We need one that's not const, due to QPtrList::at() not being const
    Q3PtrList<KPrPage>& pageList() { return m_pageList;}

    void insertPixmapKey( KoPictureKey key );

    void insertObjectInPage(double offset, KPrObject *_obj, int pos = -1);

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

    void insertObject(KPrChild* ch ){ insertChild(ch);}

    void savePage( const QString &file, int pgnum, bool ignore = false );

    void pastePage( const QMimeSource * data, int pgnum );

    QString templateFileName(bool chooseTemplate, const QString &theFile );

    int insertNewPage( const QString &cmdName,int _page, InsertPos _insPos, bool chooseTemplate, const QString &theFile );

    /**
     * insert page page and go to page insertPageNum on all views
     * which have currentPageNum as active page
     */
    void insertPage( KPrPage *page, int currentPageNum, int insertPageNum );
    /**
     * remove page page and go to page pageNum on all view which
     * had page as active page
     */
    void takePage( KPrPage *page, int pageNum );
    void deletePage( int _page );
    void copyPageToClipboard( int pgnum );

    // repaint all views
    void repaint( bool );

    void repaint( const QRect& );
    void repaint( KPrObject* );

    // stuff for screen-presentations
    /**
     * return the list of steps for the selected page
     * where objects appear/disappear.
     */
    Q3ValueList<int> getPageEffectSteps( unsigned int );

    QPen presPen() const { return _presPen; }
    void setPresPen( QPen p ) {_presPen = p; }

    void restoreBackground( KPrPage * );
    KCommand * loadPastedObjs( const QString &in, KPrPage* _page );

    void deSelectAllObj();
    void deSelectObj(KPrObject *obj);
    void updateHeaderFooterButton();

    KoPictureCollection *pictureCollection(){ return &m_pictureCollection; }
    KPrGradientCollection *gradientCollection() { return &_gradientCollection; }

    KoAutoFormat * getAutoFormat()const { return m_autoFormat; }

    void replaceObjs( bool createUndoRedo = true );

    int getLeftBorder() const;
    int getTopBorder() const;
    int getBottomBorder() const;
    int getRightBorder() const;

    void enableEmbeddedParts( bool f );

    KPrTextObject *header()const { return _header; }
    KPrTextObject *footer()const { return _footer; }

    void setHeader( bool b );
    void setFooter( bool b );

    void setDisplayObjectMasterPage( bool b );
    void setDisplayBackground( bool b );

    bool isHeader(const KPrObject *obj)const;
    bool isFooter(const KPrObject *obj)const;
    bool isHeaderFooter(const KPrObject *obj)const;

    // Returns true if the slide pgNum (0 based)
    bool isSlideSelected( int pgNum) ;
    // Returns the list of selected slides. Slide numbers are 0-based.
    Q3ValueList<int> selectedSlides();
    QString selectedForPrinting();

    virtual DCOPObject* dcopObject();

    void initConfig();
    void saveConfig();

    KoStyleCollection * styleCollection()const { return m_styleColl;}


    void updateAllStyleLists();
    void applyStyleChange( KoStyleChangeDefMap changed );
    void updateStyleListOrder( const QStringList &list );

    void addCommand( KCommand * cmd );

    KoTextZoomHandler* zoomHandler() const { return m_zoomHandler; }
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
    void terminateEditing( KPrTextObject * textObj )
        { emit sig_terminateEditing( textObj ); }
    KPrPage * initialActivePage() const;

    KPrView *firstView() const;

    void displayActivePage(KPrPage * _page);

    void updateZoomRuler();
    void newZoomAndResolution( bool updateViews, bool forPrint );

    void movePageTo( int oldPos, int newPos );

    void updateSideBarItem( KPrPage * page );
    KPrPage * findPage(KPrObject *object);
    KPrPage * findPage(Q3PtrList<KPrObject> &objects);

    void refreshAllNoteBar(int page, const QString &text, KPrView *exceptView);
    void refreshAllNoteBarMasterPage(const QString &text, KPrView *exceptView);


    bool backgroundSpellCheckEnabled() const;
    void enableBackgroundSpellCheck( bool b );

    void startBackgroundSpellCheck();

    //refresh obj when we active or disactive
    void reactivateBgSpellChecking(bool refreshTextObj=false);
    Q3PtrList<KoTextObject> allTextObjects() const;
    Q3ValueList<KoTextObject *> visibleTextObjects( ) const;

    /// Reimplementation from KoDocument.
    virtual Q3ValueList<KoTextDocument *> allTextDocuments() const;

    bool allowAutoFormat() const { return m_bAllowAutoFormat; }
    void setAllowAutoFormat(bool _b){ m_bAllowAutoFormat=_b; }

    // This setting has to be here [instead of the view] because we need to
    // format paragraphs slightly differently (to add room for the CR char)
    bool viewFormattingChars() const { return m_bViewFormattingChars; }
    void setViewFormattingChars(bool _b) { m_bViewFormattingChars=_b; }


    bool showGuideLines() const { return m_bShowGuideLines; }
    void setShowGuideLines( bool b );

    /**
     * @brief Get the positions of the horizontal guide lines
     *
     * @return list of positions of the horizontal guide lines
     */
    Q3ValueList<double> &horizontalGuideLines() { return m_hGuideLines; }

    /**
     * @brief Get the positions of the vertical guide lines
     *
     * @return list of positions of the vertical guide lines
     */
    Q3ValueList<double> &verticalGuideLines() { return m_vGuideLines; }

    /**
     * @brief Set the positions of the horizontal guide lines
     *
     * @param lines a list of positions of the horizontal guide lines
     */
    void horizontalGuideLines( const Q3ValueList<double> &lines );

    /**
     * @brief Set the positions of the vertical guide lines
     *
     * @param lines a list of positions of the vertical guide lines
     */
    void verticalGuideLines( const Q3ValueList<double> &lines );

    /**
     * @brief Add a guide line
     *
     * @param p the orientation of the guide line
     * @param p the position of the guide line
     */
    void addGuideLine( Qt::Orientation o, double pos );

    void updateGuideLineButton();

    void updateGridButton();

    double getGridX()const { return m_gridX; }
    void setGridX(double _x) { m_gridX=_x; }

    double getGridY()const { return m_gridY; }
    void setGridY(double _y) { m_gridY=_y; }

    bool snapToGrid() const { return m_bSnapToGrid; }
    void setSnapToGrid( bool _b ) { m_bSnapToGrid = _b; }

    QColor gridColor() const { return m_gridColor; }
    void setGridColor( const QColor & _col ) { m_gridColor = _col; }

    QStringList spellCheckIgnoreList() const { return m_spellCheckIgnoreList; }
    void setSpellCheckIgnoreList( const QStringList& lst );
    void addSpellCheckIgnoreWord( const QString & );

    KCommandHistory * commandHistory()const { return m_commandHistory; }
    void updateObjectStatusBarItem();
    void updateObjectSelected();
    void layout(KPrObject *kpobject);
    void layout();
    void changeBgSpellCheckingState( bool b );

    bool cursorInProtectedArea()const;
    void setCursorInProtectedArea( bool b );

    void insertFile(const QString & file );

    void spellCheckParagraphDeleted( KoTextParag *_parag,  KPrTextObject *frm);

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
    void saveEmbeddedObject(KPrPage *page, const Q3PtrList<KoDocumentChild>& childList ,QDomDocument &doc,QDomElement &presenter );
    void insertEmbedded( KoStore *store, QDomElement elem, KMacroCommand * macroCmd, KPrPage *page, int pos );

    KPrBgSpellCheck* backSpeller() const { return m_bgSpellCheck; }

    void setCustomSlideShows( const CustomSlideShowMap & customSlideShows );
    Q3ValueList <KPrPage *> customListPage( const QStringList & lst, bool loadOasis=false );


    QString presentationName() const { return m_presentationName; }
    void setPresentationName( const QString &_name ) { m_presentationName = _name; }

    QStringList presentationList();
    //return list of slide which be displaying:
    //return selected slide when presentation name is empty
    //otherwise return list of custom slide show
    Q3ValueList<int> displaySelectedSlides();
    Q3ValueList<int> listOfDisplaySelectedSlides( const Q3ValueList<KPrPage*>& lst );
    void testCustomSlideShow( const Q3ValueList<KPrPage *> &pages, KPrView *view );
    void clearTestCustomSlideShow();

    const CustomSlideShowMap & customSlideShows() { return m_customListSlideShow; }

public slots:
    void movePage( int from, int to );
    void copyPage( int from );

    void selectPage( int pgNum, bool select );
    void clipboardDataChanged();

    void slotRepaintChanged( KPrTextObject * );

    static void writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles, KoSavingContext& context, bool stylesDotXml );

    void slotGuideLinesChanged( KoView *view );

    virtual void initEmpty();

signals:
    void enablePaste( bool );
    void sig_refreshMenuCustomVariable();
    void pageNumChanged();
    void sig_updateRuler();
    void sig_terminateEditing( KPrTextObject * );
    void sig_updateMenuBar();

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotDocumentInfoModifed();

    virtual void openExistingFile( const QString& file );
    virtual void openTemplate( const QString& file );

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
    void saveGuideLines( QDomDocument &doc, QDomElement& element );

    void loadBackground( const QDomElement &element );
    void loadGuideLines( const QDomElement &element );
    KCommand * loadObjects( const QDomElement &element,bool paste=false );
    void loadTitle( const QDomElement &element );
    void loadNote( const QDomElement &element );
    void loadUsedSoundFileFromXML( const QDomElement &element );

    virtual bool completeLoading( KoStore* /* _store */ );
    void makeUsedPixmapList();
    void makeUsedSoundFileList();

    void saveUsedSoundFileToStore( KoStore *_store, QStringList _list );
    void loadUsedSoundFileFromStore( KoStore *_store, QStringList _list );
    void fillStyleStack( const QDomElement& object, KoOasisContext & context, const char* family );
    /*
     * increaseOrder = true by default, put to false when we have group of animation
     */
    int createPresentationAnimation(const QDomElement& element, int order = 0 ,bool increaseOrder = true);

    void saveOasisPresentationSettings( KoXmlWriter &contentTmpWriter, QMap<int, QString> &page2name );
    void loadOasisPresentationSettings( QDomNode &settingsDoc );
    void saveOasisPresentationCustomSlideShow( KoXmlWriter &contentTmpWriter, QMap<int, QString> &page2name );
    void loadOasisPresentationCustomSlideShow( QDomNode &settingsDoc );

    void loadOasisHeaderFooter( QDomNode & drawPage, KoOasisContext & context);

    void saveOasisSettings( KoXmlWriter &contentTmpWriter );
    void loadOasisSettings( const QDomDocument&settingsDoc );

    void saveOasisCustomFied( KoXmlWriter &writer )const;

    void loadOasisIgnoreList( const KoOasisSettings& settings );

    //we move presSpeed to each table => compatibility with old file format
    void compatibilityFromOldFileFormat();

    void parseOasisGuideLines( const QString &str );

    /**
     * recalc the variables
     * update statusbar and sidebar menu
     */
    void pageOrderChanged();
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
    KPrGradientCollection _gradientCollection;

    KPrTextObject *_header, *_footer;

    QMap<KoPictureKey, QString> m_pictureMap;

    CustomSlideShowMap m_customListSlideShow;


    KoPageLayout __pgLayout;
    int lastObj;

    QString urlIntern;

    Q3ValueList<KoPictureKey> usedPictures;
    QStringList usedSoundFile, haveNotOwnDiskSoundFile;
    Q3PtrList<KTempFile> tmpSoundFileList;
    DCOPObject *dcop;

    int saveOnlyPage;
    QString m_tempFileInClipboard;
    bool ignoreSticky;

    KCommandHistory * m_commandHistory;
    KoTextZoomHandler* m_zoomHandler;
    QFont m_defaultFont;
    KoAutoFormat * m_autoFormat;

    bool m_bShowRuler;
    bool m_bShowStatusBar;
    bool m_bAllowAutoFormat;
    bool m_bShowGuideLines;
    bool m_bViewFormattingChars;

    bool m_bShowGrid;

    bool m_bSnapToGrid;
    double m_indent; // in pt

    double m_tabStop;
    int m_maxRecentFiles;
    bool m_cursorInProtectectedArea;
    KoVariableFormatCollection *m_varFormatCollection;
    KPrVariableCollection *m_varColl;
    QColor m_gridColor;
    bool _duplicatePage;
private:
    /// list of positions of horizontal guide lines
    Q3ValueList<double> m_hGuideLines;
    /// list of positions of vertical guide lines
    Q3ValueList<double> m_vGuideLines;

    Q3PtrList<KPrPage> m_pageList;
    Q3PtrList<KPrPage> m_deletedPageList;

    QStringList m_spellCheckIgnoreList; // per-document
    QStringList m_spellCheckPersonalDict; // per-user

    double m_gridX;
    double m_gridY;
    double oldGridX;
    double oldGridY;
    int m_insertFilePage;
    KPrPage *m_initialActivePage;
    KPrPage *m_pageWhereLoadObject;
    KPrPage *m_masterPage;
    KPrBgSpellCheck *m_bgSpellCheck;
    KoStyleCollection *m_styleColl;
    KPrObject *bgObjSpellChecked;
    QString m_picturePath;
    bool  m_bInsertDirectCursor;
    QString m_globalLanguage;
    QString m_presentationName;
    bool m_bGlobalHyphenation;
    KPrLoadingInfo *m_loadingInfo;
    Q3ValueList<int> *m_customListTest;

    /// here the amount of existing children before inserting a page/file
    /// is saved, so that we load the correct children
    int m_childCountBeforeInsert;
};

#endif
