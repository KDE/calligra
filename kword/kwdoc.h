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
class KWSerialLetterDataBase;
class KWContents;
class KWFrameSet;
class KWTableFrameSet;
class KWPartFrameSet;
class KWStyle;
class KWFrame;
class KWView;
class KWViewMode;
class KoDocumentEntry;
class QPainter;
class KSpellConfig;
class KWAutoFormat;
class KCommand;
class KCommandHistory;
class KWVariable;
class KWVariableFormat;

class QFont;
class QStringList;
class QRect;

namespace KFormula {
    class KFormulaDocument;
}


#include <koDocument.h>
#include <koGlobal.h>
#include <koRect.h>
#include <koDocumentChild.h>

#include "kwimage.h"
#include "kwanchorpos.h"
#include "kwunit.h"
#include "defs.h"

#include <qmap.h>
#include <qlist.h>

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

class KWDocument : public KoDocument
{
    Q_OBJECT

public:
    KWDocument( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KWDocument();

    enum ProcessingType {WP = 0, DTP = 1};
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

    static const int CURRENT_SYNTAX_VERSION = 1;

public:
    virtual bool initDoc();
    void initEmpty();

    virtual bool loadXML( QIODevice *, const QDomDocument & dom );
    virtual bool loadChildren( KoStore *_store );
    virtual QDomDocument saveXML();
    void processImageRequests();

    //int getSyntaxVersion( ) const { return syntaxVersion; }

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
    void createEmptyRegion( QRegion & emptyRegion, KWViewMode * viewMode );
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
    KWFrameSet *getFrameSet( unsigned int _num )
    { return frames.at( _num ); }

    // Return the frameset with a given name
    KWFrameSet * getFrameSetByName( const QString & name );

    // Return the frameset that intersects position mx, my (in pt)
    KWFrameSet * getFrameSet( double mx, double my );

    // Return the total number of framesets
    unsigned int getNumFrameSets()
    { return frames.count(); }

    // Generate a new name for a frameset. @p templateName must contain a %1 [for a number].
    QString generateFramesetName( const QString & templateName );

    // Prefer this over getFrameSet(i), if iterating over all of them
    QListIterator<KWFrameSet> framesetsIterator() const { return QListIterator<KWFrameSet>(frames); }
    void addFrameSet( KWFrameSet *f, bool finalize = true );
    void delFrameSet( KWFrameSet *f, bool deleteit = true );

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
    void framesChanged( const QList<KWFrame> & frames, KWView * view = 0L );

    const QList<KWStyle> & styleList() const { return m_styleList; }
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
    //void setPages( int _pages ) { m_pages = _pages;  }

    KWImageCollection *imageCollection() { return &m_imageCollection; }

    QList <KWView> getAllViews() { return m_lstViews; }

    void appendPage( /*unsigned int _page, bool redrawBackgroundWhenAppendPage = TRUE*/ );
    void removePage( int num );

    ProcessingType processingType() { return m_processingType;  }

    int selectFrame( double mx, double my, bool simulate = false );
    void deSelectFrame( double mx, double my );
    void deSelectAllFrames();
    QCursor getMouseCursor( double mx, double my );
    QList<KWFrame> getSelectedFrames();
    KWFrame *getFirstSelectedFrame();
    int getFrameSetNum( KWFrameSet* fs ) { return frames.findRef( fs ); }
    KWFrameSet *getFirstSelectedFrameSet();

    void updateAllFrames();

    int gridX() { return m_gridX; }
    int gridY() { return m_gridY; }
    void setGridX(int _gridx) { m_gridX = _gridx; }
    void setGridY(int _gridy) { m_gridY = _gridy; }

    // Currently unused. Not sure we want to go that way, now that we have
    // paragLayoutChanged and formatChanged in applyStyleChange.
    int applyStyleChangeMask() { return styleMask; }
    void setApplyStyleChangeMask( int _f ) { styleMask = _f; }

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

    void getFrameMargins( double &l, double &r, double &t, double &b );
    bool isOnlyOneFrameSelected();
    void setFrameMargins( double l, double r, double t, double b );
    void setFrameCoords( double x, double y, double w, double h );

    // The user-chosen global unit
    QString getUnitName() { return KWUnit::unitName( m_unit ); }
    KWUnit::Unit getUnit() { return m_unit; }
    void setUnit( KWUnit::Unit _unit );

    void addCommand( KCommand * cmd );

//    KWFootNoteManager &getFootNoteManager() { return footNoteManager; }
//    void setNoteType( KWFootNoteManager::NoteType nt ) { footNoteManager.setNoteType( nt ); }
//    KWFootNoteManager::NoteType getNoteType() const { return footNoteManager.getNoteType(); }

    KWAutoFormat * getAutoFormat() { return m_autoFormat; }

    // This is used upon loading, to delay certain things until completeLoading.
    // For KWTextImage
    void addImageRequest( const QString &filename, KWTextImage *img );
    // For KWPictureFrameSet
    void addImageRequest( const QString &filename, KWPictureFrameSet *fs );
    // For KWTextParag
    void addAnchorRequest( const QString &framesetName, const KWAnchorPosition &anchorPos );

    /**
     * Find or create a format for this type of variable
     * (date, time etc.)
     */
    KWVariableFormat * variableFormat( int type );
    /**
     * A custom variable (i.e. value set by the user)
     * was inserted into the text -> register it
     */
    void registerVariable( KWVariable *var );
    /**
     * A variable was deleted -> unregister it
     */
    void unregisterVariable( KWVariable *var );
    /**
     * Recalculate all variables of a given type
     */
    void recalcVariables( int type );
    /**
     * Returns the list of all variables. Used by the dialog.
     */
    const QList<KWVariable>& getVariables() const {
        return variables;
    }

    // For custom variables
    void setVariableValue( const QString &name, const QString &value );
    QString getVariableValue( const QString &name ) const;

    KWSerialLetterDataBase *getSerialLetterDataBase() const;
    int getSerialLetterRecord() const;
    void setSerialLetterRecord( int r );

    bool onLineSpellCheck() const {
        return spellCheck;
    }
    void setOnLineSpellCheck( bool b ) {
        spellCheck = b;
    }

    bool canRemovePage( int num );

    /**
     * @return the conversion factor between pt and pixel, that
     * takes care of the zoom and the DPI setting.
     * Use zoomIt(pt) instead, though.
     */
    double zoomedResolutionX() const { return m_zoomedResolutionX; }
    double zoomedResolutionY() const { return m_zoomedResolutionY; }

    /**
     * Change the zoom factor to @p z (e.g. 150 for 150%)
     * and/or change the resolution, given in DPI.
     * This is done on startup and when printing.
     * The same call combines both so that all the updating done behind
     * the scenes is done only once, even if both zoom and DPI must be changed.
     */
    void setZoomAndResolution( int zoom, int dpiX, int dpiY, bool updateViews );

    int zoom() const { return m_zoom; }

    // Input: pt. Output: pixels. Resolution and zoom are applied.
    int zoomItX( double z ) const {
        return qRound( m_zoomedResolutionX * z );
    }
    int zoomItY( double z ) const {
        return qRound( m_zoomedResolutionY * z );
    }

    QPoint zoomPoint( const KoPoint & p ) const {
        return QPoint( zoomItX( p.x() ), zoomItY( p.y() ) );
    }
    QRect zoomRect( const KoRect & r ) const {
        return QRect( zoomItX( r.x() ), zoomItY( r.y() ),
                      zoomItX( r.width() ), zoomItY( r.height() ) );
    }

    // Input: pixels. Output: pt.
    double unzoomItX( int x ) const {
        return static_cast<double>( x ) / m_zoomedResolutionX;
    }
    double unzoomItY( int y ) const {
        return static_cast<double>( y ) / m_zoomedResolutionY;
    }
    KoPoint unzoomPoint( const QPoint & p ) const {
        return KoPoint( unzoomItX( p.x() ), unzoomItY( p.y() ) );
    }

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
     * Insert/update TOC at beginning of document
     */
    void createContents();

    /**
     * get custom kspell config
     */
    void setKSpellConfig(KSpellConfig _kspell);
    KSpellConfig * getKSpellConfig() {return m_pKSpellConfig;}

    void addStyleTemplate( KWStyle *style );

    void removeStyleTemplate ( KWStyle *style );

    void moveDownStyleTemplate ( const QString & _styleName );

    void moveUpStyleTemplate ( const QString & _styleName );

#ifndef NDEBUG
    void printDebug();
#endif

    /** calls layout() on all framesets  */
    void layout();

    /** call by undo/redo frame border => update all button border frame **/
    void refreshFrameBorderButton();

    bool getViewFormattingChars() { return _viewFormattingChars; }
    void setViewFormattingChars(bool _b) {_viewFormattingChars=_b;}
    bool getViewFrameBorders() { return _viewFrameBorders; }
    void setViewFrameBorders(bool _b){_viewFrameBorders=_b;}
    bool getViewTableGrid() { return _viewTableGrid; }
    void setViewTableGrid(bool _b) { _viewTableGrid=_b;}

    // in pt
    double getIndentValue() { return m_indent; }
    void setIndentValue(double _ind) { m_indent=_ind; }

    int getNbPagePerRow() { return m_iNbPagePerRow; }
    void setNbPagePerRow(int _nb) { m_iNbPagePerRow=_nb; }

    /**
     * @returns the document for the formulas
     */
    KFormula::KFormulaDocument* getFormulaDocument();


    void setShowRuler(bool _ruler){m_bShowRuler=_ruler;}
    bool getShowRuler(){return m_bShowRuler;}

    void reorganizeGUI();
    //necessary to update resize handle when you change layout
    // make zoom, add header, add footer etc...
    void updateResizeHandles();

    // Tell all views to stop editing this frameset, if they were doing so
    void terminateEditing( KWFrameSet * frameSet )
    { emit sig_terminateEditing( frameSet ); }

    void refreshDocStructure(FrameType);

    // Convert a color into a color to be displayed for it
    // (when using color schemes, we still want to print black on white)
    static QColor resolveTextColor( const QColor & col, QPainter * painter );
    static QColor defaultTextColor( QPainter * painter );
    static QColor resolveBgColor( const QColor & col, QPainter * painter );
    static QColor defaultBgColor( QPainter * painter );

signals:
    void sig_insertObject( KWChild *_child, KWPartFrameSet* );
    void newContentsSize();
    void pageNumChanged();

    void docStructureChanged(TypeStructDocItem);
    void sig_terminateEditing( KWFrameSet * fs );

public slots:
    void slotRepaintChanged( KWFrameSet * frameset );

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

    void newZoomAndResolution( bool updateViews );

private:
    QList<KWView> m_lstViews;
    QList<KWChild> m_lstChildren;

    unsigned int m_itemsLoaded;
    unsigned int m_nrItemsToLoad;

    KoPageLayout m_pageLayout;
    KoColumns m_pageColumns;
    KoKWHeaderFooter m_pageHeaderFooter;
    unsigned int m_ptColumnWidth;

    KWImageCollection m_imageCollection;
    QList<KWFrameSet> frames;
    QList<KWStyle> m_styleList;
    QList<KWStyle> m_deletedStyles;

    // Cached value for findStyle()
    KWStyle *m_lastStyle;

    int m_pages;

    ProcessingType m_processingType;
    int m_gridX, m_gridY;

    int styleMask;

    bool m_headerVisible, m_footerVisible;

 //   KWDisplayFont *cDisplayFont;

    KWUnit::Unit m_unit;

    KCommandHistory * m_commandHistory;
//    KWFootNoteManager footNoteManager;
    KWAutoFormat * m_autoFormat;

    QString urlIntern;

    QStringList pixmapKeys, pixmapNames;

    QMap<QString, KWTextImage *> m_imageRequests;
    QMap<QString, KWPictureFrameSet *> m_imageRequests2;
    QMap<QString, KWAnchorPosition> m_anchorRequests;

    QMap<int, KWVariableFormat*> m_mapVariableFormats;
    QList<KWVariable> variables;
    QMap< QString, QString > varValues;
    KWSerialLetterDataBase *slDataBase;
    int slRecordNum;

    bool spellCheck;

    // Holds information about the table of contents
    KWContents *contents;

    int m_zoom;
    double m_resolutionX;
    double m_resolutionY;
    double m_zoomedResolutionX;
    double m_zoomedResolutionY;

    // When a document is written out, the syntax version in use will be recorded. When read back
    // in, this variable reflects that value.
    int syntaxVersion;

    KSpellConfig *m_pKSpellConfig;

    QFont m_defaultFont;
    bool _viewFormattingChars, _viewFrameBorders, _viewTableGrid;

    // The document that is used by all formulas
    KFormula::KFormulaDocument* m_formulaDocument;

    double m_indent; // in pt

    bool m_bShowRuler;

    int m_iNbPagePerRow;
};


#endif
