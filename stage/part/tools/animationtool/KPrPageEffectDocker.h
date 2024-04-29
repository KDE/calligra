/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
   SPDX-FileCopyrightText: 2020 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGEEFFECTDOCKER_H
#define KPRPAGEEFFECTDOCKER_H

#include <QPixmap>
#include <QWidget>

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
    explicit KPrPageEffectDocker(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    void setView(KoPAViewBase *view);

public Q_SLOTS:
    void slotActivePageChanged();
    void slotApplyToAllSlides();
    void slotEffectChanged(int index);

protected:
    void updateSubTypes(const KPrPageEffectFactory *factory);
    KPrPageEffect *createPageEffect(const KPrPageEffectFactory *factory, int subType, double time);
    KUndo2Command *applyToAllSlidesCommand();

protected Q_SLOTS:
    void slotSubTypeChanged(int index);
    void slotDurationChanged(double duration);
    void slotTransitionChanged();

    void cleanup(QObject *object);

    void setEffectPreview();

private:
    KoPAViewBase *m_view;
    QComboBox *m_effectCombo;
    QComboBox *m_subTypeCombo;
    QPushButton *m_applyToAllSlidesButton;
    QDoubleSpinBox *m_durationSpinBox;
    QComboBox *m_transitionType;
    QDoubleSpinBox *m_transitionTime;
    KPrViewModePreviewPageEffect *m_previewMode;
};

#endif // KPRPAGEEFFECTDOCKER_H
