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

class KPresenterDoc;
class KPresenterView;
class DCOPObject;
class KCommand;
class KMacroCommand;
class KCommandHistory;
class KoZoomHandler;
class KoAutoFormat;
class KoUnit;
class KoVariable;
class KoVariableFormatCollection;
class KPrVariableCollection;

#include "kprpage.h"

#include <koDocument.h>
#include <koDocumentChild.h>

#include <qptrlist.h>
#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>
#include <qdatetime.h>

#include <koPageLayoutDia.h>

#include <kpobject.h>
#include <global.h>
#include <kpbackground.h>

#include <kpimage.h>
#include <kpgradientcollection.h>
#include <kpclipartcollection.h>
#include <koUnit.h>
#include <kozoomhandler.h>
//#include "kprvariable.h"
class KoDocumentEntry;
class KPFooterHeaderEditor;
class KPTextObject;
class StyleDia;
class KPPartObject;

class KoStyle;
class KSpellConfig;

/******************************************************************/
/* class KPresenterChild                                          */
/******************************************************************/
class KPresenterChild : public KoDocumentChild
{
public:

    // constructor - destructor
    KPresenterChild( KPresenterDoc *_kpr, KoDocument* _doc, const QRect& _rect, int, int );
    KPresenterChild( KPresenterDoc *_kpr );
    ~KPresenterChild();

    // get parent
    KPresenterDoc* parent() { return (KPresenterDoc*)parent(); }

    virtual KoDocument *hitTest( const QPoint &, const QWMatrix & );
};

/*****************************************************************/
/* class KPresenterDoc                                           */
/*****************************************************************/
class KPresenterDoc : public KoDocument
{
    Q_OBJECT

public:

    // constructor - destructor
    KPresenterDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* doc = 0, const char* name = 0, bool singleViewMode = false );
    ~KPresenterDoc();

    // Drawing
    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

    // save
    virtual QDomDocument saveXML();
    virtual bool completeSaving( KoStore* _store );

    // load
    virtual bool loadXML( const QDomDocument& doc );
    virtual bool loadXML( QIODevice *, const QDomDocument & doc );
    virtual bool loadChildren( KoStore* _store );

    virtual bool initDoc() { return insertNewTemplate( 0, 0, true ); }
    void initEmpty();

    // change geometry of a child

    // page layout
    void setPageLayout( KoPageLayout, int, int );
    KoPageLayout pageLayout() const { return _pageLayout; }

    // insert a page
    unsigned int insertNewPage( int, int, bool _restore=true );
    bool insertNewTemplate( int, int, bool clean=false );

    const QPtrList<KPRPage> & getPageList() const {return m_pageList;}

    QPtrList<KPRPage> pageList() const { return m_pageList;}

    void insertObjectInPage(int offset, KPObject *_obj);

    // get - set raster
    unsigned int rastX() const { return _rastX; }
    unsigned int rastY() const { return _rastY; }

    void setRasters( unsigned int rx, unsigned int ry, bool _replace = true );

    // get - set options for editmodi
    QColor txtBackCol() const { return _txtBackCol; }
    void setTxtBackCol( QColor c ) { _otxtBackCol = _txtBackCol; _txtBackCol = c; }

    // get - set roundedness
    unsigned int getRndX() const { return _xRnd; }
    unsigned int getRndY() const { return _yRnd; }

    // get values for screenpresentations
    bool spInfinitLoop() const { return _spInfinitLoop; }
    bool spManualSwitch() const { return _spManualSwitch; }
    void setInfinitLoop( bool il ) { _spInfinitLoop = il; }
    void setManualSwitch( bool ms ) { _spManualSwitch = ms; }


    // size of page
    QRect getPageRect( unsigned int num, int diffx, int diffy,
		       float fakt = 1.0, bool decBorders = true ) const;

    void insertObject(KPresenterChild* ch ){ insertChild(ch);}

    void savePage( const QString &file, int pgnum );
    void pastePage( const QMimeSource * data, int pgnum );
    int insertPage( int _page, InsertPos _insPos, bool chooseTemplate, const QString &theFile );
    void deletePage( int _page );
    void copyPageToClipboard( int pgnum );

    // repaint all views
    void repaint( bool );

    void repaint( QRect );
    void repaint( KPObject* );

    // stuff for screen-presentations
    QValueList<int> reorderPage( unsigned int, int, int, float fakt = 1.0 );

    QPen presPen() const { return _presPen; }
    void setPresPen( QPen p ) {_presPen = p; }

    void restoreBackground( KPRPage * );
    void loadPastedObjs( const QString &in, int currPage, KPRPage* _page );

    void deSelectAllObj();

    KPImageCollection *getImageCollection() { return &_imageCollection; }
    KPGradientCollection *getGradientCollection() { return &_gradientCollection; }
    KPClipartCollection *getClipartCollection() { return &_clipartCollection; }

    KoAutoFormat * getAutoFormat() { return m_autoFormat; }

    void replaceObjs( bool createUndoRedo = true );

    PresSpeed getPresSpeed() const { return presSpeed; }
    void setPresSpeed( PresSpeed _presSpeed ) { presSpeed = _presSpeed; }

    void setPageTimer( unsigned int, int );
    int getPageTimer( unsigned int );

    void setPageSoundEffect( unsigned int, bool );
    void setPageSoundFileName( unsigned int, const QString& );

    bool getPageSoundEffect( unsigned int );
    QString getPageSoundFileName( unsigned int );

    int getLeftBorder();
    int getTopBorder();
    int getBottomBorder();


    void enableEmbeddedParts( bool f );

    KPTextObject *header() { return _header; }
    KPTextObject *footer() { return _footer; }
    bool hasHeader() { return _hasHeader; }
    bool hasFooter() { return _hasFooter; }
    void setHeader( bool b );
    void setFooter( bool b );
    KPFooterHeaderEditor *getHeaderFooterEdit() { return headerFooterEdit; }

    // Returns true if the slide pgNum (0 based)
    bool isSlideSelected( int pgNum ) const;
    // Returns the list of selected slides. Slide numbers are 0-based.
    QValueList<int> selectedSlides() const;
    QString selectedForPrinting() const;

    virtual DCOPObject* dcopObject();

    void initConfig();

    bool raiseAndLowerObject;

    bool pasting;
    int pasteXOffset, pasteYOffset;

    KoStyle* standardStyle();

    void addCommand( KCommand * cmd );

    KoZoomHandler* zoomHandler() const { return m_zoomHandler; }
    QFont defaultFont() const { return m_defaultFont; }

    /**
     * get custom kspell config
     */
    void setKSpellConfig(KSpellConfig _kspell);
    KSpellConfig * getKSpellConfig() {return m_pKSpellConfig;}

    bool dontCheckUpperWord() const { return m_bDontCheckUpperWord; }
    void setDontCheckUpperWord(bool _b) { m_bDontCheckUpperWord=_b;}

    bool dontCheckTitleCase() const {return  m_bDontCheckTitleCase;}
    void setDontCheckTitleCase(bool _b) {m_bDontCheckTitleCase=_b;}

    // The user-chosen global unit
    QString getUnitName() { return KoUnit::unitName( m_unit ); }
    KoUnit::Unit getUnit() { return m_unit; }
    void setUnit( KoUnit::Unit _unit );

    // in pt
    double getIndentValue() { return m_indent; }
    void setIndentValue(double _ind) { m_indent=_ind; }

    int maxRecentFiles() const { return m_maxRecentFiles; }

    KoVariableFormatCollection *variableFormatCollection() { return m_varFormatCollection; }

    void recalcVariables( int type );

    void recalcPageNum();

    KPrVariableCollection *getVariableCollection() {return m_varColl;}

    void refreshMenuCustomVariable();

    void setShowRuler(bool _ruler){ m_bShowRuler=_ruler; }
    bool showRuler() const { return m_bShowRuler; }

    void reorganizeGUI();

    
    // For NoteBar
    void setNoteText( int _pageNum, const QString &_text );
    QString getNoteText( int _pageNum );
    QStringList getNoteTextList() { return noteTextList; }

    // Get KPresenterView
    KPresenterView *getKPresenterView() const { return m_kpresenterView; }

    int undoRedoLimit();
    void setUndoRedoLimit(int _val);


    void updateRuler();

    unsigned int getPageNums() const { return m_pageList.count(); }

    // Tell all views to stop editing this frameset, if they were doing so
    void terminateEditing( KPTextObject * textObj )
    { emit sig_terminateEditing( textObj ); }

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
    void sig_changeActivePage( KPRPage* );

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void slotDocumentInfoModifed();

protected:
    KoView* createViewInstance( QWidget* parent, const char* name );

    // ************ functions ************
    /**
     * Overloaded function from @ref Document_impl. Saves all children.
     */
    virtual bool saveChildren( KoStore* _store, const QString &_path );

    QDomDocumentFragment saveBackground( QDomDocument& );
    QDomElement saveObjects( QDomDocument &doc );
    QDomElement saveTitle( QDomDocument &doc );
    QDomElement saveNote( QDomDocument &doc );
    void loadBackground( const QDomElement &element );
    void loadObjects( const QDomElement &element, bool _paste = false, KPRPage *_page=0L );
    void loadTitle( const QDomElement &element );
    void loadNote( const QDomElement &element );
    virtual bool completeLoading( KoStore* /* _store */ );
    void makeUsedPixmapList();

    // ************ variables ************

    struct PixCache
    {
        QString filename;
        QString pix_string;
    };

    // page layout
    KoPageLayout _pageLayout;

    // screenpresentations
    bool _spInfinitLoop, _spManualSwitch;

    // options
    int _rastX, _rastY;
    int _xRnd, _yRnd;

    // options for editmode
    QColor _txtBackCol;
    QColor _otxtBackCol;

    bool _clean;
    int objStartY, objStartNum;

    int _orastX, _orastY;
    PresSpeed presSpeed;

    QPen _presPen;

    KPImageCollection _imageCollection;
    KPGradientCollection _gradientCollection;
    KPClipartCollection _clipartCollection;

    KPTextObject *_header, *_footer;
    bool _hasHeader, _hasFooter;
    KPFooterHeaderEditor *headerFooterEdit;

    QMap<KoImageKey, QString> * m_pixmapMap;

    QValueList<KPClipartCollection::Key> clipartCollectionKeys;
    QStringList clipartCollectionNames;
    KoPageLayout __pgLayout;
    int lastObj;

    QString urlIntern;

    QValueList<KPImageKey> usedPixmaps;
    QValueList<KPClipartKey> usedCliparts;
    DCOPObject *dcop;

    int saveOnlyPage;
    QString m_tempFileInClipboard;
    QValueList<bool> m_selectedSlides;
    bool ignoreSticky;

    KoStyle *m_standardStyle;

    KCommandHistory * m_commandHistory;
    KoZoomHandler* m_zoomHandler;
    QFont m_defaultFont;
    KoAutoFormat * m_autoFormat;
    KSpellConfig *m_pKSpellConfig;
    bool m_bDontCheckUpperWord;
    bool m_bDontCheckTitleCase;
    bool m_bShowRuler;

    double m_indent; // in pt

    KoUnit::Unit m_unit;
    int m_maxRecentFiles;

    KoVariableFormatCollection *m_varFormatCollection;
    KPrVariableCollection *m_varColl;

    // For NoteBar
    QStringList noteTextList;


private:
    // For NoteBar
    void pageNoteInsert( unsigned int _pageNum );
    void pageNoteDelete( unsigned int _pageNum );


    KPresenterView *m_kpresenterView;;
    QPtrList<KPRPage> m_pageList;
};

#endif
