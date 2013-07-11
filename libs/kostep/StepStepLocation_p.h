/*
    kostep -- handles changetracking using operational transformation for calligra
    Copyright (C) 2013  Luke Wolf <Lukewolf101010@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef STEPSTEPLOCATION_P_H
#define STEPSTEPLOCATION_P_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStack>
#include <QtGui/QTextCursor>

class StepStepLocationPrivate
{
public:
    explicit StepStepLocationPrivate();
    StepStepLocationPrivate* operator=(StepStepLocationPrivate location);
    void constructor(QTextCursor cursor);
    QTextCursor convertToQTextCursor(QTextDocument* ptr);
    QString ToString();
    QStack<int> location;
    int ParentFrame(QTextFrame* frame);

signals:

public slots:
private:


};

#endif // STEPSTEPLOCATION_P_H
