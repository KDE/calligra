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

class KoStore;

class View;

class DCOPObject;

class QDomDocument;

#include <koDocument.h>
#include <koPageLayoutDia.h>

#include <iostream.h>

#include <qobject.h>
#include <qrect.h>
#include <qstring.h>
#include <qpainter.h>
#include <qpen.h>
#include <kcompletion.h>

#include "kspread_interpreter.h"
#include "kspread_locale.h"

#include <koscript_context.h>

#define MIME_TYPE "application/x-kspread"

struct Reference
{
    QString table_name;
    QString ref_name;
    QRect rect;
};

/**
 * This class holds the data that makes up a spreadsheet.
 */
class KSpreadDoc : public KoDocument
{
  Q_OBJECT
public:
  KSpreadDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
  ~KSpreadDoc();

  virtual QDomDocument saveXML();

  virtual bool loadChildren( KoStore* _store );
  virtual bool loadXML( QIODevice *, const QDomDocument& doc );
  QDomElement saveAreaName( QDomDocument& doc ) ;
  void loadAreaName( QDomElement& element );


  virtual bool initDoc();

  virtual QCString mimeType() const { return MIME_TYPE; }

  /**
   * @return a pointer to a new KSpreadTable. The KSpreadTable is not added to the map
   *         nor added to the GUI.
   */
  KSpreadTable* createTable();

  /**
   * Adds a KSpreadTable to the GUI and makes it active. In addition the KSpreadTable is
   * added to the map.
   *
   * @see KSpreadView
   * @see KSpreadMap
   */
  void addTable( KSpreadTable *_table );

  KSpreadMap *map() const { return m_pMap; }

  /**
   * @return the locale which was used for creating this document.
   */
  KLocale* locale() { return &m_locale; }
    
  /**
   * @return the printable width of the paper in millimeters.
   */
  float printableWidth() { return m_paperWidth - m_leftBorder - m_rightBorder; }

  /**
   * @return the printable height of the paper in millimeters.
   */
  float printableHeight() { return m_paperHeight - m_topBorder - m_bottomBorder; }

  float paperHeight() { return m_paperHeight; }
  float paperWidth() { return m_paperWidth; }

  /**
   * @return the left border in millimeters
   */
  float leftBorder() { return m_leftBorder; }
  /**
   * @return the right border in millimeters
   */
  float rightBorder() { return m_rightBorder; }
  /**
   * @return the top border in millimeters
   */
  float topBorder() { return m_topBorder; }
  /**
   * @return the bottom border in millimeters
   */
  float bottomBorder() { return m_bottomBorder; }

  /**
   * @return the orientation of the paper.
   */
  KoOrientation orientation() { return m_orientation; }
  /**
   * @return the ascii name of the paper orientation ( like Portrait, Landscape )
   */
  const char* orientationString();

  /**
   * @return the paper format.
   */
  KoFormat paperFormat() { return m_paperFormat; }
  /**
   * @return the ascii name of the paper format ( like A4, Letter etc. )
   */
  QString paperFormatString();

  /**
   * Changes the paper layout and repaints the currently displayed KSpreadTable.
   */
  void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBoder,
                       KoFormat _paper, KoOrientation orientation );
  /**
   * A convenience function using a string as paper format and orientation.
   */
  void setPaperLayout( float _leftBorder, float _topBorder, float _rightBorder, float _bottomBoder,
                       const char* _paper, const char* _orientation );

  QString headLeft( int _p, const QString &_t  ) { if ( m_headLeft.isNull() ) return "";
  return completeHeading( m_headLeft, _p, _t ); }
  QString headMid( int _p, const QString &_t ) { if ( m_headMid.isNull() ) return "";
  return completeHeading( m_headMid, _p, _t ); }
  QString headRight( int _p, const QString &_t ) { if ( m_headRight.isNull() ) return "";
  return completeHeading( m_headRight, _p, _t ); }
  QString footLeft( int _p, const QString &_t ) { if ( m_footLeft.isNull() ) return "";
  return completeHeading( m_footLeft, _p, _t ); }
  QString footMid( int _p, const QString &_t ) { if ( m_footMid.isNull() ) return "";
  return completeHeading( m_footMid, _p, _t ); }
  QString footRight( int _p, const QString &_t ) { if ( m_footRight.isNull() ) return "";
  return completeHeading( m_footRight, _p, _t ); }

  QString headLeft() { if ( m_headLeft.isNull() ) return ""; return m_headLeft; }
  QString headMid() { if ( m_headMid.isNull() ) return ""; return m_headMid; }
  QString headRight() { if ( m_headRight.isNull() ) return ""; return m_headRight; }
  QString footLeft() { if ( m_footLeft.isNull() ) return ""; return m_footLeft; }
  QString footMid() { if ( m_footMid.isNull() ) return ""; return m_footMid; }
  QString footRight() { if ( m_footRight.isNull() ) return ""; return m_footRight; }

  void setHeadFootLine( const QString &_headl, const QString &_headm, const QString &_headr,
                        const QString &_footl, const QString &_footm, const QString &_footr );

  /**
   * @return the KScript Interpreter used by this document.
   */
  KSpreadInterpreter* interpreter() { return m_pInterpreter; }
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
  KSContext& context() { m_context.setException( 0 ); return m_context; }
  /**
   * Searches in all KSpread extension modules for the keyword.
   * The most specific ones are searches first. The modules in the
   * users apps/kspread directory are considered to be more specific
   * then the global ones.
   *
   * @see #kscriptMap
   * @see #kscriptModules;
   */
  KSValue* lookupKeyword( const QString& keyword );
  /**
   * Searches for a class in the kscript module of the KSpread interface.
   * This is needed to create proxies for KSpread objects in the kscript
   * environment.
   */
  KSValue* lookupClass( const QString& name );

  /**
   * Undo the last operation.
   */
  void undo();
  /**
   * Redo the last undo.
   */
  void redo();
  /**
   * @return the object that is respnsible for keeping track
   *         of the undo buffer.
   */
  KSpreadUndo *undoBuffer() { return m_pUndoBuffer; }

    // virtual void printMap( QPainter &_painter );

  void enableUndo( bool _b );
  void enableRedo( bool _b );

  /**
   * @return TRUE if the document is currently loading.
   */
  bool isLoading() { return m_bLoading; }

  void setDefaultGridPen( const QPen& );
  const QPen& defaultGridPen() { return m_defaultGridPen; }

  virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent );
  void paintContent( QPainter& painter, const QRect& rect, bool transparent, KSpreadTable* table );

  virtual DCOPObject* dcopObject();

  static QList<KSpreadDoc>& documents();

  void addAreaName(QRect &_rect,QString name,QString tableName);
  QValueList<Reference> listArea(){return m_refs;}
  void removeArea( QString name);

  KCompletion & completion(){return listCompletion;}
  void addStringCompletion(QString stringCompletion);

  void changeAreaTableName(QString oldName,QString tableName);

public slots:
  /**
   * Open a dialog for the "Page Layout".
   *
   * @see KoPageLayoutDia
   */
   void paperLayoutDlg();

  //void newView(); obsloete (Werner)

signals:
  // Document signals
  /**
   * Emitted if a new table is added to the document.
   */
  void sig_addTable( KSpreadTable *_table );
  /**
   * Emitted if all views have to be updated.
   */
  void sig_updateView();

protected:
  KoView* createViewInstance( QWidget* parent, const char* name );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool completeLoading( KoStore* );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool saveChildren( KoStore* _store, const QString &_path );

  /**
   * Initializes the KScript Interpreter.
   */
  void initInterpreter();
  /**
   * Destroys the interpreter.
   */
  void destroyInterpreter();

  /**
   * Looks at @ref #m_paperFormat and calculates @ref #m_paperWidth and @ref #m_paperHeight.
   */
  void calcPaperSize();

  /**
   * Replaces macros like <name>, <file>, <date> etc. in the string and
   * returns the modified one.
   *
   * @param _page is the page number for which the heading is produced.
   * @param _KSpreadTable is the name of the KSpreadTable for which we generate the headings.
   */
  QString completeHeading( const QString &_data, int _page, const QString &_table );

  /**
   * Pointer to the map that holds all the tables.
   */
  KSpreadMap *m_pMap;

  /**
   * This variable is used to give every KSpreadTable a unique default name.
   *
   * @see #newKSpreadTable
   */
  int m_iTableId;

  /**
   * The orientation of the paper.
   */
  KoOrientation m_orientation;
  /**
   * Tells about the currently seleced paper size.
   */
  KoFormat m_paperFormat;

  /**
   * The paper width in millimeters. Dont change this value, it is calculated by
   * @ref #calcPaperSize from the value @ref #m_paperFormat.
   */
  float m_paperWidth;
  /**
   * The paper height in millimeters. Dont change this value, it is calculated by
   * @ref #calcPaperSize from the value @ref #m_paperFormat.
   */
  float m_paperHeight;
  /**
   * The left border in millimeters.
   */
  float m_leftBorder;
  /**
   * The right border in millimeters.
   */
  float m_rightBorder;
  /**
   * The top border in millimeters.
   */
  float m_topBorder;
  /**
   * The right border in millimeters.
   */
  float m_bottomBorder;

  /**
   * Header string. The string may contains makros. That means
   * it has to be processed before printing.
   */
  QString m_headLeft;
  /**
   * Header string. The string may contains makros. That means
   * it has to be processed before printing.
   */
  QString m_headRight;
  /**
   * Header string. The string may contains makros. That means
   * it has to be processed before printing.
   */
  QString m_headMid;
  /**
   * Footer string. The string may contains makros. That means
   * it has to be processed before printing.
   */
  QString m_footLeft;
  /**
   * Footer string. The string may contains makros. That means
   * it has to be processed before printing.
   */
  QString m_footRight;
  /**
   * Footer string. The string may contains makros. That means
   * it has to be processed before printing.
   */
  QString m_footMid;

  /**
   * The URL of the this part. This variable is only set if the @ref #load function
   * had been called with an URL as argument.
   *
   * @see #load
   */
  QString m_strFileURL;

  /**
   * This variable hold the KScript Interpreter.
   */
  KSpreadInterpreter::Ptr m_pInterpreter;
  /**
   * Maps keywords to their kscript value. These
   * keywords are usually functions. This map is used
   * for quick keyword lookup.
   *
   * @see #lookupKeyword
   */
  QMap<QString,KSValue::Ptr> m_kscriptMap;
  /**
   * This list contains the logical names of all modules
   * which contains KSpread extensions. These modules are
   * located in the apps/kspread directory in the global
   * and the users environment. If a module of the same name
   * exists in both environments, then the most specific one
   * is in this list and the other one is dropped.
   *
   * @see #lookupKeyword
   */
  QStringList m_kscriptModules;
  /**
   * Used for undo.
   */
  KSpreadUndo *m_pUndoBuffer;

  /**
   * TRUE if loading is in process, otherwise FALSE.
   * This flag is used to avoid updates etc. during loading.
   *
   * @see #isLoading
   */
  bool m_bLoading;

  QPen m_defaultGridPen;

  /**
   * This DCOP object represents the document.
   */
  DCOPObject* m_dcop;

  /**
   * This module is used to execute formulas of this table.
   */
  KSModule::Ptr m_module;
  /**
   * This context is used to execute formulas of this table.
   */
  KSContext m_context;

  //KAction* m_newView; obsolete - done in the libs (Werner)

  static QList<KSpreadDoc>* s_docs;
  static int s_docId;

  QValueList<Reference> m_refs;
  KCompletion listCompletion;
    
  KSpreadLocale m_locale;
};

#endif

