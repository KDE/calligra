// This file is part of the KDE project
// SPDX-FileCopyrightText: 2015 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#ifndef MOCKPART_H
#define MOCKPART_H

#include <KoComponentData.h>
#include <KoPart.h>

#include <KAboutData>

#include <QGraphicsItem>

class MockPart : public KoPart
{
public:
    MockPart()
        : KoPart(KoComponentData(KAboutData(QStringLiteral("test"), QStringLiteral("Test"), QStringLiteral("0.0.9"))), nullptr)
    {
    }
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override
    {
        Q_UNUSED(document);
        Q_UNUSED(parent);
        return nullptr;
    }
    KoMainWindow *createMainWindow() override
    {
        return nullptr;
    }

protected:
    QGraphicsItem *createCanvasItem(KoDocument *document) override
    {
        Q_UNUSED(document);
        return nullptr;
    }
};

#endif // MOCKPART_H
