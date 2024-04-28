/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2005 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2007-2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWODFSHAREDLOADINGDATA_H
#define KWODFSHAREDLOADINGDATA_H

#include <KoTextSharedLoadingData.h>

#include <QHash>

class KWOdfLoader;
class KWFrame;

class KWOdfSharedLoadingData : public KoTextSharedLoadingData
{
public:
    explicit KWOdfSharedLoadingData(KWOdfLoader *loader);

    void connectFlowingTextShapes();

protected:
    void shapeInserted(KoShape *shape, const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    /**
     * Load all properties from style and apply them on the frame.
     * @return returns false if the frame passed in has been deleted and replaced with a copy frame, true otherwise.
     */
    bool fillFrameProperties(KWFrame *frame, const KoXmlElement &style);

    KWOdfLoader *m_loader;
    QHash<KoShape *, QString> m_nextShapeNames; // store the 'chain-next-name'
    QList<KoShape *> m_shapesToProcess;
};

#endif
