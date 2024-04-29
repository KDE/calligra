/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1999-2005 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FindDialog.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <KComboBox>
#include <KLocalizedString>

using namespace Calligra::Sheets;

FindOption::FindOption(QWidget *parent)
{
    QVBoxLayout *layout = new QVBoxLayout(parent);
    m_moreOptions = new QPushButton(i18n("More Options"), parent);
    layout->addWidget(m_moreOptions);

    connect(m_moreOptions, &QAbstractButton::clicked, this, &FindOption::slotMoreOptions);

    m_findExtension = new QWidget(parent);
    layout->addWidget(m_findExtension);
    QVBoxLayout *layout1 = new QVBoxLayout(m_findExtension);
    m_searchInAllSheet = new QCheckBox(i18n("Search entire sheet"), m_findExtension);
    layout1->addWidget(m_searchInAllSheet);

    QHBoxLayout *comboLayout = new QHBoxLayout();
    QLabel *label = new QLabel(i18n("Search in:"), m_findExtension);
    comboLayout->addWidget(label);

    m_searchIn = new KComboBox(m_findExtension);
    comboLayout->addWidget(m_searchIn);
    layout1->addLayout(comboLayout);

    QStringList lst;
    lst << i18n("Cell Values");
    lst << i18n("Comments");
    m_searchIn->insertItems(0, lst);

    comboLayout = new QHBoxLayout();
    label = new QLabel(i18n("Search direction:"), m_findExtension);
    comboLayout->addWidget(label);

    m_searchDirection = new KComboBox(m_findExtension);
    comboLayout->addWidget(m_searchDirection);
    layout1->addLayout(comboLayout);

    lst.clear();
    lst << i18n("Across then Down");
    lst << i18n("Down then Across");
    m_searchDirection->insertItems(0, lst);

    m_findExtension->hide();
    Q_EMIT adjustSize();
}

FindOption::searchTypeValue FindOption::searchType() const
{
    int pos = m_searchIn->currentIndex();
    if (pos == 0)
        return Value;
    else if (pos == 1)
        return Note;
    else
        return Value;
}

FindOption::searchDirectionValue FindOption::searchDirection() const
{
    int pos = m_searchDirection->currentIndex();
    if (pos == 0)
        return Row;
    else if (pos == 1)
        return Column;
    else
        return Row;
}

void FindOption::slotMoreOptions()
{
    if (m_findExtension->isHidden()) {
        m_findExtension->show();
        m_moreOptions->setText(i18n("Fewer Options"));
    } else {
        m_findExtension->hide();
        m_moreOptions->setText(i18n("More Options"));
    }
    Q_EMIT adjustSize();
}

bool FindOption::searchInAllSheet() const
{
    return m_searchInAllSheet->isChecked();
}

FindDlg::FindDlg(QWidget *parent, const QString &name, long options, const QStringList &findStrings, bool hasSelection)
    : KFindDialog(parent, options, findStrings, hasSelection)
{
    setObjectName(name);
    m_findOptions = new FindOption(findExtension());
    connect(m_findOptions, &FindOption::adjustSize, this, &FindDlg::slotAjustSize);
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

FindDlg::~FindDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void FindDlg::slotAjustSize()
{
    adjustSize();
}

bool FindDlg::searchInAllSheet() const
{
    return m_findOptions->searchInAllSheet();
}

SearchDlg::SearchDlg(QWidget *parent, const QString &name, long options, const QStringList &findStrings, const QStringList &replaceStrings, bool hasSelection)
    : KReplaceDialog(parent, options, findStrings, replaceStrings, hasSelection)
{
    setObjectName(name);
    m_findOptions = new FindOption(findExtension());
    connect(m_findOptions, &FindOption::adjustSize, this, &SearchDlg::slotAjustSize);
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
}

SearchDlg::~SearchDlg()
{
    // no need to delete child widgets, Qt does it all for us
}

void SearchDlg::slotAjustSize()
{
    adjustSize();
    setFixedSize(size());
}

bool SearchDlg::searchInAllSheet() const
{
    return m_findOptions->searchInAllSheet();
}
