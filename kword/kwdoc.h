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

#ifndef kwdoc_h
#define kwdoc_h

class KWDocument;
class KPrinter;
class KWTextImage;
class KWPictureFrameSet;
class KWClipartFrameSet;
class KWSerialLetterDataBase;
class KWFrameSet;
class KWTableFrameSet;
class KWPartFrameSet;
class KoStyle;
#define KWStyle KoStyle
class KWFrame;
class KWView;
class KWViewMode;
class KMacroCommand;
class KoDocumentEntry;
class QPainter;
class KSpellConfig;
class KoAutoFormat;
class KCommand;
class KCommandHistory;
class KoVariable;
class KoVariableFormatCollection;
class KWVariableCollection;
class KoTextObject;

class DCOPObject;

class QFont;
class QStringList;
class QRect;

namespace KFormula {
    class Document;
}


#include <koDocument.h>
#include <koGlobal.h>
#include <koDocumentChild.h>
#include <koClipartCollection.h>
#include <kozoomhandler.h>
#include "kwimage.h"
#include "kwanchorpos.h"
#include <koUnit.h>
#include "defs.h"
#include "variable.h"

#include <qmap.h>
#include <qptrlist.h>
#include <qfont.h>

/******************************************************************/
/* Class: KWChild                                              */
/******************************************************************/

class KWChild : public KoDocumentChild
{
public:
    KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc );
    KWChild( KWDocument *_wdoc );
    ~KWChild();

    KWDocument* parent()
    { return m_pKWordDoc; }

    virtual KoDocument *hitTest( const QPoint &, const QWMatrix & );

protected:
    KWDocument *m_pKWordDoc;

};

/******************************************************************/
/* Class: KWDocument                                           */
/******************************************************************/

class KWDocument : public KoDocument, public KoZoomHandler
{
    Q_OBJECT

public:
    KWDocument( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KWDocument();

    enum ProcessingType {WP = 0, DTP = 1};
    /*
    static const int U_FONT_FAMILY_SAME_SIZE = 1;
    static const int U_FONT_ALL_SAME_SIZE = 2;
    static const int U_COLOR = 4;
    static const int U_INDENT = 8;
    static const int U_BORDER = 16;
    static const int U_ALIGN = 32;
    static const int U_NUMBERING = 64;
    static const int U_FONT_FAMILY_ALL_SIZE = 128;
    static const int U_FONT_ALL_ALL_SIZE = 256;
    static const int U_TABS = 512;
    static const int U_SMART = 1024;
    */

    static const int CURRENT_SYNTAX_VERSION = 2;

public:
    virtual bool initDoc();
    void initEmpty();

    virtual bool loadXML( QIODevice *, const QDomDocument & dom );
    virtual bool loadChildren( KoStore *_store );
    virtual QDomDocument saveXML();
    void processImageRequests();
    void processAnchorRequests();

    int syntaxVersion( ) const { return m_syntaxVersion; }

    // Called by KWFrame*'s loading code to emit correct progress info
    void progressItemLoaded();

    /**
     * Draw as embedded.
     */
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );
    /**
     * @param emptyRegion The region is modified to subtract the areas painted, thus
     *                    allowing the caller to determine which areas remain to be painted.
     */
    void createEmptyRegion( const QRect & crect, QRegion & emptyRegion, KWViewMode * viewMode );
    /**
     * Erase the empty space defined by @p emptySpaceRegion.
     * Usually used to clear the space where there is no frame (e.g. page margins).
     */
    void eraseEmptySpace( QPainter * painter, const QRegion & emptySpaceRegion, const QBrush & brush );

    virtual void addView( KoView *_view );
    virtual void removeView( KoView *_view );

    virtual void addShell( KoMainWindow *shell );

    virtual void insertObject( const KoRect& _rect, KoDocumentEntry& _e );
    void setPageLayout( KoPageLayout _layout, KoColumns _cl, KoKWHeaderFooter _hf );

    void getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf );
    KoPageLayout pageLayout() const { return m_pageLayout; }

    // Return the frameset number @p _num
    KWFrameSet *frameSet( unsigned int _num )
    { return frames.at( _num ); }

    // Return the frameset with a given name
    KWFrameSet * frameSetByName( const QString & name );

    // Returns the frame under the mouse (or 0)
    // The bool border is true if the mouse is on a border, false if it's inside the frame
    KWFrame * frameUnderMouse( const QPoint& nPoint, bool* border = 0L );

    // Return the total number of framesets
    unsigned int getNumFrameSets()
    { return frames.count(); }

    // Generate a new name for a frameset. @p templateName must contain a %1 [for a number].
    QString generateFramesetName( const QString & templateName );

    // Prefer this over frameSet(i), if iterating over all of them
    QPtrListIterator<KWFrameSet> framesetsIterator() const { return QPtrListIterator<KWFrameSet>(frames); }

    QPtrList<KoTextObject> frameTextObject() const;

    void addFrameSet( KWFrameSet *f, bool finalize = true );
    // Remove frameset from list (don't delete)
    void removeFrameSet( KWFrameSet *f );

    // Frame/table deletion - with undo/redo support
    // Moved to KWDocument so that dialogs can call them if necessary
    void deleteTable( KWTableFrameSet *groupManager );
    void deleteFrame( KWFrame * frame );

    void deleteSeveralFrame();

    // Those distances are in _pixels_, i.e. with zoom and resolution applied.
    unsigned int topBorder() const { return static_cast<unsigned int>(zoomItY( m_pageLayout.ptTop )); }
    unsigned int bottomBorder() const { return static_cast<unsigned int>(zoomItY( m_pageLayout.ptBottom )); }
    unsigned int leftBorder() const { return static_cast<unsigned int>(zoomItX( m_pageLayout.ptLeft )); }
    unsigned int rightBorder() const { return static_cast<unsigned int>(zoomItX( m_pageLayout.ptRight )); }
    // WARNING: don't multiply this value by the number of the page, this leads to rounding problems.
    unsigned int paperHeight() const { return static_cast<unsigned int>(zoomItY( m_pageLayout.ptHeight )); }
    unsigned int paperWidth() const { return static_cast<unsigned int>(zoomItX( m_pageLayout.ptWidth )); }
    unsigned int columnSpacing() const { return static_cast<unsigned int>(zoomItX( m_pageColumns.ptColumnSpacing )); }
    // Top of the page number pgNum, in pixels (in the normal coord system)
    unsigned int pageTop( int pgNum /*0-based*/ ) const { return zoomItY( ptPageTop( pgNum ) ); }

    // Those distances are in _pt_, i.e. the real distances, stored in m_pageLayout
    double ptTopBorder() const { return m_pageLayout.ptTop; }
    double ptBottomBorder() const { return m_pageLayout.ptBottom; }
    double ptLeftBorder() const { return m_pageLayout.ptLeft; }
    double ptRightBorder() const { return m_pageLayout.ptRight; }
    double ptPaperHeight() const { return m_pageLayout.ptHeight; }
    double ptPaperWidth() const { return m_pageLayout.ptWidth; }
    double ptColumnWidth() const;
    double ptColumnSpacing() const { return m_pageColumns.ptColumnSpacing; }
    double ptPageTop( int pgNum /*0-based*/ ) const { return pgNum * m_pageLayout.ptHeight; }

    unsigned int getColumns() const { return m_pageColumns.columns; }

    // Returns 0-based page number where rect is (in real pt coordinates)
    // (in fact its topleft corner).
    // Use isOutOfPage to check that the rectangle is fully contained in that page.
    int getPageOfRect( KoRect & _rect ) const;

    // Return true if @p r (in real pt coordinates) is out of the page @p page
    bool isOutOfPage( KoRect & r, int page ) const;

    //update koRuler in each view
    void updateRuler();

    void repaintAllViews( bool erase = false );
    void repaintAllViewsExcept( KWView *_view, bool _erase = false );
    /**
     * Tell this method when a frame is moved / resized / created / deleted
     * and everything will be update / repainted accordingly.
     * If one view is already uptodate, pass it in @p view.
     */
    void frameChanged( KWFrame * frame, KWView * view = 0L );
    void framesChanged( const QPtrList<KWFrame> & frames, KWView * view = 0L );

    void pasteFrames( QDomElement topElem, KMacroCommand * macroCmd );

    const QPtrList<KWStyle> & styleList() const { return m_styleList; }
    /**
     * Look for a style named @p name. If not found, it will return 0L.
     */
    KWStyle* findStyle( const QString & name );
    /**
     * Return style number @p i.
     */
    KWStyle* styleAt( int i ) { return m_styleList.at(i); }

    QFont defaultFont() const { return m_defaultFont; }

    int getPages() const { return m_pages; }

    KWImageCollection *imageCollection() { return &m_imageCollection; }
    KoClipartCollection *clipartCollection() { return &m_clipartCollection; }
    KoVariableFormatCollection *variableFormatCollection() { return m_varFormatCollection; }

    QPtrList <KWView> getAllViews() { return m_lstViews; }

    void appendPage( /*unsigned int _page, bool redrawBackgroundWhenAppendPage = TRUE*/ );
    void removePage( int num );

    ProcessingType processingType() { return m_processingType;  }

    QCursor getMouseCursor( const QPoint& nPoint, bool controlPressed );
    QPtrList<KWFrame> getSelectedFrames();
    KWFrame *getFirstSelectedFrame();
    int frameSetNum( KWFrameSet* fs ) { return frames.findRef( fs ); }

    void updateAllFrames();

    // The grid is in _pt_ now
    double gridX() { return m_gridX; }
    double gridY() { return m_gridY; }
    void setGridX(double _gridx);
    void setGridY(double _gridy) { m_gridY = _gridy; }

    // Currently unused. Not sure we want to go that way, now that we have
    // paragLayoutChanged and formatChanged in applyStyleChange.
    //int applyStyleChangeMask() { return styleMask; }
    //void setApplyStyleChangeMask( int _f ) { styleMask = _f; }

    // paragLayoutChanged is a set of flags for the parag layout - see the enum in KWParagLayout
    // formatChanged is a set of flags from QTextFormat
    // If both are -1, it means the style has been deleted.
    void applyStyleChange( KWStyle * changedStyle, int paragLayoutChanged, int formatChanged );
    void updateAllStyleLists();

    bool isHeaderVisible() const { return m_headerVisible; }
    bool isFooterVisible() const { return m_footerVisible; }
    void setHeaderVisible( bool h );
    void setFooterVisible( bool f );

    void recalcFrames();

    KoHFType getHeaderType() { return m_pageHeaderFooter.header; }
    KoHFType getFooterType() { return m_pageHeaderFooter.footer; }

    void frameMargins( double &l, double &r, double &t, double &b );
    bool isOnlyOneFrameSelected();
    void setFrameMargins( double l, double r, double t, double b );
    void setFrameCoords( double x, double y, double w, double h );

    // The user-chosen global unit
    QString getUnitName() { return KoUnit::unitName( m_unit ); }
    KoUnit::Unit getUnit() { return m_unit; }
    void setUnit( KoUnit::Unit _unit );

    void addCommand( KCommand * cmd );

//    KWFootNoteManager &getFootNoteManager() { return footNoteManager; }
//    void setNoteType( KWFootNoteManager::NoteType nt ) { footNoteManager.setNoteType( nt ); }
//    KWFootNoteManager::NoteType getNoteType() const { return footNoteManager.getNoteType(); }

    KoAutoFormat * getAutoFormat() { return m_autoFormat; }

    // This is used upon loading, to delay certain things until completeLoading.
    // For KWTextImage
    void addImageRequest( const KoImageKey &key, KWTextImage *img );
    // For KWPictureFrameSet
    void addImageRequest( KWPictureFrameSet *fs );
    // For KWClipartFrameSet
    void addClipartRequest( KWClipartFrameSet *fs );
    // For KWTextParag
    void addAnchorRequest( const QString &framesetName, const KWAnchorPosition &anchorPos );

    // This is used by loadFrameSets() and by KWCanvas to paste framesets
    KWFrameSet *loadFrameSet( QDomElement framesetElem, bool loadFrames = true );
    void loadEmbedded( QDomElement embedded );

    void recalcVariables( int type );
    
    KWVariableCollection *getVariableCollection() {return m_varColl;}

    KWSerialLetterDataBase *getSerialLetterDataBase() const { return m_slDataBase; }
    int getSerialLetterRecord() const;
    void setSerialLetterRecord( int r );

#if 0
    bool onLineSpellCheck() const { return m_onlineSpellCheck; }
    void setOnLineSpellCheck( bool b ) { m_onlineSpellCheck = b; }
#endif

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
    // useless method
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

    /**
     * get custom kspell config
     */
    void setKSpellConfig(KSpellConfig _kspell);
    KSpellConfig * getKSpellConfig() {return m_pKSpellConfig;}

    KWStyle* addStyleTemplate( KWStyle *style );

    void removeStyleTemplate ( KWStyle *style );

#ifndef NDEBUG
    void printDebug();
#endif

    /** calls layout() on all framesets  */
    void layout();

    /** calls invalidate() on all framesets  */
    void invalidate();

    /** call by undo/redo frame border => update all button border frame **/
    void refreshFrameBorderButton();

    // This settings has to be here [instead of KWView] because we need to
    // format paragraphs slightly differently (to add room for the CR char)
    bool viewFormattingChars() const { return m_viewFormattingChars; }
    void setViewFormattingChars(bool _b) { m_viewFormattingChars=_b; }

    // Also view properties, but stored, loaded and saved here (lacking a more global object).
    bool viewFrameBorders() const { return m_viewFrameBorders; }
    void setViewFrameBorders( bool b ) { m_viewFrameBorders = b; }
    void setShowRuler(bool _ruler){ m_bShowRuler=_ruler; }
    bool showRuler() const { return m_bShowRuler; }

    bool dontCheckUpperWord() const { return m_bDontCheckUpperWord; }
    void setDontCheckUpperWord(bool _b) { m_bDontCheckUpperWord=_b;}

    bool dontCheckTitleCase() const {return  m_bDontCheckTitleCase;}
    void setDontCheckTitleCase(bool _b) {m_bDontCheckTitleCase=_b;}

    bool showdocStruct() const {return  m_bShowDocStruct;}
    void setShowDocStruct(bool _b){m_bShowDocStruct=_b;}

    // in pt
    double getIndentValue() { return m_indent; }
    void setIndentValue(double _ind) { m_indent=_ind; }

    int getNbPagePerRow() { return m_iNbPagePerRow; }
    void setNbPagePerRow(int _nb) { m_iNbPagePerRow=_nb; }

    int maxRecentFiles() const { return m_maxRecentFiles; }


    void setChangeLastModeView(const QString &_mode){ m_lastModeView=_mode;}
    const QString & ChangeLastModeView() {return m_lastModeView;}

    int defaultColumnSpacing(){ return m_defaultColumnSpacing ;}
    void setDefaultColumnSpacing(int _val){m_defaultColumnSpacing=_val;}
    /**
     * @returns the document for the formulas
     */
    KFormula::Document* getFormulaDocument();

    void reorganizeGUI();
    //necessary to update resize handle when you change layout
    // make zoom, add header, add footer etc...
    void updateResizeHandles();

    // Tell all views to stop editing this frameset, if they were doing so
    void terminateEditing( KWFrameSet * frameSet )
    { emit sig_terminateEditing( frameSet ); }

    void clearUndoRedoInfos();

    void refreshDocStructure(FrameSetType);
    void refreshDocStructure(int);

    int typeItemDocStructure(FrameSetType _type);

    void renameButtonTOC(bool b);

    void refreshMenuExpression();

    void refreshMenuCustomVariable();

    void frameSelectedChanged();

    void updateZoomRuler();


    void hasTOC(bool _b){ m_hasTOC=_b;}

    bool isTOC(){return m_hasTOC;}

    void updateRulerFrameStartEnd();
    void updateFrameStatusBarItem();

    /** Convert a color into a color to be displayed for it
     * (when using color schemes, we still want to print black on white).
     * See also KoTextFormat::defaultTextColor. */
    static QColor resolveTextColor( const QColor & col, QPainter * painter );
    static QColor defaultTextColor( QPainter * painter );
    static QColor resolveBgColor( const QColor & col, QPainter * painter );
    static QColor defaultBgColor( QPainter * painter );


    virtual DCOPObject* dcopObject();


    int undoRedoLimit();
    void setUndoRedoLimit(int _val);

    void updateContentsSize(){emit newContentsSize();}

    void refreshGUIButton();

signals:
    void sig_insertObject( KWChild *_child, KWPartFrameSet* );
    void newContentsSize();
    void pageNumChanged();

    void docStructureChanged(int);
    void sig_terminateEditing( KWFrameSet * fs );

    void sig_refreshMenuCustomVariable();

    void sig_frameSelectedChanged();

public slots:
    void slotRepaintChanged( KWFrameSet * frameset );
    void slotRepaintAllViews() { repaintAllViews( false ); }
    void slotRepaintVariable();

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotDocumentInfoModifed();

protected:
    KoView* createViewInstance( QWidget* parent, const char* name );
    virtual bool saveChildren( KoStore *_store, const QString &_path );

    virtual bool completeLoading( KoStore* _store );
    virtual bool completeSaving( KoStore *_store );

    void loadFrameSets( QDomElement framesets );
    void loadStyleTemplates( QDomElement styles );
    void saveStyle( KWStyle *sty, QDomElement parentElem );

    void initConfig();
    void saveConfig();
    void initUnit();

    DCOPObject *dcop;

private:
    QPtrList<KWView> m_lstViews;
    QPtrList<KWChild> m_lstChildren;

    unsigned int m_itemsLoaded;
    unsigned int m_nrItemsToLoad;

    KoPageLayout m_pageLayout;
    KoColumns m_pageColumns;
    KoKWHeaderFooter m_pageHeaderFooter;
    unsigned int m_ptColumnWidth;

    KWImageCollection m_imageCollection;
    KoClipartCollection m_clipartCollection;
    QPtrList<KWFrameSet> frames;
    QPtrList<KWStyle> m_styleList;
    QPtrList<KWStyle> m_deletedStyles;

    // Cached value for findStyle()
    KWStyle *m_lastStyle;

    int m_pages;

    ProcessingType m_processingType;
    double m_gridX, m_gridY;

    //int styleMask;

    KoUnit::Unit m_unit;

    KCommandHistory * m_commandHistory;
//    KWFootNoteManager footNoteManager;
    KoAutoFormat * m_autoFormat;

    QString urlIntern;

    QMap<KoImageKey, QString> * m_pixmapMap;
    QMap<KoClipartKey, QString> * m_clipartMap;

    QMap<KoImageKey, KWTextImage *> m_imageRequests; // to be removed
    QPtrList<KWPictureFrameSet> m_imageRequests2;
    QPtrList<KWClipartFrameSet> m_clipartRequests;
    QMap<QString, KWAnchorPosition> m_anchorRequests;

    QMap<QString,QString> * m_pasteFramesetsMap;

    KoVariableFormatCollection *m_varFormatCollection;
    KWSerialLetterDataBase *m_slDataBase;
    int slRecordNum;

    // When a document is written out, the syntax version in use will be recorded. When read back
    // in, this variable reflects that value.
    int m_syntaxVersion;

    KSpellConfig *m_pKSpellConfig;

    QFont m_defaultFont;
    bool m_headerVisible, m_footerVisible;
    bool m_viewFormattingChars;
    bool m_viewFrameBorders;
    bool m_bShowRuler;
    bool m_bDontCheckUpperWord;
    bool m_bDontCheckTitleCase;
    bool m_bShowDocStruct;
    //bool m_onlineSpellCheck;

    // The document that is used by all formulas
    KFormula::Document* m_formulaDocument;

    double m_indent; // in pt

    int m_iNbPagePerRow;
    int m_maxRecentFiles;

    int m_defaultColumnSpacing;

    bool m_hasTOC;

    QString m_lastModeView;
    KWVariableCollection *m_varColl;
};


#endif
