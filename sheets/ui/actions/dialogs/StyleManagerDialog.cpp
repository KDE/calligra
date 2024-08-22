/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2003 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "StyleManagerDialog.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <KComboBox>
#include <KLocalizedString>

#include "core/Style.h"
#include "core/StyleManager.h"

#include "ui/Selection.h"
#include "ui/dialogs/LayoutDialog.h"

using namespace Calligra::Sheets;

StyleManagerDialog::StyleManagerDialog(QWidget *parent, Selection *selection, StyleManager *manager)
    : ActionDialog(parent)
    , m_selection(selection)
    , m_styleManager(manager)
{
    setCaption(i18n("Style Manager"));

    QWidget *widget = new QWidget(this);
    setMainWidget(widget);

    QHBoxLayout *hboxLayout = new QHBoxLayout(widget);
    hboxLayout->setContentsMargins({});

    QVBoxLayout *layout = new QVBoxLayout();

    m_styleList = new QTreeWidget(this);
    m_styleList->setHeaderLabel(i18n("Style"));
    layout->addWidget(m_styleList);

    m_displayBox = new KComboBox(false, this);
    m_displayBox->insertItem(0, i18n("All Styles"));
    m_displayBox->insertItem(1, i18n("Custom Styles"));
    m_displayBox->insertItem(2, i18n("Hierarchical"));
    layout->addWidget(m_displayBox);

    hboxLayout->addLayout(layout);

    // list buttons
    QVBoxLayout *listButtonLayout = new QVBoxLayout();

    m_newButton = new QPushButton(i18n("&New..."), this);
    listButtonLayout->addWidget(m_newButton);
    m_modifyButton = new QPushButton(i18n("&Modify..."), this);
    listButtonLayout->addWidget(m_modifyButton);
    m_deleteButton = new QPushButton(i18n("&Delete..."), this);
    listButtonLayout->addWidget(m_deleteButton);
    listButtonLayout->addStretch(1);

    hboxLayout->addLayout(listButtonLayout);

    slotDisplayMode(0);
    m_newButton->setEnabled(true);
    m_modifyButton->setEnabled(true);
    m_deleteButton->setEnabled(false);

    connect(m_displayBox, QOverload<int>::of(&KComboBox::activated), this, &StyleManagerDialog::slotDisplayMode);
    connect(m_newButton, &QAbstractButton::clicked, this, &StyleManagerDialog::slotNew);
    connect(m_modifyButton, &QAbstractButton::clicked, this, &StyleManagerDialog::slotEdit);
    connect(m_deleteButton, &QAbstractButton::clicked, this, &StyleManagerDialog::slotRemove);
    connect(m_styleList, &QTreeWidget::itemDoubleClicked, this, &StyleManagerDialog::slotEdit);
    connect(m_styleList, &QTreeWidget::currentItemChanged, this, &StyleManagerDialog::selectionChanged);
}

StyleManagerDialog::~StyleManagerDialog() = default;

void StyleManagerDialog::fillComboBox()
{
    // This monstrosity fills in the tree widget with the style hierarchy. We cannot just do it linearly as the styles are in an arbitrary order
    QMap<QString, QTreeWidgetItem *> entries;
    // The default style goes first.
    QTreeWidgetItem *def = new QTreeWidgetItem(m_styleList, QStringList(i18n("Default")));
    entries[QString()] = def;

    QStringList names = m_styleManager->styleNames(false);
    bool processed = false;
    while (!processed) {
        processed = true;
        for (const QString &name : names) {
            // Check if this entry has been processed already.
            if (entries.contains(name))
                continue;
            CustomStyle *style = m_styleManager->style(name);
            // Check if we have the parent already
            QString parent = style->parentName();
            // No parent?
            if (parent.isEmpty()) {
                entries[name] = new QTreeWidgetItem(def, QStringList(name));
                continue;
            }
            // Existing parent?
            if (entries.contains(parent)) {
                entries[name] = new QTreeWidgetItem(entries[parent], QStringList(name));
                continue;
            }
            // Parent doesn't exist yet - we'll need to get back to it in the next iteration
            processed = false;
        }
    }
}

void StyleManagerDialog::slotDisplayMode(int mode)
{
    m_styleList->clear();

    if (mode == 2) {
        // "Hierarchical"
        m_styleList->setRootIsDecorated(true);
        fillComboBox();
        return;
    }

    m_styleList->setRootIsDecorated(false);

    if (mode != 1) // Include the default
        new QTreeWidgetItem(m_styleList, QStringList(i18n("Default")));

    QStringList names = m_styleManager->styleNames(false);
    for (const QString &name : names) {
        if (mode == 1) { // "Custom Styles"
            CustomStyle *style = m_styleManager->style(name);
            if (style->type() != Style::CUSTOM)
                continue;
        }
        new QTreeWidgetItem(m_styleList, QStringList(name));
    }
}

void StyleManagerDialog::onApply()
{
    QTreeWidgetItem *item = m_styleList->currentItem();
    if (!item)
        return;

    QString name(item->text(0));
    Q_EMIT setStyle(name);
}

void StyleManagerDialog::slotNew()
{
    CustomStyle *parentStyle = nullptr;
    QTreeWidgetItem *item = m_styleList->currentItem();
    if (item) {
        const QString name = item->text(0);
        if (name == i18n("Default"))
            parentStyle = m_styleManager->defaultStyle();
        else
            parentStyle = m_styleManager->style(name);
    } else
        parentStyle = m_styleManager->defaultStyle();

    int i = 1;
    QString newName(i18n("style%1", m_styleManager->count() + i));
    while (m_styleManager->style(newName) != nullptr) {
        ++i;
        newName = i18n("style%1", m_styleManager->count() + i);
    }

    CustomStyle style(newName, parentStyle);
    style.setType(Style::TENTATIVE);

    LayoutDialog *dialog = new LayoutDialog(this, m_selection->activeSheet(), m_styleManager, true);
    dialog->setCustomStyle(style);
    if (dialog->exec()) {
        CustomStyle *newStyle = new CustomStyle(dialog->customStyle());
        m_styleManager->insertStyle(newStyle);
        slotDisplayMode(m_displayBox->currentIndex());
    }

    delete dialog;
}

void StyleManagerDialog::slotEdit()
{
    QTreeWidgetItem *item = m_styleList->currentItem();

    if (!item)
        return;

    CustomStyle *style = nullptr;

    QString name(item->text(0));
    if (name == i18n("Default"))
        style = m_styleManager->defaultStyle();
    else
        style = m_styleManager->style(name);

    if (!style)
        return;

    LayoutDialog *dialog = new LayoutDialog(this, m_selection->activeSheet(), m_styleManager, true);
    dialog->setCustomStyle(*style);
    if (dialog->exec()) {
        m_styleManager->takeStyle(style);
        *style = dialog->customStyle();
        m_styleManager->insertStyle(style);
    }

    slotDisplayMode(m_displayBox->currentIndex());
    delete dialog;
}

void StyleManagerDialog::slotRemove()
{
    QTreeWidgetItem *item = m_styleList->currentItem();
    if (!item)
        return;

    const QString name = item->text(0);
    CustomStyle *style = nullptr;
    if (name == i18n("Default"))
        style = m_styleManager->defaultStyle();
    else
        style = m_styleManager->style(name);

    if (!style)
        return;

    if (style->type() != Style::CUSTOM)
        return;

    m_styleManager->takeStyle(style);
    slotDisplayMode(m_displayBox->currentIndex());
}

void StyleManagerDialog::selectionChanged(QTreeWidgetItem *item)
{
    if (!item)
        return;
    const QString name = item->text(0);
    CustomStyle *style = nullptr;
    if (name == i18n("Default"))
        style = m_styleManager->defaultStyle();
    else
        style = m_styleManager->style(name);
    if (!style) {
        m_deleteButton->setEnabled(false);
        return;
    }

    m_deleteButton->setEnabled(style->type() != Style::BUILTIN);
}
