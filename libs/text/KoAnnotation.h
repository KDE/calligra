/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOANNOTATION_H
#define KOANNOTATION_H

#include "KoTextRange.h"
#include "kotext_export.h"

class KoShape;
class KoAnnotationManager;

/**
 * An annotation is a note made by the user regarding a part of the
 * text. The annotation refers to either a position or a range of
 * text. The annotation location will be automatically updated if user
 * alters the text in the document.

 * An annotation is identified by it's name, and all annotations are
 * managed by KoAnnotationManager. An annotation can be retrieved from
 * the annotation manager by using name as identifier.
 *
 * @see KoAnnotationManager
 */
class KOTEXT_EXPORT KoAnnotation : public KoTextRange
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * By default an annotation has the SinglePosition type and an empty name.
     * The name is set when the annotation is inserted into the annotation manager.
     *
     * @param document the text document where this annotation is located
     */
    explicit KoAnnotation(const QTextCursor &);

    explicit KoAnnotation(QTextDocument *document, int position);

    ~KoAnnotation() override;

    /// reimplemented from super
    void saveOdf(KoShapeSavingContext &context, int position, TagType tagType) const override;

    /**
     * Set the new name for this annotation
     * @param name the new name of the annotation
     */
    void setName(const QString &name);

    /// @return the name of this annotation
    QString name() const;

    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    /**
     * This is called to allow Cut and Paste of annotations. This
     * method gives a correct, unique, name
     */
    static QString createUniqueAnnotationName(const KoAnnotationManager *kam, const QString &annotationName, bool isEndMarker);

    void setAnnotationShape(KoShape *shape);

    KoShape *annotationShape() const;

private:
    class Private;
    Private *const d;
};

#endif
