// This file is part of the KDE project
// SPDX-FileCopyrightText: 2003, 2004 Ariya Hidayat <ariya@kde.org>
// SPDX-FileCopyrightText: 2005 Tomas Mecir <mecirt@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

// Local
#include "FunctionRepository.h"

#include "Function.h"
#include "FunctionDescription.h"
#include "FunctionModuleRegistry.h"
#include "SheetsDebug.h"

#include <QDomElement>
#include <QFile>

#include <KLocalizedString>

Q_GLOBAL_STATIC(Calligra::Sheets::FunctionRepository, s_instance)

using namespace Calligra::Sheets;

class Q_DECL_HIDDEN FunctionRepository::Private
{
public:
    QHash<QString, QSharedPointer<Function>> functions;
    QHash<QString, QSharedPointer<Function>> alternates;
    QHash<QString, FunctionDescription *> descriptions;
    QStringList groups;
    bool initialized;
};

FunctionRepository *FunctionRepository::self()
{
    if (!s_instance.exists()) {
        *s_instance; // creates the global instance

        // register all existing functions
        FunctionModuleRegistry::instance()->registerFunctions();

#ifndef NDEBUG
        debugSheetsUI << "functions registered:" << s_instance->d->functions.count() << "descriptions loaded:" << s_instance->d->descriptions.count();

        // Verify, that every function has a description.
        QStringList missingDescriptions;
        typedef QHash<QString, QSharedPointer<Function>> Functions;
        Functions::ConstIterator end = s_instance->d->functions.constEnd();
        for (Functions::ConstIterator it = s_instance->d->functions.constBegin(); it != end; ++it) {
            if (!s_instance->d->descriptions.contains(it.key()))
                missingDescriptions << it.key();
        }
        if (missingDescriptions.count() > 0) {
            debugSheetsUI << "No function descriptions found for:";
            for (const QString &missingDescription : missingDescriptions) {
                debugSheetsUI << "\t" << missingDescription;
            }
        }
#endif
    }
    return s_instance;
}

FunctionRepository::FunctionRepository()
    : d(new Private)
{
    d->initialized = false;
}

FunctionRepository::~FunctionRepository()
{
    qDeleteAll(d->descriptions);
    delete d;
}

void FunctionRepository::add(const QSharedPointer<Function> &function)
{
    if (!function)
        return;
    d->functions.insert(function->name().toUpper(), function);

    if (!function->alternateName().isNull()) {
        d->alternates.insert(function->alternateName().toUpper(), function);
    }
}

void FunctionRepository::add(FunctionDescription *desc)
{
    if (!desc)
        return;
    if (!d->functions.contains(desc->name()))
        return;
    d->descriptions.insert(desc->name(), desc);
}

void FunctionRepository::remove(const QSharedPointer<Function> &function)
{
    const QString functionName = function->name().toUpper();
    delete d->descriptions.take(functionName);
    if (d->functions.contains(functionName)) {
        QSharedPointer<Function> function = d->functions.take(functionName);
        d->alternates.remove(function->alternateName().toUpper());
    }
}

QSharedPointer<Function> FunctionRepository::function(const QString &name)
{
    const QString n = name.toUpper();
    QSharedPointer<Function> f = d->functions.value(n);
    return !f.isNull() ? f : d->alternates.value(n);
}

FunctionDescription *FunctionRepository::functionInfo(const QString &name)
{
    return d->descriptions.value(name.toUpper());
}

// returns names of function in certain group
QStringList FunctionRepository::functionNames(const QString &group)
{
    QStringList lst;

    for (FunctionDescription *description : d->descriptions) {
        if (group.isNull() || (description->group() == group))
            lst.append(description->name());
    }

    lst.sort();
    return lst;
}

const QStringList &FunctionRepository::groups() const
{
    return d->groups;
}

void FunctionRepository::addGroup(const QString &groupname)
{
    d->groups.append(groupname);
    d->groups.sort();
}

void FunctionRepository::loadFunctionDescriptions(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QDomDocument doc;
    doc.setContent(&file);
    file.close();

    QString group;

    QDomNode n = doc.documentElement().firstChild();
    for (; !n.isNull(); n = n.nextSibling()) {
        if (!n.isElement())
            continue;
        QDomElement e = n.toElement();
        if (e.tagName() == "Group") {
            group = i18n(e.namedItem("GroupName").toElement().text().toUtf8());
            addGroup(group);

            QDomNode n2 = e.firstChild();
            for (; !n2.isNull(); n2 = n2.nextSibling()) {
                if (!n2.isElement())
                    continue;
                QDomElement e2 = n2.toElement();
                if (e2.tagName() == "Function") {
                    FunctionDescription *desc = new FunctionDescription(e2);
                    desc->setGroup(group);
                    if (d->functions.contains(desc->name()))
                        d->descriptions.insert(desc->name(), desc);
                    else {
                        debugSheetsUI << "Description for unknown function" << desc->name() << "found.";
                        delete desc;
                    }
                }
            }
            group.clear();
        }
    }
}
