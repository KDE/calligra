/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTEREFFECTEDITWIDGET_H
#define FILTEREFFECTEDITWIDGET_H

#include "FilterEffectScene.h"
#include "ui_FilterEffectEditWidget.h"
#include <QWidget>

class KoShape;
class KoCanvasBase;
class KoFilterEffect;
class KoFilterEffectStack;

class FilterEffectEditWidget : public QWidget, Ui::FilterEffectEditWidget
{
    Q_OBJECT
public:
    explicit FilterEffectEditWidget(QWidget *parent = nullptr);
    ~FilterEffectEditWidget() override;

    /// Edits effects of given shape
    void editShape(KoShape *shape, KoCanvasBase *canvas);

protected:
    /// reimplemented from QWidget
    void resizeEvent(QResizeEvent *event) override;
    /// reimplemented from QWidget
    void showEvent(QShowEvent *event) override;
private Q_SLOTS:
    void addSelectedEffect();
    void removeSelectedItem();
    void connectionCreated(ConnectionSource source, ConnectionTarget target);
    void addToPresets();
    void removeFromPresets();
    void presetSelected(KoResource *resource);
    void filterChanged();
    void sceneSelectionChanged();
    void defaultSourceChanged(int);

private:
    void fitScene();
    void addWidgetForItem(ConnectionSource item);

    FilterEffectScene *m_scene;
    KoShape *m_shape;
    KoCanvasBase *m_canvas;
    KoFilterEffectStack *m_effects;
    ConnectionSource m_currentItem;
    KComboBox *m_defaultSourceSelector;
};

#endif // FILTEREFFECTEDITWIDGET_H
