/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Frame Dialog                                           */
/******************************************************************/

#include "kword_doc.h"
#include "kword_page.h"
#include "frame.h"
#include "framedia.h"
#include "framedia.moc"

/******************************************************************/
/* Class: KWFrameDia                                              */
/******************************************************************/

/*================================================================*/
KWFrameDia::KWFrameDia(QWidget* parent,const char* name,KWFrameSet *_frameset,KWFrame *_frame,KWordDocument *_doc,KWPage *_page,int _flags)
  : QTabDialog(parent,name,true)
{
  frameset = _frameset;
  frame = _frame;
  KRect r = frame->normalize();
  frame->setRect(r.x(),r.y(),r.width(),r.height());
  flags = _flags;
  doc = _doc;
  page = _page;
  
  if (flags & FD_FRAME_CONNECT && doc)
    setupTab3ConnectTextFrames();

  if ((flags & FD_FRAME_SET) && frameset && frameset->getFrameType() == FT_TEXT)
    setupTab1TextFrameSet();
  
  if ((flags & FD_FRAME && frame && (!frameset || frameset && frameset->getFrameType() == FT_TEXT)))
    setupTab2TextFrame();
    
  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));

  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(applyChanges()));
}

/*================================================================*/
void KWFrameDia::setupTab1TextFrameSet()
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,2,1,15,7);

  cAutoCreateFrame = new QCheckBox(i18n("&If the text of the frameset doesn't fit into the the frameset anymore \n"
					"(not enough frames), automatically create a new frame."),tab1);
  cAutoCreateFrame->resize(cAutoCreateFrame->sizeHint());
  grid1->addWidget(cAutoCreateFrame,0,0);

  grid1->addColSpacing(0,cAutoCreateFrame->width());
  grid1->setColStretch(0,1);

  grid1->addRowSpacing(0,cAutoCreateFrame->height());
  grid1->setRowStretch(1,1);

  grid1->activate();

  addTab(tab1,i18n("Frameset"));

  cAutoCreateFrame->setChecked(dynamic_cast<KWTextFrameSet*>(frameset)->getAutoCreateNewFrame());
}

/*================================================================*/
void KWFrameDia::setupTab2TextFrame()
{
  tab2 = new QWidget(this);

  grid2 = new QGridLayout(tab2,3,2,15,7);

  runGroup = new QGroupBox(i18n("Text Run Around"),tab2);
  grid2->addWidget(runGroup,0,0);

  runGrid = new QGridLayout(runGroup,5,2,15,7);

  QPixmap pixmap = Icon("run_not.xpm");
  lRunNo = new QLabel(runGroup);
  lRunNo->setBackgroundPixmap(pixmap);
  lRunNo->resize(pixmap.size());
  runGrid->addWidget(lRunNo,1,0);

  pixmap = Icon("run_bounding.xpm");
  lRunBounding = new QLabel(runGroup);
  lRunBounding->setBackgroundPixmap(pixmap);
  lRunBounding->resize(pixmap.size());
  runGrid->addWidget(lRunBounding,2,0);

  pixmap = Icon("run_contur.xpm");
  lRunContur = new QLabel(runGroup);
  lRunContur->setBackgroundPixmap(pixmap);
  lRunContur->resize(pixmap.size());
  runGrid->addWidget(lRunContur,3,0);

  rRunNo = new QRadioButton(i18n("&Don't run around other frames"),runGroup);
  rRunNo->resize(rRunNo->sizeHint());
  runGrid->addWidget(rRunNo,1,1);
  connect(rRunNo,SIGNAL(clicked()),this,SLOT(runNoClicked()));

  rRunBounding = new QRadioButton(i18n("Run around the &Bounding Rectangle of other frames"),runGroup);
  rRunBounding->resize(rRunBounding->sizeHint());
  runGrid->addWidget(rRunBounding,2,1);
  connect(rRunBounding,SIGNAL(clicked()),this,SLOT(runBoundingClicked()));

  rRunContur = new QRadioButton(i18n("Run around the &Contur of other frames"),runGroup);
  rRunContur->resize(rRunContur->sizeHint());
  runGrid->addWidget(rRunContur,3,1);
  connect(rRunContur,SIGNAL(clicked()),this,SLOT(runConturClicked()));

  runGrid->addColSpacing(0,lRunNo->width());
  runGrid->addColSpacing(1,rRunNo->width());
  runGrid->addColSpacing(1,rRunBounding->width());
  runGrid->addColSpacing(1,rRunContur->width());
  runGrid->setColStretch(1,1);

  runGrid->addRowSpacing(0,10);
  runGrid->addRowSpacing(1,lRunNo->height());
  runGrid->addRowSpacing(1,rRunNo->height());
  runGrid->addRowSpacing(2,lRunBounding->height());
  runGrid->addRowSpacing(2,rRunBounding->height());
  runGrid->addRowSpacing(3,lRunContur->height());
  runGrid->addRowSpacing(3,rRunContur->height());
  runGrid->setRowStretch(4,1);

  runGrid->activate();

  grid2->addMultiCellWidget(runGroup,0,0,0,1);

  lRGap = new QLabel(i18n("Runaround Gap: (in mm)"),tab2);
  lRGap->resize(lRGap->sizeHint());
  lRGap->setAlignment(AlignRight);
  grid2->addWidget(lRGap,1,0);

  eRGap = new KRestrictedLine(tab2,"","1234567890");
  eRGap->setText("0");
  eRGap->setMaxLength(5);
  eRGap->setEchoMode(QLineEdit::Normal);
  eRGap->setFrame(true);
  eRGap->resize(eRGap->sizeHint());
  grid2->addWidget(eRGap,1,1);

  grid2->addColSpacing(0,lRGap->width());
  grid2->addColSpacing(1,eRGap->width());
  grid2->setColStretch(1,1);

  grid2->addRowSpacing(0,runGroup->height());
  grid2->addRowSpacing(1,lRGap->height());
  grid2->addRowSpacing(1,eRGap->height());
  grid2->setRowStretch(2,1);

  grid2->activate();

  addTab(tab2,i18n("Text Frame"));

  uncheckAllRuns();
  switch (frame->getRunAround())
    {
    case RA_NO: rRunNo->setChecked(true);
      break;
    case RA_BOUNDINGRECT: rRunBounding->setChecked(true);
      break;
    case RA_CONTUR: rRunContur->setChecked(true);
      break;
    }
  QString str;
  str.sprintf("%d",frame->getRunAroundGap());
  eRGap->setText(str.data());
}

/*================================================================*/
void KWFrameDia::setupTab3ConnectTextFrames()
{
  tab3 = new QWidget(this);

  grid3 = new QGridLayout(tab3,2,1,15,7);

  lFrameSet = new QLabel(i18n("Choose a frameset to which the current frame should be connected:"),tab3);
  lFrameSet->resize(lFrameSet->sizeHint());
  grid3->addWidget(lFrameSet,0,0);

  lFrameSList = new QListBox(tab3);

  for (unsigned int i = 0;i < doc->getNumFrameSets();i++)
    {
      if (i == 0 && doc->getProcessingType() == KWordDocument::WP) continue;
      QString str;
      str.sprintf("Frameset Nr. %d",i + 1);
      lFrameSList->insertItem(str,-1);
    }

  if (flags & FD_PLUS_NEW_FRAME)
    lFrameSList->insertItem(i18n("Create a new Frameset with this frame"),-1);
      
  connect(lFrameSList,SIGNAL(highlighted(int)),this,SLOT(connectListSelected(int)));
  grid3->addWidget(lFrameSList,1,0);

  grid3->addColSpacing(0,lFrameSet->width());
  grid3->setColStretch(0,1);

  grid3->addRowSpacing(0,lFrameSet->height());
  grid3->addRowSpacing(1,lFrameSet->height());
  grid3->setRowStretch(1,1);

  grid3->activate();

  addTab(tab3,i18n("Connect Text Frames"));

  lFrameSList->setSelected(0,true);
}

/*================================================================*/
void KWFrameDia::uncheckAllRuns()
{
  rRunNo->setChecked(false);
  rRunBounding->setChecked(false);
  rRunContur->setChecked(false);
}

/*================================================================*/
void KWFrameDia::runNoClicked()
{
  uncheckAllRuns();
  rRunNo->setChecked(true);
}

/*================================================================*/
void KWFrameDia::runBoundingClicked()
{
  uncheckAllRuns();
  rRunBounding->setChecked(true);
}

/*================================================================*/
void KWFrameDia::runConturClicked()
{
  uncheckAllRuns();
  rRunContur->setChecked(true);
}

/*================================================================*/
void KWFrameDia::applyChanges()
{
  if ((flags & FD_FRAME_SET) && frameset && frameset->getFrameType() == FT_TEXT)
    dynamic_cast<KWTextFrameSet*>(frameset)->setAutoCreateNewFrame(cAutoCreateFrame->isChecked());

  if ((flags & FD_FRAME && frame && (!frameset || frameset && frameset->getFrameType() == FT_TEXT)))
    {
      if (rRunNo->isChecked())
	frame->setRunAround(RA_NO);
      else if (rRunBounding->isChecked())
	frame->setRunAround(RA_BOUNDINGRECT);
      else if (rRunContur->isChecked())
	frame->setRunAround(RA_CONTUR);
      frame->setRunAroundGap(atoi(eRGap->text()));
    }

  if (flags & FD_FRAME_CONNECT && doc)
    {
      unsigned int _num = static_cast<unsigned int>(lFrameSList->currentItem());
      if (doc->getProcessingType() == KWordDocument::WP) _num++;

      if (static_cast<unsigned int>(_num) < doc->getNumFrameSets())
	doc->getFrameSet(_num)->addFrame(frame);
      else
	{
	  KWTextFrameSet *_frameSet = new KWTextFrameSet(doc);
	  _frameSet->addFrame(frame);
	  doc->addFrameSet(_frameSet);
	}
    }
}

/*================================================================*/
void KWFrameDia::connectListSelected(int _num)
{
  if (doc->getProcessingType() == KWordDocument::WP) _num++;

  if (static_cast<unsigned int>(_num) < doc->getNumFrameSets())
    {
      if (doc->getFrameSet(_num)->getFrameType() != FT_TEXT || doc->getFrameSet(_num)->getFrameInfo() != FI_BODY)
	{
	  lFrameSList->setCurrentItem(lFrameSList->count() - 1);
	  page->setHilitFrameSet(-1);
	}
      else
	page->setHilitFrameSet(_num);
    }
  else
    page->setHilitFrameSet(-1);
}
