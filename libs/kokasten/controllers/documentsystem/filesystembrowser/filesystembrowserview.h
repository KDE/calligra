/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2009 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#ifndef FILESYSTEMBROWSERVIEW_H
#define FILESYSTEMBROWSERVIEW_H

// Qt
#include <QtGui/QWidget>

class KDirOperator;
class KUrlNavigator;
class KUrl;
class KFileItem;
class KActionCollection;
class KToolBar;


namespace Kasten2
{

class FileSystemBrowserTool;


class FileSystemBrowserView : public QWidget
{
  Q_OBJECT

  public:
    explicit FileSystemBrowserView( FileSystemBrowserTool* tool, QWidget* parent = 0 );
    virtual ~FileSystemBrowserView();

  public:
    FileSystemBrowserTool* tool() const;

  private Q_SLOTS:
    void init();

    void setDirOperatorUrl( const KUrl& url );
    void setNavigatorUrl( const KUrl& url );
    void syncCurrentDocumentDirectory();
    void openFile( const KFileItem& fileItem );

  private:
    FileSystemBrowserTool* const mTool;

    KActionCollection* mActionCollection;
    KToolBar* mToolbar;
    KDirOperator* mDirOperator;
    KUrlNavigator* mUrlNavigator;
};


inline FileSystemBrowserTool* FileSystemBrowserView::tool() const { return mTool; }

}

#endif
