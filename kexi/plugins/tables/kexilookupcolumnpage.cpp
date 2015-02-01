/* This file is part of the KDE project
   Copyright (C) 2006-2008 Jarosław Staniek <staniek@kde.org>

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

#include "kexilookupcolumnpage.h"

#include <QLabel>
#include <QToolTip>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>

#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>

#include <KoIcon.h>

#include <widget/properties/KexiPropertyPaneViewBase.h>
#include <widget/KexiDataSourceComboBox.h>
#include <widget/fields/KexiFieldListView.h>
#include <widget/fields/KexiFieldComboBox.h>
#include <kexiutils/SmallToolButton.h>
#include <db/connection.h>
#include <kexiproject.h>

#include <koproperty/Property.h>
#include <koproperty/Utils.h>

QString partClassToType(const QString& partClass)
{
    if (partClass == "org.kexi-project.table")
        return "table";
    else if (partClass == "org.kexi-project.query")
        return "query";
//! @todo more types
    return partClass;
}

QString typeToPartClass(const QString& type)
{
    return QString::fromLatin1("org.kexi-project.")+type;
//! @todo more types
}

//----------------------------------------------

//! @internal
class KexiLookupColumnPage::Private
{
public:
    explicit Private(KexiLookupColumnPage *that)
            : q(that)
            , currentFieldUid(-1)
            , insideClearRowSourceSelection(false)
            , propertySetEnabled(true) {
    }
    ~Private() {
    }

    bool hasPropertySet() const {
        return propertySet;
    }

    void setPropertySet(KoProperty::Set* aPropertySet) {
        propertySet = aPropertySet;
    }

    QVariant propertyValue(const QByteArray& propertyName) const {
        return propertySet ? propertySet->property(propertyName).value() : QVariant();
    }

    void changeProperty(const QByteArray& propertyName, const QVariant &value) {
        if (!propertySetEnabled)
            return;
        propertySet->changeProperty(propertyName, value);
    }

    void updateInfoLabelForPropertySet(const QString& textToDisplayForNullSet) {
        q->updateInfoLabelForPropertySet( propertySet, textToDisplayForNullSet);
    }

    KexiLookupColumnPage *q;
    KexiDataSourceComboBox *rowSourceCombo;
    KexiFieldComboBox *boundColumnCombo, *visibleColumnCombo;
    QLabel *rowSourceLabel, *boundColumnLabel, *visibleColumnLabel;
    QToolButton *gotoRowSourceButton;
    //! Used only in assignPropertySet() to check whether we already have the set assigned
    int currentFieldUid;

    bool insideClearRowSourceSelection;
    //! True is changeProperty() works. Used to block updating properties when within assignPropertySet().
    bool propertySetEnabled;

private:
    //! A property set that is displayed on the page.
    //! The set is also updated after any change in this page's data.
    QPointer<KoProperty::Set> propertySet;
};

//----------------------------------------------

KexiLookupColumnPage::KexiLookupColumnPage(QWidget *parent)
        : KexiPropertyPaneViewBase(parent)
        , d(new Private(this))
{
    setObjectName("KexiLookupColumnPage");

//! @todo d->noDataSourceAvailableSingleText = i18n("No data source could be assigned for this widget.");
//! @todo d->noDataSourceAvailableMultiText = i18n("No data source could be assigned for multiple widgets.");

    //-Record Source

    QHBoxLayout *hlyr = new QHBoxLayout();
    mainLayout()->addLayout(hlyr);
    d->rowSourceLabel = new QLabel(i18n("Record source:"));
    d->rowSourceLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->rowSourceLabel->setMinimumHeight(IconSize(KIconLoader::Small) + 4);
    d->rowSourceLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    hlyr->addWidget(d->rowSourceLabel);
    hlyr->addStretch();

    d->gotoRowSourceButton = new KexiSmallToolButton(koIcon("go-jump"), QString());
    d->gotoRowSourceButton->setObjectName("gotoRowSourceButton");
    d->gotoRowSourceButton->setMinimumHeight(d->rowSourceLabel->minimumHeight());
    d->gotoRowSourceButton->setToolTip(i18n("Go to selected record source"));
    hlyr->addWidget(d->gotoRowSourceButton);
    connect(d->gotoRowSourceButton, SIGNAL(clicked()), this, SLOT(slotGotoSelectedRowSource()));
    d->rowSourceCombo = new KexiDataSourceComboBox;
    d->rowSourceCombo->setObjectName("rowSourceCombo");
    d->rowSourceLabel->setBuddy(d->rowSourceCombo);
    connect(d->rowSourceCombo->lineEdit(), SIGNAL(clearButtonClicked()),
        this, SLOT(clearRowSourceSelection()));
    mainLayout()->addWidget(d->rowSourceCombo);

    addWidgetSpacer();

    //- Bound Column
    d->boundColumnLabel = new QLabel(i18n("Bound column:"));
    d->boundColumnLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->boundColumnLabel->setMinimumHeight(IconSize(KIconLoader::Small) + 4);
    d->boundColumnLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    mainLayout()->addWidget(d->boundColumnLabel);

    d->boundColumnCombo = new KexiFieldComboBox();
    d->boundColumnCombo->setObjectName("boundColumnCombo");
    d->boundColumnLabel->setBuddy(d->boundColumnCombo);
    connect(d->boundColumnCombo->lineEdit(), SIGNAL(clearButtonClicked()),
        this, SLOT(clearBoundColumnSelection()));
    mainLayout()->addWidget(d->boundColumnCombo);

    addWidgetSpacer();

    //- Visible Column
    d->visibleColumnLabel = new QLabel(i18n("Visible column:"));
    d->visibleColumnLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    d->visibleColumnLabel->setMinimumHeight(IconSize(KIconLoader::Small) + 4);
    d->visibleColumnLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    mainLayout()->addWidget(d->visibleColumnLabel);

    d->visibleColumnCombo = new KexiFieldComboBox;
    d->visibleColumnCombo->setObjectName("visibleColumnCombo");
    d->visibleColumnLabel->setBuddy(d->visibleColumnCombo);
    connect(d->visibleColumnCombo->lineEdit(), SIGNAL(clearButtonClicked()),
        this, SLOT(clearVisibleColumnSelection()));
    mainLayout()->addWidget(d->visibleColumnCombo);

    mainLayout()->addStretch(1);

    connect(d->rowSourceCombo, SIGNAL(textChanged(QString)),
            this, SLOT(slotRowSourceTextChanged(QString)));
    connect(d->rowSourceCombo, SIGNAL(dataSourceChanged()), this, SLOT(slotRowSourceChanged()));
    connect(d->boundColumnCombo, SIGNAL(selected()), this, SLOT(slotBoundColumnSelected()));
    connect(d->visibleColumnCombo, SIGNAL(selected()), this, SLOT(slotVisibleColumnSelected()));

    clearBoundColumnSelection();
    clearVisibleColumnSelection();
}

KexiLookupColumnPage::~KexiLookupColumnPage()
{
    delete d;
}

void KexiLookupColumnPage::setProject(KexiProject *prj)
{
    d->rowSourceCombo->setProject(prj,
                                  true/*showTables*/, true/*showQueries*/
                                 );
    d->boundColumnCombo->setProject(prj);
    d->visibleColumnCombo->setProject(prj);
}

void KexiLookupColumnPage::assignPropertySet(KoProperty::Set* propertySet)
{
    if (!d->hasPropertySet() && !propertySet)
        return;
    if (propertySet && d->currentFieldUid == (*propertySet)["uid"].value().toInt())
        return; //already assigned

    d->propertySetEnabled = false;
    d->setPropertySet(propertySet);
    d->updateInfoLabelForPropertySet(i18n("No field selected"));

    const bool hasRowSource = d->hasPropertySet() && !d->propertyValue("rowSourceType").isNull()
                              && !d->propertyValue("rowSource").isNull();

    QString rowSource, rowSourceType;
    if (hasRowSource) {
        rowSourceType = typeToPartClass(d->propertyValue("rowSourceType").toString());
        rowSource = d->propertyValue("rowSource").toString();
    }
    d->rowSourceCombo->setDataSource(rowSourceType, rowSource);
    d->rowSourceLabel->setEnabled(d->hasPropertySet());
    d->rowSourceCombo->setEnabled(d->hasPropertySet());

    int boundColumn = -1, visibleColumn = -1;
    if (d->rowSourceCombo->isSelectionValid()) {
        boundColumn = d->propertyValue("boundColumn").toInt();
        visibleColumn = d->propertyValue("visibleColumn").toInt();
    }
    d->boundColumnCombo->setFieldOrExpression(boundColumn);
    d->visibleColumnCombo->setFieldOrExpression(visibleColumn);
    updateBoundColumnWidgetsAvailability();
    d->propertySetEnabled = true;
}

void KexiLookupColumnPage::clearBoundColumnSelection()
{
    d->boundColumnCombo->setEditText("");
    d->boundColumnCombo->setFieldOrExpression(QString());
    slotBoundColumnSelected();
}

void KexiLookupColumnPage::slotBoundColumnSelected()
{
// KexiDB::Field::Type dataType = KexiDB::Field::InvalidType;
//! @todo this should also work for expressions
    /*disabled KexiDB::Field *field = d->fieldListView->schema()->field( d->boundColumnCombo->fieldOrExpression() );
      if (field)
        dataType = field->type();
    */
    if (!d->boundColumnCombo->fieldOrExpression().isEmpty()) {
        kDebug();
    }

    // update property set
    if (d->hasPropertySet()) {
        d->changeProperty("boundColumn", d->boundColumnCombo->indexOfField());
    }
}

void KexiLookupColumnPage::clearVisibleColumnSelection()
{
    d->visibleColumnCombo->setEditText("");
    d->visibleColumnCombo->setFieldOrExpression(QString());
    slotVisibleColumnSelected();
}

void KexiLookupColumnPage::slotVisibleColumnSelected()
{
// KexiDB::Field::Type dataType = KexiDB::Field::InvalidType;
//! @todo this should also work for expressions

    // update property set
    if (d->hasPropertySet()) {
//! @todo support expression in special "visibleExpression"
        d->changeProperty("visibleColumn", d->visibleColumnCombo->indexOfField());
    }
}

void KexiLookupColumnPage::slotRowSourceChanged()
{
    if (!d->rowSourceCombo->project())
        return;
    QString partClass(d->rowSourceCombo->selectedPartClass());
    bool rowSourceFound = false;
    QString name = d->rowSourceCombo->selectedName();
    if ((partClass == "org.kexi-project.table" || partClass == "org.kexi-project.query") && d->rowSourceCombo->isSelectionValid()) {
        KexiDB::TableOrQuerySchema *tableOrQuery = new KexiDB::TableOrQuerySchema(
            d->rowSourceCombo->project()->dbConnection(), name.toLatin1(), partClass == "org.kexi-project.table");
        if (tableOrQuery->table() || tableOrQuery->query()) {
//! @todo disabled   d->fieldListView->setSchema( tableOrQuery );
            /*tmp*/
            delete tableOrQuery;
            rowSourceFound = true;
            d->boundColumnCombo->setTableOrQuery(name, partClass == "org.kexi-project.table");
            d->visibleColumnCombo->setTableOrQuery(name, partClass == "org.kexi-project.table");
        } else {
            delete tableOrQuery;
        }
    }
    if (!rowSourceFound) {
        d->boundColumnCombo->setTableOrQuery("", true);
        d->visibleColumnCombo->setTableOrQuery("", true);
    }
    clearBoundColumnSelection();
    clearVisibleColumnSelection();
    d->gotoRowSourceButton->setEnabled(rowSourceFound);
    updateBoundColumnWidgetsAvailability();

    //update property set
    if (d->hasPropertySet()) {
        d->changeProperty("rowSourceType", partClassToType(partClass));
        d->changeProperty("rowSource", name);
    }
//! @todo update d->propertySet ^^
}

void KexiLookupColumnPage::slotRowSourceTextChanged(const QString & string)
{
    Q_UNUSED(string);
    const bool enable = d->rowSourceCombo->isSelectionValid();
    if (enable) {
        updateBoundColumnWidgetsAvailability();
    } else {
        clearRowSourceSelection(d->rowSourceCombo->selectedName().isEmpty()/*alsoClearComboBox*/);
    }
}

void KexiLookupColumnPage::clearRowSourceSelection(bool alsoClearComboBox)
{
    if (d->insideClearRowSourceSelection)
        return;
    d->insideClearRowSourceSelection = true;
    if (alsoClearComboBox) {
        d->rowSourceCombo->setDataSource("", "");
    }
    d->gotoRowSourceButton->setEnabled(false);
    d->insideClearRowSourceSelection = false;
}

void KexiLookupColumnPage::slotGotoSelectedRowSource()
{
    const QString partClass( d->rowSourceCombo->selectedPartClass() );
    if (partClass == "org.kexi-project.table" || partClass == "org.kexi-project.query") {
        if (d->rowSourceCombo->isSelectionValid())
            emit jumpToObjectRequested(partClass, d->rowSourceCombo->selectedName());
    }
}

void KexiLookupColumnPage::updateBoundColumnWidgetsAvailability()
{
    const bool hasRowSource = d->rowSourceCombo->isSelectionValid();
    d->boundColumnCombo->setEnabled(hasRowSource);
    d->boundColumnLabel->setEnabled(hasRowSource);
    d->visibleColumnCombo->setEnabled(hasRowSource);
    d->visibleColumnLabel->setEnabled(hasRowSource);
}

#include "kexilookupcolumnpage.moc"
