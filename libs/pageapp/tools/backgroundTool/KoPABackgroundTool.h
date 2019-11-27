/* This file is part of the KDE project
 * Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.net>
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

    ///Reimplemented from KoToolBase
    void paint( QPainter &painter, const KoViewConverter &converter ) override;
    ///Reimplemented from KoToolBase
    void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes) override;
    ///Reimplemented from KoToolBase
    void deactivate() override;
    ///Reimplemented from KoToolBase
    void mousePressEvent( KoPointerEvent *event ) override;
    ///Reimplemented from KoToolBase
    void mouseMoveEvent( KoPointerEvent *event ) override;
    ///Reimplemented from KoToolBase
    void mouseReleaseEvent( KoPointerEvent *event ) override;

    KoPAViewBase * view() const;

    void addOptionWidget(QWidget *widget);
public Q_SLOTS:
    void slotActivePageChanged();

protected:
    ///Reimplemented from KoToolBase
    QList<QPointer<QWidget> > createOptionWidgets() override;

private:
    KoPAViewBase * m_view;
    QList<QPointer<QWidget> > m_addOnWidgets;
};

#endif //KOPABACKGROUNDTOOL_H
