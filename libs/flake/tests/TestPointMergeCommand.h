/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTPOINTMERGECOMMAND_H
#define TESTPOINTMERGECOMMAND_H

#include <QObject>

class TestPointMergeCommand : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void closeSingleLinePath();
    void closeSingleCurvePath();
    void connectLineSubpaths();
    void connectCurveSubpaths();
};

#endif // TESTPOINTMERGECOMMAND_H
