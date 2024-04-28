/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOSHAPECONTAINERPRIVATE_H
#define KOSHAPECONTAINERPRIVATE_H

#include "KoShape_p.h"
#include "flake_export.h"

class KoShapeContainerModel;

/**
 * \internal used private d-pointer class for the \a KoShapeContainer class.
 */
class FLAKE_EXPORT KoShapeContainerPrivate : public KoShapePrivate
{
public:
    explicit KoShapeContainerPrivate(KoShapeContainer *q);
    ~KoShapeContainerPrivate() override;

    KoShapeContainerModel *model;
};

#endif
