/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 * Copyright (C) 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * Copyright (C) 2010 Nandita Suri <suri.nandita@gmail.com>
 * Copyright (C) 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>
 * Copyright (C) 2011-2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 * Copyright (C) 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoListLevelProperties.h"

#include "KoTextSharedLoadingData.h"
#include "Styles_p.h"

#include <float.h>

#include "TextDebug.h"

#include <KoXmlNS.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlWriter.h>
#include <KoUnit.h>
#include <KoText.h>
#include <KoImageCollection.h>
#include <KoImageData.h>
#include <KoImageData_p.h>
#include <KoOdfNumberDefinition.h>
#include <KoGenStyle.h>
#include <KoTextSharedSavingData.h>

#include <QTextList>

class Q_DECL_HIDDEN KoListLevelProperties::Private
{
public:
    StylePrivate stylesPrivate;

    void copy(Private *other) {
        stylesPrivate = other->stylesPrivate;
    }
};

KoListLevelProperties::KoListLevelProperties()
        : QObject()
        , d(new Private())
{
    QSharedPointer<KoCharacterStyle> charStyle(new KoCharacterStyle);
    setCharacterProperties(charStyle);

    setRelativeBulletSize(100);
    setAlignmentMode(true);
    setDisplayLevel(1);
    connect(this,SIGNAL(styleChanged(int)),SLOT(onStyleChanged(int)));
}

KoListLevelProperties::KoListLevelProperties(const KoListLevelProperties &other)
        : QObject()
        , d(new Private())
{
    d->copy(other.d);
    connect(this,SIGNAL(styleChanged(int)),SLOT(onStyleChanged(int)));
}

KoListLevelProperties::~KoListLevelProperties()
{
    delete d;
}

int KoListLevelProperties::styleId() const
{
    return propertyInt(KoListStyle::StyleId);
}

void KoListLevelProperties::setStyleId(int id)
{
    setProperty(KoListStyle::StyleId, id);
}

void KoListLevelProperties::setProperty(int key, const QVariant &value)
{
    d->stylesPrivate.add(key, value);
}

int KoListLevelProperties::propertyInt(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return 0;
    return variant.toInt();
}

uint KoListLevelProperties::propertyUInt(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return 0;
    return variant.toUInt();
}

qulonglong KoListLevelProperties::propertyULongLong(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return 0;
    return variant.toULongLong();
}

qreal KoListLevelProperties::propertyDouble(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return 0.;
    return variant.toDouble();
}

bool KoListLevelProperties::propertyBoolean(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return false;
    return variant.toBool();
}

QString KoListLevelProperties::propertyString(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return QString();
    return qvariant_cast<QString>(variant);
}

QColor KoListLevelProperties::propertyColor(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (variant.isNull())
        return QColor(Qt::black);
    return qvariant_cast<QColor>(variant);
}

QVariant KoListLevelProperties::property(int key) const
{
    QVariant variant = d->stylesPrivate.value(key);
    if (!variant.isNull()) {
        return variant;
    } else {
        return QVariant();
    }
}

void KoListLevelProperties::applyStyle(QTextListFormat &format) const
{
    QList<int> keys = d->stylesPrivate.keys();
    for (int i = 0; i < keys.count(); i++) {
        QVariant variant = d->stylesPrivate.value(keys[i]);
        format.setProperty(keys[i], variant);
    }
}

bool KoListLevelProperties::operator==(const KoListLevelProperties &other) const
{
    return d->stylesPrivate == other.d->stylesPrivate;
}

bool KoListLevelProperties::operator!=(const KoListLevelProperties &other) const
{
    return d->stylesPrivate != other.d->stylesPrivate;
}

void KoListLevelProperties::setListItemPrefix(const QString &prefix)
{
    setProperty(KoListStyle::ListItemPrefix, prefix);
}

QString KoListLevelProperties::listItemPrefix() const
{
    return propertyString(KoListStyle::ListItemPrefix);
}

void KoListLevelProperties::setLabelType(KoListStyle::LabelType labelType)
{
    setProperty(QTextListFormat::ListStyle, (int) labelType);
    emit styleChanged(labelType);
}

KoListStyle::LabelType KoListLevelProperties::labelType() const
{
    return static_cast<KoListStyle::LabelType>(propertyInt(QTextListFormat::ListStyle));
}

void KoListLevelProperties::setNumberFormat(KoOdfNumberDefinition::FormatSpecification numberFormat)
{
    setProperty(KoListStyle::NumberFormat, numberFormat);
}

KoOdfNumberDefinition::FormatSpecification KoListLevelProperties::numberFormat() const
{
    return static_cast<KoOdfNumberDefinition::FormatSpecification>(propertyInt(KoListStyle::NumberFormat));
}

void KoListLevelProperties::setListItemSuffix(const QString &suffix)
{
    setProperty(KoListStyle::ListItemSuffix, suffix);
}

QString KoListLevelProperties::listItemSuffix() const
{
    return propertyString(KoListStyle::ListItemSuffix);
}

void KoListLevelProperties::setStartValue(int value)
{
    setProperty(KoListStyle::StartValue, value);
}

int KoListLevelProperties::startValue() const
{
    return propertyInt(KoListStyle::StartValue);
}

void KoListLevelProperties::setLevel(int value)
{
    setProperty(KoListStyle::Level, value);
}

int KoListLevelProperties::level() const
{
    return propertyInt(KoListStyle::Level);
}

void KoListLevelProperties::setDisplayLevel(int level)
{
    setProperty(KoListStyle::DisplayLevel, level);
}

int KoListLevelProperties::displayLevel() const
{
    return propertyInt(KoListStyle::DisplayLevel);
}

void KoListLevelProperties::setCharacterStyleId(int id)
{
    setProperty(KoListStyle::CharacterStyleId, id);
}

int KoListLevelProperties::characterStyleId() const
{
    return propertyInt(KoListStyle::CharacterStyleId);
}

void KoListLevelProperties::setCharacterProperties(QSharedPointer< KoCharacterStyle > style)
{
    setProperty(KoListStyle::CharacterProperties, QVariant::fromValue< QSharedPointer<KoCharacterStyle> >(style));
}

QSharedPointer<KoCharacterStyle> KoListLevelProperties::characterProperties() const
{
    const QVariant v = d->stylesPrivate.value(KoListStyle::CharacterProperties);
    if (v.isNull()) {
        return QSharedPointer<KoCharacterStyle>(0);
    }
    return v.value< QSharedPointer<KoCharacterStyle> >();
}

void KoListLevelProperties::setBulletCharacter(QChar character)
{
    setProperty(KoListStyle::BulletCharacter, (int) character.unicode());
}

QChar KoListLevelProperties::bulletCharacter() const
{
    return propertyInt(KoListStyle::BulletCharacter);
}

void KoListLevelProperties::setRelativeBulletSize(int percent)
{
    setProperty(KoListStyle::RelativeBulletSize, percent);
}

int KoListLevelProperties::relativeBulletSize() const
{
    return propertyInt(KoListStyle::RelativeBulletSize);
}

void KoListLevelProperties::setAlignment(Qt::Alignment align)
{
    setProperty(KoListStyle::Alignment, static_cast<int>(align));
}

Qt::Alignment KoListLevelProperties::alignment() const
{
    return static_cast<Qt::Alignment>(propertyInt(KoListStyle::Alignment));
}

void KoListLevelProperties::setMinimumWidth(qreal width)
{
    setProperty(KoListStyle::MinimumWidth, width);
}

qreal KoListLevelProperties::minimumWidth() const
{
    return propertyDouble(KoListStyle::MinimumWidth);
}

void KoListLevelProperties::setWidth(qreal width)
{
    setProperty(KoListStyle::Width, width);
}

qreal KoListLevelProperties::width() const
{
    return propertyDouble(KoListStyle::Width);
}

void KoListLevelProperties::setHeight(qreal height)
{
    setProperty(KoListStyle::Height, height);
}

qreal KoListLevelProperties::height() const
{
    return propertyDouble(KoListStyle::Height);
}

void KoListLevelProperties::setBulletImage(KoImageData *imageData)
{
    setProperty(KoListStyle::BulletImage, QVariant::fromValue(imageData));
}

KoImageData *KoListLevelProperties::bulletImage() const
{
    return property(KoListStyle::BulletImage).value< KoImageData * >();
}

KoListLevelProperties & KoListLevelProperties::operator=(const KoListLevelProperties & other)
{
    d->copy(other.d);
    return *this;
}

void KoListLevelProperties::setListId(KoListStyle::ListIdType listId)
{
    setProperty(KoListStyle::ListId, listId);
}

KoListStyle::ListIdType KoListLevelProperties::listId() const
{
    if (sizeof(KoListStyle::ListIdType) == sizeof(uint))
        return propertyUInt(KoListStyle::ListId);
    else
        return propertyULongLong(KoListStyle::ListId);
}

bool KoListLevelProperties::letterSynchronization() const
{
    return propertyBoolean(KoListStyle::LetterSynchronization);
}

void KoListLevelProperties::setLetterSynchronization(bool on)
{
    setProperty(KoListStyle::LetterSynchronization, on);
}

void KoListLevelProperties::setIndent(qreal value)
{
    setProperty(KoListStyle::Indent, value);
}

qreal KoListLevelProperties::indent() const
{
    return propertyDouble(KoListStyle::Indent);
}

void KoListLevelProperties::setMinimumDistance(qreal value)
{
    setProperty(KoListStyle::MinimumDistance, value);
}

qreal KoListLevelProperties::minimumDistance() const
{
    return propertyDouble(KoListStyle::MinimumDistance);
}

void KoListLevelProperties::setMargin(qreal value)
{
    setProperty(KoListStyle::Margin, value);
}

qreal KoListLevelProperties::margin() const
{
    return propertyDouble(KoListStyle::Margin);
}

void KoListLevelProperties::setMarginIncrease(qreal value)
{
    setProperty(KoListStyle::MarginIncrease, value);
}

qreal KoListLevelProperties::marginIncrease() const
{
    if(d->stylesPrivate.contains(KoListStyle::MarginIncrease))
        return propertyDouble(KoListStyle::MarginIncrease);
    else
        return 18; // market default it seems
}

void KoListLevelProperties::setTextIndent(qreal value)
{
    setProperty(KoListStyle::TextIndent, value);
}

qreal KoListLevelProperties::textIndent() const
{
    return propertyDouble(KoListStyle::TextIndent);
}

void KoListLevelProperties::setAlignmentMode(bool isLabelAlignmentMode)
{
    setProperty(KoListStyle::AlignmentMode, isLabelAlignmentMode);
}

bool KoListLevelProperties::alignmentMode() const
{
    return propertyBoolean(KoListStyle::AlignmentMode);
}

void KoListLevelProperties::setTabStopPosition(qreal value)
{
    setProperty(KoListStyle::TabStopPosition,value);
}

bool KoListLevelProperties::hasTabStopPosition() const
{
    return d->stylesPrivate.contains(KoListStyle::TabStopPosition);
}

void KoListLevelProperties::clearTabStopPosition()
{
    d->stylesPrivate.remove(KoListStyle::TabStopPosition);
}

qreal KoListLevelProperties::tabStopPosition() const
{
    return propertyDouble(KoListStyle::TabStopPosition);
}

void KoListLevelProperties::setLabelFollowedBy(KoListStyle::ListLabelFollowedBy value)
{
    setProperty(KoListStyle::LabelFollowedBy, value);
}

KoListStyle::ListLabelFollowedBy KoListLevelProperties::labelFollowedBy() const
{
    return (KoListStyle::ListLabelFollowedBy)propertyInt(KoListStyle::LabelFollowedBy);
}

void KoListLevelProperties::setOutlineList(bool isOutline)
{
    setProperty(KoListStyle::IsOutline, isOutline);
}

bool KoListLevelProperties::isOutlineList() const
{
    return propertyBoolean(KoListStyle::IsOutline);
}

// static
KoListLevelProperties KoListLevelProperties::fromTextList(QTextList *list)
{
    KoListLevelProperties llp;
    if (!list) {
        llp.setLabelType(KoListStyle::None);
        return llp;
    }
    llp.d->stylesPrivate = list->format().properties();
    return llp;
}

void KoListLevelProperties::onStyleChanged(int key)
{
    //for numbered list the relative bullet size is made 100
    if (KoListStyle::isNumberingStyle(key)) {
        setRelativeBulletSize(100);
    }
}

void KoListLevelProperties::loadOdf(KoShapeLoadingContext& scontext, const KoXmlElement& style)
{
    KoOdfLoadingContext &context = scontext.odfLoadingContext();

    // The text:level attribute specifies the level of the number list
    // style. It can be used on all list-level styles.
    const int level = qMax(1, style.attributeNS(KoXmlNS::text, "level", QString()).toInt());
    // The text:display-levels attribute specifies the number of
    // levels whose numbers are displayed at the current level.
    const QString displayLevel = style.attributeNS(KoXmlNS::text,
                                 "display-levels", QString());

    const QString styleName = style.attributeNS(KoXmlNS::text, "style-name", QString());
    KoCharacterStyle *cs = 0;
    if (!styleName.isEmpty()) {
//         debugText << "Should use the style =>" << styleName << "<=";

        KoSharedLoadingData *sharedData = scontext.sharedData(KOTEXT_SHARED_LOADING_ID);
        KoTextSharedLoadingData *textSharedData = 0;
        if (sharedData) {
            textSharedData = dynamic_cast<KoTextSharedLoadingData *>(sharedData);
        }
        if (textSharedData) {
            cs = textSharedData->characterStyle(styleName, context.useStylesAutoStyles());
            if (!cs) {
               warnText << "Missing KoCharacterStyle!";
            }
            else {
//                debugText << "==> cs.name:" << cs->name();
//                debugText << "==> cs.styleId:" << cs->styleId();
                setCharacterStyleId(cs->styleId());
            }
        }
    }

    if (style.localName() == "list-level-style-bullet") {   // list with bullets
        // special case bullets:
        //debugText << QChar(0x2202) << QChar(0x25CF) << QChar(0xF0B7) << QChar(0xE00C)
        //<< QChar(0xE00A) << QChar(0x27A2)<< QChar(0x2794) << QChar(0x2714) << QChar(0x2d) << QChar(0x2717);

        //1.6: KoParagCounter::loadOasisListStyle
        QString bulletChar = style.attributeNS(KoXmlNS::text, "bullet-char", QString());
        if (bulletChar.isEmpty()) {  // list without any visible bullets
            setBulletCharacter(QChar());
        } else {
            setBulletCharacter(bulletChar[0]);
        }
        setLabelType(KoListStyle::BulletCharLabelType);
        QString size = style.attributeNS(KoXmlNS::text, "bullet-relative-size", QString());
        if (!size.isEmpty()) {
            setRelativeBulletSize(size.remove('%').toInt());
        }

    } else if (style.localName() == "list-level-style-number" || style.localName() == "outline-level-style") { // it's a numbered list

        if (style.localName() == "outline-level-style") {
            setOutlineList(true);
        }
        setRelativeBulletSize(100); //arbitrary value for numbered list

        KoOdfNumberDefinition numberDefinition;
        numberDefinition.loadOdf(style);
        setNumberFormat(numberDefinition.formatSpecification());

        if (numberDefinition.formatSpecification() == KoOdfNumberDefinition::Empty) {
            setLabelType(KoListStyle::None);
        } else {
            setLabelType(KoListStyle::NumberLabelType);
        }

        if (!numberDefinition.prefix().isNull()) {
            setListItemPrefix(numberDefinition.prefix());
        }

        if (!numberDefinition.suffix().isNull()) {
            setListItemSuffix(numberDefinition.suffix());
        }
        const QString startValue = style.attributeNS(KoXmlNS::text, "start-value", QString("1"));
        setStartValue(startValue.toInt());
    }
    else if (style.localName() == "list-level-style-image") {   // list with image
        setLabelType(KoListStyle::ImageLabelType);
        KoImageCollection *imageCollection = scontext.imageCollection();
        const QString href = style.attribute("href");
        if(imageCollection) {
            if (!href.isEmpty()) {
                KoStore *store = context.store();
                setBulletImage(imageCollection->createImageData(href, store));
            } else {
                // check if we have an office:binary data element containing the image data
                const KoXmlElement &binaryData(KoXml::namedItemNS(style, KoXmlNS::office, "binary-data"));
                if (!binaryData.isNull()) {
                    QImage image;
                    if (image.loadFromData(QByteArray::fromBase64(binaryData.text().toLatin1()))) {
                        setBulletImage(imageCollection->createImageData(image));
                    }
                }
            }
        }
    }
    else { // if not defined, we can do nothing
//         debugText << "stylename else:" << style.localName() << "level=" << level << "displayLevel=" << displayLevel;
        setLabelType(KoListStyle::NumberLabelType);
        setListItemSuffix(".");
    }

    setLevel(level);
    if (!displayLevel.isEmpty())
        setDisplayLevel(displayLevel.toInt());

    KoXmlElement property;
    forEachElement(property, style) {
        if (property.namespaceURI() != KoXmlNS::style)
            continue;
        const QString localName = property.localName();
        if (localName == "list-level-properties") {
            QString mode(property.attributeNS(KoXmlNS::text, "list-level-position-and-space-mode", "label-width-and-position"));
            if (mode == "label-alignment") {
                QString textAlign(property.attributeNS(KoXmlNS::fo, "text-align"));
                setAlignment(textAlign.isEmpty() ? Qt::AlignLeft : KoText::alignmentFromString(textAlign));

                KoXmlElement p;
                forEachElement(p, property) {
                     if (p.namespaceURI() == KoXmlNS::style && p.localName() == "list-level-label-alignment") {
                        // The <style:list-level-label-alignment> element and the fo:text-align attribute are used to define
                        // the position and spacing of the list label and the list item. The values of the attributes for
                        // text:space-before, text:min-label-width and text:min-label-distance are assumed to be 0.
                        setAlignmentMode(true);

                        QString textindent(p.attributeNS(KoXmlNS::fo, "text-indent"));
                        QString marginleft(p.attributeNS(KoXmlNS::fo, "margin-left"));
                        qreal ti = textindent.isEmpty() ? 0 : KoUnit::parseValue(textindent);
                        qreal ml = marginleft.isEmpty() ? 0 : KoUnit::parseValue(marginleft);
                        setTextIndent(ti);
                        setMargin(ml);

                        QString labelFollowedBy(p.attributeNS(KoXmlNS::text, "label-followed-by","space"));
                        if(labelFollowedBy.compare("listtab",Qt::CaseInsensitive)==0) {

                            setLabelFollowedBy(KoListStyle::ListTab);

                            // list tab position is evaluated only if label is followed by listtab
                            // the it is only evaluated if there is a list-tab-stop-position specified
                            // if not specified use the fo:margin-left:
                            QString tabStop(p.attributeNS(KoXmlNS::text, "list-tab-stop-position"));
                            if (!tabStop.isEmpty()) {
                                qreal tabStopPos = KoUnit::parseValue(tabStop);
                                setTabStopPosition(qMax<qreal>(0.0, tabStopPos));
                            }

                        }else if(labelFollowedBy.compare("nothing",Qt::CaseInsensitive)==0) {

                            setLabelFollowedBy(KoListStyle::Nothing);

                        }else {

                            setLabelFollowedBy(KoListStyle::Space);

                        }

                        setMinimumWidth(0);
                        setMinimumDistance(0);

                        //TODO support ODF 18.829 text:label-followed-by and 18.832 text:list-tab-stop-position
                     }
                }
            } else { // default is mode == "label-width-and-position"
                // The text:space-before, text:min-label-width, text:minimum-label-distance and fo:text-align attributes
                // are used to define the position and spacing of the list label and the list item.

                setAlignmentMode(false);

                QString spaceBefore(property.attributeNS(KoXmlNS::text, "space-before"));
                if (!spaceBefore.isEmpty())
                    setIndent(KoUnit::parseValue(spaceBefore));

                QString minLableWidth(property.attributeNS(KoXmlNS::text, "min-label-width"));
                if (!minLableWidth.isEmpty())
                    setMinimumWidth(KoUnit::parseValue(minLableWidth));

                QString textAlign(property.attributeNS(KoXmlNS::fo, "text-align"));
                if (!textAlign.isEmpty())
                    setAlignment(KoText::alignmentFromString(textAlign));

                QString minLableDistance(property.attributeNS(KoXmlNS::text, "min-label-distance"));
                if (!minLableDistance.isEmpty())
                    setMinimumDistance(KoUnit::parseValue(minLableDistance));               
            }

            QString width(property.attributeNS(KoXmlNS::fo, "width"));
            if (!width.isEmpty())
                setWidth(KoUnit::parseValue(width));

            QString height(property.attributeNS(KoXmlNS::fo, "height"));
            if (!height.isEmpty())
                setHeight(KoUnit::parseValue(height));

        } else if (localName == "text-properties") {
            QSharedPointer<KoCharacterStyle> charStyle(new KoCharacterStyle);
            charStyle->loadOdf(&style, scontext);
            setCharacterProperties(charStyle);
        }
    }
}

void KoListLevelProperties::saveOdf(KoXmlWriter *writer, KoShapeSavingContext &context) const
{
    if (labelType() == KoListStyle::NumberLabelType || isOutlineList()) {
        if (isOutlineList()) {
            writer->startElement("text:outline-level-style"); } else {
            writer->startElement("text:list-level-style-number");
        }

        if (d->stylesPrivate.contains(KoListStyle::StartValue))
            writer->addAttribute("text:start-value", d->stylesPrivate.value(KoListStyle::StartValue).toInt());
        if (d->stylesPrivate.contains(KoListStyle::DisplayLevel))
            writer->addAttribute("text:display-levels", d->stylesPrivate.value(KoListStyle::DisplayLevel).toInt());
        if (d->stylesPrivate.contains(KoListStyle::ListItemPrefix))
            writer->addAttribute("style:num-prefix", d->stylesPrivate.value(KoListStyle::ListItemPrefix).toString());
        if (d->stylesPrivate.contains(KoListStyle::ListItemSuffix))
            writer->addAttribute("style:num-suffix", d->stylesPrivate.value(KoListStyle::ListItemSuffix).toString());

        KoOdfNumberDefinition numberFormatter;
        numberFormatter.setFormatSpecification(numberFormat());
        numberFormatter.setLetterSynchronization(letterSynchronization());
        numberFormatter.saveOdf(writer);
    }
    else if (labelType() == KoListStyle::ImageLabelType) {
        KoImageData *imageData = d->stylesPrivate.value(KoListStyle::BulletImage).value<KoImageData *>();
        Q_ASSERT(imageData->priv()->collection);
        if (imageData && imageData->priv()->collection) {
            writer->startElement("text:list-level-style-image");
            writer->addAttribute("xlink:show", "embed");
            writer->addAttribute("xlink:actuate", "onLoad");
            writer->addAttribute("xlink:type", "simple");
            writer->addAttribute("xlink:href", context.imageHref(imageData));
            context.addDataCenter(imageData->priv()->collection);
        }
    }
    else {
        writer->startElement("text:list-level-style-bullet");

        int bullet = 0;
        if (d->stylesPrivate.contains(KoListStyle::BulletCharacter)) {
            bullet = d->stylesPrivate.value(KoListStyle::BulletCharacter).toInt();
        }
        writer->addAttribute("text:bullet-char", QChar(bullet));
    }

    KoTextSharedSavingData *sharedSavingData = 0;
    if (d->stylesPrivate.contains(KoListStyle::CharacterStyleId) && (characterStyleId() != 0) &&
           (sharedSavingData = static_cast<KoTextSharedSavingData *>(context.sharedData(KOTEXT_SHARED_SAVING_ID)))) {
        QString styleName = sharedSavingData->styleName(characterStyleId());
               // dynamic_cast<KoTextSharedSavingData *>(context.sharedData(KOTEXT_SHARED_SAVING_ID))->styleName(characterStyleId());
        if (!styleName.isEmpty()) {
            writer->addAttribute("text:style-name", styleName);
         }
    }

    // These apply to bulleted and numbered lists
    if (d->stylesPrivate.contains(KoListStyle::Level))
        writer->addAttribute("text:level", d->stylesPrivate.value(KoListStyle::Level).toInt());

    writer->startElement("style:list-level-properties", false);

    if (d->stylesPrivate.contains(KoListStyle::Width)) {
        writer->addAttributePt("fo:width", width());
    }
    if (d->stylesPrivate.contains(KoListStyle::Height)) {
        writer->addAttributePt("fo:height", height());
    }

    if(d->stylesPrivate.contains(KoListStyle::AlignmentMode) && alignmentMode()==false) {

        writer->addAttribute("text:list-level-position-and-space-mode","label-width-and-position");

        if (d->stylesPrivate.contains(KoListStyle::Indent))
            writer->addAttributePt("text:space-before", indent());

        if (d->stylesPrivate.contains(KoListStyle::MinimumWidth))
            writer->addAttributePt("text:min-label-width", minimumWidth());

        if (d->stylesPrivate.contains(KoListStyle::Alignment))
            writer->addAttribute("fo:text-align", KoText::alignmentToString(alignment()));

        if (d->stylesPrivate.contains(KoListStyle::MinimumDistance))
            writer->addAttributePt("text:min-label-distance", minimumDistance());
    } else {
        writer->addAttribute("text:list-level-position-and-space-mode","label-alignment");

        if (d->stylesPrivate.contains(KoListStyle::Alignment))
            writer->addAttribute("fo:text-align", KoText::alignmentToString(alignment()));

        writer->startElement("style:list-level-label-alignment");

        if(labelFollowedBy()==KoListStyle::ListTab) {
            writer->addAttribute("text:label-followed-by","listtab");
            writer->addAttributePt("text:list-tab-stop-position", tabStopPosition());
        } else if (labelFollowedBy()==KoListStyle::Nothing){
            writer->addAttribute("text:label-followed-by","nothing");
        }else{
            writer->addAttribute("text:label-followed-by","space");
        }

        writer->addAttributePt("fo:text-indent", textIndent());
        writer->addAttributePt("fo:margin-left", margin());

        writer->endElement();
    }

    writer->endElement(); // list-level-properties

    // text properties

    if (d->stylesPrivate.contains(KoListStyle::CharacterProperties)) {
        KoGenStyle liststyle(KoGenStyle::ListStyle);

        QSharedPointer<KoCharacterStyle> cs = characterProperties();
        cs->saveOdf(liststyle);

        liststyle.writeStyleProperties(writer, KoGenStyle::TextType);
    }

//   debugText << "Key KoListStyle::ListItemPrefix :" << d->stylesPrivate.value(KoListStyle::ListItemPrefix);
//   debugText << "Key KoListStyle::ListItemSuffix :" << d->stylesPrivate.value(KoListStyle::ListItemSuffix);
//   debugText << "Key KoListStyle::CharacterStyleId :" << d->stylesPrivate.value(KoListStyle::CharacterStyleId);
//   debugText << "Key KoListStyle::RelativeBulletSize :" << d->stylesPrivate.value(KoListStyle::RelativeBulletSize);
//   debugText << "Key KoListStyle::Alignment :" << d->stylesPrivate.value(KoListStyle::Alignment);
//   debugText << "Key KoListStyle::LetterSynchronization :" << d->stylesPrivate.value(KoListStyle::LetterSynchronization);

    writer->endElement();
}
