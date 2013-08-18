/* This file is part of the KDE libraries

    Copyright (c) 2012 Albert Astals Cid <aacid@kde.org>

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

#include <QtTest/QtTest>
#include <qtestevent.h>

#include <QDialogButtonBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>

#include <kconfigdialog.h>
#include <kconfigskeleton.h>
#include <kcolorcombo.h>

class ComboBoxPage : public QWidget
{
public:
    ComboBoxPage()
    {
        colorCombo = new KColorCombo(this);
        colorCombo->setObjectName("kcfg_Color");
        colorCombo->setColor(Qt::red);

        enumCombo = new QComboBox(this);
        enumCombo->setObjectName("kcfg_Enum");
        enumCombo->addItems(QStringList() << "A" << "B" << "C");

        textCombo = new QComboBox(this);
        textCombo->setObjectName("kcfg_Text");
        textCombo->setEditable(true);
        textCombo->addItems(QStringList() << "A" << "B" << "C");

        numInput = new QSpinBox(this);
        numInput->setValue(1);
        numInput->setObjectName("kcfg_IntNumInput");
    }

    KColorCombo *colorCombo;
    QComboBox *enumCombo;
    QComboBox *textCombo;
    QSpinBox *numInput;
};

class ComboSettings : public KConfigSkeleton
{
public:
    ComboSettings()
    {
        colorItem = new ItemColor( currentGroup(), QLatin1String( "Color" ), color, Qt::white );
        addItem( colorItem, QLatin1String( "Color" ) );

        QList<ItemEnum::Choice2> textValues;
        {
            ItemEnum::Choice2 choice;
            choice.name = QLatin1String("A");
            textValues.append( choice );
        }
        {
            ItemEnum::Choice2 choice;
            choice.name = QLatin1String("B");
            textValues.append( choice );
        }
        {
            ItemEnum::Choice2 choice;
            choice.name = QLatin1String("C");
            textValues.append( choice );
        }
        enumItem = new ItemEnum( currentGroup(), QLatin1String( "Enum" ), enumIndex, textValues, 1 );
        addItem( enumItem, QLatin1String( "Enum" ) );

        stringItem = new ItemString( currentGroup(), QLatin1String( "Text" ), string, QLatin1String( "hh:mm" ) );
        addItem( stringItem, QLatin1String( "Text" ) );

        intValueItem = new ItemInt( currentGroup(), QLatin1String( "IntNumInput" ), intValue, 42 );
        addItem( intValueItem, QLatin1String( "IntNumInput" ) );
    }

    ItemColor *colorItem;
    QColor color;

    ItemEnum *enumItem;
    int enumIndex;

    ItemString *stringItem;
    QString string;

    ItemInt *intValueItem;
    int intValue;
};

class KConfigDialog_UnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        QStandardPaths::enableTestMode(true);
        // Leftover configuration breaks combosTest
        const QString configFile = QStandardPaths::locate(QStandardPaths::ConfigLocation, "kconfigdialog_unittestrc");
        if (!configFile.isEmpty())
            QFile::remove(configFile);
    }

    void test()
    {
        ComboSettings *skeleton = new ComboSettings();
        KConfigDialog *dialog = new KConfigDialog(0, "settings", skeleton);
        ComboBoxPage *page = new ComboBoxPage();

        QCOMPARE(page->colorCombo->color(), QColor(Qt::red));
        QCOMPARE(page->enumCombo->currentIndex(), 0);
        QCOMPARE(page->textCombo->currentText(), QString("A"));
        QCOMPARE(page->numInput->value(), 1);

        dialog->addPage(page, "General");

        QCOMPARE(page->colorCombo->color(), QColor(Qt::white));
        QCOMPARE(page->enumCombo->currentIndex(), 1);
        QCOMPARE(page->textCombo->currentText(), QLatin1String( "hh:mm" ));
        QCOMPARE(page->numInput->value(), 42);

        page->colorCombo->setColor(Qt::blue);
        page->enumCombo->setCurrentIndex(2);
        page->textCombo->setCurrentIndex(2);
        page->numInput->setValue(2);

        QDialogButtonBox *buttonBox = dialog->findChild<QDialogButtonBox*>();
        QVERIFY(buttonBox!=0);
        buttonBox->button(QDialogButtonBox::Apply)->click();
        QCOMPARE(skeleton->colorItem->property().value<QColor>(), QColor(Qt::blue));
        QCOMPARE(skeleton->enumItem->property().toInt(), 2);
        QCOMPARE(skeleton->stringItem->property().toString(), QLatin1String("C"));
        QCOMPARE(skeleton->intValueItem->property().toInt(), 2);

        delete dialog;
        delete skeleton;
    }

};

QTEST_MAIN(KConfigDialog_UnitTest)

#include "kconfigdialog_unittest.moc"
