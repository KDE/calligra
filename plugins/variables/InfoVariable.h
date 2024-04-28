/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Pierre Ducroquet <pinaraf@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef INFOVARIABLE_H
#define INFOVARIABLE_H

#include <KoVariable.h>

/**
 * This is a KoVariable for document information.
 */
class InfoVariable : public KoVariable
{
public:
    /**
     * Constructor.
     */
    InfoVariable();

    void readProperties(const KoProperties *props);

    void propertyChanged(Property property, const QVariant &value) override;

    /// reimplemented
    void saveOdf(KoShapeSavingContext &context) override;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /// get the list of tags supported by this variable
    static QStringList tags();

private:
    KoInlineObject::Property m_type;
};

#endif
