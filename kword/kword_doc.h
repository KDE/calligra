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

#ifndef kword_doc_h
#define kword_doc_h

class KWordDocument;
class KWPage;
class QPrinter;
class KWCharImage;
class KWPictureFrameSet;
class KWSerialLetterDataBase;
class KWContents;

#include <koDocument.h>
#include <koDocumentChild.h>
#include <koPageLayoutDia.h>
#include <koQueryTrader.h>

#include <qtextstream.h>

#include "kword_view.h"
#include "fc.h"
#include "parag.h"
#include "paraglayout.h"
#include "formatcollection.h"
#include "imagecollection.h"
#include "kword_frame.h"
#include "variable.h"
#include "footnote.h"
#include "autoformat.h"
#include "kword_undo.h"

#include <qlist.h>
#include <qpainter.h>
#include <qstring.h>
#include <qstrlist.h>
#include <qintdict.h>
#include <qstringlist.h>
#include <qrect.h>
#include <qdict.h>

/******************************************************************/
/* Class: KWordChild                                              */
/******************************************************************/

class KWordChild : public KoDocumentChild
{
public:
    KWordChild( KWordDocument *_wdoc, const QRect& _rect, KoDocument *_doc, int diffx, int diffy );
    KWordChild( KWordDocument *_wdoc );
    ~KWordChild();

    KWordDocument* parent()
    { return m_pKWordDoc; }

    virtual KoDocument *hitTest( const QPoint &, const QWMatrix & );

    bool load( KOMLParser& parser, QValueList<KOMLAttrib>& _attribs );
    bool save( QTextStream & out );

protected:
    KWordDocument *m_pKWordDoc;

};

/******************************************************************/
/* Class: KWordDocument                                           */
/******************************************************************/

class KWordDocument : public KoDocument
{
    Q_OBJECT

public:
    KWordDocument( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KWordDocument();

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
    virtual bool saveToStream( QIODevice * dev );
    virtual bool completeSaving( KoStore *_store );

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE );

    virtual QStrList outputFormats();
    virtual QStrList inputFormats();

    virtual void addView( KoView *_view );
    virtual void removeView( KoView *_view );

    virtual void insertObject( const QRect& _rect, KoDocumentEntry& _e, int diffx, int diffy );
    void setPageLayout( KoPageLayout _layout, KoColumns _cl, KoKWHeaderFooter _hf );

    void getPageLayout( KoPageLayout& _layout, KoColumns& _cl, KoKWHeaderFooter& _hf );

    KWFrameSet *getFrameSet( unsigned int _num )
    { return frames.at( _num ); }
    unsigned int getNumFrameSets()
    { return frames.count(); }
    void addFrameSet( KWFrameSet *f )
    { frames.append(f); /*updateAllFrames();*/ setModified( true ); /*updateAllViews(0L);*/ }
    void delFrameSet( KWFrameSet *f, bool deleteit = true );

    KWParag *getFirstParag( unsigned int _num ) {
        if ( frames.at( _num )->getFrameType() == FT_TEXT )
            return dynamic_cast<KWTextFrameSet*>( frames.at( _num ) )->getFirstParag();
        else
            return 0L;
    }

    KWUserFont* getDefaultUserFont();
    KWParagLayout *getDefaultParagLayout()
    { return defaultParagLayout; }

    QList<KWUserFont> userFontList;

    QList<KWDisplayFont> displayFontList;

    QList<KWParagLayout> paragLayoutList;

    KWUserFont* findUserFont( QString _fontname );

    KWDisplayFont* findDisplayFont( KWUserFont* _font, unsigned int _size, int _weight, bool _italic, bool _underline );

    KWParagLayout* findParagLayout( QString _name );

    KWParag* findFirstParagOfPage( unsigned int _page, unsigned int _frameset );
    KWParag* findFirstParagOfRect( unsigned int _ypos, unsigned int _page, unsigned int _frameset );

    unsigned int getPTTopBorder() { return static_cast<unsigned int>(zoomIt( pageLayout.ptTop )); }
    unsigned int getPTBottomBorder() { return static_cast<unsigned int>(zoomIt( pageLayout.ptBottom )); }
    unsigned int getPTLeftBorder() { return static_cast<unsigned int>(zoomIt( pageLayout.ptLeft )); }
    unsigned int getPTRightBorder() { return static_cast<unsigned int>(zoomIt( pageLayout.ptRight )); }
    unsigned int getPTPaperHeight() { return static_cast<unsigned int>(zoomIt( pageLayout.ptHeight )); }
    unsigned int getPTPaperWidth() { return static_cast<unsigned int>(zoomIt( pageLayout.ptWidth )); }
    unsigned int getPTColumnWidth() { return static_cast<unsigned int>(zoomIt( ptColumnWidth )); }
    unsigned int getPTColumnSpacing() { return static_cast<unsigned int>(zoomIt( pageColumns.ptColumnSpacing )); }
    double getMMPaperHeight() { return zoomIt( pageLayout.mmHeight ); }
    double getINCHPaperHeight() { return zoomIt( pageLayout.inchHeight ); }

    unsigned int getColumns() { return pageColumns.columns; }

    void print() {}

    bool isPTYInFrame( unsigned int _frameSet, unsigned int _frame, unsigned int _ypos );

    bool printLine( KWFormatContext &_fc, QPainter &_painter, int xOffset, int yOffset, int _w, int _h,
                    bool _viewFormattingChars = false, bool _drawVarBack = true, int dbx = -1, int dby = -1,
                    int dbw = -1, int dbh = -1, const QBrush &dbback = Qt::NoBrush );
    void printBorders( QPainter &_painter, int xOffset, int yOffset, int _w, int _h );

    void drawMarker( KWFormatContext &_fc, QPainter *_painter, int xOffset, int yOffset );

    void updateAllViews( KWordView *_view, bool _clear = false );
    void updateAllViewportSizes();
    void updateAllSelections();
    void setUnitToAll();
    void updateAllCursors();
    void drawAllBorders( bool back = true);
    void recalcWholeText( bool _cursor = false, bool completeRender=false);
    void recalcWholeText( KWParag *start, unsigned int fs );

    int getPages() { return pages; }

    void setPages( int _pages )
    { pages = _pages;  }
    KWFormatCollection *getFormatCollection()
    { return &formatCollection; }

    KWImageCollection *getImageCollection()
    { return &imageCollection; }

    void insertPicture( QString _filename, KWPage *_paperWidget );

    void setSelStart( KWFormatContext &_fc )
    { selStart = _fc; }
    void setSelEnd( KWFormatContext &_fc )
    { selEnd = _fc; }
    /**
     * getSetStart/getSelEnd are used when making a selection.
     * Be careful, getSelStart can be < or > than getSelEnd.
     * Don't use those to iterate over the selection, use @ref getSelTop
     * and @ref getSetBottom.
     */
    KWFormatContext *getSelStart()
    { return &selStart;  }
    KWFormatContext *getSelEnd()
    { return &selEnd; }

    /**
     * getSelTop returns the format context of the top of the selection,
     * guaranteed to be <= getSelBottom. Use this e.g. to iterate over the
     * paragraphs in the selection.
     */
    KWFormatContext *getSelTop()
    { return selStart < selEnd ? &selStart : &selEnd; }

    /**
     * getSelBottom returns the format context of the bottom of the selection,
     * guaranteed to be >= getSelTop. Use this e.g. to iterate over the
     * paragraphs in the selection.
     */
    KWFormatContext *getSelBottom()
    { return selStart < selEnd ? &selEnd : &selStart; }

    void drawSelection( QPainter &_painter, int xOffset, int yOffset,
                        KWFormatContext *_selStart = 0L, KWFormatContext *_selEnd = 0L );
    void setSelection( bool _has )
    { hasSelection = _has; }

    bool has_selection() { return hasSelection; }
    QList <KWordView> getAllViews();

    void deleteSelectedText( KWFormatContext *_fc );
    void copySelectedText();
    void setFormat( KWFormat &_format, int flags = KWFormat::All );

    void paste( KWFormatContext *_fc, QString _string, KWPage *_page,
                KWFormat *_format = 0L, const QString &_mime = "text/plain" );

    void appendPage( unsigned int _page, bool redrawBackgroundWhenAppendPage = TRUE );

    ProcessingType getProcessingType()
    { return processingType;  }

    int getFrameSet( unsigned int mx, unsigned int my );
    int selectFrame( unsigned int mx, unsigned int my, bool simulate = false );
    void deSelectFrame( unsigned int mx, unsigned int my );
    void deSelectAllFrames();
    QCursor getMouseCursor( unsigned int mx, unsigned int my );
    QList<KWFrame> getSelectedFrames();
    KWFrame *getFirstSelectedFrame();
    KWFrame *getFirstSelectedFrame( int &_frameset );
    int getFrameSetNum( KWFrameSet* fs ) { return frames.findRef( fs ); }
    KWFrameSet *getFirstSelectedFrameSet();

    void print( QPainter *painter, QPrinter *printer, float left_margin, float top_margin );

    void updateAllFrames();

    int getRastX() { return rastX; }
    int getRastY() { return rastY; }

    int getApplyStyleTemplate() { return applyStyleTemplate; }
    void setApplyStyleTemplate( int _f ) { applyStyleTemplate = _f; }

    void updateAllStyles();
    void updateAllStyleLists();

    void setStyleChanged( QString _name );
    bool isStyleChanged( QString _name );

    bool hasHeader() { return _header; }
    bool hasFooter() { return _footer; }
    void setHeader( bool h );
    void setFooter( bool f );

    void recalcFrames( bool _cursor = false );

    KoHFType getHeaderType() { return pageHeaderFooter.header; }
    KoHFType getFooterType() { return pageHeaderFooter.footer; }

    bool canResize( KWFrameSet *frameset, KWFrame *frame, int page, int diff );

    bool needRedraw() { return _needRedraw; }
    void setNeedRedraw( bool _r ) { _needRedraw = _r; }

    void addGroupManager( KWGroupManager *gm ) { grpMgrs.append( gm ); }
    unsigned int getNumGroupManagers() { return grpMgrs.count(); }
    KWGroupManager *getGroupManager( int i ) { return grpMgrs.at( i ); }
    void delGroupManager( KWGroupManager *g, bool deleteit = true );

    QPen setBorderPen( KWParagLayout::Border _brd );
    void enableEmbeddedParts( bool f );

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

    int &getNumParags() { return numParags; }

    void saveParagInUndoBuffer( QList<KWParag> parags, int frameset, KWFormatContext *_fc );
    void saveParagInUndoBuffer( KWParag *parag, int frameset, KWFormatContext *_fc );

    void undo();
    void redo();

    void updateTableHeaders( QList<KWGroupManager> &grpMgrs );

    QIntDict<KWVariableFormat> &getVarFormats() { return varFormats; }

    long int getPageNum( int bottom );

    KWFootNoteManager &getFootNoteManager() { return footNoteManager; }
    void setNoteType( KWFootNoteManager::NoteType nt ) { footNoteManager.setNoteType( nt ); }
    KWFootNoteManager::NoteType getNoteType() const { return footNoteManager.getNoteType(); }

    KWAutoFormat &getAutoFormat() { return autoFormat; }

    void setPageLayoutChanged( bool c ) { pglChanged = c; }

    bool getPageLayoutChanged() const { return pglChanged; }

    void addImageRequest( const QString &filename, KWCharImage *img );
    void addImageRequest( const QString &filename, KWPictureFrameSet *fs );

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

    bool onLineSpellCheck() const {
        return spellCheck;
    }
    void setOnLineSpellCheck( bool b ) {
        spellCheck = b;
    }

    void createContents();
    KWContents *getContents() {
        return contents;
    }

    void checkNumberOfPages( KWFormatContext *fc );
    bool canRemovePage( int num, KWFrame *f );

    void setZoom( int z ) { zoom = z; }
    int getZoom() const { return zoom; }

    int zoomIt( int z ) const;
    unsigned int zoomIt( unsigned int z ) const;
    double zoomIt( double z ) const;

    int getSyntaxVersion( ) const { return syntaxVersion; };

    void updateFrameSizes( int oldZoom );

    bool getViewFormattingChars() {return _viewFormattingChars;}
    void setViewFormattingChars(bool _b) {_viewFormattingChars=_b;}

    bool getViewFrameBorders() {return  _viewFrameBorders;}
    void setViewFrameBorders(bool _b) { _viewFrameBorders=_b;}

    bool getViewTableGrid() {return _viewTableGrid;}
    void setViewTableGrid(bool _b) {_viewTableGrid=_b;}

    void moveDownStyleTemplate ( const QString & _styleName );
    void moveUpStyleTemplate ( const QString & _styleName );


signals:
    void sig_imageModified();
    void sig_insertObject( KWordChild *_child, KWPartFrameSet* );
    void sig_updateChildGeometry( KWordChild *_child ); // unused
    void sig_removeObject( KWordChild *_child );

protected slots:
    void slotUndoRedoChanged( QString, QString );

    void slotDocumentLoaded();

    void slotChildChanged( KoDocumentChild * child );

protected:
    KoView* createViewInstance( QWidget* parent, const char* name );
    virtual bool saveChildren( KoStore *_store, const QString &_path );

    virtual bool completeLoading( KoStore* /* _store */ );

    virtual void draw( QPaintDevice*, long int _width, long int _height,
                       float _scale );

    void loadFrameSets( KOMLParser&, QValueList<KOMLAttrib>& );
    void loadStyleTemplates( KOMLParser&, QValueList<KOMLAttrib>& );

    void addStyleTemplate( KWParagLayout *pl );

    QList<KWordView> m_lstViews;
    QList<KWordChild> m_lstChildren;

    KoPageLayout pageLayout;
    KoColumns pageColumns;
    KoKWHeaderFooter pageHeaderFooter;

    unsigned int ptColumnWidth;

    KWUserFont *cUserFont;
    KWUserFont *defaultUserFont;
    KWFormatCollection formatCollection;
    KWImageCollection imageCollection;
    QList<KWFrameSet> frames;
    QList<KWGroupManager> grpMgrs;

    KWParagLayout *defaultParagLayout;

    int pages;

    KWFormatContext selStart, selEnd;
    bool hasSelection;

    ProcessingType processingType;
    int rastX, rastY;

    int applyStyleTemplate;

    QStringList changedStyles;

    bool _loaded;

    QPixmap ret_pix;

    bool _header, _footer;

    KWDisplayFont *cDisplayFont;
    KWParagLayout *cParagLayout;

    bool _needRedraw;

    QString unit;
    int numParags;

    KWCommandHistory history;
    QIntDict<KWVariableFormat> varFormats;
    KWFootNoteManager footNoteManager;
    KWAutoFormat autoFormat;

    QString urlIntern;
    bool pglChanged;

    QStringList pixmapKeys, pixmapNames;
    QDict<KWCharImage> imageRequests;
    QDict<KWPictureFrameSet> imageRequests2;
    QList<KWVariable> variables;
    QMap< QString, QString > varValues;
    KWSerialLetterDataBase *slDataBase;
    int slRecordNum;

    bool spellCheck;
    KWContents *contents;

    KoMainWindow *tmpShell;
    QRect tmpShellSize;

    int zoom;

    // When a document is written out, the syntax version in use will be recorded. When read back
    // in, this variable reflects that value.
    int syntaxVersion;

    bool _viewFormattingChars, _viewFrameBorders, _viewTableGrid;

};

inline int KWordDocument::zoomIt( int z ) const
{
    if ( zoom == 100 )
        return z;
    return ( zoom * z ) / 100;
}

inline unsigned int KWordDocument::zoomIt( unsigned int z ) const
{
    if ( zoom == 100 )
        return z;
    return ( zoom * z ) / 100;
}

inline double KWordDocument::zoomIt( double z ) const
{
    if ( zoom == 100 )
        return z;
    return ( (double)zoom * z ) / 100.0;
}

#endif
