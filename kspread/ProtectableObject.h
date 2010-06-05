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

#ifndef KSPREAD_PROTECTABLE_OBJECT
#define KSPREAD_PROTECTABLE_OBJECT

#include <KoXmlReader.h>

#include <QByteArray>

#include "kspread_export.h"

namespace KSpread
{

/**
 * \ingroup Protection
 * Provides methods for setting a password protection.
 * The inheriting object decides which of its data should be protected.
 * It has to use isProtected() to check wether it's protected.
 */
class KSPREAD_EXPORT ProtectableObject
{
public:
    enum Mode
    {
        Lock,
        Unlock
    };

    /**
     * Retrieves the \p password.
     */
    void password(QByteArray &password) const;

    /**
     * \return \c true on enabled protection; \c false on disabled protection
     */
    bool isProtected() const;

    /**
     * Sets this object to be protected by \p password.
     */
    void setProtected(QByteArray const &password);

    /**
     * Checks if \p password matches the password of this object.
     */
    bool checkPassword(QByteArray const &password) const;

    /**
     * Shows a dialog for entering the password.
     * If the password is correct, the protection is enabled for
     * \p mode being \c Lock, or it is disabled for \p mode being \c Unlock.
     * \param title the window title
     * \return \c true on success; \c false on failure
     */
    bool showPasswordDialog(QWidget* parent, Mode mode, const QString& title);

    /**
     * \ingroup NativeFormat
     */
    void loadXmlProtection(const KoXmlElement& element);

    /**
     * \ingroup OpenDocument
     */
    void loadOdfProtection(const KoXmlElement& element);

private:
    // disable assignment operator
    void operator=(const ProtectableObject&);

    QByteArray m_password;
};

} // namespace KSpread

#endif // KSPREAD_PROTECTABLE_OBJECT
