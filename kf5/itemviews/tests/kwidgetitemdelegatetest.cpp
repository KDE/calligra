/**
  * This file is part of the KDE project
  * Copyright (C) 2007-2008 Rafael Fernández López <ereslibre@kde.org>
  * Copyright (C) 2008 Kevin Ottens <ervin@kde.org>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Library General Public
  * License as published by the Free Software Foundation; either
  * version 2 of the License, or (at your option) any later version.
  *
  * This library is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Library General Public License for more details.
  *
  * You should have received a copy of the GNU Library General Public License
  * along with this library; see the file COPYING.LIB.  If not, write to
  * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  * Boston, MA 02110-1301, USA.
  */

#include <QApplication>
#include <QAbstractItemView>
#include <QListView>
#include <QLineEdit>
#include <QPushButton>
#include <QStringListModel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QRadialGradient>
#include <QPaintEvent>
#include <QtCore/QDebug>
#include <QToolButton>
#include <QMenu>

#include <kwidgetitemdelegate.h>

#define HARDCODED_BORDER 10
#define EQUALLY_SIZED_TOOLBUTTONS 1

#if EQUALLY_SIZED_TOOLBUTTONS
#include <QStyleOptionToolButton>
#endif

class TestWidget
    : public QWidget
{
    Q_OBJECT

public:
    TestWidget(QWidget *parent = 0)
        : QWidget(parent)
    {
        setMouseTracking(true);
        setAttribute(Qt::WA_Hover);
    }

    ~TestWidget()
    {
    }

    QSize sizeHint() const
    {
        return QSize(30, 30);
    }

protected:
    void paintEvent (QPaintEvent *event)
    {
        QPainter p(this);

        QRadialGradient radialGrad(QPointF(event->rect().width() / 2,
                                           event->rect().height() / 2),
                                           qMin(event->rect().width() / 2,
                                                event->rect().height() / 2));

        if (underMouse())
            radialGrad.setColorAt(0, Qt::green);
        else
            radialGrad.setColorAt(0, Qt::red);

        radialGrad.setColorAt(1, Qt::transparent);

        p.fillRect(event->rect(), radialGrad);

        p.setPen(Qt::black);
        p.drawLine(0, 15, 30, 15);
        p.drawLine(15, 0, 15, 30);

        p.end();
    }

    bool event(QEvent *event)
    {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            if (mouseEvent->pos().x() > 15 &&
                mouseEvent->pos().y() < 15)
                qDebug() << "First quarter";
            else if (mouseEvent->pos().x() < 15 &&
                     mouseEvent->pos().y() < 15)
                qDebug() << "Second quarter";
            else if (mouseEvent->pos().x() < 15 &&
                     mouseEvent->pos().y() > 15)
                qDebug() << "Third quarter";
            else
                qDebug() << "Forth quarter";
        }

        return QWidget::event(event);
    }
};

class MyDelegate
    : public KWidgetItemDelegate
{
    Q_OBJECT

public:
    MyDelegate(QAbstractItemView *itemView, QObject *parent = 0)
        : KWidgetItemDelegate(itemView, parent)
    {
        for (int i = 0; i < 100; i++)
        {
            installed[i] = i % 5;
        }
    }

    ~MyDelegate()
    {
    }

    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const
    {
        Q_UNUSED(option);
        Q_UNUSED(index);

        return sizeHint();
    }

    QSize sizeHint() const
    {
        return QSize(600, 60);
    }

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const
    {
        painter->save();

        itemView()->style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, 0);

        if (option.state & QStyle::State_Selected)
        {
            painter->setPen(option.palette.highlightedText().color());
        }

        painter->restore();
    }

    QList<QWidget*> createItemWidgets() const
    {
        QPushButton *button = new QPushButton();
        QToolButton *toolButton = new QToolButton();

        setBlockedEventTypes(button, QList<QEvent::Type>() << QEvent::MouseButtonPress
                             << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick);

        setBlockedEventTypes(toolButton, QList<QEvent::Type>() << QEvent::MouseButtonPress
                             << QEvent::MouseButtonRelease << QEvent::MouseButtonDblClick);

        connect(button, SIGNAL(clicked(bool)), this, SLOT(mySlot()));
        connect(toolButton, SIGNAL(triggered(QAction*)), this, SLOT(mySlot2()));
        connect(toolButton, SIGNAL(clicked(bool)), this, SLOT(mySlot3()));

        return QList<QWidget*>()
            << button
            << new TestWidget()
            << new QLineEdit()
            << toolButton;
    }

    void updateItemWidgets(const QList<QWidget*> widgets,
                           const QStyleOptionViewItem &option,
                           const QPersistentModelIndex &index) const
    {
        QPushButton *button = static_cast<QPushButton*>(widgets[0]);
        button->setText("Test me");
        button->setIcon(QIcon::fromTheme("kde"));
        button->resize(button->sizeHint());
        button->move(HARDCODED_BORDER, sizeHint().height() / 2 - button->height() / 2);

        TestWidget *testWidget = static_cast<TestWidget*>(widgets[1]);

        testWidget->resize(testWidget->sizeHint());
        testWidget->move(2 * HARDCODED_BORDER + button->sizeHint().width(),
                         sizeHint().height() / 2 - testWidget->size().height() / 2);

        // Hide the test widget when row can be divided by three
        testWidget->setVisible( (index.row() % 3) != 0 );

        QLineEdit *lineEdit = static_cast<QLineEdit*>(widgets[2]);

        //lineEdit->setClearButtonShown(true);
        lineEdit->resize(lineEdit->sizeHint());
        lineEdit->move(3 * HARDCODED_BORDER
                     + button->sizeHint().width()
                     + testWidget->sizeHint().width(),
                       sizeHint().height() / 2 - lineEdit->size().height() / 2);

        QToolButton *toolButton = static_cast<QToolButton*>(widgets[3]);

        if (!toolButton->menu())
        {
            QMenu *myMenu = new QMenu(toolButton);
            myMenu->addAction("Save");
            myMenu->addAction("Load");
            myMenu->addSeparator();
            myMenu->addAction("Close");
            toolButton->setMenu(myMenu);
        }

        toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        toolButton->setPopupMode(QToolButton::MenuButtonPopup);

        if (installed[index.row()])
        {
            toolButton->setText("Uninstall");
        }
        else
        {
            toolButton->setText("Install");
        }

        toolButton->resize(toolButton->sizeHint());
#if EQUALLY_SIZED_TOOLBUTTONS
        QStyleOptionToolButton toolButtonOpt;
        toolButtonOpt.initFrom(toolButton);
        toolButtonOpt.features = QStyleOptionToolButton::MenuButtonPopup;
        toolButtonOpt.arrowType = Qt::DownArrow;
        toolButtonOpt.toolButtonStyle = Qt::ToolButtonTextBesideIcon;

        toolButtonOpt.text = "Install";
        int widthInstall = QApplication::style()->sizeFromContents(QStyle::CT_ToolButton, &toolButtonOpt, QSize(option.fontMetrics.width("Install") + HARDCODED_BORDER * 3, option.fontMetrics.height()), toolButton).width();
        toolButtonOpt.text = "Uninstall";
        int widthUninstall = QApplication::style()->sizeFromContents(QStyle::CT_ToolButton, &toolButtonOpt, QSize(option.fontMetrics.width("Uninstall") + HARDCODED_BORDER * 3, option.fontMetrics.height()), toolButton).width();

        QSize size = toolButton->sizeHint();
        size.setWidth(qMax(widthInstall, widthUninstall));
        toolButton->resize(size);
#endif
        toolButton->move(option.rect.width() - toolButton->size().width() - HARDCODED_BORDER,
                         sizeHint().height() / 2 - toolButton->size().height() / 2);

        // Eat more space
        lineEdit->resize(option.rect.width()
                         - toolButton->width()
                         - testWidget->width()
                         - button->width()
                         - 5 * HARDCODED_BORDER,
                         lineEdit->height());
    }

private Q_SLOTS:
    void mySlot()
    {
        QMessageBox::information(0, "Button clicked", QString("The button in row %1 was clicked").arg(focusedIndex().row()));
    }

    void mySlot2()
    {
        QMessageBox::information(0, "Toolbutton menu item clicked", QString("A menu item was triggered in row %1").arg(focusedIndex().row()));
    }

    void mySlot3()
    {
        bool isInstalled = installed[focusedIndex().row()];
        installed[focusedIndex().row()] = !isInstalled;
        const_cast<QAbstractItemModel*>(focusedIndex().model())->setData(focusedIndex(), QString("makemodelbeupdated"));
    }

private:
    bool installed[100];
};

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QMainWindow *mainWindow = new QMainWindow();
    mainWindow->setMinimumSize(640, 480);
    QListView *listView = new QListView();
    QStringListModel *model = new QStringListModel();

    model->insertRows(0, 100);
    for (int i = 0; i < 100; ++i)
    {
        model->setData(model->index(i, 0), QString("Test " + QString::number(i)), Qt::DisplayRole);
    }

    listView->setModel(model);
    MyDelegate *myDelegate = new MyDelegate(listView);
    listView->setItemDelegate(myDelegate);
    listView->setVerticalScrollMode(QListView::ScrollPerPixel);

    mainWindow->setCentralWidget(listView);

    mainWindow->show();

    model->removeRows(0, 95);

    return app.exec();
}

#include "kwidgetitemdelegatetest.moc"
