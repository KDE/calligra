/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2008 Johannes Simon <johannes.simon@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef TEXTLABELDUMMY_H
#define TEXTLABELDUMMY_H

#include <KoShape.h>

class TextLabelDummy : public KoShape
{
public:
    void paint(QPainter &, const KoViewConverter &, KoShapePaintingContext &paintcontext) override;
    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override;
    void saveOdf(KoShapeSavingContext &) const override;
};

#endif
