/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KARBONGRADIENTTOOL_H
#define KARBONGRADIENTTOOL_H

#include <KoSnapGuide.h>
#include <KoToolBase.h>
#include <QGradient>
#include <QMultiMap>

class GradientStrategy;
class KoGradientEditWidget;
class KUndo2Command;
class KoShape;
class KoResource;

/**
 * A tool for editing gradient backgrounds of shapes.
 * The gradients can be edited by moving gradient
 * handles directly on the canvas.
 */
class KarbonGradientTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KarbonGradientTool(KoCanvasBase *canvas);
    ~KarbonGradientTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void repaintDecorations() override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;

public Q_SLOTS:
    void documentResourceChanged(int key, const QVariant &res) override;

protected:
    /// reimplemented from KoToolBase
    QList<QPointer<QWidget>> createOptionWidgets() override;

private Q_SLOTS:
    void initialize();
    void gradientChanged();
    void gradientSelected(KoResource *);

private:
    QGradient *m_gradient;
    QMultiMap<KoShape *, GradientStrategy *> m_strategies; ///< the list of gradient strategies
    GradientStrategy *m_currentStrategy; ///< the current editing strategy
    GradientStrategy *m_hoverStrategy; ///< the strategy the mouse hovers over
    KoGradientEditWidget *m_gradientWidget;
    KUndo2Command *m_currentCmd;
    KoSnapGuide::Strategies m_oldSnapStrategies; ///< the previously enables snap strategies
};

#endif // KARBONGRADIENTTOOL_H
