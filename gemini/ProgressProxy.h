/* This file is part of the KDE project
 * Copyright (C) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@kogmbh.com>
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
#ifndef CALLIGRAGEMINI_PROGRESSPROXY_H
#define CALLIGRAGEMINI_PROGRESSPROXY_H

#include <QObject>
#include <KoProgressProxy.h>

class ProgressProxy : public QObject, public KoProgressProxy
{
    Q_OBJECT
    Q_PROPERTY(QString taskName READ taskName NOTIFY taskNameChanged)

public:
    explicit ProgressProxy(QObject* parent = 0);
    ~ProgressProxy() override;

    QString taskName() const;

    void setFormat(const QString& format) override;
    void setRange(int minimum, int maximum) override;
    void setValue(int value) override;
    int maximum() const override;

Q_SIGNALS:
    void valueChanged(int value);
    void taskStarted();
    void taskEnded();
    void taskNameChanged();

private:
    class Private;
    const QScopedPointer<Private> d;
};

#endif // CALLIGRAGEMINI_ROGRESSPROXY_H
