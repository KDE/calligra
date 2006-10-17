/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qheader.h>

#include <kiconloader.h>
#include <klocale.h>
#include <ktoolbarbutton.h>
#include <kdebug.h>
#include <kpopupmenu.h>

#include <widget/kexipropertyeditorview.h>
#include <widget/kexidatasourcecombobox.h>
#include <widget/kexifieldlistview.h>
#include <widget/kexifieldcombobox.h>
#include <widget/kexismalltoolbutton.h>
#include <kexidb/connection.h>
#include <kexiproject.h>

#include <koproperty/property.h>
#include <koproperty/utils.h>

QString mimeTypeToType(const QString& mimeType)
{
	if (mimeType=="kexi/table")
		return "table";
	else if (mimeType=="kexi/query")
		return "query";
//! @todo more types
	return mimeType;
}

QString typeToMimeType(const QString& type)
{
	if (type=="table")
		return "kexi/table";
	else if (type=="query")
		return "kexi/query";
//! @todo more types
	return type;
}

//----------------------------------------------

//! @internal
class KexiLookupColumnPage::Private
{
	public:
		Private()
		 : currentFieldUid(-1)
		 , insideClearRowSourceSelection(false)
		 , propertySetEnabled(true)
		{
		}
		~Private()
		{
		}

		bool hasPropertySet() const {
			return propertySet;
		}

		void setPropertySet(KoProperty::Set* aPropertySet) {
			propertySet = aPropertySet;
		}

		QVariant propertyValue(const QCString& propertyName) const {
			return propertySet ? propertySet->property(propertyName).value() : QVariant();
		}

		void changeProperty(const QCString &property, const QVariant &value)
		{
			if (!propertySetEnabled)
				return;
			propertySet->changeProperty(property, value);
		}

		void updateInfoLabelForPropertySet(const QString& textToDisplayForNullSet) {
			KexiPropertyEditorView::updateInfoLabelForPropertySet(
				objectInfoLabel, propertySet, textToDisplayForNullSet);
		}

		KexiDataSourceComboBox *rowSourceCombo;
		KexiFieldComboBox *boundColumnCombo, *visibleColumnCombo;
		KexiObjectInfoLabel *objectInfoLabel;
//		KexiDataSourceComboBox* d->dataSourceCombo;
		QLabel *rowSourceLabel, *boundColumnLabel, *visibleColumnLabel;
		//, *d->noDataSourceAvailableLabel, *d->availableFieldsLabel,
		//	*d->mousePointerLabel, *d->availableFieldsDescriptionLabel;
		QToolButton *clearRowSourceButton, *gotoRowSourceButton, *clearBoundColumnButton,
			*clearVisibleColumnButton;
		//, *d->clearDSButton, *d->gotoButton, *d->addField;
//		QFrame *d->rowSourceSeparator;
//		QString d->noDataSourceAvailableSingleText, d->noDataSourceAvailableMultiText;
		//! Used only in assignPropertySet() to check whether we already have the set assigned
		int currentFieldUid;

		bool insideClearRowSourceSelection : 1;
		//! True is changeProperty() works. Used to block updating properties when within assignPropertySet().
		bool propertySetEnabled : 1;
//disable		KexiFieldListView* d->fieldListView;

	private:
		//! A property set that is displayed on the page. 
		//! The set is also updated after any change in this page's data.
		QGuardedPtr<KoProperty::Set> propertySet;
};

//----------------------------------------------

KexiLookupColumnPage::KexiLookupColumnPage(QWidget *parent)
 : QWidget(parent)
 , d(new Private())
{
	setName("KexiLookupColumnPage");

	QVBoxLayout *vlyr = new QVBoxLayout(this);
	d->objectInfoLabel = new KexiObjectInfoLabel(this, "KexiObjectInfoLabel");
	vlyr->addWidget(d->objectInfoLabel);

//todo	d->noDataSourceAvailableSingleText = i18n("No data source could be assigned for this widget.");
//todo	d->noDataSourceAvailableMultiText = i18n("No data source could be assigned for multiple widgets.");

//	vlyr->addSpacing(8);

	//-Row Source
	QWidget *contents = new QWidget(this);
	vlyr->addWidget(contents);
	QVBoxLayout *contentsVlyr = new QVBoxLayout(contents);

/*todo	d->noDataSourceAvailableLabel = new QLabel(d->noDataSourceAvailableSingleText, contents);
	d->noDataSourceAvailableLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	d->noDataSourceAvailableLabel->setMargin(2);
	d->noDataSourceAvailableLabel->setAlignment(Qt::WordBreak | Qt::AlignBottom | Qt::AlignLeft);
	contentsVlyr->addWidget(d->noDataSourceAvailableLabel);
*/
	QHBoxLayout *hlyr = new QHBoxLayout(contentsVlyr);
	d->rowSourceLabel = new QLabel(i18n("Row source:"), contents);
	d->rowSourceLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->rowSourceLabel->setMargin(2);
	d->rowSourceLabel->setMinimumHeight(IconSize(KIcon::Small)+4);
	d->rowSourceLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
	hlyr->addWidget(d->rowSourceLabel);

	d->gotoRowSourceButton = new KexiSmallToolButton(contents, QString::null, "goto", "gotoRowSourceButton");
	d->gotoRowSourceButton->setMinimumHeight(d->rowSourceLabel->minimumHeight());
	QToolTip::add(d->gotoRowSourceButton, i18n("Go to selected row source"));
	hlyr->addWidget(d->gotoRowSourceButton);
	connect(d->gotoRowSourceButton, SIGNAL(clicked()), this, SLOT(slotGotoSelectedRowSource()));

	d->clearRowSourceButton = new KexiSmallToolButton(contents, QString::null,
		"clear_left", "clearRowSourceButton");
	d->clearRowSourceButton->setMinimumHeight(d->rowSourceLabel->minimumHeight());
	QToolTip::add(d->clearRowSourceButton, i18n("Clear row source"));
	hlyr->addWidget(d->clearRowSourceButton);
	connect(d->clearRowSourceButton, SIGNAL(clicked()), this, SLOT(clearRowSourceSelection()));

	d->rowSourceCombo = new KexiDataSourceComboBox(contents, "rowSourceCombo");
	d->rowSourceLabel->setBuddy(d->rowSourceCombo);
	contentsVlyr->addWidget(d->rowSourceCombo);

/*	d->rowSourceSeparator = new QFrame(contents);
	d->rowSourceSeparator->setFrameShape(QFrame::HLine);
z		d->rowSourceSeparator->setFrameShadow(QFrame::Sunken);
	contentsVlyr->addWidget(d->rowSourceSeparator);*/

	contentsVlyr->addSpacing(8);

	//- Bound Column
	hlyr = new QHBoxLayout(contentsVlyr);
	d->boundColumnLabel = new QLabel(i18n("Bound column:"), contents);
	d->boundColumnLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->boundColumnLabel->setMargin(2);
	d->boundColumnLabel->setMinimumHeight(IconSize(KIcon::Small)+4);
	d->boundColumnLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
	hlyr->addWidget(d->boundColumnLabel);

	d->clearBoundColumnButton = new KexiSmallToolButton(contents, QString::null,
		"clear_left", "clearBoundColumnButton");
	d->clearBoundColumnButton->setMinimumHeight(d->boundColumnLabel->minimumHeight());
	QToolTip::add(d->clearBoundColumnButton, i18n("Clear bound column"));
	hlyr->addWidget(d->clearBoundColumnButton);
	connect(d->clearBoundColumnButton, SIGNAL(clicked()), this, SLOT(clearBoundColumnSelection()));

	d->boundColumnCombo = new KexiFieldComboBox(contents, "boundColumnCombo");
	d->boundColumnLabel->setBuddy(d->boundColumnCombo);
	contentsVlyr->addWidget(d->boundColumnCombo);

	contentsVlyr->addSpacing(8);

	//- Visible Column
	hlyr = new QHBoxLayout(contentsVlyr);
	d->visibleColumnLabel = new QLabel(i18n("Visible column:"), contents);
	d->visibleColumnLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->visibleColumnLabel->setMargin(2);
	d->visibleColumnLabel->setMinimumHeight(IconSize(KIcon::Small)+4);
	d->visibleColumnLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
	hlyr->addWidget(d->visibleColumnLabel);

	d->clearVisibleColumnButton = new KexiSmallToolButton(contents, QString::null,
		"clear_left", "clearVisibleColumnButton");
	d->clearVisibleColumnButton->setMinimumHeight(d->visibleColumnLabel->minimumHeight());
	QToolTip::add(d->clearVisibleColumnButton, i18n("Clear visible column"));
	hlyr->addWidget(d->clearVisibleColumnButton);
	connect(d->clearVisibleColumnButton, SIGNAL(clicked()), this, SLOT(clearVisibleColumnSelection()));

	d->visibleColumnCombo = new KexiFieldComboBox(contents, "visibleColumnCombo");
	d->visibleColumnLabel->setBuddy(d->visibleColumnCombo);
	contentsVlyr->addWidget(d->visibleColumnCombo);

	
#if 0

#ifdef KEXI_NO_AUTOFIELD_WIDGET
	d->availableFieldsLabel = 0;
	d->addField = 0;
//	d->fieldListView = 0;
	vlyr->addStretch();
#else
	vlyr->addSpacing(fontMetrics().height());
/*	QFrame *separator = new QFrame(this);
	separator->setFrameShape(QFrame::HLine);
	separator->setFrameShadow(QFrame::Sunken);
	vlyr->addWidget(separator);*/
/*
	KPopupTitle *title = new KPopupTitle(this);
	title->setTitle(i18n("Inserting fields"));
	vlyr->addWidget(title);
	vlyr->addSpacing(4);*/


	//2. Inserting fields
	container = new KoProperty::GroupContainer(i18n("Inserting Fields"), this);
	vlyr->addWidget(container, 1);

	//helper info
//! @todo allow to hide such helpers by adding global option
	contents = new QWidget(container);
	container->setContents(contents);
	contentsVlyr = new QVBoxLayout(contents);
	hlyr = new QHBoxLayout(contentsVlyr);
	d->mousePointerLabel = new QLabel(contents);
	hlyr->addWidget(d->mousePointerLabel);
	d->mousePointerLabel->setPixmap( SmallIcon("mouse_pointer") );
	d->mousePointerLabel->setFixedWidth( d->mousePointerLabel->pixmap() ? d->mousePointerLabel->pixmap()->width() : 0);
	d->availableFieldsDescriptionLabel = new QLabel(
		i18n("Select fields from the list below and drag them onto a form or click the \"Insert\" button"), contents);
	d->availableFieldsDescriptionLabel->setAlignment( Qt::AlignAuto | Qt::WordBreak );
	hlyr->addWidget(d->availableFieldsDescriptionLabel);

	//Available Fields
	contentsVlyr->addSpacing(4);
	hlyr = new QHBoxLayout(contentsVlyr);
	d->availableFieldsLabel = new QLabel(i18n("Available fields:"), contents);
	d->availableFieldsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	d->availableFieldsLabel->setMargin(2);
	d->availableFieldsLabel->setMinimumHeight(IconSize(KIcon::Small));
	hlyr->addWidget(d->availableFieldsLabel);

	d->addField = new QToolButton(contents, "addFieldButton");
	d->addField->setFocusPolicy(StrongFocus);
	d->addField->setUsesTextLabel(true);
	d->addField->setTextPosition(QToolButton::Right);
	d->addField->setTextLabel(i18n("Insert selected field into form", "Insert"));
	d->addField->setIconSet(SmallIconSet("add_field"));
	d->addField->setMinimumHeight(d->availableFieldsLabel->minimumHeight());
	d->addField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	d->addField->setAutoRaise(true);
	d->addField->setPaletteBackgroundColor(palette().active().background());
	QToolTip::add(d->addField, i18n("Insert selected fields into form"));
	hlyr->addWidget(d->addField);
	connect(d->addField, SIGNAL(clicked()), this, SLOT(slotInsertSelectedFields()));

	d->fieldListView = new KexiFieldListView(contents, "fieldListView",
		KexiFieldListView::ShowDataTypes | KexiFieldListView::AllowMultiSelection );
	d->fieldListView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
	d->availableFieldsLabel->setBuddy(d->fieldListView);
	contentsVlyr->addWidget(d->fieldListView, 1);
	connect(d->fieldListView, SIGNAL(selectionChanged()), this, SLOT(slotFieldListViewSelectionChanged()));
	connect(d->fieldListView, SIGNAL(fieldDoubleClicked(const QString&, const QString&, const QString&)),
		this, SLOT(slotFieldDoubleClicked(const QString&, const QString&, const QString&)));
#endif

#endif//0

	vlyr->addStretch(1);

#if 0

	slotFieldListViewSelectionChanged();
#endif
	connect(d->rowSourceCombo, SIGNAL(textChanged(const QString &)), 
		this, SLOT(slotRowSourceTextChanged(const QString &)));
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
	d->rowSourceCombo->setProject(prj);
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

	d->setPropertySet( propertySet );

	d->updateInfoLabelForPropertySet( i18n("No field selected") );
	
//	if (d->propertySet && d->propertySet->contains("caption"))
//		objectCaption = (*d->propertySet)["caption"].value().toCString();
//	if (!objectCaption.isEmpty() && objectCaption == d->currentObjectCaption)
//		return; //the same object
//	d->currentObjectCaption = objectCaption;

//	setEnabled(d->propertySet);
//	d->objectInfoLabel->setObjectClassIcon(d->propertySet ? "lineedit" : "");
//! @todo "field" icon?
//	d->objectInfoLabel->setObjectClassName( 
//		d->propertySet ? i18n("Table field") : "");
//	d->objectInfoLabel->setObjectName(d->propertySet ? objectCaption : i18n("No field selected"));

	const bool hasRowSource = d->hasPropertySet() && !d->propertyValue("rowSourceType").isNull()
		&& !d->propertyValue("rowSource").isNull();

	QString rowSource, rowSourceType;
	if (hasRowSource) {
		rowSourceType = typeToMimeType( d->propertyValue("rowSourceType").toString() );
		rowSource = d->propertyValue("rowSource").toString();
	}
//		d->noDataSourceAvailableLabel->hide();
	d->rowSourceCombo->setDataSource( rowSourceType, rowSource );
	d->rowSourceLabel->setEnabled( d->hasPropertySet() );
	d->rowSourceCombo->setEnabled( d->hasPropertySet() );
	if (!d->hasPropertySet())
		d->clearRowSourceButton->setEnabled( false );
//	d->clearBoundColumnButton->setEnabled(!d->boundColumnCombo->currentText().isEmpty());
//	d->clearVisibleColumnButton->setEnabled(!d->visibleColumnCombo->currentText().isEmpty());

	int boundColumn = -1, visibleColumn = -1;
	if (d->rowSourceCombo->isSelectionValid()) {
		boundColumn = d->propertyValue("boundColumn").toInt();
		visibleColumn = d->propertyValue("visibleColumn").toInt();
	}
	d->boundColumnCombo->setFieldOrExpression(boundColumn);
	d->visibleColumnCombo->setFieldOrExpression(visibleColumn);
	updateBoundColumnWidgetsAvailability();
/*
			d->widgetDSLabel->show();
			d->clearWidgetDSButton->show();
			d->sourceFieldCombo->show();
			d->dataSourceSeparator->hide();*/
			//updateBoundColumnWidgetsAvailability();
	//}

//	if (isForm) {
	//	d->noDataSourceAvailableLabel->hide();
		//d->dataSourceSeparator->hide();
//	}
	//else 
	//if (!hasRowSource) {
//		d->dataSourceSeparator->show();
//		//make 'No data source could be assigned' label's height the same as the 'source field' combo+label
//		d->noDataSourceAvailableLabel->setMinimumHeight(d->widgetDSLabel->height()
//			+ d->sourceFieldCombo->height()-d->dataSourceSeparator->height());
//		d->boundColumnCombo->setCurrentText("");
	//}

/*	if (isForm || !hasDataSourceProperty) {
		//no source field can be set
		d->widgetDSLabel->hide();
		d->clearWidgetDSButton->hide();
		d->sourceFieldCombo->hide();
	}*/
	d->propertySetEnabled = true;
}

void KexiLookupColumnPage::clearBoundColumnSelection()
{
	d->boundColumnCombo->setCurrentText("");
	d->boundColumnCombo->setFieldOrExpression(QString::null);
	slotBoundColumnSelected();
	d->clearBoundColumnButton->setEnabled(false);
}

void KexiLookupColumnPage::slotBoundColumnSelected()
{
//	KexiDB::Field::Type dataType = KexiDB::Field::InvalidType;
//! @todo this should also work for expressions
/*disabled	KexiDB::Field *field = d->fieldListView->schema()->field( d->boundColumnCombo->fieldOrExpression() );
	if (field)
		dataType = field->type();
*/
	d->clearBoundColumnButton->setEnabled( !d->boundColumnCombo->fieldOrExpression().isEmpty() );
	if (!d->boundColumnCombo->fieldOrExpression().isEmpty()) {
		kdDebug() << endl;
	}

	// update property set
	if (d->hasPropertySet()) {
		d->changeProperty("boundColumn", d->boundColumnCombo->indexOfField());
	}
/*
	emit boundColumnChanged(
		d->boundColumnCombo->fieldOrExpression(),
		d->boundColumnCombo->fieldOrExpressionCaption(),
		dataType
	);*/
}

void KexiLookupColumnPage::clearVisibleColumnSelection()
{
	d->visibleColumnCombo->setCurrentText("");
	d->visibleColumnCombo->setFieldOrExpression(QString::null);
	slotVisibleColumnSelected();
	d->clearVisibleColumnButton->setEnabled(false);
}

void KexiLookupColumnPage::slotVisibleColumnSelected()
{
//	KexiDB::Field::Type dataType = KexiDB::Field::InvalidType;
//! @todo this should also work for expressions
	d->clearVisibleColumnButton->setEnabled( !d->visibleColumnCombo->fieldOrExpression().isEmpty() );

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
	QString mime = d->rowSourceCombo->selectedMimeType();
	bool rowSourceFound = false;
	QString name = d->rowSourceCombo->selectedName();
	if ((mime=="kexi/table" || mime=="kexi/query") && d->rowSourceCombo->isSelectionValid()) {
		KexiDB::TableOrQuerySchema *tableOrQuery = new KexiDB::TableOrQuerySchema(
			d->rowSourceCombo->project()->dbConnection(), name.latin1(), mime=="kexi/table");
		if (tableOrQuery->table() || tableOrQuery->query()) {
//disabled			d->fieldListView->setSchema( tableOrQuery );
/*tmp*/			delete tableOrQuery;
			rowSourceFound = true;
			d->boundColumnCombo->setTableOrQuery(name, mime=="kexi/table");
			d->visibleColumnCombo->setTableOrQuery(name, mime=="kexi/table");
		}
		else {
			delete tableOrQuery;
		}
	}
	if (!rowSourceFound) {
		d->boundColumnCombo->setTableOrQuery("", true);
		d->visibleColumnCombo->setTableOrQuery("", true);
	}
	clearBoundColumnSelection();
	clearVisibleColumnSelection();
	d->clearRowSourceButton->setEnabled(rowSourceFound);
	d->gotoRowSourceButton->setEnabled(rowSourceFound);
/* disabled
	if (dataSourceFound) {
		slotFieldListViewSelectionChanged();
	} else {
		d->addField->setEnabled(false);
	}*/
	updateBoundColumnWidgetsAvailability();

	//update property set
	if (d->hasPropertySet()) {
		d->changeProperty("rowSourceType", mimeTypeToType(mime));
		d->changeProperty("rowSource", name);
	}

//disabled	emit formDataSourceChanged(mime, name);
//! @todo update d->propertySet ^^
}

void KexiLookupColumnPage::slotRowSourceTextChanged(const QString & string)
{
	Q_UNUSED(string);
	const bool enable = d->rowSourceCombo->isSelectionValid();
	if (enable) {
		updateBoundColumnWidgetsAvailability();
	}
	else {
		clearRowSourceSelection( d->rowSourceCombo->selectedName().isEmpty()/*alsoClearComboBox*/ );
	}
/*#ifndef KEXI_NO_AUTOFIELD_WIDGET
	d->fieldListView->setEnabled(enable);
//	d->addField->setEnabled(enable);
	d->availableFieldsLabel->setEnabled(enable);
#endif*/
}

void KexiLookupColumnPage::clearRowSourceSelection(bool alsoClearComboBox)
{
	if (d->insideClearRowSourceSelection)
		return;
	d->insideClearRowSourceSelection = true;
	if (alsoClearComboBox && !d->rowSourceCombo->selectedName().isEmpty())
		d->rowSourceCombo->setDataSource("", "");
//	if (!d->dataSourceCombo->currentText().isEmpty()) {
//		d->dataSourceCombo->setCurrentText("");
//		emit d->dataSourceCombo->dataSourceSelected();
//	}
	d->clearRowSourceButton->setEnabled(false);
	d->gotoRowSourceButton->setEnabled(false);
//disabled	d->addField->setEnabled(false);
//disabled	d->fieldListView->clear();
	d->insideClearRowSourceSelection = false;
}

void KexiLookupColumnPage::slotGotoSelectedRowSource()
{
	QString mime = d->rowSourceCombo->selectedMimeType();
	if (mime=="kexi/table" || mime=="kexi/query") {
		if (d->rowSourceCombo->isSelectionValid())
			emit jumpToObjectRequested(mime.latin1(), d->rowSourceCombo->selectedName().latin1());
	}
}

void KexiLookupColumnPage::updateBoundColumnWidgetsAvailability()
{
	const bool hasRowSource = d->rowSourceCombo->isSelectionValid();
	d->boundColumnCombo->setEnabled( hasRowSource );
	d->boundColumnLabel->setEnabled( hasRowSource );
	d->clearBoundColumnButton->setEnabled( hasRowSource && !d->boundColumnCombo->fieldOrExpression().isEmpty() );
	d->visibleColumnCombo->setEnabled( hasRowSource );
	d->visibleColumnLabel->setEnabled( hasRowSource );
	d->clearVisibleColumnButton->setEnabled( hasRowSource && !d->visibleColumnCombo->fieldOrExpression().isEmpty() );
/*disabled	d->fieldListView->setEnabled( hasDataSource );
	d->availableFieldsLabel->setEnabled( hasDataSource );
	d->mousePointerLabel->setEnabled( hasDataSource );
	d->availableFieldsDescriptionLabel->setEnabled( hasDataSource );
*/
}



//---------------------
#if 0

void KexiDataSourcePage::slotInsertSelectedFields()
{
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	QStringList selectedFieldNames(d->fieldListView->selectedFieldNames());
	if (selectedFieldNames.isEmpty())
		return;

	emit insertAutoFields(d->fieldListView->schema()->table() ? "kexi/table" : "kexi/query",
		d->fieldListView->schema()->name(), selectedFieldNames);
#endif
}

void KexiDataSourcePage::slotFieldDoubleClicked(const QString& sourceMimeType, const QString& sourceName,
	const QString& fieldName)
{
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	QStringList selectedFields;
	selectedFields.append(fieldName);
	emit insertAutoFields(sourceMimeType, sourceName, selectedFields);
#endif
}

void KexiDataSourcePage::setDataSource(const QString& mimeType, const QString& name)
{
	d->dataSourceCombo->setDataSource(mimeType, name);
}

void KexiDataSourcePage::slotFieldListViewSelectionChanged()
{
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	//update "add field" button's state
	for (QListViewItemIterator it(d->fieldListView); it.current(); ++it) {
		if (it.current()->isSelected()) {
			d->addField->setEnabled(true);
			return;
		}
	}
	d->addField->setEnabled(false);
#endif
}

#endif

#include "kexilookupcolumnpage.moc"
