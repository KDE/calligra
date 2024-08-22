/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoAnnotationManager.h"
#include "KoAnnotation.h"

#include "TextDebug.h"
#include <QHash>

class KoAnnotationManagerPrivate
{
public:
    KoAnnotationManagerPrivate() = default;
    QHash<QString, KoAnnotation *> annotationHash;
    QList<QString> annotationNameList;
    int lastId;
};

KoAnnotationManager::KoAnnotationManager()
    : d(new KoAnnotationManagerPrivate)
{
}

KoAnnotationManager::~KoAnnotationManager()
{
    delete d;
}

void KoAnnotationManager::insert(const QString &name, KoAnnotation *annotation)
{
    annotation->setName(name);
    d->annotationHash[name] = annotation;
    d->annotationNameList.append(name);
}

void KoAnnotationManager::remove(const QString &name)
{
    d->annotationHash.remove(name);
    d->annotationNameList.removeAt(d->annotationNameList.indexOf(name));
}

void KoAnnotationManager::rename(const QString &oldName, const QString &newName)
{
    QHash<QString, KoAnnotation *>::iterator i = d->annotationHash.begin();

    while (i != d->annotationHash.end()) {
        if (i.key() == oldName) {
            KoAnnotation *annotation = d->annotationHash.take(i.key());
            annotation->setName(newName);
            d->annotationHash.insert(newName, annotation);
            int listPos = d->annotationNameList.indexOf(oldName);
            d->annotationNameList.replace(listPos, newName);
            return;
        }
        ++i;
    }
}

KoAnnotation *KoAnnotationManager::annotation(const QString &name) const
{
    KoAnnotation *annotation = d->annotationHash.value(name);
    return annotation;
}

QList<QString> KoAnnotationManager::annotationNameList() const
{
    return d->annotationNameList;
}
