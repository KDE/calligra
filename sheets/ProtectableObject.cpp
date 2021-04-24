/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ProtectableObject.h"

#include "part/Digest.h" // FIXME detach from part
#include "SheetsDebug.h"

#include <KoXmlNS.h>

#include <kcodecs.h>
#include <KLocalizedString>
#include <kmessagebox.h>
#include <knewpassworddialog.h>
#include <kpassworddialog.h>

#include <QPointer>

using namespace Calligra::Sheets;

void ProtectableObject::password(QByteArray & password) const
{
    password = m_password;
}

bool ProtectableObject::isProtected() const
{
    return !m_password.isNull();
}

void ProtectableObject::setProtected(QByteArray const & password)
{
    m_password = password;
}

bool ProtectableObject::checkPassword(QByteArray const & password) const
{
    return (password == m_password);
}

bool ProtectableObject::showPasswordDialog(QWidget* parent, Mode mode, const QString& title)
{
    if (mode == Lock) {
        QPointer<KNewPasswordDialog> dlg = new KNewPasswordDialog(parent);
        dlg->setPrompt(i18n("Enter a password."));
        dlg->setWindowTitle(title);
        if (dlg->exec() != KPasswordDialog::Accepted) {
            return false;
        }

        QByteArray hash;
        QString password = dlg->password();
        if (password.length() > 0) {
            SHA1::getHash(password, hash);
        }
        m_password = hash;
        delete dlg;
    } else { /* Unlock */
        QPointer<KPasswordDialog> dlg = new KPasswordDialog(parent);
        dlg->setPrompt(i18n("Enter the password."));
        dlg->setWindowTitle(title);
        if (dlg->exec() != KPasswordDialog::Accepted) {
            return false;
        }

        QByteArray hash("");
        QString password(dlg->password());
        if (password.length() > 0) {
            SHA1::getHash(password, hash);
        }
        if (!checkPassword(hash)) {
            KMessageBox::error(parent, i18n("Password is incorrect."));
            return false;
        }
        m_password = QByteArray();
        delete dlg;
    }
    return true;
}

void ProtectableObject::loadXmlProtection(const KoXmlElement& element)
{
    if (element.hasAttribute("protected")) {
        const QString passwd = element.attribute("protected");
        QByteArray str(passwd.toUtf8());
        m_password = KCodecs::base64Decode(str);
    }
}

