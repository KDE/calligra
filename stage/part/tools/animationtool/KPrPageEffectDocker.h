/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>
   Copyright (C) 2020 Dag Andersen <danders@get2net.dk>

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

class QComboBox;
class QDoubleSpinBox;
class QTimeEdit;
class QPushButton;
class KPrPageEffect;
class KPrPageEffectFactory;
class KoPAViewBase;
class KPrViewModePreviewPageEffect;
class KUndo2Command;

/**
 * This is the page effect docker widget that let's you choose a page animation.
 */
class KPrPageEffectDocker : public QWidget
{

    Q_OBJECT
public:
    explicit KPrPageEffectDocker( QWidget* parent = 0, Qt::WindowFlags flags = 0 );

    void setView( KoPAViewBase* view );

public Q_SLOTS:
    void slotActivePageChanged();
    void slotApplyToAllSlides();
    void slotEffectChanged( int index );

protected:
    void updateSubTypes( const KPrPageEffectFactory * factory );
    KPrPageEffect * createPageEffect( const KPrPageEffectFactory * factory, int subType, double time );
    KUndo2Command * applyToAllSlidesCommand();

protected Q_SLOTS:
    void slotSubTypeChanged( int index );
    void slotDurationChanged( double duration );
    void slotTransitionChanged();

    void cleanup( QObject* object );

    void setEffectPreview();

private:
    KoPAViewBase* m_view;
    QComboBox* m_effectCombo;
    QComboBox* m_subTypeCombo;
    QPushButton *m_applyToAllSlidesButton;
    QDoubleSpinBox* m_durationSpinBox;
    QComboBox *m_transitionType;
    QDoubleSpinBox* m_transitionTime;
    KPrViewModePreviewPageEffect *m_previewMode;
};

#endif // KPRPAGEEFFECTDOCKER_H
