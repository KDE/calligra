/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2006-2014 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#include "KexiStandardContainerFormWidgets.h"
#include "container.h"
#include "form.h"
#include "formIO.h"
#include "objecttree.h"
#include "widgetlibrary.h"
#include "utils.h"
#include "commands.h"
#include <kexiutils/utils.h>

#include <KexiIcon.h>

#include <KLocalizedString>

#include <QStackedWidget>
#include <QPainter>
#include <QInputDialog>
#include <QLayout>

ContainerWidget::ContainerWidget(QWidget *parent)
        : QWidget(parent)
{
}

ContainerWidget::~ContainerWidget()
{
}

QSize ContainerWidget::sizeHint() const
{
    return QSize(30, 30); //default
}

void ContainerWidget::dragMoveEvent(QDragMoveEvent *e)
{
    QWidget::dragMoveEvent(e);
    emit handleDragMoveEvent(e);
}

void ContainerWidget::dropEvent(QDropEvent *e)
{
    QWidget::dropEvent(e);
    emit handleDropEvent(e);
}

////////////////////////

GroupBox::GroupBox(const QString & title, QWidget *parent)
        : QGroupBox(title, parent)
{
}

GroupBox::~GroupBox()
{
}

void GroupBox::dragMoveEvent(QDragMoveEvent *e)
{
    QGroupBox::dragMoveEvent(e);
    emit handleDragMoveEvent(e);
}

void GroupBox::dropEvent(QDropEvent *e)
{
    QGroupBox::dropEvent(e);
    emit handleDropEvent(e);
}

QSize GroupBox::sizeHint() const
{
    return title().isEmpty() ? QGroupBox::sizeHint() : QSize(fontMetrics().width(title()), fontMetrics().height()*3);
}

////////////////////////

KFDTabWidget::KFDTabWidget(KFormDesigner::Container *container, QWidget *parent)
        : KFormDesigner::TabWidget(parent), m_container(container)
{
}

KFDTabWidget::~KFDTabWidget()
{
}

QSize
KFDTabWidget::sizeHint() const
{
    QSize s(30, 30); // default min size
    for (int i = 0; i < count(); i++)
        s = s.expandedTo(KFormDesigner::getSizeFromChildren(widget(i)));

    return s + QSize(10/*margin*/, tabBar()->height() + 20/*margin*/);
}

void KFDTabWidget::dragMoveEvent(QDragMoveEvent *e)
{
    TabWidgetBase::dragMoveEvent(e);
    if (qobject_cast<ContainerWidget*>(currentWidget()))
        emit qobject_cast<ContainerWidget*>(currentWidget())->handleDragMoveEvent(e);
    emit handleDragMoveEvent(e);
}

void KFDTabWidget::dropEvent(QDropEvent *e)
{
    TabWidgetBase::dropEvent(e);
    if (qobject_cast<ContainerWidget*>(currentWidget()))
        emit qobject_cast<ContainerWidget*>(currentWidget())->handleDropEvent(e);
    emit handleDropEvent(e);
}

/// Various layout widgets /////////////////:

HBox::HBox(QWidget *parent)
        : QFrame(parent)
{
}

HBox::~HBox()
{
}

void
HBox::paintEvent(QPaintEvent *)
{
    if (!designMode())
        return;
    QPainter p(this);
    p.setPen(QPen(Qt::red, 2, Qt::DashLine));
    p.drawRect(1, 1, width() - 1, height() - 1);
}

VBox::VBox(QWidget *parent)
        : QFrame(parent)
{
}

VBox::~VBox()
{
}

void
VBox::paintEvent(QPaintEvent *)
{
    if (!designMode())
        return;
    QPainter p(this);
    p.setPen(QPen(Qt::blue, 2, Qt::DashLine));
    p.drawRect(1, 1, width() - 1, height() - 1);
}

Grid::Grid(QWidget *parent)
        : QFrame(parent)
{
}

Grid::~Grid()
{
}

void
Grid::paintEvent(QPaintEvent *)
{
    if (!designMode())
        return;
    QPainter p(this);
    p.setPen(QPen(Qt::darkGreen, 2, Qt::DashLine));
    p.drawRect(1, 1, width() - 1, height() - 1);
}

HFlow::HFlow(QWidget *parent)
        : QFrame(parent)
{
}

HFlow::~HFlow()
{
}

void
HFlow::paintEvent(QPaintEvent *)
{
    if (!designMode())
        return;
    QPainter p(this);
    p.setPen(QPen(Qt::magenta, 2, Qt::DashLine));
    p.drawRect(1, 1, width() - 1, height() - 1);
}

VFlow::VFlow(QWidget *parent)
        : QFrame(parent)
{
}

VFlow::~VFlow()
{
}

void
VFlow::paintEvent(QPaintEvent *)
{
    if (!designMode())
        return;
    QPainter p(this);
    p.setPen(QPen(Qt::cyan, 2, Qt::DashLine));
    p.drawRect(1, 1, width() - 1, height() - 1);
}

QSize
VFlow::sizeHint() const
{
    if (layout())
        return layout()->sizeHint();
    else
        return QSize(700, 50); // default
}

/////// Sub forms ////////////////////////:

#if 0
SubForm::SubForm(KFormDesigner::Form *parentForm, QWidget *parent)
        : QScrollArea(parent), m_parentForm(parentForm), m_form(0), m_widget(0)
{
    setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
    viewport()->setPaletteBackgroundColor(colorGroup().mid());
}

SubForm::~SubForm()
{
}

void
SubForm::setFormName(const QString &name)
{
    if (name.isEmpty())
        return;

    QFileInfo info(name);
    if (!info.exists() || (m_parentForm && info.fileName() == m_parentForm->filename())) {
//! @todo ???
        return; // we check if this form is valid
    }

    // we create the container widget
    delete m_widget;
    m_widget = new QWidget(viewport());
    m_widget->setObjectName("subform_widget");
    addChild(m_widget);
    m_form = new KFormDesigner::Form(m_parentForm);
    m_form->setObjectName(this->objectName());
    m_form->createToplevel(m_widget);

    // and load the sub form
    KFormDesigner::FormIO::loadFormFromFile(m_form, m_widget, name);
    m_form->setMode(KFormDesigner::Form::DesignMode);

    m_formName = name;
}
#endif //0

/////   Internal actions

AddTabAction::AddTabAction(KFormDesigner::Container *container,
                           TabWidgetBase *receiver, QObject *parent)
    : QAction(koIcon("tab-new"), xi18nc("Add page to tab widget", "Add Page"),
              parent)
    , m_container(container)
    , m_receiver(receiver)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
}

void AddTabAction::slotTriggered()
{
    if (!qobject_cast<QTabWidget*>(m_receiver))
        return;
    KFormDesigner::Command *command = new KFormDesigner::InsertPageCommand(m_container, m_receiver);
    if (m_receiver->count() == 0) {
        command->execute();
        delete command;
    }
    else {
        m_container->form()->addCommand(command);
    }
}

RemoveTabAction::RemoveTabAction(KFormDesigner::Container *container,
                                 TabWidgetBase *receiver, QObject *parent)
    : QAction(koIcon("tab-close-other"), xi18nc("Remove tab widget's page", "Remove Page"),
              parent)
    , m_container(container)
    , m_receiver(receiver)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
    if (m_receiver->count() <= 1) {
        setEnabled(false);
    }
}

void RemoveTabAction::slotTriggered()
{
    if (!qobject_cast<QTabWidget*>(m_receiver) || m_receiver->count() == 0)
        return;
    KFormDesigner::Command *com = new KFormDesigner::RemovePageCommand(m_container, m_receiver);
    m_container->form()->addCommand(com);
}

RenameTabAction::RenameTabAction(KFormDesigner::Container *container,
                                 TabWidgetBase *receiver, QObject *parent)
    : QAction(koIcon("edit-rename"), xi18nc("Rename tab widget's page", "Rename Page..."),
              parent)
    , m_container(container)
    , m_receiver(receiver)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
}

void RenameTabAction::slotTriggered()
{
    if (!qobject_cast<QTabWidget*>(m_receiver))
        return;
    QWidget *w = m_receiver->currentWidget();
    bool ok;
    QString name = QInputDialog::getText(w->topLevelWidget(), xi18nc("@window:title", "New Page Title"),
                                         xi18n("Enter a new title for the current page:"),
                                         QLineEdit::Normal,
                                         m_receiver->tabText(m_receiver->indexOf(w)), &ok);
    if (ok)
        m_receiver->setTabText(m_receiver->indexOf(w), name);
}

AddStackPageAction::AddStackPageAction(KFormDesigner::Container *container,
                                       QWidget *receiver, QObject *parent)
    : QAction(koIcon("tab-new"), xi18nc("Add page to a stacked widget", "Add Page..."),
              parent)
    , m_container(container)
    , m_receiver(receiver)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
}

void AddStackPageAction::slotTriggered()
{
    if (   !KexiUtils::objectIsA(m_receiver, "QStackedWidget")
        && /* compat */ !KexiUtils::objectIsA(m_receiver, "QWidgetStack"))
    {
        return;
    }
    KFormDesigner::Command *command = new KFormDesigner::InsertPageCommand(m_container, m_receiver);
    if (!qobject_cast<QStackedWidget*>(m_receiver)->currentWidget()) {
        command->execute();
        delete command;
    }
    else {
        m_container->form()->addCommand(command);
    }
}

RemoveStackPageAction::RemoveStackPageAction(KFormDesigner::Container *container,
                                             QWidget *receiver, QObject *parent)
    : QAction(koIcon("tab-close-other"), xi18nc("Remove page from a stacked widget", "Remove Page"),
              parent)
    , m_container(container)
    , m_receiver(receiver)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
    if (qobject_cast<TabWidgetBase*>(m_receiver)->count() <= 1) {
        setEnabled(false);
    }
}

void RemoveStackPageAction::slotTriggered()
{
    if (   !KexiUtils::objectIsA(m_receiver, "QStackedWidget")
        && /* compat */ !KexiUtils::objectIsA(m_receiver, "QWidgetStack"))
    {
        return;
    }
    QStackedWidget *stack = qobject_cast<QStackedWidget*>(m_receiver);
    QWidget *page = stack->currentWidget();

    QWidgetList list;
    list.append(page);
    KFormDesigner::Command *com = new KFormDesigner::DeleteWidgetCommand(*m_container->form(), list);

    // raise prev/next widget
    int index = stack->indexOf(page);
    if (index > 0) {
        index--;
    }
    else if (index < (stack->count()-1)) {
        index++;
    }
    else {
        index = -1;
    }
    if (index >= 0) {
        stack->setCurrentIndex(index);
    }
    stack->removeWidget(page);
    m_container->form()->addCommand(com);
}

GoToStackPageAction::GoToStackPageAction(Direction direction,
                                         KFormDesigner::Container *container,
                                         QWidget *receiver, QObject *parent)
    : QAction(QIcon::fromTheme(direction == Previous ? koIconName("go-previous") : koIconName("go-next")),
              direction == Previous ? xi18nc("Go to Previous Page of a Stacked Widget", "Go to Previous Page")
                                    : xi18nc("Go to Next Page of a Stacked Widget", "Go to Next Page"),
              parent)
    , m_direction(direction)
    , m_container(container)
    , m_receiver(receiver)
{
    connect(this, SIGNAL(triggered()), this, SLOT(slotTriggered()));
    QStackedWidget *stack = qobject_cast<QStackedWidget*>(m_receiver);
    if (!stack || !stack->widget(nextWidgetIndex())) {
        setEnabled(false);
    }
}

int GoToStackPageAction::nextWidgetIndex() const
{
    QStackedWidget *stack = qobject_cast<QStackedWidget*>(m_receiver);
    if (!stack)
        return -1;
    return stack->currentIndex() + (m_direction == Previous ? -1 : 1);
}

void GoToStackPageAction::slotTriggered()
{
    QStackedWidget *stack = qobject_cast<QStackedWidget*>(m_receiver);
    if (stack && stack->widget(nextWidgetIndex())) {
        stack->setCurrentIndex(nextWidgetIndex());
    }
}
