/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2005 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

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
#include <KoXmlReader.h>
#include <KoGenStyle.h>

#include "Global.h"

class QDomDocument;
class QPainter;

class KCompletion;

class KoGenStyles;
class KoOasisSettings;
class KoStore;
class KoXmlWriter;
class KoDataCenter;

#define MIME_TYPE "application/x-kspread"

namespace KSpread
{
class Sheet;
class Doc;
class View;
class Map;
class Region;
class UndoAction;
class LoadingInfo;

/**
 * This class holds the data that makes up a spreadsheet.
 */
class KSPREAD_EXPORT Doc : public KoDocument
{
  Q_OBJECT
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
    QMap<QString, KoDataCenter*> dataCenterMap();

  /**
   * @return the Map that belongs to this Document
   */
  Map *map () const;

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
   */
  bool saveOasisHelper( SavingContext &documentContext, SaveFlag saveFlag,
                        QString* plainText = 0 );

  /**
   * \ingroup OpenDocument
   * Main saving method.
   */
  virtual bool saveOdf( SavingContext &documentContext );

  /**
   * \ingroup OpenDocument
   * Main loading method.
   * @see Map::loadOasis
   */
  virtual bool loadOdf( KoOdfReadStore & odfStore );

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

  KCompletion& completion();
  void addStringCompletion(const QString & stringCompletion);

  void initConfig();
  void saveConfig();
  void refreshLocale();

  void updateBorderButton();

    void addIgnoreWordAll( const QString & word);
    void clearIgnoreWordAll( );
    void addIgnoreWordAllList( const QStringList & _lst);
    QStringList spellListIgnoreAll() const ;

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
  void repaint( const QRectF& );

#if 0 // UNDOREDOLIMIT
  int undoRedoLimit() const;
  void setUndoRedoLimit(int _val);
#endif

public Q_SLOTS:
    void refreshInterface();

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

protected Q_SLOTS:
  virtual void openTemplate( const KUrl& url );

protected:
  KoView* createViewInstance( QWidget* parent );

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
