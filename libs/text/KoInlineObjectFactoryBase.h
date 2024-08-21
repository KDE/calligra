/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOINLINEOBJECTFACTORY_H
#define KOINLINEOBJECTFACTORY_H

#include <QString>
#include <QVector>

#include "kotext_export.h"

class KoInlineObject;
class InlineObjectFactoryPrivate;
class KoProperties;

/// A template used in the KoInlineObjectFactoryBase
struct KOTEXT_EXPORT KoInlineObjectTemplate {
    QString id; ///< The id of the inlineObject
    QString name; ///< The name to be shown for this template
    /**
     * The properties which, when passed to the KoInlineObjectFactoryBase::createInlineObject() method
     * result in the object this template represents.
     */
    const KoProperties *properties;
};

Q_DECLARE_TYPEINFO(KoInlineObjectTemplate, Q_MOVABLE_TYPE);

/**
 * A factory for inline text objects. There should be one for each plugin type to
 * allow the creation of the inlineObject from that plugin.
 * The factory additionally has information to allow showing a menu entry for user
 * access to the object-type.
 * @see KoInlineObjectRegistry
 */
class KOTEXT_EXPORT KoInlineObjectFactoryBase
{
public:
    /// The type of inlineObject this factory creates.
    enum ObjectType {
        TextVariable, ///< The factory creates KoVariable inheriting objects.
        Other = 0x100 ///< The factory creates objects that should not be shown in any menu
    };

    /**
     * Create the new factory
     * @param id a string that will be used internally for referencing the variable-type.
     * @param type the factory type.
     */
    KoInlineObjectFactoryBase(const QString &id, ObjectType type);
    virtual ~KoInlineObjectFactoryBase();

    /**
     * Create a new instance of an inline object.
     */
    virtual KoInlineObject *createInlineObject(const KoProperties *properties = nullptr) const = 0;

    /**
     * return the id for the variable this factory creates.
     * @return the id for the variable this factory creates.
     */
    QString id() const;

    /**
     * Returns the type of object this factory creates.
     * The main purpose is to group plugins per type in, for example, a menu.
     */
    ObjectType type() const;

    /**
     * Return all the templates this factory knows about.
     * Each template shows a different way to create an object this factory is specialized in.
     */
    QVector<KoInlineObjectTemplate> templates() const;

    QStringList odfElementNames() const;

    QString odfNameSpace() const;

    void setOdfElementNames(const QString &nameSpace, const QStringList &names);

protected:
    /**
     * Add a template with the properties of a specific type of object this factory can generate
     * using the createInlineObject() method.
     * @param params The new template this factory knows to produce
     */
    void addTemplate(const KoInlineObjectTemplate &params);

private:
    InlineObjectFactoryPrivate *const d;
};

#endif
