/* This file is part of the Calligra libraries
    SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "calligraversion.h"

unsigned int Calligra::version()
{
    return CALLIGRA_VERSION;
}

unsigned int Calligra::versionMajor()
{
    return CALLIGRA_VERSION_MAJOR;
}

unsigned int Calligra::versionMinor()
{
    return CALLIGRA_VERSION_MINOR;
}

unsigned int Calligra::versionRelease()
{
    return CALLIGRA_VERSION_RELEASE;
}

const char *Calligra::versionString()
{
    return CALLIGRA_VERSION_STRING;
}
