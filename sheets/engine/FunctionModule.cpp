// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "FunctionModule.h"

#include "Function.h"

#include <QList>

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN FunctionModule::Private
{
public:
    QList<QSharedPointer<Function>> functions;
};

FunctionModule::FunctionModule(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
}

FunctionModule::~FunctionModule()
{
    delete d;
}

QList<QSharedPointer<Function>> FunctionModule::functions() const
{
    return d->functions;
}

bool FunctionModule::isRemovable()
{
    QList<Function *> checkedFunctions;
    QWeakPointer<Function> weakPointer;
    while (d->functions.count() != 0) {
        weakPointer = d->functions.last().toWeakRef();
        checkedFunctions.append(d->functions.takeLast().data());
        if (!weakPointer.isNull()) {
            // Put it and the other checked ones back in.
            d->functions.append(weakPointer.toStrongRef());
            // The failing on was used, so we do not put it in twice.
            checkedFunctions.removeLast();
            for (Function *function : checkedFunctions) {
                // It is okay to recreate the shared pointers, as they were not used.
                d->functions.append(QSharedPointer<Function>(function));
            }
            return false;
        }
    }
    return true;
}

QString FunctionModule::id() const
{
    return descriptionFileName();
}

void FunctionModule::add(Function *function)
{
    if (!function) {
        return;
    }
    d->functions.append(QSharedPointer<Function>(function));
}
