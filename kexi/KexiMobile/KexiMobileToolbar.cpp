/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "KexiMobileToolbar.h"
#include <QToolButton>
#include <QVBoxLayout>
#include <KIconLoader>
#include <QAction>
#include <kdebug.h>
#include <QPushButton>

KexiMobileToolbar::KexiMobileToolbar(QWidget* parent): QToolBar(parent)
{
    setOrientation(Qt::Vertical);
    KIconLoader *iconLoader = KIconLoader::global();

    m_openFileAction = new QAction(iconLoader->loadIconSet("document-open", KIconLoader::MainToolbar), "Open", this);
    m_gotoNavigatorAction = new QAction(iconLoader->loadIconSet("application-vnd.oasis.opendocument.database", KIconLoader::MainToolbar), "Project", this);

    addAction(m_openFileAction);
    addAction(m_gotoNavigatorAction);

    connect(m_gotoNavigatorAction, SIGNAL(triggered(bool)), this, SLOT(gotoNavigatorClicked()));
    connect(m_openFileAction, SIGNAL(triggered(bool)), this, SLOT(openFileClicked()));
}

KexiMobileToolbar::~KexiMobileToolbar()
{

}

void KexiMobileToolbar::gotoNavigatorClicked()
{
    qDebug() << "Goto Navigator";
    emit(pageNavigator());
}

void KexiMobileToolbar::openFileClicked()
{
    kDebug();
    emit(pageOpenFile());
}

#include "KexiMobileToolbar.moc"
