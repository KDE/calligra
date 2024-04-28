/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002 The Karbon Developers

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __XAMLIMPORT_H__
#define __XAMLIMPORT_H__

#include "xamlgraphiccontext.h"
#include <KoFilter.h>
#include <QByteArray>
#include <QDomDocument>
#include <QMap>
#include <QStack>
#include <QTransform>
#include <QVariantList>
#include <core/KarbonDocument.h>
#include <core/vfill.h>
#include <core/vfillrule.h>
#include <core/vgradient.h>
#include <core/vstroke.h>

class VGroup;

class XAMLImport : public KoFilter
{
    Q_OBJECT

public:
    XAMLImport(QObject *parent, const QVariantList &);
    virtual ~XAMLImport();

    virtual KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to);

protected:
    class GradientHelper
    {
    public:
        GradientHelper()
        {
            bbox = true;
        }
        VGradient gradient;
        bool bbox;
        QTransform gradientTransform;
    };

    void parseGroup(VGroup *, const QDomElement &);
    void parseStyle(VObject *, const QDomElement &);
    void parsePA(VObject *, XAMLGraphicsContext *, const QString &, const QString &);
    void parseGradient(const QDomElement &);
    void parseColorStops(VGradient *, const QDomElement &);
    double parseUnit(const QString &, bool horiz = false, bool vert = false, KoRect bbox = KoRect());
    void parseColor(VColor &, const QString &);
    QColor parseColor(const QString &);
    double toPercentage(QString);
    double fromPercentage(QString);
    void setupTransform(const QDomElement &);
    void addGraphicContext();
    QDomDocument inpdoc;
    QDomDocument outdoc;
    void convert();
    VObject *createObject(const QDomElement &);
    void createText(VGroup *, const QDomElement &);
    void parseFont(const QDomElement &);
    // find object with given id in document
    VObject *findObject(const QString &name);
    // find object with given id in given group
    VObject *findObject(const QString &name, VGroup *);

private:
    KarbonDocument m_document;
    QStack<XAMLGraphicsContext *> m_gc;
    QMap<QString, GradientHelper> m_gradients;
    QMap<QString, QDomElement> m_paths;
    KoRect m_outerRect;
};

#endif
