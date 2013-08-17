/* This file is part of the KDE libraries
   Copyright (C) 2007 Urs Wolfer <uwolfer @ kde.org>

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
#include <ktitlewidget.h>

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class KTitleWidgetTestWidget : public QWidget
{
public:
    KTitleWidgetTestWidget(QWidget *parent = 0)
     : QWidget(parent)
    {
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        KTitleWidget *titleWidget = new KTitleWidget(this);
        titleWidget->setText(QStringLiteral("Title"));
        titleWidget->setPixmap(QIcon::fromTheme(QStringLiteral("screen")).pixmap(22, 22), KTitleWidget::ImageLeft);

        mainLayout->addWidget(titleWidget);

        KTitleWidget *errorTitle = new KTitleWidget(this);
        errorTitle->setText(QStringLiteral("Title"));
        errorTitle->setComment(QStringLiteral("Error Comment"), KTitleWidget::ErrorMessage);

        mainLayout->addWidget(errorTitle);

        KTitleWidget *checkboxTitleWidget = new KTitleWidget(this);

        QWidget *checkBoxTitleMainWidget = new QWidget(this);
        QVBoxLayout *titleLayout = new QVBoxLayout(checkBoxTitleMainWidget);
        titleLayout->setMargin(6);

        QCheckBox *checkBox = new QCheckBox(QStringLiteral("Text Checkbox"), checkBoxTitleMainWidget);
        titleLayout->addWidget(checkBox);
        checkboxTitleWidget->setWidget(checkBoxTitleMainWidget);

        mainLayout->addWidget(checkboxTitleWidget);

        QLabel *otherLabel = new QLabel(QStringLiteral("Some text..."), this);

        mainLayout->addWidget(otherLabel);

        mainLayout->addStretch();
    }
};

int main(int argc, char **argv)
{
    QApplication::setApplicationName(QStringLiteral("ktitlewidgettest"));

    QApplication app(argc, argv);

    KTitleWidgetTestWidget *mainWidget = new KTitleWidgetTestWidget;
    mainWidget->show();

    return app.exec();
}
