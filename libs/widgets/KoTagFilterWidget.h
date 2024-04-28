/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *    SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTAGFILTERWIDGET_H
#define KOTAGFILTERWIDGET_H

#include <QWidget>

class KoTagFilterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KoTagFilterWidget(QWidget *parent);
    ~KoTagFilterWidget() override;
    void allowSave(bool allow);
    void clear();
Q_SIGNALS:
    void filterTextChanged(const QString &filterText);
    void saveButtonClicked();
private Q_SLOTS:
    void onTextChanged(const QString &lineEditText);
    void onSaveButtonClicked();

private:
    class Private;
    Private *const d;
};

#endif // KOTAGFILTERWIDGET_H
