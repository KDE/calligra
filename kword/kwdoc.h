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
class QPrinter;
//class KWCharImage;
class KWPictureFrameSet;
class KWSerialLetterDataBase;
class KWContents;
class KWFrameSet;
class KWGroupManager;
class KWPartFrameSet;

class KSpellConfig;

#include <koDocument.h>
#include <koGlobal.h>
#include <koDocumentChild.h>
//#include <koPageLayoutDia.h>
#include <koQueryTrader.h>

#include <qtextstream.h>

#include "kwview.h"
#include "kwstyle.h"
#include "imagecollection.h"
#include "kwframe.h"
//#include "variable.h"
//#include "footnote.h"
#include "autoformat.h"
#include <kcommand.h>

#include <qlist.h>
#include <qpainter.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qintdict.h>
#include <qstringlist.h>
#include <qrect.h>
#include <qdict.h>

#include <qrichtext_p.h>

/******************************************************************/
/* Class: KWChild                                              */
/******************************************************************/

class KWChild : public KoDocumentChild
{
public:
    KWChild( KWDocument *_wdoc, const QRect& _rect, KoDocument *_doc, int diffx, int diffy );
    KWChild( KWDocument *_wdoc );
    ~KWChild();

    KWDocument* parent()
    { return m_pKWordDoc; }

    virtual KoDocument *hitTest( const QPoint &, const QWMatrix & );

    virtual bool load( const QDomElement &attribs );
    virtual QDomElement save( QDomDocument &doc );

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
    virtual bool completeSaving( KoStore *_store );

    // Called by KWFrame*'s loading code to emit correct progress info
    void progressItemLoaded();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual QStrList outputFormats();
    virtual QStrList inputFormats();

    virtual void addView( KoView *_view );
    virtual void removeView( KoView *_view );

    virtual void addShell( KoMainWindow *shell );

    virtual void insertObject( const QRect& _rect, KoDocumentEntry& _e, int diffx, int diffy );
    void setPageLayout( KoPageLayout _layout, KoColumns _cl, KoKWHeaderFooter _hf );

    void getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf );

    KWFrameSet *getFrameSet( unsigned int _num )
    { return frames.at( _num ); }
    KWFrameSet * getFrameSet( unsigned int mx, unsigned int my );
    unsigned int getNumFrameSets()
    { return frames.count(); }
    // Prefer this over getFrameSet(i), if iterating over all of them
    QListIterator<KWFrameSet> framesetsIterator() const { return QListIterator<KWFrameSet>(frames); }
    void addFrameSet( KWFrameSet *f );
    void delFrameSet( KWFrameSet *f, bool deleteit = true );

    unsigned int ptTopBorder() const { return static_cast<unsigned int>(zoomIt( pageLayout.ptTop )); }
    unsigned int ptBottomBorder() const { return static_cast<unsigned int>(zoomIt( pageLayout.ptBottom )); }
    unsigned int ptLeftBorder() const { return static_cast<unsigned int>(zoomIt( pageLayout.ptLeft )); }
    unsigned int ptRightBorder() const { return static_cast<unsigned int>(zoomIt( pageLayout.ptRight )); }
    unsigned int ptPaperHeight() const { return static_cast<unsigned int>(zoomIt( pageLayout.ptHeight )); }
    unsigned int ptPaperWidth() const { return static_cast<unsigned int>(zoomIt( pageLayout.ptWidth )); }
    unsigned int ptColumnWidth() const;
    unsigned int ptColumnSpacing() const { return static_cast<unsigned int>(zoomIt( pageColumns.ptColumnSpacing )); }

    unsigned int getColumns() const { return pageColumns.columns; }

    void print() {}

    bool isPTYInFrame( unsigned int _frameSet, unsigned int _frame, unsigned int _ypos );
    int getPageOfRect( QRect & _rect ) const;

    void printBorders( QPainter &_painter, int xOffset, int yOffset, int _w, int _h );

    void updateAllViews( KWView *_view, bool _erase = false );
    void updateAllViewportSizes();
    void updateAllSelections();
    void setUnitToAll();
    void drawAllBorders( bool back = true);
    void refreshAllFrames();

    const QList<KWStyle> & styleList() const { return m_styleList; }
    /**
     * Look for a style named @p name. If not found, it will
     * either return 0L (if noFallback is true) or (by default) it will
     * return the default style (first one in the list of styles).
     */
    KWStyle* findStyle( const QString & name, bool noFallback = false );
    /**
     * Return style number @p i.
     */
    KWStyle* styleAt( int i ) { return m_styleList.at(i); }

    int getPages() const { return pages; }
    //void setPages( int _pages ) { pages = _pages;  }

    KWImageCollection *getImageCollection() { return &imageCollection; }

    bool selection();
    QList <KWView> getAllViews() { return m_lstViews; }

    void appendPage( /*unsigned int _page, bool redrawBackgroundWhenAppendPage = TRUE*/ );
    void removePage( int num );

    ProcessingType processingType() { return m_processingType;  }

    int selectFrame( unsigned int mx, unsigned int my, bool simulate = false );
    void deSelectFrame( unsigned int mx, unsigned int my );
    void deSelectAllFrames();
    QCursor getMouseCursor( unsigned int mx, unsigned int my );
    QList<KWFrame> getSelectedFrames();
    KWFrame *getFirstSelectedFrame();
    int getFrameSetNum( KWFrameSet* fs ) { return frames.findRef( fs ); }
    KWFrameSet *getFirstSelectedFrameSet();

    void print( QPainter *painter, QPrinter *printer, float left_margin, float top_margin );

    void updateAllFrames();

    int gridX() { return m_gridX; }
    int gridY() { return m_gridY; }
    void setGridX(int _gridx) {m_gridX=_gridx;}
    void setGridY(int _gridy) {m_gridY=_gridy;}

    int getApplyStyleTemplate() { return applyStyleTemplate; }
    void setApplyStyleTemplate( int _f ) { applyStyleTemplate = _f; }

    void applyStyleChange( const QString & changedStyle );
    void updateAllStyleLists();

    bool isHeaderVisible() const { return m_headerVisible; }
    bool isFooterVisible() const { return m_footerVisible; }
    void setHeaderVisible( bool h );
    void setFooterVisible( bool f );

    void recalcFrames( bool _cursor = false );

    KoHFType getHeaderType() { return pageHeaderFooter.header; }
    KoHFType getFooterType() { return pageHeaderFooter.footer; }

    bool canResize( KWFrameSet *frameset, KWFrame *frame, int page, int diff );

    void addGroupManager( KWGroupManager *gm ) { grpMgrs.append( gm ); }
    unsigned int getNumGroupManagers() { return grpMgrs.count(); }
    KWGroupManager *getGroupManager( int i ) { return grpMgrs.at( i ); }
    void delGroupManager( KWGroupManager *g, bool deleteit = true );

    //void enableEmbeddedParts( bool f );

    RunAround getRunAround();
    KWUnit getRunAroundGap();

    void setRunAround( RunAround _ra );
    void setRunAroundGap( KWUnit _gap );

    void getFrameMargins( KWUnit &l, KWUnit &r, KWUnit &t, KWUnit &b );
    bool isOnlyOneFrameSelected();
    KWFrameSet *getFrameCoords( unsigned int &x, unsigned int &y, unsigned int &w, unsigned int &h, unsigned int &num );

    void setFrameMargins( KWUnit l, KWUnit r, KWUnit t, KWUnit b );
    void setFrameCoords( unsigned int x, unsigned int y, unsigned int w, unsigned int h );

    QString getUnit() { return unit; }
    void setUnit( QString _unit ) { unit = _unit; }

    void addCommand( KCommand * cmd );

    void updateTableHeaders( QList<KWGroupManager> &grpMgrs );

    //QIntDict<KWVariableFormat> &getVarFormats() { return varFormats; }

    long int getPageNum( int bottom );

//    KWFootNoteManager &getFootNoteManager() { return footNoteManager; }
//    void setNoteType( KWFootNoteManager::NoteType nt ) { footNoteManager.setNoteType( nt ); }
//    KWFootNoteManager::NoteType getNoteType() const { return footNoteManager.getNoteType(); }

    KWAutoFormat &getAutoFormat() { return autoFormat; }

    void setPageLayoutChanged( bool c ) { pglChanged = c; }

    bool getPageLayoutChanged() const { return pglChanged; }

    //void addImageRequest( const QString &filename, KWCharImage *img );
    void addImageRequest( const QString &filename, KWPictureFrameSet *fs );

#if 0
    void registerVariable( KWVariable *var );
    void unregisterVariable( KWVariable *var );
    QList<KWVariable> *getVariables() {
        return &variables;
    }

    void setVariableValue( const QString &name, const QString &value );
    QString getVariableValue( const QString &name ) const;

    KWSerialLetterDataBase *getSerialLetterDataBase() const;
    int getSerialLetterRecord() const;
    void setSerialLetterRecord( int r );
#endif

    bool onLineSpellCheck() const {
        return spellCheck;
    }
    void setOnLineSpellCheck( bool b ) {
        spellCheck = b;
    }

    void createContents();
    //KWContents *getContents() {
    //    return contents;
    //}

    bool canRemovePage( int num, KWFrame *f );

    void setZoom( int z ) { zoom = z; }
    int getZoom() const { return zoom; }

    int zoomIt( int z ) const;
    unsigned int zoomIt( unsigned int z ) const;
    double zoomIt( double z ) const;

    int getSyntaxVersion( ) const { return syntaxVersion; };

    void updateFrameSizes( int oldZoom );

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

    void addStyleTemplate( KWStyle *style );

    void removeStyleTemplate ( const QString & _styleName);

    void moveDownStyleTemplate ( const QString & _styleName);

    void moveUpStyleTemplate ( const QString & _styleName);

    void printDebug();

    /** calls layout() on all framesets  */
    void layout();

    bool getViewFormattingChars() { return _viewFormattingChars; }
    void setViewFormattingChars(bool _b) {_viewFormattingChars=_b;}
    bool getViewFrameBorders() { return _viewFrameBorders; }
    void setViewFrameBorders(bool _b){_viewFrameBorders=_b;}
    bool getViewTableGrid() { return _viewTableGrid; }
    void setViewTableGrid(bool _b) { _viewTableGrid=_b;}


signals:
    void sig_imageModified();
    void sig_insertObject( KWChild *_child, KWPartFrameSet* );
    void sig_updateChildGeometry( KWChild *_child ); // unused
    void sig_removeObject( KWChild *_child );
    void sig_newContentsSize( int width, int height );

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotChildChanged( KoDocumentChild * child );

protected:
    KoView* createViewInstance( QWidget* parent, const char* name );
    virtual bool saveChildren( KoStore *_store, const QString &_path );

    virtual bool completeLoading( KoStore* /* _store */ );

    virtual void draw( QPaintDevice*, long int _width, long int _height,
                       float _scale );

    void loadFrameSets( QDomElement framesets );
    void loadStyleTemplates( QDomElement styles );

    //void addStyleTemplate( KWStyle *style );

private:
    QList<KWView> m_lstViews;
    QList<KWChild> m_lstChildren;

    unsigned int m_itemsLoaded;
    unsigned int m_nrItemsToLoad;

    KoPageLayout pageLayout;
    KoColumns pageColumns;
    KoKWHeaderFooter pageHeaderFooter;
    unsigned int m_ptColumnWidth;

    KWImageCollection imageCollection;
    QList<KWFrameSet> frames;
    QList<KWStyle> m_styleList;
    QList<KWGroupManager> grpMgrs;

    // Cached value for findStyle()
    KWStyle *m_lastStyle;

    int pages;

    ProcessingType m_processingType;
    int m_gridX, m_gridY;

    int applyStyleTemplate;

    bool m_headerVisible, m_footerVisible;

 //   KWDisplayFont *cDisplayFont;

    QString unit;

    KCommandHistory history;
 //   QIntDict<KWVariableFormat> varFormats;
//    KWFootNoteManager footNoteManager;
    KWAutoFormat autoFormat;

    QString urlIntern;
    bool pglChanged;

    QStringList pixmapKeys, pixmapNames;
//    QDict<KWCharImage> imageRequests;
    QDict<KWPictureFrameSet> imageRequests2;
 //   QList<KWVariable> variables;
    QMap< QString, QString > varValues;
    KWSerialLetterDataBase *slDataBase;
    int slRecordNum;

    bool spellCheck;

    // Holds information about the table of contents
    KWContents *contents;

    //KoMainWindow *tmpShell;
    //QRect tmpShellSize;

    int zoom;

    // When a document is written out, the syntax version in use will be recorded. When read back
    // in, this variable reflects that value.
    int syntaxVersion;

    KSpellConfig *m_pKSpellConfig;

    bool _viewFormattingChars, _viewFrameBorders, _viewTableGrid;
};

inline int KWDocument::zoomIt( int z ) const
{
    if ( zoom == 100 )
        return z;
    return ( zoom * z ) / 100;
}

inline unsigned int KWDocument::zoomIt( unsigned int z ) const
{
    if ( zoom == 100 )
        return z;
    return ( zoom * z ) / 100;
}

inline double KWDocument::zoomIt( double z ) const
{
    if ( zoom == 100 )
        return z;
    return ( (double)zoom * z ) / 100.0;
}


#endif
