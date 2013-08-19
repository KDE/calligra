/* This file is part of the KDE libraries
    Copyright 2013 Benjamin Port <benjamin.port@kde.org>

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
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QToolBar>
#include <QPushButton>
#include <QString>

#include <ktoolbarlabelaction.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT
private Q_SLOTS:
    void updateLabel1()
    {
        label1->setText(labelText->text());
    }

public:
    MainWindow()
    {
        QWidget* mainWidget = new QWidget( this );
        setCentralWidget( mainWidget );

        QVBoxLayout* layout = new QVBoxLayout( mainWidget );
        layout->setMargin(0);
        layout->setSpacing(0);
        QToolBar* toolBar = addToolBar(QStringLiteral("Toolbar"));

        labelText = new QLineEdit();
        QPushButton* validateButton = new QPushButton(QStringLiteral("Update label"));

        layout->addWidget(labelText);
        layout->addWidget(validateButton);
        connect(validateButton, SIGNAL(clicked()), this,SLOT(updateLabel1()));

        QWidgetAction* lineEditAction2 = new QWidgetAction( toolBar );
        QLineEdit* lineEdit2 = new QLineEdit;
        lineEditAction2->setDefaultWidget(lineEdit2);
        label1 = new KToolBarLabelAction(QStringLiteral("&Label 1"), toolBar);
        label2 = new KToolBarLabelAction(lineEditAction2, QStringLiteral("&Second label"), toolBar);
        QWidgetAction* lineEditAction = new QWidgetAction( toolBar );
        QLineEdit* lineEdit = new QLineEdit;
        lineEditAction->setDefaultWidget(lineEdit);
        // set buddy for label1
        label1->setBuddy( lineEditAction );
        toolBar->addAction(label1);
        toolBar->addAction(lineEditAction);
        toolBar->addAction(label2);
        toolBar->addAction(lineEditAction2);
    }

    QLineEdit* labelText;
    KToolBarLabelAction* label1;
    KToolBarLabelAction* label2;
};

int main( int argc, char **argv )
{
    QApplication::setApplicationName(QStringLiteral("Test KToolBarLabelAction"));
    QApplication app(argc, argv);

    MainWindow* window = new MainWindow;
    window->show();

    return app.exec();
}

#include "ktoolbarlabelactiontest.moc"
