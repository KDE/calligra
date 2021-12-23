/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PROTECTABLE_OBJECT
#define CALLIGRA_SHEETS_PROTECTABLE_OBJECT

#include <KoXmlReader.h>

#include <QByteArray>

#include "sheets_odf_export.h"

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
class CALLIGRA_SHEETS_ODF_EXPORT ProtectableObject
{
public:
    enum Mode {
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
     * \param parent the parent Qwidget
     * \param mode the mode
     * \param title the window title
     * \return \c true on success; \c false on failure
     */
    bool showPasswordDialog(QWidget* parent, Mode mode, const QString& title);

private:
    // disable assignment operator
    void operator=(const ProtectableObject&);

    QByteArray m_password;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_PROTECTABLE_OBJECT
