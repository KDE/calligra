/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTRENAMECUSTOMSLIDESHOWCOMMAND_H
#define TESTRENAMECUSTOMSLIDESHOWCOMMAND_H

#include <QObject>

class TestRenameCustomSlideShowCommand : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void renameCustomShow();
};

#endif // TESTRENAMECUSTOMSLIDESHOWCOMMAND_H
