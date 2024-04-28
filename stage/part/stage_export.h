/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2006 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef STAGE_EXPORT_H_WRAPPER
#define STAGE_EXPORT_H_WRAPPER

/* This is the one which does the actual work, generated by cmake */
#include "stage_generated_export.h"

/* Now the same for STAGE_TEST_EXPORT, if compiling with unit tests enabled */
#ifdef COMPILING_TESTS
#define STAGE_TEST_EXPORT STAGE_EXPORT
#else /* not compiling tests */
#define STAGE_TEST_EXPORT
#endif

#endif
