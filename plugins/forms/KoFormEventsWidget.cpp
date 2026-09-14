/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KoFormEventsWidget.h"

#include <KLocalizedString>

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSet>
#include <QVBoxLayout>
#include <array>
#include <utility>

using namespace Qt::StringLiterals;

namespace
{
const QList<QPair<KLocalizedString, QLatin1StringView>> eventTypes = {
    {ki18nc("@item:form event", "Before action"), "form:approveaction"_L1},
    {ki18nc("@item:form event", "When action performed"), "form:performaction"_L1},
    {ki18nc("@item:form event", "When changed"), "dom:change"_L1},
    {ki18nc("@item:form event", "When text changed"), "form:textchange"_L1},
    {ki18nc("@item:form event", "When item state changed"), "form:itemstatechange"_L1},
    {ki18nc("@item:form event", "When focused"), "dom:DOMFocusIn"_L1},
    {ki18nc("@item:form event", "When focus lost"), "dom:DOMFocusOut"_L1},
    {ki18nc("@item:form event", "When key pressed"), "dom:keydown"_L1},
    {ki18nc("@item:form event", "When key released"), "dom:keyup"_L1},
    {ki18nc("@item:form event", "When mouse entered"), "dom:mouseover"_L1},
    {ki18nc("@item:form event", "When mouse dragged"), "form:mousedrag"_L1},
    {ki18nc("@item:form event", "When mouse moved"), "dom:mousemove"_L1},
    {ki18nc("@item:form event", "When mouse pressed"), "dom:mousedown"_L1},
    {ki18nc("@item:form event", "When mouse released"), "dom:mouseup"_L1},
    {ki18nc("@item:form event", "When mouse exited"), "dom:mouseout"_L1},
    {ki18nc("@item:form event", "Before reset"), "form:approvereset"_L1},
    {ki18nc("@item:form event", "When reset"), "dom:reset"_L1},
    {ki18nc("@item:form event", "When submitted"), "dom:submit"_L1},
    {ki18nc("@item:form event", "Before update"), "form:approveupdate"_L1},
    {ki18nc("@item:form event", "When updated"), "form:update"_L1},
    {ki18nc("@item:form event", "When loaded"), "dom:load"_L1},
    {ki18nc("@item:form event", "Before reload"), "form:startreload"_L1},
    {ki18nc("@item:form event", "When reloaded"), "form:reload"_L1},
    {ki18nc("@item:form event", "Before unload"), "form:startunload"_L1},
    {ki18nc("@item:form event", "When unloaded"), "dom:unload"_L1},
    {ki18nc("@item:form event", "Before deleting"), "form:confirmdelete"_L1},
    {ki18nc("@item:form event", "Before row change"), "form:approverowchange"_L1},
    {ki18nc("@item:form event", "When row changed"), "form:rowchange"_L1},
    {ki18nc("@item:form event", "Before cursor move"), "form:approvecursormove"_L1},
    {ki18nc("@item:form event", "When cursor moved"), "form:cursormove"_L1},
    {ki18nc("@item:form event", "Before supplying parameters"), "form:supplyparameter"_L1},
    {ki18nc("@item:form event", "When an error occurs"), "dom:error"_L1},
    {ki18nc("@item:form event", "When adjustment changed"), "form:adjust"_L1},
    {ki18nc("@item:form event", "When activated"), "dom:DOMActivate"_L1},
};
}

KoFormEventsWidget::KoFormEventsWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins({});
    m_eventList = new QListWidget(this);
    m_eventList->setMaximumHeight(120);
    layout->addWidget(m_eventList);

    auto *form = new QFormLayout;
    m_eventSelector = new QComboBox(this);
    m_handler = new QLineEdit(this);
    m_handler->setReadOnly(false);
    m_handler->setEnabled(true);
    m_handler->setPlaceholderText(i18nc("@info:placeholder", "Script URI or macro name"));
    form->addRow(i18nc("@label:form event", "Event:"), m_eventSelector);
    auto *handlerLayout = new QHBoxLayout;
    handlerLayout->setContentsMargins({});
    handlerLayout->addWidget(m_handler);
    m_pick = new QPushButton(i18nc("@button", "Select…"), this);
    m_pick->setIcon(QIcon::fromTheme(u"document-open-symbolic"_s));
    handlerLayout->addWidget(m_pick);
    form->addRow(i18nc("@label:form event", "Script or macro:"), handlerLayout);
    layout->addLayout(form);

    auto *buttons = new QHBoxLayout;
    m_add = new QPushButton(i18nc("@button", "Add"), this);
    m_remove = new QPushButton(i18nc("@button", "Remove"), this);
    m_add->setIcon(QIcon::fromTheme(u"list-add-symbolic"_s));
    m_remove->setIcon(QIcon::fromTheme(u"list-remove-symbolic"_s));
    buttons->addWidget(m_add);
    buttons->addWidget(m_remove);
    layout->addLayout(buttons);

    connect(m_eventList, &QListWidget::currentRowChanged, this, &KoFormEventsWidget::updateEditor);
    connect(m_eventSelector, qOverload<int>(&QComboBox::currentIndexChanged), this, &KoFormEventsWidget::updateEditor);
    connect(m_handler, &QLineEdit::editingFinished, this, &KoFormEventsWidget::commitEditor);
    connect(m_add, &QPushButton::clicked, this, &KoFormEventsWidget::addEvent);
    connect(m_remove, &QPushButton::clicked, this, &KoFormEventsWidget::removeEvent);
    connect(m_pick, &QPushButton::clicked, this, &KoFormEventsWidget::pickScript);
}

void KoFormEventsWidget::pickScript()
{
    QDialog dialog(this);
    dialog.setWindowTitle(i18nc("@title:form", "Select script or macro"));
    auto *layout = new QVBoxLayout(&dialog);
    auto *scripts = new QListWidget(&dialog);
    const QString current = m_handler->text();
    QSet<QString> knownScripts;
    for (const auto &script : std::as_const(m_scripts)) {
        if (!script.name.isEmpty()) {
            knownScripts.insert(script.name);
        }
    }
    for (const auto &handler : std::as_const(m_events)) {
        if (!handler.isEmpty()) {
            knownScripts.insert(handler);
        }
    }
    for (const auto &script : std::as_const(knownScripts)) {
        scripts->addItem(script);
    }
    auto *entry = new QLineEdit(&dialog);
    entry->setPlaceholderText(i18nc("@info:placeholder", "Script URI or macro name"));
    entry->setText(current);
    layout->addWidget(scripts);
    layout->addWidget(entry);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    auto *newScript = buttons->addButton(i18nc("@button", "New script…"), QDialogButtonBox::ActionRole);
    newScript->setIcon(QIcon::fromTheme(u"document-new-symbolic"_s));
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(newScript, &QPushButton::clicked, &dialog, [this, scripts, entry] {
        QDialog scriptDialog(this);
        scriptDialog.setWindowTitle(i18nc("@title:form", "Create script"));
        auto *scriptForm = new QFormLayout(&scriptDialog);
        auto *name = new QLineEdit(&scriptDialog);
        auto *language = new QLineEdit(u"ooo:Basic"_s, &scriptDialog);
        auto *content = new QPlainTextEdit(&scriptDialog);
        scriptForm->addRow(i18nc("@label:form script", "Name:"), name);
        scriptForm->addRow(i18nc("@label:form script", "Language:"), language);
        scriptForm->addRow(i18nc("@label:form script", "Content:"), content);
        auto *scriptButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &scriptDialog);
        scriptForm->addRow(scriptButtons);
        connect(scriptButtons, &QDialogButtonBox::accepted, &scriptDialog, &QDialog::accept);
        connect(scriptButtons, &QDialogButtonBox::rejected, &scriptDialog, &QDialog::reject);
        if (scriptDialog.exec() != QDialog::Accepted || name->text().trimmed().isEmpty()) {
            return;
        }
        KoOdfScript::Script script;
        script.name = name->text().trimmed();
        script.language = language->text().trimmed();
        script.content = content->toPlainText();
        m_scripts.append(script);
        if (scripts->findItems(script.name, Qt::MatchExactly).isEmpty()) {
            scripts->addItem(script.name);
        }
        scripts->setCurrentRow(scripts->count() - 1);
        entry->setText(script.name);
    });
    connect(scripts, &QListWidget::itemDoubleClicked, &dialog, &QDialog::accept);
    connect(scripts, &QListWidget::currentTextChanged, entry, &QLineEdit::setText);
    if (dialog.exec() == QDialog::Accepted && !entry->text().trimmed().isEmpty()) {
        m_handler->setText(entry->text().trimmed());
        commitEditor();
    }
}

QMap<QString, QString> KoFormEventsWidget::events() const
{
    return m_events;
}

KoOdfScript::Scripts KoFormEventsWidget::scripts() const
{
    return m_scripts;
}

void KoFormEventsWidget::setScripts(const KoOdfScript::Scripts &scripts)
{
    m_scripts = scripts;
}

void KoFormEventsWidget::setEvents(const QMap<QString, QString> &events)
{
    m_events = events;
    rebuildList();
}

void KoFormEventsWidget::rebuildList()
{
    const QString currentEvent = m_eventList->currentItem() ? m_eventList->currentItem()->data(Qt::UserRole).toString() : QString();
    m_eventSelector->clear();
    for (const auto &event : eventTypes) {
        if (!m_events.contains(event.second) || event.second == currentEvent) {
            m_eventSelector->addItem(event.first.toString(), event.second.toString());
        }
    }
    m_eventList->clear();
    for (auto it = m_events.cbegin(); it != m_events.cend(); ++it) {
        auto *item = new QListWidgetItem(it.key() + u": "_s + it.value(), m_eventList);
        item->setData(Qt::UserRole, it.key());
    }
    if (m_eventList->count() > 0) {
        m_eventList->setCurrentRow(0);
    }
    m_remove->setEnabled(m_eventList->currentRow() >= 0);
    m_add->setEnabled(m_eventSelector->count() > 0);
    updateEditor();
}

void KoFormEventsWidget::updateEditor()
{
    const int row = m_eventList->currentRow();
    if (row < 0 || row >= m_events.size()) {
        m_handler->clear();
        m_remove->setEnabled(false);
        return;
    }
    m_remove->setEnabled(true);
    const QString event = m_events.keys().at(row);
    m_eventSelector->setCurrentIndex(m_eventSelector->findData(event));
    m_handler->setText(m_events.value(event));
}

void KoFormEventsWidget::commitEditor()
{
    const int row = m_eventList->currentRow();
    if (row < 0 || row >= m_events.size()) {
        return;
    }
    const QString event = m_events.keys().at(row);
    m_events[event] = m_handler->text();
    rebuildList();
    m_eventList->setCurrentRow(row);
    Q_EMIT eventsChanged();
}

void KoFormEventsWidget::addEvent()
{
    QString event = m_eventSelector->currentData().toString();
    for (int index = 0; index < m_eventSelector->count(); ++index) {
        const QString candidate = m_eventSelector->itemData(index).toString();
        if (!m_events.contains(candidate)) {
            event = candidate;
            break;
        }
    }
    if (event.isEmpty() || m_events.contains(event)) {
        return;
    }
    m_events[event] = {};
    rebuildList();
    m_eventList->setCurrentRow(m_events.keys().indexOf(event));
    m_handler->setFocus();
    Q_EMIT eventsChanged();
}

void KoFormEventsWidget::removeEvent()
{
    const int row = m_eventList->currentRow();
    if (row < 0 || row >= m_events.size()) {
        return;
    }
    m_events.remove(m_events.keys().at(row));
    rebuildList();
    Q_EMIT eventsChanged();
}
