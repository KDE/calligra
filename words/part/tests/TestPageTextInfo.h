/* This file is part of the KOffice project
 * Copyright (C) 2008 Thomas Zander <zander@kde.org>
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

#ifndef KWPAGETEXTINFO_H
#define KWPAGETEXTINFO_H

<<<<<<< HEAD:words/part/tests/TestPageTextInfo.h
#include <QObject>
#include <QtTest/QtTest>
#include <qtest_kde.h>

class TestPageTextInfo : public QObject
{
    Q_OBJECT
private slots: // tests
    void init();
    void testPageNumber();
=======
#include <krita_export.h>
#include <QList>

class KisColorSource;
class KisPropertiesConfiguration;
class KoID;
class KisPainter;

class PAINTOP_EXPORT KisColorSourceOption
{
public:
    enum Type {
      PLAIN,
      GRADIENT,
      UNIFORM_RANDOM,
      TOTAL_RANDOM,
      PATTERN,
      PATTERN_LOCKED
    };
public:
    KisColorSourceOption();
    ~KisColorSourceOption();
    void writeOptionSetting(KisPropertiesConfiguration* setting) const;
    void readOptionSetting(const KisPropertiesConfiguration* setting);

    KisColorSource* createColorSource(const KisPainter* _painter) const;
    QString colorSourceTypeId() const;
    void setColorSourceType(Type _type);
    void setColorSourceType(const QString& _type);
    static QList<KoID> sourceIds();
    
private:
    struct Private;
    Private* const d;
>>>>>>> add a widget to select the color source:krita/plugins/paintops/libpaintop/kis_color_source_option.h
};

#endif
