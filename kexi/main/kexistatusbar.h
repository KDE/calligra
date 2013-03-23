/* This file is part of the KDE project
   Copyright (C) 2003-2010 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and,or
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

   Loosely based on kdevelop/src/statusbar.h
   Copyright (C) 2001 by Bernd Gehrmann <bernd@kdevelop.org>
*/

#ifndef KEXISTATUSBAR_H
#define KEXISTATUSBAR_H

//temporary
#define KexiStatusBar_KTEXTEDITOR_USED 0

#include <KStatusBar>

class KMenu;
class QCheckBox;

#if KexiStatusBar_KTEXTEDITOR_USED
namespace KTextEditor
{
    class ViewStatusMsgInterface;
    class ViewCursorInterface;
}
#endif

class KexiStatusBar : public KStatusBar
{
    Q_OBJECT
public:
    KexiStatusBar(QWidget *parent = 0);
    virtual ~KexiStatusBar();
//  virtual void addWidget( QWidget *widget, int stretch = 0, bool permanent = false);

//! @todo extend to more generic API
    QAction *m_showNavigatorAction;
    QAction *m_showPropertyEditorAction;

public slots:
    virtual void setStatus(const QString &str);
    virtual void setReadOnlyFlag(bool readOnly);

signals:

protected slots:
    void cursorPositionChanged();
    void setCursorPosition(int line, int col);

protected:
    int m_msgID, m_readOnlyID;
//  QLabel *m_status, *m_readOnlyStatus;

#if KexiStatusBar_KTEXTEDITOR_USED
    KTextEditor::ViewCursorInterface * m_cursorIface;
    KTextEditor::ViewStatusMsgInterface * m_viewmsgIface;
#endif
    KMenu *m_viewMenu;
};

#endif
