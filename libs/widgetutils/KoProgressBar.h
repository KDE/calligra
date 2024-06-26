/* This file is part of the KDE project
 * Copyright (C) Boudewijn Rempt <boud@valdyas.org>, (C) 2009
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPROGRESSBAR_H
#define KOPROGRESSBAR_H

#include "kowidgetutils_export.h"
#include <KoProgressProxy.h>
#include <QProgressBar>

/**
 * KoProgressBar is a thin wrapper around QProgressBar that also implements
 * the abstract base class KoProgressProxy. Use this class, not QProgressBar
 * to pass to KoProgressUpdater.
 */
class KOWIDGETUTILS_EXPORT KoProgressBar : public QProgressBar, public KoProgressProxy
{
    Q_OBJECT
public:
    explicit KoProgressBar(QWidget *parent = nullptr);

    ~KoProgressBar() override;

    int maximum() const override;
    void setValue(int value) override;
    void setRange(int minimum, int maximum) override;
    void setFormat(const QString &format) override;

Q_SIGNALS:

    void done();
};

#endif
