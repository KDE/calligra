/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "UserVariableOptionsWidget.h"

#include "UserVariable.h"
#include "VariablesDebug.h"

#include <KoVariableManager.h>

#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QValidator>

#include <KLocalizedString>
#include <KMessageBox>

UserVariableOptionsWidget::UserVariableOptionsWidget(UserVariable *userVariable, QWidget *parent)
    : QWidget(parent)
    , userVariable(userVariable)
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setColumnStretch(1, 1);
    setLayout(layout);

    QLabel *nameLabel = new QLabel(i18n("Name:"), this);
    nameLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(nameLabel, 0, 0);
    QHBoxLayout *nameLayout = new QHBoxLayout();
    nameEdit = new QComboBox(this);
    nameEdit->setObjectName(QLatin1String("nameEdit"));
    nameEdit->setMinimumContentsLength(10);
    nameLabel->setBuddy(nameEdit);
    connect(nameEdit, &QComboBox::currentIndexChanged, this, &UserVariableOptionsWidget::nameChanged);
    nameLayout->addWidget(nameEdit);

    newButton = new QPushButton(i18n("New"), this);
    connect(newButton, &QAbstractButton::clicked, this, &UserVariableOptionsWidget::newClicked);
    nameLayout->addWidget(newButton);

    deleteButton = new QPushButton(i18n("Delete"), this);
    deleteButton->setObjectName("DeleteButton");
    connect(deleteButton, &QAbstractButton::clicked, this, &UserVariableOptionsWidget::deleteClicked);
    nameLayout->addWidget(deleteButton);

    layout->addLayout(nameLayout, 0, 1);

    QLabel *typeLabel = new QLabel(i18n("Format:"), this);
    typeLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(typeLabel, 1, 0);
    typeEdit = new QComboBox(this);
    typeEdit->setObjectName(QLatin1String("typeEdit"));
    typeLabel->setBuddy(typeEdit);
    typeEdit->addItem(i18n("String"), QLatin1String("string"));
    typeEdit->addItem(i18n("Boolean"), QLatin1String("boolean"));
    typeEdit->addItem(i18n("Float"), QLatin1String("float"));
    typeEdit->addItem(i18n("Percentage"), QLatin1String("percentage"));
    typeEdit->addItem(i18n("Currency"), QLatin1String("currency"));
    typeEdit->addItem(i18n("Date"), QLatin1String("date"));
    typeEdit->addItem(i18n("Time"), QLatin1String("time"));
    typeEdit->addItem(i18n("Formula"), QLatin1String("formula"));
    typeEdit->addItem(i18n("Void"), QLatin1String("void"));
    typeEdit->setCurrentIndex(qMax(0, typeEdit->findData(variableManager()->userType(userVariable->name()))));
    connect(typeEdit, &QComboBox::currentIndexChanged, this, &UserVariableOptionsWidget::typeChanged);
    layout->addWidget(typeEdit, 1, 1);

    QLabel *valueLabel = new QLabel(i18n("Value:"), this);
    valueLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(valueLabel, 2, 0);
    valueEdit = new QLineEdit(this);
    valueEdit->setObjectName(QLatin1String("valueEdit"));
    valueLabel->setBuddy(valueEdit);
    valueEdit->setText(variableManager()->value(userVariable->name()));
    connect(valueEdit, &QLineEdit::textChanged, this, &UserVariableOptionsWidget::valueChanged);
    layout->addWidget(valueEdit, 2, 1);

    updateNameEdit();
}

UserVariableOptionsWidget::~UserVariableOptionsWidget() = default;

KoVariableManager *UserVariableOptionsWidget::variableManager()
{
    return userVariable->variableManager();
}

void UserVariableOptionsWidget::nameChanged()
{
    bool enabled = !variableManager()->userVariables().isEmpty();

    nameEdit->setEnabled(enabled);
    userVariable->setName(nameEdit->currentText());

    bool wasBlocked = typeEdit->blockSignals(true);
    typeEdit->setCurrentIndex(qMax(0, typeEdit->findData(variableManager()->userType(userVariable->name()))));
    typeEdit->blockSignals(wasBlocked);
    typeEdit->setEnabled(enabled);

    wasBlocked = valueEdit->blockSignals(true);
    valueEdit->setText(variableManager()->value(userVariable->name()));
    valueEdit->blockSignals(wasBlocked);
    valueEdit->setEnabled(enabled);

    deleteButton->setEnabled(enabled);
}

void UserVariableOptionsWidget::typeChanged()
{
    QString value = variableManager()->value(userVariable->name());
    QString type = typeEdit->itemData(typeEdit->currentIndex()).toString();
    variableManager()->setValue(userVariable->name(), value, type);
    // userVariable->valueChanged();
}

void UserVariableOptionsWidget::valueChanged()
{
    QString value = valueEdit->text();
    QString type = variableManager()->userType(userVariable->name());
    variableManager()->setValue(userVariable->name(), value, type);
    // userVariable->valueChanged();
}

void UserVariableOptionsWidget::newClicked()
{
    class Validator : public QValidator
    {
    public:
        Validator(KoVariableManager *variableManager)
            : m_variableManager(variableManager)
        {
        }
        State validate(QString &input, int &) const override
        {
            QString s = input.trimmed();
            return s.isEmpty() || m_variableManager->userVariables().contains(s) ? Intermediate : Acceptable;
        }

    private:
        KoVariableManager *m_variableManager;
    };
    Validator validator(variableManager());
    QString name = QInputDialog::getText(this, i18n("New Variable"), i18n("Name for new variable:") /*QT5TODO:, &validator*/).trimmed();
    if (name.isEmpty()) {
        return;
    }
    userVariable->setName(name);
    variableManager()->setValue(userVariable->name(), QString(), QLatin1String("string"));
    updateNameEdit();
    valueEdit->setFocus();
}

void UserVariableOptionsWidget::deleteClicked()
{
    if (!variableManager()->userVariables().contains(userVariable->name())) {
        return;
    }
    if (KMessageBox::questionTwoActions(this,
                                        i18n("Delete variable <b>%1</b>?", userVariable->name()),
                                        i18n("Delete Variable"),
                                        KStandardGuiItem::del(),
                                        KStandardGuiItem::cancel(),
                                        QString(),
                                        KMessageBox::Dangerous | KMessageBox::Notify)
        != KMessageBox::PrimaryAction) {
        return;
    }
    variableManager()->remove(userVariable->name());
    userVariable->setName(QString());
    updateNameEdit();
}

void UserVariableOptionsWidget::updateNameEdit()
{
    QStringList names = variableManager()->userVariables();
    bool wasBlocked = nameEdit->blockSignals(true);
    nameEdit->clear();
    nameEdit->addItems(names);
    nameEdit->blockSignals(wasBlocked);
    if (userVariable->name().isNull() && !names.isEmpty()) {
        userVariable->setName(names.first());
    }
    nameEdit->setCurrentIndex(qMax(0, names.indexOf(userVariable->name())));
    nameChanged();
}
