/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2004-2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LayoutPage_General.h"

#include "core/Style.h"
#include "core/StyleManager.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>

#include <KComboBox>
#include <KLineEdit>
#include <KLocalizedString>

using namespace Calligra::Sheets;

LayoutPageGeneral::LayoutPageGeneral(QWidget *parent, StyleManager *manager)
    : QWidget(parent)
    , m_manager(manager)
{
    QGridLayout *layout = new QGridLayout(this);

    QGroupBox *groupBox = new QGroupBox(this);
    groupBox->setTitle(i18n("Style"));

    QGridLayout *groupBoxLayout = new QGridLayout(groupBox);
    groupBoxLayout->setAlignment(Qt::AlignTop);

    QLabel *label1 = new QLabel(groupBox);
    label1->setText(i18nc("Name of the style", "Name:"));
    groupBoxLayout->addWidget(label1, 0, 0);

    m_nameEdit = new KLineEdit(groupBox);
    groupBoxLayout->addWidget(m_nameEdit, 0, 1);

    m_nameStatus = new QLabel(groupBox);
    m_nameStatus->hide();
    groupBoxLayout->addWidget(m_nameStatus, 1, 1);

    QLabel *label2 = new QLabel(groupBox);
    label2->setText(i18n("Inherit style:"));
    groupBoxLayout->addWidget(label2, 2, 0);

    m_parentBox = new KComboBox(false, groupBox);

    connect(m_parentBox, &KComboBox::textActivated, this, &LayoutPageGeneral::parentChanged);
    connect(m_nameEdit, &QLineEdit::textChanged, this, &LayoutPageGeneral::styleNameChanged);

    groupBoxLayout->addWidget(m_parentBox, 2, 1);

    m_parentStatus = new QLabel(groupBox);
    m_parentStatus->hide();
    groupBoxLayout->addWidget(m_parentStatus, 3, 1);

    QSpacerItem *spacer = new QSpacerItem(20, 260, QSizePolicy::Minimum, QSizePolicy::Expanding);

    layout->addWidget(groupBox, 0, 0);
    layout->addItem(spacer, 1, 0);

    resize(QSize(534, 447).expandedTo(minimumSizeHint()));
}

LayoutPageGeneral::~LayoutPageGeneral() = default;

void LayoutPageGeneral::styleNameChanged(const QString &name)
{
    if ((name != m_name) && (!m_manager->validateStyleName(name))) {
        m_nameStatus->setText(i18n("A style with this name already exists."));
        m_nameStatus->show();
        Q_EMIT validDataChanged(false);
    } else if (name.isEmpty()) {
        m_nameStatus->setText(i18n("The style name can not be empty."));
        m_nameStatus->show();
        Q_EMIT validDataChanged(false);
    } else {
        m_nameStatus->hide();
        Q_EMIT validDataChanged(true);
    }
}

void LayoutPageGeneral::parentChanged(const QString &parentName)
{
    if (m_nameEdit->text() == parentName) {
        m_parentStatus->setText(i18n("A style cannot inherit from itself."));
        m_parentStatus->show();
        Q_EMIT validDataChanged(false);
    } else if (!m_manager->checkCircle(m_nameEdit->text(), parentName)) {
        m_parentStatus->setText(i18n("The style cannot inherit from '%1' because of recursive references.", m_parentBox->currentText()));
        m_parentStatus->show();
        Q_EMIT validDataChanged(false);
    } else {
        m_parentStatus->hide();
        Q_EMIT validDataChanged(true);
    }
}

bool LayoutPageGeneral::apply(CustomStyle *style)
{
    if (m_nameEdit->isEnabled()) {
        if (style->type() != Style::BUILTIN) {
            QString name(style->name());
            style->setName(m_nameEdit->text());
            if (m_parentBox->isEnabled()) {
                if (m_parentBox->currentText() == i18n("Default") || m_parentBox->currentText().isEmpty())
                    style->clearAttribute(Style::NamedStyleKey);
                else
                    style->setParentName(m_parentBox->currentText());
            }
            m_manager->changeName(name, m_nameEdit->text());
        }
    }

    if (style->type() == Style::TENTATIVE)
        style->setType(Style::CUSTOM);

    return true;
}

void LayoutPageGeneral::loadFrom(const CustomStyle &style)
{
    m_name = style.name();
    m_nameEdit->setText(m_name);
    QString defname = i18n("Default");

    QStringList tmp = m_manager->styleNames();
    tmp.removeAll(m_name);
    tmp.removeAll(defname);
    m_parentBox->clear();
    // place the default style first
    m_parentBox->insertItem(0, defname);
    m_parentBox->insertItems(1, tmp);

    QString parent = style.parentName();
    if (parent.isNull())
        parent = defname;
    m_parentBox->setCurrentIndex(m_parentBox->findText(parent));

    bool enableName = (style.type() != Style::BUILTIN);
    m_nameEdit->setEnabled(enableName);
    m_parentBox->setEnabled(enableName);
}
