/*
 *  SPDX-FileCopyrightText: 2007 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef TestColorConversionSystem_H
#define TestColorConversionSystem_H

#include <QList>
#include <QObject>
#include <QString>

struct ModelDepthProfile {
    ModelDepthProfile(const QString &_model, const QString &_depth, const QString &_profile)
        : model(_model)
        , depth(_depth)
        , profile(_profile)
    {
    }
    QString model;
    QString depth;
    QString profile;
};

class TestColorConversionSystem : public QObject
{
    Q_OBJECT
public:
    TestColorConversionSystem();
private Q_SLOTS:
    void testConnections_data();
    void testConnections();
    void testGoodConnections_data();
    void testGoodConnections();
    void testFailedConnections();

private:
    QList<ModelDepthProfile> listModels;
    int countFail;
};

#endif
