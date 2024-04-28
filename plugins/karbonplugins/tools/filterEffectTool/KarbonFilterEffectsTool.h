/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009-2010 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KARBONFILTEREFFECTSTOOL_H
#define KARBONFILTEREFFECTSTOOL_H

#include "KoInteractionTool.h"

class KoResource;
class KoInteractionStrategy;

class KarbonFilterEffectsTool : public KoInteractionTool
{
    Q_OBJECT
public:
    enum EditMode { None, MoveAll, MoveLeft, MoveRight, MoveTop, MoveBottom };

    explicit KarbonFilterEffectsTool(KoCanvasBase *canvas);
    ~KarbonFilterEffectsTool() override;

    /// reimplemented from KoToolBase
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    /// reimplemented from KoToolBase
    void repaintDecorations() override;
    /// reimplemented from KoToolBase
    void mouseMoveEvent(KoPointerEvent *event) override;

    /// reimplemented from KoToolBase
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;

protected:
    /// reimplemented from KoToolBase
    QList<QPointer<QWidget>> createOptionWidgets() override;
    /// reimplemented from KoToolBase
    KoInteractionStrategy *createStrategy(KoPointerEvent *event) override;
private Q_SLOTS:
    void editFilter();
    void clearFilter();
    void filterChanged();
    void filterSelected(int index);
    void slotSelectionChanged();
    void presetSelected(KoResource *resource);
    void regionXChanged(double x);
    void regionYChanged(double y);
    void regionWidthChanged(double width);
    void regionHeightChanged(double height);

private:
    class Private;
    Private *const d;
};

#endif // KARBONFILTEREFFECTSTOOL_H
