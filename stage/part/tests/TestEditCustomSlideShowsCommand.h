/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTEDITCUSTOMSLIDESHOWSCOMMAND_H
#define TESTEDITCUSTOMSLIDESHOWSCOMMAND_H

#include <QObject>

class TestEditCustomSlideShowsCommand : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // Just test one edit action because the command stores old and new list in the same
    // way for all actions.
    void moveSingleSlide();
};
#endif // TESTEDITCUSTOMSLIDESHOWSCOMMAND_H
