/*
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "KoSectionEnd.h"

#include <KoSection.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>

class KoSectionEndPrivate
{
public:
    KoSectionEndPrivate(KoSection *_section)
        : section(_section)
    {
        Q_ASSERT(section);
    }

    KoSection *section; //< pointer to the corresponding section
};

KoSectionEnd::KoSectionEnd(KoSection *section)
    : d_ptr(new KoSectionEndPrivate(section))
{
    Q_D(KoSectionEnd);
    d->section->setSectionEnd(this);
}

KoSectionEnd::~KoSectionEnd() = default;

QString KoSectionEnd::name() const
{
    Q_D(const KoSectionEnd);
    return d->section->name();
}

KoSection *KoSectionEnd::correspondingSection() const
{
    Q_D(const KoSectionEnd);
    return d->section;
}

void KoSectionEnd::saveOdf(KoShapeSavingContext &context) const
{
    KoXmlWriter *writer = &context.xmlWriter();
    Q_ASSERT(writer);
    writer->endElement();
}
