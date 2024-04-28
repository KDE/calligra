/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Vidhyapria  Arunkumar <vidhyapria.arunkumar@nokia.com>
 * Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PLUGINSHAPE_H
#define PLUGINSHAPE_H

#include <KoFrameShape.h>
#include <KoShape.h>
#include <QMap>

#define PLUGINSHAPEID "PluginShape"

class PluginShape : public KoShape, public KoFrameShape
{
public:
    PluginShape();
    ~PluginShape() override;

    // reimplemented
    void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext) override;
    // reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;
    // reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

protected:
    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;

private:
    // Note:- We assume that all the name of draw:param are unique.
    QMap<QString, QString> m_drawParams;
    QString m_mimetype;
    QString m_xlinkactuate;
    QString m_xlinkhref;
    QString m_xlinkshow;
    QString m_xlinktype;
};

#endif
