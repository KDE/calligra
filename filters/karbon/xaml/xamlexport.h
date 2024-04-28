/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002, 2003 The Karbon Developers

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __XAMLEXPORT_H__
#define __XAMLEXPORT_H__

#include <KoFilter.h>

#include "vgradient.h"
#include "vvisitor.h"

#include "xamlgraphiccontext.h"

#include <QByteArray>
#include <QStack>
#include <QTextStream>
#include <QVariantList>

class QTextStream;
class VColor;
class VPath;
class KarbonDocument;
class VFill;
class VGroup;
class VSubpath;
class VStroke;
class VText;

class XAMLExport : public KoFilter, private VVisitor
{
    Q_OBJECT

public:
    XAMLExport(KoFilter *parent, const char *name, const QVariantList &);
    virtual ~XAMLExport()
    {
    }

    virtual KoFilter::ConversionStatus convert(const QByteArray &from, const QByteArray &to);

private:
    virtual void visitVPath(VPath &composite);
    virtual void visitVDocument(KarbonDocument &document);
    virtual void visitVGroup(VGroup &group);
    virtual void visitVSubpath(VSubpath &path);
    // virtual void visitVText( VText& text );

    void getStroke(const VStroke &stroke);
    void getColorStops(const QVector<VColorStop *> &colorStops);
    void getFill(const VFill &fill);
    void getGradient(const VGradient &grad);
    void getHexColor(QTextStream *, const VColor &color);
    QString getID(VObject *obj);

    QTextStream *m_stream;
    QTextStream *m_defs;
    QTextStream *m_body;

    QStack<XAMLGraphicsContext *> m_gc;
};

#endif
