/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef USERVARIABLEOPTIONSWIDGET_H
#define USERVARIABLEOPTIONSWIDGET_H

#include <QWidget>

class QLineEdit;
class QComboBox;
class QPushButton;
class KoVariableManager;
class UserVariable;

class UserVariableOptionsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UserVariableOptionsWidget(UserVariable *userVariable, QWidget *parent = nullptr);
    ~UserVariableOptionsWidget() override;
private Q_SLOTS:
    void nameChanged();
    void typeChanged();
    void valueChanged();
    void newClicked();
    void deleteClicked();

private:
    KoVariableManager *variableManager();
    void updateNameEdit();

    UserVariable *userVariable;
    QComboBox *nameEdit;
    QComboBox *typeEdit;
    QLineEdit *valueEdit;
    QPushButton *newButton, *deleteButton;
};

#endif
