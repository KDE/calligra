/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidatasourcepage.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <q3header.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3Frame>
#include <Q3CString>

#include <kiconloader.h>
#include <klocale.h>

#include <kdebug.h>
#include <kpopupmenu.h>

#include <widget/kexipropertyeditorview.h>
#include <widget/kexidatasourcecombobox.h>
#include <widget/kexifieldlistview.h>
#include <widget/kexifieldcombobox.h>
#include <widget/kexismalltoolbutton.h>
#include <kexidb/connection.h>
#include <kexiproject.h>

#include <formeditor/commands.h>

#include <koproperty/property.h>
#include <koproperty/utils.h>

KexiDataSourcePage::KexiDataSourcePage(QWidget *parent, const char *name)
 : QWidget(parent, name)
 , m_insideClearDataSourceSelection(false)
{
	Q3VBoxLayout *vlyr = new Q3VBoxLayout(this);
	m_objectInfoLabel = new KexiObjectInfoLabel(this, "KexiObjectInfoLabel");
	vlyr->addWidget(m_objectInfoLabel);

	m_noDataSourceAvailableSingleText = i18n("No data source could be assigned for this widget.");
	m_noDataSourceAvailableMultiText = i18n("No data source could be assigned for multiple widgets.");

	vlyr->addSpacing(8);

	//Section 1: Form's/Widget's Data Source
	KoProperty::GroupContainer *container = new KoProperty::GroupContainer(i18n("Data Source"), this);
	vlyr->addWidget(container);

	QWidget *contents = new QWidget(container);
	container->setContents(contents);
	Q3VBoxLayout *contentsVlyr = new Q3VBoxLayout(contents);

	m_noDataSourceAvailableLabel = new QLabel(m_noDataSourceAvailableSingleText, contents);
	m_noDataSourceAvailableLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	m_noDataSourceAvailableLabel->setMargin(2);
	m_noDataSourceAvailableLabel->setAlignment(Qt::TextWordWrap | Qt::AlignBottom | Qt::AlignLeft);
	contentsVlyr->addWidget(m_noDataSourceAvailableLabel);

	//Widget's Data Source
	//-Widget's Data Source
	Q3HBoxLayout *hlyr = new Q3HBoxLayout(contentsVlyr);
#if 0
//! @todo unhide this when expression work
//	m_widgetDSLabel = new QLabel(i18n("Table Field, Query Field or Expression", "Source field or expression:"), this);
#else
	m_widgetDSLabel = new QLabel(i18n("Table Field or Query Field", "Widget's data source:"), contents);
#endif
	m_widgetDSLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	m_widgetDSLabel->setMargin(2);
	m_widgetDSLabel->setMinimumHeight(IconSize(K3Icon::Small)+4);
	m_widgetDSLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
	hlyr->addWidget(m_widgetDSLabel);

	m_clearWidgetDSButton = new KexiSmallToolButton(contents, QString::null, "clear_left", "clearWidgetDSButton");
	m_clearWidgetDSButton->setMinimumHeight(m_widgetDSLabel->minimumHeight());
	m_clearWidgetDSButton->setToolTip( i18n("Clear widget's data source"));
	hlyr->addWidget(m_clearWidgetDSButton);
	connect(m_clearWidgetDSButton, SIGNAL(clicked()), this, SLOT(clearWidgetDataSourceSelection()));

	m_sourceFieldCombo = new KexiFieldComboBox(contents, "sourceFieldCombo");
	m_widgetDSLabel->setBuddy(m_sourceFieldCombo);
	contentsVlyr->addWidget(m_sourceFieldCombo);

/*	m_dataSourceSeparator = new Q3Frame(contents);
	m_dataSourceSeparator->setFrameShape(Q3Frame::HLine);
	m_dataSourceSeparator->setFrameShadow(Q3Frame::Sunken);
	contentsVlyr->addWidget(m_dataSourceSeparator);*/

	contentsVlyr->addSpacing(8);

	//- Form's Data Source
	hlyr = new Q3HBoxLayout(contentsVlyr);
	m_dataSourceLabel = new QLabel(i18n("Form's data source:"), contents);
	m_dataSourceLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	m_dataSourceLabel->setMargin(2);
	m_dataSourceLabel->setMinimumHeight(IconSize(K3Icon::Small)+4);
	m_dataSourceLabel->setAlignment(Qt::AlignLeft|Qt::AlignBottom);
	hlyr->addWidget(m_dataSourceLabel);

	m_gotoButton = new KexiSmallToolButton(contents, QString::null, "goto-page", "gotoButton");
	m_gotoButton->setMinimumHeight(m_dataSourceLabel->minimumHeight());
	m_gotoButton->setToolTip( i18n("Go to selected form's data source"));
	hlyr->addWidget(m_gotoButton);
	connect(m_gotoButton, SIGNAL(clicked()), this, SLOT(slotGotoSelected()));

	m_clearDSButton = new KexiSmallToolButton(contents, QString::null, "clear_left", "clearDSButton");
	m_clearDSButton->setMinimumHeight(m_dataSourceLabel->minimumHeight());
	m_clearDSButton->setToolTip( i18n("Clear form's data source"));
	hlyr->addWidget(m_clearDSButton);
	connect(m_clearDSButton, SIGNAL(clicked()), this, SLOT(clearDataSourceSelection()));

	m_dataSourceCombo = new KexiDataSourceComboBox(contents, "dataSourceCombo");
	m_dataSourceLabel->setBuddy(m_dataSourceCombo);
	contentsVlyr->addWidget(m_dataSourceCombo);

#ifdef KEXI_NO_AUTOFIELD_WIDGET
	m_availableFieldsLabel = 0;
	m_addField = 0;
//	m_fieldListView = 0;
	vlyr->addStretch();
#else
	vlyr->addSpacing(fontMetrics().height());
/*	Q3Frame *separator = new QFrame(this);
	separator->setFrameShape(Q3Frame::HLine);
	separator->setFrameShadow(Q3Frame::Sunken);
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
	contentsVlyr = new Q3VBoxLayout(contents);
	hlyr = new Q3HBoxLayout(contentsVlyr);
	m_mousePointerLabel = new QLabel(contents);
	hlyr->addWidget(m_mousePointerLabel);
	m_mousePointerLabel->setPixmap( SmallIcon("mouse_pointer") );
	m_mousePointerLabel->setFixedWidth( m_mousePointerLabel->pixmap() ? m_mousePointerLabel->pixmap()->width() : 0);
	m_availableFieldsDescriptionLabel = new QLabel(
		i18n("Select fields from the list below and drag them onto a form or click the \"Insert\" button"), contents);
	m_availableFieldsDescriptionLabel->setAlignment( Qt::AlignLeft | Qt::TextWordWrap );
	hlyr->addWidget(m_availableFieldsDescriptionLabel);

	//Available Fields
	contentsVlyr->addSpacing(4);
	hlyr = new Q3HBoxLayout(contentsVlyr);
	m_availableFieldsLabel = new QLabel(i18n("Available fields:"), contents);
	m_availableFieldsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	m_availableFieldsLabel->setMargin(2);
	m_availableFieldsLabel->setMinimumHeight(IconSize(K3Icon::Small));
	hlyr->addWidget(m_availableFieldsLabel);

	m_addField = new KexiSmallToolButton(contents, i18n("Insert selected field into form", "Insert"), 
		"add_field", "addFieldButton");
	m_addField->setMinimumHeight(m_availableFieldsLabel->minimumHeight());
//	m_addField->setTextPosition(QToolButton::Right);
	m_addField->setFocusPolicy(StrongFocus);
	m_addField->setToolTip( i18n("Insert selected fields into form"));
	hlyr->addWidget(m_addField);
	connect(m_addField, SIGNAL(clicked()), this, SLOT(slotInsertSelectedFields()));

	m_fieldListView = new KexiFieldListView(contents, "fieldListView",
		KexiFieldListView::ShowDataTypes | KexiFieldListView::AllowMultiSelection );
	m_fieldListView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
	m_availableFieldsLabel->setBuddy(m_fieldListView);
	contentsVlyr->addWidget(m_fieldListView, 1);
	connect(m_fieldListView, SIGNAL(selectionChanged()), this, SLOT(slotFieldListViewSelectionChanged()));
	connect(m_fieldListView, SIGNAL(fieldDoubleClicked(const QString&, const QString&, const QString&)),
		this, SLOT(slotFieldDoubleClicked(const QString&, const QString&, const QString&)));
#endif

	vlyr->addStretch(1);

	connect(m_dataSourceCombo, SIGNAL(textChanged(const QString &)), this, SLOT(slotDataSourceTextChanged(const QString &)));
	connect(m_dataSourceCombo, SIGNAL(dataSourceChanged()), this, SLOT(slotDataSourceChanged()));
	connect(m_sourceFieldCombo, SIGNAL(selected()), this, SLOT(slotFieldSelected()));

	clearDataSourceSelection();
	slotFieldListViewSelectionChanged();
}

KexiDataSourcePage::~KexiDataSourcePage()
{
}

void KexiDataSourcePage::setProject(KexiProject *prj)
{
	m_sourceFieldCombo->setProject(prj);
	m_dataSourceCombo->setProject(prj);
}

void KexiDataSourcePage::clearDataSourceSelection(bool alsoClearComboBox)
{
	if (m_insideClearDataSourceSelection)
		return;
	m_insideClearDataSourceSelection = true;
	if (alsoClearComboBox && !m_dataSourceCombo->selectedName().isEmpty())
		m_dataSourceCombo->setDataSource("", "");
//	if (!m_dataSourceCombo->currentText().isEmpty()) {
//		m_dataSourceCombo->setCurrentText("");
//		emit m_dataSourceCombo->dataSourceSelected();
//	}
	m_clearDSButton->setEnabled(false);
	m_gotoButton->setEnabled(false);
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	m_addField->setEnabled(false);
	m_fieldListView->clear();
#endif
	m_insideClearDataSourceSelection = false;
}

void KexiDataSourcePage::clearWidgetDataSourceSelection()
{
	if (!m_sourceFieldCombo->currentText().isEmpty()) {
		m_sourceFieldCombo->setCurrentText("");
		m_sourceFieldCombo->setFieldOrExpression(QString::null);
		slotFieldSelected();
	}
	m_clearWidgetDSButton->setEnabled(false);
}

void KexiDataSourcePage::slotGotoSelected()
{
	Q3CString mime = m_dataSourceCombo->selectedMimeType().latin1();
	if (mime=="kexi/table" || mime=="kexi/query") {
		if (m_dataSourceCombo->isSelectionValid())
			emit jumpToObjectRequested(mime, m_dataSourceCombo->selectedName().latin1());
	}
}

void KexiDataSourcePage::slotInsertSelectedFields()
{
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	QStringList selectedFieldNames(m_fieldListView->selectedFieldNames());
	if (selectedFieldNames.isEmpty())
		return;

	emit insertAutoFields(m_fieldListView->schema()->table() ? "kexi/table" : "kexi/query",
		m_fieldListView->schema()->name(), selectedFieldNames);
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

void KexiDataSourcePage::slotDataSourceTextChanged(const QString & string)
{
	Q_UNUSED(string);
	const bool enable = m_dataSourceCombo->isSelectionValid(); //!string.isEmpty() && m_dataSourceCombo->selectedName() == string.latin1();
	if (!enable) {
		clearDataSourceSelection( m_dataSourceCombo->selectedName().isEmpty()/*alsoClearComboBox*/ );
	}
	updateSourceFieldWidgetsAvailability();
/*#ifndef KEXI_NO_AUTOFIELD_WIDGET
	m_fieldListView->setEnabled(enable);
//	m_addField->setEnabled(enable);
	m_availableFieldsLabel->setEnabled(enable);
#endif*/
}

void KexiDataSourcePage::slotDataSourceChanged()
{
	if (!m_dataSourceCombo->project())
		return;
	Q3CString mime = m_dataSourceCombo->selectedMimeType().latin1();
	bool dataSourceFound = false;
	Q3CString name = m_dataSourceCombo->selectedName().latin1();
	if ((mime=="kexi/table" || mime=="kexi/query") && m_dataSourceCombo->isSelectionValid()) {
		KexiDB::TableOrQuerySchema *tableOrQuery = new KexiDB::TableOrQuerySchema(
			m_dataSourceCombo->project()->dbConnection(), name, mime=="kexi/table");
		if (tableOrQuery->table() || tableOrQuery->query()) {
#ifdef KEXI_NO_AUTOFIELD_WIDGET
			m_tableOrQuerySchema = tableOrQuery;
#else
			m_fieldListView->setSchema( tableOrQuery );
#endif
			dataSourceFound = true;
			m_sourceFieldCombo->setTableOrQuery(name, mime=="kexi/table");
		}
		else {
			delete tableOrQuery;
		}
	}
	if (!dataSourceFound) {
		m_sourceFieldCombo->setTableOrQuery("", true);
	}
	//if (m_sourceFieldCombo->hasFocus())
//		m_dataSourceCombo->setFocus();
	m_clearDSButton->setEnabled(dataSourceFound);
	m_gotoButton->setEnabled(dataSourceFound);
	if (dataSourceFound) {
		slotFieldListViewSelectionChanged();
	} else {
#ifndef KEXI_NO_AUTOFIELD_WIDGET
		m_addField->setEnabled(false);
#endif
	}
	updateSourceFieldWidgetsAvailability();
	emit formDataSourceChanged(mime, name);
}

void KexiDataSourcePage::slotFieldSelected()
{
	KexiDB::Field::Type dataType = KexiDB::Field::InvalidType;
#ifdef KEXI_NO_AUTOFIELD_WIDGET
	KexiDB::Field *field = m_tableOrQuerySchema->field( m_sourceFieldCombo->fieldOrExpression() ); //temp
#else
//! @todo this should also work for expressions
	KexiDB::Field *field = m_fieldListView->schema()->field( m_sourceFieldCombo->fieldOrExpression() );
#endif
	if (field)
		dataType = field->type();

	m_clearWidgetDSButton->setEnabled( !m_sourceFieldCombo->fieldOrExpression().isEmpty() );

	emit dataSourceFieldOrExpressionChanged(
		m_sourceFieldCombo->fieldOrExpression(),
		m_sourceFieldCombo->fieldOrExpressionCaption(),
		dataType
	);
}

void KexiDataSourcePage::setDataSource(const Q3CString& mimeType, const Q3CString& name)
{
	m_dataSourceCombo->setDataSource(mimeType, name);
}

void KexiDataSourcePage::assignPropertySet(KoProperty::Set* propertySet)
{
	Q3CString objectName;
	if (propertySet && propertySet->contains("name"))
		objectName = (*propertySet)["name"].value().toCString();
	if (!objectName.isEmpty() && objectName == m_currentObjectName)
		return; //the same object
	m_currentObjectName = objectName;

	Q3CString objectClassName;
	if (propertySet && propertySet->contains("this:className"))
		objectClassName = (*propertySet)["this:className"].value().toCString();
/*moved	if (propertySet) {
		Q3CString iconName;
		QString objectClassString;
		if (propertySet->contains("this:iconName"))
			iconName = (*propertySet)["this:iconName"].value().toCString();
		if (propertySet->contains("this:classString"))
			objectClassString = (*propertySet)["this:classString"].value().toString();
		m_objectInfoLabel->setObjectName(objectName);
		m_objectInfoLabel->setObjectClassIcon(iconName);
		m_objectInfoLabel->setObjectClassName(objectClassString);
		if (propertySet->contains("this:className"))
			objectClassName = (*propertySet)["this:className"].value().toCString();
	}*/
	KexiPropertyEditorView::updateInfoLabelForPropertySet(
		m_objectInfoLabel, propertySet);

	const bool isForm = objectClassName=="KexiDBForm";
//	kDebug() << "objectClassName=" << objectClassName << endl;
//	{
/*		//this is top level form's surface: data source means table or query
		QCString dataSourceMimeType, dataSource;
		if (buffer->hasProperty("dataSourceMimeType"))
			dataSourceMimeType = (*buffer)["dataSourceMimeType"].value().toCString();
		if (buffer->hasProperty("dataSource"))
			dataSource = (*buffer)["dataSource"].value().toCString();
		m_dataSourceCombo->setDataSource(dataSourceMimeType, dataSource);*/
//	}
//	else {

	const bool multipleSelection = objectClassName=="special:multiple";
	const bool hasDataSourceProperty = propertySet && propertySet->contains("dataSource") && !multipleSelection;

	if (!isForm) {
		//this is a widget
		Q3CString dataSource;
		if (hasDataSourceProperty) {
			if (propertySet)
				dataSource = (*propertySet)["dataSource"].value().toCString();
			m_noDataSourceAvailableLabel->hide();
			m_sourceFieldCombo->setFieldOrExpression(dataSource);
			m_sourceFieldCombo->setEnabled(true);
			m_clearWidgetDSButton->setEnabled(!m_sourceFieldCombo->currentText().isEmpty());
			m_widgetDSLabel->show();
			m_clearWidgetDSButton->show();
			m_sourceFieldCombo->show();
//			m_dataSourceSeparator->hide();
			updateSourceFieldWidgetsAvailability();
		}
	}

	if (isForm) {
		m_noDataSourceAvailableLabel->hide();
//		m_dataSourceSeparator->hide();
	}
	else if (!hasDataSourceProperty) {
		if (multipleSelection)
			m_noDataSourceAvailableLabel->setText(m_noDataSourceAvailableMultiText);
		else
			m_noDataSourceAvailableLabel->setText(m_noDataSourceAvailableSingleText);
		m_noDataSourceAvailableLabel->show();
//		m_dataSourceSeparator->show();
		//make 'No data source could be assigned' label's height the same as the 'source field' combo+label
		m_noDataSourceAvailableLabel->setMinimumHeight(m_widgetDSLabel->height()
			+ m_sourceFieldCombo->height()/*-m_dataSourceSeparator->height()*/);
		m_sourceFieldCombo->setCurrentText("");
	}

	if (isForm || !hasDataSourceProperty) {
		//no source field can be set
		m_widgetDSLabel->hide();
		m_clearWidgetDSButton->hide();
		m_sourceFieldCombo->hide();
	}
}

void KexiDataSourcePage::slotFieldListViewSelectionChanged()
{
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	//update "add field" button's state
	for (Q3ListViewItemIterator it(m_fieldListView); it.current(); ++it) {
		if (it.current()->isSelected()) {
			m_addField->setEnabled(true);
			return;
		}
	}
	m_addField->setEnabled(false);
#endif
}

void KexiDataSourcePage::updateSourceFieldWidgetsAvailability()
{
	const bool hasDataSource = m_dataSourceCombo->isSelectionValid(); //!m_dataSourceCombo->selectedName().isEmpty();
	m_sourceFieldCombo->setEnabled( hasDataSource );
	m_widgetDSLabel->setEnabled( hasDataSource );
#ifndef KEXI_NO_AUTOFIELD_WIDGET
	m_fieldListView->setEnabled( hasDataSource );
	m_availableFieldsLabel->setEnabled( hasDataSource );
	m_mousePointerLabel->setEnabled( hasDataSource );
	m_availableFieldsDescriptionLabel->setEnabled( hasDataSource );
#endif
}

#include "kexidatasourcepage.moc"
