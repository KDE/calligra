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

#include <QList>
#include <QMap>
#include <QRect>
#include <QString>

#include <kglobalsettings.h>

#include <KoDocument.h>
#include <KoZoomHandler.h>

#include "kspread_global.h"

class QDomDocument;
class QObject;
class QPainter;

class DCOPObject;
class KCommand;
class KCommandHistory;
class KCompletion;
class KSpellConfig;

class KoGenStyles;
class KoOasisSettings;
class KoPicture;
class KoPictureCollection;
class KoPictureKey;
class KoStore;
class KoXmlWriter;

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

/**
 * \ingroup Plugins
 */
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
   * @param parentWidget the parent widget
   * @param parent the parent object
   * @param singleViewMode enables single view mode, if @c true
   */
  Doc( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );

  /**
   * Destroys the document.
   */
  ~Doc();

  /**
   * Possible values for the "type" argument of the KoGenStyle constructor.
   */
  enum
  {
    STYLE_PAGE = 20,      /**< table styles */
    STYLE_COLUMN,         /**< column styles */
    STYLE_ROW,            /**< row styles */
    STYLE_CELL_USER,      /**< common cell styles (used in StyleManager) */
    STYLE_CELL_AUTO,      /**< auto cell styles */
    STYLE_PAGEMASTER,     /**< master pages */
    STYLE_NUMERIC_NUMBER  /**< number styles */
  };

  /**
   * \ingroup OpenDocument
   */
  enum SaveFlag { SaveAll, SaveSelected }; // kpresenter and kword have have SavePage too

  /**
   * @return list of all documents
   */
  static QList<Doc*> documents();

  /**
   * @return the MIME type of KSpread document
   */
  virtual QByteArray mimeType() const { return MIME_TYPE; }


  /**
   * A sheet could use a different localization as the KDE default.
   * @return the KLocale associated with this sheet
   */
  KLocale *locale () const;

  /**
   * @return the Map that belongs to this Document
   */
  Map *map () const;

  /**
   * @return the StyleManager of this Document
   */
  StyleManager *styleManager () const;

  /**
   * @return the value parser of this Document
   */
  ValueParser *parser () const;

  /**
   * @return the value formatter of this Document
   */
  ValueFormatter *formatter () const;

  /**
   * @return the value converter of this Document
   */
  ValueConverter *converter () const;

  /**
   * @return the value calculator of this Document
   */
  ValueCalc *calc () const;


  /**
   * \ingroup Operations
   * Adds @p command to the command history. The command itself
   * would not be executed.
   * @param command the command which will be added
   */
  void addCommand( KCommand* command );

  /**
   * \ingroup Operations
   * Adds an undo object @p command . This is the same as addCommand, except
   * that it accepts Undo instance. Once every undo object
   * is converted to KCommand, this function will be obsoleted.
   * @param command the action which will be added
   * @see addCommand(KCommand*)
   */
  void addCommand( UndoAction* command );

  /**
   * \ingroup Operations
   * Undoes the last operation.
   */
  void undo();

  /**
   * \ingroup Operations
   * Redoes the last undo.
   */
  void redo();

  /**
   * \ingroup Operations
   * Locks/Unlocks undo buffer.
   * The locks are counted, so you calls of this method can be nested.
   */
  void setUndoLocked( bool lock );

  /**
   * \ingroup Operations
   * Returns true if undo buffer is locked.
   */
  bool undoLocked() const;

  /**
   * \ingroup Operations
   * @return the command history for the document. This is used
   * in View for updating the actions (i.e through
   * signal KCommandHistory::commandExecuted)
   * @see addCommand
   * @see undo
   * @see redo
   */
  KCommandHistory* commandHistory();

  /**
   * \ingroup Operations
   * Function to begin a KSpread 'operation'.
   *
   * Calls to emitBeginOperation and emitEndOperation should surround each
   * logical user operation.
   * During the operation, the following should hold true:
   * - No painting will be done to the screen
   * - No cell calculation will be done (maybe there are exceptions, such
   *   as the goalseek operation needs to calculate values)
   * During an operation, calls to Sheet::setRegionPaintDirty mark regions
   * as needing repainted.
   * Calls to begin/endOperation may be nested.
   * Calcualation and painting will be delayed until the outer begin/end
   * pair has finished.
   *
   * @param waitCursor specifies whether to put the hourglass up during
   *                   the operation.
   */
  void emitBeginOperation(bool waitCursor);

  /**
   * \ingroup Operations
   * Function to begin a KSpread 'operation'.
   * Changes the cursor to an hourglass.
   * @see emitBeginOperation(bool)
   * @reimp default override of KoDocument version.
   */
  virtual void emitBeginOperation();

  /**
   * \ingroup Operations
   * Marks the end of an operation.
   * If cells have been marked dirty while the operation, a repainted
   * of them is triggered.
   * @see emitBeginOperation(bool)
   * @see Sheet::setRegionPaintDirty
   * @reimp
   */
  virtual void emitEndOperation();

  /**
   * \ingroup Operations
   * Convenience function.
   * Marks the cells in @p region to get repainted and calls emitEndOperation().
   * @see emitBeginOperation(bool)
   * @see emitEndOperation()
   * @see Sheet::setRegionPaintDirty
   */
  void emitEndOperation( const Region& region );

  /**
   * \ingroup Operations
   */
  void increaseNumOperation();

  /**
   * \ingroup Operations
   */
  void decreaseNumOperation();

  /**
   * \ingroup Operations
   */
  void addDamage( Damage* damage );

  /**
   * \ingroup Operations
   */
  void enableUndo( bool _b );

  /**
   * \ingroup Operations
   */
  void enableRedo( bool _b );


  /**
   * @return the name of the unit used to display margins.
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
   * If b is true, horizontal scrollbar is visible, otherwise
   * it will be hidden.
   */
  void setShowHorizontalScrollBar( bool b );

  /**
   * Returns true if horizontal scroll bar is visible.
   */
  bool showHorizontalScrollBar() const;

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
   * If b is true, row header is visible, otherwise
   * it will be hidden.
   */
  void setShowRowHeader( bool b );

  /**
   * Returns true if row header is visible.
   */
  bool showRowHeader() const;

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
   * If b is true, status bar is visible, otherwise
   * it will be hidden.
   */
  void setShowStatusBar( bool b );

  /**
   * Returns true if status bar is visible.
   */
  bool showStatusBar() const;

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
   * If b is true, formula bar is visible, otherwise
   * it will be hidden.
   */
  void setShowFormulaBar( bool b );

  /**
   * Returns true if formula bar is visible.
   */
  bool showFormulaBar() const;

  /**
   * If b is true, an error message will pop up whenever error occurs.
   */
  void setShowMessageError( bool b );

  /**
   * Returns true if error message should pop up whenever error occurs.
   */
  bool showMessageError() const;

  /**
   * @return completion mode
   */
  KGlobalSettings::Completion completionMode() const ;

  /**
   * Sets the completion mode.
   * @param mode the mode to be set
   */
  void setCompletionMode( KGlobalSettings::Completion mode );

  KSpread::MoveTo moveToValue() const;
  void setMoveToValue(KSpread::MoveTo _moveTo);

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


  /**
   * \ingroup NativeFormat
   * Main saving method.
   */
  virtual QDomDocument saveXML();

  /**
   * \ingroup NativeFormat
   * Main loading method.
   */
  virtual bool loadXML( QIODevice *, const QDomDocument& doc );

  /**
   * \ingroup NativeFormat
   */
  QDomElement saveAreaName( QDomDocument& doc ) ;

  /**
   * \ingroup NativeFormat
   */
  void loadAreaName( const QDomElement& element );


  bool savingWholeDocument();


  /**
   * \ingroup OpenDocument
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

  /**
   * \ingroup OpenDocument
   * Main saving method.
   */
  virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

  /**
   * \ingroup OpenDocument
   * Saves the docment styles.
   */
  void saveOasisDocumentStyles( KoStore* store, KoGenStyles& mainStyles ) const;

  /**
   * \ingroup OpenDocument
   * Main loading method.
   * @see Map::loadOasis
   */
  virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles,
                          const QDomDocument& settings, KoStore* );

  /**
   * \ingroup OpenDocument
   */
  void saveOasisAreaName( KoXmlWriter & xmlWriter );

  /**
   * \ingroup OpenDocument
   */
  void loadOasisAreaName( const QDomElement& element );

  /**
   * \ingroup OpenDocument
   */
  void loadOasisCellValidation( const QDomElement&body );


  virtual int supportedSpecialFormats() const;

  virtual bool loadChildren( KoStore* _store );

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

  /**
   * @return true if the document is currently loading.
   */
  bool isLoading() const;

  bool docData( QString const & xmlTag, QDomElement & data );


  /**
   * \ingroup Plugins
   */
  void deregisterPlugin( Plugin * plugin );

  /**
   * \ingroup Plugins
   */
  void registerPlugin( Plugin * plugin );


  /**
   * \ingroup Painting
   * Primary entry point for painting.  Use this function to paint groups of cells
   *
   * @param painter the painter object to use.  This should already be
   *                initialized with the world matrix.  The begin and end calls
   *                should surround this function.
   *
   * @param viewRect the document coordinates showing what is actually visible in
   *                 the screen
   *
   * @param view the view of the region -- may be 0 but no selection markers
   *        can be passed in that case.
   *
   * @param region a list of rectangles indicating the cell ranges needing
   *               painted.
   */
  void paintCellRegions(QPainter& painter, const QRect &viewRect,
                        View* view, const Region& region);

  /**
   * \ingroup Painting
   */
  virtual void paintContent( QPainter & painter, const QRect & rect, bool transparent = false,
                             double zoomX = 1.0, double zoomY = 1.0 );

  /**
   * \ingroup Painting
   */
  void paintContent( QPainter & painter, const QRect & rect, bool transparent,
                     Sheet * sheet, bool drawCursor = true );


  virtual DCOPObject* dcopObject();

  void addAreaName(const QRect &_rect,const QString & name,const QString & sheetName);
  const QList<Reference>  & listArea();
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
   * @return the list of all embedded objects (charts, pictures and koffice
   *         objects) Use insertObject to add an object to the list
   */
  QList<EmbeddedObject*>& embeddedObjects();
  KoPictureCollection *pictureCollection();

  void initConfig();
  void saveConfig();
  void refreshLocale();

  bool delayCalculation() const;

  void updateBorderButton();

  void insertSheet( Sheet * sheet );
  void takeSheet( Sheet * sheet );

  /**
   * @deprecated
   */
  static QString getAttribute(const QDomElement &element, const char *attributeName, const QString &defaultValue)
  {
    return element.attribute( attributeName, defaultValue );
  }

  /**
   * @deprecated
   */
  static int getAttribute(const QDomElement &element, const char *attributeName, int defaultValue)
  {
    QString value;
    if ( ( value = element.attribute( attributeName ) ) != QString::null )
	return value.toInt();
    else
	return defaultValue;
  }

  /**
   * @deprecated
   */
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
   * \ingroup OpenDocument
   * \ingroup NativeFormat
   * @return the loading info
   */
  KSPLoadingInfo* loadingInfo() const;

  /**
   * \ingroup OpenDocument
   * \ingroup NativeFormat
   * Deletes the loading info. Called after loading is complete.
   */
  void deleteLoadingInfo();

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
  void refreshInterface();
  void flushDamages();

signals:
  /**
   * Emitted if all views have to be updated.
   */
  void sig_updateView();

  /**
   * Emitted if all interfaces have to be updated.
   */
  void sig_refreshView();

  /**
   * Emitted if we update to locale system.
   */
  void sig_refreshLocale();

  void sig_addAreaName( const QString & );
  void sig_removeAreaName( const QString & );

  void damagesFlushed( const QList<Damage*>& damages );

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
   * @reimp Overloaded function of KoDocument.
   */
  virtual bool completeLoading( KoStore* );

  /**
   * @reimp Overloaded function of KoDocument.
   */
  virtual bool saveChildren( KoStore* _store );

private:
  class Private;
  Private* d;

  // don't allow copy or assignment
  Doc( const Doc& );
  Doc& operator=( const Doc& );

  /* helper functions for painting */

  /**
   * \ingroup Painting
   * This function is called at the end of an operation and is responsible
   * for painting any changes that have occurred in the meantime
   */
  void paintUpdates();

  /**
   * \ingroup Painting
   */
  void paintRegion(QPainter& painter, const KoRect &viewRegion,
                   View* view, const QRect &paintRegion,
                   const Sheet* sheet);

  void loadPaper( QDomElement const & paper );

  /**
   * \ingroup OpenDocument
   * Saves the Document related settings.
   * The actual saving takes place in Map::saveOasisSettings.
   * @see Map::saveOasisSettings
   */
  void saveOasisSettings( KoXmlWriter &settingsWriter );

  /**
   * \ingroup OpenDocument
   * Loads the Document related settings.
   * The actual loading takes place in Map::loadOasisSettings.
   * @see Map::loadOasisSettings
   */
  void loadOasisSettings( const QDomDocument&settingsDoc );

  /**
   * \ingroup OpenDocument
   * Load the spell checker ignore list.
   */
  void loadOasisIgnoreList( const KoOasisSettings& settings );
};

} // namespace KSpread

#endif /* KSPREAD_DOC */
