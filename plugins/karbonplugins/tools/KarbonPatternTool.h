/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef _KARBONPATTERNTOOL_H_
#define _KARBONPATTERNTOOL_H_

#include <KoToolBase.h>
#include <QMap>

class QPainter;
class KoResource;
class KarbonPatternEditStrategyBase;
class KarbonPatternOptionsWidget;
class KoShape;

class KarbonPatternTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KarbonPatternTool(KoCanvasBase *canvas);
    ~KarbonPatternTool() override;

    void paint(QPainter &painter, const KoViewConverter &converter) override;
    void repaintDecorations() override;

    void mousePressEvent(KoPointerEvent *event) override;
    void mouseMoveEvent(KoPointerEvent *event) override;
    void mouseReleaseEvent(KoPointerEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    void deactivate() override;

public Q_SLOTS:
    void documentResourceChanged(int key, const QVariant &res) override;

protected:
    QList<QPointer<QWidget>> createOptionWidgets() override;

private Q_SLOTS:
    void patternSelected(KoResource *resource);
    void initialize();
    /// updates options widget from selected pattern
    void updateOptionsWidget();
    void patternChanged();

private:
    QMap<KoShape *, KarbonPatternEditStrategyBase *> m_strategies; ///< the list of editing strategies, one for each shape
    KarbonPatternEditStrategyBase *m_currentStrategy; ///< the current editing strategy
    KarbonPatternOptionsWidget *m_optionsWidget;
};

#endif // _KARBONPATTERNTOOL_H_
