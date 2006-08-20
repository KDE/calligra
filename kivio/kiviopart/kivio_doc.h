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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef __kivio_doc_h__
#define __kivio_doc_h__

#include "kivio_grid_data.h"

#include <qptrlist.h>
#include <qobject.h>

#include <KoDocument.h>
#include <KoMainWindow.h>
#include <KoGlobal.h>

#include "kivio_config.h"
#include <koffice_export.h>
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

class KIVIO_EXPORT KivioDoc : public KoDocument
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

    virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget=0);

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
    void addSpawnerSetDuringLoad(const QString& dirName, bool hidden);
    bool removeSpawnerSet( KivioStencilSpawnerSet * );

    QPtrList<KivioStencilSpawnerSet> *spawnerSets()const { return m_pLstSpawnerSets; }

    KivioStencilSpawner *findStencilSpawner( const QString& setId, const QString& stencilId );
    KivioStencilSpawner *findInternalStencilSpawner( const QString& title );

    void addInternalStencilSpawner(KivioStencilSpawner* spawner);
    KivioStencilSpawnerSet* internalSpawnerSet() { return m_pInternalSet; }

    KivioMap* map() const { return m_pMap; }

    /**
    * @return true if the document is currently loading.
    */
    bool isLoading()const { return m_bLoading; }

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );
    void paintContent(KivioPainter& painter, const QRect& rect, bool transparent, KivioPage* page,
                      QPoint, KoZoomHandler*, bool, bool);

    void printContent( KPrinter& prn );

    static QPtrList<KivioDoc>& documents();

    KivioGridData grid() { return Kivio::Config::grid(); }

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

    void setDefaultFont(const QFont& f) { m_font = f; }

    void updateGuideLines(KoView* sender);
    void updateGuideLines(KivioPage* page);

    virtual void initEmpty();

  protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();
    void loadStencil();

    virtual void openExistingFile( const QString& file );
    virtual void openTemplate( const QString& file );

  signals:
    void sig_selectionChanged();
    void sig_addPage(KivioPage*);
    void sig_addSpawnerSet( KivioStencilSpawnerSet * );
    void sig_updateView(KivioPage*);
    void sig_pageNameChanged(KivioPage*, const QString&);
    void sig_deleteStencilSet( DragBarButton*, QWidget *, KivioStackBar * );
    void sig_updateGrid();

    void initProgress();
    void progress(int);
    void endProgress();

    void loadingFinished();

  protected:
    bool checkStencilsForSpawnerSet(KivioStencilSpawnerSet *);
    bool checkGroupForSpawnerSet(KivioStencil *, KivioStencilSpawnerSet *);
    KivioStencilSpawnerSet* findSpawnerSet(const QString& dirName, const QString& name);

    void saveOasisSettings( KoXmlWriter &/*settingsWriter*/ );
    void loadOasisSettings( const QDomDocument&settingsDoc );

    KoView* createViewInstance( QWidget* parent );

    /**
    * List of stencil spawner sets
    */
    QPtrList <KivioStencilSpawnerSet> *m_pLstSpawnerSets;


    /**
    * Loads a KivioStencilSpawnerSet based on it's id
    */
    bool loadStencilSpawnerSet(const QString &id, bool hidden);

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
    * true if loading is in process, otherwise false.
    * This flag is used to avoid updates etc. during loading.
    *
    * @see #isLoading
    */
    bool m_bLoading;

    static QPtrList<KivioDoc>* s_docs;
    static int s_docId;

    KivioStencilSpawnerSet* m_pInternalSet;

    KivioGridData gridData;

    DCOPObject *dcop;
    KoCommandHistory * m_commandHistory;

    QFont m_font;

    QTimer* m_loadTimer;
    unsigned int m_currentFile;
    KivioStencilSpawnerSet* m_currentSet;
    QValueList<KivioStencilSpawnerSet*> m_stencilSetLoadQueue;

    bool m_docOpened; // Used to for a hack that make kivio not crash if you cancel startup dialog.
};

#endif

