/* This file is part of the KDE libraries
    Copyright (C) 2001 David Faure <faure@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#ifndef koPartSelectAction_h
#define koPartSelectAction_h

#include <kaction.h>
#include <koQueryTrader.h>

/**
 * An action for inserting an embedded object.
 * In a toolbar it presents itself as a button that opens the part-select dialog,
 * but in a popupmenu it presents itself as a submenu with all available parts.
 */
class KoPartSelectAction : public KActionMenu
{
    Q_OBJECT

public:
    KoPartSelectAction( const QString& text, QObject* parent = 0, const char* name = 0 );
    KoPartSelectAction( const QString& text, const QString& icon,
                        QObject* parent = 0, const char* name = 0 );
    KoPartSelectAction( const QString& text, const QString& icon,
                        QObject* receiver, const char* slot, QObject* parent, const char* name = 0 );

    KoDocumentEntry documentEntry() const { return m_documentEntry; }

    //virtual void plug(QWidget* widget, int index);

protected:
    void init();

protected slots:
    virtual void slotActivated();
    void slotActionActivated();

private:
    QValueList<KoDocumentEntry> m_lstEntries;
    KoDocumentEntry m_documentEntry;
};

#endif
