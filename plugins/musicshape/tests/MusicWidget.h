/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Marijn Kruisselbrink <mkruisselbrink@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include "../Engraver.h"
#include "../MusicStyle.h"
#include "../Renderer.h"
#include <QWidget>

namespace MusicCore
{
class Sheet;
}
class Engraver;

class MusicWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MusicWidget(QWidget *parent = nullptr);

    /**
     * Set the sheet that is displayed in this MusicWidget. The widget doesn't take ownership of the sheet,
     * (unless it is already the parent QObject of it), but it is still probably not a good idea to have the
     * same sheet in multiple widgets, as the layout of the sheet is stored in the sheet itself.
     */
    void setSheet(MusicCore::Sheet *sheet);

    /**
     * Get the sheet that is currently displayed in this widget.
     */
    MusicCore::Sheet *sheet() const;

    /**
     * Sets the scale at which the music should be painted in this widget.
     */
    void setScale(qreal scale);

    /**
     * Gets the scale at which the music is painted in this widget.
     */
    qreal scale() const;
public Q_SLOTS:
    /**
     * Call this method when the contents of the sheet have changed in such a way that a re-engraving
     * of the music is required. This is done automatically if the widget changes size or scale.
     */
    void engrave();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    Engraver m_engraver;
    MusicStyle m_style;
    MusicRenderer m_renderer;
    MusicCore::Sheet *m_sheet;
    qreal m_scale;
    int m_lastSystem;
};

#endif // MUSICWIDGET_H
