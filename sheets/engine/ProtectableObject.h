/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PROTECTABLE_OBJECT
#define CALLIGRA_SHEETS_PROTECTABLE_OBJECT

#include <QByteArray>
#include <QString>

#include "sheets_engine_export.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \ingroup Protection
 * Provides methods for setting a password protection.
 * The inheriting object decides which of its data should be protected.
 * It has to use isProtected() to check whether it's protected.
 */
class CALLIGRA_SHEETS_ENGINE_EXPORT ProtectableObject
{
public:
    enum Mode { Lock, Unlock };

    ProtectableObject();

    /**
     * Copy constructor. Needs to be defined because of the dummy operator=.
     */
    ProtectableObject(const ProtectableObject &other);

    /**
     * Retrieves the password hash.
     */
    QByteArray passwordHash() const;

    /**
     * \return \c true on enabled protection; \c false on disabled protection
     */
    bool isProtected() const;

    /**
     * Sets this object to be protected by \p password hash.
     */
    void setProtected(QByteArray const &password);

    /**
     * Sets this object to be protected by \p password.
     */
    void setProtected(const QString &password);

    /**
     * Checks if \p password matches the password of this object.
     */
    bool checkPassword(const QString &password) const;

private:
    // disable assignment operator
    void operator=(const ProtectableObject &) = delete;

    QByteArray passwordHash(const QString &password) const;
    QByteArray m_password;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PROTECTABLE_OBJECT
