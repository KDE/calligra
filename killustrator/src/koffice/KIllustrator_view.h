#ifndef KIllustrator_view_h_
#define KIllustrator_view_h_

#include <part_frame_impl.h>
#include <view_impl.h>
#include <document_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>

#include <qlist.h>

#include "KIllustrator.h"
#include "KIllustrator_doc.h"

#include "MainView.h"
#include "CommandHistory.h"

class KIllustratorDocument;
class Canvas;
class GDocument;
class QwViewport;
class TransformationDialog;
class ToolPalette;
class ColorSelectionPalette;
class ToolController;
class Canvas;
class Ruler;

class KIllustratorChildFrame : public PartFrame_impl {
  Q_OBJECT
public:
  KIllustratorChildFrame (KIllustratorView* view, KIllustratorChild* child);
  KIllustratorChild* child () { return m_pChild; }
  KIllustratorView* view () { return m_pView; }

protected:
  KIllustratorChild *m_pChild;
  KIllustratorView *m_pView;
};

class KIllustratorView : public QWidget, public MainView,
			 virtual public View_impl,
			 virtual public KIllustrator::View_skel {
  Q_OBJECT
public:
  KIllustratorView (QWidget* parent, const char* name = 0L, 
		    KIllustratorDocument* doc = 0L);
  ~KIllustratorView ();

  void createGUI ();
  void construct ();
  void cleanUp ();

  GDocument* activeDocument () { return m_pDoc; }
  Canvas* getCanvas () { return canvas; }

  // --- IDL ---
  void newView ();
  void setMode (OPParts::Part::Mode mode);
  void setFocus (CORBA::Boolean mode);
  CORBA::Boolean printDlg ();

  void editUndo ();
  void editRedo ();
  void editCut ();
  void editCopy ();
  void editPaste ();
  void editSelectAll ();
  void editDelete ();
  void editInsertOject ();
  void editProperties ();

protected:
  void setupMenu ();
  void setupCanvas ();
  void resizeEvent (QResizeEvent*);

  OPParts::MenuBarFactory_var m_vMenuBarFactory;
  MenuBar_ref m_rMenuBar;
  /* Menu: Edit */
  CORBA::Long m_idMenuEdit, m_idMenuEdit_Undo, m_idMenuEdit_Redo,
    m_idMenuEdit_Cut, m_idMenuEdit_Copy, m_idMenuEdit_Paste, 
    m_idMenuEdit_Delete, m_idMenuEdit_SelectAll, 
    m_idMenuEdit_InsertObject, m_idMenuEdit_Properties;
  /* Menu: View */
  CORBA::Long m_idMenuView, m_idMenuView_Outline, m_idMenuView_Draft,
    m_idMenuView_Normal, m_idMenuView_Ruler, m_idMenuView_Grid;
  /* Menu: Layout */
  CORBA::Long m_idMenuLayout, m_idMenuLayout_InsertPage, 
    m_idMenuLayout_RemovePage, m_idMenuLayout_GotoPage, 
    m_idMenuLayout_PageLayout, m_idMenuLayout_Layers, 
    m_idMenuLayout_SetupGrid, m_idMenuLayout_AlignToGrid;
  /* Menu: Arrange */
  CORBA::Long m_idMenuArrange, m_idMenuTransform, m_idMenuTransform_Position,
    m_idMenuTransform_Dimension, m_idMenuTranform_Rotation, 
    m_idMenuTransform_Mirror, m_idMenuArrange_Align, 
    m_idMenuArrange_ToFront, m_idMenuArrange_ToBack, 
    m_idMenuArrange_1Forward, m_idMenuArrange_1Back, 
    m_idMenuArrange_Group, m_idMenuArrange_Ungroup;
  /* Menu: Extras */
  CORBA::Long m_idMenuExtras;
  /* Menu: Help */
  CORBA::Long m_idMenuHelp_About;

  Part_impl *m_pPart;
  KIllustratorDocument *m_pDoc;

  QList<KIllustratorChildFrame> m_lstFrames;

  bool m_bShowGUI;

  ToolPalette *tpalette;
  ColorSelectionPalette *cpalette;
  ToolController *tcontroller;
  QwViewport *viewport;
  Canvas *canvas;
  Ruler *hRuler, *vRuler;
  TransformationDialog *transformationDialog;
  QWidget *mainWidget;

  CommandHistory cmdHistory;
};

#endif
