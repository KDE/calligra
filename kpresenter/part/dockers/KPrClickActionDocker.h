/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>

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

#ifndef KPRCLICKACTIONDOCKER_H
#define KPRCLICKACTIONDOCKER_H

#include <QDockWidget>
#include <QPixmap>
#include <KoCanvasObserver.h>

class QLabel;
class QComboBox;
class QDoubleSpinBox;
class KPrView;
class KoCanvasBase;
class QCheckBox;
class KPrSoundCollection;

/**
 * This is the page effect docker widget that let's you choose a page animation.
 */
class KPrClickActionDocker : public QDockWidget, public KoCanvasObserver
{

    Q_OBJECT
public:
    KPrClickActionDocker( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    void setView( KPrView* view );

private slots:
    /// selection has changed
    void selectionChanged();

    /// reimplemented
    virtual void setCanvas( KoCanvasBase *canvas );

    void soundComboChanged();
private:

    KPrView *m_view;
    KPrSoundCollection *m_soundCollection;
    KoCanvasBase *m_canvas;
    QCheckBox *m_cbNavigate;
    QComboBox *m_cbPlaySound;
};

#endif // KPRCLICKACTIONDOCKER_H

