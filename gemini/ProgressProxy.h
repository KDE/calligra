/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 KO GmbH. Contact : Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef CALLIGRAGEMINI_PROGRESSPROXY_H
#define CALLIGRAGEMINI_PROGRESSPROXY_H

#include <KoProgressProxy.h>
#include <QObject>

class ProgressProxy : public QObject, public KoProgressProxy
{
    Q_OBJECT
    Q_PROPERTY(QString taskName READ taskName NOTIFY taskNameChanged)

public:
    explicit ProgressProxy(QObject *parent = nullptr);
    ~ProgressProxy() override;

    QString taskName() const;

    void setFormat(const QString &format) override;
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
