/*
 *  SPDX-FileCopyrightText: 2015 Boudewijn Rempt <boud@valdyas.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef CALLIGRAVERSIONWRAPPER_H
#define CALLIGRAVERSIONWRAPPER_H

#include "koversion_export.h"
#include <QString>

namespace CalligraVersionWrapper {

    KOVERSION_EXPORT QString versionYear();
    KOVERSION_EXPORT QString versionString(bool checkGit = false);
}

#endif // CALLIGRAVERSIONWRAPPER_H
