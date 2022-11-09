/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2017-2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "autocorrection_export.h"

/* Classes which are exported only for unit tests */
#ifdef BUILD_TESTING
#ifndef AUTOCORRECTION_TESTS_EXPORT
#define AUTOCORRECTION_TESTS_EXPORT AUTOCORRECTION_EXPORT
#endif
#else /* not compiling tests */
#define AUTOCORRECTION_TESTS_EXPORT
#endif
