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
#include <QFont>
#include <QColor>
#include <qpopupmenu.h>

#include <kiconloader.h>
#include <ktextedit.h>
#include <kfontcombo.h>
#include <kcolorbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <karrowbutton.h>

#include "kivio_stenciltexteditorui.h"

namespace Kivio {

StencilTextEditor::StencilTextEditor(const QString& caption, QWidget *parent, const char *name)
  : KDialogBase(parent, name, true, caption, KDialogBase::Ok|KDialogBase::Cancel)
{
  m_hAlign = -1;
  m_vAlign = -1;
  m_mainWidget = new StencilTextEditorUI(this);
  setMainWidget(m_mainWidget);

  m_mainWidget->m_boldButton->setIconSet(SmallIconSet("text_bold", 16));
  m_mainWidget->m_italicsButton->setIconSet(SmallIconSet("text_italic", 16));
  m_mainWidget->m_underLineButton->setIconSet(SmallIconSet("text_under", 16));

  QPopupMenu* menu = new QPopupMenu(m_mainWidget->m_hAlignButton, "hAlignMenu");
  menu->setCheckable(true);
  menu->insertItem(SmallIconSet("text_left", 16), i18n("Align Left"), Qt::AlignLeft);
  menu->insertItem(SmallIconSet("text_center", 16), i18n("Align Center"), Qt::AlignHCenter);
  menu->insertItem(SmallIconSet("text_right", 16), i18n("Align Right"), Qt::AlignRight);
  m_mainWidget->m_hAlignButton->setPopup(menu);
  connect(menu, SIGNAL(activated(int)), this, SLOT(setHorizontalAlign(int)));
  connect(m_mainWidget->m_hAlignButton, SIGNAL(clicked()), this, SLOT(showHAlignPopup()));

  menu = new QPopupMenu(m_mainWidget->m_vAlignButton, "hAlignMenu");
  menu->setCheckable(true);
  menu->insertItem(SmallIconSet("align_top", 16), i18n("Align Top"), Qt::AlignTop);
  menu->insertItem(SmallIconSet("align_vcenter", 16), i18n("Align Vertical Center"), Qt::AlignVCenter);
  menu->insertItem(SmallIconSet("align_bottom", 16), i18n("Align Bottom"), Qt::AlignBottom);
  m_mainWidget->m_vAlignButton->setPopup(menu);
  connect(menu, SIGNAL(activated(int)), this, SLOT(setVerticalAlign(int)));
  connect(m_mainWidget->m_vAlignButton, SIGNAL(clicked()), this, SLOT(showVAlignPopup()));

  connect(m_mainWidget->m_fontCombo, SIGNAL(activated(int)), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_fontSizeCombo, SIGNAL(activated(int)), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_boldButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_italicsButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_underLineButton, SIGNAL(clicked()), this, SLOT(updateFormating()));
  connect(m_mainWidget->m_textColorButton, SIGNAL(clicked()), this, SLOT(updateFormating()));

  m_mainWidget->m_textArea->setFocus();
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

void StencilTextEditor::setHorizontalAlign(int flag)
{
  if(m_hAlign >= 0) {
    m_mainWidget->m_hAlignButton->popup()->setItemChecked(m_hAlign, false);
  }

  m_mainWidget->m_hAlignButton->popup()->setItemChecked(flag, true);
  m_mainWidget->m_hAlignButton->setIconSet(*(m_mainWidget->m_hAlignButton->popup()->iconSet(flag)));
  m_mainWidget->m_textArea->setAlignment(flag|verticalAlignment());
  m_hAlign = flag;
}

void StencilTextEditor::setVerticalAlign(int flag)
{
  if(m_vAlign >= 0) {
    m_mainWidget->m_vAlignButton->popup()->setItemChecked(m_vAlign, false);
  }

  m_mainWidget->m_vAlignButton->popup()->setItemChecked(flag, true);
  m_mainWidget->m_vAlignButton->setIconSet(*(m_mainWidget->m_vAlignButton->popup()->iconSet(flag)));
  m_mainWidget->m_textArea->setAlignment(flag|horizontalAlignment());
  m_vAlign = flag;
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

Qt::AlignmentFlags StencilTextEditor::horizontalAlignment() const
{
  return static_cast<Qt::AlignmentFlags>(m_hAlign);
}

Qt::AlignmentFlags StencilTextEditor::verticalAlignment() const
{
  return static_cast<Qt::AlignmentFlags>(m_vAlign);
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
  m_mainWidget->m_textArea->setAlignment(horizontalAlignment()|verticalAlignment());
  m_mainWidget->m_textArea->selectAll(false);

  m_mainWidget->m_textArea->setFocus();
}

void StencilTextEditor::showHAlignPopup()
{
  m_mainWidget->m_hAlignButton->openPopup();
}

void StencilTextEditor::showVAlignPopup()
{
  m_mainWidget->m_vAlignButton->openPopup();
}

}

#include "stenciltexteditor.moc"
