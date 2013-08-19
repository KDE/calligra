/* This file is part of the KDE libraries
   Copyright (C) 2013 David Edmundson <davidedmundson@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include <QApplication>
#include <QVBoxLayout>

#include <kfontrequester.h>


class KFontRequesterTest : public QWidget
{
public:
    KFontRequesterTest(QWidget *parent = 0)
     : QWidget(parent)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        KFontRequester *test1 = new KFontRequester(this);
        mainLayout->addWidget(test1);

        KFontRequester *test2 = new KFontRequester(this);
        test2->setSampleText(QLatin1String("This is different sample text"));
        test2->setTitle(QLatin1String("A different title"));
        test2->setFont(QFont(QLatin1String("comic-sans"), 12, 1));

        mainLayout->addWidget(test2);
    }
};

int main(int argc, char **argv)
{
    QApplication::setApplicationName(QStringLiteral("kfontrequestertest"));

    QApplication app(argc, argv);

    KFontRequesterTest *mainWidget = new KFontRequesterTest;
    mainWidget->show();

    return app.exec();
}
