/* This file is part of the KDE project
 * Copyright 2014  Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
