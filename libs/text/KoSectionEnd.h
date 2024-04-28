/*
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOSECTIONEND_H
#define KOSECTIONEND_H

#include "kotext_export.h"

#include <QList>
#include <QMetaType>
#include <QScopedPointer>
#include <QString>

class KoShapeSavingContext;
class KoSection;

class KoSectionEndPrivate;
/**
 * Marks the end of the section
 */
class KOTEXT_EXPORT KoSectionEnd
{
public:
    ~KoSectionEnd(); // this is needed for QScopedPointer

    void saveOdf(KoShapeSavingContext &context) const;

    QString name() const;
    KoSection *correspondingSection() const;

protected:
    const QScopedPointer<KoSectionEndPrivate> d_ptr;

private:
    Q_DISABLE_COPY(KoSectionEnd)
    Q_DECLARE_PRIVATE(KoSectionEnd)

    explicit KoSectionEnd(KoSection *section);

    friend class KoSectionModel;
    friend class TestKoTextEditor;
};

Q_DECLARE_METATYPE(KoSectionEnd *)
Q_DECLARE_METATYPE(QList<KoSectionEnd *>)

#endif // KOSECTIONEND_H
