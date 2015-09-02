/* This file is part of the KDE project
   Copyright (C) 2003-2015 Jarosław Staniek <staniek@kde.org>

   Contains code from kglobalsettings.cpp:
   Copyright (C) 2000, 2006 David Faure <faure@kde.org>
   Copyright (C) 2008 Friedrich W. H. Kossebau <kossebau@kde.org>

   Contains code from kdialog.cpp:
   Copyright (C) 1998 Thomas Tanghus (tanghus@earthling.net)
   Additions 1999-2000 by Espen Sand (espen@kde.org)
                       and Holger Freyther <freyther@kde.org>
             2005-2009 Olivier Goffart <ogoffart @ kde.org>
             2006      Tobias Koenig <tokoe@kde.org>

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
#include <KexiIcon.h>

#include <QPainter>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QIcon>
#include <QMetaProperty>
#include <QFocusEvent>
#include <QFile>
#include <QStyle>
#include <QLayout>
#include <KMessageBox>
#include <QFileInfo>
#include <QClipboard>
#include <QMimeDatabase>
#include <QMimeType>
#include <QUrl>
#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QTextCodec>
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>

#include <KRun>
#include <KToolInvocation>
#include <KIconEffect>
#include <KColorScheme>
#include <KLocalizedString>
#include <KConfigGroup>
#include <KAboutData>

#if HAVE_LANGINFO_H
#include <langinfo.h>
#endif

#ifdef Q_OS_WIN
#include <windows.h>

static QRgb qt_colorref2qrgb(COLORREF col)
{
    return qRgb(GetRValue(col), GetGValue(col), GetBValue(col));
}
#endif

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

Q_GLOBAL_STATIC(DelayedCursorHandler, _delayedCursorHandler)

void KexiUtils::setWaitCursor(bool noDelay)
{
    if (qobject_cast<QApplication*>(qApp)) {
        _delayedCursorHandler->start(noDelay);
    }
}

void KexiUtils::removeWaitCursor()
{
    if (qobject_cast<QApplication*>(qApp)) {
        _delayedCursorHandler->stop();
    }
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

QObject* KexiUtils::findFirstQObjectChild(QObject *o, const char* className, const char* objName)
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

QMetaProperty KexiUtils::findPropertyWithSuperclasses(const QObject* object,
        const char* name)
{
    const int index = object->metaObject()->indexOfProperty(name);
    if (index == -1)
        return QMetaProperty();
    return object->metaObject()->property(index);
}

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

QString KexiUtils::fileDialogFilterString(const QMimeType &mime, bool kdeFormat)
{
    if (!mime.isValid()) {
        return QString();
    }

    QString str;
    if (kdeFormat) {
        if (mime.globPatterns().isEmpty()) {
            str = "*";
        } else {
            str = mime.globPatterns().join(" ");
        }
        str += "|";
    }
    str += mime.comment();
    if (!mime.globPatterns().isEmpty() || !kdeFormat) {
        str += " (";
        if (mime.globPatterns().isEmpty())
            str += "*";
        else
            str += mime.globPatterns().join("; ");
        str += ")";
    }
    if (kdeFormat)
        str += "\n";
    else
        str += ";;";
    return str;
}

QString KexiUtils::fileDialogFilterString(const QString& mimeName, bool kdeFormat)
{
    QMimeDatabase db;
    QMimeType mime = db.mimeTypeForName(mimeName);
    return fileDialogFilterString(mime, kdeFormat);
}

QString KexiUtils::fileDialogFilterStrings(const QStringList& mimeStrings, bool kdeFormat)
{
    QString ret;
    QStringList::ConstIterator endIt = mimeStrings.constEnd();
    for (QStringList::ConstIterator it = mimeStrings.constBegin(); it != endIt; ++it)
        ret += fileDialogFilterString(*it, kdeFormat);
    return ret;
}

//! @internal
static QFileDialog* getImageDialog(QWidget *parent, const QString &caption, const QUrl &directory,
                                   const QList<QByteArray> supportedMimeTypes)
{
    QFileDialog *dialog = new QFileDialog(parent, caption);
    dialog->setDirectoryUrl(directory);
    const QStringList mimeTypeFilters
        = KexiUtils::convertTypes<QByteArray, QString, &QString::fromLatin1>(supportedMimeTypes);
    dialog->setMimeTypeFilters(mimeTypeFilters);
    return dialog;
}

QUrl KexiUtils::getOpenImageUrl(QWidget *parent, const QString &caption, const QUrl &directory)
{
    QScopedPointer<QFileDialog> dialog(
        getImageDialog(parent, caption.isEmpty() ? i18n("Open") : caption, directory,
                       QImageReader::supportedMimeTypes()));
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setAcceptMode(QFileDialog::AcceptOpen);
    dialog->exec();
    return dialog->selectedUrls().value(0);
}

QUrl KexiUtils::getSaveImageUrl(QWidget *parent, const QString &caption, const QUrl &directory)
{
    QScopedPointer<QFileDialog> dialog(
        getImageDialog(parent, caption.isEmpty() ? i18n("Save") : caption, directory,
                       QImageWriter::supportedMimeTypes()));
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->exec();
    return dialog->selectedUrls().value(0);
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
    else if (s <= 5 && v <= 5)
        v += factor - 50;
    c2.setHsv(h, s, qMin(255, v + factor - 100));
    return c2;
}

QIcon KexiUtils::colorizeIconToTextColor(const QPixmap& icon, const QPalette& palette,
                                         QPalette::ColorRole role)
{
    QPixmap pm(
        KIconEffect().apply(icon, KIconEffect::Colorize, 1.0f,
                            palette.color(role), false));
    KIconEffect::semiTransparent(pm);
    return QIcon(pm);
}

QPixmap KexiUtils::emptyIcon(KIconLoader::Group iconGroup)
{
    QPixmap noIcon(IconSize(iconGroup), IconSize(iconGroup));
    noIcon.fill(Qt::transparent);
    return noIcon;
}

static void drawOrScalePixmapInternal(QPainter* p, const WidgetMargins& margins, const QRect& rect,
                                      QPixmap& pixmap, QPoint &pos, Qt::Alignment alignment,
                                      bool scaledContents, bool keepAspectRatio,
                                      Qt::TransformationMode transformMode = Qt::FastTransformation)
{
    if (pixmap.isNull())
        return;

    const bool fast = false;
    const int w = rect.width() - margins.left - margins.right;
    const int h = rect.height() - margins.top - margins.bottom;
//! @todo we can optimize painting by drawing rescaled pixmap here
//! and performing detailed painting later (using QTimer)
//    QPixmap pixmapBuffer;
//    QPainter p2;
//    QPainter *target;
//    if (fast) {
//       target = p;
//    } else {
//        target = &p2;
//    }
//! @todo only create buffered pixmap of the minimum size and then do not fillRect()
// target->fillRect(0,0,rect.width(),rect.height(), backgroundColor);

    pos = rect.topLeft() + QPoint(margins.left, margins.top);
    if (scaledContents) {
        if (keepAspectRatio) {
            QImage img(pixmap.toImage());
            img = img.scaled(w, h, Qt::KeepAspectRatio, transformMode);
            if (img.width() < w) {
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
            if (p) {
                p->drawImage(pos, img);
            }
            else {
                pixmap = QPixmap::fromImage(img);
            }
        } else {
            if (!fast) {
                pixmap = pixmap.scaled(w, h, Qt::IgnoreAspectRatio, transformMode);
                if (p) {
                    p->drawPixmap(pos, pixmap);
                }
            }
        }
    }
    else {
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
        pos += QPoint(margins.left, margins.top);
        if (p) {
            p->drawPixmap(pos, pixmap);
        }
    }
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

void KexiUtils::setFocusWithReason(QWidget* widget, Qt::FocusReason reason)
{
    if (!widget)
        return;
    QFocusEvent fe(QEvent::FocusIn, reason);
    QCoreApplication::sendEvent(widget, &fe);
}

void KexiUtils::unsetFocusWithReason(QWidget* widget, Qt::FocusReason reason)
{
    if (!widget)
        return;
    QFocusEvent fe(QEvent::FocusOut, reason);
    QCoreApplication::sendEvent(widget, &fe);
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

Q_GLOBAL_STATIC(QFont, _smallFont)

QFont KexiUtils::smallFont(QWidget *init)
{
    if (init) {
        *_smallFont = init->font();
        const int wdth = QApplication::desktop()->screenGeometry(init).width();
        int size = 10 + qMax(0, wdth - 1100) / 100;
        size = qMin(init->fontInfo().pixelSize(), size);
        size = qMax(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont).pixelSize(), size);
        _smallFont->setPixelSize(size);
    }
    return *_smallFont;
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

int KexiUtils::marginHint()
{
    return QApplication::style()->pixelMetric(QStyle::PM_DefaultChildMargin);
}

int KexiUtils::spacingHint()
{
    return QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
}

void KexiUtils::setStandardMarginsAndSpacing(QLayout *layout)
{
    setMargins(layout, KexiUtils::marginHint());
    layout->setSpacing(KexiUtils::spacingHint());
}

void KexiUtils::setMargins(QLayout *layout, int value)
{
    layout->setContentsMargins(value, value, value, value);
}

void KexiUtils::replaceColors(QPixmap* original, const QColor& color)
{
    Q_ASSERT(original);
    QImage dest(original->toImage());
    replaceColors(&dest, color);
    *original = QPixmap::fromImage(dest);
}

void KexiUtils::replaceColors(QImage* original, const QColor& color)
{
    Q_ASSERT(original);
    QPainter p(original);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(original->rect(), color);
}

bool KexiUtils::isLightColorScheme()
{
    return KColorScheme(QPalette::Active, KColorScheme::Window).background().color().lightness() >= 128;
}

int KexiUtils::dimmedAlpha()
{
    return 150;
}

QPalette KexiUtils::paletteWithDimmedColor(const QPalette &pal, QPalette::ColorGroup group,
                                           QPalette::ColorRole role)
{
    QPalette result(pal);
    QColor color(result.color(group, role));
    color.setAlpha(dimmedAlpha());
    result.setColor(group, role, color);
    return result;
}

QPalette KexiUtils::paletteWithDimmedColor(const QPalette &pal, QPalette::ColorRole role)
{
    QPalette result(pal);
    QColor color(result.color(role));
    color.setAlpha(dimmedAlpha());
    result.setColor(role, color);
    return result;
}

QPalette KexiUtils::paletteForReadOnly(const QPalette &palette)
{
    QPalette p(palette);
    p.setBrush(QPalette::Base, palette.brush(QPalette::Disabled, QPalette::Base));
    p.setBrush(QPalette::Text, palette.brush(QPalette::Disabled, QPalette::Text));
    p.setBrush(QPalette::Highlight, palette.brush(QPalette::Disabled, QPalette::Highlight));
    p.setBrush(QPalette::HighlightedText, palette.brush(QPalette::Disabled, QPalette::HighlightedText));
    return p;
}

//---------------------

void KexiUtils::installRecursiveEventFilter(QObject *object, QObject *filter)
{
    if (!object || !filter || !object->isWidgetType())
        return;

//    qDebug() << "Installing event filter on widget:" << object
//        << "directed to" << filter->objectName();
    object->installEventFilter(filter);

    const QObjectList list(object->children());
    foreach(QObject *obj, list) {
        installRecursiveEventFilter(obj, filter);
    }
}

void KexiUtils::removeRecursiveEventFilter(QObject *object, QObject *filter)
{
    object->removeEventFilter(filter);
    if (!object->isWidgetType())
        return;

    const QObjectList list(object->children());
    foreach(QObject *obj, list) {
        removeRecursiveEventFilter(obj, filter);
    }
}

PaintBlocker::PaintBlocker(QWidget* parent)
 : QObject(parent)
 , m_enabled(true)
{
    parent->installEventFilter(this);
}

void PaintBlocker::setEnabled(bool set)
{
    m_enabled = set;
}

bool PaintBlocker::enabled() const
{
    return m_enabled;
}

bool PaintBlocker::eventFilter(QObject* watched, QEvent* event)
{
    if (m_enabled && watched == parent() && event->type() == QEvent::Paint) {
        return true;
    }
    return false;
}

void KexiUtils::openHyperLink(const QUrl &url, QWidget *parent, const OpenHyperlinkOptions &options)
{
    if (url.isLocalFile()) {
        QFileInfo fileInfo(url.toLocalFile());
        if (!fileInfo.exists()) {
            KMessageBox::sorry(parent, xi18nc("@info", "The file or directory <filename>%1</filename> does not exist.", fileInfo.absoluteFilePath()));
            return;
        }
    }

    if (!url.isValid()) {
        KMessageBox::sorry(parent, xi18nc("@info", "Invalid hyperlink <link>%1</link>.",
                                          url.url(QUrl::PreferLocalFile)));
        return;
    }

    QMimeDatabase db;
    QString type = db.mimeTypeForUrl(url).name();

    if (!options.allowExecutable && KRun::isExecutableFile(url, type)) {
        KMessageBox::sorry(parent, xi18nc("@info", "Executable <link>%1</link> not allowed.",
                                          url.url(QUrl::PreferLocalFile)));
        return;
    }

    if (!options.allowRemote && !url.isLocalFile()) {
        KMessageBox::sorry(parent, xi18nc("@info", "Remote hyperlink <link>%1</link> not allowed.",
                                          url.url(QUrl::PreferLocalFile)));
        return;
    }

    if (KRun::isExecutableFile(url, type)) {
        int ret = KMessageBox::questionYesNo(parent
                    , xi18nc("@info", "Do you want to run this file?"
                            "<warning>Running executables can be dangerous.</warning>")
                    , QString()
                    , KGuiItem(xi18nc("@action:button Run script file", "Run"), koIconName("system-run"))
                    , KStandardGuiItem::no()
                    , "AllowRunExecutable", KMessageBox::Dangerous);

        if (ret != KMessageBox::Yes) {
            return;
        }
    }

    switch(options.tool) {
        case OpenHyperlinkOptions::DefaultHyperlinkTool:
            KRun::runUrl(url, type, parent);
            break;
        case OpenHyperlinkOptions::BrowserHyperlinkTool:
            QDesktopServices::openUrl(url);
            break;
        case OpenHyperlinkOptions::MailerHyperlinkTool:
            QDesktopServices::openUrl(url);
            break;
    }
}

// ----

KexiDBDebugTreeWidget::KexiDBDebugTreeWidget(QWidget *parent)
 : QTreeWidget(parent)
{
}

void KexiDBDebugTreeWidget::copy()
{
    if (currentItem()) {
        qApp->clipboard()->setText(currentItem()->text(0));
    }
}

// ----

DebugWindow::DebugWindow(QWidget * parent)
    : QWidget(parent, Qt::Window)
{
}

// ----

QSize KexiUtils::comboBoxArrowSize(QStyle *style)
{
    if (!style) {
        style = QApplication::style();
    }
    QStyleOptionComboBox cbOption;
    return style->subControlRect(QStyle::CC_ComboBox, &cbOption, QStyle::SC_ComboBoxArrow).size();
}

void KexiUtils::addDirtyFlag(QString *text)
{
    Q_ASSERT(text);
    *text = xi18nc("'Dirty (modified) object' flag", "%1*", *text);
}

//! From klocale_kde.cpp
//! @todo KEXI3 support other OS-es (use from klocale_*.cpp)
static QByteArray systemCodeset()
{
    QByteArray codeset;
#if HAVE_LANGINFO_H
    // Qt since 4.2 always returns 'System' as codecForLocale and KDE (for example
    // KEncodingFileDialog) expects real encoding name. So on systems that have langinfo.h use
    // nl_langinfo instead, just like Qt compiled without iconv does. Windows already has its own
    // workaround

    codeset = nl_langinfo(CODESET);

    if ((codeset == "ANSI_X3.4-1968") || (codeset == "US-ASCII")) {
        // means ascii, "C"; QTextCodec doesn't know, so avoid warning
        codeset = "ISO-8859-1";
    }
#endif
    return codeset;
}

QTextCodec* g_codecForEncoding = 0;

bool setEncoding(int mibEnum)
{
    QTextCodec *codec = QTextCodec::codecForMib(mibEnum);
    if (codec) {
        g_codecForEncoding = codec;
    }

    return codec != 0;
}

//! From klocale_kde.cpp
static void initEncoding()
{
    if (!g_codecForEncoding) {
        // This all made more sense when we still had the EncodingEnum config key.

        QByteArray codeset = systemCodeset();

        if (!codeset.isEmpty()) {
            QTextCodec *codec = QTextCodec::codecForName(codeset);
            if (codec) {
                setEncoding(codec->mibEnum());
            }
        } else {
            setEncoding(QTextCodec::codecForLocale()->mibEnum());
        }

        if (!g_codecForEncoding) {
            qWarning() << "Cannot resolve system encoding, defaulting to ISO 8859-1.";
            const int mibDefault = 4; // ISO 8859-1
            setEncoding(mibDefault);
        }
        Q_ASSERT(g_codecForEncoding);
    }
}

QByteArray KexiUtils::encoding()
{
    initEncoding();
    return g_codecForEncoding->name();
}

namespace {

//! @internal for graphicEffectsLevel()
class GraphicEffectsLevel
{
public:
    GraphicEffectsLevel() {
        KConfigGroup g(KSharedConfig::openConfig(), "KDE-Global GUI Settings");

        // Asking for hasKey we do not ask for graphicEffectsLevelDefault() that can
        // contain some very slow code. If we can save that time, do it. (ereslibre)
        if (g.hasKey("GraphicEffectsLevel")) {
            value = ((GraphicEffects) g.readEntry("GraphicEffectsLevel", QVariant((int) NoEffects)).toInt());
            return;
        }

        // For now, let always enable animations by default. The plan is to make
        // this code a bit smarter. (ereslibre)
        value = ComplexAnimationEffects;
    }
    GraphicEffects value;
};
}

Q_GLOBAL_STATIC(GraphicEffectsLevel, g_graphicEffectsLevel)

GraphicEffects KexiUtils::graphicEffectsLevel()
{
    return g_graphicEffectsLevel->value;
}

bool KexiUtils::activateItemsOnSingleClick(QWidget *widget)
{
    QStyle *style = widget ? widget->style() : QApplication::style();
    return style->styleHint(QStyle::SH_ItemView_ActivateItemOnSingleClick, 0, widget);
}

// NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
QColor KexiUtils::inactiveTitleColor()
{
#ifdef Q_OS_WIN
    return qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTION));
#else
    KConfigGroup g(KSharedConfig::openConfig(), "WM");
    return g.readEntry("inactiveBackground", QColor(224, 223, 222));
#endif
}

// NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
QColor KexiUtils::inactiveTextColor()
{
#ifdef Q_OS_WIN
    return qt_colorref2qrgb(GetSysColor(COLOR_INACTIVECAPTIONTEXT));
#else
    KConfigGroup g(KSharedConfig::openConfig(), "WM");
    return g.readEntry("inactiveForeground", QColor(75, 71, 67));
#endif
}

// NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
QColor KexiUtils::activeTitleColor()
{
#ifdef Q_OS_WIN
    return qt_colorref2qrgb(GetSysColor(COLOR_ACTIVECAPTION));
#else
    KConfigGroup g(KSharedConfig::openConfig(), "WM");
    return g.readEntry("activeBackground", QColor(48, 174, 232));
#endif
}

// NOTE: keep this in sync with kdebase/workspace/kcontrol/colors/colorscm.cpp
QColor KexiUtils::activeTextColor()
{
#ifdef Q_OS_WIN
    return qt_colorref2qrgb(GetSysColor(COLOR_CAPTIONTEXT));
#else
    KConfigGroup g(KSharedConfig::openConfig(), "WM");
    return g.readEntry("activeForeground", QColor(255, 255, 255));
#endif
}

QString KexiUtils::makeStandardCaption(const QString &userCaption, CaptionFlags flags)
{
    QString caption = KAboutData::applicationData().displayName();
    if (caption.isEmpty()) {
        return QCoreApplication::instance()->applicationName();
    }
    QString captionString = userCaption.isEmpty() ? caption : userCaption;

    // If the document is modified, add '[modified]'.
    if (flags & ModifiedCaption) {
        captionString += QString::fromUtf8(" [") + xi18n("modified") + QString::fromUtf8("]");
    }

    if (!userCaption.isEmpty()) {
        // Add the application name if:
        // User asked for it, it's not a duplication  and the app name (caption()) is not empty
        if (flags & AppNameCaption &&
                !caption.isEmpty() &&
                !userCaption.endsWith(caption)) {
            // TODO: check to see if this is a transient/secondary window before trying to add the app name
            //       on platforms that need this
            captionString += xi18nc("Document/application separator in titlebar", " – ") + caption;
        }
    }
    return captionString;
}
