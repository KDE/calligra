/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef __kspread_doc_h__
#define __kspread_doc_h__

class KSpreadDoc;
class KSpreadInterpreter;
class KSpreadUndo;
class KSpreadView;
class KSpreadMap;
class KSpreadSheet;
class KSpreadStyleManager;

class KoStore;

class View;

class DCOPObject;

class QDomDocument;

class KSpellConfig;


#include <koDocument.h>
#include <koPageLayoutDia.h>
#include <kozoomhandler.h>

#include <kcompletion.h>
#include <qmap.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpen.h>
#include <qrect.h>
#include <qstring.h>

#include "kspread_interpreter.h"
#include "kspread_locale.h"

#define MIME_TYPE "application/x-kspread"

struct Reference
{
    QString table_name;
    QString ref_name;
    QRect rect;
};

class KSpreadPlugin 
{
 public:
  KSpreadPlugin() {}
  virtual ~KSpreadPlugin() {}
  virtual QDomElement saveXML( QDomDocument & doc ) const = 0;
};

class DocPrivate;

/**
 * This class holds the data that makes up a spreadsheet.
 */
class KSpreadDoc : public KoDocument, public KoZoomHandler
{
  Q_OBJECT
  Q_PROPERTY( bool getShowRowHeader READ getShowRowHeader )
  Q_PROPERTY( bool getShowColHeader READ getShowColHeader )
  Q_PROPERTY( double getIndentValue READ getIndentValue WRITE setIndentValue )
  Q_PROPERTY( bool getShowMessageError READ getShowMessageError WRITE setShowMessageError)
  Q_PROPERTY( bool dontCheckUpperWord READ dontCheckUpperWord WRITE setDontCheckUpperWord)
  Q_PROPERTY( bool dontCheckTitleCase READ dontCheckTitleCase WRITE setDontCheckTitleCase)

public:

  /**
   * Creates a new document.
   */
  KSpreadDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
  
  /**
   * Destroys the document.
   */
  ~KSpreadDoc();

  /**
   * Returns the workbook which holds all the sheets.
   */
  KSpreadMap* workbook() const; 
   
  /**
   * Same as workbook().
   * This function is obsolete and will be removed in future version.
   */
  KSpreadMap * map() const;
  
  /**
   * Returns the MIME type of KSpread document.
   */
  virtual QCString mimeType() const { return MIME_TYPE; }  

  /**
   * Undo the last operation.
   */
  void undo();

  /**
   * Redo the last undo.
   */
  void redo();

  /*
   * Returns the style manager for this document.
   */  
  KSpreadStyleManager * styleManager();

  virtual QDomDocument saveXML();

  virtual bool loadChildren( KoStore* _store );
  virtual bool loadXML( QIODevice *, const QDomDocument& doc );
  QDomElement saveAreaName( QDomDocument& doc ) ;
  void loadAreaName( const QDomElement& element );
  virtual void addView( KoView *_view );

  virtual bool initDoc();


  /**
   * @return a pointer to a new KSpreadSheet. The KSpreadSheet is not added to the map
   *         nor added to the GUI.
   */
  KSpreadSheet * createTable();

  /**
   * Adds a KSpreadSheet to the GUI and makes it active. In addition the KSpreadSheet is
   * added to the map.
   *
   * @see KSpreadView
   * @see KSpreadMap
   */
  void addTable( KSpreadSheet * _table );

  /**
   * @return the locale which was used for creating this document.
   */
  KLocale * locale();

  /**
   * Change the zoom factor to @p z (e.g. 150 for 150%)
   * and/or change the resolution, given in DPI.
   * This is done on startup and when printing.
   * The same call combines both so that all the updating done behind
   * the scenes is done only once, even if both zoom and DPI must be changed.
   */
  virtual void setZoomAndResolution( int zoom, int dpiX, int dpiY );

  /**
   * updates all properties after zoom changed
   */
  void newZoomAndResolution( bool updateViews, bool forPrint );

  /**
   * @return the KScript Interpreter used by this document.
   */
  KSpreadInterpreter* interpreter()const;

  /**
   * Kills the interpreter and creates a new one and
   * reloads all scripts. This is useful if they have been
   * edited and the changes should take effect.
   */
  void resetInterpreter();

  /**
   * @return a context that can be used for evaluating formulas.
   *         This function does remove any exception from the context.
   */
  KSContext & context();

  /**
   * @return the object that is respnsible for keeping track
   *         of the undo buffer.
   */
  KSpreadUndo * undoBuffer()const;

  // virtual void printMap( QPainter & _painter );

  void enableUndo( bool _b );
  void enableRedo( bool _b );

  /**
   * @return TRUE if the document is currently loading.
   */
  bool isLoading() const;

  void setDefaultGridPen( const QPen & );
  const QPen & defaultGridPen() ;

  virtual void paintContent( QPainter & painter, const QRect & rect, bool transparent = false, 
                             double zoomX = 1.0, double zoomY = 1.0 );
  void paintContent( QPainter & painter, const QRect & rect, bool transparent, 
                     KSpreadSheet * table, bool drawCursor = true );
  
  bool docData( QString const & xmlTag, QDomElement & data );
  void deregisterPlugin( KSpreadPlugin * plugin );
  void registerPlugin( KSpreadPlugin * plugin );

  /**
   * Primary entry point for painting.  Use this function to paint groups of cells
   *
   * @param painter the painter object to use.  This should already be
   *                initialized with the world matrix.  The begin and end calls
   *                should surround this function.
   *
   * @param viewRect the document coordinates showing what is actually visible in
   *                 the screen
   *
   * @param view the view of the region -- may be NULL but no selection markers
   *        can be passed in that case.
   *
   * @param cellRegions a list of rectangles indicating the cell ranges needing
   *                    painted.
   *
   * @param table the table which the cells in cellRegions come from
   *
   * @param drawCursor whether or not to draw the selection rectangle and the choose
   *                   marker
   */
  void paintCellRegions(QPainter& painter, const QRect &viewRect,
                        KSpreadView* view,
                        QValueList<QRect> cellRegions,
                        const KSpreadSheet* table, bool drawCursor);

  virtual DCOPObject* dcopObject();

  static QPtrList<KSpreadDoc>& documents();

  void addAreaName(const QRect &_rect,const QString & name,const QString & tableName);
  const QValueList<Reference>  & listArea();
  void removeArea( const QString &name);

  KCompletion & completion();
  void addStringCompletion(const QString & stringCompletion);

  void changeAreaTableName(const QString & oldName,const QString &tableName);


  QRect getRectArea(const QString &  _tableName);

  /**
  * hide/show scrollbar
  */
  void setShowVerticalScrollBar(bool _show) {   m_bVerticalScrollBarShow=_show;}
  void setShowHorizontalScrollBar(bool _show) {   m_bHorizontalScrollBarShow=_show;}

  bool getShowVerticalScrollBar()const { return  m_bVerticalScrollBarShow;}
  bool getShowHorizontalScrollBar()const {  return  m_bHorizontalScrollBarShow;}

  /**
  * completion mode
  */

  KGlobalSettings::Completion completionMode( )const { return m_iCompletionMode;}
  void setCompletionMode( KGlobalSettings::Completion _complMode) {  m_iCompletionMode=_complMode;}

  /**
  * hide/show row/col header
  */
  void setShowColHeader(bool _show) { m_bShowColHeader=_show; }
  void setShowRowHeader(bool _show) { m_bShowRowHeader=_show; }

  bool getShowColHeader()const { return  m_bShowColHeader; }
  bool getShowRowHeader()const { return  m_bShowRowHeader; }

  /**
  * value of indent
  */
  double getIndentValue()const { return m_dIndentValue; }
  void setIndentValue( double _val ) { m_dIndentValue = _val; }

  KSpread::MoveTo getMoveToValue()const {return m_EMoveTo;}
  void setMoveToValue(KSpread::MoveTo _moveTo) {m_EMoveTo=_moveTo;}

  /**
  * Show or not error message
  */
  void setShowMessageError(bool _show) {   m_bShowError=_show;}
  bool getShowMessageError()const { return  m_bShowError;}

  /**
  * Method of calc
  */
  void setTypeOfCalc( MethodOfCalc _calc) { m_EMethodOfCalc=_calc;}
  MethodOfCalc getTypeOfCalc()const{ return m_EMethodOfCalc;}

  /**
   * show/hide tabbar
   */
  void setShowTabBar(bool _tabbar) {  m_bShowTabBar=_tabbar;}
  bool getShowTabBar()const { return  m_bShowTabBar;}

  void changeDefaultGridPenColor( const QColor &_col);

  QColor pageBorderColor() const;
  void changePageBorderColor( const QColor  & _color);

  /**
   * show/hide comment indicator
   */
  void setShowCommentIndicator(bool _indic) {  m_bShowCommentIndicator=_indic;}
  bool getShowCommentIndicator()const { return  m_bShowCommentIndicator;}

  /**
   * show/hide formula bar
   */
  void setShowFormulaBar(bool _formulaBar) {  m_bShowFormulaBar=_formulaBar;}
  bool getShowFormulaBar()const { return  m_bShowFormulaBar;}

  /**
   * get custom kspell config
   */
  void setKSpellConfig(KSpellConfig _kspell);
  KSpellConfig * getKSpellConfig()const {return m_pKSpellConfig;}

  bool dontCheckUpperWord() const { return m_bDontCheckUpperWord; }
  void setDontCheckUpperWord(bool _b) { m_bDontCheckUpperWord = _b; }

  bool dontCheckTitleCase() const { return  m_bDontCheckTitleCase; }
  void setDontCheckTitleCase(bool _b) { m_bDontCheckTitleCase = _b; }

  /**
   * show/hide status bar
   */
  void setShowStatusBar(bool _statusBar) { m_bShowStatusBar=_statusBar;}
  bool getShowStatusBar()const { return  m_bShowStatusBar;}

  void insertChild( KoDocumentChild * child ) { KoDocument::insertChild( child ); }

  void initConfig();
  void saveConfig();
  void refreshLocale();

  /**
   * Functions to begin a kspread 'operation'.  Calls to emitBeginOperation
   * and emitEndOperation should surround each logical user operation.
   * During the operation, the following should hold true:
   * - No painting will be done to the screen
   * - No cell calculation will be done (maybe there are exceptions, such
   *   as the goalseek operation needs to calculate values)
   * During an operation, calls to KSpreadSheet::setRegionPaintDirty mark
   * regions as needing repainted.  Once the emitEndOperation function is
   * called, those regions will be painted all at once, values being calculated
   * as necessary.
   * Calls to begin/endOperation may be nested.  Calcualation/painting will
   * be delayed until the outer begin/end pair has finished.
   *
   * The waitCursor parameter specifies whether to put the hourglass
   * up during the operation.
   *
   */
  void emitBeginOperation(bool waitCursor);

  /** default override of koDocument version.  Changes the cursor to an
   * hourglass
   */
  virtual void emitBeginOperation();

  /**
   * Mark the end of an operation and triggers repaints/calculations.
   * See above comment to emitBeginOperation(bool).
   */
  void emitEndOperation();

  /**
   * s.a. difference does only specified tasks and thats why it improves performance
   * significantly. rect normally are the visible cells/the cells that need to be updated. 
   */
  void emitEndOperation( QRect const & rect );

  bool delayCalculation();


  void updateBorderButton();

  void insertTable( KSpreadSheet * table );
  void takeTable( KSpreadSheet * table );
  
  // The user-chosen global unit
  QString getUnitName()const { return KoUnit::unitName( m_unit ); }
  KoUnit::Unit getUnit()const { return m_unit; }
  void setUnit( KoUnit::Unit _unit );

  /**
   * Returns the syntax version of the currently opened file
   */
  int syntaxVersion( ) const;

  static QString getAttribute(const QDomElement &element, const char *attributeName, const QString &defaultValue)
  {
    return element.attribute( attributeName, defaultValue );
  }

  static int getAttribute(const QDomElement &element, const char *attributeName, int defaultValue)
  {
    QString value;
    if ( ( value = element.attribute( attributeName ) ) != QString::null )
	return value.toInt();
    else
	return defaultValue;
  }

  static double getAttribute(const QDomElement &element, const char *attributeName, double defaultValue)
  {
    QString value;
      if ( ( value = element.attribute( attributeName ) ) != QString::null )
	return value.toDouble();
      else
	return defaultValue;
  }

    void addIgnoreWordAll( const QString & word);
    void clearIgnoreWordAll( );
    void addIgnoreWordAllList( const QStringList & _lst)
        { m_spellListIgnoreAll = _lst;}
    QStringList spellListIgnoreAll() const { return m_spellListIgnoreAll;}

    void setDisplayTable(KSpreadSheet *_Table );
    KSpreadSheet * displayTable()const;

  void increaseNumOperation() { ++m_numOperations; }
  void decreaseNumOperation() { --m_numOperations; }

public slots:

  //void newView(); obsloete (Werner)

  void refreshInterface();

signals:
  // Document signals
  /**
   * Emitted if a new table is added to the document.
   */
  void sig_addTable( KSpreadSheet *_table );
  /**
   * Emitted if all views have to be updated.
   */
  void sig_updateView();
  /**
  *  Emitted if all interfaces have to be updated.
  */
  void sig_refreshView();
  /**
   * Emitted if we update to locale system
   */
  void sig_refreshLocale();

protected:
  KoView* createViewInstance( QWidget* parent, const char* name );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool completeLoading( KoStore* );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool saveChildren( KoStore* _store );

  /**
   * Initializes the KScript Interpreter.
   */
  void initInterpreter();
  /**
   * Destroys the interpreter.
   */
  void destroyInterpreter();

  static QPtrList<KSpreadDoc>* s_docs;
  static int s_docId;

  /**
  * bool which define if you can show scroolbar
  */
  bool m_bVerticalScrollBarShow;
  bool m_bHorizontalScrollBarShow;

  /**
  * completion mode
  */
  KGlobalSettings::Completion m_iCompletionMode;

  /**
  * bool which define if you can show col/row header
  */
  bool m_bShowColHeader;
  bool m_bShowRowHeader;

  double m_dIndentValue;

  KSpread::MoveTo m_EMoveTo;

  bool m_bShowError;

  MethodOfCalc m_EMethodOfCalc;

  bool m_bShowTabBar;

  bool m_bShowCommentIndicator;
  bool m_bShowFormulaBar;
  bool m_bShowStatusBar;

  bool m_bDelayCalculation;

  KSpellConfig *m_pKSpellConfig;
  bool m_bDontCheckUpperWord;
  bool m_bDontCheckTitleCase;
  QStringList m_spellListIgnoreAll;
  KoUnit::Unit m_unit;
  KSpreadSheet *m_activeTable;

  int m_numOperations;

  class SavedDocParts : public QMap<QString, QDomElement> {};
  SavedDocParts m_savedDocParts;

private:

  DocPrivate* d;

  // don't allow copy or assignment
  KSpreadDoc( const KSpreadDoc& );
  KSpreadDoc& operator=( const KSpreadDoc& );
  
  /* helper functions for painting */

  /**
   * This function is called at the end of an operation and is responsible
   * for painting any changes that have occurred in the meantime
   */
  void paintUpdates();

  void PaintRegion(QPainter& painter, const KoRect &viewRegion,
                   KSpreadView* view, const QRect &paintRegion,
                   const KSpreadSheet* table);
  void PaintChooseRect(QPainter& painter, const KoRect &viewRect,
                       KSpreadView* view, const KSpreadSheet* table,
                       const QRect &chooseRect);
  void PaintNormalMarker(QPainter& painter, const KoRect &viewRect,
                         KSpreadView* view, const KSpreadSheet* table,
                         const QRect &selection);

  /**
   * helper function in drawing the marker and choose marker.
   * @param marker the rectangle that represents the marker being drawn
   *               (cell coordinates)
   * @param table the table this marker is on
   * @param viewRect the visible area on the canvas
   * @param positions output parameter where the viewable left, top, right, and
   *                  bottom of the marker will be.  They are stored in the array
   *                  in that order, and take into account cropping due to part
   *                  of the marker being off screen.  This array should have
   *                  at least a size of 4 pre-allocated.
   * @param paintSides booleans indicating whether a particular side is visible.
   *                   Again, these are in the order left, top, right, bottom.
   *                   This should be preallocated with a size of at least 4.
   */
  void retrieveMarkerInfo( const QRect &marker, const KSpreadSheet* table,
                           KSpreadView* view, const KoRect &viewRect,
                           double positions[], bool paintSides[] );
  void loadPaper( QDomElement const & paper );
};

#endif
