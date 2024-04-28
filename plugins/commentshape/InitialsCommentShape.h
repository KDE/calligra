/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef INITIALSCOMMENTSHAPE_H
#define INITIALSCOMMENTSHAPE_H

#include <KoShape.h>

class InitialsCommentShape : public KoShape
{
public:
    InitialsCommentShape();
    virtual ~InitialsCommentShape();

    virtual void saveOdf(KoShapeSavingContext &context) const;
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual void paint(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext);

    void setInitials(const QString &initials);
    QString initials();

    bool isActive() const;
    void setActive(bool activate);
    void toogleActive();

private:
    bool m_active;
    QString m_initials;
};

#endif // INITIALSCOMMENTSHAPE_H
