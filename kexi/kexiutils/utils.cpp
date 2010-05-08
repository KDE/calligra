/* This file is part of the KDE project
   Copyright (C) 2003-2009 Jarosław Staniek <staniek@kde.org>

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

#include "utils.h"
#include "utils_p.h"
#include "kexiutils_global.h"

#include <QRegExp>
#include <QPainter>
#include <QImage>
#include <QIcon>
#include <QMetaProperty>
#include <QBitmap>
#include <QFocusEvent>
#include <QFile>
#include <QStyle>
#include <QLayout>

#include <KDebug>
#include <KCursor>
#include <KApplication>
#include <KIconEffect>
#include <KIconLoader>
#include <KGlobalSettings>
#include <KAction>
#include <KDialog>

using namespace KexiUtils;

DelayedCursorHandler::DelayedCursorHandler()
        : startedOrActive(false)
{
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(show()));
}
void DelayedCursorHandler::start(bool noDelay)
{
    startedOrActive = true;
    timer.start(noDelay ? 0 : 1000);
}
void DelayedCursorHandler::stop()
{
    startedOrActive = false;
    timer.stop();
    QApplication::restoreOverrideCursor();
}
void DelayedCursorHandler::show()
{
    QApplication::restoreOverrideCursor();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

K_GLOBAL_STATIC(DelayedCursorHandler, _delayedCursorHandler)

void KexiUtils::setWaitCursor(bool noDelay)
{
    if (qApp->type() != QApplication::Tty)
        _delayedCursorHandler->start(noDelay);
}
void KexiUtils::removeWaitCursor()
{
    if (qApp->type() != QApplication::Tty)
        _delayedCursorHandler->stop();
}

WaitCursor::WaitCursor(bool noDelay)
{
    setWaitCursor(noDelay);
}

WaitCursor::~WaitCursor()
{
    removeWaitCursor();
}

WaitCursorRemover::WaitCursorRemover()
{
    m_reactivateCursor = _delayedCursorHandler->startedOrActive;
    _delayedCursorHandler->stop();
}

WaitCursorRemover::~WaitCursorRemover()
{
    if (m_reactivateCursor)
        _delayedCursorHandler->start(true);
}

//--------------------------------------------------------------------------------

QObject* KexiUtils::findFirstQObjectChild(QObject *o, const char* className /* compat with Qt3 */, const char* objName)
{
    if (!o)
        return 0;
    const QObjectList list(o->children());
    foreach(QObject *child, list) {
        if (child->inherits(className) && (!objName || child->objectName() == objName))
            return child;
    }
    //try children
    foreach(QObject *child, list) {
        child = findFirstQObjectChild(child, className, objName);
        if (child)
            return child;
    }
    return 0;
}

#if 0
int KexiUtils::indexOfPropertyWithSuperclasses(const QObject *object, const char* name)
{
    const int index = object->metaObject()->indexOfProperty(name);
}
#endif

QMetaProperty KexiUtils::findPropertyWithSuperclasses(const QObject* object,
        const char* name)
{
    const int index = object->metaObject()->indexOfProperty(name);
    if (index == -1)
        return QMetaProperty();
    return object->metaObject()->property(index);
}

#if 0
QMetaProperty KexiUtils::findPropertyWithSuperclasses(const QObject* object,
        int index)
{
    return object->metaObject()->property(index);
}
#endif

bool KexiUtils::objectIsA(QObject* object, const QList<QByteArray>& classNames)
{
    foreach(const QByteArray& ba, classNames) {
        if (objectIsA(object, ba.constData()))
            return true;
    }
    return false;
}

QList<QMetaMethod> KexiUtils::methodsForMetaObject(
    const QMetaObject *metaObject, QFlags<QMetaMethod::MethodType> types,
    QFlags<QMetaMethod::Access> access)
{
    const int count = metaObject ? metaObject->methodCount() : 0;
    QList<QMetaMethod> result;
    for (int i = 0; i < count; i++) {
        QMetaMethod method(metaObject->method(i));
        if (types & method.methodType() && access & method.access())
            result += method;
    }
    return result;
}

QList<QMetaMethod> KexiUtils::methodsForMetaObjectWithParents(
    const QMetaObject *metaObject, QFlags<QMetaMethod::MethodType> types,
    QFlags<QMetaMethod::Access> access)
{
    QList<QMetaMethod> result;
    while (metaObject) {
        const int count = metaObject->methodCount();
        for (int i = 0; i < count; i++) {
            QMetaMethod method(metaObject->method(i));
            if (types & method.methodType() && access & method.access())
                result += method;
        }
        metaObject = metaObject->superClass();
    }
    return result;
}

QList<QMetaProperty> KexiUtils::propertiesForMetaObject(
    const QMetaObject *metaObject)
{
    const int count = metaObject ? metaObject->propertyCount() : 0;
    QList<QMetaProperty> result;
    for (int i = 0; i < count; i++)
        result += metaObject->property(i);
    return result;
}

QList<QMetaProperty> KexiUtils::propertiesForMetaObjectWithInherited(
    const QMetaObject *metaObject)
{
    QList<QMetaProperty> result;
    while (metaObject) {
        const int count = metaObject->propertyCount();
        for (int i = 0; i < count; i++)
            result += metaObject->property(i);
        metaObject = metaObject->superClass();
    }
    return result;
}

QStringList KexiUtils::enumKeysForProperty(const QMetaProperty& metaProperty)
{
    QStringList result;
    QMetaEnum enumerator(metaProperty.enumerator());
    const int count = enumerator.keyCount();
    for (int i = 0; i < count; i++)
        result.append(QString::fromLatin1(enumerator.key(i)));
    return result;
}

QString KexiUtils::fileDialogFilterString(const KMimeType::Ptr& mime, bool kdeFormat)
{
    if (mime.isNull())
        return QString();

    QString str;
    if (kdeFormat) {
        if (mime->patterns().isEmpty())
            str = "*";
        else
            str = mime->patterns().join(" ");
        str += "|";
    }
    str += mime->comment();
    if (!mime->patterns().isEmpty() || !kdeFormat) {
        str += " (";
        if (mime->patterns().isEmpty())
            str += "*";
        else
            str += mime->patterns().join("; ");
        str += ")";
    }
    if (kdeFormat)
        str += "\n";
    else
        str += ";;";
    return str;
}

QString KexiUtils::fileDialogFilterString(const QString& mimeString, bool kdeFormat)
{
    KMimeType::Ptr ptr = KMimeType::mimeType(mimeString);
    return fileDialogFilterString(ptr, kdeFormat);
}

QString KexiUtils::fileDialogFilterStrings(const QStringList& mimeStrings, bool kdeFormat)
{
    QString ret;
    QStringList::ConstIterator endIt = mimeStrings.constEnd();
    for (QStringList::ConstIterator it = mimeStrings.constBegin(); it != endIt; ++it)
        ret += fileDialogFilterString(*it, kdeFormat);
    return ret;
}

QColor KexiUtils::blendedColors(const QColor& c1, const QColor& c2, int factor1, int factor2)
{
    return QColor(
               int((c1.red()*factor1 + c2.red()*factor2) / (factor1 + factor2)),
               int((c1.green()*factor1 + c2.green()*factor2) / (factor1 + factor2)),
               int((c1.blue()*factor1 + c2.blue()*factor2) / (factor1 + factor2)));
}

QColor KexiUtils::contrastColor(const QColor& c)
{
    int g = qGray(c.rgb());
    if (g > 110)
        return c.dark(200);
    else if (g > 80)
        return c.light(150);
    else if (g > 20)
        return c.light(300);
    return Qt::gray;
}

QColor KexiUtils::bleachedColor(const QColor& c, int factor)
{
    int h, s, v;
    c.getHsv(&h, &s, &v);
    QColor c2;
    if (factor < 100)
        factor = 100;
    if (s >= 250 && v >= 250) //for colors like cyan or red, make the result more white
        s = qMax(0, s - factor - 50);
    else if (s <= 5 && s <= 5)
        v += factor - 50;
    c2.setHsv(h, s, qMin(255, v + factor - 100));
    return c2;
}

QIcon KexiUtils::colorizeIconToTextColor(const QPixmap& icon, const QPalette& palette)
{
#ifdef __GNUC__
#warning KexiUtils::colorizeIconToTextColor OK?
#else
#pragma WARNING(port KexiUtils::colorizeIconToTextColor OK?)
#endif
    QPixmap pm(
        KIconEffect().apply(icon, KIconEffect::Colorize, 1.0f,
                            palette.color(QPalette::Active, QPalette::ButtonText), false));
    KIconEffect::semiTransparent(pm);
    return QIcon(pm);
}

QPixmap KexiUtils::emptyIcon(KIconLoader::Group iconGroup)
{
    QPixmap noIcon(IconSize(iconGroup), IconSize(iconGroup));
    QBitmap bmpNoIcon(noIcon.size());
    bmpNoIcon.fill(Qt::color0);
    noIcon.setMask(bmpNoIcon);
    return noIcon;
}

void KexiUtils::serializeMap(const QMap<QString, QString>& map, QByteArray& array)
{
    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_3_1);
    ds << map;
}

void KexiUtils::serializeMap(const QMap<QString, QString>& map, QString& string)
{
    QByteArray array;
    QDataStream ds(&array, QIODevice::WriteOnly);
    ds.setVersion(QDataStream::Qt_3_1);
    ds << map;
    kDebug() << array[3] << " " << array[4] << " " << array[5];
    const uint size = array.size();
    string.clear();
    string.reserve(size);
    for (uint i = 0; i < size; i++) {
        string[i] = QChar(ushort(array[i]) + 1);
    }
}

QMap<QString, QString> KexiUtils::deserializeMap(const QByteArray& array)
{
    QMap<QString, QString> map;
    QByteArray ba(array);
    QDataStream ds(&ba, QIODevice::ReadOnly);
    ds.setVersion(QDataStream::Qt_3_1);
    ds >> map;
    return map;
}

QMap<QString, QString> KexiUtils::deserializeMap(const QString& string)
{
    QByteArray array;
    const uint size = string.length();
    array.resize(size);
    for (uint i = 0; i < size; i++) {
        array[i] = char(string[i].unicode() - 1);
    }
    QMap<QString, QString> map;
    QDataStream ds(&array, QIODevice::ReadOnly);
    ds.setVersion(QDataStream::Qt_3_1);
    ds >> map;
    return map;
}

QString KexiUtils::stringToFileName(const QString& string)
{
    QString _string(string);
    _string.replace(QRegExp("[\\\\/:\\*?\"<>|]"), " ");
    return _string.simplified();
}

void KexiUtils::simpleCrypt(QString& string)
{
    for (int i = 0; i < string.length(); i++)
        string[i] = QChar(string[i].unicode() + 47 + i);
}

void KexiUtils::simpleDecrypt(QString& string)
{
    for (int i = 0; i < string.length(); i++)
        string[i] = QChar(string[i].unicode() - 47 - i);
}

static void drawOrScalePixmapInternal(QPainter* p, const WidgetMargins& margins, const QRect& rect,
                                      QPixmap& pixmap, QPoint &pos, Qt::Alignment alignment,
                                      bool scaledContents, bool keepAspectRatio,
                                      Qt::TransformationMode transformMode = Qt::FastTransformation)
{
    if (pixmap.isNull())
        return;

    const bool fast = false;//pixmap.width() > 1000 && pixmap.height() > 800; //fast drawing needed
    const int w = rect.width() - margins.left - margins.right;
    const int h = rect.height() - margins.top - margins.bottom;
//! @todo we can optimize painting by drawing rescaled pixmap here
//! and performing detailed painting later (using QTimer)
//    QPixmap pixmapBuffer;
//    QPainter p2;
    QPainter *target;
//    if (fast) {
        target = p;
//    } else {
//moved  pixmapBuffer.resize(rect.size()-QSize(lineWidth, lineWidth));
//moved  p2.begin(&pm, p.device());
//        target = &p2;
//    }
//! @todo only create buffered pixmap of the minimum size and then do not fillRect()
// target->fillRect(0,0,rect.width(),rect.height(), backgroundColor);

    if (scaledContents) {
        if (keepAspectRatio) {
            QImage img(pixmap.toImage());
            img = img.scaled(w, h, Qt::KeepAspectRatio, transformMode);
            pos = rect.topLeft();
            if (img.width() < w) {
//                int hAlign = QApplication::horizontalAlignment(alignment);
                if (alignment & Qt::AlignRight)
                    pos.setX(pos.x() + w - img.width());
                else if (alignment & Qt::AlignHCenter)
                    pos.setX(pos.x() + w / 2 - img.width() / 2);
            }
            else if (img.height() < h) {
                if (alignment & Qt::AlignBottom)
                    pos.setY(pos.y() + h - img.height());
                else if (alignment & Qt::AlignVCenter)
                    pos.setY(pos.y() + h / 2 - img.height() / 2);
            }
//            pixmapBuffer.fromImage(img);
            if (!fast) {
//                p2.begin(&pixmapBuffer);
//                p2.initFrom(p.device());
            }
            else {
//                target->drawPixmap(pos, pixmapBuffer);
            }
            if (p) {
                p->drawImage(pos, img);
            }
            else {
                pixmap = QPixmap::fromImage(img);
            }
        } else {
            if (!fast) {
//                pixmapBuffer = QPixmap(rect.size() - QSize(margins.right, margins.bottom));
//                p2.begin(&pixmapBuffer);
                //, p.device());
//                p2.drawPixmap(QRect(rect.x(), rect.y(), w, h), pixmap);
                pos = rect.topLeft();
                pixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, transformMode);
                if (p) {
                    p->drawPixmap(pos, pixmap);
                }
            }
            else {
//                target->drawPixmap(QRect(rect.x() + margins.left, rect.y() + margins.top, w, h), pixmap);
            }
        }
    }
    else {
//        int hAlign = QApplication::horizontalAlignment(alignment);
        if (alignment & Qt::AlignRight)
            pos.setX(pos.x() + w - pixmap.width());
        else if (alignment & Qt::AlignHCenter)
            pos.setX(pos.x() + w / 2 - pixmap.width() / 2);
        else //left, etc.
            pos.setX(pos.x());

        if (alignment & Qt::AlignBottom)
            pos.setY(pos.y() + h - pixmap.height());
        else if (alignment & Qt::AlignVCenter)
            pos.setY(pos.y() + h / 2 - pixmap.height() / 2);
        else //top, etc.
            pos.setY(pos.y());
//  target->drawPixmap(pos, pixmap);
//  if (!fast)
//   p2.begin(&pixmapBuffer, p.device());
        pos += QPoint(margins.left, margins.top);
        if (p) {
            p->drawPixmap(pos, pixmap);
        }
    }
/*    if (scaledContents && !fast && p.isActive()) {
        p2.end();
        p.drawPixmap(
           (int)p.worldMatrix().dx() + rect.x() + margins.left + pos.x(),
           (int)p.worldMatrix().dy() + rect.y() + margins.top + pos.y(),
            pixmapBuffer,
            rect.x(), rect.y(), w, h);
    }*/
}

void KexiUtils::drawPixmap(QPainter& p, const WidgetMargins& margins, const QRect& rect,
                           const QPixmap& pixmap, Qt::Alignment alignment, bool scaledContents, bool keepAspectRatio,
                           Qt::TransformationMode transformMode)
{
    QPixmap px(pixmap);
    QPoint pos;
    drawOrScalePixmapInternal(&p, margins, rect, px, pos, alignment, scaledContents, keepAspectRatio, transformMode);
}

QPixmap KexiUtils::scaledPixmap(const WidgetMargins& margins, const QRect& rect,
                                const QPixmap& pixmap, QPoint& pos, Qt::Alignment alignment,
                                bool scaledContents, bool keepAspectRatio, Qt::TransformationMode transformMode)
{
    QPixmap px(pixmap);
    drawOrScalePixmapInternal(0, margins, rect, px, pos, alignment, scaledContents, keepAspectRatio, transformMode);
    return px;
}

QString KexiUtils::ptrToStringInternal(void* ptr, uint size)
{
    QString str;
    unsigned char* cstr_ptr = (unsigned char*) & ptr;
    for (uint i = 0; i < size; i++) {
        QString s;
        s.sprintf("%2.2x", cstr_ptr[i]);
        str.append(s);
    }
    return str;
}

void* KexiUtils::stringToPtrInternal(const QString& str, uint size)
{
    if ((str.length() / 2) < (int)size)
        return 0;
    QByteArray array;
    array.resize(size);
    bool ok;
    for (uint i = 0; i < size; i++) {
        array[i] = (unsigned char)(str.mid(i * 2, 2).toUInt(&ok, 16));
        if (!ok)
            return 0;
    }
    return *(void**)(array.data());
}

void KexiUtils::setFocusWithReason(QWidget* widget, Qt::FocusReason reason)
{
    if (!widget)
        return;
    QFocusEvent fe(QEvent::FocusIn, reason);
    //QFocusEvent::setReason(reason);
    QCoreApplication::sendEvent(widget, &fe);
    //QFocusEvent::resetReason();
}

void KexiUtils::unsetFocusWithReason(QWidget* widget, Qt::FocusReason reason)
{
    if (!widget)
        return;
    QFocusEvent fe(QEvent::FocusOut, reason);
    //QFocusEvent::setReason(reason);
    QCoreApplication::sendEvent(widget, &fe);
    //QFocusEvent::resetReason();
}

//--------

KexiUtils::WidgetMargins::WidgetMargins()
        : left(0), top(0), right(0), bottom(0)
{
}

KexiUtils::WidgetMargins::WidgetMargins(QWidget *widget)
{
    copyFromWidget(widget);
}

KexiUtils::WidgetMargins::WidgetMargins(int _left, int _top, int _right, int _bottom)
        : left(_left), top(_top), right(_right), bottom(_bottom)
{
}

KexiUtils::WidgetMargins::WidgetMargins(int commonMargin)
        : left(commonMargin), top(commonMargin), right(commonMargin), bottom(commonMargin)
{
}

void KexiUtils::WidgetMargins::copyFromWidget(QWidget *widget)
{
    Q_ASSERT(widget);
    widget->getContentsMargins(&left, &top, &right, &bottom);
}

void KexiUtils::WidgetMargins::copyToWidget(QWidget *widget)
{
    widget->setContentsMargins(left, top, right, bottom);
}

WidgetMargins& KexiUtils::WidgetMargins::operator+= (const WidgetMargins & margins)
{
    left += margins.left;
    top += margins.top;
    right += margins.right;
    bottom += margins.bottom;
    return *this;
}

const WidgetMargins KexiUtils::operator+ (
    const WidgetMargins& margins1, const WidgetMargins & margins2)
{
    // margins2 is not used
    Q_UNUSED(margins2);
    return WidgetMargins(
               margins1.left + margins1.left,
               margins1.top + margins1.top,
               margins1.right + margins1.right,
               margins1.bottom + margins1.bottom);
}

//---------

K_GLOBAL_STATIC(QFont, _smallFont)

QFont KexiUtils::smallFont(QWidget *init)
{
    if (init) {
        *_smallFont = init->font();
        const int wdth = KGlobalSettings::desktopGeometry(init).width();
        int size = 10 + qMax(0, wdth - 1100) / 100;
        size = qMin(init->fontInfo().pixelSize(), size);
        size = qMax(KGlobalSettings::smallestReadableFont().pixelSize(), size);
        _smallFont->setPixelSize(size);
    }
    return *_smallFont;
}

//---------

//! @internal
class StaticSetOfStrings::Private
{
public:
    Private() : array(0), set(0) {}
    ~Private() {
        delete set;
    }
    const char** array;
    QSet<QByteArray> *set;
};

StaticSetOfStrings::StaticSetOfStrings()
        : d(new Private)
{
}

StaticSetOfStrings::StaticSetOfStrings(const char* array[])
        : d(new Private)
{
    setStrings(array);
}

StaticSetOfStrings::~StaticSetOfStrings()
{
    delete d;
}

void StaticSetOfStrings::setStrings(const char* array[])
{
    delete d->set;
    d->set = 0;
    d->array = array;
}

bool StaticSetOfStrings::isEmpty() const
{
    return d->array == 0;
}

bool StaticSetOfStrings::contains(const QByteArray& string) const
{
    if (!d->set) {
        d->set = new QSet<QByteArray>();
        for (const char ** p = d->array;*p;p++)
            d->set->insert(QByteArray::fromRawData(*p, qstrlen(*p) + 1));
    }
    return d->set->contains(string);
}

//---------------------

KTextEditorFrame::KTextEditorFrame(QWidget * parent, Qt::WindowFlags f)
        : QFrame(parent, f)
{
    QEvent dummy(QEvent::StyleChange);
    changeEvent(&dummy);
}

void KTextEditorFrame::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::StyleChange) {
        if (style()->objectName() != "oxygen") // oxygen already nicely paints the frame
            setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
        else
            setFrameStyle(QFrame::NoFrame);
    }
}

//---------------------

void KexiUtils::setStandardMarginsAndSpacing(QLayout *layout)
{
    setMargins(layout, KDialog::marginHint());
    layout->setSpacing( KDialog::spacingHint() );
}

void KexiUtils::setMargins(QLayout *layout, int value)
{
    layout->setContentsMargins(value, value, value, value);
}

QPixmap KexiUtils::replaceColors(const QPixmap& original, const QColor& color)
{
    QPixmap dest(original);
    dest.fill(color);
    {
        QPainter p(&dest);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.drawPixmap(0, 0, original);
    }
    return dest;
}

#include "utils_p.moc"
