/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2007 Jarosław Staniek <staniek@kde.org>

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

#include "kexidbimagebox.h"

#include <qapplication.h>
#include <qpixmap.h>
#include <QStyle>
#include <QStyleOptionFocusRect>
#include <qclipboard.h>
#include <qtooltip.h>
#include <qimage.h>
#include <qbuffer.h>
#include <qfiledialog.h>
#include <qpainter.h>

#include <kdebug.h>
#include <kmenu.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kfiledialog.h>
#include <kimageio.h>
#include <kstandarddirs.h>
#include <kstdaccel.h>
#include <kmessagebox.h>
#include <kguiitem.h>

#include <widget/utils/kexidropdownbutton.h>
#include <widget/utils/kexicontextmenuutils.h>
#include <kexiutils/utils.h>
#include <kexidb/field.h>
#include <kexidb/utils.h>
#include <kexidb/queryschema.h>
#include <formeditor/widgetlibrary.h>
#include <kexi_global.h>

//#ifdef Q_WS_WIN
//#include <win32_utils.h>
//#include <krecentdirs.h>
//#endif

#include "kexidbutils.h"
#include "../kexiformpart.h"

//! @internal
struct KexiDBImageBox_Static {
    KexiDBImageBox_Static() : pixmap(0), small(0) {}
    QPixmap *pixmap;
    QPixmap *small;
};

K_GLOBAL_STATIC(KexiDBImageBox_Static, KexiDBImageBox_static)

KexiDBImageBox::KexiDBImageBox(bool designMode, QWidget *parent)
        : KexiFrame(parent /* Qt 4 not neede: , Qt::WNoAutoErase*/)
        , KexiFormDataItemInterface()
        , m_alignment(Qt::AlignAuto | Qt::AlignTop)
        , m_designMode(designMode)
        , m_readOnly(false)
        , m_scaledContents(false)
        , m_keepAspectRatio(true)
        , m_insideSetData(false)
        , m_setFocusOnButtonAfterClosingPopup(false)
        , m_lineWidthChanged(false)
        , m_paintEventEnabled(true)
        , m_dropDownButtonVisible(true)
        , m_insideSetPalette(false)
{
    installEventFilter(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    //setup context menu
    m_contextMenu = new KexiImageContextMenu(this);
    m_contextMenu->installEventFilter(this);

    if (m_designMode) {
        m_chooser = 0;
    } else {
        m_chooser = new KexiDropDownButton(this);
        m_chooser->setFocusPolicy(Qt::StrongFocus);
        m_chooser->setMenu(m_contextMenu);
        setFocusProxy(m_chooser);
        m_chooser->installEventFilter(this);
//  m_chooser->setPalette(qApp->palette());
//  hlyr->addWidget(m_chooser);
    }

    setBackgroundMode(Qt::NoBackground);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Plain);
    setFrameColor(Qt::black);

    m_paletteBackgroundColorChanged = false; //set this here, not before

    connect(m_contextMenu, SIGNAL(updateActionsAvailabilityRequested(bool&, bool&)),
            this, SLOT(slotUpdateActionsAvailabilityRequested(bool&, bool&)));
    connect(m_contextMenu, SIGNAL(insertFromFileRequested(const KUrl&)),
            this, SLOT(handleInsertFromFileAction(const KUrl&)));
    connect(m_contextMenu, SIGNAL(saveAsRequested(const QString&)),
            this, SLOT(handleSaveAsAction(const QString&)));
    connect(m_contextMenu, SIGNAL(cutRequested()),
            this, SLOT(handleCutAction()));
    connect(m_contextMenu, SIGNAL(copyRequested()),
            this, SLOT(handleCopyAction()));
    connect(m_contextMenu, SIGNAL(pasteRequested()),
            this, SLOT(handlePasteAction()));
    connect(m_contextMenu, SIGNAL(clearRequested()),
            this, SLOT(clear()));
    connect(m_contextMenu, SIGNAL(showPropertiesRequested()),
            this, SLOT(handleShowPropertiesAction()));

// connect(m_contextMenu, SIGNAL(aboutToHide()), this, SLOT(slotAboutToHidePopupMenu()));
// if (m_chooser) {
    //we couldn't use m_chooser->setPopup() because of drawing problems
//  connect(m_chooser, SIGNAL(pressed()), this, SLOT(slotChooserPressed()));
//  connect(m_chooser, SIGNAL(released()), this, SLOT(slotChooserReleased()));
//  connect(m_chooser, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
// }

    setDataSource(QString());   //to initialize popup menu and actions availability
}

KexiDBImageBox::~KexiDBImageBox()
{
}

KexiImageContextMenu* KexiDBImageBox::contextMenu() const
{
    return m_contextMenu;
}

QVariant KexiDBImageBox::value()
{
    if (dataSource().isEmpty()) {
        //not db-aware
        return QVariant();
    }
    //db-aware mode
    return m_value; //todo
    //return QVariant(); //todo
}

void KexiDBImageBox::setValueInternal(const QVariant& add, bool removeOld, bool loadPixmap)
{
    if (isReadOnly())
        return;
    m_contextMenu->hide();
    if (removeOld)
        m_value = add.toByteArray();
    else //do not add "m_origValue" to "add" as this is QByteArray
        m_value = m_origValue.toByteArray();
    bool ok = !m_value.isEmpty();
    if (ok) {
        ///unused (m_valueMimeType is not available unless the px is inserted) QString type( KImageIO::typeForMime(m_valueMimeType) );
        ///ok = KImageIO::canRead( type );
        ok = loadPixmap ? m_pixmap.loadFromData(m_value) : true; //, type.toLatin1());
        if (!ok) {
            //! @todo inform about error?
        }
    }
    if (!ok) {
        m_valueMimeType.clear();
        m_pixmap = QPixmap();
    }
    repaint();
}

void KexiDBImageBox::setInvalidState(const QString& displayText)
{
    Q_UNUSED(displayText);

// m_pixmapLabel->setPixmap(QPixmap());
    if (!dataSource().isEmpty()) {
        m_value = QByteArray();
    }
// m_pixmap = QPixmap();
// m_originalFileName.clear();

//! @todo m_pixmapLabel->setText( displayText );

    if (m_chooser)
        m_chooser->hide();
    setReadOnly(true);
}

bool KexiDBImageBox::valueIsNull()
{
    return m_value.isEmpty();
// return !m_pixmapLabel->pixmap() || m_pixmapLabel->pixmap()->isNull();
}

bool KexiDBImageBox::valueIsEmpty()
{
    return false;
}

bool KexiDBImageBox::isReadOnly() const
{
    return m_readOnly;
}

void KexiDBImageBox::setReadOnly(bool set)
{
    m_readOnly = set;
}

QPixmap KexiDBImageBox::pixmap() const
{
    if (dataSource().isEmpty()) {
        //not db-aware
        return m_data.pixmap();
    }
    //db-aware mode
    return m_pixmap;
}

uint KexiDBImageBox::pixmapId() const
{
    if (dataSource().isEmpty()) {// && !m_data.stored()) {
        //not db-aware
        return m_data.id();
    }
    return 0;
}

void KexiDBImageBox::setPixmapId(uint id)
{
    if (m_insideSetData) //avoid recursion
        return;
    setData(KexiBLOBBuffer::self()->objectForId(id, /*unstored*/false));
    repaint();
}

uint KexiDBImageBox::storedPixmapId() const
{
    if (dataSource().isEmpty() && m_data.stored()) {
        //not db-aware
        return m_data.id();
    }
    return 0;
}

void KexiDBImageBox::setStoredPixmapId(uint id)
{
    setData(KexiBLOBBuffer::self()->objectForId(id, /*stored*/true));
    repaint();
}

bool KexiDBImageBox::hasScaledContents() const
{
    return m_scaledContents;
// return m_pixmapLabel->hasScaledContents();
}

/*void KexiDBImageBox::setPixmap(const QByteArray& pixmap)
{
  setValueInternal(pixmap, true);
// setBackgroundMode(pixmap.isNull() ? Qt::NoBackground : Qt::PaletteBackground);
}*/

void KexiDBImageBox::setScaledContents(bool set)
{
//todo m_pixmapLabel->setScaledContents(set);
    m_scaledContents = set;
    repaint();
}

void KexiDBImageBox::setKeepAspectRatio(bool set)
{
    m_keepAspectRatio = set;
    if (m_scaledContents)
        repaint();
}

QWidget* KexiDBImageBox::widget()
{
    //! @todo
// return m_pixmapLabel;
    return this;
}

bool KexiDBImageBox::cursorAtStart()
{
    return true;
}

bool KexiDBImageBox::cursorAtEnd()
{
    return true;
}

QByteArray KexiDBImageBox::data() const
{
    if (dataSource().isEmpty()) {
        //static mode
        return m_data.data();
    } else {
        //db-aware mode
        return m_value;
    }
}

void KexiDBImageBox::insertFromFile()
{
    m_contextMenu->insertFromFile();
}

void KexiDBImageBox::handleInsertFromFileAction(const KUrl& url)
{
    if (!dataSource().isEmpty() && isReadOnly())
        return;

    if (dataSource().isEmpty()) {
        //static mode
        KexiBLOBBuffer::Handle h = KexiBLOBBuffer::self()->insertPixmap(url);
        if (!h)
            return;
        setData(h);
        repaint();
    } else {
        //db-aware
        QString fileName(url.isLocalFile() ? url.path() : url.prettyUrl());

        //! @todo download the file if remote, then set fileName properly
        QFile f(fileName);
        if (!f.open(IO_ReadOnly)) {
            //! @todo err msg
            return;
        }
        QByteArray ba = f.readAll();
        if (f.error() != QFile::NoError) {
            //! @todo err msg
            f.close();
            return;
        }
        m_valueMimeType = KMimeType::findByUrl(fileName, 0, url.isLocalFile())->name();
        setValueInternal(ba, true);
    }

//! @todo emit signal for setting "dirty" flag within the design
    if (!dataSource().isEmpty()) {
        signalValueChanged();
    }
}

void KexiDBImageBox::handleAboutToSaveAsAction(
    QString& origFilename, QString& fileExtension, bool& dataIsEmpty)
{
    if (data().isEmpty()) {
        kWarning() << "KexiDBImageBox::handleAboutToSaveAs(): no pixmap!" << endl;
        dataIsEmpty = false;
        return;
    }
    if (dataSource().isEmpty()) { //for static images filename and mimetype can be available
        origFilename = m_data.originalFileName();
        if (!origFilename.isEmpty())
            origFilename = QString("/") + origFilename;
        if (!m_data.mimeType().isEmpty())
            fileExtension = KImageIO::typeForMime(m_data.mimeType()).first().toLower();
    }
}

void KexiDBImageBox::handleSaveAsAction(const QString& fileName)
{
    QFile f(fileName);
    if (!f.open(IO_WriteOnly)) {
        //! @todo err msg
        return;
    }
    f.write(data());
    if (f.error() != QFile::NoError) {
        //! @todo err msg
        f.close();
        return;
    }
    f.close();
}

void KexiDBImageBox::handleCutAction()
{
    if (!dataSource().isEmpty() && isReadOnly())
        return;
    handleCopyAction();
    clear();
}

void KexiDBImageBox::handleCopyAction()
{
    qApp->clipboard()->setPixmap(pixmap(), QClipboard::Clipboard);
}

void KexiDBImageBox::handlePasteAction()
{
    if (isReadOnly() || (!m_designMode && !hasFocus()))
        return;
    QPixmap pm(qApp->clipboard()->pixmap(QClipboard::Clipboard));
// if (!pm.isNull())
//  setValueInternal(pm, true);
    if (dataSource().isEmpty()) {
        //static mode
        setData(KexiBLOBBuffer::self()->insertPixmap(pm));
    } else {
        //db-aware mode
        m_pixmap = pm;
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(IO_WriteOnly);
        if (m_pixmap.save(&buffer, "PNG")) {  // write pixmap into ba in PNG format
            setValueInternal(ba, true, false/* !loadPixmap */);
        } else {
            setValueInternal(QByteArray(), true);
        }
    }

    repaint();
    if (!dataSource().isEmpty()) {
//  emit pixmapChanged();
        signalValueChanged();
    }
}

void KexiDBImageBox::clear()
{
    if (dataSource().isEmpty()) {
        //static mode
        setData(KexiBLOBBuffer::Handle());
    } else {
        if (isReadOnly())
            return;
        //db-aware mode
        setValueInternal(QByteArray(), true);
        //m_pixmap = QPixmap();
    }

// m_originalFileName.clear();

    //! @todo emit signal for setting "dirty" flag within the design

// m_pixmap = QPixmap(); //will be loaded on demand
    repaint();
    if (!dataSource().isEmpty()) {
//  emit pixmapChanged();//valueChanged(data());
        signalValueChanged();
    }
}

void KexiDBImageBox::handleShowPropertiesAction()
{
    //! @todo
}

void KexiDBImageBox::slotUpdateActionsAvailabilityRequested(bool& valueIsNull, bool& valueIsReadOnly)
{
    valueIsNull = !(
                      (dataSource().isEmpty() && !pixmap().isNull()) /*static pixmap available*/
                      || (!dataSource().isEmpty() && !this->valueIsNull())  /*db-aware pixmap available*/
                  );
    // read-only if static pixmap or db-aware pixmap for read-only widget:
    valueIsReadOnly = !m_designMode && dataSource().isEmpty() || !dataSource().isEmpty() && isReadOnly()
                      || m_designMode && !dataSource().isEmpty();
}

/*
void KexiDBImageBox::slotAboutToHidePopupMenu()
{
// kexipluginsdbg << "##### slotAboutToHidePopupMenu() " << endl;
  m_clickTimer.start(50, true);
  if (m_chooser && m_chooser->isOn()) {
    m_chooser->toggle();
    if (m_setFocusOnButtonAfterClosingPopup) {
      m_setFocusOnButtonAfterClosingPopup = false;
      m_chooser->setFocus();
    }
  }
}*/

void KexiDBImageBox::contextMenuEvent(QContextMenuEvent * e)
{
    if (popupMenuAvailable())
        m_contextMenu->exec(e->globalPos());
}

/*void KexiDBImageBox::slotChooserPressed()
{
// if (!m_clickTimer.isActive())
//  return;
// m_chooser->setDown( false );
}

void KexiDBImageBox::slotChooserReleased()
{
}

void KexiDBImageBox::slotToggled(bool on)
{
  return;

// kexipluginsdbg << "##### slotToggled() " << on << endl;
  if (m_clickTimer.isActive() || !on) {
    m_chooser->disableMousePress = true;
    return;
  }
  m_chooser->disableMousePress = false;
  QRect screen = qApp->desktop()->availableGeometry( m_chooser );
  QPoint p;
  if ( QApplication::reverseLayout() ) {
    if ( (mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_contextMenu->sizeHint().height()) <= screen.height() )
      p = m_chooser->mapToGlobal( m_chooser->rect().bottomRight() );
    else
      p = m_chooser->mapToGlobal( m_chooser->rect().topRight() - QPoint( 0, m_contextMenu->sizeHint().height() ) );
    p.rx() -= m_contextMenu->sizeHint().width();
  }
  else {
    if ( (m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() ).y() + m_contextMenu->sizeHint().height()) <= screen.height() )
      p = m_chooser->mapToGlobal( m_chooser->rect().bottomLeft() );
    else
      p = m_chooser->mapToGlobal( m_chooser->rect().topLeft() - QPoint( 0, m_contextMenu->sizeHint().height() ) );
  }
  if (!m_contextMenu->isVisible() && on) {
    m_contextMenu->exec( p, -1 );
    m_contextMenu->setFocus();
  }
  //m_chooser->setDown( false );
}*/

void KexiDBImageBox::updateActionStrings()
{
    if (!m_contextMenu)
        return;
    if (m_designMode) {
        /*  QString titleString( i18n("Image Box") );
            if (!dataSource().isEmpty())
              titleString.prepend(dataSource() + " : ");
            m_contextMenu->changeTitle(m_contextMenu->idAt(0), m_contextMenu->titlePixmap(m_contextMenu->idAt(0)), titleString);*/
    } else {
        //update title in data view mode, based on the data source
        if (columnInfo()) {
            KexiImageContextMenu::updateTitle(m_contextMenu, columnInfo()->captionOrAliasOrName(),
                                              KexiFormPart::library()->iconName(metaObject()->className()));
        }
    }

    if (m_chooser) {
        if (popupMenuAvailable() && dataSource().isEmpty()) { //this may work in the future (see @todo below)
            m_chooser->setToolTip(i18n("Click to show actions for this image box"));
        } else {
            QString beautifiedImageBoxName;
            if (m_designMode) {
                beautifiedImageBoxName = dataSource();
            } else {
                beautifiedImageBoxName = columnInfo() ? columnInfo()->captionOrAliasOrName() : QString();
                /*! @todo look at makeFirstCharacterUpperCaseInCaptions setting [bool]
                 (see doc/dev/settings.txt) */
                beautifiedImageBoxName = beautifiedImageBoxName[0].toUpper() + beautifiedImageBoxName.mid(1);
            }
            m_chooser->setToolTip(
                i18n("Click to show actions for \"%1\" image box", beautifiedImageBoxName));
        }
    }
}

bool KexiDBImageBox::popupMenuAvailable()
{
    /*! @todo add kexi-global setting which anyway, allows to show this button
              (read-only actions like copy/save as/print can be available) */
    //chooser button can be only visible when data source is specified
    return !dataSource().isEmpty();
}

void KexiDBImageBox::setDataSource(const QString &ds)
{
    KexiFormDataItemInterface::setDataSource(ds);
    setData(KexiBLOBBuffer::Handle());
    updateActionStrings();
    KexiFrame::setFocusPolicy(focusPolicy());   //set modified policy

    if (m_chooser) {
        m_chooser->setEnabled(popupMenuAvailable());
        if (m_dropDownButtonVisible && popupMenuAvailable()) {
            m_chooser->show();
        } else {
            m_chooser->hide();
        }
    }

    // update some properties s not changed by user
//! @todo get default line width from global style settings
    if (!m_lineWidthChanged) {
        KexiFrame::setLineWidth(ds.isEmpty() ? 0 : 1);
    }
    if (!m_paletteBackgroundColorChanged && parentWidget()) {
        KexiFrame::setPaletteBackgroundColor(
            dataSource().isEmpty() ? parentWidget()->paletteBackgroundColor() : palette().active().base());
    }
}

QSize KexiDBImageBox::sizeHint() const
{
    if (pixmap().isNull())
        return QSize(80, 80);
    return pixmap().size();
}

int KexiDBImageBox::realLineWidth() const
{
    if (frameShape() == QFrame::Box
            && (frameShadow() == QFrame::Sunken || frameShadow() == QFrame::Raised)) {
        return 2 * lineWidth();
    } else {
        return lineWidth();
    }
}

void KexiDBImageBox::paintEvent(QPaintEvent *pe)
{
    if (!m_paintEventEnabled)
        return;
    QPainter p(this);
    p.setClipRect(pe->rect());
    const int _realLineWidth = realLineWidth();
    KexiUtils::WidgetMargins margins(this);
    margins += KexiUtils::WidgetMargins(_realLineWidth);
//Qt3 replaced with 'margins': const int m = realLineWidth() + margin();
    QColor bg(palette().color(backgroundRole())); //Qt3 eraseColor());
    if (m_designMode && pixmap().isNull()) {
        QPixmap pm(size() - QSize(margins.left + margins.right, margins.top + margins.bottom));
        QPainter p2;
//Qt3  p2.begin(&pm, this);
        p2.begin(&pm);
        p2.fillRect(0, 0, width(), height(), bg);

        updatePixmap();
        QPixmap *imagBoxPm;
        const bool tooLarge = (height() - margins.top - margins.bottom) <= KexiDBImageBox_static->pixmap->height();
        if (tooLarge || (width() - margins.left - margins.right) <= KexiDBImageBox_static->pixmap->width())
            imagBoxPm = KexiDBImageBox_static->small;
        else
            imagBoxPm = KexiDBImageBox_static->pixmap;
        QImage img(imagBoxPm->toImage());
#ifdef __GNUC__
#warning Qt4 TODO KexiDBImageBox::paintEvent()
#else
#pragma WARNING( Qt4 TODO KexiDBImageBox::paintEvent() )
#endif
        /* TODO
            img = KImageEffect::flatten(img, bg.dark(150),
              qGray( bg.rgb() ) <= 20 ? QColor(Qt::gray).dark(150) : bg.light(105));*/

        QPixmap converted(QPixmap::fromImage(img));
//  if (tooLarge)
//   p2.drawPixmap(2, 2, converted);
//  else
        p2.drawPixmap(2, height() - margins.top - margins.bottom - imagBoxPm->height() - 2, converted);
        QFont f(qApp->font());
        p2.setFont(f);
        p2.setPen(KexiUtils::contrastColor(bg));
        p2.drawText(pm.rect(), Qt::AlignCenter,
                    dataSource().isEmpty()
                    ? objectName() + "\n" + i18nc("Unbound Image Box", "(unbound)")
                    : dataSource());
        p2.end();
        bitBlt(this, margins.left, margins.top, &pm);
    } else {
        QSize internalSize(size());
        if (m_chooser && m_dropDownButtonVisible && !dataSource().isEmpty())
            internalSize.setWidth(internalSize.width() - m_chooser->width());

        //clearing needed here because we may need to draw a pixmap with transparency
        p.fillRect(0, 0, width(), height(), bg);

        KexiUtils::drawPixmap(p, margins, QRect(QPoint(0, 0), internalSize), pixmap(), m_alignment,
                              m_scaledContents, m_keepAspectRatio);
    }
    KexiFrame::drawFrame(&p);

    // if the widget is focused, draw focus indicator rect _if_ there is no chooser button
    if (!m_designMode && !dataSource().isEmpty()
            && hasFocus()
            && (!m_chooser || !m_chooser->isVisible())) {
        QStyleOptionFocusRect option;
        option.initFrom(this);
        //option.rect = style().subRect(QStyle::SR_PushButtonContents);
        style()->drawPrimitive(
            QStyle::PE_FrameFocusRect, &option, &p, this
            /*Qt4 , palette().active()*/);
    }
}

/*  virtual void KexiDBImageBox::paletteChange ( const QPalette & oldPalette )
{
  QFrame::paletteChange(oldPalette);
  if (oldPalette.active().background()!=palette().active().background()) {
    delete KexiDBImageBox_pm;
    KexiDBImageBox_pm = 0;
    repaint();
  }
}*/

void KexiDBImageBox::updatePixmap()
{
    if (!(m_designMode && pixmap().isNull()))
        return;

    if (!KexiDBImageBox_static->pixmap) {
        QString fname(KStandardDirs::locate("data", QString("kexi/pics/imagebox.png")));
        KexiDBImageBox_static->pixmap = new QPixmap(fname, "PNG");
        QImage img(KexiDBImageBox_static->pixmap->toImage());
        KexiDBImageBox_static->small
        = new QPixmap(
            QPixmap::fromImage(
                img.scaled(
                    img.width() / 2, img.height() / 2, Qt::KeepAspectRatio,
                    Qt::SmoothTransformation)
            )
        );
    }
}

void KexiDBImageBox::setAlignment(Qt::Alignment alignment)
{
    m_alignment = alignment;
    if (!m_scaledContents || m_keepAspectRatio)
        repaint();
}

void KexiDBImageBox::setData(const KexiBLOBBuffer::Handle& handle)
{
    if (m_insideSetData) //avoid recursion
        return;
    m_insideSetData = true;
    m_data = handle;
    emit idChanged(handle.id());
    m_insideSetData = false;
    update();
}

void KexiDBImageBox::resizeEvent(QResizeEvent * e)
{
    KexiFrame::resizeEvent(e);
    if (m_chooser) {
        QSize s(m_chooser->sizeHint());
        QSize margin(realLineWidth(), realLineWidth());
        s.setHeight(height() - 2*margin.height());
        s = s.boundedTo(size() - 2 * margin);
        m_chooser->resize(s);
        m_chooser->move(QRect(QPoint(0, 0), e->size() - m_chooser->size() - margin + QSize(1, 1)).bottomRight());
    }
}

/*
bool KexiDBImageBox::setProperty( const char * name, const QVariant & value )
{
  const bool ret = QLabel::setProperty(name, value);
  if (p_shadowEnabled) {
    if (0==qstrcmp("format-indent-more", name) || 0==qstrcmp("font", name) || 0==qstrcmp("margin", name)
      || 0==qstrcmp("frameShadow", name) || 0==qstrcmp("frameShape", name)
      || 0==qstrcmp("frameStyle", name) || 0==qstrcmp("midLineWidth", name)
      || 0==qstrcmp("lineWidth", name)) {
      p_privateLabel->setProperty(name, value);
      updatePixmap();
    }
  }
  return ret;
}
*/

void KexiDBImageBox::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
    KexiFormDataItemInterface::setColumnInfo(cinfo);
    //updating strings and title is needed
    updateActionStrings();
}

bool KexiDBImageBox::keyPressed(QKeyEvent *ke)
{
    // Esc key should close the popup
    if (ke->modifiers() == Qt::NoModifier && ke->key() == Qt::Key_Escape) {
        if (m_contextMenu->isVisible()) {
            m_setFocusOnButtonAfterClosingPopup = true;
            return true;
        }
    }
// else if (ke->modifiers() == Qt::ControlButton && KStdAccel::shortcut(KStdAccel::Copy).keyCodeQt() == (ke->key()|Qt::CTRL)) {
// }
    return false;
}

void KexiDBImageBox::setLineWidth(int width)
{
    m_lineWidthChanged = true;
    KexiFrame::setLineWidth(width);
}

void KexiDBImageBox::setPalette(const QPalette &pal)
{
    KexiFrame::setPalette(pal);
    if (m_insideSetPalette)
        return;
    m_insideSetPalette = true;
    setPaletteBackgroundColor(pal.active().base());
    setPaletteForegroundColor(pal.active().foreground());
    m_insideSetPalette = false;
}

void KexiDBImageBox::setPaletteBackgroundColor(const QColor & color)
{
    kexipluginsdbg << "KexiDBImageBox::setPaletteBackgroundColor(): " << color.name() << endl;
    m_paletteBackgroundColorChanged = true;
    KexiFrame::setPaletteBackgroundColor(color);
    if (m_chooser)
        m_chooser->setPalette(qApp->palette());
}

bool KexiDBImageBox::dropDownButtonVisible() const
{
    return m_dropDownButtonVisible;
}

void KexiDBImageBox::setDropDownButtonVisible(bool set)
{
//! @todo use global default setting for this property
    if (m_dropDownButtonVisible == set)
        return;
    m_dropDownButtonVisible = set;
    if (m_chooser) {
        if (m_dropDownButtonVisible)
            m_chooser->show();
        else
            m_chooser->hide();
    }
}

bool KexiDBImageBox::subwidgetStretchRequired(KexiDBAutoField* autoField) const
{
    Q_UNUSED(autoField);
    return true;
}

bool KexiDBImageBox::eventFilter(QObject * watched, QEvent * e)
{
    if (watched == this || watched == m_chooser) { //we're watching chooser as well because it's a focus proxy even if invisible
        if (e->type() == QEvent::FocusIn || e->type() == QEvent::FocusOut || e->type() == QEvent::MouseButtonPress) {
            update(); //to repaint focus rect
        }
    }
    // hide popup menu as soon as it loses focus
    if (watched == m_contextMenu && e->type() == QEvent::FocusOut) {
        m_contextMenu->hide();
    }
    return KexiFrame::eventFilter(watched, e);
}

Qt::FocusPolicy KexiDBImageBox::focusPolicy() const
{
    if (dataSource().isEmpty())
        return Qt::NoFocus;
    return m_focusPolicyInternal;
}

Qt::FocusPolicy KexiDBImageBox::focusPolicyInternal() const
{
    return m_focusPolicyInternal;
}

void KexiDBImageBox::setFocusPolicy(Qt::FocusPolicy policy)
{
    m_focusPolicyInternal = policy;
    KexiFrame::setFocusPolicy(focusPolicy());   //set modified policy
}

#include "kexidbimagebox.moc"
