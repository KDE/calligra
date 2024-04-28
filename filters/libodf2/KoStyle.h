/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOSTYLE_H
#define KOSTYLE_H

#include <QSharedPointer>
#include <QString>

#include <KoGenStyle.h>
#include <KoGenStyles.h>

#include "koodf2_export.h"

/**
 * \class KoStyle
 * \brief This class is the base for all of the styles used in KoOdf.
 * Allows to easily share the styles among different components.
 *
 * As all the styles it can be shared
 **/

class KOODF2_EXPORT KoStyle
{
public:
    KoStyle();
    virtual ~KoStyle();

    QString saveOdf(KoGenStyles &styles) const;

    void setName(const QString &name);
    QString name() const;

    void setAutoStyleInStylesDotXml(bool b);
    bool autoStyleInStylesDotXml() const;

protected:
    virtual void prepareStyle(KoGenStyle &style) const = 0;
    virtual QString defaultPrefix() const = 0;
    virtual KoGenStyle::Type styleType() const = 0;
    virtual KoGenStyle::Type automaticstyleType() const = 0;
    virtual const char *styleFamilyName() const = 0;

private:
    KoGenStyles::InsertionFlags insertionFlags() const;

    QString m_name;
    bool m_autoStyleInStylesDotXml;
};

#define KOSTYLE_DECLARE_SHARED_POINTER(class)                                                                                                                  \
    typedef QSharedPointer<class> Ptr;                                                                                                                         \
    static Ptr create();

#define KOSTYLE_DECLARE_SHARED_POINTER_IMPL(class)                                                                                                             \
    class ::Ptr class ::create()                                                                                                                               \
    {                                                                                                                                                          \
        return QSharedPointer<class>(new class);                                                                                                               \
    }

#endif
