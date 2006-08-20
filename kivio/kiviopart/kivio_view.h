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
#ifndef __kivio_gui_h__
#define __kivio_gui_h__

class KivioView;
class KivioCanvas;
class KivioPage;
class KivioDoc;
class KivioPainter;
class KivioPaperLayout;
class KivioShell;
class KivioStackBar;
class KivioStencilSpawner;
class KivioStencilSpawnerSet;
class KivioTabBar;
class TKSelectColorAction;
class StencilBarDockManager;
class KivioArrowHeadAction;

namespace Kivio {
  class ToolController;
  class PluginManager;
  class AddStencilSetDialog;
  class AddStencilTool;
  class ObjectListPalette;
}

class KivioBirdEyePanel;
class KivioLayerPanel;
class KivioProtectionPanel;
class KivioStencilGeometryPanel;

class KoDocumentEntry;
class KoTabBar;

class KAction;
class KFontAction;
class KFontSizeAction;
class KToggleAction;

class KSelectColorAction;
class KActionMenu;

class QStringList;
class QPushButton;
class QProgressBar;
class QScrollBar;
class DCOPObject;
class KoRuler;
class KoZoomHandler;
class KoUnitDoubleSpinBox;
class KStatusBarLabel;
class KoLineWidthAction;
class KoLineStyleAction;
class KoPaletteManager;

#include <qdom.h>
#include <qptrlist.h>
#include <qframe.h>

#include <KoView.h>
#include <KoDocument.h>
#include <KoUnit.h>

#include <koffice_export.h>
using namespace Kivio;

class KIVIO_EXPORT KivioView : public KoView
{
  Q_OBJECT
  friend class KivioCanvas;

  public:
    KivioView( QWidget *_parent, KivioDoc *_doc );
    ~KivioView();

    virtual DCOPObject* dcopObject();

    KivioCanvas* canvasWidget() const { return m_pCanvas; }
    KivioDoc* doc()const { return m_pDoc; }

    KivioPage* activePage() const;

    void setActiveSpawnerSet( KivioStencilSpawnerSet* );
    KivioStencilSpawnerSet *activeSpawnerSet();

    KoTabBar* tabBar()const { return  m_pTabBar;}
    KoPaletteManager* paletteManager() { return m_pPaletteManager; };
    void updateMenuPage( );

    virtual void setupPrinter(KPrinter&);
    virtual void print(KPrinter&);

    virtual QWidget* canvas() const;
    virtual int canvasXOffset() const;
    virtual int canvasYOffset() const;

    bool isSnapGuides()const { return m_bSnapGuides; }
    bool isShowGuides()const { return m_bShowGuides; }
    bool isShowRulers()const { return m_bShowRulers; }
    bool isShowPageMargins()const { return m_bShowPageMargins; }

    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;


    // Returns the current interface color/lineWidth settings
    QColor fgColor()const;
    QColor bgColor()const;
    double lineWidth()const;
    int lineStyle() const;
    void updateButton();
    void insertPage( KivioPage* page );
    void resetLayerPanel();
    void updateProtectPanelCheckBox();

    KoZoomHandler* zoomHandler() const;

    KoRuler* horzRuler() const { return hRuler; }
    KoRuler* vertRuler() const { return vRuler; }

    Kivio::PluginManager* pluginManager();

    int hTextAlign();
    int vTextAlign();

  signals:
    void zoomChanged(int);
    void updateStencilSetList();

  protected:
    void createGeometryDock();
    void createLayerDock();
    void createBirdEyeDock();
    void createProtectionDock();
    void createObjectListPalette();

  public slots:
    void paperLayoutDlg();

    void togglePageMargins(bool);
    void toggleShowRulers(bool);
    void toggleShowGrid(bool);
    void toggleSnapGrid(bool);
    void toggleShowGuides(bool);

    void toggleStencilGeometry(bool);
    void toggleViewManager(bool);

    void addPage( KivioPage* );
    void removePage( KivioPage* );
    void insertPage();
    void removePage();
    void renamePage();
    void hidePage();
    void showPage();
    void viewZoom(int);

    void groupStencils();
    void ungroupStencils();

    void selectAllStencils();
    void unselectAllStencils();

    void bringStencilToFront();
    void sendStencilToBack();

    void addStencilFromSpawner( KivioStencilSpawner *pSpawner, double x = 0.0, double y = 0.0 );

    void changePage( const QString& _name );

    void updateToolBars();

    void cutStencil();
    void copyStencil();
    void pasteStencil();
    /// Called by the delete action
    void deleteObject();

    void alignStencilsDlg();
    void optionsDialog();

    void slotPageHidden( KivioPage* page );
    void slotPageShown( KivioPage* page );

    void setRulerPageLayout(const KoPageLayout& l);

    void popupTabBarMenu( const QPoint& point );

    void initStatusBarProgress();
    void setStatusBarProgress(int progress);
    void removeStatusBarProgress();

    void setActivePage( KivioPage* );

    void setStatusBarInfo(const QString& text);

  protected slots:
    void slotPageRenamed( KivioPage* page, const QString& old_name );
    void slotUpdateView( KivioPage *_page );
    void slotUpdateGrid();

    void setFGColor();
    void setBGColor();
    void setTextColor();

    void setFontFamily( const QString & );
    void setFontSize( int );

    void setLineWidth(double);
    void setLineStyle(int);

    void toggleFontBold(bool);
    void toggleFontItalics(bool);
    void toggleFontUnderline(bool);

    void setHParaAlign( int );
    void setVParaAlign( int );
    void textAlignLeft();
    void textAlignCenter();
    void textAlignRight();
    void textSuperScript();
    void textSubScript();
    void showAlign( int align );
    void showVAlign( int align );

    void slotSetStartArrow( int );
    void slotSetEndArrow( int );

    void slotSetStartArrowSize();
    void slotSetEndArrowSize();

    void slotChangeStencilPosition(double, double);
    void slotChangeStencilSize(double, double);
    void slotChangeStencilRotation(int);

    void viewZoom(const QString& s);

    void addSpawnerToStackBar( KivioStencilSpawnerSet * );
    void addStencilSet( const QString& );

    void setMousePos( int mx, int my );
    void setRulerUnit(KoUnit::Unit);
    void setRulerHOffset(int h);
    void setRulerVOffset(int v);
    void rulerChangedUnit(KoUnit::Unit u);

    void textFormat();
    void stencilFormat();
    void arrowHeadFormat();

    void clipboardDataChanged();

    void installStencilSet();

    /// This slot gets called when the loading has finished (doh!)
    void loadingFinished();

    void moveTab(unsigned tab, unsigned target);

    void addGuideLine();

    void showAddStencilSetDialog();

    void updateRulers();

  protected:
    void setupActions();
    void initActions();

    virtual void updateReadWrite( bool readwrite );
    virtual void partActivateEvent(KParts::PartActivateEvent* event);
    
    void updatePageStatusLabel();

  private:
    KivioCanvas *m_pCanvas;
    KoTabBar *m_pTabBar;

    KAction* m_paperLayout;
    KAction* m_insertPage;
    KAction* m_removePage;
    KAction* m_renamePage;
    KAction* m_hidePage;
    KAction* m_showPage;
    KAction* m_arrowHeadsMenuAction;
    KAction* m_menuTextFormatAction;
    KAction* m_menuStencilConnectorsAction;

    TKSelectColorAction *m_setFGColor;
    TKSelectColorAction *m_setBGColor;

    KFontAction *m_setFontFamily;
    KFontSizeAction *m_setFontSize;
    KToggleAction *m_setBold;
    KToggleAction *m_setItalics;
    KToggleAction *m_setUnderline;
    TKSelectColorAction *m_setTextColor;
    KToggleAction* m_textAlignLeft;
    KToggleAction* m_textAlignCenter;
    KToggleAction* m_textAlignRight;
    KToggleAction* m_textVAlignSuper;
    KToggleAction* m_textVAlignSub;

    KivioArrowHeadAction* m_setArrowHeads;

    QStringList m_lineWidthList;

    KivioDoc* m_pDoc;
    KivioPage* m_pActivePage;
    KivioStencilSpawnerSet* m_pActiveSpawnerSet;

    StencilBarDockManager* m_pStencilBarDockManager;
    KoPaletteManager* m_pPaletteManager;

    KoRuler* vRuler;
    KoRuler* hRuler;

    KivioStencilGeometryPanel* m_pStencilGeometryPanel;
    KivioLayerPanel* m_pLayersPanel;
    KivioBirdEyePanel* m_pBirdEyePanel;
    KivioProtectionPanel* m_pProtectionPanel;
    Kivio::ObjectListPalette* m_objectListPalette;
    KToggleAction* showPageMargins;
    KToggleAction* showRulers;
    KToggleAction* showGrid;
    KToggleAction* showGuides;
    KAction *m_selectAll;
    KAction *m_selectNone;
    KAction *m_editCopy;
    KAction* m_editCut;
    KAction* m_editPaste;
    KAction* m_editDelete;
    bool m_bShowPageMargins;
    bool m_bShowRulers;
    bool m_bSnapGuides;
    bool m_bShowGuides;

    DCOPObject *dcop;

    KoZoomHandler* m_zoomHandler;

    KStatusBarLabel* m_coordSLbl;
    KStatusBarLabel* m_pageCountSLbl;
    KStatusBarLabel* m_infoSLbl;

    Kivio::PluginManager* m_pluginManager;

    KAction* m_groupAction;
    KAction* m_ungroupAction;
    KAction* m_stencilToFront;
    KAction* m_stencilToBack;
    KAction* m_alignAndDistribute;
    KoLineWidthAction* m_lineWidthAction;
    KoLineStyleAction* m_lineStyleAction;

    QProgressBar* m_statusBarProgress;

    QScrollBar* m_vertScrollBar;
    QScrollBar* m_horzScrollBar;
    Kivio::AddStencilTool* m_addStencilTool;

    Kivio::AddStencilSetDialog* m_addStencilSetDialog;
};

#endif
