/***************************************************************************
                          kudesigner.cpp  -  description
                             -------------------
    begin                : Thu Jun  6 11:31:39 EEST 2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// include files for QT
#include <qdir.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qsize.h>
#include <qradiobutton.h>
#include <qinputdialog.h>

// include files for KDE
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kstdaction.h>
#include <kpopupmenu.h>
#include <kprinter.h>

// application specific includes
#include "kudesigner.h"
#include "kudesignerview.h"
#include "kudesignerdoc.h"
#include "mycanvas.h"
#include "dlgnew.h"
#include "creportitem.h"
#include "clabel.h"
#include "cfield.h"
#include "cspecialfield.h"
#include "ccalcfield.h"
#include "cline.h"
#include "canvbox.h"

#define ID_STATUS_MSG 1

KuDesignerApp::KuDesignerApp(QWidget* , const char* name):KMainWindow(0, name)
{
  config=kapp->config();

  ///////////////////////////////////////////////////////////////////
  // call inits to invoke all other construction parts
  initStatusBar();
  initActions();
  initDocument();
  initView();
	
  readOptions();

  ///////////////////////////////////////////////////////////////////
  // disable actions at startup
  fileSave->setEnabled(false);
  fileSaveAs->setEnabled(false);
  filePrint->setEnabled(false);
  editCut->setEnabled(false);
  editCopy->setEnabled(false);
  editPaste->setEnabled(false);

  disableDocumentActions();

  resize(600, 440);
}

void KuDesignerApp::enableDocumentActions()
{
    sectionsReportHeader->setEnabled(true);
    sectionsReportFooter->setEnabled(true);
    sectionsPageFooter->setEnabled(true);
    sectionsPageHeader->setEnabled(true);
    sectionsDetailHeader->setEnabled(true);
    sectionsDetail->setEnabled(true);
    sectionsDetailFooter->setEnabled(true);

    itemsNothing->setEnabled(true);
    itemsLabel->setEnabled(true);
    itemsField->setEnabled(true);
    itemsSpecial->setEnabled(true);
    itemsCalculated->setEnabled(true);
    itemsLine->setEnabled(true);
}

void KuDesignerApp::disableDocumentActions()
{
    sectionsReportHeader->setEnabled(false);
    sectionsReportFooter->setEnabled(false);
    sectionsPageFooter->setEnabled(false);
    sectionsPageHeader->setEnabled(false);
    sectionsDetailHeader->setEnabled(false);
    sectionsDetail->setEnabled(false);
    sectionsDetailFooter->setEnabled(false);

    itemsNothing->setEnabled(false);
    itemsLabel->setEnabled(false);
    itemsField->setEnabled(false);
    itemsSpecial->setEnabled(false);
    itemsCalculated->setEnabled(false);
    itemsLine->setEnabled(false);
}

KuDesignerApp::~KuDesignerApp()
{

}

void KuDesignerApp::initActions()
{
  fileNewWindow = new KAction(i18n("New &Window"), 0, 0, this, SLOT(slotFileNewWindow()), actionCollection(),"file_new_window");
  fileNew = KStdAction::openNew(this, SLOT(slotFileNew()), actionCollection());
  fileOpen = KStdAction::open(this, SLOT(slotFileOpen()), actionCollection());
  fileOpenRecent = KStdAction::openRecent(this, SLOT(slotFileOpenRecent(const KURL&)), actionCollection());
  fileSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  fileSaveAs = KStdAction::saveAs(this, SLOT(slotFileSaveAs()), actionCollection());
  fileClose = KStdAction::close(this, SLOT(slotFileClose()), actionCollection());
  filePrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  fileQuit = KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  editCut = KStdAction::cut(this, SLOT(slotEditCut()), actionCollection());
  editCopy = KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  editPaste = KStdAction::paste(this, SLOT(slotEditPaste()), actionCollection());
  viewToolBar = KStdAction::showToolbar(this, SLOT(slotViewToolBar()), actionCollection());
  viewStatusBar = KStdAction::showStatusbar(this, SLOT(slotViewStatusBar()), actionCollection());


    sectionsReportHeader = new KAction(i18n("Report Header"), "irh", 0, this,
        SLOT(slotAddReportHeader()), actionCollection(), "rheader");
    sectionsReportFooter = new KAction(i18n("Report Footer"), "irf", 0, this,
        SLOT(slotAddReportFooter()), actionCollection(), "rfooter");
    sectionsPageHeader = new KAction(i18n("Page Header"), "iph", 0, this,
        SLOT(slotAddPageHeader()), actionCollection(), "pheader");
    sectionsPageFooter = new KAction(i18n("Page Footer"), "ipf", 0, this,
        SLOT(slotAddPageFooter()), actionCollection(), "pfooter");
    sectionsDetailHeader = new KAction(i18n("Detail Header"), "idh", 0, this,
        SLOT(slotAddDetailHeader()), actionCollection(), "dheader");
    sectionsDetail = new KAction(i18n("Detail"), "id", 0, this,
        SLOT(slotAddDetail()), actionCollection(), "detail");
    sectionsDetailFooter = new KAction(i18n("Detail Footer"), "idf", 0, this,
        SLOT(slotAddDetailFooter()), actionCollection(), "dfooter");

    itemsNothing = new KRadioAction(i18n("Clear Selection"), "frame_edit", 0, this,
        SLOT(slotAddItemNothing()), actionCollection(), "nothing");
    itemsNothing->setExclusiveGroup("itemsToolBar");
    itemsNothing->setChecked(true);
    itemsLabel = new KRadioAction(i18n("Label"), "frame_text", 0, this,
        SLOT(slotAddItemLabel()), actionCollection(), "label");
    itemsLabel->setExclusiveGroup("itemsToolBar");
    itemsField = new KRadioAction(i18n("Field"), "frame_field", 0, this,
        SLOT(slotAddItemField()), actionCollection(), "field");
    itemsField->setExclusiveGroup("itemsToolBar");
    itemsSpecial = new KRadioAction(i18n("Special Field"), "frame_query", 0, this,
        SLOT(slotAddItemSpecial()), actionCollection(), "special");
    itemsSpecial->setExclusiveGroup("itemsToolBar");
    itemsCalculated = new KRadioAction(i18n("Calculated Field"), "frame_formula", 0, this,
        SLOT(slotAddItemCalculated()), actionCollection(), "calcfield");
    itemsCalculated->setExclusiveGroup("itemsToolBar");
    itemsLine = new KRadioAction(i18n("Line"), "frame_chart", 0, this,
        SLOT(slotAddItemLine()), actionCollection(), "line");
    itemsLine->setExclusiveGroup("itemsToolBar");



    //creating own menu items and toolbars

  fileNewWindow->setToolTip(i18n("Opens a new application window"));
  fileNew->setToolTip(i18n("Creates a new document"));
  fileOpen->setToolTip(i18n("Opens an existing document"));
  fileOpenRecent->setToolTip(i18n("Opens a recently used file"));
  fileSave->setToolTip(i18n("Saves the actual document"));
  fileSaveAs->setToolTip(i18n("Saves the actual document as..."));
  fileClose->setToolTip(i18n("Closes the actual document"));
  filePrint ->setToolTip(i18n("Prints out the actual document"));
  fileQuit->setToolTip(i18n("Quits the application"));
  editCut->setToolTip(i18n("Cuts the selected section and puts it to the clipboard"));
  editCopy->setToolTip(i18n("Copies the selected section to the clipboard"));
  editPaste->setToolTip(i18n("Pastes the clipboard contents to actual position"));
  viewToolBar->setToolTip(i18n("Enables/disables the toolbar"));
  viewStatusBar->setToolTip(i18n("Enables/disables the statusbar"));

  // use the absolute path to your kudesignerui.rc file for testing purpose in createGUI();
  createGUI();

}


void KuDesignerApp::initStatusBar()
{
  ///////////////////////////////////////////////////////////////////
  // STATUSBAR
  // TODO: add your own items you need for displaying current application status.
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG);
}

void KuDesignerApp::initDocument()
{
  doc = new KuDesignerDoc(this);
  doc->setModified(false);
//  doc->newDocument();
}

void KuDesignerApp::initView()
{ 
  ////////////////////////////////////////////////////////////////////
  // create the main widget here that is managed by KTMainWindow's view-region and
  // connect the widget to your document to display document contents.

  view = new KuDesignerView(this);
  doc->addView(view);
  setCentralWidget(view);	
  setCaption(doc->URL().fileName(),false);
  connect(view, SIGNAL(selectedActionProcessed()), this, SLOT(unselectItemAction()));
}

void KuDesignerApp::openDocumentFile(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));

  doc->openDocument( url);
  fileOpenRecent->addURL( url );
  slotStatusMsg(i18n("Ready."));
}


KuDesignerDoc *KuDesignerApp::getDocument() const
{
  return doc;
}

void KuDesignerApp::saveOptions()
{	
  config->setGroup("General Options");
  config->writeEntry("Geometry", size());
  config->writeEntry("Show Toolbar", viewToolBar->isChecked());
  config->writeEntry("Show Statusbar",viewStatusBar->isChecked());
  config->writeEntry("ToolBarPos", (int) toolBar("mainToolBar")->barPos());
  fileOpenRecent->saveEntries(config,"Recent Files");
}


void KuDesignerApp::readOptions()
{
	
  config->setGroup("General Options");

  // bar status settings
  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
  viewToolBar->setChecked(bViewToolbar);
  slotViewToolBar();

  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  viewStatusBar->setChecked(bViewStatusbar);
  slotViewStatusBar();


  // bar position settings
  KToolBar::BarPosition toolBarPos;
  toolBarPos=(KToolBar::BarPosition) config->readNumEntry("ToolBarPos", KToolBar::Top);
  toolBar("mainToolBar")->setBarPos(toolBarPos);
	
  // initialize the recent file list
  fileOpenRecent->loadEntries(config,"Recent Files");

  QSize size=config->readSizeEntry("Geometry");
  if(!size.isEmpty())
  {
    resize(size);
  }
}

void KuDesignerApp::saveProperties(KConfig *_cfg)
{
  if(doc->URL().fileName()!=i18n("Untitled") && !doc->isModified())
  {
    // saving to tempfile not necessary

  }
  else
  {
    KURL url=doc->URL();	
    _cfg->writeEntry("filename", url.url());
    _cfg->writeEntry("modified", doc->isModified());
    QString tempname = kapp->tempSaveName(url.url());
    QString tempurl= KURL::encode_string(tempname);
    KURL _url(tempurl);
    doc->saveDocument(_url);
  }
}


void KuDesignerApp::readProperties(KConfig* _cfg)
{
  QString filename = _cfg->readEntry("filename", "");
  KURL url(filename);
  bool modified = _cfg->readBoolEntry("modified", false);
  if(modified)
  {
    bool canRecover;
    QString tempname = kapp->checkRecoverFile(filename, canRecover);
    KURL _url(tempname);
  	
    if(canRecover)
    {
      doc->openDocument(_url);
      doc->setModified();
      setCaption(_url.fileName(),true);
      QFile::remove(tempname);
    }
  }
  else
  {
    if(!filename.isEmpty())
    {
      doc->openDocument(url);
      setCaption(url.fileName(),false);
    }
  }
}		

bool KuDesignerApp::queryClose()
{
  return doc->saveModified();
}

bool KuDesignerApp::queryExit()
{
  saveOptions();
  return true;
}

/////////////////////////////////////////////////////////////////////
// SLOT IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

void KuDesignerApp::slotFileNewWindow()
{
  slotStatusMsg(i18n("Opening a new application window..."));
	
  KuDesignerApp *new_window= new KuDesignerApp();
  new_window->show();

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFileNew()
{
  slotStatusMsg(i18n("Creating new document..."));

  if(!doc->saveModified())
  {
     // here saving wasn't successful

  }
  else
  {	
      dlgNew *newReport = new dlgNew;
      if ((newReport->exec()) == QDialog::Accepted)
      {
          doc->newDocument(newReport->pSize, newReport->sbTop->value(),
            newReport->sbBottom->value(), newReport->sbLeft->value(),
            newReport->sbRight->value(), newReport->rbLandscape->isOn(),
            newReport->cbPageSize->currentItem());

            for (KuDesignerView *it = doc->pViewList->first(); it; it = doc->pViewList->next())
            {
                it->setCanvas(doc->canvas());
            }
          setCaption(doc->URL().fileName(), false);
          enableDocumentActions();
      }
      else
      {
	  disableDocumentActions();
      }
      delete newReport;

  }

  slotStatusMsg(i18n("Ready."));
  filePrint->setEnabled(true);
  fileSaveAs->setEnabled(true);
}

void KuDesignerApp::slotFileOpen()
{
  slotStatusMsg(i18n("Opening file..."));
	
  if(!doc->saveModified())
  {
     // here saving wasn't successful

  }
  else
  {	
    KURL url=KFileDialog::getOpenURL(QString::null,
        i18n("*|All files"), this, i18n("Open File..."));
    if(!url.isEmpty())
    {
      if (!doc->openDocument(url)) return;
      setCaption(url.fileName(), false);
      fileOpenRecent->addURL( url );
      filePrint->setEnabled(true);
      fileSave->setEnabled(true);
      fileSaveAs->setEnabled(true);
      enableDocumentActions();
    }
    else
    {
	disableDocumentActions();
    }
  }
  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFileOpenRecent(const KURL& url)
{
  slotStatusMsg(i18n("Opening file..."));
	
  if(!doc->saveModified())
  {
     // here saving wasn't successful
  }
  else
  {
    if (!doc->openDocument(url)) return;
    setCaption(url.fileName(), false);
    enableDocumentActions();
    filePrint->setEnabled(true);
    fileSave->setEnabled(true);
    fileSaveAs->setEnabled(true);
  }

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving file..."));
	
  doc->saveDocument(doc->URL());

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFileSaveAs()
{
  slotStatusMsg(i18n("Saving file with a new filename..."));

  KURL url=KFileDialog::getSaveURL(QDir::currentDirPath(),
        i18n("*|All files"), this, i18n("Save as..."));
  if(!url.isEmpty())
  {
    doc->saveDocument(url);
    fileOpenRecent->addURL(url);
    setCaption(url.fileName(),doc->isModified());
    fileSave->setEnabled(true);
  }

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFileClose()
{
  slotStatusMsg(i18n("Closing file..."));
	
  close();

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));

  KPrinter printer;
  printer.setFullPage(true);
  printer.setPageSize(KPrinter::A4);
  printer.setOrientation(KPrinter::Portrait);
  if (printer.setup(this))
  {
    view->print(&printer);
  }

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotFileQuit()
{
  slotStatusMsg(i18n("Exiting..."));
  saveOptions();
  // close the first window, the list makes the next one the first again.
  // This ensures that queryClose() is called on each window to ask for closing
/*  KMainWindow* w;
  if(memberList)
  {
    for(w=memberList->first(); w!=0; w=memberList->first())
    {
      // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
      // the window and the application stay open.
      if(!w->close())
		break;
    }
  }	*/

  close();
}

void KuDesignerApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting selection..."));

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying selection to clipboard..."));

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotEditPaste()
{
  slotStatusMsg(i18n("Inserting clipboard contents..."));

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotViewToolBar()
{
  slotStatusMsg(i18n("Toggling toolbars..."));
  ///////////////////////////////////////////////////////////////////
  // turn Toolbar on or off
  if(!viewToolBar->isChecked())
  {
    toolBar("mainToolBar")->hide();
    toolBar("sectionsToolBar")->hide();
    toolBar("itemsToolBar")->hide();
  }
  else
  {
    toolBar("mainToolBar")->show();
    toolBar("sectionsToolBar")->show();
    toolBar("itemsToolBar")->show();
  }		

  slotStatusMsg(i18n("Ready."));
}

void KuDesignerApp::slotViewStatusBar()
{
  slotStatusMsg(i18n("Toggle the statusbar..."));
  ///////////////////////////////////////////////////////////////////
  //turn Statusbar on or off
  if(!viewStatusBar->isChecked())
  {
    statusBar()->hide();
  }
  else
  {
    statusBar()->show();
  }

  slotStatusMsg(i18n("Ready."));
}


void KuDesignerApp::slotStatusMsg(const QString &text)
{
  ///////////////////////////////////////////////////////////////////
  // change status message permanently
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

/** No descriptions */
void KuDesignerApp::slotAddReportHeader(){
    if (!doc->canvas()->templ->reportHeader)
    {
    	CanvasReportHeader *rh = new CanvasReportHeader(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
		    doc->canvas()->templ->props["LeftMargin"].first.toInt(),
    		50, doc->canvas());
	    doc->canvas()->templ->reportHeader = rh;
    	doc->canvas()->templ->arrangeSections();
    }
}
/** No descriptions */
void KuDesignerApp::slotAddReportFooter(){
    if (!doc->canvas()->templ->reportFooter)
    {
    	CanvasReportFooter *rf = new CanvasReportFooter(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
    		doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, doc->canvas());
    	doc->canvas()->templ->reportFooter = rf;
    	doc->canvas()->templ->arrangeSections();
    }
}
/** No descriptions */
void KuDesignerApp::slotAddPageHeader(){
    if (!doc->canvas()->templ->pageHeader)
    {
    	CanvasPageHeader *ph = new CanvasPageHeader(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
    		doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, doc->canvas());
	    doc->canvas()->templ->pageHeader = ph;
    	doc->canvas()->templ->arrangeSections();
    }
}
/** No descriptions */
void KuDesignerApp::slotAddPageFooter(){
    if (!doc->canvas()->templ->pageFooter)
    {
    	CanvasPageFooter *pf = new CanvasPageFooter(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
    		doc->canvas()->templ->props["LeftMargin"].first.toInt(),
    		50, doc->canvas());
    	doc->canvas()->templ->pageFooter = pf;
    	doc->canvas()->templ->arrangeSections();
    }
}
/** No descriptions */
void KuDesignerApp::slotAddDetailHeader(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add detail header"), i18n("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (doc->canvas()->templ->detailsCount >= level)
    {
    	CanvasDetailHeader *dh = new CanvasDetailHeader(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
    		doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, doc->canvas());
    	dh->props["Level"].first = QString("%1").arg(level);
	    doc->canvas()->templ->details[level].first.first = dh;
    	doc->canvas()->templ->arrangeSections();
    }
}
/** No descriptions */
void KuDesignerApp::slotAddDetail(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add detail"), i18n("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if ( ((level == 0) && (doc->canvas()->templ->detailsCount == 0))
    	|| (doc->canvas()->templ->detailsCount == level))
    {
    	CanvasDetail *d = new CanvasDetail(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
        	doc->canvas()->templ->props["LeftMargin"].first.toInt(),
    		50, doc->canvas());
    	d->props["Level"].first = QString("%1").arg(level);
    	doc->canvas()->templ->details[level].second = d;
    	doc->canvas()->templ->arrangeSections();
    	doc->canvas()->templ->detailsCount++;
    }
}
/** No descriptions */
void KuDesignerApp::slotAddDetailFooter(){
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger(i18n("Add detail footer"), i18n("Enter detail level:"),
				    0, 0, 100, 1, &Ok, this);
    if (!Ok) return;
    if (doc->canvas()->templ->detailsCount >= level)
    {
    	CanvasDetailFooter *df = new CanvasDetailFooter(doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	0, doc->canvas()->templ->width() - doc->canvas()->templ->props["RightMargin"].first.toInt() -
    		doc->canvas()->templ->props["LeftMargin"].first.toInt(),
	    	50, doc->canvas());
    	df->props["Level"].first = QString("%1").arg(level);
	    doc->canvas()->templ->details[level].first.second = df;
    	doc->canvas()->templ->arrangeSections();
    }
}
/** No descriptions */
void KuDesignerApp::slotAddItemNothing(){
    if (doc->canvas())
    {
        if (view->selectedItem)
        {
            delete view->selectedItem;
            view->selectedItem = 0;
        }
    }
}
/** No descriptions */
void KuDesignerApp::slotAddItemLabel(){
    if (doc->canvas())
    {
        if (view->selectedItem)
            delete view->selectedItem;
	    CanvasLabel *l = new CanvasLabel(0, 0, 50, 20, doc->canvas());
    	view->selectedItem = l;
    }
}
/** No descriptions */
void KuDesignerApp::slotAddItemField(){
    if (doc->canvas())
    {
        if (view->selectedItem)
            delete view->selectedItem;
	    CanvasField *l = new CanvasField(0, 0, 50, 20, doc->canvas());
    	view->selectedItem = l;
    }
}
/** No descriptions */
void KuDesignerApp::slotAddItemSpecial(){
    if (doc->canvas())
    {
        if (view->selectedItem)
            delete view->selectedItem;
	    CanvasSpecialField *l = new CanvasSpecialField(0, 0, 50, 20, doc->canvas());
    	view->selectedItem = l;
    }
}
/** No descriptions */
void KuDesignerApp::slotAddItemCalculated(){
    if (doc->canvas())
    {
        if (view->selectedItem)
            delete view->selectedItem;
	    CanvasCalculatedField *l = new CanvasCalculatedField(0, 0, 50, 20, doc->canvas());
    	view->selectedItem = l;
    }
}
/** No descriptions */
void KuDesignerApp::slotAddItemLine(){
    if (doc->canvas())
    {
        if (view->selectedItem)
            delete view->selectedItem;
	    CanvasLine *l = new CanvasLine(0, 0, 50, 20, doc->canvas());
    	view->selectedItem = l;
    }
}
/** No descriptions */
void KuDesignerApp::unselectItemAction(){
    itemsNothing->setChecked(true);
}
#include "kudesigner.moc"
