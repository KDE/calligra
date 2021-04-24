/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#include "gitplugin.h"
#include "gitcontroller.h"
#include "gitlogmodel.h"
#include "checkoutcreator.h"

#include <QtQml/QtQml>
#include <QQmlEngine>
#include <QQmlContext>
#include <QApplication>

void GitPlugin::registerTypes(const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT( uri == QLatin1String( "Calligra.Gemini.Git" ) );
    qmlRegisterType<GitController>("Calligra.Gemini.Git", 1, 0, "GitController");
    qmlRegisterType<GitLogModel>("Calligra.Gemini.Git", 1, 0, "GitLogModel");
    qmlRegisterType<QAbstractListModel>();
}

void GitPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    Q_UNUSED(uri)
    Q_ASSERT( uri == QLatin1String( "Calligra.Gemini.Git" ) );

    CheckoutCreator* ac = new CheckoutCreator(qApp);

    QQmlContext *context = engine->rootContext();
    context->setContextProperty("GitCheckoutCreator", ac);
}
