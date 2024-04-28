/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KWFACTORY_H
#define KWFACTORY_H

#include "words_export.h"
#include <KPluginFactory>

class KoComponentData;

/**
 * Factory for the Words application.
 * Will use the Words library to create a new instance of the Words doc.
 */
class WORDS_EXPORT KWFactory : public KPluginFactory
{
    Q_OBJECT
public:
    /**
     * Constructor
     */
    explicit KWFactory();
    ~KWFactory() override;

    /// overwritten method from superclass
    QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args) override;

    static const KoComponentData &componentData();

private:
    static KoComponentData *s_componentData;
};

#endif
