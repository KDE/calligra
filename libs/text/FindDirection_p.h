/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef FINDDIRECTION_P_H
#define FINDDIRECTION_P_H

class KoCanvasResourceManager;
class KoFindPrivate;
class QTextCursor;
class QTextDocument;

class FindDirection
{
public:
    explicit FindDirection(KoCanvasResourceManager *provider);
    virtual ~FindDirection();

    virtual bool positionReached(const QTextCursor &currentPos, const QTextCursor &endPos) = 0;

    virtual void positionCursor(QTextCursor &currentPos) = 0;

    virtual void select(const QTextCursor &cursor) = 0;

    virtual void nextDocument(QTextDocument *document, KoFindPrivate *findPrivate) = 0;

protected:
    KoCanvasResourceManager *m_provider;
};

class FindForward : public FindDirection
{
public:
    explicit FindForward(KoCanvasResourceManager *provider);
    ~FindForward() override;

    bool positionReached(const QTextCursor &currentPos, const QTextCursor &endPos) override;

    void positionCursor(QTextCursor &currentPos) override;

    void select(const QTextCursor &cursor) override;

    void nextDocument(QTextDocument *document, KoFindPrivate *findPrivate) override;
};

class FindBackward : public FindDirection
{
public:
    explicit FindBackward(KoCanvasResourceManager *provider);
    ~FindBackward() override;

    bool positionReached(const QTextCursor &currentPos, const QTextCursor &endPos) override;

    void positionCursor(QTextCursor &currentPos) override;

    void select(const QTextCursor &cursor) override;

    void nextDocument(QTextDocument *document, KoFindPrivate *findPrivate) override;
};

#endif // FINDDIRECTION_P_H
