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
#ifndef KIVIOSTENCILTEXTEDITOR_H
#define KIVIOSTENCILTEXTEDITOR_H

#include <kdialogbase.h>

class QFont;
class QColor;

namespace Kivio {

class StencilTextEditorUI;

/**
 * Dialog for editing stencil text
 */
class StencilTextEditor : public KDialogBase
{
  Q_OBJECT
  public:
    StencilTextEditor(const QString& caption, QWidget *parent = 0, const char *name = 0);
    ~StencilTextEditor();

    void setFont(const QFont& font);
    void setFontColor(const QColor& color);
    void setBackgroundColor(const QColor& color);

    QFont font() const;
    QColor fontColor() const;
    Qt::AlignmentFlags horizontalAlignment() const;
    Qt::AlignmentFlags verticalAlignment() const;

    void setText(const QString& text);
    QString text() const;

  public slots:
    void setHorizontalAlign(int flag);
    void setVerticalAlign(int flag);

  protected slots:
    void updateFormating();

    void showHAlignPopup();
    void showVAlignPopup();

  private:
    StencilTextEditorUI* m_mainWidget;

    int m_hAlign;
    int m_vAlign;
};

}

#endif
