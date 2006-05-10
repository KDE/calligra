/* This file is part of the KDE project
   Copyright (C) 2003 Peter Simonsson <psn@linux.se>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kiviotextformatdlg.h"

#include <QLabel>
#include <QLayout>
#include <qbuttongroup.h>
#include <qradiobutton.h>

#include <kfontdialog.h>
#include <kcolorbutton.h>
#include <klocale.h>

#include "kivio_view.h"
#include "kivio_doc.h"

KivioTextFormatDlg::KivioTextFormatDlg(KivioView* parent, const char* name)
  : KDialogBase(Tabbed, i18n("Text Format"), Ok|Cancel|Default, Ok, parent, name)
{
  m_valign = Qt::AlignVCenter;
  m_halign = Qt::AlignHCenter;
  initFontTab();
  initPositionTab();
}

void KivioTextFormatDlg::initFontTab()
{
  QFrame* tab = addPage(i18n("Font"));
  m_fontChooser = new KFontChooser(tab);
  QLabel* textColorLbl = new QLabel(i18n("Text color:"), tab);
  m_textCBtn = new KColorButton(tab);
  QGridLayout* gl = new QGridLayout(tab);
  gl->setSpacing(KDialog::spacingHint());
  gl->addMultiCellWidget(m_fontChooser, 0, 0, 0, 1);
  gl->addWidget(textColorLbl, 1, 0);
  gl->addWidget(m_textCBtn, 1, 1);
}

void KivioTextFormatDlg::initPositionTab()
{
  QFrame* tab = addPage(i18n("Position"));
  m_valignBGrp = new QButtonGroup(1, Qt::Horizontal, i18n("Vertical"), tab);
  (void) new QRadioButton(i18n("&Top"), m_valignBGrp);
  (void) new QRadioButton(i18n("&Center"), m_valignBGrp);
  (void) new QRadioButton(i18n("&Bottom"), m_valignBGrp);
  m_valignBGrp->setButton(1);
  m_halignBGrp = new QButtonGroup(1, Qt::Vertical, i18n("Horizontal"), tab);
  (void) new QRadioButton(i18n("&Left"), m_halignBGrp);
  (void) new QRadioButton(i18n("C&enter"), m_halignBGrp);
  (void) new QRadioButton(i18n("&Right"), m_halignBGrp);
  m_halignBGrp->setButton(1);
  m_preview = new QLabel(i18n("Preview"), tab);
  m_preview->setFrameStyle(QFrame::Box | QFrame::Sunken);
  m_preview->setAlignment(m_valign | m_halign);
  QGridLayout* gl = new QGridLayout(tab);
  gl->setSpacing(KDialog::spacingHint());
  gl->setRowStretch(0, 10);
  gl->setColStretch(1, 10);
  gl->addWidget(m_valignBGrp, 0, 0);
  gl->addWidget(m_preview, 0, 1);
  gl->addWidget(m_halignBGrp, 1, 1);

  connect(m_valignBGrp, SIGNAL(clicked(int)), SLOT(updateVAlign(int)));
  connect(m_halignBGrp, SIGNAL(clicked(int)), SLOT(updateHAlign(int)));
}

void KivioTextFormatDlg::updateVAlign(int i)
{
  switch(i) {
    case 0:
      m_valign = Qt::AlignTop;
      break;
    case 1:
      m_valign = Qt::AlignVCenter;
      break;
    case 2:
      m_valign = Qt::AlignBottom;
      break;
  }

  m_preview->setAlignment(m_valign | m_halign);
}

void KivioTextFormatDlg::updateHAlign(int i)
{
  switch(i) {
    case 0:
      m_halign = Qt::AlignLeft;
      break;
    case 1:
      m_halign = Qt::AlignHCenter;
      break;
    case 2:
      m_halign = Qt::AlignRight;
      break;
  }

  m_preview->setAlignment(m_valign | m_halign);
}

int KivioTextFormatDlg::valign()
{
  return m_valign;
}

int KivioTextFormatDlg::halign()
{
  return m_halign;
}

QFont KivioTextFormatDlg::font()
{
  return m_fontChooser->font();
}

QColor KivioTextFormatDlg::textColor()
{
  return m_textCBtn->color();
}

void KivioTextFormatDlg::setVAlign(int i)
{
  switch(i) {
    case Qt::AlignTop:
      m_valignBGrp->setButton(0);
      break;
    case Qt::AlignVCenter:
      m_valignBGrp->setButton(1);
      break;
    case Qt::AlignBottom:
      m_valignBGrp->setButton(2);
      break;
  }

  m_valign = static_cast<Qt::AlignmentFlags>(i);
  m_preview->setAlignment(m_valign | m_halign);
}

void KivioTextFormatDlg::setHAlign(int i)
{
  switch(i) {
    case Qt::AlignLeft:
      m_halignBGrp->setButton(0);
      break;
    case Qt::AlignHCenter:
      m_halignBGrp->setButton(1);
      break;
    case Qt::AlignRight:
      m_halignBGrp->setButton(2);
      break;
  }

  m_halign = static_cast<Qt::AlignmentFlags>(i);
  m_preview->setAlignment(m_valign | m_halign);
}

void KivioTextFormatDlg::setFont(QFont f)
{
  m_fontChooser->setFont(f);
}

void KivioTextFormatDlg::setTextColor(QColor c)
{
  m_textCBtn->setColor(c);
}

void KivioTextFormatDlg::slotDefault()
{
  setFont((static_cast<KivioView*>(parent()))->doc()->defaultFont());
  setTextColor(QColor(0, 0, 0));
  setHAlign(Qt::AlignHCenter);
  setVAlign(Qt::AlignVCenter);
}

#include "kiviotextformatdlg.moc"
