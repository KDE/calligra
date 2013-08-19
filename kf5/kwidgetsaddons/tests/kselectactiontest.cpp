/*
    Copyright 2006 Hamish Rodda <rodda@kde.org>
    Copyright 2006 Simon Hausmann <hausmann@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QToolBar>
#include <QApplication>
#include <QMenuBar>

#include <QDebug>

#include "kselectactiontest.h"

#include <kselectaction.h>

int main( int argc, char **argv )
{
    QApplication::setApplicationName(QStringLiteral("kselectactiontest"));
    QApplication app(argc, argv);

    SelectActionTest* test = new SelectActionTest;
    test->show();

    return app.exec();
}

SelectActionTest::SelectActionTest(QWidget *parent)
    : QMainWindow(parent)
    , m_comboSelect(new KSelectAction(QStringLiteral("Combo Selection"), this))
    , m_buttonSelect(new KSelectAction(QStringLiteral("Button Selection"), this))
{
    for (int i = 0; i < 7; ++i) {
      QAction* action = m_comboSelect->addAction(QStringLiteral("Combo Action %1").arg(i));
      connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered(bool)));
      action = m_buttonSelect->addAction(QStringLiteral("Action %1").arg(i));
      connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered(bool)));
    }

    m_comboSelect->setToolBarMode(KSelectAction::ComboBoxMode);
    m_comboSelect->setWhatsThis(QStringLiteral("What's this?"));
    connect(m_comboSelect, SIGNAL(triggered(QAction*)), SLOT(triggered(QAction*)));
    connect(m_comboSelect, SIGNAL(triggered(int)), SLOT(triggered(int)));
    connect(m_comboSelect, SIGNAL(triggered(QString)), SLOT(triggered(QString)));

    m_buttonSelect->setToolBarMode(KSelectAction::MenuMode);
    m_buttonSelect->setWhatsThis(QStringLiteral("What's this?"));
    connect(m_buttonSelect, SIGNAL(triggered(QAction*)), SLOT(triggered(QAction*)));
    connect(m_buttonSelect, SIGNAL(triggered(int)), SLOT(triggered(int)));
    connect(m_buttonSelect, SIGNAL(triggered(QString)), SLOT(triggered(QString)));

    menuBar()->addAction(m_comboSelect);
    menuBar()->addAction(m_buttonSelect);
    menuBar()->addAction(QStringLiteral("Add an action"), this, SLOT(addAction()));
    menuBar()->addAction(QStringLiteral("Remove an action"), this, SLOT(removeAction()));

    QToolBar* toolBar = addToolBar(QStringLiteral("Test"));
    toolBar->addAction(m_comboSelect);
    toolBar->addAction(m_buttonSelect);
}

void SelectActionTest::triggered(QAction* action)
{
  qDebug() << Q_FUNC_INFO << action;
}

void SelectActionTest::triggered(int index)
{
  qDebug() << Q_FUNC_INFO << index;
}

void SelectActionTest::triggered(const QString& text)
{
  qDebug() << Q_FUNC_INFO << '"' << text << '"';
}

void SelectActionTest::addAction()
{
    QAction* action = m_comboSelect->addAction(QStringLiteral("Combo Action %1").arg(m_comboSelect->actions().count()));
    connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered(bool)));
    action = m_buttonSelect->addAction(QStringLiteral("Action %1").arg(m_buttonSelect->actions().count()));
    connect(action, SIGNAL(triggered(bool)), SLOT(slotActionTriggered(bool)));
}

void SelectActionTest::removeAction()
{
    if (!m_comboSelect->actions().isEmpty())
        m_comboSelect->removeAction(m_comboSelect->actions().last());

    if (!m_buttonSelect->actions().isEmpty())
        m_buttonSelect->removeAction(m_buttonSelect->actions().last());
}

void SelectActionTest::slotActionTriggered(bool state)
{
    qDebug() << sender() << " state " << state;
}


