/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson <psn@linux.se>
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
#include "stenciltexteditor.h"

#include <qtoolbutton.h>
#include <qfont.h>
#include <qcolor.h>
#include <qbuttongroup.h>

#include <kiconloader.h>
#include <ktextedit.h>
#include <kfontcombo.h>
#include <kcolorbutton.h>
#include <kdebug.h>

#include "kivio_stenciltexteditorui.h"

namespace Kivio {

StencilTextEditor::StencilTextEditor(const QString& caption, QWidget *parent, const char *name)
  : KDialogBase(parent, name, true, caption, KDialogBase::Ok|KDialogBase::Cancel)
{
  m_mainWidget = new StencilTextEditorUI(this);
  setMainWidget(m_mainWidget);

  m_mainWidget->m_boldButton->setIconSet(SmallIconSet("text_bold", 16));
  m_mainWidget->m_italicsButton->setIconSet(SmallIconSet("text_italic", 16));
  m_mainWidget->m_underLineButton->setIconSet(SmallIconSet("text_under", 16));

  m_mainWidget->m_alignLeftButton->setIconSet(SmallIconSet("text_left", 16));
  m_mainWidget->m_alignCenterButton->setIconSet(SmallIconSet("text_center", 16));
  m_mainWidget->m_alignRightButton->setIconSet(SmallIconSet("text_right", 16));

  QButtonGroup* btnGrp = new QButtonGroup(m_mainWidget);
  btnGrp->hide();
  btnGrp->setExclusive(true);
  btnGrp->insert(m_mainWidget->m_alignLeftButton);
  btnGrp->insert(m_mainWidget->m_alignCenterButton);
  btnGrp->insert(m_mainWidget->m_alignRightButton);

  connect(m_mainWidget->m_fontCombo, SIGNAL(activated(int)), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_fontSizeCombo, SIGNAL(activated(int)), this, SLOT(updateFormating()));
  connect(btnGrp, SIGNAL(clicked(int)), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_boldButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_italicsButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_underLineButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_textColorButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
}

StencilTextEditor::~StencilTextEditor()
{
}

void StencilTextEditor::setFont(const QFont& font)
{
  m_mainWidget->m_textArea->setFont(font);
  m_mainWidget->m_fontCombo->setCurrentFont(font.family());
  m_mainWidget->m_fontSizeCombo->setCurrentText(QString::number(font.pointSize()));
  m_mainWidget->m_boldButton->setOn(font.bold());
  m_mainWidget->m_italicsButton->setOn(font.italic());
  m_mainWidget->m_underLineButton->setOn(font.underline());
}

void StencilTextEditor::setFontColor(const QColor& color)
{
  m_mainWidget->m_textArea->setPaletteForegroundColor(color);
  m_mainWidget->m_textColorButton->setColor(color);
}

void StencilTextEditor::setBackgroundColor(const QColor& color)
{
  m_mainWidget->m_textArea->setPaletteBackgroundColor(color);
}

void StencilTextEditor::setHorizontalAlign(Qt::AlignmentFlags flag)
{
  switch(flag) {
    case Qt::AlignRight:
      m_mainWidget->m_alignLeftButton->setOn(false);
      m_mainWidget->m_alignCenterButton->setOn(false);
      m_mainWidget->m_alignRightButton->setOn(true);
      break;
    case Qt::AlignHCenter:
      m_mainWidget->m_alignLeftButton->setOn(false);
      m_mainWidget->m_alignCenterButton->setOn(true);
      m_mainWidget->m_alignRightButton->setOn(false);
      break;
    case Qt::AlignLeft:
    default:
      m_mainWidget->m_alignLeftButton->setOn(true);
      m_mainWidget->m_alignCenterButton->setOn(false);
      m_mainWidget->m_alignRightButton->setOn(false);
      break;
  }

  m_mainWidget->m_textArea->setAlignment(flag);
}

QFont StencilTextEditor::font() const
{
  QFont font;
  font.setFamily(m_mainWidget->m_fontCombo->currentFont());
  font.setPointSize(m_mainWidget->m_fontSizeCombo->currentText().toInt());
  font.setBold(m_mainWidget->m_boldButton->isOn());
  font.setItalic(m_mainWidget->m_italicsButton->isOn());
  font.setUnderline(m_mainWidget->m_underLineButton->isOn());

  return font;
}

QColor StencilTextEditor::fontColor() const
{
  return m_mainWidget->m_textColorButton->color();
}

Qt::AlignmentFlags StencilTextEditor::alignment() const
{
  Qt::AlignmentFlags flag = Qt::AlignLeft;

  if(m_mainWidget->m_alignCenterButton->isOn()) {
    flag = Qt::AlignHCenter;
  } else if(m_mainWidget->m_alignRightButton->isOn()) {
    flag = Qt::AlignRight;
  }

  return flag;
}

void StencilTextEditor::setText(const QString& text)
{
  m_mainWidget->m_textArea->setText(text);
}

QString StencilTextEditor::text() const
{
  return m_mainWidget->m_textArea->text();
}

void StencilTextEditor::updateFormating()
{
  m_mainWidget->m_textArea->setFont(font());

  m_mainWidget->m_textArea->setPaletteForegroundColor(fontColor());


  m_mainWidget->m_textArea->selectAll(true);
  m_mainWidget->m_textArea->setAlignment(alignment());
  m_mainWidget->m_textArea->selectAll(false);

  m_mainWidget->m_textArea->setFocus();
}

}

#include "stenciltexteditor.moc"
