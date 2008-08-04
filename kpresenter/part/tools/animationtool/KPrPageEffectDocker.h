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

#ifndef KPRPAGEEFFECTDOCKER_H
#define KPRPAGEEFFECTDOCKER_H

#include <QWidget>
#include <QPixmap>

class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QTimer;
class KPrPageEffect;
class KPrPageEffectRunner;
class KPrPageEffectFactory;
class KoPAView;
class KPrPreviewWidget;

/**
 * This is the page effect docker widget that let's you choose a page animation.
 */
class KPrPageEffectDocker : public QWidget
{

    Q_OBJECT
public:
    KPrPageEffectDocker( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    void setView( KoPAView* view );

public slots:
    void slotActivePageChanged();

    void slotEffectChanged( int index );

protected:
    bool eventFilter( QObject* object, QEvent* event );
    void updateSubTypes( const KPrPageEffectFactory * factory );
    KPrPageEffect * createPageEffect( const KPrPageEffectFactory * factory, int subType, double time );

protected slots:
    void slotSubTypeChanged( int index );
    void slotDurationChanged( double duration );

    void cleanup( QObject* object );

    void setEffectPreview();

private:
    KPrPageEffectRunner* m_runner;
    KoPAView* m_view;
    QComboBox* m_effectCombo;
    QComboBox* m_subTypeCombo;
    QDoubleSpinBox* m_durationSpinBox;
    KPrPreviewWidget* m_preview;
    QPixmap m_activePageBuffer;

    QTimer* m_updateTimer;
};

#endif // KPRPAGEEFFECTDOCKER_H

