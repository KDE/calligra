/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEST_KO_INLINE_TEXT_OBJECT_MANAGER_H
#define TEST_KO_INLINE_TEXT_OBJECT_MANAGER_H

#include <QObject>
#include <QString>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include <KoInlineObject.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

class DummyInlineObject : public KoInlineObject
{
    Q_OBJECT
public:
    DummyInlineObject(bool propertyListener)
        : KoInlineObject(propertyListener)
        , m_position(-1)
    {
    }

    ~DummyInlineObject() override = default;

    void saveOdf(KoShapeSavingContext &context) override
    {
        Q_UNUSED(context)
        // dummy impl
    }

    bool loadOdf(const KoXmlElement &, KoShapeLoadingContext &) override
    {
        // dummy impl
        return false;
    }

    void updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format) override
    {
        Q_UNUSED(format)
        Q_ASSERT(posInDocument <= document->toPlainText().size());
        Q_UNUSED(document);
        m_position = posInDocument;
    }

    void resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd) override
    {
        Q_UNUSED(document)
        Q_UNUSED(object)
        Q_UNUSED(posInDocument)
        Q_UNUSED(format)
        Q_UNUSED(pd)
        // dummy impl
    }

    void paint(QPainter &painter,
               QPaintDevice *pd,
               const QTextDocument *document,
               const QRectF &rect,
               const QTextInlineObject &object,
               int posInDocument,
               const QTextCharFormat &format) override
    {
        Q_UNUSED(painter)
        Q_UNUSED(pd)
        Q_UNUSED(document)
        Q_UNUSED(rect)
        Q_UNUSED(object)
        Q_UNUSED(posInDocument)
        Q_UNUSED(format)
        // dummy impl
    }

    void propertyChanged(Property property, const QVariant &value) override
    {
        Q_UNUSED(property)
        m_property = value;
    }

    QVariant m_property;
    int m_position;
};

class TestKoInlineTextObjectManager : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void testCreation();
    void testInsertInlineObject();
    void testRetrieveInlineObject();
    void testRemoveInlineObject();
    void testListenToProperties();
};

#endif // TEST_KO_INLINE_TEXT_OBJECT_MANAGER_H
