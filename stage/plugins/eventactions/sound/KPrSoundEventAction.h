/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRSOUNDEVENTACTION_H
#define KPRSOUNDEVENTACTION_H

#include <KoEventAction.h>

#include <QObject>

namespace Phonon
{
class MediaObject;
}

class KPrSoundData;

#define KPrSoundEventActionId "KPrSoundEventAction"

class KPrSoundEventAction : public QObject, public KoEventAction
{
    Q_OBJECT
public:
    KPrSoundEventAction();
    virtual ~KPrSoundEventAction();

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext &context) const override;

    void start() override;
    void finish() override;

    void setSoundData(KPrSoundData *soundData);
    KPrSoundData *soundData() const;

public Q_SLOTS:
    void finished();

private:
    Phonon::MediaObject *m_media;
    KPrSoundData *m_soundData;
};

#endif /* KPRSOUNDEVENTACTION_H */
