//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002-2006 David Faure <faure@kde.org>
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
 * Boston, MA 02110-1301, USA.
*/

#ifndef kwdoc_h
#define kwdoc_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

class Q3DragObject;
class KoSavingContext;
class KoGenStyles;
class KWDocument;
class KPrinter;
class KWTextImage;
class KWTextFrameSet;
class KWPictureFrameSet;
class KWMailMergeDataBase;
class KWFrameSet;
class KWTableFrameSet;
class KWPartFrameSet;
class KoStyleCollection;
class KoParagStyle;
class KWFrameStyle;
class KWTableStyle;
class KWTableTemplate;
#define KoParagStyle KoParagStyle
class KWFrame;
class KWViewMode;
class KMacroCommand;
class KoDocumentEntry;
class QPainter;
class KoAutoFormat;
class KCommand;
class KCommandHistory;
class KoVariable;
class KoVariableFormatCollection;
class KWVariableCollection;
class KoTextObject;
class KWBgSpellCheck;
class KoStyleCollection;
class KWFrameStyleCollection;
class KWTableStyleCollection;
class KWTableTemplateCollection;
class KWFootNoteVariable;
class DCOPObject;
class KWLoadingInfo;
class KoPicture;
class KoTextBookmark;
class KoTextBookmarkList;
class KoPictureCollection;
class KWDocumentChild;
class KWPageManager;
class KWPage;

class QFont;
class QStringList;
class QRect;

namespace KFormula {
    class Document;
    class DocumentWrapper;
}

class KoTextParag;
class KoOasisSettings;

#include "KWAnchorPos.h" // legacy loading stuff
#include "KWView.h"

#include <KoDocument.h>
#include <KoTextZoomHandler.h>
#include <KoPictureKey.h>
#include <KoStyleCollection.h> // for KoStyleChangeDefMap

#include <qmap.h>
#include <q3ptrlist.h>
#include <qfont.h>
#include <q3valuevector.h>

/******************************************************************/
/* Class: KWDocument                                           */
/******************************************************************/

class KWDocument : public KoDocument, public KoTextZoomHandler
{
    Q_OBJECT
    Q_PROPERTY( double ptColumnWidth READ ptColumnWidth )
    Q_PROPERTY( double ptColumnSpacing READ ptColumnSpacing )
    Q_PROPERTY( double gridX READ gridX WRITE setGridX )
    Q_PROPERTY( double gridY READ gridY WRITE setGridY )
    Q_PROPERTY( bool snapToGrid READ snapToGrid WRITE setSnapToGrid )
    Q_PROPERTY( double indentValue READ indentValue WRITE setIndentValue )
    Q_PROPERTY( int nbPagePerRow READ nbPagePerRow WRITE setNbPagePerRow )
    Q_PROPERTY( double defaultColumnSpacing READ defaultColumnSpacing WRITE setDefaultColumnSpacing )
    Q_PROPERTY( int maxRecentFiles READ maxRecentFiles )
    Q_PROPERTY( QString globalLanguage READ globalLanguage WRITE setGlobalLanguage )
    Q_PROPERTY( bool globalHyphenation READ globalHyphenation WRITE setGlobalHyphenation )
    Q_PROPERTY( bool insertDirectCursor READ insertDirectCursor WRITE setInsertDirectCursor )
    Q_PROPERTY( QStringList personalExpressionPath READ personalExpressionPath WRITE setPersonalExpressionPath )
    Q_PROPERTY( bool viewFormattingBreak READ viewFormattingBreak WRITE setViewFormattingBreak )
    Q_PROPERTY( bool viewFormattingTabs READ viewFormattingTabs WRITE setViewFormattingTabs )
    Q_PROPERTY( bool viewFormattingSpace READ viewFormattingSpace WRITE setViewFormattingSpace )
    Q_PROPERTY( bool viewFormattingEndParag READ viewFormattingEndParag WRITE setViewFormattingEndParag )
    Q_PROPERTY( bool cursorInProtectedArea READ cursorInProtectedArea WRITE setCursorInProtectedArea )
    Q_PROPERTY( bool pgUpDownMovesCaret READ pgUpDownMovesCaret WRITE setPgUpDownMovesCaret )
    Q_PROPERTY( bool allowAutoFormat READ allowAutoFormat WRITE setAllowAutoFormat )
    Q_PROPERTY( int undoRedoLimit READ undoRedoLimit WRITE setUndoRedoLimit )

public:
    friend class KWOasisLoader;
    friend class KWStartupWidget;

    KWDocument( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );
    ~KWDocument();

    enum ProcessingType {WP = 0, DTP = 1};

    static const int CURRENT_SYNTAX_VERSION;

public:
    virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget=0);

    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* store );

    /**
     * Those values are used as KoGenStyle types.
     *
     * STYLE_FRAME_AUTO: for frame styles (family="graphic") used by actual frames, saved into content.xml
     * STYLE_FRAME_USER: for frame styles (family="graphic") defined by the user, saved into styles.xml
     * STYLE_TABLE_CELL_AUTO: for table-cell styles (family="table-cell") used by actual tables, saved into content.xml
     * STYLE_TABLE_CELL_USER: for table-cell styles (family="table-cell") defined by the user, saved into styles.xml
     * ...
     */
    enum {  STYLE_FRAME_AUTO = 20, STYLE_FRAME_USER,
            STYLE_TABLE_CELL_AUTO, STYLE_TABLE_CELL_USER,
            STYLE_TABLE, STYLE_TABLE_COLUMN, STYLE_TABLE_ROW };

    virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

    virtual int supportedSpecialFormats() const;

    enum SaveFlag { SaveAll, SaveSelected }; // kpresenter will also have SavePage

    /**
     * Return a drag object with the selected frames
     */
    Q3DragObject* dragSelected( const Q3ValueList<KWFrameView*> &selectedFrames);
    /**
     * Return a drag object with the selected text
     */
    Q3DragObject* dragSelected( QWidget *parent, KWTextFrameSet* fs );

    virtual bool loadXML( QIODevice *, const QDomDocument & dom );
    virtual bool loadChildren( KoStore *store );
    virtual QDomDocument saveXML();
    void processPictureRequests();
    void processAnchorRequests();
    bool processFootNoteRequests();

    int syntaxVersion( ) const { return m_syntaxVersion; }

    /// Called by KWFrame*'s loading code to emit correct progress info
    void progressItemLoaded();

    /**
     * Draw as embedded.
     */
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

    virtual QPixmap generatePreview( const QSize &size );

    /**
     * Document wide version of KWFrameSet::createEmptyRegion
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     * @param crect the cliprect; only parts inside this rect are of interrest to us
     * @param viewMode For coordinate conversion, always set.
     */
    void createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode * viewMode );
    /**
     * Erase the empty space defined by @p emptySpaceRegion.
     * Usually used to clear the space where there is no frame (e.g. page margins).
     */
    void eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush );

    virtual void setEmpty();

    virtual void addView( KoView *view );
    virtual void removeView( KoView *view );

    virtual void addShell( KoMainWindow *shell );

    KWPartFrameSet* insertObject( const KoRect& rect, KoDocumentEntry& e, QWidget* parentWidget );

    /// Create an embedded document; used by KWPartFrameSet but is defined here
    /// because KoDocument:insertChild is protected.
    KWDocumentChild* createChildDoc( const KoRect& rect, KoDocument* childDoc );

    void setPageLayout( const KoPageLayout& layout, const KoColumns& cl, const KoKWHeaderFooter& hf, bool updateViews = true );

    void getPageLayout( KoPageLayout& layout, KoColumns& cl, KoKWHeaderFooter& hf );

    KWTextFrameSet * textFrameSet ( unsigned int num ) const;
    /// Return the frameset number @p num
    KWFrameSet *frameSet( unsigned int num )
    { return m_lstFrameSet.at( num ); }

    /// Return the frameset with a given name
    KWFrameSet * frameSetByName( const QString & name );

    /// Return the total number of framesets
    unsigned int frameSetCount() const
    { return m_lstFrameSet.count(); }

    /// Generate a new name for a frameset. @p templateName must contain a %1 [for a number].
    QString generateFramesetName( const QString & templateName );

    /// Prefer this over frameSet(i), if iterating over all of them
    Q3PtrListIterator<KWFrameSet> framesetsIterator() const { return Q3PtrListIterator<KWFrameSet>(m_lstFrameSet); }

    Q3ValueList<KoTextObject *> visibleTextObjects(KWViewMode *viewmode) const;

    /// Register new frameset
    void addFrameSet( KWFrameSet *f, bool finalize = true );
    /// Remove frameset from list (don't delete)
    void removeFrameSet( KWFrameSet *f );

    /// Frame/table deletion - with undo/redo support
    /// Moved to KWDocument so that dialogs can call them if necessary
    void deleteTable( KWTableFrameSet *groupManager );
    void deleteFrame( KWFrame * frame );

    /// return the height of one page in pixels in the current zoom level (normal coord system)
    unsigned int paperHeight(int pageNum) const;
    /// return the width of one page in pixels in the current zoom level (normal coord system)
    unsigned int paperWidth(int pageNum) const;
    /// Top of the page number pgNum, in pixels (in the normal coord system)
    unsigned int pageTop( int pgNum ) const;
    double ptColumnWidth() const;
    double ptColumnSpacing() const { return m_pageColumns.ptColumnSpacing; }
    double ptFootnoteBodySpacing() const { return m_pageHeaderFooter.ptFootNoteBodySpacing; }

    unsigned int numColumns() const { return m_pageColumns.columns; }

    void repaintAllViews( bool erase = false );
    /** Update all views of this document, area can be cleared before redrawing with the
     * erase flag. (false implied). All views EXCEPT the argument view are updated ( give 0L for all )
     */
    void repaintAllViewsExcept( KWView *view, bool erase = false );


    /**
     * schedule a repaint of all views but don't execute immediately
     */
    void delayedRepaintAllViews();

    /**
     * schedule a frame layout (e.g. for footnotes) but don't execute immediately
     */
    void delayedRecalcFrames( int fromPage );

    /**
     * Return a double-buffer pixmap of (at least) the given size.
     */
    QPixmap* doubleBufferPixmap( const QSize& );
    /**
     * Call this when you're done with the double-buffer pixmap (at the
     * end of the current painting, for all objects that need to be painted).
     * If it's too big, KWDocument will delete it to save memory.
     */
    void maybeDeleteDoubleBufferPixmap();

    /**
     * Tell this method when a frame is moved / resized / created / deleted
     * and everything will be update / repainted accordingly.
     */
    void frameChanged( KWFrame * frame );
    void framesChanged( const Q3PtrList<KWFrame> & frames, KWView * view = 0L );

    QString uniqueFramesetName( const QString& oldName );
    /**
     * @param topElem the DOM element that contains the OASIS document
     * @param macroCmd for undo/redo purposes, pass this if you want to undo the paste later
     * @param copyFootNote ???
     * @param dontCreateFootNote true when we copy footnote into an other frameset than mainFrameSet => footnote is removed !
     * @param selectFrames if true, pasted frames are auto-selected. Set to false when loading from a file etc.
     */
    void pasteFrames( QDomElement topElem, KMacroCommand * macroCmd, bool copyFootNote = false, bool dontCreateFootNote = false, bool selectFrames = true );

    void insertEmbedded( KoStore *store, QDomElement topElem, KMacroCommand * macroCmd, double offset );
    void completePasting();
    void completeOasisPasting();
    void saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles, KoSavingContext& savingContext, SaveFlag saveFlag, const QByteArray& headerFooterContent ) const;

    KoStyleCollection * styleCollection()const  { return m_styleColl;}
    KWFrameStyleCollection * frameStyleCollection()const  { return m_frameStyleColl;}
    KWTableStyleCollection * tableStyleCollection()const  { return m_tableStyleColl;}
    KWTableTemplateCollection * tableTemplateCollection()const  { return m_tableTemplateColl;}

    QFont defaultFont() const { return m_defaultFont; }
    void setDefaultFont( const QFont & newFont ) {
        m_defaultFont = newFont;
    }

    /**
     * returns the amount of pages in the document.
     * @see startPage() @see lastPage()
     */
    int pageCount() const;
    /**
     * returns the page number of the first page in this document, this is the page number
     * that will be shown on prints and used in the TOC and user-variables.
     * @see pageCount() @see lastPage()
     */
    int startPage() const;
    /**
     * Returns the last page number in this document.
     * With a higher startPage and a constante pagecount this number will also get higher.
     */
    int lastPage() const;

    KoPictureCollection *pictureCollection() { return m_pictureCollection; }
    KoVariableFormatCollection *variableFormatCollection()const { return m_varFormatCollection; }

    Q3ValueList<KWView *> getAllViews() const { return m_lstViews; }

    /**
     * Insert a new page after another,
     * creating followup frames (but not headers/footers),
     * @param afterPageNum the page is inserted after the one specified here
     * If afterPageNum is -1, a page is inserted before page 0.
     * In all cases, the new page will have the number afterPageNum+1.
     * Use appendPage in WP mode, insertPage in DTP mode.
     */
    KWPage* insertPage( int afterPageNum );
    /**
     * Append a new page, creating followup frames (but not headers/footers),
     * and return the page number.
     */
    KWPage* appendPage();
    /**
     * Call this after appendPage, to get headers/footers on the new page,
     * and all the caches properly updated. This is separate from appendPage
     * so that KWFrameLayout can call appendPage() only.
     */
    void afterInsertPage( int num );
    /**
     * @return list of frames that will be copied onto the new page
     * Used by insertPage but also by KWTextFrameSet to check if it's worth
     * auto-inserting a new page (to avoid infinite loops if not)
     */
    Q3PtrList<KWFrame> framesToCopyOnNewPage( int afterPageNum ) const;

    /**
     * Remove a page. Call afterRemovePages() after removing one or more pages.
     */
    void removePage( int num );

    /**
     * Update things after removing one or more pages.
     */
    void afterRemovePages();

    /**
     * Check if we can remove empty page(s) from the end
     * If so, do it and return true.
     * Note that this doesn't call afterRemovePages, this is up to the caller.
     */
    bool tryRemovingPages();

    ProcessingType processingType()const { return m_processingType;  }
    int frameSetNum( KWFrameSet* fs ) { return m_lstFrameSet.findRef( fs ); }

    void lowerMainFrames( int pageNum );
    void lowerMainFrames( int pageNum, int lowestZOrder );

    /// \note This method considers _all_ text framesets, even table cells
    Q3PtrList<KWTextFrameSet> allTextFramesets( bool onlyReadWrite ) const;
    /// \note This method considers _all_ text framesets, even table cells
    int numberOfTextFrameSet( KWFrameSet* fs, bool onlyReadWrite );
    /// \note This method considers _all_ text framesets, even table cells
    KWFrameSet * textFrameSetFromIndex( unsigned int num, bool onlyReadWrite );

    /// Reimplementation from KoDocument.
    /// \note This method considers _all_ text framesets, even table cells
    virtual Q3ValueList<KoTextDocument *> allTextDocuments() const;


    /** Gather all the frames which are on a certain page and return them.
     * The list is ordered. @see KWFrameSet::framesInPage
     * @param pageNum the number of the page
     * @param sorted if true the list is ordered per z-order. should be true always.
     */
    Q3PtrList<KWFrame> framesInPage( int pageNum , bool sorted=true) const;


    /**
     * Max z-order among all frames on the given page
     * \note There is no minZOrder() method, because of the main frameset, see kwview::lowerFrame
     */
    int maxZOrder( int pageNum ) const;

    void updateAllFrames( int flags = 0xff /* see KWFrameSet::UpdateFramesFlags */ );

    // The grid is in _pt_ now
    double gridX()const { return m_gridX; }
    double gridY()const { return m_gridY; }
    void setGridX(double gridx);
    void setGridY(double gridy) { m_gridY = gridy; }

    void updateGridButton();

    bool showGrid() const { return m_bShowGrid; }
    void setShowGrid ( bool grid ) { m_bShowGrid = grid; }

    bool snapToGrid() const { return m_bSnapToGrid; }
    void setSnapToGrid( bool b ) { m_bSnapToGrid = b; }

    // Currently unused. Not sure we want to go that way, now that we have
    // paragLayoutChanged and formatChanged in applyStyleChange.
    //int applyStyleChangeMask() { return styleMask; }
    //void setApplyStyleChangeMask( int f ) { styleMask = f; }

    // paragLayoutChanged is a set of flags for the parag layout - see the enum in KWParagLayout
    // formatChanged is a set of flags from KoTextFormat
    // If both are -1, it means the style has been deleted.
    void applyStyleChange( KoStyleChangeDefMap changed );
    void updateAllStyleLists();
    void updateStyleListOrder( const QStringList &list );

    void updateAllFrameStyleLists();
    void updateAllTableStyleLists();

    bool isHeaderVisible() const { return m_headerVisible; }
    bool isFooterVisible() const { return m_footerVisible; }
    void setHeaderVisible( bool h );
    void setFooterVisible( bool f );
    bool hasEndNotes() const;

    /**
     * Recalculate and (re)position the main frame and header/footers.
     * @param fromPage the start pagenumber to do the recalculation on.
     * @param toPage the last page that will be recalulated
     * @param flags see KWFrameLayout
     */
    void recalcFrames( int fromPage = 0, int toPage = -1, uint flags = 0 );

    KoHFType headerType() const { return m_pageHeaderFooter.header; }
    KoHFType footerType() const { return m_pageHeaderFooter.footer; }
    const KoKWHeaderFooter& headerFooterInfo() const { return m_pageHeaderFooter; }

    void setFrameCoords( double x, double y, double w, double h );

    void addCommand( KCommand * cmd );

    KCommandHistory * commandHistory() const { return m_commandHistory; }
    KoAutoFormat * autoFormat() const { return m_autoFormat; }

    /**
     * This is used upon loading, to delay certain things until completeLoading,
     * for KWTextImage
     */
    void addTextImageRequest( KWTextImage *img );
    /**
     * This is used upon loading, to delay certain things until completeLoading,
     * for KWPictureFrameSet
     */
    void addPictureRequest( KWPictureFrameSet *fs );
    /**
     * This is used upon loading, to delay certain things until completeLoading,
     * for KWTextParag
     */
    void addAnchorRequest( const QString &framesetName, const KWAnchorPosition &anchorPos );
    /**
     * This is used upon loading, to delay certain things until completeLoading,
     * for KWFootNoteVariable
     */
    void addFootNoteRequest( const QString &framesetName, KWFootNoteVariable* var );

    /// This is used by loadFrameSets() and by KWCanvas to paste framesets
    KWFrameSet *loadFrameSet( QDomElement framesetElem, bool loadFrames = true , bool loadFootnote = true);
    void loadEmbeddedObjects( QDomElement& word );
    void saveEmbeddedObjects( QDomElement& parentElem, const Q3PtrList<KoDocumentChild>& childList );
    void loadEmbedded( const QDomElement &embedded );

    void recalcVariables( int type );

    KWVariableCollection *variableCollection() const { return m_varColl; }

    KWMailMergeDataBase *mailMergeDataBase() const { return m_slDataBase; }
    int mailMergeRecord() const;
    void setMailMergeRecord( int r );

    bool backgroundSpellCheckEnabled() const;
    void enableBackgroundSpellCheck( bool b );

    bool canRemovePage( int num );

    /**
     * Change the zoom factor to @p z (e.g. 150 for 150%)
     * and/or change the resolution, given in DPI.
     * This is done on startup and when printing.
     * The same call combines both so that all the updating done behind
     * the scenes is done only once, even if both zoom and DPI must be changed.
     */
    virtual void setZoomAndResolution( int zoom, int dpiX, int dpiY );

    void newZoomAndResolution( bool updateViews, bool forPrint );

    /**
     * Due to the way the text formatter works (it caches layout information in
     * the paragraphs and characters), we currently can't have one viewmode per view.
     * It has to be the same for all views.
     */
    QString viewModeType() const { return m_viewModeType; }

    /**
     * The view mode used for text layouting.
     */
    KWViewMode* layoutViewMode() const { return m_layoutViewMode; }

    /**
     * Changes m_viewMode, and updates all views to this viewmode
     */
    void switchViewMode( const QString& newViewMode );


    /// \todo useless method
    static QString getAttribute(QDomElement &element, const char *attributeName, const QString &defaultValue)
      {
          return element.attribute( attributeName, defaultValue );
      }

    static int getAttribute(QDomElement &element, const char *attributeName, int defaultValue)
    {
        QString value;
        if ( ( value = element.attribute( attributeName ) ) != QString::null )
            return value.toInt();
        else
            return defaultValue;
    }

    static double getAttribute(QDomElement &element, const char *attributeName, double defaultValue)
    {
        QString value;
        if ( ( value = element.attribute( attributeName ) ) != QString::null )
            return value.toDouble();
        else
            return defaultValue;
    }

#ifndef NDEBUG
    void printStyleDebug();
    void printDebug();
#endif

    /** calls layout() on all framesets  */
    void layout();

    // This settings has to be here [instead of KWView] because we need to
    // format paragraphs slightly differently (to add room for the CR char)
    bool viewFormattingChars() const { return m_viewFormattingChars; }
    void setViewFormattingChars(bool b) { m_viewFormattingChars=b; }

    bool viewFormattingEndParag() const { return m_viewFormattingEndParag; }
    void setViewFormattingEndParag(bool b) { m_viewFormattingEndParag=b; }

    bool viewFormattingSpace() const { return m_viewFormattingSpace; }
    void setViewFormattingSpace(bool b) { m_viewFormattingSpace=b; }

    bool viewFormattingTabs() const { return m_viewFormattingTabs; }
    void setViewFormattingTabs(bool b) { m_viewFormattingTabs=b; }

    bool viewFormattingBreak() const { return m_viewFormattingBreak; }
    void setViewFormattingBreak(bool b) { m_viewFormattingBreak=b; }

    // Also view properties, but stored, loaded and saved here (lacking a more global object).
    bool viewFrameBorders() const { return m_viewFrameBorders; }
    void setViewFrameBorders( bool b );
    void setShowRuler(bool ruler){ m_bShowRuler=ruler; }
    bool showRuler() const { return m_bShowRuler; }

    bool showStatusBar() const { return m_bShowStatusBar;}
    void setShowStatusBar( bool status ) { m_bShowStatusBar = status;}

    bool showScrollBar() const { return m_bShowScrollBar; }
    void setShowScrollBar( bool status ) { m_bShowScrollBar = status;}

    bool pgUpDownMovesCaret() const { return m_pgUpDownMovesCaret; }
    void setPgUpDownMovesCaret( bool b ) { m_pgUpDownMovesCaret = b; }

    bool showdocStruct() const {return  m_bShowDocStruct;}
    void setShowDocStruct(bool b){m_bShowDocStruct=b;}

    bool allowAutoFormat() const { return m_bAllowAutoFormat; }
    void setAllowAutoFormat(bool b){ m_bAllowAutoFormat=b; }

    bool insertDirectCursor() const { return m_bInsertDirectCursor; }
    void setInsertDirectCursor(bool b);


    // in pt
    double indentValue()const { return m_indent; }
    void setIndentValue(double ind) { m_indent=ind; }

    int nbPagePerRow() const{ return m_iNbPagePerRow; }
    void setNbPagePerRow(int nb) { m_iNbPagePerRow=nb; }

    int maxRecentFiles() const { return m_maxRecentFiles; }


    // in pt
    double defaultColumnSpacing()const{ return m_defaultColumnSpacing ;}
    void setDefaultColumnSpacing(double val){ m_defaultColumnSpacing=val; }
    /**
     * @returns the document for the formulas
     * @param init if true mathematical fonts may be installed if needed.
     * Should be true unless no real document is being used (i. e. in
     * configuration dialog>
     */
    KFormula::Document* formulaDocument( bool init = true );

    void reorganizeGUI();
    /// Tell all views to stop editing this frameset, if they were doing so
    void terminateEditing( KWFrameSet * frameSet )
        { emit sig_terminateEditing( frameSet ); }

    void clearUndoRedoInfos();

    void refreshDocStructure(FrameSetType);
    void refreshDocStructure(int);

    int typeItemDocStructure(FrameSetType type);

    void refreshMenuExpression();

    void refreshMenuCustomVariable();

    void updateZoomRuler();

    /// Mark the document to have a table of contents and update the view-menu-item text
    void setTocPresent(bool hasToc);
    /// Returns if the document has a table of contents
    bool tocPresent(){return m_hasTOC;}

    QString sectionTitle( int pageNum ) const;

    void updateRulerFrameStartEnd();

    /** Convert a color into a color to be displayed for it
     * (when using color schemes, we still want to print black on white).
     * See also KoTextFormat::defaultTextColor. */
    static QColor resolveTextColor( const QColor & col, QPainter * painter );
    static QColor defaultTextColor( QPainter * painter );
    static QColor resolveBgColor( const QColor & col, QPainter * painter = 0 );
    static QBrush resolveBgBrush( const QBrush & brush, QPainter * painter = 0 );
    static QColor defaultBgColor( QPainter * painter );


    virtual DCOPObject* dcopObject();

    int undoRedoLimit() const;
    void setUndoRedoLimit(int val);

    void updateContentsSize(){emit newContentsSize();}

    void refreshGUIButton();

    void initConfig();
    void saveConfig();
    void startBackgroundSpellCheck();
    void reactivateBgSpellChecking();

    void updateHeaderButton();
    void updateFooterButton();

    QStringList spellCheckIgnoreList() const { return m_spellCheckIgnoreList; }
    void setSpellCheckIgnoreList( const QStringList& lst );
    void addSpellCheckIgnoreWord( const QString & );

    void updateTextFrameSetEdit();
    void changeFootNoteConfig();
    void displayFootNoteFieldCode();


    double tabStopValue() const { return m_tabStop; }
    void setTabStopValue ( double tabStop );

    void changeBgSpellCheckingState( bool b );

    // To position the cursor when opening a document
    QString initialFrameSet() const; ///< \note can be empty for "unset"
    int initialCursorParag() const;
    int initialCursorIndex() const;
    /// Once we're done with this info, get rid of it
    void deleteInitialEditingInfo();

    bool cursorInProtectedArea()const;
    void setCursorInProtectedArea( bool b );

    SeparatorLinePos footNoteSeparatorLinePosition()const { return m_footNoteSeparatorLinePos;}
    void setFootNoteSeparatorLinePosition(SeparatorLinePos pos) {m_footNoteSeparatorLinePos = pos;}

    int footNoteSeparatorLineLength() const { return m_iFootNoteSeparatorLineLength;}
    void setFootNoteSeparatorLineLength( int length){  m_iFootNoteSeparatorLineLength = length;}

    double footNoteSeparatorLineWidth() const { return m_footNoteSeparatorLineWidth;}
    void setFootNoteSeparatorLineWidth( double width){  m_footNoteSeparatorLineWidth=width;}

    SeparatorLineLineType footNoteSeparatorLineType()const { return m_footNoteSeparatorLineType;}
    void setFootNoteSeparatorLineType( SeparatorLineLineType type) {m_footNoteSeparatorLineType = type;}

    const KoTextBookmarkList* bookmarkList() const { return m_bookmarkList; }
    void insertBookmark( const QString &name, KoTextParag *startparag, KoTextParag *endparag, int start, int end );
    void deleteBookmark( const QString &name );
    void renameBookmark( const QString &oldname, const QString &newName );

    const KoTextBookmark * bookmarkByName( const QString & name ) const;
    QStringList listOfBookmarkName(KWViewMode * viewMode) const;

    void paragraphDeleted( KoTextParag *parag, KWFrameSet *frm);
    void paragraphModified(KoTextParag* parag, int /*KoTextParag::ParagModifyType*/ type, int start, int lenght);

    void initBookmarkList();
    void loadImagesFromStore( KoStore *store );
    void loadPictureMap ( QDomElement& domElement );

    void testAndCloseAllFrameSetProtectedContent();
    void updateRulerInProtectContentMode();

    KoPageLayout pageLayout(int pageNumber = 0) const;

    QStringList personalExpressionPath() const { return m_personalExpressionPath;}
    void setPersonalExpressionPath( const QStringList & );


    void updateDirectCursorButton();

    QString globalLanguage()const { return m_globalLanguage; }
    void setGlobalLanguage( const QString & lang ){m_globalLanguage = lang;}
    void addWordToDictionary( const QString & );

    bool globalHyphenation() const { return m_bGlobalHyphenation; }
    void setGlobalHyphenation ( bool hyphen );

    KWLoadingInfo* createLoadingInfo();
    KWLoadingInfo* loadingInfo() const { return m_loadingInfo; }
    void deleteLoadingInfo();

    KFormula::DocumentWrapper* formulaDocumentWrapper() { return m_formulaDocumentWrapper; }

    KWPageManager *pageManager() const { return m_pageManager; }

    KWBgSpellCheck* backSpeller() const { return m_bgSpellCheck; }

    /// Load the given page layout; public for KWTextParag
    bool loadOasisPageLayout( const QString& masterPageName, KoOasisContext& context );

    // end of public methods
signals:

    /// This is emitted by setPageLayout if updateViews=true
    void pageLayoutChanged( const KoPageLayout& );

    /// Emitted when the scrollview contents must be resized (e.g. new page, new layout...)
    void newContentsSize();

    /** This is emitted when the height of the text in the main frameset changes
     * \note Mostly useful for the text viewmode.
     */
    void mainTextHeightChanged();

    /// This is emitted when the number of pages changes.
    void numPagesChanged();

    void docStructureChanged(int);
    void sig_terminateEditing( KWFrameSet * fs );

    void sig_refreshMenuCustomVariable();

    void sigFrameSetAdded(KWFrameSet*);
    void sigFrameSetRemoved(KWFrameSet*);

public slots:
    void slotRepaintChanged( KWFrameSet * frameset );
    void framesChanged( const Q3ValueList<KWFrame*> &frames);

    /** calls invalidate() on all framesets  */
    void invalidate(const KWFrameSet *skipThisFrameSet=0);

    virtual void initEmpty();

protected slots:
    void slotRecalcFrames();
    void slotRepaintAllViews();
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotDocumentInfoModifed();
    void slotChapterParagraphFormatted( KoTextParag* parag );
    void saveDialogShown(); ///< called just before the save-dialog is shown

    virtual void openExistingFile( const QString& file );
    virtual void openTemplate( const QString& file );

private slots:
    /// is called from a singleShot timer due to frameChanged()
    void updateFramesChanged();

protected:
    void nextParagraphNeedingCheck();
    /// fix up Z-order for import from older kword versions.
    void fixZOrders();
    QString checkSectionTitleInParag( KoTextParag* parag, KWTextFrameSet*, int pageNum ) const;
    KoView* createViewInstance( QWidget* parent, const char* name );

    virtual bool completeLoading( KoStore* store );
    virtual bool completeSaving( KoStore *store );

    void loadFrameSets( const QDomElement &framesets );
    void loadStyleTemplates( const QDomElement &styles );
    void saveStyle( KoParagStyle *sty, QDomElement parentElem );
    void saveFrameStyle( KWFrameStyle *sty, QDomElement parentElem );
    void saveTableStyle( KWTableStyle *sty, QDomElement parentElem );

    void loadFrameStyleTemplates( const QDomElement &styles );
    void loadDefaultFrameStyleTemplates();
    void loadTableStyleTemplates( const QDomElement &styles );
    void loadDefaultTableStyleTemplates();
    void loadDefaultTableTemplates();
    void loadMasterPageStyle( const QString& masterPageName, KoOasisContext& context );

    void saveOasisBody( KoXmlWriter& writer, KoSavingContext& context ) const;
    void saveOasisCustomFied( KoXmlWriter &writer )const;

    Q3ValueList<KoPictureKey> savePictureList();

    /// helper method for the 2 different dragSelected() versions
    Q3DragObject* dragSelectedPrivate( QWidget *parent, const Q3ValueList<KWFrameView*> &selectedFrames, KWTextFrameSet* fs);
    /**
     * Save the whole document, or just the selection, into OASIS format
     * When saving the selection, also return the data as plain text and/or plain picture,
     * which are used to insert into the KMultipleDrag drag object.
     *
     * @param store the KoStore to save into
     * @param manifestWriter pointer to a koxmlwriter to add entries to the manifest
     * @param saveFlag either the whole document, or only the selected text/objects.
     * @param plainText must be set when saveFlag==SaveSelected.
     *        It returns the plain text format of the saved data, when available.
     * @param picture must be set when saveFlag==SaveSelected.
     *        It returns the selected picture, when exactly one picture was selected.
     * @param fs the text frameset, which must be set when saveFlag==SaveSelected.
     * @param selectedFrames a list of the selected frames to save. This has to be passed since
     *        selection is per view, not per document.
     */
    bool saveOasisHelper( KoStore* store, KoXmlWriter* manifestWriter, SaveFlag saveFlag,
                    const Q3ValueList<KWFrameView*> &selectedFrames,
                    QString* plainText = 0, KoPicture* picture = 0, KWTextFrameSet* fs = 0 );

    void saveOasisSettings( KoXmlWriter &settingsWriter ) const;
    void saveSelectedFrames( KoXmlWriter& bodyWriter,
                             KoSavingContext& savingContext, Q3ValueList<KoPictureKey>& pictureList,
                             const Q3ValueList<KWFrameView*> &selectedFrames, QString* plainText ) const;

    // inherited from KoDocument
    QWidget* createCustomDocumentWidget(QWidget *parent);

private:
    void clear();
    void endOfLoading();

    class FramesChangedHandler {
        public:
            FramesChangedHandler(KWDocument *parent);
            void addFrame(KWFrame*);
            void addFrameSet(KWFrameSet*);
            void execute();
        private:
            KWDocument *m_parent;
            Q3ValueList<KWFrameSet*> m_frameSets;
            bool m_needLayout;
    };

    // Variables:
    Q3ValueList<KWView *> m_lstViews;

    KoColumns m_pageColumns;
    KoKWHeaderFooter m_pageHeaderFooter;

    KoPictureCollection* m_pictureCollection;

    Q3PtrList<KWFrameSet> m_lstFrameSet;

    unsigned int m_itemsLoaded;
    unsigned int m_nrItemsToLoad;

    ProcessingType m_processingType;
    double m_gridX, m_gridY;

    DCOPObject *dcop;

    KCommandHistory * m_commandHistory;
    KoAutoFormat * m_autoFormat;

    // ===== Legacy loading stuff, remove when switching to OASIS =====
    /// \note Shared between loadXML and loadComplete
    QString m_urlIntern;

    QMap<KoPictureKey, QString> m_pictureMap;

    /// List used to help loading and saving images of the old type ("text image" of class KWTextImage)
    Q3PtrList<KWTextImage> m_textImageRequests;
    Q3PtrList<KWPictureFrameSet> m_pictureRequests;
    QMap<QString, KWAnchorPosition> m_anchorRequests;
    QMap<QString, KWFootNoteVariable *> m_footnoteVarRequests; ///< \todo still needed? (move to KWLoadingInfo if so)
    // ===== End of legacy loading stuff =====

    QMap<QString,QString> * m_pasteFramesetsMap;

    KoVariableFormatCollection *m_varFormatCollection;
    KWMailMergeDataBase *m_slDataBase;
    int slRecordNum;

    /**
     * When a document is written out, the syntax version in use will be recorded. When read back
     * in, this variable reflects that value.
     * \note KWord legacy format only
     */
    int m_syntaxVersion;

    QFont m_defaultFont;
    bool m_headerVisible, m_footerVisible;
    bool m_viewFrameBorders;
    bool m_bShowRuler;
    bool m_bShowDocStruct;
    bool m_hasTOC;
    bool m_bShowStatusBar;
    bool m_pgUpDownMovesCaret;
    bool m_repaintAllViewsPending;
    bool m_bAllowAutoFormat;
    bool m_bShowScrollBar;
    bool m_cursorInProtectectedArea;
    bool m_bInsertDirectCursor;
    bool m_bHasEndNotes;

    bool m_viewFormattingChars;
    bool m_viewFormattingEndParag;
    bool m_viewFormattingSpace;
    bool m_viewFormattingTabs;
    bool m_viewFormattingBreak;

    /** The wrapper that contains the formula's document and its
     * actions. It owns the real document.
     */
    KFormula::DocumentWrapper* m_formulaDocumentWrapper;

    double m_indent; ///< \note in pt
    double m_defaultColumnSpacing;

    int m_iNbPagePerRow;
    int m_maxRecentFiles;
    int m_recalcFramesPending;

    /// The name of the viewmode used by all views.
    QString m_viewModeType;
    /// The viewmode used for text layouting
    KWViewMode* m_layoutViewMode;
    KWVariableCollection *m_varColl;
    KWBgSpellCheck *m_bgSpellCheck;
    KoStyleCollection *m_styleColl;
    KWFrameStyleCollection *m_frameStyleColl;
    KWTableStyleCollection *m_tableStyleColl;
    KWTableTemplateCollection *m_tableTemplateColl;


    SeparatorLinePos m_footNoteSeparatorLinePos;
    /// It's a percentage of page.
    int m_iFootNoteSeparatorLineLength;

    double m_footNoteSeparatorLineWidth;

    SeparatorLineLineType m_footNoteSeparatorLineType;

    /** Page number -> section title array, for the Section variable.
     * Note that pages without a section title don't appear in the array. */
    Q3ValueVector< QString > m_sectionTitles;

    double m_tabStop;
    QStringList m_spellCheckIgnoreList; // per-document
    QStringList m_spellCheckPersonalDict; // per-user
    QPixmap* m_bufPixmap;

    KWLoadingInfo* m_loadingInfo;

    class InitialEditing;
    /// \note Remains alive a little bit longer than the loading info (until KWCanvas ctor)
    InitialEditing *m_initialEditing;

    KoTextBookmarkList* m_bookmarkList;

    QStringList m_personalExpressionPath;
    QString m_globalLanguage;
    bool m_bGlobalHyphenation;
    bool m_bGeneratingPreview;

    bool m_bShowGrid;
    bool m_bSnapToGrid;

    KWPageManager *m_pageManager;
    FramesChangedHandler *m_framesChangedHandler;
};

#endif
