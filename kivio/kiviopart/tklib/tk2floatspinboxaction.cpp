/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#include "tk2floatspinboxaction.h"
#include "tkfloatspinboxaction.h"
#include "tkunits.h"

#include <qlayout.h>
#include <qpixmap.h>
#include <qiconset.h>

#include <ktoolbar.h>

TK2FloatSpinBoxActionWidget::TK2FloatSpinBoxActionWidget(TKUFloatSpinBoxAction* a1, TKUFloatSpinBoxAction* a2, QWidget* parent, const char* name)
: QWidget(parent,name)
{
  m_layout = new QVBoxLayout(this,1,0);

  QWidget* p1 = new QWidget(this);
  QVBoxLayout* pl1 = new QVBoxLayout(p1);
  a1->plug(p1);
  pl1->addWidget((QWidget*)p1->child("KTToolBarLayout"));

  QWidget* p2 = new QWidget(this);
  QVBoxLayout* pl2 = new QVBoxLayout(p2);
  a2->plug(p2);
  pl2->addWidget((QWidget*)p2->child("KTToolBarLayout"));

  m_layout->addWidget(p1);
  m_layout->addWidget(p2);
}

TK2FloatSpinBoxActionWidget::~TK2FloatSpinBoxActionWidget()
{
}

void TK2FloatSpinBoxActionWidget::resizeEvent(QResizeEvent* ev)
{
  QWidget::resizeEvent(ev);
  int d = (height()-1)/2;
  m_layout->setSpacing(height()-2*d);
}
/****************************************************************/
TK2UFloatSpinBoxAction::TK2UFloatSpinBoxAction( QObject* parent, const char* name )
: TKAction(parent,name)
{
  m_action1 = new TKUFloatSpinBoxAction("","",0,0,0);
  m_action1->setMinimumStyle(true);
  connect( m_action1, SIGNAL(activated()), this, SLOT(slotActivated1()) );

  m_action2 = new TKUFloatSpinBoxAction("","",0,0,0);
  m_action2->setMinimumStyle(true);
  connect( m_action2, SIGNAL(activated()), this, SLOT(slotActivated2()) );
}

TK2UFloatSpinBoxAction::~TK2UFloatSpinBoxAction()
{
  delete m_action1;
  delete m_action2;
}

void TK2UFloatSpinBoxAction::setUnit(int unit)
{
  m_action1->setUnit(unit);
  m_action2->setUnit(unit);
}

int TK2UFloatSpinBoxAction::unit()
{
  return m_action1->unit();
}

int TK2UFloatSpinBoxAction::plug( QWidget* widget, int index )
{
  if ( widget->inherits("KToolBar") )
  {
    KToolBar* bar = static_cast<KToolBar*>( widget );
    int id_ = KAction::getToolButtonID();

    QWidget* base = createLayout(bar,new TK2FloatSpinBoxActionWidget(m_action1,m_action2));
    bar->insertWidget( id_, 100, base, index );
    addContainer( bar, id_ );

    connect( bar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }
  return -1;
}

void TK2UFloatSpinBoxAction::slotActivated1()
{
  emit activated();
}

void TK2UFloatSpinBoxAction::slotActivated2()
{
  emit activated();
}
/****************************************************************/
TKSizeAction::TKSizeAction( QObject* parent, const char* name )
: TK2UFloatSpinBoxAction(parent,name)
{
  static char* width_xpm[] = {
  "13 11 3 1",
  "  c Gray0",
  ". c #808080",
  "X c None",
  "XXXXXXXXXXXXX",
  "XXXXXXXXXXXXX",
  ".XXXXXXXXXXX.",
  ".XX XXXXX XX.",
  ".X  XXXXX  X.",
  ".           .",
  ".X  XXXXX  X.",
  ".XX XXXXX XX.",
  ".XXXXXXXXXXX.",
  "XXXXXXXXXXXXX",
  "XXXXXXXXXXXXX"
  };

  static char* height_xpm[] = {
  "13 11 3 1",
  "  c Gray0",
  ". c #808080",
  "X c None",
  "XXX.......XXX",
  "XXXXXX XXXXXX",
  "XXXXX   XXXXX",
  "XXXX     XXXX",
  "XXXXXX XXXXXX",
  "XXXXXX XXXXXX",
  "XXXXXX XXXXXX",
  "XXXX     XXXX",
  "XXXXX   XXXXX",
  "XXXXXX XXXXXX",
  "XXX.......XXX"
  };

  m_action1->setIconSet(QIconSet(QPixmap((const char**)width_xpm)));
  m_action1->setDecimals(3);
  m_action1->setWrapping(false);
  m_action1->setMinValue(0.0);
  m_action1->setMaxValue(1000.0);
  m_action1->setLineStep(0.5);
  m_action1->setPrefix("");
  m_action1->setSuffix("pt");
  m_action1->setValue(0.0);

  m_action2->setIconSet(QIconSet(QPixmap((const char**)height_xpm)));
  m_action2->setDecimals(3);
  m_action2->setWrapping(false);
  m_action2->setMinValue(0.0);
  m_action2->setMaxValue(1000.0);
  m_action2->setLineStep(0.5);
  m_action2->setPrefix("");
  m_action2->setSuffix("pt");
  m_action2->setValue(0.0);
}

TKSizeAction::~TKSizeAction()
{
}

void TKSizeAction::size(float& v1, float& v2, int unit)
{
  v1 = m_action1->value(unit);
  v2 = m_action2->value(unit);
}

void TKSizeAction::setSize(float v1, float v2, int unit)
{
  m_action1->setValue(v1,unit);
  m_action2->setValue(v2,unit);
}
/****************************************************************/
TKPositionAction::TKPositionAction( QObject* parent, const char* name )
: TK2UFloatSpinBoxAction(parent,name)
{
  static char* xpos_xpm[] = {
  "13 11 2 1",
  "  c Gray0",
  ". c None",
  ".. .. .......",
  ".. .. .......",
  "...  ........",
  "...  ........",
  ".. .. .......",
  ".. .. .     .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  ".......     ."
  };

  static char* ypos_xpm[] = {
  "13 11 2 1",
  "  c Gray0",
  ". c None",
  "... . .......",
  "... . .......",
  "... . .......",
  ".... ........",
  ".... ........",
  "..  ...     .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  "....... ... .",
  ".......     ."
  };

  m_action1->setIconSet(QIconSet(QPixmap((const char**)xpos_xpm)));
  m_action1->setDecimals(3);
  m_action1->setWrapping(false);
  m_action1->setMinValue(-1000.0);
  m_action1->setMaxValue(1000.0);
  m_action1->setLineStep(0.5);
  m_action1->setPrefix("");
  m_action1->setSuffix("pt");
  m_action1->setValue(0.0);

  m_action2->setIconSet(QIconSet(QPixmap((const char**)ypos_xpm)));
  m_action2->setDecimals(3);
  m_action2->setWrapping(false);
  m_action2->setMinValue(-1000.0);
  m_action2->setMaxValue(1000.0);
  m_action2->setLineStep(0.5);
  m_action2->setPrefix("");
  m_action2->setSuffix("pt");
  m_action2->setValue(0.0);
}

TKPositionAction::~TKPositionAction()
{
}

void TKPositionAction::pos(float& v1, float& v2, int unit)
{
  v1 = m_action1->value(unit);
  v2 = m_action2->value(unit);
}

void TKPositionAction::setPos(float v1, float v2, int unit)
{
  m_action1->setValue(v1,unit);
  m_action2->setValue(v2,unit);
}
#include "tk2floatspinboxaction.moc"
