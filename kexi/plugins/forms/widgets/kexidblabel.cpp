/* This file is part of the KDE project
   Copyright (C) 2005 Christian Nitschkowski <segfault_ii@web.de>
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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

#include "kexidblabel.h"

#include <qbitmap.h>
#include <qpainter.h>
#include <qdrawutil.h>
#include <qapplication.h>
#include <qtimer.h>
#include <QPaintEvent>
#include <QLabel>
#include <QShowEvent>
#include <QResizeEvent>

#include <kdebug.h>

#include <kexidb/field.h>
#include <kexiutils/utils.h>
#include <formeditor/utils.h>

#define SHADOW_OFFSET_X 3
#define SHADOW_OFFSET_Y 3
#define SHADOW_FACTOR 16.0
#define SHADOW_OPACITY 50.0
#define SHADOW_AXIS_FACTOR 2.0
#define SHADOW_DIAGONAL_FACTOR 1.0
#define SHADOW_THICKNESS 1

#if 0
//! @internal
class KexiDBInternalLabel : public QLabel
{
    friend class KexiDBLabel;
public:
    KexiDBInternalLabel(KexiDBLabel*);
    virtual ~KexiDBInternalLabel();

protected:
//  virtual void paintEvent( QPaintEvent * event );
    void updateFrame();

    QImage makeShadow(const QImage& textImage, const QColor &bgColor, const QRect& boundingRect);
    QRect getBounding(const QImage &image, const QRect& startRect);
//  double defaultDecay( QImage& source, int i, int j );
    QPixmap getShadowPixmap();

    QRect m_shadowRect;
    KexiDBLabel *m_parentLabel;
};

KexiDBInternalLabel::KexiDBInternalLabel(KexiDBLabel* parent)
        : QLabel(parent)
        , m_parentLabel(parent)
{
    Qt::Alignment a = alignment();
    a &= (0xffffff ^ Qt::AlignVertical_Mask);
    a |= Qt::AlignTop;
    setAlignment(a);
    setWordWrap(true);
    updateFrame();
}

void KexiDBInternalLabel::updateFrame()
{
    setIndent(m_parentLabel->indent());
    setMargin(m_parentLabel->margin());
    setFont(m_parentLabel->font());

    setFrameShadow(m_parentLabel->frameShadow());
    setFrameShape(m_parentLabel->frameShape());
    setFrameStyle(m_parentLabel->frameStyle());
    setMidLineWidth(m_parentLabel->midLineWidth());
    setLineWidth(m_parentLabel->lineWidth());
}

KexiDBInternalLabel::~KexiDBInternalLabel()
{
}

/*!
* This method is copied from kdebase/kdesktop/kshadowengine.cpp
* Some modifactions were made.
* --
* Christian Nitschkowski
*/
QImage KexiDBInternalLabel::makeShadow(const QImage& textImage,
                                       const QColor &bgColor, const QRect& boundingRect)
{
    QString origText(text());

    // create a new image for for the shaddow
    const int w = textImage.width();
    const int h = textImage.height();

    // avoid calling these methods for every pixel
    const int bgRed = bgColor.red();
    const int bgGreen = bgColor.green();
    const int bgBlue = bgColor.blue();

    const int startX = boundingRect.x() + SHADOW_THICKNESS;
    const int startY = boundingRect.y() + SHADOW_THICKNESS;
    const int effectWidth = boundingRect.bottomRight().x() - SHADOW_THICKNESS;
    const int effectHeight = boundingRect.bottomRight().y() - SHADOW_THICKNESS;
// const int period = (effectWidth - startX) / 10;

    double alphaShadow;

    /*
     * This is the source pixmap
     */
    QImage img = textImage.convertToFormat(QImage::Format_ARGB32);
    QImage result(w, h, QImage::Format_ARGB32);

// result.fill( 0 ); // all black
    double realOpacity = SHADOW_OPACITY + qMin(50.0 / double(256.0 - qGray(bgColor.rgb())), 50.0);
    //int _h, _s, _v;
    //.getHsv( &_h, &_s, &_v );
    if (palette().color(backgroundRole()) == Qt::red)//_s>=250 && _v>=250) //for colors like cyan or red, make the result more white
        realOpacity += 50.0;
    result.fill((int)realOpacity);
    //Qt4: ARGB32 already set in convertToFormat: result.setAlphaBuffer( true );

    for (int i = startX; i < effectWidth; i++) {
        for (int j = startY; j < effectHeight; j++) {
            /*!
            * This method is copied from kdebase/kdesktop/kshadowengine.cpp
            * Some modifactions were made.
            * --
            * Christian Nitschkowski
            */
            if ((i < 1) || (j < 1) || (i > img.width() - 2) || (j > img.height() - 2))
                continue;
            else
                alphaShadow = (qGray(img.pixel(i - 1, j - 1)) * SHADOW_DIAGONAL_FACTOR +
                               qGray(img.pixel(i - 1, j)) * SHADOW_AXIS_FACTOR +
                               qGray(img.pixel(i - 1, j + 1)) * SHADOW_DIAGONAL_FACTOR +
                               qGray(img.pixel(i , j - 1)) * SHADOW_AXIS_FACTOR +
                               0 +
                               qGray(img.pixel(i , j + 1)) * SHADOW_AXIS_FACTOR +
                               qGray(img.pixel(i + 1, j - 1)) * SHADOW_DIAGONAL_FACTOR +
                               qGray(img.pixel(i + 1, j)) * SHADOW_AXIS_FACTOR +
                               qGray(img.pixel(i + 1, j + 1)) * SHADOW_DIAGONAL_FACTOR) / SHADOW_FACTOR;

            // update the shadow's i,j pixel.
            if (alphaShadow > 0)
                result.setPixel(i, j, qRgba(bgRed, bgGreen , bgBlue,
                                            (int)((alphaShadow > realOpacity) ? realOpacity : alphaShadow)
                                           ));
        }
        /*caused too much redraw problems  if (period && i % period) {
              qApp->processEvents();
              if (text() != origText) //text has been changed in the meantime: abort
                return QImage();
            }*/
    }
    return result;
}

QPixmap KexiDBInternalLabel::getShadowPixmap()
{
    /*!
    * Backup the default color used to draw text.
    */
    const QColor textColor = palette().color(foregroundRole());

    /*!
    * Temporary storage for the generated shadow
    */
    QPixmap tempPixmap;
    QImage shadowImage, tempImage;
    QPainter painter;

    m_shadowRect = QRect();

    tempPixmap = QPixmap(size());
    tempPixmap.fill(Qt::black);
    tempPixmap.setMask(tempPixmap.createHeuristicMask(true));

    /*!
    * The textcolor has to be white for creating shadows!
    */
//    setPaletteForegroundColor(Qt::white);
    QPalette pal( palette() );
    pal.setColor(foregroundRole(), Qt::white);
    setPalette(pal);

    /*!
    Draw the label "as usual" in a pixmap
    */
    painter.begin(&tempPixmap);
    painter.setFont(font());
#ifdef __GNUC__
#warning Qt4 TODO drawContents( &painter );
#else
#pragma WARNING( Qt4 TODO drawContents( &painter ); )
#endif
    painter.end();
//    setPaletteForegroundColor(textColor);
    pal = palette();
    pal.setColor(foregroundRole(), textColor);
    setPalette(pal);

    /*!
    * Calculate the first bounding rect.
    * This will fit around the unmodified text.
    */
    shadowImage = tempPixmap.toImage();

    /*!
    Get the first bounding rect.
    This may speed up makeShadow later.
    */
    m_shadowRect = getBounding(shadowImage, m_shadowRect);

    /*!
    * Enlarge the bounding rect to make sure the shadow
    * will fit in.
    * The new rect has to fit in the pixmap.
    * I have to admit this isn't really nice code...
    */
    m_shadowRect.setX(qMax(m_shadowRect.x() - (m_shadowRect.width() / 4), 0));
    m_shadowRect.setY(qMax(m_shadowRect.y() - (m_shadowRect.height() / 4), 0));
    m_shadowRect.setBottomRight(QPoint(
                                    qMin(m_shadowRect.x() + (m_shadowRect.width() * 3 / 2), shadowImage.width()),
                                    qMin(m_shadowRect.y() + (m_shadowRect.height() * 3 / 2), shadowImage.height())));

    shadowImage = makeShadow(shadowImage,
                             qGray(palette().color(backgroundRole()).rgb()) < 127 ? Qt::white : Qt::black,
                             m_shadowRect);
    if (shadowImage.isNull())
        return QPixmap();

    /*!
    Now get the final bounding rect.
    */
    m_shadowRect = getBounding(shadowImage, m_shadowRect);

    /*!
    Paint the labels background in a new pixmap.
    */
    QPixmap finalPixmap(size());
    painter.begin(&finalPixmap);
    painter.fillRect(0, 0, finalPixmap.width(), finalPixmap.height(),
                     palette().brush(
                         isEnabled() ? QPalette::Active : QPalette::Disabled,
                         QColorGroup::Background));
    painter.end();

    /*!
    Copy the part of the background the shadow will be on
    to another pixmap.
    */
    tempPixmap = QPixmap(m_shadowRect.size());
    tempPixmap.setMask(QBitmap());
    if (!finalPixmap.isNull()) {
        bitBlt(&tempPixmap, 0, 0, &finalPixmap,
               m_shadowRect.x() + SHADOW_OFFSET_X,
               m_shadowRect.y() + SHADOW_OFFSET_Y,
               m_shadowRect.width(),
               m_shadowRect.height());
    }
    /*!
    Replace the big background pixmap with the
    part we could out just before.
    */
    finalPixmap = tempPixmap;

    /*!
    Copy the "interesting" part of the shadow image
    to a new image.
    I tried to copy this to a pixmap directly,
    but it didn't work correctly.
    Maybe a Qt bug?
    */
    tempImage = shadowImage.copy(m_shadowRect);
    tempPixmap.convertFromImage(tempImage);
    /*!
    Anyways, merge the shadow with the background.
    */
    if (!tempPixmap.isNull()) {
        bitBlt(&finalPixmap, 0, 0, &tempPixmap);
    }

    /**
    Now move the rect.
    Don't do this before the shadow is copied from shadowImage!
    */
    m_shadowRect.translate(SHADOW_OFFSET_X, SHADOW_OFFSET_Y);

    return finalPixmap;
}

QRect KexiDBInternalLabel::getBounding(const QImage &image, const QRect& startRect)
{
    QPoint topLeft;
    QPoint bottomRight;

    const int startX = startRect.x();
    const int startY = startRect.y();
    /*!
    * Ugly beast to get the correct width and height
    */
    const int width = qMin((startRect.bottomRight().x() > 0
                            ? startRect.bottomRight().x() : QCOORD_MAX),
                           image.width());
    const int height = qMin((startRect.bottomRight().y() > 0
                             ? startRect.bottomRight().y() : QCOORD_MAX),
                            image.height());

    /*!
    Assume the first pixel has the color of the
    background that has to be cut away.
    Qt uses the four corner pixels to guess the
    correct color, but in this case the topleft
    pixel should be enough.
    */
    QRgb trans = image.pixel(0, 0);

    for (int y = startY; y < height; y++) {
        for (int x = startX; x < width; x++) {
            if (image.pixel(x, y) != trans) {
                topLeft.setY(y);
                y = height;
                break;
            }
        }
    }

    for (int x = startX; x < width; x++) {
        for (int y = startY; y < height; y++) {
            if (image.pixel(x, y) != trans) {
                topLeft.setX(x);
                x = width;
                break;
            }
        }
    }

    for (int y = height - 1; y > topLeft.y(); y--) {
        for (int x = width - 1; x > topLeft.x(); x--) {
            if (image.pixel(x, y) != trans) {
                bottomRight.setY(y + 1);
                y = 0;
                break;
            }
        }
    }

    for (int x = width - 1; x > topLeft.x(); x--) {
        for (int y = height - 1; y > topLeft.y(); y--) {
            if (image.pixel(x, y) != trans) {
                bottomRight.setX(x + 1);
                x = 0;
                break;
            }
        }
    }

    return QRect(
               topLeft.x(),
               topLeft.y(),
               bottomRight.x() - topLeft.x(),
               bottomRight.y() - topLeft.y());
}
#endif

//=========================================================

//! @internal
class KexiDBLabel::Private
{
public:
    Private()
            : timer(0)
//  , autonumberDisplayParameters(0)
            , pixmapDirty(true)
            , shadowEnabled(false)
            , resizeEvent(false) {
    }
    ~Private() {}
    QPixmap shadowPixmap;
    QPoint shadowPosition;
//    KexiDBInternalLabel* internalLabel;
    QTimer* timer;
    QColor frameColor;
    bool pixmapDirty : 1;
    bool shadowEnabled : 1;
    bool resizeEvent : 1;
};

//=========================================================

KexiDBLabel::KexiDBLabel(QWidget *parent, Qt::WFlags f)
        : QLabel(parent, f)
        , KexiDBTextWidgetInterface()
        , KexiFormDataItemInterface()
        , d(new Private())
{
    init();
}

KexiDBLabel::KexiDBLabel(const QString& text, QWidget *parent, Qt::WFlags f)
        : QLabel(parent, f)
        , KexiDBTextWidgetInterface()
        , KexiFormDataItemInterface()
        , d(new Private())
{
    init();
    setText(text);
}

KexiDBLabel::~KexiDBLabel()
{
    delete d;
}

void KexiDBLabel::init()
{
    m_hasFocusableWidget = false;
//2.0    d->internalLabel = new KexiDBInternalLabel(this);
//2.0    d->internalLabel->hide();
    d->frameColor = palette().color(foregroundRole());

//2.0    setAlignment(d->internalLabel->alignment());
}

#if 0 //2.0
void KexiDBLabel::updatePixmapLater()
{
    if (d->resizeEvent) {
        if (!d->timer) {
            d->timer = new QTimer(this);
            d->timer->setObjectName("KexiDBLabelTimer");
            connect(d->timer, SIGNAL(timeout()), this, SLOT(updatePixmap()));
        }
        d->timer->setSingleShot(true);
        d->timer->start(100);
        d->resizeEvent = false;
        return;
    }
    if (d->timer && d->timer->isActive())
        return;
    updatePixmap();
}

void KexiDBLabel::updatePixmap()
{
    /*!
    Whatever has changed in KexiDBLabel,
    every parameter is set to our private-label.
    Just in case...
    */
    d->internalLabel->setText(text());
    d->internalLabel->setFixedSize(size());
    d->internalLabel->setPalette(palette());
    d->internalLabel->setAlignment(alignment());
// d->shadowPixmap = KPixmap(); //parallel repaints won't hurt us cause incomplete pixmap
    QPixmap shadowPixmap(d->internalLabel->getShadowPixmap());
    if (shadowPixmap.isNull())
        return;
    d->shadowPixmap = shadowPixmap;
    d->shadowPosition = d->internalLabel->m_shadowRect.topLeft();
    d->pixmapDirty = false;
    repaint();
}
#endif

void KexiDBLabel::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    if (d->shadowEnabled) {
        /*!
        If required, update the pixmap-cache.
        */
        if (d->pixmapDirty) {
//2.0            updatePixmapLater();
        }

#if 0 //2.0
        /*!
        If the part that should be redrawn intersects with our shadow,
        redraw the shadow where it intersects with e->rect().
        Have to move the clipping rect around a bit because
        the shadow has to be drawn using an offset relative to
        the widgets border.
        */
        if (!d->pixmapDirty && e->rect().contains(d->shadowPosition) && !d->shadowPixmap.isNull()) {
            QRect clipRect = QRect(
                                 qMax(e->rect().x() - d->shadowPosition.x(), 0),
                                 qMax(e->rect().y() - d->shadowPosition.y(), 0),
                                 qMin(e->rect().width() + d->shadowPosition.x(), d->shadowPixmap.width()),
                                 qMin(e->rect().height() + d->shadowPosition.y(), d->shadowPixmap.height()));
            p.drawPixmap(d->internalLabel->m_shadowRect.topLeft(), d->shadowPixmap, clipRect);
        }
#endif
    }
    KexiDBTextWidgetInterface::paint(this, &p, text().isEmpty(), alignment(), false);
    p.end();

    QPalette origPal;
    if (editingMode()) {
        origPal = palette();
        QPalette pal(palette());
        pal.setBrush(QPalette::WindowText, Qt::transparent);
        setPalette(pal);
    }
    QLabel::paintEvent(e);
    if (editingMode()) {
        setPalette(origPal);
    }

    const bool hasFrame = frameWidth() >= 1 && frameShape() != QFrame::NoFrame;
    if (designMode() && !hasFrame) {
        p.begin(this);
        KFormDesigner::paintWidgetFrame(p, rect());
        p.end();
    }
}

void KexiDBLabel::setValueInternal(const QVariant& add, bool removeOld)
{
    if (removeOld)
        setText(add.toString());
    else
        setText(m_origValue.toString() + add.toString());
}

QVariant KexiDBLabel::value()
{
    return text();
}

void KexiDBLabel::setInvalidState(const QString& displayText)
{
    setText(displayText);
}

bool KexiDBLabel::valueIsNull()
{
    return text().isNull();
}

bool KexiDBLabel::valueIsEmpty()
{
    return text().isEmpty();
}

bool KexiDBLabel::isReadOnly() const
{
    return true;
}

void KexiDBLabel::setReadOnly(bool readOnly)
{
    Q_UNUSED(readOnly);
}

QWidget* KexiDBLabel::widget()
{
    return this;
}

bool KexiDBLabel::cursorAtStart()
{
    return false;
}

bool KexiDBLabel::cursorAtEnd()
{
    return false;
}

void KexiDBLabel::clear()
{
    setText(QString());
}

bool KexiDBLabel::setProperty(const char * name, const QVariant & value)
{
    const bool ret = QLabel::setProperty(name, value);
    if (d->shadowEnabled) {
        if (0 == qstrcmp("indent", name) || 0 == qstrcmp("font", name) || 0 == qstrcmp("margin", name)
                || 0 == qstrcmp("frameShadow", name) || 0 == qstrcmp("frameShape", name)
                || 0 == qstrcmp("frameStyle", name) || 0 == qstrcmp("midLineWidth", name)
                || 0 == qstrcmp("lineWidth", name)) {
//2.0            d->internalLabel->setProperty(name, value);
//2.0            updatePixmap();
        }
    }
    return ret;
}

void KexiDBLabel::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    KexiDBTextWidgetInterface::setColumnInfo(cinfo, this);
}

void KexiDBLabel::setShadowEnabled(bool state)
{
    d->shadowEnabled = state;
    d->pixmapDirty = true;
//2.0    if (state)
//2.0        d->internalLabel->updateFrame();
    repaint();
}

void KexiDBLabel::resizeEvent(QResizeEvent* e)
{
    if (isVisible())
        d->resizeEvent = true;
    d->pixmapDirty = true;
    QLabel::resizeEvent(e);
}

void KexiDBLabel::fontChange(const QFont& font)
{
    d->pixmapDirty = true;
//2.0    d->internalLabel->setFont(font);
    QLabel::fontChange(font);
}

void KexiDBLabel::styleChange(QStyle& style)
{
    d->pixmapDirty = true;
    QLabel::styleChange(style);
}

void KexiDBLabel::enabledChange(bool enabled)
{
    d->pixmapDirty = true;
//2.0    d->internalLabel->setEnabled(enabled);
    QLabel::enabledChange(enabled);
}

void KexiDBLabel::paletteChange(const QPalette& oldPal)
{
    Q_UNUSED(oldPal);
    d->pixmapDirty = true;
//2.0    d->internalLabel->setPalette(palette());
}

/*const QColor & KexiDBLabel::paletteForegroundColor () const
{
  return d->foregroundColor;
}

void KexiDBLabel::setPaletteForegroundColor ( const QColor& color )
{
  d->foregroundColor = color;
}*/

#ifdef __GNUC__
#warning TODO
#else
#pragma WARNING( TODO  )
#endif
/*TODO
void KexiDBLabel::frameChanged()
{
  d->pixmapDirty = true;
  d->internalLabel->updateFrame();
  QFrame::frameChanged();
}*/

void KexiDBLabel::showEvent(QShowEvent* e)
{
    d->pixmapDirty = true;
    QLabel::showEvent(e);
}

void KexiDBLabel::setText(const QString& text)
{
    d->pixmapDirty = true;
    QLabel::setText(text);
    //This is necessary for KexiFormDataItemInterface
    valueChanged();
    repaint();
}

bool KexiDBLabel::shadowEnabled() const
{
    return d->shadowEnabled;
}

#define ClassName KexiDBLabel
#define SuperClassName QLabel
#include "kexiframeutils_p.cpp"
#include "kexidblabel.moc"
