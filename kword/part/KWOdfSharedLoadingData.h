/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007-2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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

protected:
    virtual void shapeInserted(KoShape *shape, const KoXmlElement &element, KoShapeLoadingContext &context);

private:
    /**
     * Load all properties from style and apply them on the frame.
     * @return returns false if the frame passed in has been deleted and replaced with a copy frame, true otherwise.
     */
    bool fillFrameProperties(KWFrame *frame, const KoXmlElement &style);

    KWOdfLoader *m_loader;
    QHash<QString, KWFrame*> m_nextFrames; // store the 'chain-next-name' property to the frame it was found on
};

#endif
