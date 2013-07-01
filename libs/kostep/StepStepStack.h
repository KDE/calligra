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

#ifndef STEPSTEPSTACK_H
#define STEPSTEPSTACK_H

#include <QtCore/QObject>
#include <QtCore/QStack>
#include <QtCore/QAbstractListModel>
#include <QtCore/QVariant>

class StepStepBase;
class StepStepStack_p;
class StepStepStack :  public QAbstractListModel
{

  Q_OBJECT
public:
    StepStepStack(QObject* parent);
    StepStepStack(const StepStepStack& other);
    ~StepStepStack();
    StepStepStack& operator=(const StepStepStack& other);
    bool operator==(const StepStepStack& other);

    StepStepBase at(int i);
    StepStepBase pop();
    StepStepBase top();

    void serialize(QString Filename);
    void deserialize(QString Filename);

    void insertAt(int i, StepStepBase step);
    void removeAt(int i);

    void push(StepStepBase step);

    //QAbstractListModel Required Member Functions
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    StepStepStack_p* d;
};

#endif // STEPSTEPSTACK_H
