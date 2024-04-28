/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "CalligraComponentsPlugin.h"

#include "Document.h"
#include "Enums.h"
#include "Global.h"
#include "ImageDataItem.h"
#include "LinkArea.h"
#include "View.h"
#include "ViewController.h"
#include "models/ContentsModel.h"

using namespace Calligra::Components;

QObject *singletonFactory(QQmlEngine *, QJSEngine *)
{
    return new Calligra::Components::Global{};
}

void CalligraComponentsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String{"org.kde.calligra"});

    qmlRegisterType<Calligra::Components::Document>(uri, 1, 0, "Document");
    qmlRegisterType<Calligra::Components::View>(uri, 1, 0, "View");
    qmlRegisterType<Calligra::Components::ViewController>(uri, 1, 0, "ViewController");
    qmlRegisterType<Calligra::Components::ContentsModel>(uri, 1, 0, "ContentsModel");
    qmlRegisterType<Calligra::Components::ImageDataItem>(uri, 1, 0, "ImageDataItem");
    qmlRegisterType<Calligra::Components::LinkArea>(uri, 1, 0, "LinkArea");

    qmlRegisterUncreatableType<Calligra::Components::DocumentType>(uri, 1, 0, "DocumentType", "Provides the DocumentType enum");
    qmlRegisterUncreatableType<Calligra::Components::DocumentStatus>(uri, 1, 0, "DocumentStatus", "Provides the DocumentStatus enum");

    qmlRegisterSingletonType<Calligra::Components::Global>(uri, 1, 0, "Global", &singletonFactory);
}
