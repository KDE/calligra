/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef __kivio_doc_h__
#define __kivio_doc_h__

class KivioDoc;
class KivioView;
class KivioMap;
class KivioPage;
class KivioOptions;

class ExportPageDialog;
class KivioGroupStencil;
class KivioStencilSpawner;
class KivioStencilSpawnerSet;
class KivioStackBar;
class DragBarButton;
class KivioPainter;
class KivioStencil;
class ViewItemList;

class KoStore;
class View;
class QDomDocument;

#include <koDocument.h>
#include <koMainWindow.h>
#include "kivio_grid_data.h"

#include <iostream.h>

#include <qobject.h>
#include <qrect.h>
#include <qstring.h>
#include <qpainter.h>
#include <qpen.h>
#include <qlist.h>
#include <qprinter.h>


#define MIME_TYPE "application/x-kivio"

class KivioDoc : public KoDocument
{ Q_OBJECT
friend class KivioPage;
public:
  KivioDoc( QWidget *parentWidget = 0, const char* widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
  ~KivioDoc();

  virtual void addShell(KoMainWindow *shell);

  virtual QDomDocument saveXML();

  virtual bool loadXML( QIODevice *, const QDomDocument& doc );

  virtual bool initDoc();

  KivioOptions* config() { return m_options; }

  virtual QCString mimeType() const { return MIME_TYPE; }

  /**
   * @return a pointer to a new KivioPage. The KivioPage is not added to the map nor added to the GUI.
   */
  KivioPage* createPage();

  /**
   * Adds a KivioPage to the GUI and makes it active. In addition the KivioPage is
   * added to the map.
   *
   * @see KivioView
   * @see KivioMap
   */
  void addPage( KivioPage* page );

  /**
   * Adds a KivioStencilSpawnerSet to the list of spawner sets and make it active.
   *
   */
  KivioStencilSpawnerSet *addSpawnerSet( QString );
  KivioStencilSpawnerSet *addSpawnerSetDuringLoad( QString );
  bool removeSpawnerSet( KivioStencilSpawnerSet * );

  QList<KivioStencilSpawnerSet> *spawnerSets() { return m_pLstSpawnerSets; }

  KivioStencilSpawner *findStencilSpawner( const QString& setId, const QString& stencilId );
  KivioStencilSpawner *findInternalStencilSpawner( const QString& title );

  KivioMap* map() const { return m_pMap; }

  /**
   * @return TRUE if the document is currently loading.
   */
  bool isLoading() { return m_bLoading; }

  virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent );
  void paintContent( KivioPainter& painter, const QRect& rect, bool transparent, KivioPage* page, QPoint, float, bool );

  void printContent( QPrinter& prn );
  bool exportPage( KivioPage *pPage, const QString &fileName, ExportPageDialog * );

  static QList<KivioDoc>& documents();

  KivioGroupStencil *clipboard();
  void setClipboard( KivioGroupStencil * );

  int units() { return m_units; }

  KivioGridData grid() { return gridData; }
  void setGrid(KivioGridData g) { gridData = g;}

  ViewItemList* viewItems() { return viewItemList; }

public slots:
  void updateView(KivioPage*, bool modified=true);

  void slotDeleteStencilSet( DragBarButton *, QWidget *, KivioStackBar * );
  void slotSelectionChanged();
  void setUnits(int);

  void aboutKivio();
  void aboutGetStencilSets();

signals:
  void sig_selectionChanged();
  void sig_addPage(KivioPage*);
  void sig_addSpawnerSet( KivioStencilSpawnerSet * );
  void sig_updateView(KivioPage*);
  void sig_pageNameChanged(KivioPage*, const QString&);
  void sig_deleteStencilSet( DragBarButton*, QWidget *, KivioStackBar * );

  void unitsChanged(int);

protected:
  bool checkStencilsForSpawner( KivioStencilSpawner * );
  bool checkGroupForSpawner( KivioStencil *, KivioStencilSpawner *);
  bool setIsAlreadyLoaded( QString dirName, QString name );

  KoView* createViewInstance( QWidget* parent, const char* name );

  /**
   * List of stencil spawner sets
   */
  QList <KivioStencilSpawnerSet> *m_pLstSpawnerSets;


  /**
   * Loads a KivioStencilSpawnerSet based on it's id
   */
  bool loadStencilSpawnerSet( const QString &id );

  /**
   * Overloaded function of @ref KoDocument.
   */
  virtual bool completeLoading( KoStore* );

  /**
   * Pointer to the map that holds all the pages.
   */
  KivioMap *m_pMap;

  /**
   * This variable is used to give every KivioPage a unique default name.
   *
   * @see #newKivioPage
   */
  int m_iPageId;

  /**
   * The URL of the this part. This variable is only set if the @ref #load function
   * had been called with an URL as argument.
   *
   * @see #load
   */
  QString m_strFileURL;

  /**
   * TRUE if loading is in process, otherwise FALSE.
   * This flag is used to avoid updates etc. during loading.
   *
   * @see #isLoading
   */
  bool m_bLoading;

  KivioGroupStencil *m_pClipboard;

  static QList<KivioDoc>* s_docs;
  static int s_docId;

  KivioStencilSpawnerSet* m_pInternalSet;

  int m_units;
  KivioGridData gridData;

  ViewItemList* viewItemList;

  KivioOptions* m_options;
};

#endif

