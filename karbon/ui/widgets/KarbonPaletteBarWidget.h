/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    explicit KarbonPaletteBarWidget(Qt::Orientation orientation, QWidget *parent = nullptr);
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
    KarbonPaletteWidget *m_colorBar;

    KoColorSet m_documentColors;
    KoResourceServerAdapter<KoColorSet> m_palettes;
};

#endif // KARBONPALETTEBARWIDGET_H
