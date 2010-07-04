/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#include "ProtectableObject.h"

#include "part/Digest.h" // FIXME detach from part

#include <KoXmlNS.h>

#include <KCodecs>
#include <KDebug>
#include <KLocale>
#include <KMessageBox>
#include <knewpassworddialog.h>
#include <KPasswordDialog>

using namespace KSpread;

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
        KNewPasswordDialog dlg(parent);
        dlg.setPrompt(i18n("Enter a password."));
        dlg.setWindowTitle(title);
        if (dlg.exec() != KPasswordDialog::Accepted) {
            return false;
        }

        QByteArray hash;
        QString password = dlg.password();
        if (password.length() > 0) {
            SHA1::getHash(password, hash);
        }
        m_password = hash;
    } else { /* Unlock */
        KPasswordDialog dlg(parent);
        dlg.setPrompt(i18n("Enter the password."));
        dlg.setWindowTitle(title);
        if (dlg.exec() != KPasswordDialog::Accepted) {
            return false;
        }

        QByteArray hash("");
        QString password(dlg.password());
        if (password.length() > 0) {
            SHA1::getHash(password, hash);
        }
        if (!checkPassword(hash)) {
            KMessageBox::error(parent, i18n("Password is incorrect."));
            return false;
        }
        m_password = QByteArray();
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

void ProtectableObject::loadOdfProtection(const KoXmlElement& element)
{
    if (element.hasAttributeNS(KoXmlNS::table, "protection-key")) {
        QString p = element.attributeNS(KoXmlNS::table, "protection-key", QString());
        if (!p.isNull()) {
            QByteArray str(p.toUtf8());
            kDebug(30518) <<"Decoding password:" << str;
            m_password = KCodecs::base64Decode(str);
        }
    }
}
