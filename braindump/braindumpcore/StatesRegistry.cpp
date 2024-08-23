// SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "StatesRegistry.h"

#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>

#include <KLocalizedString>

#include "State.h"
#include "StateCategory.h"
#include "StateCategory_p.h"

struct Q_DECL_HIDDEN StatesRegistry::Private {
    static StatesRegistry *s_instance;
    QMap<QString, StateCategory *> categories;
    void parseStatesRC(const QString &_filename);
};

StatesRegistry *StatesRegistry::Private::s_instance = nullptr;

void StatesRegistry::Private::parseStatesRC(const QString &_filename)
{
    QDomDocument doc;
    QFile file(_filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Can't open " << _filename;
        return;
    }
    auto parseResult = doc.setContent(file.readAll());
    if (!parseResult) {
        qCritical() << "At (" << parseResult.errorLine << ", " << parseResult.errorColumn << ") " << parseResult.errorMessage;
        file.close();
        return;
    }
    file.close();

    QDir directory = QFileInfo(_filename).absoluteDir();

    QDomElement docElem = doc.documentElement();
    if (docElem.nodeName() != "states") {
        qCritical() << "Invalid state file";
        return;
    }
    QDomNode nCat = docElem.firstChild();
    while (!nCat.isNull()) {
        QDomElement eCat = nCat.toElement(); // try to convert the node to an element.
        if (!eCat.isNull() && eCat.tagName() == "category") {
            QString catId = eCat.attribute("id");
            QString catName = eCat.attribute("name");
            int catPriority = eCat.attribute("priority", "1000").toInt();
            StateCategory *category = nullptr;
            if (catId.isEmpty()) {
                qCritical() << "Missing category id";
            } else {
                if (categories.contains(catId)) {
                    category = categories[catId];
                } else if (!catName.isEmpty()) {
                    category = new StateCategory(catId, i18n(catName.toUtf8()), catPriority);
                    categories[catId] = category;
                }
                if (category) {
                    // Parse the states
                    QDomNode nState = eCat.firstChild();
                    while (!nState.isNull()) {
                        QDomElement eState = nState.toElement();
                        if (!eState.isNull() && eState.tagName() == "state") {
                            QString stateId = eState.attribute("id");
                            QString stateName = eState.attribute("name");
                            QString stateFilename = eState.attribute("filename");
                            int statePriority = eState.attribute("priority", "1000").toInt();
                            if (stateId.isEmpty() || stateName.isEmpty() || stateFilename.isEmpty()) {
                                qCritical() << "Missing attribute: id = " << stateId << " name = " << stateName << " filename = " << stateFilename;
                            } else {
                                QString file = directory.absoluteFilePath(stateFilename);
                                if (QFileInfo::exists(file)) {
                                    if (category->d->states.contains(stateId)) {
                                        delete category->d->states[stateId];
                                    }
                                    qDebug() << "Adding state id = " << stateId << " name = " << stateName << " filename = " << stateFilename;
                                    category->d->states[stateId] = new State(stateId, stateName, category, file, statePriority);
                                } else {
                                    qCritical() << "Missing file " << file;
                                }
                            }
                        } else {
                            qCritical() << "Invalid node in category " << catId;
                        }
                        nState = nState.nextSibling();
                    }
                } else {
                    qCritical() << "Couldn't make a category for " << catId;
                }
            }
        } else {
            qCritical() << "Invalid XML node.";
        }
        nCat = nCat.nextSibling();
    }
}

StatesRegistry::StatesRegistry()
    : d(new Private)
{
    QStringList statesFilenames;
    const QStringList stateFileDirs =
        QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, "calligra_shape_state/states/", QStandardPaths::LocateDirectory);
    Q_FOREACH (const QString &dir, stateFileDirs) {
        QDirIterator iter(dir, QStringList() << QStringLiteral("*.xml"));
        while (iter.hasNext()) {
            statesFilenames.append(iter.next());
        }
    }

    foreach (const QString &filename, statesFilenames) {
        qDebug() << "Load state: " << filename;
        d->parseStatesRC(filename);
    }
}

StatesRegistry::~StatesRegistry()
{
    delete d;
}

const StatesRegistry *StatesRegistry::instance()
{
    if (!Private::s_instance) {
        Private::s_instance = new StatesRegistry;
    }
    return Private::s_instance;
}

QList<QString> StatesRegistry::categorieIds() const
{
    return d->categories.keys();
}

QList<QString> StatesRegistry::stateIds(const QString &_id) const
{
    Q_ASSERT(d->categories.contains(_id));
    return d->categories[_id]->stateIds();
}

const State *StatesRegistry::state(const QString &_category, const QString &_state) const
{
    if (d->categories.contains(_category))
        return d->categories[_category]->state(_state);
    qWarning() << "No category " << _category << " found among " << d->categories.keys();
    return nullptr;
}

const State *StatesRegistry::nextState(const State *_state) const
{
    if (_state) {
        QList<const State *> states = _state->category()->d->states.values();
        int idx = states.indexOf(_state);
        idx += 1;
        if (idx >= states.count())
            idx = 0;
        return states[idx];
    }
    return nullptr;
}
