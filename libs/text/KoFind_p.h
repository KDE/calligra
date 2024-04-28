/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFIND_P_H
#define KOFIND_P_H

#include <QTextCursor>

#include "FindDirection_p.h"
#include "KoFindStrategy.h"
#include "KoReplaceStrategy.h"

class KoFind;
class KoCanvasResourceManager;
class QAction;
class QTextDocument;
class QVariant;
class QWidget;

class KoFindPrivate
{
public:
    KoFindPrivate(KoFind *find, KoCanvasResourceManager *crp, QWidget *w);

    void resourceChanged(int key, const QVariant &variant);

    void findActivated();

    void findNextActivated();

    void findPreviousActivated();

    void replaceActivated();

    // executed when the user presses the 'find' button.
    void startFind();

    // executed when the user presses the 'replace' button.
    void startReplace();

    QAction *findNext;
    QAction *findPrev;

    void findDocumentSetNext(QTextDocument *document);
    void findDocumentSetPrevious(QTextDocument *document);

protected:
    void parseSettingsAndFind();

private:
    KoFind *q;
    KoCanvasResourceManager *provider;
    KoFindStrategy findStrategy; /// strategy used for find
    KoReplaceStrategy replaceStrategy; /// strategy used for replace
    KoFindStrategyBase *strategy; /// the current strategy used

    QTextDocument *document;
    QTextDocument *startDocument;
    QTextCursor lastKnownPosition;
    bool restarted;
    bool start; /// if true find/replace is restarted
    bool inFind; /// if true find/replace is running (not showing the dialog)
    QTextCursor startPosition;
    QTextCursor endPosition;
    FindDirection *findDirection; /// the current direction used
    FindForward findForward;
    FindBackward findBackward;
};

#endif /* KOFIND_P_H */
