/* This file is part of the KDE project
 * Copyright (C) 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KARBONPALETTEBARWIDGET_H
#define KARBONPALETTEBARWIDGET_H

#include <KoColorSet.h>
#include <KoResourceServerAdapter.h>
#include <QWidget>

class QToolButton;
class KarbonPaletteWidget;

class KarbonPaletteBarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KarbonPaletteBarWidget(Qt::Orientation orientation, QWidget *parent = 0);
    ~KarbonPaletteBarWidget() override;

    /// Sets the orientation of the color bar
    void setOrientation(Qt::Orientation orientation);

public Q_SLOTS:
    void updateDocumentColors();

Q_SIGNALS:
    /// Emitted whenever a color was clicked
    void colorSelected(const KoColor &color);

private Q_SLOTS:
    void updateButtons();
    void selectPalette();

private:
    void createLayout();

    QToolButton *m_prevButton;
    QToolButton *m_nextButton;
    QToolButton *m_choosePalette;
    KarbonPaletteWidget * m_colorBar;

    KoColorSet m_documentColors;
    KoResourceServerAdapter<KoColorSet> m_palettes;
};

#endif // KARBONPALETTEBARWIDGET_H
