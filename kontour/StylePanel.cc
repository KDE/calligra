/* -*- C++ -*-

  This file is part of Kontour.
  Copyright (C) 2001 Rob Buis (rwlbuis@wanadoo.nl)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "StylePanel.h"

#include <qtabwidget.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qpainter.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kcombobox.h>
#include <kdebug.h>

#include <koColorChooser.h>
#include <koIconChooser.h>

#include "kontour_factory.h"
#include "kontour_view.h"
#include "GDocument.h"
#include "GPage.h"
#include "GObject.h"
#include "GStyle.h"
#include "GStyleList.h"
#include "SetPropertyCmd.h"

OutlinePanel::OutlinePanel(KontourView *aView, QWidget *parent, const char *name):
QDockWindow(QDockWindow::InDock, parent, name)
{
  mView = aView;

  mTab = new QTabWidget(this);
  mTab->setTabShape(QTabWidget::Triangular);

  QWidget *mOutline = new QWidget(mTab);
  QGridLayout *mOutlineLayout = new QGridLayout(mOutline, 4, 2);

  mStroked = new QCheckBox(i18n("Stroked"), mOutline);
  connect(mStroked, SIGNAL(toggled(bool)), this, SLOT(slotChangeStroked(bool)));

  QLabel *mStartArrowText = new QLabel(i18n("Start Arrow"), mOutline);
  mStartArrowBox = new KComboBox(mOutline); // It's only temporary
  mStartArrowBox->insertItem("no");
  mStartArrowBox->insertItem("<-");
  mStartArrowBox->insertItem("<<-");
  mStartArrowBox->setCurrentItem(0);
  connect(mStartArrowBox, SIGNAL(activated(int)), this, SLOT(slotChangeStartArrow(int)));

  QLabel *mEndArrowText = new QLabel(i18n("End Arrow"), mOutline);
  mEndArrowBox = new KComboBox(mOutline); // It's only temporary
  mEndArrowBox->insertItem("no");
  mEndArrowBox->insertItem("->");
  mEndArrowBox->insertItem("->>");
  mEndArrowBox->setCurrentItem(0);
  connect(mEndArrowBox, SIGNAL(activated(int)), this, SLOT(slotChangeEndArrow(int)));

  QLabel *mOpacityText = new QLabel(i18n("Opacity"), mOutline);
  mOpacityBox = new QSpinBox(0, 100, 5, mOutline);
  mOpacityBox->setSuffix("%");
  connect(mOpacityBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeOpacity(int)));

  mOutlineLayout->addMultiCellWidget(mStroked, 0, 0, 0, 1);
  mOutlineLayout->addWidget(mStartArrowText, 1, 0);
  mOutlineLayout->addWidget(mStartArrowBox, 1, 1);
  mOutlineLayout->addWidget(mEndArrowText, 2, 0);
  mOutlineLayout->addWidget(mEndArrowBox, 2, 1);
  mOutlineLayout->addWidget(mOpacityText, 3, 0);
  mOutlineLayout->addWidget(mOpacityBox, 3, 1);

  mTab->insertTab(mOutline, i18n("Stroking"));

  /* Color tab */
  mOutlinePanel = new KoColorChooser(mTab);
  connect(mOutlinePanel, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  mTab->insertTab(mOutlinePanel, i18n("Color"));

  /* Style tab */
  QWidget *mOutlineStyle = new QWidget(mTab);
  QGridLayout *mOutlineStyleLayout = new QGridLayout(mOutlineStyle, 4, 2);

  /* Outline width */
  QLabel *mWidthText = new QLabel(i18n("Width"), mOutlineStyle);
  mWidthBox = new QSpinBox(1, 100, 1, mOutlineStyle);
  connect(mWidthBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeLineWidth(int)));

  QLabel *mLineStyleText = new QLabel(i18n("Line Style"), mOutlineStyle);
  mLineStyleBox = new KComboBox(mOutlineStyle); // It's only temporary
  mLineStyleBox->insertItem("----------");
  mLineStyleBox->insertItem("- - - - - ");
  mLineStyleBox->insertItem("-  -  -  -");
  mLineStyleBox->setCurrentItem(0);
  connect(mLineStyleBox, SIGNAL(activated(int)), this, SLOT(slotChangeLineStyle(int)));

  /* Join style selection */
  QLabel *mJoinText = new QLabel(i18n("Join"), mOutlineStyle);
  mJoinBox = new QButtonGroup(3, Qt::Horizontal, mOutlineStyle);
  mJoinBox->setFrameStyle(QFrame::NoFrame);
  mJoinBox->setInsideMargin(0);
  mJoinBox->setFixedHeight(20);
  mJoinBox->setExclusive(true);
  QPushButton *mMiterBtn = new QPushButton(mJoinBox);
  mMiterBtn->setToggleButton(true);
  mMiterBtn->setFixedWidth(20);
  mMiterBtn->setFixedHeight(20);
  mMiterBtn->setPixmap(SmallIcon("join_miter", KontourFactory::global()));
  QPushButton *mRoundBtn = new QPushButton(mJoinBox);
  mRoundBtn->setToggleButton(true);
  mRoundBtn->setFixedWidth(20);
  mRoundBtn->setFixedHeight(20);
  mRoundBtn->setPixmap(SmallIcon("join_round", KontourFactory::global()));
  QPushButton *mBevelBtn = new QPushButton(mJoinBox);
  mBevelBtn->setToggleButton(true);
  mBevelBtn->setFixedWidth(20);
  mBevelBtn->setFixedHeight(20);
  mBevelBtn->setPixmap(SmallIcon("join_bevel", KontourFactory::global()));
  connect(mJoinBox, SIGNAL(pressed(int)), this, SLOT(slotJoinPressed(int)));

  /* Cap style selection */
  QLabel *mCapText = new QLabel(i18n("Cap"), mOutlineStyle);
  mCapBox = new QButtonGroup(3, Qt::Horizontal, mOutlineStyle);
  mCapBox->setFrameStyle(QFrame::NoFrame);
  mCapBox->setInsideMargin(0);
  mCapBox->setFixedHeight(20);
  mCapBox->setExclusive(true);
  QPushButton *mFlatBtn = new QPushButton(mCapBox);
  mFlatBtn->setToggleButton(true);
  mFlatBtn->setFixedWidth(20);
  mFlatBtn->setFixedHeight(20);
  mFlatBtn->setPixmap(SmallIcon("cap_butt", KontourFactory::global()));
  connect(mCapBox, SIGNAL(pressed(int)), this, SLOT(slotCapPressed(int)));
  QPushButton *mCRoundBtn = new QPushButton(mCapBox);
  mCRoundBtn->setToggleButton(true);
  mCRoundBtn->setFixedWidth(20);
  mCRoundBtn->setFixedHeight(20);
  mCRoundBtn->setPixmap(SmallIcon("cap_round", KontourFactory::global()));
  QPushButton *mSquareBtn = new QPushButton(mCapBox);
  mSquareBtn->setToggleButton(true);
  mSquareBtn->setFixedWidth(20);
  mSquareBtn->setFixedHeight(20);
  mSquareBtn->setPixmap(SmallIcon("cap_square", KontourFactory::global()));

  mOutlineStyleLayout->addWidget(mWidthText, 0, 0);
  mOutlineStyleLayout->addWidget(mWidthBox, 0, 1);
  mOutlineStyleLayout->addWidget(mLineStyleText, 1, 0);
  mOutlineStyleLayout->addWidget(mLineStyleBox, 1, 1);
  mOutlineStyleLayout->addWidget(mJoinText, 2, 0);
  mOutlineStyleLayout->addWidget(mJoinBox, 2, 1);
  mOutlineStyleLayout->addWidget(mCapText, 3, 0);
  mOutlineStyleLayout->addWidget(mCapBox, 3, 1);

  mTab->insertTab(mOutlineStyle, i18n("Style"));

  setWidget(mTab);
  setCloseMode(QDockWindow::Always);
  setOpaqueMoving(true);
  setCaption(i18n("Outline"));
  slotUpdate();
}

void OutlinePanel::slotUpdate()
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    bool b = mView->activeDocument()->styles()->style()->stroked();
    mStroked->setChecked(b);
    mStartArrowBox->setEnabled(b);
    mEndArrowBox->setEnabled(b);
    mOpacityBox->setEnabled(b);
    mOutlinePanel->setEnabled(b);
    mWidthBox->setEnabled(b);
    mLineStyleBox->setEnabled(b);
    mJoinBox->setEnabled(b);
    mCapBox->setEnabled(b);
    mOpacityBox->setValue(mView->activeDocument()->styles()->style()->outlineOpacity());
    mWidthBox->setValue(static_cast<int>(mView->activeDocument()->styles()->style()->outlineWidth()));
    mJoinBox->setButton(mView->activeDocument()->styles()->style()->joinStyle());
    mCapBox->setButton(mView->activeDocument()->styles()->style()->capStyle());
  }
  else
  {
    bool b = mView->activeDocument()->activePage()->getSelection().first()->style()->stroked();
    mStroked->setChecked(b);
    mStartArrowBox->setEnabled(b);
    mEndArrowBox->setEnabled(b);
    mOpacityBox->setEnabled(b);
    mOutlinePanel->setEnabled(b);
    mWidthBox->setEnabled(b);
    mLineStyleBox->setEnabled(b);
    mJoinBox->setEnabled(b);
    mCapBox->setEnabled(b);
    mOpacityBox->setValue(mView->activeDocument()->activePage()->getSelection().first()->style()->outlineOpacity());
    mWidthBox->setValue(static_cast<int>(mView->activeDocument()->activePage()->getSelection().first()->style()->outlineWidth()));
    mJoinBox->setButton(mView->activeDocument()->activePage()->getSelection().first()->style()->joinStyle());
    mCapBox->setButton(mView->activeDocument()->activePage()->getSelection().first()->style()->capStyle());
  }
}

void OutlinePanel::slotChangeStroked(bool b)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->stroked(b);
  }
  else
  {
    kdDebug(38000) << "---SET = "<< b << endl;
    SetOutlineCmd *cmd = new SetOutlineCmd(mView->activeDocument(), b);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

void OutlinePanel::slotChangeStartArrow(int /*w*/)
{
}

void OutlinePanel::slotChangeEndArrow(int /*w*/)
{
}

void OutlinePanel::slotChangeOpacity(int o)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->outlineOpacity(o);
  }
  else
  {
    SetOutlineOpacityCmd *cmd = new SetOutlineOpacityCmd(mView->activeDocument(), o);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

void OutlinePanel::slotChangeColor(const KoColor &c)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->outlineColor(c);
  }
  else
  {
    SetOutlineColorCmd *cmd = new SetOutlineColorCmd(mView->activeDocument(), c);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

void OutlinePanel::slotChangeLineWidth(int l)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->outlineWidth(l);
  }
  else
  {
    SetOutlineWidthCmd *cmd = new SetOutlineWidthCmd(mView->activeDocument(), l);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

void OutlinePanel::slotChangeLineStyle(int /*w*/)
{

}

void OutlinePanel::slotJoinPressed(int w)
{
  KoOutline::Join style;
  switch(w)
  {
  case 0: style = KoOutline::JoinMiter; break;
  case 1: style = KoOutline::JoinRound; break;
  case 2: style = KoOutline::JoinBevel;
  }
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->joinStyle(style);
  }
  else
  {
    SetJoinStyleCmd *cmd = new SetJoinStyleCmd(mView->activeDocument(), style);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

void OutlinePanel::slotCapPressed(int w)
{
  KoOutline::Cap style;
  switch(w)
  {
  case 0: style = KoOutline::CapButt; break;
  case 1: style = KoOutline::CapRound; break;
  case 2: style = KoOutline::CapSquare;
  }
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->capStyle(style);
  }
  else
  {
    SetCapStyleCmd *cmd = new SetCapStyleCmd(mView->activeDocument(), style);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

/*=============================================================*/

PaintPanel::PaintPanel(KontourView *aView, QWidget *parent, const char *name):
QDockWindow(QDockWindow::InDock, parent, name)
{
  mView = aView;

  mTab = new QTabWidget(this);
  mTab->setTabShape(QTabWidget::Triangular);

  QWidget *mPainting = new QWidget(mTab);
  QGridLayout *mPaintingLayout = new QGridLayout(mPainting, 2, 2);

  QLabel *mPaintingText = new QLabel(i18n("Painting"), mPainting);
  mPaintingBox = new KComboBox(mPainting);
  mPaintingBox->insertItem(i18n("No"));
  mPaintingBox->insertItem(i18n("Color"));
  mPaintingBox->insertItem(i18n("Gradient"));
  mPaintingBox->insertItem(i18n("Bitmap"));
  mPaintingBox->setCurrentItem(0);
  connect(mPaintingBox, SIGNAL(activated(int)), this, SLOT(slotChangeFilled(int)));

  QLabel *mOpacityText = new QLabel(i18n("Opacity"), mPainting);
  mOpacityBox = new QSpinBox(0, 100, 5, mPainting);
  connect(mOpacityBox, SIGNAL(valueChanged(int)), this, SLOT(slotChangeOpacity(int)));
  mOpacityBox->setSuffix("%");

  mPaintingLayout->addWidget(mPaintingText, 0, 0);
  mPaintingLayout->addWidget(mPaintingBox, 0, 1);
  mPaintingLayout->addWidget(mOpacityText, 1, 0);
  mPaintingLayout->addWidget(mOpacityBox, 1, 1);

  mTab->insertTab(mPainting, i18n("Painting"));

  mPaintPanel = new KoColorChooser(mTab);
  connect(mPaintPanel, SIGNAL(colorChanged(const KoColor &)), this, SLOT(slotChangeColor(const KoColor &)));
  mTab->insertTab(mPaintPanel, i18n("Color"));


  mBitmapPanel = new KoIconChooser(QSize(30, 30), mTab);
  mTab->insertTab(mBitmapPanel, i18n("Bitmap"));

  setWidget(mTab);
  setCloseMode(QDockWindow::Always);
  setOpaqueMoving(true);
  setCaption(i18n("Painting"));
  slotUpdate();
}

void PaintPanel::slotUpdate(bool pages)
{
  int f;
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
    f = mView->activeDocument()->styles()->style()->filled();
  else
    f = mView->activeDocument()->activePage()->getSelection().first()->style()->filled();
  mPaintingBox->setCurrentItem(f);
  if(f == 0)
    mOpacityBox->setEnabled(false);
  else
    mOpacityBox->setEnabled(true);
  if(pages)
  {
    QWidget *p = mTab->currentPage();
    mTab->removePage(mPaintPanel);
    mTab->removePage(mBitmapPanel);
    if(f == 1)
      mTab->insertTab(mPaintPanel, i18n("Color"));
    else if(f == 3)
      mTab->insertTab(mBitmapPanel, i18n("Bitmap"));
    mTab->showPage(p);
  }
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mOpacityBox->setValue(mView->activeDocument()->styles()->style()->fillOpacity());
  }
  else
  {
    mOpacityBox->setValue(mView->activeDocument()->activePage()->getSelection().first()->style()->fillOpacity());
  }
}

void PaintPanel::slotChangeFilled(int f)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->filled(f);
  }
  else
  {
    SetFillCmd *cmd = new SetFillCmd(mView->activeDocument(), f);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate();
}

void PaintPanel::slotChangeOpacity(int o)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->fillOpacity(o);
  }
  else
  {
    SetFillOpacityCmd *cmd = new SetFillOpacityCmd(mView->activeDocument(), o);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate(false);
}

void PaintPanel::slotChangeColor(const KoColor &c)
{
  if(mView->activeDocument()->activePage()->selectionIsEmpty())
  {
    mView->activeDocument()->styles()->style()->fillColor(c);
  }
  else
  {
    SetFillColorCmd *cmd = new SetFillColorCmd(mView->activeDocument(), c);
    KontourDocument *doc = (KontourDocument *)mView->koDocument();
    doc->history()->addCommand(cmd);
  }
  slotUpdate(false);
}

#include "StylePanel.moc"
