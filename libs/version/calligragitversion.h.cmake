/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Dmitry Kazakov <dimula73@gmail.com>
 * SPDX-FileCopyrightText: 2014 Jarosław Staniek <staniek@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef __CALLIGRA_GIT_VERSION_H
#define __CALLIGRA_GIT_VERSION_H

/**
 * @def CALLIGRA_GIT_SHA1_STRING
 * @ingroup CalligraMacros
 * @brief Indicates the git sha1 commit which was used for compilation of Calligra
 */
#cmakedefine CALLIGRA_GIT_SHA1_STRING "@CALLIGRA_GIT_SHA1_STRING@"

/**
 * @def CALLIGRA_GIT_BRANCH_STRING
 * @ingroup CalligraMacros
 * @brief Indicates the git branch name which was used for compilation of Calligra
 */
#cmakedefine CALLIGRA_GIT_BRANCH_STRING "@CALLIGRA_GIT_BRANCH_STRING@"

#endif /* __CALLIGRA_GIT_VERSION_H */
