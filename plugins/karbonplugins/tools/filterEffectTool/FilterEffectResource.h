/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTEREFFECTRESOURCE_H
#define FILTEREFFECTRESOURCE_H

#include "KoResource.h"
#include <KoXmlReader.h>

class KoFilterEffectStack;

class FilterEffectResource : public KoResource
{
public:
    explicit FilterEffectResource(const QString &filename);

    /// reimplemented from KoResource
    bool load() override;

    /// reimplemented from KoResource
    bool loadFromDevice(QIODevice *dev) override;

    /// reimplemented from KoResource
    bool save() override;

    /// reimplemented from KoResource
    bool saveToDevice(QIODevice *dev) const override;

    /// reimplemented from KoResource
    QString defaultFileExtension() const override;

    /// Creates resource from given filter effect stack
    static FilterEffectResource *fromFilterEffectStack(KoFilterEffectStack *filterStack);

    /// Creates a new filter stack from this filter resource
    KoFilterEffectStack *toFilterStack() const;

protected:
    QByteArray generateMD5() const override;

private:
    QDomDocument m_data;
};

#endif // FILTEREFFECTRESOURCE_H
