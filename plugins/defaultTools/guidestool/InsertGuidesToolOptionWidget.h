/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Carlos Licea <carlos.licea@kdemail.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INSERTGUIDESTOOLOPTIONWIDGET_H
#define INSERTGUIDESTOOLOPTIONWIDGET_H

#include <ui_InsertGuidesToolOptionWidget.h>

#include <QWidget>

// This is the resulting transaction to be applied.
// NOTE: it is a class instead of a struct so to be able to forward include i
class GuidesTransaction
{
public:
    bool insertVerticalEdgesGuides;
    bool insertHorizontalEdgesGuides;
    bool erasePreviousGuides;
    int verticalGuides;
    int horizontalGuides;
};

class InsertGuidesToolOptionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InsertGuidesToolOptionWidget(QWidget *parent = nullptr);
    ~InsertGuidesToolOptionWidget() override;

Q_SIGNALS:
    void createGuides(GuidesTransaction *transaction);

private Q_SLOTS:
    void onCreateButtonClicked(bool checked);

private:
    Ui_InsertGuidesToolOptionWidget m_widget;
};

#endif
