/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoEncryptionChecker.h"

#ifdef QCA2

// QCA headers have "slots" and "signals", which QT_NO_SIGNALS_SLOTS_KEYWORDS does not like
#define slots Q_SLOTS
#define signals Q_SIGNALS
#include <QtCrypto>
#undef slots
#undef signals
#include <StoreDebug.h>

bool KoEncryptionChecker::isEncryptionSupported()
{
    QCA::Initializer* initializer = new QCA::Initializer();
    bool supported = QCA::isSupported("sha1") && QCA::isSupported("pbkdf2(sha1)") && QCA::isSupported("blowfish-cfb");
    if (!supported) {
        warnStore << "QCA is enabled but sha1, pbkdf2(sha1) or blowfish-cfb are not supported. Encryption is disabled.";
    }
    delete initializer;
    return supported;
}
#else
bool KoEncryptionChecker::isEncryptionSupported()
{

    return false;
}
#endif
