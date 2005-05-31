/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <qpopupmenu.h>
#include <qscrollview.h>
#include <qcursor.h>
#include <qpainter.h>

#include <kgenericfactory.h>
#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <knumvalidator.h>
#include <kdatetbl.h>

#include <container.h>
#include <form.h>
#include <formIO.h>
#include <formmanager.h>
#include <objecttree.h>
#include <formeditor/utils.h>
#include <kexidb/utils.h>
#include <kexidb/connection.h>
#include <kexipart.h>
#include <widgetlibrary.h>
#include <kexigradientwidget.h>
#include <kexi_utils.h>
#include <keximainwindow.h>

#include "kexidbform.h"
#include "kexiformview.h"
#include "kexilabel.h"
#include "kexidbinputwidget.h"
#include "kexidataawarewidgetinfo.h"

#include "kexidbfactory.h"
#include <core/kexi.h>

#define KEXI_NO_KexiDBInputWidget //temp. for 0.1


KexiSubForm::KexiSubForm(Form *parentForm, QWidget *parent, const char *name)
: QScrollView(parent, name), m_parentForm(parentForm), m_form(0), m_widget(0)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
}
/*
void
KexiSubForm::paintEvent(QPaintEvent *ev)
{
	QScrollView::paintEvent(ev);
	QPainter p;

	setWFlags(WPaintUnclipped);

	QString txt("Subform");
	QFont f = font();
	f.setPointSize(f.pointSize() * 3);
	QFontMetrics fm(f);
	const int txtw = fm.width(txt), txth = fm.height();

	p.begin(this, true);
	p.setPen(black);
	p.setFont(f);
	p.drawText(width()/2, height()/2, txt, Qt::AlignCenter|Qt::AlignVCenter);
	p.end();

	clearWFlags( WPaintUnclipped );
}
*/
void
KexiSubForm::setFormName(const QString &name)
{
	if(m_formName==name)
		return;

	m_formName = name; //assign, even if the name points to nowhere

	if(name.isEmpty()) {
		delete m_widget;
		m_widget = 0;
		updateScrollBars();
		return;
	}

	QWidget *pw = parentWidget();
	KexiFormView *view = 0;
	QStringList list;
	while(pw) {
		if(pw->isA("KexiSubForm")) {
			if(list.contains(pw->name())) {
//! @todo error message
				return; // Be sure to don't run into a endless-loop cause of recursive subforms.
			}
			list.append(pw->name());
		}
		else if(! view && pw->isA("KexiFormView"))
			view = static_cast<KexiFormView*>(pw); // we need a KexiFormView*
		pw = pw->parentWidget();
	}

	if (!view || !view->parentDialog() || !view->parentDialog()->mainWin()
		|| !view->parentDialog()->mainWin()->project()->dbConnection())
		return;

	KexiDB::Connection *conn = view->parentDialog()->mainWin()->project()->dbConnection();

	// we check if there is a form with this name
	int id = KexiDB::idForObjectName(*conn, name, KexiPart::FormObjectType);
	if((id == 0) || (id == view->parentDialog()->id())) // == our form
		return; // because of recursion when loading

	// we create the container widget
	delete m_widget;
	m_widget = new KexiDBFormBase(viewport(), "kexisubform_widget");
	m_widget->show();
	addChild(m_widget);
	m_form = new Form(m_parentForm->manager(), this->name());
	m_form->createToplevel(m_widget);

	// and load the sub form
	QString data;
	bool ok = conn->loadDataBlock(id, data, QString::null);
	if (ok)
		ok = KFormDesigner::FormIO::loadFormFromString(m_form, m_widget, data);
	if(!ok) {
		delete m_widget;
		m_widget = 0;
		updateScrollBars();
		m_formName = QString::null;
		return;
	}
	m_form->setDesignMode(false);

	// Install event filters on the whole newly created form
	KFormDesigner::ObjectTreeItem *tree = m_parentForm->objectTree()->lookup(QObject::name());
	KFormDesigner::installRecursiveEventFilter(this, tree->eventEater());
}



//////////////////////////////////////////

KexiDBLineEdit::KexiDBLineEdit(QWidget *parent, const char *name)
 : KLineEdit(parent, name)
 , KexiFormDataItemInterface()
 , m_autonumberDisplayParameters(0)
{
	connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(slotTextChanged(const QString&)));
}

KexiDBLineEdit::~KexiDBLineEdit()
{
	delete m_autonumberDisplayParameters;
}

void KexiDBLineEdit::setInvalidState( const QString& displayText )
{
	setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	setText(displayText);
}

void KexiDBLineEdit::setValueInternal(const QVariant& add, bool removeOld)
{
	if (m_field && m_field->type()==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		setText( add.toBool() ? "1" : "0" );
	}
	else {
		if (removeOld)
			setText( add.toString() );
		else
			setText( m_origValue.toString() + add.toString() );
	}
}

QVariant KexiDBLineEdit::value()
{
	return text();
}

void KexiDBLineEdit::slotTextChanged(const QString&)
{
	signalValueChanged();
}

bool KexiDBLineEdit::valueIsNull()
{
	return text().isNull();
}

bool KexiDBLineEdit::valueIsEmpty()
{
	return text().isEmpty();
}

bool KexiDBLineEdit::isReadOnly() const
{
	return KLineEdit::isReadOnly();
}

QWidget* KexiDBLineEdit::widget()
{
	return this;
}

bool KexiDBLineEdit::cursorAtStart()
{
	return cursorPosition()==0;
}

bool KexiDBLineEdit::cursorAtEnd()
{
	return cursorPosition()==(int)text().length();
}

void KexiDBLineEdit::clear()
{
	setText(QString::null);
}

void KexiDBLineEdit::setField(KexiDB::Field* field)
{
	KexiFormDataItemInterface::setField(field);
	if (!field)
		return;
//! @todo merge this code with KexiTableEdit code!
//! @todo set maximum length validator
//! @todo handle input mask (via QLineEdit::setInputMask()
	const KexiDB::Field::Type t = field->type();
	if (field->isIntegerType()) {
		QValidator *validator = 0;
		const bool u = field->isUnsigned();
		int bottom, top;
		if (t==KexiDB::Field::Byte) {
			bottom = u ? 0 : -0x80;
			top = u ? 0xff : 0x7f;
		}
		else if (t==KexiDB::Field::ShortInteger) {
			bottom = u ? 0 : -0x8000;
			top = u ? 0xffff : 0x7fff;
		}
		else if (t==KexiDB::Field::Integer) {
			bottom = u ? 0 : -0x7fffffff-1;
			top = u ? 0xffffffff : 0x7fffffff;
		}
		else if (t==KexiDB::Field::BigInteger) {
/*! @todo couldn't work with KIntValidator: implement lonlong validator!
			bottom = u ? 0 : -0x7fffffffffffffff;
			top = u ? 0xffffffffffffffff : 127;*/
			validator = new KIntValidator(this);
		}

		if (!validator)
			validator = new KIntValidator(bottom, top, this);
		setValidator( validator );
	}
	else if (field->isFPNumericType()) {
		QValidator *validator;
		if (t==KexiDB::Field::Float) {
			if (field->isUnsigned()) //ok?
				validator = new KDoubleValidator(0, 3.4e+38, field->scale(), this);
			else
				validator = new KDoubleValidator(this);
		}
		else {//double
			if (field->isUnsigned()) //ok?
				validator = new KDoubleValidator(0, 1.7e+308, field->scale(), this);
			else
				validator = new KDoubleValidator(this);
		}
		setValidator( validator );
	}
	else if (t==KexiDB::Field::Date) {
//! @todo use KDateWidget
		QValidator *validator = new KDateValidator(this);
		setValidator( validator );
	}
	else if (t==KexiDB::Field::Time) {
//! @todo use KTimeWidget
		setInputMask("00:00:00");
	}
	else if (t==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		QValidator *validator = new KIntValidator(0, 1, this);
		setValidator( validator );
	}

	if (m_field->isAutoIncrement()) {
		if (!m_autonumberDisplayParameters)
			m_autonumberDisplayParameters = new KexiDisplayUtils::DisplayParameters();
		KexiDisplayUtils::initDisplayForAutonumberSign(*m_autonumberDisplayParameters, this);
	}
}

void KexiDBLineEdit::paintEvent ( QPaintEvent *pe )
{
	KLineEdit::paintEvent( pe );
	QPainter p(this);
	if (m_field && m_field->isAutoIncrement() && m_autonumberDisplayParameters 
		&& cursorAtNewRow() && text().isEmpty())
	{
		if (hasFocus()) {
			p.setPen(blendColors(m_autonumberDisplayParameters->textColor, palette().active().base(), 1, 3));
		}
		int m = lineWidth()+midLineWidth();
		KexiDisplayUtils::drawAutonumberSign(*m_autonumberDisplayParameters, &p, 
			2+m+margin(), m, width()-m*2 -2-2, height()-m*2 -2, alignment(), hasFocus());
	}
}

bool KexiDBLineEdit::event( QEvent * e )
{
	const bool ret = KLineEdit::event( e );
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {
		if (m_autonumberDisplayParameters && text().isEmpty())
			repaint();
	}
	return ret;
}

//////////////////////////////////////////

KexiPushButton::KexiPushButton( const QString & text, QWidget * parent, const char * name )
: KPushButton(text, parent, name)
{
}

KexiPushButton::~KexiPushButton()
{
}

//////////////////////////////////////////

KexiDBFactory::KexiDBFactory(QObject *parent, const char *name, const QStringList &)
 : KFormDesigner::WidgetFactory(parent, name)
{
//	KFormDesigner::WidgetInfo *wView = new KFormDesigner::WidgetInfo(this);
	KexiDataAwareWidgetInfo *wView = new KexiDataAwareWidgetInfo(this);
	wView->setPixmap("form");
	wView->setClassName("KexiDBForm");
	wView->setName(i18n("Database Form"));
	wView->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "form"));
	wView->setDescription(i18n("A data-aware form widget"));
	addClass(wView);
	
#ifndef KEXI_NO_SUBFORM
	KexiDataAwareWidgetInfo *wSubForm = new KexiDataAwareWidgetInfo(this);
	wSubForm->setPixmap("form");
	wSubForm->setClassName("KexiSubForm");
	wSubForm->setName(i18n("Sub Form"));
	wSubForm->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "subForm"));
	wSubForm->setDescription(i18n("A form widget included in another Form"));
	wSubForm->setAutoSyncForProperty( "formName", false );
	addClass(wSubForm);
#endif

//	KexiDataAwareWidgetInfo *wLineEdit = new KexiDataAwareWidgetInfo(this);
	// inherited
	KFormDesigner::WidgetInfo *wLineEdit = new KFormDesigner::WidgetInfo(
		this, "stdwidgets", "KLineEdit");
	wLineEdit->setPixmap("lineedit");
	wLineEdit->setClassName("KexiDBLineEdit");
	wLineEdit->addAlternateClassName("QLineEdit", true/*override*/);
	wLineEdit->addAlternateClassName("KLineEdit", true/*override*/);
	wLineEdit->setIncludeFileName("klineedit.h");
	wLineEdit->setName(i18n("Text Box"));
	wLineEdit->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "textBox"));
	wLineEdit->setDescription(i18n("A widget for entering and displaying text"));
	addClass(wLineEdit);

	KexiDataAwareWidgetInfo *wLabel = new KexiDataAwareWidgetInfo(
		this, "stdwidgets" /*we're inheriting to get i18n'd strings already translated there*/);
	wLabel->setPixmap("label");
	wLabel->setClassName("KexiLabel");
	wLabel->addAlternateClassName("QLabel", true/*override*/);
	wLabel->setIncludeFileName("qlabel.h");
	wLabel->setName(i18n("Label"));
	wLabel->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "label"));
	wLabel->setDescription(i18n("A widget for displaying text"));
	addClass(wLabel);

#ifndef KEXI_NO_KexiDBInputWidget
/*avoid i18n 
	KexiDataAwareWidgetInfo *wInput = new KexiDataAwareWidgetInfo(this);
	wInput->setPixmap("edit");
	wInput->setClassName("KexiDBInputWidget");
	wInput->addAlternateClassName("QLabel", true);
//todo	wInput->addAlternateClassName("QLineEdit", true);
//todo	wInput->addAlternateClassName("KLineEdit", true);
//todo	wInput->addAlternateClassName("KexiDBLineEdit", true); //for compat.
	wInput->setIncludeFileName("qlabel.h");
	wInput->setName(i18n("Input Widget"));
	wInput->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters", "inputWidget"));
//	wInput->setDescription(i18n("A super-duper widget"));
	addClass(wInput);*/
#endif

	// inherited
	KFormDesigner::WidgetInfo *wPushButton = new KFormDesigner::WidgetInfo(
		this, "stdwidgets", "KPushButton");
	wPushButton->addAlternateClassName("KexiPushButton");
	wPushButton->setName(i18n("Command Button"));
	wPushButton->setNamePrefix(
		i18n("Widget name. This string will be used to name widgets of this class. It must _not_ contain white spaces and non latin1 characters.", "button"));
	wPushButton->setDescription(i18n("A command button to execute actions"));
	addClass(wPushButton);

	m_propDesc["dataSource"] = i18n("Data Source");
	m_propDesc["formName"] = i18n("Form Name");
	m_propDesc["onClickAction"] = i18n("On Click");
	m_propDesc["autoTabStops"] = i18n("Auto Tab Stops");
	m_propDesc["shadowEnabled"] = i18n("Shadow Enabled");

#ifdef KEXI_NO_UNFINISHED
	//we don't want not-fully implemented/usable classes:
	hideClass("KexiPictureLabel");
	hideClass("KIntSpinBox");
	hideClass("KComboBox");
#endif
}

KexiDBFactory::~KexiDBFactory()
{
}

QWidget*
KexiDBFactory::create(const QCString &c, QWidget *p, const char *n, KFormDesigner::Container *container)
{
	kexipluginsdbg << "KexiDBFactory::create() " << this << endl;

	QWidget *w=0;
	QString text = container->form()->manager()->lib()->textForWidgetName(n, c);

	if(c == "KexiSubForm")
	{
		w = new KexiSubForm(container->form(), p, n);
	}
	else if(c == "KexiDBLineEdit")
	{
		w = new KexiDBLineEdit(p, n);
		w->setCursor(QCursor(Qt::ArrowCursor));
	}
	else if(c == "KexiLabel")
	{
		w = new KexiLabel(text, p, n);
	}
	else if(c == "KexiDBInputWidget") //todo: || c == "KexiDBLineEdit"/*for compatibility*/)
	{
		w = new KexiDBInputWidget(p, n);
	}
	else if(c == "KPushButton" || c == "KexiPushButton")
		w = new KexiPushButton(text, p, n);

	return w;
}

bool
KexiDBFactory::createMenuActions(const QCString &classname, QWidget *w, QPopupMenu *menu,
		   KFormDesigner::Container *)
{
	if(classname == "QPushButton" || classname == "KPushButton" || classname == "KexiPushButton")
	{
/*! @todo also call createMenuActions() for inherited factory! */
		m_assignAction->plug( menu );
		return true;
	}
	return false;
}

void
KexiDBFactory::createCustomActions(KActionCollection* col)
{
	//this will create shared instance action for design mode (special collection is provided)
	m_assignAction = new KAction( i18n("Assign Action..."), SmallIconSet("form_action"),
		0, 0, 0, col, "widget_assign_action");
}

/*KexiDBFactory::assignAction()
{
	emit executeCustomAction("assignAction", m_widget);
}*/

bool
KexiDBFactory::startEditing(const QCString &classname, QWidget *w, KFormDesigner::Container *container)
{
	m_container = container;
#ifndef Q_WS_WIN
	#warning Is there any reason to edit a lineedit in design-mode?
#endif
	if(classname == "KexiDBLineEdit")
	{
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
		KLineEdit *lineedit = static_cast<KLineEdit*>(w);
		createEditor(classname, lineedit->text(), lineedit, container, 
			lineedit->geometry(), lineedit->alignment(), true);
		return true;
	}
	else if ( classname == "KexiLabel" ) {
		KexiLabel *label = static_cast<KexiLabel*>(w);
		m_widget = w;
		if(label->textFormat() == RichText)
		{
			QString text = label->text();
			if ( editRichText( label, text ) )
			{
				changeProperty( "textFormat", "RichText", container );
				changeProperty( "text", text, container );
			}

			if ( classname == "KexiLabel" )
				w->resize(w->sizeHint());
		}
		else
		{
			createEditor(classname, label->text(), label, container, 
				label->geometry(), label->alignment());
		}
		return true;
	}
	else if (classname == "KexiSubForm") {
		// open the form in design mode
		KexiMainWindow *mainWin = Kexi::findParent<KexiMainWindow>(w, "KexiMainWindow");
		KexiSubForm *subform = static_cast<KexiSubForm*>(w);
		if(mainWin)
			mainWin->openObject("kexi/form", subform->formName(), Kexi::DesignViewMode);
	}

	return false;
}

bool
KexiDBFactory::previewWidget(const QCString &, QWidget *, KFormDesigner::Container *)
{
	return false;
}

bool
KexiDBFactory::clearWidgetContent(const QCString &classname, QWidget *w)
{
//! @todo this code should not be copied here but
//! just inherited StdWidgetFactory::clearWidgetContent() should be called
	if(classname == "KexiDBLineEdit")
		static_cast<KLineEdit*>(w)->clear();
	if(classname == "KexiLabel")
		static_cast<QLabel*>(w)->clear();
	else
		return false;
	return true;
}

QValueList<QCString>
KexiDBFactory::autoSaveProperties(const QCString &classname)
{
	QValueList<QCString> lst;
//	if(classname == "KexiSubForm")
		//lst << "formName";
//	if(classname == "KexiDBLineEdit")
//	lst += "dataSource";
	return lst;
}

bool 
KexiDBFactory::isPropertyVisibleInternal(const QCString& classname, QWidget *, 
	const QCString& property)
{
	if(classname == "KexiPushButton") {
		return property!="isDragEnabled" 
#ifdef KEXI_NO_UNFINISHED
			&& property!="onClickAction" /*! @todo reenable */
			&& property!="iconSet" /*! @todo reenable */
			&& property!="stdItem" /*! @todo reenable stdItem */
#endif
			;
	}
	else if(classname == "KexiDBLineEdit")
		return property!="urlDropsEnabled"
			&& property != "vAlign"
#ifdef KEXI_NO_UNFINISHED
			&& property!="inputMask"
			&& property!="maxLength" //!< we may want to integrate this with db schema
#endif
		;
	else if(classname == "KexiSubForm")
		return property!="dragAutoScroll"
			&& property!="resizePolicy"
			&& property!="focusPolicy";
	else if(classname == "KexiDBForm")
		return property!="iconText";
	else if(classname == "KexiLabel")
		return property!="focusPolicy";
	return true;
}


K_EXPORT_COMPONENT_FACTORY(kexidbwidgets, KGenericFactory<KexiDBFactory>("kexidbwidgets"))

#include "kexidbfactory.moc"
