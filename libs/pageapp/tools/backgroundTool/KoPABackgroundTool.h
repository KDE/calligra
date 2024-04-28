/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPABACKGROUNDTOOL_H
#define KOPABACKGROUNDTOOL_H

#include "kopageapp_export.h"

#include <KoToolBase.h>

class KoPAViewBase;

class KOPAGEAPP_EXPORT KoPABackgroundTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KoPABackgroundTool(KoCanvasBase *base);
    ~KoPABackgroundTool() override;

    /// Reimplemented from KoToolBase
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    /// Reimplemented from KoToolBase
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// Reimplemented from KoToolBase
    void deactivate() override;
    /// Reimplemented from KoToolBase
    void mousePressEvent(KoPointerEvent *event) override;
    /// Reimplemented from KoToolBase
    void mouseMoveEvent(KoPointerEvent *event) override;
    /// Reimplemented from KoToolBase
    void mouseReleaseEvent(KoPointerEvent *event) override;

    KoPAViewBase *view() const;

    void addOptionWidget(QWidget *widget);
public Q_SLOTS:
    void slotActivePageChanged();

protected:
    /// Reimplemented from KoToolBase
    QList<QPointer<QWidget>> createOptionWidgets() override;

private:
    KoPAViewBase *m_view;
    QList<QPointer<QWidget>> m_addOnWidgets;
};

#endif // KOPABACKGROUNDTOOL_H
