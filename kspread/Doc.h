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
   Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DOC
#define KSPREAD_DOC

#include <QList>
#include <QMap>
#include <QRect>
#include <QString>

#include <kglobalsettings.h>

#include <KoDocument.h>
#include <KoShapeControllerBase.h>
#include <KoXmlReader.h>

#include "Global.h"

class QDomDocument;
class QObject;
class QPainter;

class KCompletion;
class K3SpellConfig;

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
class ColumnFormat;
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
class RowFormat;
class StyleManager;
class UndoAction;
class LoadingInfo;
class EmbeddedObject;

struct Reference
{
    QString sheet_name;
    QString ref_name;
    QRect rect;
};

/**
 * This class holds the data that makes up a spreadsheet.
 */
class KSPREAD_EXPORT Doc : public KoDocument, public KoShapeControllerBase
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
#if 0 // UNDOREDOLIMIT
  Q_PROPERTY( int undoRedoLimit READ undoRedoLimit WRITE setUndoRedoLimit )
#endif

public:
  /**
   * Creates a new document.
   * @param parentWidget the parent widget
   * @param parent the parent object
   * @param singleViewMode enables single view mode, if @c true
   */
  explicit Doc( QWidget* parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );

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
    STYLE_COLUMN_USER,    /**< commmon column styles */
    STYLE_COLUMN_AUTO,    /**< auto column styles */
    STYLE_ROW_USER,       /**< common row styles */
    STYLE_ROW_AUTO,       /**< auto row styles */
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

    // KoShapeControllerBase interface
    /// reimplemented method from KoShapeControllerBase
    virtual void addShape( KoShape* shape );
    /// reimplemented method from KoShapeControllerBase
    virtual void removeShape(KoShape* shape);

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
     * \return the default row format
     */
    const ColumnFormat* defaultColumnFormat() const;

    /**
     * \return the default row format
     */
    const RowFormat* defaultRowFormat() const;

    /**
     * Sets the default column width to \p width.
     */
    void setDefaultColumnWidth( double width );

    /**
     * Sets the default row height to \p height.
     */
    void setDefaultRowHeight( double height );

  /**
   * \ingroup Operations
   * Adds @p command to the command history. The command itself
   * would not be executed.
   * @param command the command which will be added
   */
  void addCommand( QUndoCommand* command );

  /**
   * \ingroup Operations
   * Adds an undo object @p command . This is the same as addCommand, except
   * that it accepts Undo instance. Once every undo object
   * is converted to QUndoCommand, this function will be obsoleted.
   * @param command the action which will be added
   * @see addCommand(QUndoCommand*)
   */
  void addCommand( UndoAction* command );

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

  void emitEndOperation( const QRect& rect );

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
   * For example, if unit() returns KoUnit::Millimeter, then
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
  void setKSpellConfig(K3SpellConfig _kspell);
  K3SpellConfig * getKSpellConfig();

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
  virtual bool loadXML( QIODevice *, const KoXmlDocument& doc );

  /**
   * \ingroup NativeFormat
   */
  QDomElement saveAreaName( QDomDocument& doc ) ;

  /**
   * \ingroup NativeFormat
   */
  void loadAreaName( const KoXmlElement& element );


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
  virtual bool loadOasis( const KoXmlDocument& doc, KoOasisStyles& oasisStyles,
                          const KoXmlDocument& settings, KoStore* );

  /**
   * \ingroup OpenDocument
   */
  void saveOasisAreaName( KoXmlWriter & xmlWriter );

  /**
   * \ingroup OpenDocument
   */
  void loadOasisAreaName( const KoXmlElement& element );

  /**
   * \ingroup OpenDocument
   */
  void loadOasisCellValidation( const KoXmlElement& body );

  /**
   * \ingroup OpenDocument
   */
  void loadOasisCalculationSettings( const KoXmlElement& body );


  virtual int supportedSpecialFormats() const;

  virtual bool loadChildren( KoStore* _store );

  virtual void addView( KoView *_view );

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
  virtual void paintContent( QPainter & painter, const QRect & rect);

  /**
   * \ingroup Painting
   */
  void paintContent( QPainter & painter, const QRect & rect, Sheet * sheet, bool drawCursor = true );


//   virtual DCOPObject* dcopObject();

  /**
   * Registers a named area.
   * \note The name is valid for the whole document.
   * \param rect the cell range to be named
   * \param name the name of the new area
   * \param sheetName the name of the sheet the area belongs to
   */
  void addAreaName( const QRect& rect, const QString& name, const QString& sheetName );

  /**
   * Returns the list of all registered named areas.
   * \return the list of named areas
   */
  const QList<Reference>& listArea();

  /**
   * Removes a named area.
   * \param name the name of the area to be removed
   */
  void removeArea( const QString& name );

  /**
   * Returns the cell range associated with the area named \p name .
   * \param name the name of the area
   * \return the cell range
   */
  QRect namedArea( const QString& name );

  /**
   * Changes the sheet name of all named areas.
   * \param oldName the old sheet name
   * \param newName the new sheet name
   */
  void changeAreaSheetName( const QString& oldName, const QString& newName );

  KCompletion& completion();
  void addStringCompletion(const QString & stringCompletion);

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

  void updateBorderButton();

  void insertSheet( Sheet * sheet );
  void takeSheet( Sheet * sheet );


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
  LoadingInfo* loadingInfo() const;

  /**
   * \ingroup OpenDocument
   * \ingroup NativeFormat
   * Deletes the loading info. Called after loading is complete.
   */
  void deleteLoadingInfo();

/* Function specific when we load config from file */
  void loadConfigFromFile();
  bool configLoadFromFile() const;

  void setCaptureAllArrowKeys( bool capture );
  bool captureAllArrowKeys() const;

  // repaint (update) all views
  void repaint( EmbeddedObject* );
  void repaint( const QRectF& );

#if 0 // UNDOREDOLIMIT
  int undoRedoLimit() const;
  void setUndoRedoLimit(int _val);
#endif

    /**
     * Sets the reference year.
     * All two-digit-year dates are stored as numbers relative to a reference year.
     *
     * \param year the new reference year
     */
    void setReferenceYear( int year );

    /**
     * Returns the reference year all two-digit-year dates are stored relative to.
     *
     * \return the reference year (default: 1930)
     */
    int referenceYear() const;

    /**
     * Sets the reference date.
     * All dates are stored as numbers relative to a reference date.
     *
     * \param date the new reference date
     */
    void setReferenceDate( const QDate& date );

    /**
     * Returns the reference date all date are stored relative to.
     *
     * \return the reference date (default: 1899-12-30)
     */
    QDate referenceDate() const;

    /**
     * Sets the default decimal precision.
     * It is used to format decimal numbers, if the cell style does not define
     * one.
     *
     * \param precision the default decimal precision
     */
    void setDefaultDecimalPrecision( int precision );

    /**
     * Returns the default decimal precision, which is used, if the cell style
     * does not define one.
     *
     * \return the default decimal precision
     */
    int defaultDecimalPrecision() const;

public Q_SLOTS:
    void refreshInterface();
    void flushDamages();
    void handleDamages( const QList<Damage*>& damages );

    virtual void initEmpty();

Q_SIGNALS:
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
  virtual void openTemplate( const KUrl& url );

protected:
  KoView* createViewInstance( QWidget* parent );

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
    Q_DISABLE_COPY( Doc )

    class Private;
    Private * const d;

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
  void paintRegion(QPainter& painter, const QRectF &viewRegion,
                   View* view, const QRect &paintRegion,
                   const Sheet* sheet);

  void loadPaper( KoXmlElement const & paper );

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
  void loadOasisSettings( const KoXmlDocument&settingsDoc );

  /**
   * \ingroup OpenDocument
   * Load the spell checker ignore list.
   */
  void loadOasisIgnoreList( const KoOasisSettings& settings );
};

} // namespace KSpread

#endif /* KSPREAD_DOC */
