/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOANNOTATIONMANAGER_H
#define KOANNOTATIONMANAGER_H

#include "kotext_export.h"

#include <QList>
#include <QObject>

class KoAnnotation;
class KoAnnotationManagerPrivate;

/**
 * A manager for all annotations in a document. Every annotation is identified by a unique name.
 * Note that only SinglePosition and StartAnnotation annotations can be retrieved from this
 * manager. An end annotation should be retrieved from it's parent (StartAnnotation) using
 * KoAnnotation::endAnnotation()
 * This class also maintains a list of annotation names so that it can be easily used to
 * show all available annotation.
 */
class KOTEXT_EXPORT KoAnnotationManager : public QObject
{
    Q_OBJECT
public:
    /// constructor
    KoAnnotationManager();
    ~KoAnnotationManager() override;

    /// @return an annotation with the specified name, or 0 if there is none
    KoAnnotation *annotation(const QString &name) const;

    /// @return a list of QString containing all annotation names
    QList<QString> annotationNameList() const;

public Q_SLOTS:
    /**
     * Insert a new annotation to this manager. The name of the annotation
     * will be set to @p name, no matter what name has been set on
     * it.
     * @param name the name of the annotation
     * @param annotation the annotation object to insert
     */
    void insert(const QString &name, KoAnnotation *annotation);

    /**
     * Remove an annotation from this manager.
     * @param name the name of the annotation to remove
     */
    void remove(const QString &name);

    /**
     * Rename an annotation
     * @param oldName the old name of the annotation
     * @param newName the new name of the annotation
     */
    void rename(const QString &oldName, const QString &newName);

private:
    KoAnnotationManagerPrivate *const d;
};

#endif
