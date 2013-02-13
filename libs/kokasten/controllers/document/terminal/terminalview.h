/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2010 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TERMINALVIEW_H
#define TERMINALVIEW_H

// Qt
#include <QtGui/QWidget>

class TerminalInterface;
namespace KParts
{
class ReadOnlyPart;
}
class KUrl;


namespace Kasten2
{

class TerminalTool;


class TerminalView : public QWidget
{
  Q_OBJECT

  public:
    explicit TerminalView( TerminalTool* tool, QWidget* parent = 0 );
    virtual ~TerminalView();

  public:
    TerminalTool* tool() const;

  private Q_SLOTS:
    void createTerminalPart();

    void onCurrentUrlChanged( const KUrl& currentUrl );

    void onTerminalPartDestroyed();

  private:
    TerminalTool* const mTool;

    KParts::ReadOnlyPart* mTerminalPart;
    TerminalInterface* mTerminalInterface;
};


inline TerminalTool* TerminalView::tool() const { return mTool; }

}

#endif
