/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPOSITIONSELECTOR_H
#define KOPOSITIONSELECTOR_H

#include "kowidgets_export.h"
#include <KoFlake.h>
#include <QWidget>

/**
 * Widget to show a set of radio buttons so the user can select a position.
 */
class KOWIDGETS_EXPORT KoPositionSelector : public QWidget
{
    Q_OBJECT
public:
    explicit KoPositionSelector(QWidget *parent);
    ~KoPositionSelector() override;

    KoFlake::Position position() const;
    void setPosition(KoFlake::Position position);

Q_SIGNALS:
    void positionSelected(KoFlake::Position position);

protected:
    /// reimplemented
    void paintEvent(QPaintEvent *event) override;

private Q_SLOTS:
    void positionChanged(int position);

private:
    class Private;
    Private *const d;
};

#endif
