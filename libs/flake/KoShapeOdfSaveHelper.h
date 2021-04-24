/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPEODFSAVEHELPER_H
#define KOSHAPEODFSAVEHELPER_H

#include "KoDragOdfSaveHelper.h"
#include "flake_export.h"

#include <QList>

class KoShape;
class KoShapeOdfSaveHelperPrivate;

/**
 * Save helper for saving shapes to odf.
 *
 * The shapes are saved in an office:text document.
 */
class FLAKE_EXPORT KoShapeOdfSaveHelper : public KoDragOdfSaveHelper
{
public:
    /**
     * Constructor
     *
     * @param shapes The list of shapes to save. If the shapes contain
     *               children these are also saved.
     */
    explicit KoShapeOdfSaveHelper(const QList<KoShape *> &shapes);

    /// reimplemented
    bool writeBody() override;

private:
    Q_DECLARE_PRIVATE(KoShapeOdfSaveHelper)
};

#endif /* KOSHAPEODFSAVEHELPER_H */
