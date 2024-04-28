/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOCONNECTIONSHAPELOADINGUPDATER_H
#define KOCONNECTIONSHAPELOADINGUPDATER_H

#include <KoLoadingShapeUpdater.h>

class KoConnectionShape;
class KoShape;

class KoConnectionShapeLoadingUpdater : public KoLoadingShapeUpdater
{
public:
    enum ConnectionPosition { First, Second };

    KoConnectionShapeLoadingUpdater(KoConnectionShape *connectionShape, ConnectionPosition position);
    ~KoConnectionShapeLoadingUpdater() override;

    void update(KoShape *shape) override;

private:
    KoConnectionShape *m_connectionShape;
    ConnectionPosition m_position;
};

#endif /* KOCONNECTIONSHAPELOADINGUPDATER_H */
