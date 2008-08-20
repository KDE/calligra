/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIQUERYDESIGNERSQLHISTORY_H
#define KEXIQUERYDESIGNERSQLHISTORY_H

#include <q3scrollview.h>
#include <qdatetime.h>
#include <qlist.h>
#include <qmap.h>
#include <QMouseEvent>

class Q3SimpleRichText;
class KMenu;
class KexiQueryDesignerSQLHistory;

class HistoryEntry
{
public:
    HistoryEntry(KexiQueryDesignerSQLHistory *parent, bool success, const QTime &time,
                 const QString &statement, /*int y,*/ const QString &error = QString());
    ~HistoryEntry();

    QRect geometry(int y, int width, const QFontMetrics& f);
    void drawItem(QPainter *p, int width);

    void setSelected(bool selected);
    bool isSelected() const {
        return m_selected;
    }
    void highlight();

    QString statement() const {
        return m_statement;
    }
    void setTime(const QTime &execTime);

private:
    QTime m_execTime;
    QString m_execTimeString;
    QString m_statement;
    QString m_error;
    Q3SimpleRichText *m_formated;
    KexiQueryDesignerSQLHistory *m_parent;

    int m_y;
bool m_succeeded : 1;
bool m_selected : 1;
};

typedef QList<HistoryEntry*> History;

class KexiQueryDesignerSQLHistory : public Q3ScrollView
{
    Q_OBJECT

public:
    KexiQueryDesignerSQLHistory(QWidget *parent);
    virtual ~KexiQueryDesignerSQLHistory();

    KMenu* popupMenu() const;

//  void  contextMenu(const QPoint &pos, HistoryEntry *e);

    void setHistory(History *h);

    QString selectedStatement() const;

public slots:
    void addEvent(const QString& q, bool s, const QString &error);

    void slotToClipboard();
    void slotEdit();

    void clear();

//  HistoryItem itemAt(int y);

protected:
    void addEntry(HistoryEntry *e);
    virtual void drawContents(QPainter *p, int cx, int cy, int cw, int ch);
    virtual void contentsMousePressEvent(QMouseEvent * e);
    virtual void contentsMouseDoubleClickEvent(QMouseEvent * e);
    virtual bool eventFilter(QObject *obj, QEvent *event);

signals:
    void editRequested(const QString &text);
    void currentItemDoubleClicked();

private:
    History *m_history;
    HistoryEntry *m_selected;
    QPalette::ColorGroup m_prevColorGroup;
    KMenu *m_popup;
};

#endif
