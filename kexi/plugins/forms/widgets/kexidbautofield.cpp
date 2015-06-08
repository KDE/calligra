/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbautofield.h"
#include "kexidbcheckbox.h"
#include "kexidbimagebox.h"
#include "kexidblabel.h"
#include "kexidblineedit.h"
#include "kexidbtextedit.h"
#include "kexidbcombobox.h"
#include "KexiDBPushButton.h"
#include "kexidbform.h"
#include <kexi_global.h>
#include <formeditor/utils.h>
#include <kexiutils/utils.h>

#include <KDbQuerySchema>

#include <KLocalizedString>

#include <QLabel>
#include <QApplication>
#include <QDebug>

#define KexiDBAutoField_SPACING 10 //10 pixel for spacing between a label and an editor widget

//! @internal
class KexiDBAutoField::Private
{
public:
    Private() {
    }

    WidgetType widgetType; //!< internal: equal to m_widgetType_property or equal to result
                           //!< of widgetTypeForFieldType() if widgetTypeForFieldType is Auto
    WidgetType  widgetType_property; //!< provides widget type or Auto
    LabelPosition  lblPosition;
    QBoxLayout  *layout;
    QLabel  *label;
    QString  caption;
    KDbField::Type fieldTypeInternal;
    QString fieldCaptionInternal;
    QBrush baseBrush; //!< needed because for unbound mode editor==0
    QBrush textBrush; //!< needed because for unbound mode editor==0
    bool autoCaption;
    bool focusPolicyChanged;
};

//-------------------------------------

KexiDBAutoField::KexiDBAutoField(const QString &text, WidgetType type, LabelPosition pos,
                                 QWidget *parent)
        : QWidget(parent)
        , KexiFormDataItemInterface()
        , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
        , d(new Private())
{
    init(text, type, pos);
}

KexiDBAutoField::KexiDBAutoField(QWidget *parent, LabelPosition pos)
        : QWidget(parent)
        , KexiFormDataItemInterface()
        , KFormDesigner::DesignTimeDynamicChildWidgetHandler()
        , d(new Private())
{
    init(QString()/*xi18n("Auto Field")*/, Auto, pos);
}

KexiDBAutoField::~KexiDBAutoField()
{
    setUpdatesEnabled(false);
    if (subwidget())
        subwidget()->setUpdatesEnabled(false);
    delete d;
}

void
KexiDBAutoField::init(const QString &text, WidgetType type, LabelPosition pos)
{
    d->fieldTypeInternal = KDbField::InvalidType;
    d->layout = 0;
    setSubwidget(0);
    d->label = new QLabel(text, this);
    d->label->installEventFilter(this);
    d->autoCaption = true;
    d->focusPolicyChanged = false;
    d->widgetType = Auto;
    d->widgetType_property = (type == Auto ? Text : type); //to force "differ" to be true in setWidgetType()
    setLabelPosition(pos);
    setWidgetType(type);
    d->baseBrush = palette().base();
    d->textBrush = palette().text();
}

void
KexiDBAutoField::setWidgetType(WidgetType type)
{
    const bool differ = (type != d->widgetType_property);
    d->widgetType_property = type;
    if (differ) {
        if (type == Auto) {// try to guess type from data source type
            if (visibleColumnInfo())
                d->widgetType = KexiDBAutoField::widgetTypeForFieldType(visibleColumnInfo()->field->type());
            else
                d->widgetType = Auto;
        } else
            d->widgetType = d->widgetType_property;
        createEditor();
    }
}

void
KexiDBAutoField::createEditor()
{
    if (subwidget()) {
        delete(QWidget *)subwidget();
    }

    QWidget *newSubwidget;
    //qDebug() << "widgetType:" << d->widgetType;
    switch (d->widgetType) {
    case Text:
    case Double: //! @todo setup validator
    case Integer: //! @todo setup validator
    case Date:
    case Time:
    case DateTime: {
        KexiDBLineEdit *le = new KexiDBLineEdit(this);
        newSubwidget = le;
        le->setFrame(false);
        break;
    }
    case MultiLineText:
        newSubwidget = new KexiDBTextEdit(this);
        break;
    case Boolean:
        newSubwidget = new KexiDBCheckBox(dataSource(), this);
        break;
    case Image:
        newSubwidget = new KexiDBImageBox(designMode(), this);
        break;
    case ComboBox: {
        KexiDBComboBox *cbox = new KexiDBComboBox(this);
        newSubwidget = cbox;
        cbox->setDesignMode(designMode());
        break;
    }
    default:
        newSubwidget = 0;
        changeText(d->caption);
        break;
    }

    //qDebug() << newSubwidget;
    setSubwidget(newSubwidget);   //this will also allow to declare subproperties, see KFormDesigner::WidgetWithSubpropertiesInterface
    if (newSubwidget) {
        newSubwidget->setObjectName(
            QString::fromLatin1("KexiDBAutoField_") + newSubwidget->metaObject()->className());
        dynamic_cast<KexiDataItemInterface*>(newSubwidget)->setParentDataItemInterface(this);
        dynamic_cast<KexiFormDataItemInterface*>(newSubwidget)
            ->setColumnInfo(columnInfo()); //needed at least by KexiDBImageBox
        dynamic_cast<KexiFormDataItemInterface*>(newSubwidget)
            ->setVisibleColumnInfo(visibleColumnInfo()); //needed at least by KexiDBComboBox
        newSubwidget->setProperty("dataSource", dataSource()); //needed at least by KexiDBImageBox
        KFormDesigner::DesignTimeDynamicChildWidgetHandler::childWidgetAdded(this);
        newSubwidget->show();
        d->label->setBuddy(newSubwidget);
        if (d->focusPolicyChanged) {//if focusPolicy is changed at top level, editor inherits it
            newSubwidget->setFocusPolicy(focusPolicy());
        } else {//if focusPolicy is not changed at top level, inherit it from editor
            QWidget::setFocusPolicy(newSubwidget->focusPolicy());
        }
        setFocusProxy(newSubwidget); //ok?
        if (parentWidget())
            newSubwidget->setPalette(qApp->palette());
        copyPropertiesToEditor();
    }

    setLabelPosition(labelPosition());
}

void KexiDBAutoField::copyPropertiesToEditor()
{
    //qDebug() << subwidget();
    if (subwidget()) {
//  qDebug() << "base col: " <<  d->baseColor.name() <<
//   "; text col: " << d->textColor.name();
        QPalette p(subwidget()->palette());
        p.setBrush(QPalette::Base, d->baseBrush);
        if (d->widgetType == Boolean)
            p.setBrush(QPalette::Foreground, d->textBrush);
        else
            p.setBrush(QPalette::Text, d->textBrush);
        subwidget()->setPalette(p);
    }
}

void
KexiDBAutoField::setLabelPosition(LabelPosition position)
{
    d->lblPosition = position;
    if (d->layout) {
        QBoxLayout *lyr = d->layout;
        d->layout = 0;
        delete lyr;
    }

    if (subwidget())
        subwidget()->show();
    //! \todo support right-to-left layout where positions are inverted
    if (position == Top || position == Left) {
        Qt::Alignment align = d->label->alignment();
        if (position == Top) {
            d->layout = (QBoxLayout*) new QVBoxLayout(this);
            align |= Qt::AlignVertical_Mask;
            align ^= Qt::AlignVertical_Mask;
            align |= Qt::AlignTop;
        } else {
            d->layout = (QBoxLayout*) new QHBoxLayout(this);
            align |= Qt::AlignVertical_Mask;
            align ^= Qt::AlignVertical_Mask;
            align |= Qt::AlignVCenter;
        }
        d->label->setAlignment(align);
        if (d->widgetType == Boolean
                || (d->widgetType == Auto && fieldTypeInternal() == KDbField::InvalidType && !designMode())) {
            d->label->hide();
        } else {
            d->label->show();
        }
        d->layout->addWidget(d->label, 0, position == Top ? Qt::AlignLeft : QFlags<Qt::AlignmentFlag>(0));
        if (position == Left && d->widgetType != Boolean)
            d->layout->addSpacing(KexiDBAutoField_SPACING);
        d->layout->addWidget(subwidget(), 1);
        KexiSubwidgetInterface *subwidgetInterface = dynamic_cast<KexiSubwidgetInterface*>((QWidget*)subwidget());
        if (subwidgetInterface) {
            if (subwidgetInterface->appendStretchRequired(this))
                d->layout->addStretch(0);
            if (subwidgetInterface->subwidgetStretchRequired(this)) {
                QSizePolicy sizePolicy(subwidget()->sizePolicy());
                if (position == Left) {
                    sizePolicy.setHorizontalPolicy(QSizePolicy::Minimum);
                    d->label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
                } else {
                    sizePolicy.setVerticalPolicy(QSizePolicy::Minimum);
                    d->label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
                }
                subwidget()->setSizePolicy(sizePolicy);
            }
        }
    } else {
        d->layout = (QBoxLayout*) new QHBoxLayout(this);
        d->label->hide();
        d->layout->addWidget(subwidget());
    }
    //a hack to force layout to be refreshed (any better idea for this?)
    resize(size() + QSize(1, 0));
    resize(size() - QSize(1, 0));
    if (dynamic_cast<KexiDBAutoField*>((QWidget*)subwidget())) {
        //needed for KexiDBComboBox
        dynamic_cast<KexiDBAutoField*>((QWidget*)subwidget())->setLabelPosition(position);
    }
}

void
KexiDBAutoField::setInvalidState(const QString &text)
{
    // Widget with an invalid dataSource is just a QLabel
    if (designMode())
        return;
    d->widgetType = Auto;
    createEditor();
    setFocusPolicy(Qt::NoFocus);
    if (subwidget())
        subwidget()->setFocusPolicy(Qt::NoFocus);
//! @todo or set this to editor's text?
    d->label->setText(text);
}

bool
KexiDBAutoField::isReadOnly() const
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->isReadOnly();
    else
        return false;
}

void
KexiDBAutoField::setReadOnly(bool readOnly)
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->setReadOnly(readOnly);
}

void
KexiDBAutoField::setValueInternal(const QVariant& add, bool removeOld)
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->setValue(KexiDataItemInterface::originalValue(), add, removeOld);
}

QVariant
KexiDBAutoField::value()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->value();
    return QVariant();
}

bool
KexiDBAutoField::valueIsNull()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->valueIsNull();
    return true;
}

bool
KexiDBAutoField::valueIsEmpty()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->valueIsEmpty();
    return true;
}

bool
KexiDBAutoField::valueIsValid()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->valueIsValid();
    return true;
}

bool
KexiDBAutoField::valueChanged()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    //qDebug() << KexiDataItemInterface::originalValue();
    if (iface)
        return iface->valueChanged();
    return false;
}

void
KexiDBAutoField::installListener(KexiDataItemChangesListener* listener)
{
    KexiFormDataItemInterface::installListener(listener);
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->installListener(listener);
}

KexiDBAutoField::WidgetType KexiDBAutoField::widgetType() const
{
    return d->widgetType_property;
}

KexiDBAutoField::LabelPosition KexiDBAutoField::labelPosition() const
{
    return d->lblPosition;
}

QString KexiDBAutoField::caption() const
{
    return d->caption;
}

bool KexiDBAutoField::hasAutoCaption() const
{
    return d->autoCaption;
}

QWidget* KexiDBAutoField::editor() const
{
    return subwidget();
}

QLabel* KexiDBAutoField::label() const
{
    return d->label;
}

int KexiDBAutoField::fieldTypeInternal() const
{
    return d->fieldTypeInternal;
}

QString KexiDBAutoField::fieldCaptionInternal() const
{
    return d->fieldCaptionInternal;
}

bool
KexiDBAutoField::cursorAtStart()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->cursorAtStart();
    return false;
}

bool
KexiDBAutoField::cursorAtEnd()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        return iface->cursorAtEnd();
    return false;
}

void
KexiDBAutoField::clear()
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->clear();
}

void
KexiDBAutoField::setFieldTypeInternal(int kexiDBFieldType)
{
    d->fieldTypeInternal = (KDbField::Type)kexiDBFieldType;
    KDbField::Type fieldType;
    //find real fied type to use
    if (d->fieldTypeInternal == KDbField::InvalidType) {
        if (visibleColumnInfo())
            fieldType = KDbField::Text;
        else
            fieldType = KDbField::InvalidType;
    } else
        fieldType = d->fieldTypeInternal;

    const WidgetType newWidgetType = KexiDBAutoField::widgetTypeForFieldType(fieldType);

    if (d->widgetType != newWidgetType) {
        d->widgetType = newWidgetType;
        createEditor();
    }
    setFieldCaptionInternal(d->fieldCaptionInternal);
}

void
KexiDBAutoField::setFieldCaptionInternal(const QString& text)
{
    d->fieldCaptionInternal = text;
    //change text only if autocaption is set and no columnInfo is available
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if ((!iface || !iface->columnInfo()) && d->autoCaption) {
        changeText(d->fieldCaptionInternal);
    }
}

void
KexiDBAutoField::setColumnInfo(KDbQueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    setColumnInfoInternal(cinfo, cinfo);
}

void
KexiDBAutoField::setColumnInfoInternal(KDbQueryColumnInfo* cinfo, KDbQueryColumnInfo* visibleColumnInfo)
{
    // change widget type depending on field type
    if (d->widgetType_property == Auto) {
        WidgetType newWidgetType = Auto;
        KDbField::Type fieldType;
        if (cinfo)
            fieldType = visibleColumnInfo->field->type();
        else if (dataSource().isEmpty())
            fieldType = KDbField::InvalidType;
        else
            fieldType = KDbField::Text;

        if (fieldType != KDbField::InvalidType) {
            newWidgetType = KexiDBAutoField::widgetTypeForFieldType(fieldType);
        }
        if (d->widgetType != newWidgetType || newWidgetType == Auto) {
            d->widgetType = newWidgetType;
            createEditor();
        }
    }
    // update label's text
    changeText((cinfo && d->autoCaption) ? cinfo->captionOrAliasOrName() : d->caption);

    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->setColumnInfo(visibleColumnInfo);
}

//static
KexiDBAutoField::WidgetType
KexiDBAutoField::widgetTypeForFieldType(KDbField::Type type)
{
    switch (type) {
    case KDbField::Integer:
    case KDbField::ShortInteger:
    case KDbField::BigInteger:
        return Integer;
    case  KDbField::Boolean:
        return Boolean;
    case KDbField::Float:
    case KDbField::Double:
        return Double;
    case KDbField::Date:
        return Date;
    case KDbField::DateTime:
        return DateTime;
    case KDbField::Time:
        return Time;
    case KDbField::Text:
        return Text;
    case KDbField::LongText:
        return MultiLineText;
    case KDbField::Enum:
        return ComboBox;
    case KDbField::InvalidType:
        return Auto;
    case KDbField::BLOB:
        return Image;
    default:
        break;
    }
    return Text;
}

void
KexiDBAutoField::changeText(const QString &text, bool beautify)
{
    QString realText;
    bool unbound = false;
    if (d->autoCaption && (d->widgetType == Auto || dataSource().isEmpty())) {
        if (designMode())
            realText = futureI18nc2("Unbound Auto Field", "%1 (unbound)", objectName());
        else
            realText.clear();
        unbound = true;
    } else {
        if (beautify) {
            /*! @todo look at appendColonToAutoLabels setting [bool]
              @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
              (see doc/dev/settings.txt) */
            if (!text.isEmpty()) {
                realText = text[0].toUpper() + text.mid(1);
                if (d->widgetType != Boolean) {
//! @todo ":" suffix looks weird for checkbox; remove this condition when [x] is displayed _after_ label
//! @todo support right-to-left layout where position of ":" is inverted
                    realText += ": ";
                }
            }
        } else
            realText = text;
    }

    if (unbound) {
        d->label->setAlignment(Qt::AlignCenter);
        d->label->setWordWrap(true);
    } else {
        d->label->setAlignment(Qt::AlignCenter);
    }
    if (d->widgetType == Boolean) {
        static_cast<QCheckBox*>((QWidget*)subwidget())->setText(realText);
    } else {
        d->label->setText(realText);
    }
}

void
KexiDBAutoField::setCaption(const QString &caption)
{
    d->caption = caption;
    if (!d->autoCaption && !caption.isEmpty())
        changeText(d->caption);
}

void
KexiDBAutoField::setAutoCaption(bool autoCaption)
{
    d->autoCaption = autoCaption;
    if (d->autoCaption) {
        if (columnInfo()) {
            changeText(columnInfo()->captionOrAliasOrName());
        } else {
            changeText(d->fieldCaptionInternal);
        }
    } else
        changeText(d->caption);
}

void
KexiDBAutoField::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    if (ds.isEmpty()) {
        setColumnInfo(0);
    }
}

QSize
KexiDBAutoField::sizeHint() const
{
    if (d->lblPosition == NoLabel)
        return subwidget() ? subwidget()->sizeHint() : QWidget::sizeHint();

    QSize s1(0, 0);
    if (subwidget())
        s1 = subwidget()->sizeHint();
    QSize s2(d->label->sizeHint());
    if (d->lblPosition == Top)
        return QSize(qMax(s1.width(), s2.width()), s1.height() + KexiDBAutoField_SPACING + s2.height());

    //left
    return QSize(s1.width() + KexiDBAutoField_SPACING + s2.width(), qMax(s1.height(), s2.height()));
}

void
KexiDBAutoField::setFocusPolicy(Qt::FocusPolicy policy)
{
    d->focusPolicyChanged = true;
    QWidget::setFocusPolicy(policy);
    d->label->setFocusPolicy(policy);
    if (subwidget())
        subwidget()->setFocusPolicy(policy);
}

void
KexiDBAutoField::updateInformationAboutUnboundField()
{
    if ((d->autoCaption && (dataSource().isEmpty() || dataSourcePartClass().isEmpty()))
            || (!d->autoCaption && d->caption.isEmpty())) {
        d->label->setText(futureI18nc2("Unbound Auto Field", "%1 (unbound)", objectName()));
    }
}

void
KexiDBAutoField::paletteChange(const QPalette& oldPal)
{
    Q_UNUSED(oldPal);
    d->label->setPalette(palette());
}

void KexiDBAutoField::unsetPalette()
{
    setPalette(QPalette());
}

// ===== methods below are just proxies for the internal editor or label =====

QColor KexiDBAutoField::paletteForegroundColor() const
{
//! @todo how about brush?
    return d->textBrush.color();
}

void KexiDBAutoField::setPaletteForegroundColor(const QColor & color)
{
//! @todo how about brush?
    d->textBrush.setColor(color);
    copyPropertiesToEditor();
}

QColor KexiDBAutoField::paletteBackgroundColor() const
{
//! @todo how about brush?
    return d->baseBrush.color();
}

void KexiDBAutoField::setPaletteBackgroundColor(const QColor & color)
{
    //qDebug();
//! @todo how about brush?
    d->baseBrush.setColor(color);
    copyPropertiesToEditor();
}

QColor KexiDBAutoField::foregroundLabelColor() const
{
    if (d->widgetType == Boolean)
        return paletteForegroundColor();

    return d->label->palette().color(d->label->foregroundRole());
}

void KexiDBAutoField::setForegroundLabelColor(const QColor & color)
{
    if (d->widgetType == Boolean)
        setPaletteForegroundColor(color);
    else {
        QPalette pal(d->label->palette());
        pal.setColor(d->label->foregroundRole(), color);
        d->label->setPalette(pal);
        pal = palette();
        pal.setColor(foregroundRole(), color);
        setPalette(pal);
    }
}

QColor KexiDBAutoField::backgroundLabelColor() const
{
    if (d->widgetType == Boolean)
        return paletteBackgroundColor();

    return d->label->palette().color(d->label->backgroundRole());
}

void KexiDBAutoField::setBackgroundLabelColor(const QColor & color)
{
    if (d->widgetType == Boolean)
        setPaletteBackgroundColor(color);
    else {
        QPalette pal(d->label->palette());
        pal.setColor(d->label->backgroundRole(), color);
        d->label->setPalette(pal);
        pal = palette();
        pal.setColor(backgroundRole(), color);
        setPalette(pal);
    }
}

QVariant KexiDBAutoField::property(const char * name) const
{
    bool ok;
    QVariant val = KFormDesigner::WidgetWithSubpropertiesInterface::subproperty(name, ok);
    if (ok)
        return val;
    return QWidget::property(name);
}

bool KexiDBAutoField::setProperty(const char * name, const QVariant & value)
{
    bool ok = KFormDesigner::WidgetWithSubpropertiesInterface::setSubproperty(name, value);
    if (ok)
        return true;
    return QWidget::setProperty(name, value);
}

bool KexiDBAutoField::eventFilter(QObject *o, QEvent *e)
{
    if (o == d->label && d->label->buddy() && e->type() == QEvent::MouseButtonRelease) {
        //focus label's buddy when user clicked the label
        d->label->buddy()->setFocus();
    }
    return QWidget::eventFilter(o, e);
}

void KexiDBAutoField::setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue)
{
    KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
    if (dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget()))
        dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget())->setDisplayDefaultValue(subwidget(), displayDefaultValue);
}

void KexiDBAutoField::moveCursorToEnd()
{
    KexiDataItemInterface *iface = dynamic_cast<KexiDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->moveCursorToEnd();
}

void KexiDBAutoField::moveCursorToStart()
{
    KexiDataItemInterface *iface = dynamic_cast<KexiDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->moveCursorToStart();
}

void KexiDBAutoField::selectAll()
{
    KexiDataItemInterface *iface = dynamic_cast<KexiDataItemInterface*>((QWidget*)subwidget());
    if (iface)
        iface->selectAll();
}

bool KexiDBAutoField::keyPressed(QKeyEvent *ke)
{
    KexiFormDataItemInterface *iface = dynamic_cast<KexiFormDataItemInterface*>((QWidget*)subwidget());
    if (iface && iface->keyPressed(ke))
        return true;
    return false;
}

