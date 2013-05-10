/* This file is part of the KDE project
   Copyright (C) 2013 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiPasswordPage.h"
#include <widget/KexiPasswordWidget.h>
#include <db/connectiondata.h>

#include <KLocale>
#include <KLineEdit>

#include <QVBoxLayout>
#include <QFontMetrics>

class KexiPasswordPage::Private
{
public:
    Private() {}
    KexiPasswordWidget *widget;
};

KexiPasswordPage::KexiPasswordPage(const QString &title, QWidget* parent)
    : KexiAssistantPage(title, QString(), parent), d(new Private)
{
    init();
}

KexiPasswordPage::KexiPasswordPage(QWidget* parent)
    : KexiAssistantPage(i18n("Database Password"), QString(), parent), d(new Private)
{
    init();
}

void KexiPasswordPage::init()
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    QVBoxLayout *lyr = new QVBoxLayout;
    d->widget = new KexiPasswordWidget(0,
        KexiPasswordWidget::ShowUsernameLine | KexiPasswordWidget::ShowDomainLine
        | KexiPasswordWidget::UsernameReadOnly | KexiPasswordWidget::DomainReadOnly);
    KLineEdit *passEdit = d->widget->findChild<KLineEdit*>("passEdit");
    Q_ASSERT(passEdit);
    passEdit->setMaximumWidth(passEdit->fontMetrics().width("W")*24);
    lyr->addWidget(d->widget);
    lyr->addStretch();
    setFocusWidget(passEdit);
    setDescription(d->widget->prompt());
    d->widget->setPrompt(QString());
    setContents(lyr);

    connect(d->widget, SIGNAL(returnPressed()), this, SLOT(next()));
}

KexiPasswordPage::~KexiPasswordPage()
{
    delete d;
}

void KexiPasswordPage::setPassword(const QString& password)
{
    d->widget->setPassword(password);
}

QString KexiPasswordPage::password() const
{
    return d->widget->password();
}

void KexiPasswordPage::setUserName(const QString& username)
{
    d->widget->setUsername(username);
}

QString KexiPasswordPage::userName() const
{
    return d->widget->username();
}

void KexiPasswordPage::setServer(const QString& server)
{
    d->widget->setDomain(server);
}

QString KexiPasswordPage::server() const
{
    return d->widget->domain();
}

void KexiPasswordPage::showDatabaseName(bool show)
{
    d->widget->showDatabaseName(show);
}

void KexiPasswordPage::setDatabaseNameReadOnly(bool readOnly)
{
    d->widget->setDatabaseNameReadOnly(readOnly);
}

void KexiPasswordPage::setDatabaseName(const QString& databaseName)
{
    d->widget->setDatabaseName(databaseName);
}

void KexiPasswordPage::setConnectionData(const KexiDB::ConnectionData &data)
{
    setPassword(data.savePassword ? data.password : QString());
    setUserName(data.userName);
    setServer(data.serverInfoString(false));
}

void KexiPasswordPage::updateConnectionData(KexiDB::ConnectionData *data)
{
    if (data) {
        data->password = password();
    }
}
