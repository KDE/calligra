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

#include <koFrame.h>
#include <koDocument.h>
#include <koPrintExt.h>

#include <iostream.h>

#include <qobject.h>
#include <qrect.h>
#include <qstring.h>
#include <qpainter.h>
#include <qpen.h>
#include <qdom.h>

#include "kspread.h"
#include "kspread_undo.h"
#include "kspread_view.h"
#include "kspread_map.h"
#include "kspread_interpreter.h"

#include <koPageLayoutDia.h>

class QIODevice;

/**
 * This class holds the data that makes up a spreadsheet.
 */
class KSpreadDoc : public QObject,
		   virtual public KoDocument,
		   virtual public KoPrintExt,
		   virtual public KSpread::Document_skel
{
  Q_OBJECT
public:
  // C++
  KSpreadDoc();
  ~KSpreadDoc();

  // C++
  virtual bool save( QIODevice*, KOStore::Store_ptr, const char *_format );

  // C++
  virtual bool loadChildren( KOStore::Store_ptr _store );
  virtual bool loadXML( const QDOM::Document&, KOStore::Store_ptr );
  
  virtual void cleanUp();

  virtual void removeView( KSpreadView* _view );

  // C++
  virtual KSpreadView* createSpreadView( QWidget* _parent = 0 );

  // IDL
  virtual CORBA::Boolean initDoc();

  virtual KSpread::Book_ptr book();

  /**
   * Wrapper for @ref #createSpreadView
   */
  virtual OpenParts::View_ptr createView();
  
  virtual void viewList( OpenParts::Document::ViewList*& _list );
  
  virtual char* mimeType() { return CORBA::string_dup( "application/x-kspread" ); }
  
  virtual CORBA::Boolean isModified() { return m_bModified; }

  virtual KOffice::MainWindow_ptr createMainWindow();
  
  // C++
  virtual int viewCount();
  
  // C++
  virtual void setModified( bool _c ) { m_bModified = _c; if ( _c ) m_bEmpty = false; }
  virtual bool isEmpty() { return m_bEmpty; }
  
  // C++
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
  
  KSpreadMap *map() { return m_pMap; }
  
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
  
  QString headLeft( int _p, const QString& _t  ) { if ( m_headLeft.isEmpty() ) return QString::null;
    return completeHeading( m_headLeft, _p, _t ); }
  QString headMid( int _p, const QString& _t ) { if ( m_headMid.isEmpty() ) return QString::null;
    return completeHeading( m_headMid, _p, _t ); }
  QString headRight( int _p, const char *_t ) { if ( m_headRight.isEmpty() ) return QString::null;
    return completeHeading( m_headRight, _p, _t ); }
  QString footLeft( int _p, const char *_t ) { if ( m_footLeft.isEmpty() ) return QString::null;
    return completeHeading( m_footLeft, _p, _t ); }
  QString footMid( int _p, const char *_t ) { if ( m_footMid.isEmpty() ) return QString::null;
    return completeHeading( m_footMid, _p, _t ); }
  QString footRight( int _p, const char *_t ) { if ( m_footRight.isEmpty() ) return QString::null;
    return completeHeading( m_footRight, _p, _t ); }
  
  QString headLeft() { return m_headLeft; }
  QString headMid() { return m_headMid; }
  QString headRight() { return m_headRight; }
  QString footLeft() { return m_footLeft; }
  QString footMid() { return m_footMid; }
  QString footRight() { return m_footRight; }
  
  void setHeadFootLine( const QString& _headl, const QString& _headm, const QString& _headr,
			const QString& _footl, const QString& _footm, const QString& _footr );
    
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
  
  virtual void printMap( QPainter &_painter );
  
  void enableUndo( bool _b );
  void enableRedo( bool _b );

  /**
   * @return TRUE if the document is currently loading.
   */
  bool isLoading() { return m_bLoading; }

  const QPen& defaultGridPen() { return m_defaultGridPen; }

public slots:
  /**
   * Open a dialog for the "Page Layout".
   *
   * @see KoPageLayoutDia
   */
   void paperLayoutDlg();
  
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
  /**
   * Needed for the printing extension KOffice::Print
   */
  virtual void draw( QPaintDevice*, CORBA::Long _width, CORBA::Long _height,
		     CORBA::Float _scale );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool completeLoading( KOStore::Store_ptr );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual void makeChildListIntern( KOffice::Document_ptr _root, const char *_path );
  
  /**
   * @return true if one of the direct children wants to
   *              be saved embedded. If there are no children or if
   *              every direct child saves itself into its own file
   *              then false is returned.
   * 
   */
  virtual bool hasToWriteMultipart();
  
  /**
   * Initializes the KScript Interpreter.
   */
  void initInterpreter();
  /**
   * Destroys the interpreter.
   */
  void destroyInterpreter();
  /**
   * @return the full qualified filenames of scripts in
   *         the requested path.
   */
  QStringList findScripts( const QString& path );

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
  QString completeHeading( const char *_data, int _page, const char *_KSpreadTable );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual const char* copyright() { return "kspread (c) Torben Weis, <weis@kde.org> 1998,1999"; }

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
   * Indicates wether the user should save the document before deleting it.
   *
   * @see #isModified
   */
  bool m_bModified;
  /**
   * TRUE if the document is empty.
   */
  bool m_bEmpty;
  
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
   * All views associetaed with this document.
   */
  QList<KSpreadView> m_lstViews;
  
  /**
   * TRUE if loading is in process, otherwise FALSE.
   * This flag is used to avoid updates etc. during loading.
   */
  bool m_bLoading;

  QPen m_defaultGridPen;
};

#endif

