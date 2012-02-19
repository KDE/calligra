/*
 *  Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_COLOR_SELECTOR_H
#define KIS_COLOR_SELECTOR_H

#include "kis_color_selector_base.h"
#include "kis_color_selector_surface.h"

class QPushButton;

class KisColorSelector : public KisColorSelectorBase
{
    Q_OBJECT
public:

//    enum MainType {Ring, Square, Wheel};
//    enum SubType {Triangle, Square, Slider};
//    enum MainTypeParameter {SL, SV, SH, VH, LH, VSV/*experimental*/};
//    enum SubTypeParameter {H, S, V, L};

    KisColorSelector(KisColorSelectorSurface::Configuration conf, QWidget* parent = 0);
    KisColorSelector(QWidget* parent=0);
    KisColorSelectorBase* createPopup() const;

    void setConfiguration(KisColorSelectorSurface::Configuration conf);
    KisColorSelectorSurface::Configuration configuration() const;
    void setColor(const QColor& color);

public slots:
    void updateSettings();

signals:
    void settingsButtonClicked();

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    bool displaySettingsButton();


private:
    void init();

    QPushButton* m_button;

    QTimer* m_updateTimer;

    QColor m_lastColor;
    
    KisColorSelectorSurface m_surface;
};

#endif // KIS_COLSELNG_COLOR_SELECTOR_H
