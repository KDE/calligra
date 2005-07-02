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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kexidatasourcepage.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qheader.h>

#include <kiconloader.h>
#include <klocale.h>
#include <ktoolbarbutton.h>

#include <widget/kexipropertyeditorview.h>
#include <widget/kexidatasourcecombobox.h>
#include <widget/kexifieldlistview.h>
#include <widget/kexifieldcombobox.h>
#include <kexidb/connection.h>
#include <kexidb/utils.h>
#include <kexiproject.h>

#include <koproperty/set.h>
#include <koproperty/property.h>

KexiDataSourcePage::KexiDataSourcePage(QWidget *parent, const char *name)
 : QWidget(parent, name)
 , m_insideClearDataSourceSelection(false)
{
	QVBoxLayout *vlyr = new QVBoxLayout(this);
	m_objectInfoLabel = new KexiObjectInfoLabel(this, "KexiObjectInfoLabel");
	vlyr->addWidget(m_objectInfoLabel);

	m_noDataSourceAvailableSingleText = i18n("No data source could be assigned for this widget.");
	m_noDataSourceAvailableMultiText = i18n("No data source could be assigned for multiple widgets.");
	m_noDataSourceAvailableLabel = new QLabel(m_noDataSourceAvailableSingleText, this);
	m_noDataSourceAvailableLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
	m_noDataSourceAvailableLabel->setMargin(2);
	m_noDataSourceAvailableLabel->setAlignment(Qt::WordBreak | Qt::AlignBottom | Qt::AlignLeft);
	vlyr->addWidget(m_noDataSourceAvailableLabel);

	//Widget's Data Source
	QHBoxLayout *hlyr = new QHBoxLayout(vlyr);
	m_widgetDSLabel = new QLabel(i18n("Table Field, Query Field or Expression", "Source Field or Expression:"), this);
	m_widgetDSLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	m_widgetDSLabel->setMargin(2);
	m_widgetDSLabel->setMinimumHeight(IconSize(KIcon::Small)+4);
	m_widgetDSLabel->setAlignment(AlignLeft|AlignBottom);
	hlyr->addWidget(m_widgetDSLabel);

	m_clearWidgetDSButton = new QToolButton(this, "clearWidgetDSButton");
	m_clearWidgetDSButton->setIconSet(SmallIconSet("clear_left"));
	m_clearWidgetDSButton->setMinimumHeight(m_widgetDSLabel->minimumHeight());
	m_clearWidgetDSButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_clearWidgetDSButton->setAutoRaise(true);
	m_clearWidgetDSButton->setPaletteBackgroundColor(palette().normal().background());
	QToolTip::add(m_clearWidgetDSButton, i18n("Clear widget's data source"));
	hlyr->addWidget(m_clearWidgetDSButton);
	connect(m_clearWidgetDSButton, SIGNAL(clicked()), this, SLOT(clearWidgetDataSourceSelection()));

	m_sourceFieldCombo = new KexiFieldComboBox(this, "sourceFieldCombo");
	vlyr->addWidget(m_sourceFieldCombo);

	vlyr->addSpacing(8);

	m_dataSourceSeparator = new QFrame(this);
	m_dataSourceSeparator->setFrameShape(QFrame::HLine);
	m_dataSourceSeparator->setFrameShadow(QFrame::Sunken);
	vlyr->addWidget(m_dataSourceSeparator);

	//Form's Data Source
	hlyr = new QHBoxLayout(vlyr);
	QLabel *dsLabel = new QLabel(i18n("Form's Data Source:"), this);
	dsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	dsLabel->setMargin(2);
	dsLabel->setMinimumHeight(IconSize(KIcon::Small)+4);
	dsLabel->setAlignment(AlignLeft|AlignBottom);
	hlyr->addWidget(dsLabel);

	m_gotoButton = new QToolButton(this, "gotoButton");
	m_gotoButton->setIconSet(SmallIconSet("goto"));
	m_gotoButton->setMinimumHeight(dsLabel->minimumHeight());
	m_gotoButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_gotoButton->setAutoRaise(true);
	m_gotoButton->setPaletteBackgroundColor(palette().normal().background());
	QToolTip::add(m_gotoButton, i18n("Go to selected data source"));
	hlyr->addWidget(m_gotoButton);
	connect(m_gotoButton, SIGNAL(clicked()), this, SLOT(slotGotoSelected()));

	m_clearDSButton = new QToolButton(this, "clearDSButton");
	m_clearDSButton->setIconSet(SmallIconSet("clear_left"));
	m_clearDSButton->setMinimumHeight(dsLabel->minimumHeight());
	m_clearDSButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_clearDSButton->setAutoRaise(true);
	m_clearDSButton->setPaletteBackgroundColor(palette().normal().background());
	QToolTip::add(m_clearDSButton, i18n("Clear data source"));
	hlyr->addWidget(m_clearDSButton);
	connect(m_clearDSButton, SIGNAL(clicked()), this, SLOT(clearDataSourceSelection()));
	
	m_dataSourceCombo = new KexiDataSourceComboBox(this, "dataSourceCombo");
	dsLabel->setBuddy(m_dataSourceCombo);
	vlyr->addWidget(m_dataSourceCombo);

	vlyr->addSpacing(4);
	hlyr = new QHBoxLayout(vlyr);

	//Available Fields
	m_availableFieldsLabel = new QLabel(i18n("Available Fields:"), this);
	m_availableFieldsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	m_availableFieldsLabel->setMargin(2);
	m_availableFieldsLabel->setMinimumHeight(IconSize(KIcon::Small));
	hlyr->addWidget(m_availableFieldsLabel);

	m_addField = new QToolButton(this, "addFieldButton");
	m_addField->setIconSet(SmallIconSet("add_field"));
	m_addField->setMinimumHeight(m_availableFieldsLabel->minimumHeight());
	m_addField->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	m_addField->setAutoRaise(true);
	m_addField->setPaletteBackgroundColor(palette().normal().background());
	QToolTip::add(m_addField, i18n("Add selected field to form"));
	hlyr->addWidget(m_addField);
	connect(m_addField, SIGNAL(clicked()), this, SLOT(slotAddSelectedField()));

	m_fieldListView = new KexiFieldListView(this, "fieldListView", 
		KexiFieldListView::ShowDataTypes 
		| KexiFieldListView::HideTableAsterisk 
		| KexiFieldListView::AllowMultiSelection );
	m_fieldListView->header()->show();
	m_fieldListView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum, 0, 2));
	vlyr->addWidget(m_fieldListView);

	connect(m_dataSourceCombo, SIGNAL(textChanged(const QString &)), this, SLOT(slotDataSourceTextChanged(const QString &)));
	connect(m_dataSourceCombo, SIGNAL(dataSourceSelected()), this, SLOT(slotDataSourceSelected()));
	connect(m_sourceFieldCombo, SIGNAL(selected()), this, SLOT(slotFieldSelected()));

	clearDataSourceSelection();
}

KexiDataSourcePage::~KexiDataSourcePage()
{
}

void KexiDataSourcePage::setProject(KexiProject *prj)
{
	m_sourceFieldCombo->setProject(prj);
	m_dataSourceCombo->setProject(prj);
}

void KexiDataSourcePage::clearDataSourceSelection()
{
	if (m_insideClearDataSourceSelection)
		return;
	m_insideClearDataSourceSelection = true;
	if (!m_dataSourceCombo->selectedName().isEmpty())
		m_dataSourceCombo->setDataSource("", ""); 
//	if (!m_dataSourceCombo->currentText().isEmpty()) {
//		m_dataSourceCombo->setCurrentText("");
//		emit m_dataSourceCombo->dataSourceSelected();
//	}
	m_clearDSButton->setEnabled(false);
	m_gotoButton->setEnabled(false);
	m_addField->setEnabled(false);
	m_fieldListView->clear();
	m_insideClearDataSourceSelection = false;
}

void KexiDataSourcePage::clearWidgetDataSourceSelection()
{
	if (!m_sourceFieldCombo->currentText().isEmpty())
		m_sourceFieldCombo->setCurrentText("");
	m_clearWidgetDSButton->setEnabled(false);
}

void KexiDataSourcePage::slotGotoSelected()
{
	QCString mime = m_dataSourceCombo->selectedMimeType();
	if (mime=="kexi/table" || mime=="kexi/query") {
		QCString name = m_dataSourceCombo->selectedName();
		if (name.isEmpty())
			return;
		emit jumpToObjectRequested(mime, name);
	}
}

void KexiDataSourcePage::slotAddSelectedField()
{
}

void KexiDataSourcePage::slotDataSourceTextChanged(const QString & string)
{
	const bool enable = !string.isEmpty();
	if (!enable) {
		clearDataSourceSelection();
	}
	m_fieldListView->setEnabled(enable);
	m_addField->setEnabled(enable);
	m_availableFieldsLabel->setEnabled(enable);
}

void KexiDataSourcePage::slotDataSourceSelected()
{
	if (!m_dataSourceCombo->project())
		return;
	QCString mime = m_dataSourceCombo->selectedMimeType();
	bool dataSourceFound = false;
	QCString name = m_dataSourceCombo->selectedName();
	if ((mime=="kexi/table" || mime=="kexi/query") && !name.isEmpty()) {
		KexiDB::TableOrQuerySchema *tableOrQuery = new KexiDB::TableOrQuerySchema(
			m_dataSourceCombo->project()->dbConnection(), name, mime=="kexi/table");
		if (tableOrQuery->table() || tableOrQuery->query()) {
			m_fieldListView->setSchema( tableOrQuery );
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
	m_clearDSButton->setEnabled(dataSourceFound);
	m_gotoButton->setEnabled(dataSourceFound);
	m_addField->setEnabled(dataSourceFound);

	emit formDataSourceChanged(mime, name);
}

void KexiDataSourcePage::slotFieldSelected()
{
	emit dataSourceFieldOrExpressionChanged(m_sourceFieldCombo->fieldOrExpression());
}

void KexiDataSourcePage::setDataSource(const QCString& mimeType, const QCString& name)
{
	m_dataSourceCombo->setDataSource(mimeType, name);
}

void KexiDataSourcePage::assignPropertySet(KoProperty::Set* propertySet)
{
	QCString objectClassName;
	if (propertySet) {
		QCString objectName, iconName;
		QString objectClassString;
		if (propertySet->contains("name"))
			objectName = (*propertySet)["name"].value().toCString();
		if (propertySet->contains("this:iconName"))
			iconName = (*propertySet)["this:iconName"].value().toCString();
		if (propertySet->contains("this:classString"))
			objectClassString = (*propertySet)["this:classString"].value().toString();
		m_objectInfoLabel->setObjectName(objectName);
		m_objectInfoLabel->setObjectClassIcon(iconName);
		m_objectInfoLabel->setObjectClassName(objectClassString);
		if (propertySet->contains("this:className"))
			objectClassName = (*propertySet)["this:className"].value().toCString();
	}


	const bool isForm = objectClassName=="KexiDBForm";
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

	const bool hasDataSourceProperty = propertySet && propertySet->contains("dataSource");

	if (!isForm) {
		//this is a widget
		QCString dataSource;
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
			m_dataSourceSeparator->hide();
		}
	}

	if (isForm) {
		m_noDataSourceAvailableLabel->hide();
		m_dataSourceSeparator->hide();
	}
	else if (!hasDataSourceProperty) {
		if (objectClassName=="special:multiple")
			m_noDataSourceAvailableLabel->setText(m_noDataSourceAvailableMultiText);
		else
			m_noDataSourceAvailableLabel->setText(m_noDataSourceAvailableSingleText);
		m_noDataSourceAvailableLabel->show();
		m_dataSourceSeparator->show();
		//make 'No data source could be assigned' label's height the same as the 'source field' combo+label
		m_noDataSourceAvailableLabel->setMinimumHeight(m_widgetDSLabel->height()
			+ m_sourceFieldCombo->height()-m_dataSourceSeparator->height());
		m_sourceFieldCombo->setCurrentText("");
	}

	if (isForm || !hasDataSourceProperty) {
		//no source field can be set
		m_widgetDSLabel->hide();
		m_clearWidgetDSButton->hide();
		m_sourceFieldCombo->hide();
	}
}

#include "kexidatasourcepage.moc"
