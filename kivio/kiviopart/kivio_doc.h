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

#include "kivio_grid_data.h"

#include <qptrlist.h>
#include <qobject.h>

#include <koDocument.h>
#include <koMainWindow.h>
#include <koUnit.h>
#include <koGlobal.h>

#include "kivio_config.h"

class KivioDoc;
class KivioView;
class KivioMap;
class KivioPage;

class ExportPageDialog;
class KivioGroupStencil;
class KivioStencilSpawner;
class KivioStencilSpawnerSet;
class KivioStackBar;
class KivioPainter;
class KivioStencil;

namespace Kivio {
  class DragBarButton;
  class ViewItemList;
}

class KoStore;
class QDomDocument;
class DCOPObject;
class KoCommandHistory;

class QPainter;
class KPrinter;
class KCommand;
class KoZoomHandler;
class QFont;

#define MIME_TYPE "application/x-kivio"

using namespace Kivio;

class KivioDoc : public KoDocument
{
  Q_OBJECT
  friend class KivioPage;
  public:
    KivioDoc( QWidget *parentWidget = 0, const char* widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KivioDoc();

    virtual DCOPObject* dcopObject();

    virtual QDomDocument saveXML();
    virtual bool saveOasis(KoStore* store, KoXmlWriter* manifestWriter);

    virtual bool loadXML( QIODevice *, const QDomDocument& doc );
    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* );

    virtual bool initDoc();

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
    void addSpawnerSet( const QString& );
    void addSpawnerSetDuringLoad( const QString& );
    bool removeSpawnerSet( KivioStencilSpawnerSet * );

    QPtrList<KivioStencilSpawnerSet> *spawnerSets()const { return m_pLstSpawnerSets; }

    KivioStencilSpawner *findStencilSpawner( const QString& setId, const QString& stencilId );
    KivioStencilSpawner *findInternalStencilSpawner( const QString& title );

    KivioMap* map() const { return m_pMap; }

    /**
    * @return TRUE if the document is currently loading.
    */
    bool isLoading()const { return m_bLoading; }

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );
    void paintContent( KivioPainter& painter, const QRect& rect, bool transparent, KivioPage* page, QPoint, KoZoomHandler*, bool );

    void printContent( KPrinter& prn );
    bool exportPage( KivioPage *pPage, const QString &fileName, ExportPageDialog * );

    static QPtrList<KivioDoc>& documents();

    KoUnit::Unit units()const { return m_units; }

    KivioGridData grid() { return Kivio::Config::grid(); }

    ViewItemList* viewItems()const { return viewItemList; }

    void initConfig();
    void saveConfig();
    void updateButton();
    void addCommand( KCommand * cmd );

    int undoRedoLimit() const;
    void setUndoRedoLimit(int val);
    KoCommandHistory * commandHistory()const { return m_commandHistory; }

    void insertPage( KivioPage * page );
    void takePage( KivioPage * page );
    void resetLayerPanel();
    void updateProtectPanelCheckBox();

    QFont defaultFont() const { return m_font; }

  public slots:
    void updateView(KivioPage*);

    void slotDeleteStencilSet( DragBarButton *, QWidget *, KivioStackBar * );
    void slotSelectionChanged();
    void setUnits(KoUnit::Unit);

    void setDefaultFont(const QFont& f) { m_font = f; }

  protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void loadStencil();

  signals:
    void sig_selectionChanged();
    void sig_addPage(KivioPage*);
    void sig_addSpawnerSet( KivioStencilSpawnerSet * );
    void sig_updateView(KivioPage*);
    void sig_pageNameChanged(KivioPage*, const QString&);
    void sig_deleteStencilSet( DragBarButton*, QWidget *, KivioStackBar * );
    void sig_updateGrid();

    void unitsChanged(KoUnit::Unit);

    void initProgress();
    void progress(int);
    void endProgress();

  protected:
    bool checkStencilsForSpawner( KivioStencilSpawner * );
    bool checkGroupForSpawner( KivioStencil *, KivioStencilSpawner *);
    bool setIsAlreadyLoaded( QString dirName, QString name );

    void saveOasisSettings( KoXmlWriter &/*settingsWriter*/ );
    void loadOasisSettings( const QDomDocument&settingsDoc );

    KoView* createViewInstance( QWidget* parent, const char* name );

    /**
    * List of stencil spawner sets
    */
    QPtrList <KivioStencilSpawnerSet> *m_pLstSpawnerSets;


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

    static QPtrList<KivioDoc>* s_docs;
    static int s_docId;

    KivioStencilSpawnerSet* m_pInternalSet;

    KoUnit::Unit m_units;
    KivioGridData gridData;

    ViewItemList* viewItemList;

    DCOPObject *dcop;
    KoCommandHistory * m_commandHistory;

    QFont m_font;

    QTimer* m_loadTimer;
    unsigned int m_currentFile;
    KivioStencilSpawnerSet* m_currentSet;
    QValueList<KivioStencilSpawnerSet*> m_stencilSetLoadQueue;
};

#endif

