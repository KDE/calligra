/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef COMMENTSHAPE_H
#define COMMENTSHAPE_H

#include <KoDocumentResourceManager.h>
#include <KoShapeContainer.h>

#include <QDate>

class KoTextShapeData;
class InitialsCommentShape;

class CommentShape : public KoShapeContainer
{
public:
    explicit CommentShape(KoDocumentResourceManager *resourceManager);
    virtual ~CommentShape();

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);
    virtual void saveOdf(KoShapeSavingContext &context) const;
    virtual void setSize(const QSizeF &size);

    virtual void paintComponent(QPainter &painter, const KoViewConverter &converter, KoShapePaintingContext &paintcontext);

    void toogleActive();
    void setActive(bool active);
    bool isActive() const;

private:
    KoTextShapeData *commentData() const;

    QString m_creator;
    QDate m_date;
    KoShape *m_comment;
    InitialsCommentShape *m_initials;

    bool m_active;
};

#endif // COMMENTSHAPE_H
