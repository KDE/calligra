#include "ktreewidgetsearchlinetest.h"

#include <QApplication>
#include <QDialogButtonBox>
#include <QTreeWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QHeaderView>
#include <QString>

#include <ktreewidgetsearchline.h>

KTreeWidgetSearchLineTest::KTreeWidgetSearchLineTest()
    : QDialog()
{
    // to test KWhatsThisManager too:
    setWhatsThis("This is a test dialog for KTreeWidgetSearchLineTest");
    tw = new QTreeWidget(this);
    tw->setColumnCount(4);
    tw->setHeaderLabels(QStringList() << "Item" << "Price" << "HIDDEN COLUMN" << "Source");
    tw->hideColumn(2);

    KTreeWidgetSearchLineWidget* searchWidget = new KTreeWidgetSearchLineWidget(this,tw);
    m_searchLine = searchWidget->searchLine();

    QTreeWidgetItem* red = new QTreeWidgetItem( tw, QStringList() << "Red");
    red->setWhatsThis( 0, "This item is red" );
    red->setWhatsThis( 1, "This item is pricy" );
    tw->expandItem(red);
    QTreeWidgetItem* blue = new QTreeWidgetItem( tw, QStringList() << "Blue");
    tw->expandItem(blue);
    QTreeWidgetItem* green = new QTreeWidgetItem( tw, QStringList() << "Green");
    tw->expandItem(green);
    QTreeWidgetItem* yellow = new QTreeWidgetItem( tw, QStringList() << "Yellow");
    tw->expandItem(yellow);

    create2ndLevel(red);
    create2ndLevel(blue);
    create2ndLevel(green);
    create2ndLevel(yellow);

    QVBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);
    QHBoxLayout* hbox = new QHBoxLayout();

    QPushButton* caseSensitive = new QPushButton("&Case Sensitive", this);
    hbox->addWidget(caseSensitive);
    caseSensitive->setCheckable(true);
    connect(caseSensitive, SIGNAL(toggled(bool)), SLOT(switchCaseSensitivity(bool)));

    QPushButton* keepParentsVisible = new QPushButton("Keep &Parents Visible", this);
    hbox->addWidget(keepParentsVisible);
    keepParentsVisible->setCheckable(true);
    keepParentsVisible->setChecked(true);
    connect(keepParentsVisible, SIGNAL(toggled(bool)), m_searchLine, SLOT(setKeepParentsVisible(bool)));

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    layout->addWidget(searchWidget);
    layout->addWidget(tw);
    layout->addLayout(hbox);
    layout->addWidget(buttonBox);

    m_searchLine->setFocus();

    resize(350, 600);
}

void KTreeWidgetSearchLineTest::create3rdLevel( QTreeWidgetItem * item )
{
    new QTreeWidgetItem( item, QStringList() << "Growing" << "$2.00" << "" << "Farmer" );
    new QTreeWidgetItem( item, QStringList() << "Ripe" << "$8.00" << "" << "Market" );
    new QTreeWidgetItem( item, QStringList() << "Decaying" << "$0.50" << "" << "Ground" );
    new QTreeWidgetItem( item, QStringList() << "Pickled"  << "$4.00" << "" << "Shop" );
}

void KTreeWidgetSearchLineTest::create2ndLevel( QTreeWidgetItem * item )
{
    QTreeWidgetItem* beans = new QTreeWidgetItem(item, QStringList() << "Beans");
    tw->expandItem(beans);
    create3rdLevel(beans);

    QTreeWidgetItem* grapes = new QTreeWidgetItem(item, QStringList() << "Grapes");
    tw->expandItem(grapes);
    create3rdLevel(grapes);

    QTreeWidgetItem* plums = new QTreeWidgetItem(item, QStringList() << "Plums");
    tw->expandItem(plums);
    create3rdLevel(plums);

    QTreeWidgetItem* bananas = new QTreeWidgetItem(item, QStringList() << "Bananas");
    tw->expandItem(bananas);
    create3rdLevel(bananas);
}

void KTreeWidgetSearchLineTest::switchCaseSensitivity(bool cs)
{
    m_searchLine->setCaseSensitivity(cs ? Qt::CaseSensitive : Qt::CaseInsensitive);
}

void KTreeWidgetSearchLineTest::showEvent( QShowEvent * event )
{
    QDialog::showEvent(event);

    for (int i = 0; i < tw->header()->count(); ++i)
        if (!tw->header()->isSectionHidden(i))
            tw->resizeColumnToContents(i);
}

int main( int argc, char **argv )
{
    QApplication::setApplicationName("KTreeWidgetSearchLineTest");
    QApplication app(argc, argv);
    KTreeWidgetSearchLineTest dialog;

    dialog.exec();

    return 0;
}

