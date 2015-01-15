/* This file is part of the KDE project
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiDBPasswordDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <KLocale>

#include <KoIcon.h>

#include <db/connectiondata.h>

#include <kexiutils/utils.h>

class KexiDBPasswordDialog::Private
{
 public:
    explicit Private(KexiDB::ConnectionData* data);
    ~Private();

    KexiDB::ConnectionData *cdata;
    bool showConnectionDetailsRequested;
};

KexiDBPasswordDialog::Private::Private(KexiDB::ConnectionData* data)
    : cdata(data)
    , showConnectionDetailsRequested(false)
{
}

KexiDBPasswordDialog::Private::~Private()
{
}

KexiDBPasswordDialog::KexiDBPasswordDialog(QWidget *parent, KexiDB::ConnectionData& cdata,
                                           Flags flags)
        : KPasswordDialog(parent,
            ShowUsernameLine | ShowDomainLine | ((flags & ServerReadOnly) ? DomainReadOnly : KPasswordDialog::NoFlags),
            (flags & ShowDetailsButton) ? KDialog::User1 : KDialog::None)
        , d(new Private(&cdata))
{
    setCaption(i18nc("@title:window", "Opening Database"));
    setPrompt(i18nc("@info", "Supply a password below."));
    /*  msg += cdata.userName.isEmpty() ?
          "<p>"+i18n("Please enter the password.")
          : "<p>"+i18n("Please enter the password for user.").arg("<b>"+cdata.userName+"</b>");*/

    QString srv = cdata.serverInfoString(false);
//    if (srv.isEmpty() || srv.toLower() == "localhost")
//        srv = i18n("local database server");

    QLabel *domainLabel = KexiUtils::findFirstChild<QLabel*>(this, "QLabel", "domainLabel");
    if (domainLabel) {
        domainLabel->setText(i18n("Database server:"));
    }
    setDomain(srv);

    QString usr;
    if (cdata.userName.isEmpty())
        usr = i18nc("unspecified user", "(unspecified)");
    else
        usr = cdata.userName;
    setUsernameReadOnly(true);
    setUsername(usr);

    if ((flags & ShowDetailsButton)) {
        connect(this, SIGNAL(user1Clicked()),
                this, SLOT(slotShowConnectionDetails()));
        setButtonText(KDialog::User1, i18n("&Details") + " >>");
    }
    setButtonText(KDialog::Ok, i18n("&Open"));
    setButtonIcon(KDialog::Ok, koIcon("document-open"));
}

KexiDBPasswordDialog::~KexiDBPasswordDialog()
{
    delete d;
}

bool KexiDBPasswordDialog::showConnectionDetailsRequested() const
{
    return d->showConnectionDetailsRequested;
}

void KexiDBPasswordDialog::slotButtonClicked(int button)
{
    if (button == KDialog::Ok || button == KDialog::User1) {
        d->cdata->password = password();
        QLineEdit *userEdit = KexiUtils::findFirstChild<QLineEdit*>(this, "QLineEdit", "userEdit");
        if (!userEdit->isReadOnly()) {
            d->cdata->userName = userEdit->text();
        }
    }
    else {
        //d->cdata->password.clear();
    }
    KPasswordDialog::slotButtonClicked(button);
}

void KexiDBPasswordDialog::slotShowConnectionDetails()
{
    d->showConnectionDetailsRequested = true;
    close();
}

//static
tristate KexiDBPasswordDialog::getPasswordIfNeeded(KexiDB::ConnectionData *data, QWidget *parent)
{
    if (data->passwordNeeded() && data->password.isNull() /* null means missing password */) {
        //ask for password
        KexiDBPasswordDialog pwdDlg(parent, *data, KexiDBPasswordDialog::ServerReadOnly);
        return QDialog::Accepted == pwdDlg.exec() ? tristate(true): cancelled;
    }
    return false;
}

#include "KexiDBPasswordDialog.moc"
