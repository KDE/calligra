/*
 *  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_COLOR_SPACE_ENGINE_H_
#define _KO_COLOR_SPACE_ENGINE_H_

#include <KoColorConversionTransformationAbstractFactory.h>
#include <KoGenericRegistry.h>

/**
 * A KoColorSpaceEngine is a class use to create color conversion
 * transformation between color spaces, for which all profiles can
 * output to all profiles.
 *
 * Typically, when you have an ICC color space and color profile, you
 * can convert to any other ICC color space and color profile. While
 * creating a KoColorTransformationFactory for each of this transformation
 * is possible, the number of links will make the Color Conversion explode
 * System. KoColorSpaceEngine provides a virtual node in the Color
 * Conversion System that can convert to any other node supported by the
 * engine.
 */
class PIGMENTCMS_EXPORT KoColorSpaceEngine : public KoColorConversionTransformationAbstractFactory
{
public:
    KoColorSpaceEngine(const QString &id, const QString &name);
    ~KoColorSpaceEngine() override;
    const QString &id() const;
    const QString &name() const;
    virtual void addProfile(const QString &filename) = 0;
    virtual void removeProfile(const QString &filename) = 0;

private:
    struct Private;
    Private *const d;
};

class PIGMENTCMS_EXPORT KoColorSpaceEngineRegistry : public KoGenericRegistry<KoColorSpaceEngine *>
{
public:
    KoColorSpaceEngineRegistry();
    ~KoColorSpaceEngineRegistry() override;
    static KoColorSpaceEngineRegistry *instance();
};

#endif
