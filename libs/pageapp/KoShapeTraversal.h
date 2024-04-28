/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPETRAVERSAL_H
#define KOSHAPETRAVERSAL_H

class KoShape;
class KoShapeContainer;
class QString;

/**
 * @brief Class to traverse shapes
 */
class KoShapeTraversal
{
public:
    /**
     * @brief Get the next shape in the tree
     *
     * @param current The current shape
     *
     * @return The shape after the given one or 0 if there is no next shape
     */
    static KoShape *nextShape(const KoShape *current);

    /**
     * @brief Get the next shape in the tree of the given type
     *
     * @param current The current shape
     * @param shapeId The shape id of the shape to find
     *
     * @return The shape with the shape id given after the current one or 0 if there is no
     *         such shape after the current one
     */
    static KoShape *nextShape(const KoShape *current, const QString &shapeId);

    /**
     * @brief Get the previous shape in the tree
     *
     * @param current The current shape
     *
     * @return The shape before the given one
     */
    static KoShape *previousShape(const KoShape *current);

    /**
     * @brief Get the previous shape in the tree of the given type
     *
     * @param current The current shape
     * @param shapeId The shape id of the shape to find
     *
     * @return The shape with the shape id given before the current one or 0 if there is no
     *         such shape before the current one
     */
    static KoShape *previousShape(const KoShape *current, const QString &shapeId);

    /**
     * @brief Get the last shape in subtree
     *
     * @param current The current shape
     *
     * @return The last shape in the current sub tree
     */
    static KoShape *last(KoShape *current);

private:
    static KoShape *nextShapeStep(const KoShape *current, const KoShapeContainer *parent);

    static KoShape *previousShapeStep(const KoShape *current, const KoShapeContainer *parent);
};

#endif /* KOSHAPETRAVERSAL_H */
