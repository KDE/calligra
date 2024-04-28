/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KONAMEDVARIABLE_H
#define KONAMEDVARIABLE_H

#include "KoVariable.h"
#include "kotext_export.h"

/**
 * This inlineObject shows the current value of a variable as registered in the KoVariableManager.
 * The proper way to create a new class is to use KoVariableManager::createVariable()
 */
class KoNamedVariable : public KoVariable
{
    Q_OBJECT
public:
    /// return the name of this named variable
    QString name() const
    {
        return m_name;
    }

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) override;

protected:
    friend class KoVariableManager;
    /**
     * Constructor
     * @param key the property that represents the named variable. As defined internally in the KoVariableManager
     * @param name the name of the variable.
     */
    KoNamedVariable(Property key, const QString &name);

private:
    /// reimplemented method
    void propertyChanged(Property property, const QVariant &value) override;
    /// reimplemented method
    void setup() override;

    const QString m_name;
    const Property m_key;
};

#endif
