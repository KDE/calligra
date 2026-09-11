/*
 * SPDX-FileCopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#pragma once

#include <QMap>
#include <QWidget>

class QComboBox;
class QLineEdit;
class QListWidget;
class QPushButton;

class KoFormEventsWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit KoFormEventsWidget(QWidget *parent = nullptr);

    QMap<QString, QString> events() const;
    void setEvents(const QMap<QString, QString> &events);

Q_SIGNALS:
    void eventsChanged();

private Q_SLOTS:
    void updateEditor();
    void commitEditor();
    void addEvent();
    void removeEvent();
    void pickScript();

private:
    void rebuildList();
    QListWidget *m_eventList = nullptr;
    QComboBox *m_eventSelector = nullptr;
    QLineEdit *m_handler = nullptr;
    QPushButton *m_pick = nullptr;
    QPushButton *m_remove = nullptr;
    QPushButton *m_add = nullptr;
    QMap<QString, QString> m_events;
};
