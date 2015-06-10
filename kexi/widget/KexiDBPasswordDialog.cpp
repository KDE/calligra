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
#include <KexiIcon.h>
#include <kexiutils/utils.h>

#include <KDbConnectionData>

#include <KLocalizedString>

#include <QLabel>
#include <QLineEdit>

class KexiDBPasswordDialog::Private
{
 public:
    explicit Private(KDbConnectionData* data);
    ~Private();

    KDbConnectionData *cdata;
    bool showConnectionDetailsRequested;
};

KexiDBPasswordDialog::Private::Private(KDbConnectionData* data)
    : cdata(data)
    , showConnectionDetailsRequested(false)
{
}

KexiDBPasswordDialog::Private::~Private()
{
}

KexiDBPasswordDialog::KexiDBPasswordDialog(QWidget *parent, KDbConnectionData& cdata,
                                           Flags flags)
        : KPasswordDialog(parent,
            ShowUsernameLine | ShowDomainLine | ((flags & ServerReadOnly) ? DomainReadOnly : KPasswordDialog::NoFlags))
        , d(new Private(&cdata))
{
    setWindowTitle(xi18nc("@title:window", "Opening Database"));
    setPrompt(xi18nc("@info", "Supply a password below."));

    if ((flags & ShowDetailsButton)) {
        //! @todo KEXI3 OK?
        buttonBox()->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel
                                        | QDialogButtonBox::Help);
        QPushButton *detailsButton = buttonBox()->button(QDialogButtonBox::Help);
        connect(detailsButton, SIGNAL(clicked()),
                this, SLOT(slotShowConnectionDetails()));
        detailsButton->setText(xi18n("&Details" + " >>"));
        connect(detailsButton, SIGNAL(clicked()),
                this, SLOT(slotOkOrDetailsButtonClicked()))
    }
    /*  msg += cdata.userName.isEmpty() ?
          "<p>"+xi18n("Please enter the password.")
          : "<p>"+xi18n("Please enter the password for user.").arg("<b>"+cdata.userName+"</b>");*/

    QString srv = cdata.toUserVisibleString(KDbConnectionData::NoUserVisibleStringOption);
//    if (srv.isEmpty() || srv.toLower() == "localhost")
//        srv = xi18n("local database server");

    QLabel *domainLabel = KexiUtils::findFirstChild<QLabel*>(this, "QLabel", "domainLabel");
    if (domainLabel) {
        domainLabel->setText(xi18n("Database server:"));
    }
    setDomain(srv);

    QString usr;
    if (cdata.userName.isEmpty())
        usr = xi18nc("unspecified user", "(unspecified)");
    else
        usr = cdata.userName;
    setUsernameReadOnly(true);
    setUsername(usr);

    buttonBox()->button(QDialogButtonBox::Ok)->setText(xi18n("&Open"));
    connect(buttonBox()->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
            this, SLOT(slotOkOrDetailsButtonClicked()))
    //! @todo KEXI3 buttonBox()->button(QDialogButtonBox::Ok)->setIcon(koIcon("document-open");
}

KexiDBPasswordDialog::~KexiDBPasswordDialog()
{
    delete d;
}

bool KexiDBPasswordDialog::showConnectionDetailsRequested() const
{
    return d->showConnectionDetailsRequested;
}

void KexiDBPasswordDialog::slotOkOrDetailsButtonClicked()
{
    d->cdata->password = password();
    QLineEdit *userEdit = KexiUtils::findFirstChild<QLineEdit*>(this, "QLineEdit", "userEdit");
    if (!userEdit->isReadOnly()) {
        d->cdata->userName = userEdit->text();
    }
}

void KexiDBPasswordDialog::slotShowConnectionDetails()
{
    d->showConnectionDetailsRequested = true;
    close();
}

//static
tristate KexiDBPasswordDialog::getPasswordIfNeeded(KDbConnectionData *data, QWidget *parent)
{
    if (data->passwordNeeded() && data->password.isNull() /* null means missing password */) {
        //ask for password
        KexiDBPasswordDialog pwdDlg(parent, *data, KexiDBPasswordDialog::ServerReadOnly);
        return QDialog::Accepted == pwdDlg.exec() ? tristate(true): cancelled;
    }
    return false;
}

