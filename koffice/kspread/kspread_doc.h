/* This file is part of the KDE project
   Copyright (C) 2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2000-2005 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Phillip Mueller <philipp.mueller@gmx.de>
             (C) 2000 Werner Trobin <trobin@kde.org>
             (C) 1999-2000 Simon Hausmann <hausmann@kde.org>
             (C) 1999 David Faure <faure@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_DOC
#define KSPREAD_DOC

#include <qmap.h>
#include <qobject.h>
#include <qpainter.h>
#include <qrect.h>
#include <qstring.h>
#include <qvaluelist.h>

#include <kcompletion.h>

#include <KoDocument.h>
#include <KoZoomHandler.h>

#include "kspread_global.h"

class KCommand;

class KoCommandHistory;
class KoGenStyles;
class KoOasisSettings;
class KoPicture;
class KoPictureCollection;
class KoPictureKey;
class KoStore;
class KoXmlWriter;

class DCOPObject;
class QDomDocument;
class KSpellConfig;

#define MIME_TYPE "application/x-kspread"

namespace KSpread
{
class Damage;
class ValueParser;
class ValueConverter;
class ValueFormatter;
class ValueCalc;
class Sheet;
class Doc;
class View;
class Map;
class Region;
class StyleManager;
class UndoAction;
class KSPLoadingInfo;
class EmbeddedObject;

struct Reference
{
    QString sheet_name;
    QString ref_name;
    QRect rect;
};

class Plugin
{
 public:
  Plugin() {}
  virtual ~Plugin() {}
  virtual QDomElement saveXML( QDomDocument & doc ) const = 0;
};

/**
 * This class holds the data that makes up a spreadsheet.
 */
class KSPREAD_EXPORT Doc : public KoDocument, public KoZoomHandler
{
  Q_OBJECT
  Q_PROPERTY( bool getShowRowHeader READ getShowRowHeader )
  Q_PROPERTY( bool getShowColHeader READ getShowColHeader )
  Q_PROPERTY( double getIndentValue READ getIndentValue WRITE setIndentValue )
  Q_PROPERTY( bool getShowMessageError READ getShowMessageError WRITE setShowMessageError)
  Q_PROPERTY( bool dontCheckUpperWord READ dontCheckUpperWord WRITE setDontCheckUpperWord)
  Q_PROPERTY( bool dontCheckTitleCase READ dontCheckTitleCase WRITE setDontCheckTitleCase)

  Q_PROPERTY( int syntaxVersion READ syntaxVersion )
  Q_PROPERTY( bool showVerticalScrollBar READ showVerticalScrollBar WRITE setShowVerticalScrollBar )
  Q_PROPERTY( bool showHorizontalScrollBar READ showHorizontalScrollBar WRITE setShowHorizontalScrollBar )
  Q_PROPERTY( bool showColumnHeader READ showColumnHeader WRITE setShowColumnHeader )
  Q_PROPERTY( bool showRowHeader READ showRowHeader WRITE setShowRowHeader )
  Q_PROPERTY( bool showStatusBar READ showStatusBar WRITE setShowStatusBar )
  Q_PROPERTY( bool showFormulaBar READ showFormulaBar WRITE setShowFormulaBar )
  Q_PROPERTY( bool showTabBar READ showTabBar WRITE setShowTabBar )
  Q_PROPERTY( int undoRedoLimit READ undoRedoLimit WRITE setUndoRedoLimit )

public:

  /**
   * Creates a new document.
   */
  Doc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0,
  const char* name = 0, bool singleViewMode = false );

  /**
   * Destroys the document.
   */
  ~Doc();

  /**
   * Possible values for the "type" argument of the KoGenStyle constructor.
   */
  enum { STYLE_PAGE = 20,
         STYLE_COLUMN_USER,    /**< commmon column styles */
         STYLE_COLUMN_AUTO,    /**< auto column styles */
         STYLE_ROW_USER,       /**< common row styles */
         STYLE_ROW_AUTO,       /**< auto row styles */
         STYLE_CELL_USER,
         STYLE_CELL_AUTO,
         STYLE_PAGEMASTER,
         STYLE_NUMERIC_NUMBER };

  /**
   * Returns list of all documents.
   */
  static QValueList<Doc*> documents();

  /**
   * Returns the MIME type of KSpread document.
   */
  virtual QCString mimeType() const { return MIME_TYPE; }

  KLocale *locale () const;
  Map *map () const;
  StyleManager *styleManager () const;
  ValueParser *parser () const;
  ValueFormatter *formatter () const;
  ValueConverter *converter () const;
  ValueCalc *calc () const;

  /**
   * Adds a command to the command history. The command itself
   * would not be executed.
   */
  void addCommand( KCommand* command );

  /*
   * Adds an undo object. This is the same as addCommand, except
   * that it accepts Undo instance. Once every undo object
   * is converted to KCommand, this function will be obsoleted.
   */
  void addCommand( UndoAction* command );

  /**
   * Undoes the last operation.
   */
  void undo();

  /**
   * Redoes the last undo.
   */
  void redo();

  /**
   * Locks the undo buffer.
   */
  void undoLock();

  /**
   * Releases lock of undo buffer.
   */
  void undoUnlock();

  /**
   * Returns true if undo buffer is locked.
   */
  bool undoLocked() const;

  /**
   * Returns the command history for the document. This is used
   * in View for updating the actions (i.e through
   * signal KoCommandHistory::commandExecuted)
   */
  KoCommandHistory* commandHistory();

  /**
   * Returns the name of the unit used to display margins.
   * For example, if unit() returns KoUnit::U_MM, then
   * this functions return "mm".
   */
  QString unitName() const;

  /**
   * Returns the syntax version of the currently opened file
   */
  int syntaxVersion( ) const;

  /**
   * If b is true, vertical scrollbar is visible, otherwise
   * it will be hidden.
   */
  void setShowVerticalScrollBar( bool b );

  /**
   * Returns true if vertical scroll bar is visible.
   */
  bool showVerticalScrollBar() const;

  /**
   * \deprecated Use showVerticalScrollBar().
   */
  bool getShowVerticalScrollBar() const { return showVerticalScrollBar(); }

  /**
   * If b is true, horizontal scrollbar is visible, otherwise
   * it will be hidden.
   */
  void setShowHorizontalScrollBar( bool b );

  /**
   * Returns true if horizontal scroll bar is visible.
   */
  bool showHorizontalScrollBar() const;

  /**
   * \deprecated Use showHorizontalScrollBar().
   */
  bool getShowHorizontalScrollBar() const { return showHorizontalScrollBar(); }

  /**
   * If b is true, column header is visible, otherwise
   * it will be hidden.
   */
  void setShowColumnHeader( bool b );

  /**
   * Returns true if column header is visible.
   */
  bool showColumnHeader() const;

  /**
   * \deprecated Use setShowColumnHeader().
   */
  void setShowColHeader( bool b ){ setShowColumnHeader( b ) ; }

  /**
   * \deprecated Use showColumnHeader().
   */
  bool getShowColHeader() const { return showColumnHeader(); }

  /**
   * If b is true, row header is visible, otherwise
   * it will be hidden.
   */
  void setShowRowHeader( bool b );

  /**
   * Returns true if row header is visible.
   */
  bool showRowHeader() const;

  /**
   * \deprecated Use showRowHeader().
   */
  bool getShowRowHeader() const { return showRowHeader(); }

  /**
   * Sets the color of the grid.
   */
  void setGridColor( const QColor& color );

  /**
   * Returns the color of the grid.
   */
  QColor gridColor() const;

  /**
   * Sets the indentation value.
   */
  void setIndentValue( double val );

  /**
   * Returns the indentation value.
   */
  double indentValue() const;

  /**
   * \deprecated Use indentValue().
   */
  double getIndentValue() const { return indentValue(); }

  /**
   * If b is true, status bar is visible, otherwise
   * it will be hidden.
   */
  void setShowStatusBar( bool b );

  /**
   * Returns true if status bar is visible.
   */
  bool showStatusBar() const;

  /**
   * \deprecated Use showStatusBar().
   */
  bool getShowStatusBar() const { return showStatusBar(); }

  /**
   * If b is true, tab bar is visible, otherwise
   * it will be hidden.
   */
  void setShowTabBar( bool b );

  /**
   * Returns true if tab bar is visible.
   */
  bool showTabBar() const;

  /**
   * \deprecated Use showTabBar().
   */
  bool getShowTabBar() const { return showTabBar(); }

  /**
   * If b is true, formula bar is visible, otherwise
   * it will be hidden.
   */
  void setShowFormulaBar( bool b );

  /**
   * Returns true if formula bar is visible.
   */
  bool showFormulaBar() const;

  /**
   * \deprecated Use showFormulaBar().
   */
  bool getShowFormulaBar() const { return showFormulaBar(); }

  /**
   * If b is true, an error message will pop up whenever error occurs.
   */
  void setShowMessageError( bool b );

  /**
   * Returns true if error message should pop up whenever error occurs.
   */
  bool showMessageError() const;

  /**
   * \deprecated Use showMessageError().
   */
  bool getShowMessageError() const{ return showMessageError(); }

  /**
  * completion mode
  */

  KGlobalSettings::Completion completionMode( )const ;
  void setCompletionMode( KGlobalSettings::Completion _complMode);

  KSpread::MoveTo getMoveToValue()const;
  void setMoveToValue(KSpread::MoveTo _moveTo) ;

  /**
  * Method of calc
  */
  void setTypeOfCalc( MethodOfCalc _calc);
  MethodOfCalc getTypeOfCalc() const;


  /**
   * get custom kspell config
   */
  void setKSpellConfig(KSpellConfig _kspell);
  KSpellConfig * getKSpellConfig();

  bool dontCheckUpperWord() const;
  void setDontCheckUpperWord(bool _b);

  bool dontCheckTitleCase() const;
  void setDontCheckTitleCase(bool _b);

  QColor pageBorderColor() const;
  void changePageBorderColor( const QColor  & _color);

  virtual bool completeSaving( KoStore* _store );

  virtual QDomDocument saveXML();

  enum SaveFlag { SaveAll, SaveSelected }; // kpresenter and kword have have SavePage too

  bool savingWholeDocument();

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
     */
  bool saveOasisHelper( KoStore* store, KoXmlWriter* manifestWriter, SaveFlag saveFlag,
                        QString* plainText = 0, KoPicture* picture = 0 );

  virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );
  void saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles ) const;
  virtual int supportedSpecialFormats() const;

  virtual bool loadXML( QIODevice *, const QDomDocument& doc );
  virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* );
  virtual bool loadChildren( KoStore* _store );
  QDomElement saveAreaName( QDomDocument& doc ) ;
    void saveOasisAreaName( KoXmlWriter & xmlWriter );

  void loadAreaName( const QDomElement& element );
  void loadOasisAreaName( const QDomElement& element );
  void loadOasisCellValidation( const QDomElement&body );

  virtual void addView( KoView *_view );

  virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget=0);

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

  // virtual void printMap( QPainter & _painter );

  void enableUndo( bool _b );
  void enableRedo( bool _b );

  /**
   * @return true if the document is currently loading.
   */
  bool isLoading() const;

  /**
     Prevents painting. Useful when importing data.
   * @param status switch the painting on/off
   */
  void doNotPaint(bool status);

  virtual void paintContent( QPainter & painter, const QRect & rect, bool transparent = false,
                             double zoomX = 1.0, double zoomY = 1.0 );
  void paintContent( QPainter & painter, const QRect & rect, bool transparent,
                     Sheet * sheet, bool drawCursor = true );

  bool docData( QString const & xmlTag, QDomElement & data );
  void deregisterPlugin( Plugin * plugin );
  void registerPlugin( Plugin * plugin );

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
   * @param sheet the sheet which the cells in cellRegions come from
   *
   * @param drawCursor whether or not to draw the selection rectangle and the choose
   *                   marker
   */
  void paintCellRegions(QPainter& painter, const QRect &viewRect,
                        View* view,
                        QValueList<QRect> cellRegions,
                        const Sheet* sheet, bool drawCursor);

  virtual DCOPObject* dcopObject();

  void addAreaName(const QRect &_rect,const QString & name,const QString & sheetName);
  const QValueList<Reference>  & listArea();
  void removeArea( const QString &name);
  KCompletion & completion();
  void addStringCompletion(const QString & stringCompletion);

  void changeAreaSheetName(const QString & oldName,const QString &sheetName);


  QRect getRectArea(const QString &  _sheetName);

  /**
   * Inserts an object to the object list.
   */
  void insertObject( EmbeddedObject * obj );
    /**
    * @return the list of all embedded objects (charts, pictures and koffice objects) Use insertObject to add an object to the list.
    */
  QPtrList<EmbeddedObject>& embeddedObjects();
  KoPictureCollection *pictureCollection();

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
   * During an operation, calls to Sheet::setRegionPaintDirty mark
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
   * Mark the end of an operation and triggers a repaint of any cells or regions of cells which
   * have been marked as 'dirty' via calls to Sheet::setRegionPaintDirty
   * See above comment to emitBeginOperation(bool).
   */
  virtual void emitEndOperation();

  /**
   * s.a. difference does only specified tasks and thats why it improves performance
   * significantly. rect normally are the visible cells/the cells that need to be updated.
   *
   * TODO:  This is essentially redundant now since emitEndOperation behaviour has been fixed to
   * only update cells that were already in the paint dirty list.
   */
  void emitEndOperation( const Region& region );

  bool delayCalculation() const;


  void updateBorderButton();

  void insertSheet( Sheet * sheet );
  void takeSheet( Sheet * sheet );

  // The user-chosen global unit

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
    void addIgnoreWordAllList( const QStringList & _lst);
    QStringList spellListIgnoreAll() const ;

    void setDisplaySheet(Sheet *_Sheet );
    Sheet * displaySheet() const;

    /**
     * @return the loading info
     */
    KSPLoadingInfo* loadingInfo() const;

    /**
     * deletes the loading info after loading is complete
     */
    void deleteLoadingInfo();

  void increaseNumOperation();
  void decreaseNumOperation();

  void addDamage( Damage* damage );

/* Function specific when we load config from file */
  void loadConfigFromFile();
  bool configLoadFromFile() const;

  // repaint (update) all views
  void repaint( const QRect& );
  void repaint( EmbeddedObject* );
  void repaint( const KoRect& );

  virtual void addShell( KoMainWindow *shell );

  int undoRedoLimit() const;
  void setUndoRedoLimit(int _val);

public slots:

  //void newView(); obsloete (Werner)

  void refreshInterface();

  void flushDamages();

signals:
  // Document signals
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

  void sig_addAreaName( const QString & );
  void sig_removeAreaName( const QString & );

  void damagesFlushed( const QValueList<Damage*>& damages );

protected slots:
  void commandExecuted();
  void documentRestored();

  virtual void openTemplate( const QString& file );
  virtual void initEmpty();

protected:
  KoView* createViewInstance( QWidget* parent, const char* name );

  void makeUsedPixmapList();
  void insertPixmapKey( KoPictureKey key );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool completeLoading( KoStore* );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool saveChildren( KoStore* _store );

  class SavedDocParts : public QMap<QString, QDomElement> {};
  SavedDocParts m_savedDocParts;

private:
  class Private;
  Private* d;

  // don't allow copy or assignment
  Doc( const Doc& );
  Doc& operator=( const Doc& );

  /* helper functions for painting */

  /**
   * This function is called at the end of an operation and is responsible
   * for painting any changes that have occurred in the meantime
   */
  void paintUpdates();

  void PaintRegion(QPainter& painter, const KoRect &viewRegion,
                   View* view, const QRect &paintRegion,
                   const Sheet* sheet);

  void loadPaper( QDomElement const & paper );

    void saveOasisSettings( KoXmlWriter &settingsWriter );
    void loadOasisSettings( const QDomDocument&settingsDoc );
    void loadOasisIgnoreList( const KoOasisSettings& settings );
    KSPLoadingInfo *m_loadingInfo;
};

} // namespace KSpread

#endif /* KSPREAD_DOC */
