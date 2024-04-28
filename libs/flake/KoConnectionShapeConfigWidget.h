/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOCONNECTIONSHAPECONFIGWIDGET_H
#define KOCONNECTIONSHAPECONFIGWIDGET_H

#include "ui_KoConnectionShapeConfigWidget.h"

#include <KoShapeConfigWidgetBase.h>

#include "flake_export.h"

class KoConnectionShape;

class FLAKE_EXPORT KoConnectionShapeConfigWidget : public KoShapeConfigWidgetBase
{
    Q_OBJECT
public:
    KoConnectionShapeConfigWidget();
    /// reimplemented
    void open(KoShape *shape) override;
    /// reimplemented
    void save() override;
    /// reimplemented
    bool showOnShapeCreate() override
    {
        return false;
    }
    /// reimplemented
    KUndo2Command *createCommand() override;

Q_SIGNALS:
    void connectionTypeChanged(int type);

public Q_SLOTS:
    void setConnectionType(int type);

private:
    Ui::KoConnectionShapeConfigWidget widget;
    KoConnectionShape *m_connection;
};

#endif // KOCONNECTIONSHAPECONFIGWIDGET_H
