/* This file is part of the KDE project
   Copyright (C) 2005-2009 Jarosław Staniek <staniek@kde.org>

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

#include <QLabel>
#include <QLineEdit>
#include <QToolTip>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include <KIconLoader>
#include <KLocale>
#include <KDebug>
#include <KFadeWidgetEffect>

#include <widget/properties/KexiPropertyEditorView.h>
#include <widget/KexiObjectInfoLabel.h>
#include <widget/KexiDataSourceComboBox.h>
#include <widget/fields/KexiFieldListView.h>
#include <widget/fields/KexiFieldComboBox.h>
#include <kexiutils/SmallToolButton.h>
#include <kexidb/connection.h>
#include <kexiproject.h>

#include <formeditor/commands.h>
#include <koproperty/Property.h>
#include <koproperty/Utils.h>

KexiDataSourcePage::KexiDataSourcePage(QWidget *parent)
        : KexiPropertyPaneViewBase(parent)
        , m_noDataSourceAvailableSingleText(
            i18n("No data source could be assigned for this widget.") )
        , m_noDataSourceAvailableMultiText(
            i18n("No data source could be assigned for multiple widgets.") )
        , m_insideClearFormDataSourceSelection(false)
{
/*moved
    Q3VBoxLayout *vlyr = new Q3VBoxLayout(this);
    m_objectInfoLabel = new KexiObjectInfoLabel(this);
    m_objectInfoLabel->setObjectName("KexiObjectInfoLabel");
    vlyr->addWidget(m_objectInfoLabel);*/
//moved    vlyr->addSpacing(8);

    //Section 1: Form's/Widget's Data Source
/*2.0    KoProperty::GroupContainer *container = new KoProperty::GroupContainer(
        i18n("Data Source"), this);
    layout()->addWidget(container);

    QWidget *contents = new QWidget(container);
    container->setContents(contents);*/
//2.0    QVBoxLayout *contentsVlyr = new QVBoxLayout(this);
//2.0    contentsVlyr->setContentsMargins(0, 0, 0, 0);

    infoLabel()->setContentsMargins(0, 0, 0, spacing());

    m_noDataSourceAvailableLabel = new QLabel(m_noDataSourceAvailableSingleText, this);
    m_noDataSourceAvailableLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    m_noDataSourceAvailableLabel->setContentsMargins(0, 0, 0, spacing());
//m_noDataSourceAvailableLabel->setAutoFillBackground(true);
//m_noDataSourceAvailableLabel->setPaletteBackgroundColor(Qt::red);
//2.0    m_noDataSourceAvailableLabel->setMargin(2);
    m_noDataSourceAvailableLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
    m_noDataSourceAvailableLabel->setWordWrap(true);
    mainLayout()->addWidget(m_noDataSourceAvailableLabel);

    //-Widget's Data Source
    QHBoxLayout *hlyr = new QHBoxLayout();
    mainLayout()->addLayout(hlyr);
#if 0
//! @todo unhide this when expression work
// m_widgetDSLabel = new QLabel(i18nc("Table Field, Query Field or Expression", "Source field or expression"), this);
#else
    m_widgetDSLabel = new QLabel(
        i18nc("Table Field or Query Field", "Widget's data source"), this);
#endif
    m_widgetDSLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
//2.0    m_widgetDSLabel->setMargin(2);
//2.0    m_widgetDSLabel->setMinimumHeight(IconSize(KIconLoader::Small) + 4);
    m_widgetDSLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    hlyr->addWidget(m_widgetDSLabel);

#if 0 //2.0: clear button is available in the combobox itself
    m_clearWidgetDSButton = new KexiSmallToolButton(
        KIcon("edit-clear-locationbar-rtl"), QString(), this);
    m_clearWidgetDSButton->setObjectName("clearWidgetDSButton");
    m_clearWidgetDSButton->setMinimumHeight(m_widgetDSLabel->minimumHeight());
    m_clearWidgetDSButton->setToolTip(i18n("Clear widget's data source"));
    hlyr->addWidget(m_clearWidgetDSButton);
    connect(m_clearWidgetDSButton, SIGNAL(clicked()),
            this, SLOT(clearWidgetDataSourceSelection()));
#endif

    m_widgetDataSourceCombo = new KexiFieldComboBox(this);
    m_widgetDataSourceCombo->setObjectName("sourceFieldCombo");
    m_widgetDataSourceCombo->setContentsMargins(0, 0, 0, 0);
    m_widgetDSLabel->setBuddy(m_widgetDataSourceCombo);
    connect(m_widgetDataSourceCombo->lineEdit(), SIGNAL(clearButtonClicked()),
        this, SLOT(clearWidgetDataSourceSelection()));
    mainLayout()->addWidget(m_widgetDataSourceCombo);
    
    m_widgetDataSourceComboSpacer = addWidgetSpacer();

    /* m_dataSourceSeparator = new Q3Frame(contents);
      m_dataSourceSeparator->setFrameShape(Q3Frame::HLine);
      m_dataSourceSeparator->setFrameShadow(Q3Frame::Sunken);
      mainLayout()->addWidget(m_dataSourceSeparator);*/

//    addSpacing();

    //- Form's Data Source
    hlyr = new QHBoxLayout();
    hlyr->setContentsMargins(0, 0, 0, 0);
    mainLayout()->addLayout(hlyr);
    m_dataSourceLabel = new QLabel(i18n("Form's data source"), this);
    m_dataSourceLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
//2.0    m_dataSourceLabel->setMargin(2);
//2.0    m_dataSourceLabel->setMinimumHeight(IconSize(KIconLoader::Small) + 4);
    m_dataSourceLabel->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    hlyr->addWidget(m_dataSourceLabel);

//m_dataSourceLabel->setAutoFillBackground(true);
//m_dataSourceLabel->setPaletteBackgroundColor(Qt::red);

    m_gotoButton = new KexiSmallToolButton(
        KIcon("go-jump"), QString(), this);
    m_gotoButton->setObjectName("gotoButton");
//2.0    m_gotoButton->setMinimumHeight(m_dataSourceLabel->minimumHeight());
    m_gotoButton->setToolTip(i18n("Go to selected form's data source"));
    m_gotoButton->setWhatsThis(i18n("Goes to selected form's data source"));
    hlyr->addWidget(m_gotoButton);
    connect(m_gotoButton, SIGNAL(clicked()), this, SLOT(slotGotoSelected()));

#if 0 //2.0: clear button is available in the combobox itself
    m_clearDSButton = new KexiSmallToolButton(
        KIcon("edit-clear-locationbar-rtl"), QString(), this);
    m_clearDSButton->setObjectName("clearDSButton");
    m_clearDSButton->setMinimumHeight(m_dataSourceLabel->minimumHeight());
    m_clearDSButton->setToolTip(i18n("Clear form's data source"));
    hlyr->addWidget(m_clearDSButton);
    connect(m_clearDSButton, SIGNAL(clicked()), this, SLOT(clearFormDataSourceSelection()));
#endif

    m_formDataSourceCombo = new KexiDataSourceComboBox(this);
    m_formDataSourceCombo->setObjectName("dataSourceCombo");
    m_formDataSourceCombo->setContentsMargins(0, 0, 0, 0);
    m_dataSourceLabel->setBuddy(m_formDataSourceCombo);
    connect(m_formDataSourceCombo->lineEdit(), SIGNAL(clearButtonClicked()),
        this, SLOT(clearFormDataSourceSelection()));
    mainLayout()->addWidget(m_formDataSourceCombo);

    m_formDataSourceComboSpacer = addWidgetSpacer();

#ifdef KEXI_NO_AUTOFIELD_WIDGET
    m_availableFieldsLabel = 0;
    m_addField = 0;
// m_fieldListView = 0;
    mainLayout()->addStretch();
#else
//    addSpacing();
    /* Q3Frame *separator = new QFrame(this);
      separator->setFrameShape(Q3Frame::HLine);
      separator->setFrameShadow(Q3Frame::Sunken);
      vlyr->addWidget(separator);*/
    /*
      KPopupTitle *title = new KPopupTitle(this);
      title->setTitle(i18n("Inserting fields"));
      vlyr->addWidget(title);
      vlyr->addSpacing(4);*/


    //2. Inserting fields
/*2.0    container = new KoProperty::GroupContainer(i18n("Inserting Fields"), this);
    static_cast<QBoxLayout*>(layout())->addWidget(container, 1);*/

    //helper info
//! @todo allow to hide such helpers by adding global option
//2.0    contents = new QWidget(container);
//2.0    container->setContents(contents);
//2.0    mainLayout() = new QVBoxLayout(contents);
    hlyr = new QHBoxLayout();
    hlyr->setContentsMargins(0, 0, 0, 0);
    mainLayout()->addLayout(hlyr);
    m_mousePointerLabel = new QLabel(this);
    hlyr->addWidget(m_mousePointerLabel);
    m_mousePointerLabel->setPixmap(SmallIcon("mouse_pointer"));
    m_mousePointerLabel->setFixedWidth(m_mousePointerLabel->pixmap()
                                       ? m_mousePointerLabel->pixmap()->width() : 0);
    m_availableFieldsDescriptionLabel = new QLabel(
        i18n("Select fields from the list below and drag them onto"
             " a form or click the \"Insert\" button"), this);
    m_availableFieldsDescriptionLabel->setAlignment(Qt::AlignLeft);
    m_availableFieldsDescriptionLabel->setWordWrap(true);
    hlyr->addWidget(m_availableFieldsDescriptionLabel);

    //Available Fields
//2.0    mainLayout()->addSpacing(4);
    hlyr = new QHBoxLayout();
    hlyr->setContentsMargins(0, 0, 0, 0);
    mainLayout()->addLayout(hlyr);
    m_availableFieldsLabel = new QLabel(i18n("Available fields"), this);
    m_availableFieldsLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
//2.0    m_availableFieldsLabel->setMargin(2);
//2.0    m_availableFieldsLabel->setMinimumHeight(IconSize(KIconLoader::Small));
    hlyr->addWidget(m_availableFieldsLabel);

    m_addField = new KexiSmallToolButton(
        KIcon("add_field"), i18nc("Insert selected field into form", "Insert"), this);
    m_addField->setObjectName("addFieldButton");
//2.0    m_addField->setMinimumHeight(m_availableFieldsLabel->minimumHeight());
// m_addField->setTextPosition(QToolButton::Right);
    m_addField->setFocusPolicy(Qt::StrongFocus);
    m_addField->setToolTip(i18n("Insert selected fields into form"));
    m_addField->setWhatsThis(i18n("Inserts selected fields into form"));
    hlyr->addWidget(m_addField);
    connect(m_addField, SIGNAL(clicked()), this, SLOT(slotInsertSelectedFields()));

    m_fieldListView = new KexiFieldListView(this,
        KexiFieldListView::ShowDataTypes | KexiFieldListView::AllowMultiSelection);
    m_fieldListView->setObjectName("fieldListView");
    m_fieldListView->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    m_availableFieldsLabel->setBuddy(m_fieldListView);
    mainLayout()->addWidget(m_fieldListView, 1);
    connect(m_fieldListView, SIGNAL(selectionChanged()),
            this, SLOT(slotFieldListViewSelectionChanged()));
    connect(m_fieldListView,
            SIGNAL(fieldDoubleClicked(const QString&, const QString&, const QString&)),
            this, SLOT(slotFieldDoubleClicked(const QString&, const QString&, const QString&)));
#endif

    mainLayout()->addStretch(1);

    connect(m_formDataSourceCombo, SIGNAL(textChanged(const QString &)),
            this, SLOT(slotFormDataSourceTextChanged(const QString &)));
    connect(m_formDataSourceCombo, SIGNAL(dataSourceChanged()),
            this, SLOT(slotFormDataSourceChanged()));
    connect(m_widgetDataSourceCombo, SIGNAL(selected()),
            this, SLOT(slotFieldSelected()));

    clearFormDataSourceSelection();
    slotFieldListViewSelectionChanged();
}

KexiDataSourcePage::~KexiDataSourcePage()
{
#ifdef KEXI_NO_AUTOFIELD_WIDGET
    delete m_tableOrQuerySchema;
#endif
}

void KexiDataSourcePage::setProject(KexiProject *prj)
{
    m_widgetDataSourceCombo->setProject(prj);
    m_formDataSourceCombo->setProject(prj);
}

void KexiDataSourcePage::clearFormDataSourceSelection(bool alsoClearComboBox)
{
    if (m_insideClearFormDataSourceSelection)
        return;
    m_insideClearFormDataSourceSelection = true;
    if (alsoClearComboBox && !m_formDataSourceCombo->selectedName().isEmpty())
        m_formDataSourceCombo->setDataSource(QString(), QString());
// if (!m_formDataSourceCombo->currentText().isEmpty()) {
//  m_formDataSourceCombo->setCurrentText("");
//  emit m_formDataSourceCombo->dataSourceSelected();
// }
/*2.0: clear button is available in the combobox itself
    m_clearDSButton->setEnabled(false);*/
    m_gotoButton->setEnabled(false);
    m_widgetDataSourceCombo->setFieldOrExpression(QString());
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    m_addField->setEnabled(false);
    m_fieldListView->clear();
#endif
    m_insideClearFormDataSourceSelection = false;
}

void KexiDataSourcePage::clearWidgetDataSourceSelection()
{
//    if (!m_widgetDataSourceCombo->currentText().isEmpty()) {
//2.0        m_widgetDataSourceCombo->setEditText(QString());
        m_widgetDataSourceCombo->setFieldOrExpression(QString());
        slotFieldSelected();
//    }
/*2.0: clear button is available in the combobox itself
    m_clearWidgetDSButton->setEnabled(false);*/
}

void KexiDataSourcePage::slotGotoSelected()
{
    const QString partClass(m_formDataSourceCombo->selectedPartClass());
    if (partClass == "org.kexi-project.table" || partClass == "org.kexi-project.query") {
        if (m_formDataSourceCombo->isSelectionValid())
            emit jumpToObjectRequested(partClass, m_formDataSourceCombo->selectedName());
    }
}

void KexiDataSourcePage::slotInsertSelectedFields()
{
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    QStringList selectedFieldNames(m_fieldListView->selectedFieldNames());
    if (selectedFieldNames.isEmpty())
        return;

    emit insertAutoFields(m_fieldListView->schema()->table()
                            ? "org.kexi-project.table" : "org.kexi-project.query",
                          m_fieldListView->schema()->name(), selectedFieldNames);
#endif
}

void KexiDataSourcePage::slotFieldDoubleClicked(const QString& sourcePartClass, const QString& sourceName,
        const QString& fieldName)
{
#ifdef KEXI_NO_AUTOFIELD_WIDGET
    Q_UNUSED(sourcePartClass);
    Q_UNUSED(sourceName);
    Q_UNUSED(fieldName);
#else
    QStringList selectedFields;
    selectedFields.append(fieldName);
    emit insertAutoFields(sourcePartClass, sourceName, selectedFields);
#endif
}

void KexiDataSourcePage::slotFormDataSourceTextChanged(const QString & string)
{
    Q_UNUSED(string);
    const bool enable = m_formDataSourceCombo->isSelectionValid(); //!string.isEmpty() && m_formDataSourceCombo->selectedName() == string.toLatin1();
    if (!enable) {
        clearFormDataSourceSelection(m_formDataSourceCombo->selectedName().isEmpty()/*alsoClearComboBox*/);
    }
    updateSourceFieldWidgetsAvailability();
    /*#ifndef KEXI_NO_AUTOFIELD_WIDGET
      m_fieldListView->setEnabled(enable);
    // m_addField->setEnabled(enable);
      m_availableFieldsLabel->setEnabled(enable);
    #endif*/
}

void KexiDataSourcePage::slotFormDataSourceChanged()
{
    if (!m_formDataSourceCombo->project())
        return;
    const QString partClass(m_formDataSourceCombo->selectedPartClass());
    bool dataSourceFound = false;
    QString name(m_formDataSourceCombo->selectedName());
    const bool isPartAcceptable = partClass == QLatin1String("org.kexi-project.table")
        || partClass == QLatin1String("org.kexi-project.query");
    if (isPartAcceptable && m_formDataSourceCombo->isSelectionValid())
    {
        KexiDB::TableOrQuerySchema *tableOrQuery = new KexiDB::TableOrQuerySchema(
            m_formDataSourceCombo->project()->dbConnection(), name.toLatin1(), 
            partClass == "org.kexi-project.table");
        if (tableOrQuery->table() || tableOrQuery->query()) {
#ifdef KEXI_NO_AUTOFIELD_WIDGET
            m_tableOrQuerySchema = tableOrQuery;
#else
            m_fieldListView->setSchema(tableOrQuery);
#endif
            dataSourceFound = true;
            m_widgetDataSourceCombo->setTableOrQuery(name, partClass == "org.kexi-project.table");
        } else {
            delete tableOrQuery;
        }
    }
    if (!dataSourceFound) {
        m_widgetDataSourceCombo->setTableOrQuery(QString(), true);
    }
    //if (m_widgetDataSourceCombo->hasFocus())
//  m_formDataSourceCombo->setFocus();
/*2.0: clear button is available in the combobox itself
    m_clearDSButton->setEnabled(dataSourceFound);*/
    m_gotoButton->setEnabled(dataSourceFound);
    if (dataSourceFound) {
        slotFieldListViewSelectionChanged();
    } else {
#ifndef KEXI_NO_AUTOFIELD_WIDGET
        m_addField->setEnabled(false);
#endif
    }
    updateSourceFieldWidgetsAvailability();
    emit formDataSourceChanged(partClass, name);
}

void KexiDataSourcePage::slotFieldSelected()
{
    KexiDB::Field::Type dataType = KexiDB::Field::InvalidType;
#ifdef KEXI_NO_AUTOFIELD_WIDGET
    KexiDB::Field *field = m_tableOrQuerySchema->field(
                               m_widgetDataSourceCombo->fieldOrExpression());  //temp
#else
//! @todo this should also work for expressions
    KexiDB::Field *field = m_fieldListView->schema()->field(
                               m_widgetDataSourceCombo->fieldOrExpression());
#endif
    if (field)
        dataType = field->type();

/*2.0: clear button is available in the combobox itself
    m_clearWidgetDSButton->setEnabled(!m_widgetDataSourceCombo->fieldOrExpression().isEmpty());*/

    emit dataSourceFieldOrExpressionChanged(
        m_widgetDataSourceCombo->fieldOrExpression(),
        m_widgetDataSourceCombo->fieldOrExpressionCaption(),
        dataType
    );
}

void KexiDataSourcePage::setFormDataSource(const QString& partClass, const QString& name)
{
    m_formDataSourceCombo->setDataSource(partClass, name);
}

#define KexiDataSourcePage_FADE 1

void KexiDataSourcePage::assignPropertySet(KoProperty::Set* propertySet)
{
    QString objectName;
    if (propertySet)
        objectName = propertySet->propertyValue("objectName").toString();
    if (!objectName.isEmpty() && objectName == m_currentObjectName)
        return; //the same object
    m_currentObjectName = objectName;

#if KexiDataSourcePage_FADE //TODO
    KFadeWidgetEffect *animation = 0;
    if (isVisible())
        animation = new KFadeWidgetEffect(this);
#endif
    QString objectClassName;
    if (propertySet) {
        objectClassName = propertySet->propertyValue("this:className").toString();
    }
    updateInfoLabelForPropertySet(propertySet);

    const bool isForm = objectClassName == "KexiDBForm";
// kDebug() << "objectClassName=" << objectClassName;
// {
    /*  //this is top level form's surface: data source means table or query
        QCString dataSourcePartClass, dataSource;
        if (buffer->hasProperty("dataSourcePartClass"))
          dataSourcePartClass = (*buffer)["dataSourcePartClass"].value().toCString();
        if (buffer->hasProperty("dataSource"))
          dataSource = (*buffer)["dataSource"].value().toCString();
        m_formDataSourceCombo->setDataSource(dataSourcePartClass, dataSource);*/
// }
// else {

    const bool multipleSelection = objectClassName == "special:multiple";
    const bool hasDataSourceProperty = propertySet
                                       && propertySet->contains("dataSource") && !multipleSelection;

    if (!isForm) {
        //this is a widget
        QString dataSource;
        if (hasDataSourceProperty) {
            if (propertySet) {
                dataSource = (*propertySet)["dataSource"].value().toString();
            }
            m_noDataSourceAvailableLabel->hide();
            m_widgetDataSourceCombo->setFieldOrExpression(dataSource);
            m_widgetDataSourceCombo->setEnabled(true);
/*2.0: clear button is available in the combobox itself
            m_clearWidgetDSButton->setEnabled(!m_widgetDataSourceCombo->currentText().isEmpty());*/
            m_widgetDSLabel->show();
/*2.0: clear button is available in the combobox itself
            m_clearWidgetDSButton->show();*/
            m_widgetDataSourceCombo->show();
            m_widgetDataSourceComboSpacer->show();
//   m_dataSourceSeparator->hide();
            updateSourceFieldWidgetsAvailability();
        }
    }

    if (isForm) {
        m_noDataSourceAvailableLabel->hide();
//  m_dataSourceSeparator->hide();
    }
    else if (!hasDataSourceProperty) {
        if (multipleSelection) {
            m_noDataSourceAvailableLabel->setText(m_noDataSourceAvailableMultiText);
        }
        else {
            m_noDataSourceAvailableLabel->setText(m_noDataSourceAvailableSingleText);
        }
        m_noDataSourceAvailableLabel->show();
//  m_dataSourceSeparator->show();
        //make 'No data source could be assigned' label's height the same as the 'source field' combo+label
//2.0        m_noDataSourceAvailableLabel->setMinimumHeight(m_widgetDSLabel->height()
//2.0                + m_widgetDataSourceCombo->height()/*-m_dataSourceSeparator->height()*/);
        m_widgetDataSourceCombo->setEditText(QString());
    }

    if (isForm || !hasDataSourceProperty) {
        //no source field can be set
        m_widgetDSLabel->hide();
/*2.0: clear button is available in the combobox itself
        m_clearWidgetDSButton->hide();*/
        m_widgetDataSourceCombo->hide();
        m_widgetDataSourceComboSpacer->hide();
    }
#if KexiDataSourcePage_FADE //TODO
    if (animation)
        animation->start(100);
#endif
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
    const bool hasDataSource = m_formDataSourceCombo->isSelectionValid();
    m_widgetDataSourceCombo->setEnabled(hasDataSource);
    m_widgetDSLabel->setEnabled(hasDataSource);
#ifndef KEXI_NO_AUTOFIELD_WIDGET
    m_fieldListView->setEnabled(hasDataSource);
    m_availableFieldsLabel->setEnabled(hasDataSource);
    m_mousePointerLabel->setEnabled(hasDataSource);
    m_availableFieldsDescriptionLabel->setEnabled(hasDataSource);
#endif
}

#include "kexidatasourcepage.moc"
