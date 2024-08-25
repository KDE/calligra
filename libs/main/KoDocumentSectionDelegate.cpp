/*
  SPDX-FileCopyrightText: 2006 Gábor Lehel <illissius@gmail.com>
  SPDX-FileCopyrightText: 2008 Cyrille Berger <cberger@cberger.net>
  SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoDocumentSectionDelegate.h"
#include "KoDocumentSectionModel.h"
#include "KoDocumentSectionToolTip.h"
#include "KoDocumentSectionView.h"
#include "KoItemToolTip.h"

#include <QApplication>
#include <QKeyEvent>
#include <QLibraryInfo>
#include <QLineEdit>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>
#include <QStyle>
#include <QStyleOptionViewItem>
#include <QToolTip>
#include <QVersionNumber>

#include <KLocalizedString>

class KoDocumentSectionDelegate::Private
{
public:
    Private() = default;

    KoDocumentSectionView *view = nullptr;
    QPointer<QWidget> edit = nullptr;
    KoDocumentSectionToolTip tip;
    static const int margin = 1;
};

KoDocumentSectionDelegate::KoDocumentSectionDelegate(KoDocumentSectionView *view, QObject *parent)
    : QAbstractItemDelegate(parent)
    , d(std::make_unique<Private>())
{
    d->view = view;
    view->setItemDelegate(this);
    QApplication::instance()->installEventFilter(this);
}

KoDocumentSectionDelegate::~KoDocumentSectionDelegate() = default;

QSize KoDocumentSectionDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (d->view->displayMode()) {
    case View::ThumbnailMode: {
        const int height = thumbnailHeight(option, index) + textBoxHeight(option) + d->margin * 2;
        return QSize(availableWidth(), height);
    }
    case View::DetailedMode:
        return QSize(option.rect.width(), textBoxHeight(option) + option.decorationSize.height() + d->margin);
    case View::MinimalMode:
        return QSize(option.rect.width(), textBoxHeight(option));
    default:
        return option.rect.size();
    }
}

void KoDocumentSectionDelegate::paint(QPainter *p, const QStyleOptionViewItem &o, const QModelIndex &index) const
{
    p->save();
    {
        QStyleOptionViewItem option = getOptions(o, index);
        QStyle *style = option.widget ? option.widget->style() : QApplication::style();
        style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, p, option.widget);

        p->setFont(option.font);

        drawText(p, option, index);
        drawIcons(p, option, index);
        drawThumbnail(p, option, index);
        drawDecoration(p, option, index);
        drawProgressBar(p, option, index);
    }
    p->restore();
}

bool KoDocumentSectionDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if ((event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonDblClick) && (index.flags() & Qt::ItemIsEnabled)) {
        auto mouseEvent = static_cast<QMouseEvent *>(event);

        const QRect iconsRect_ = iconsRect(option, index).translated(option.rect.topLeft());

        if (iconsRect_.isValid() && iconsRect_.contains(mouseEvent->pos())) {
            const int iconWidth = option.decorationSize.width();
            int xPos = mouseEvent->pos().x() - iconsRect_.left();
            if (xPos % (iconWidth + d->margin) < iconWidth) { // it's on an icon, not a margin
                Model::PropertyList propertyList = index.data(Model::PropertiesRole).value<Model::PropertyList>();
                int clickedProperty = -1;
                // Discover which of all properties was clicked
                for (int i = 0; i < propertyList.count(); ++i) {
                    if (propertyList[i].isMutable) {
                        xPos -= iconWidth + d->margin;
                    }
                    ++clickedProperty;
                    if (xPos < 0)
                        break;
                }
                // Using Ctrl+click to enter stasis
                if (mouseEvent->modifiers() == Qt::ControlModifier && propertyList[clickedProperty].canHaveStasis) {
                    // STEP 0: Prepare to Enter or Leave control key stasis
                    quint16 numberOfLeaves = model->rowCount(index.parent());
                    QModelIndex eachItem;
                    // STEP 1: Go.
                    if (propertyList[clickedProperty].isInStasis == false) { // Enter
                        /* Make every leaf of this node go State = False, saving the old property value to stateInStasis */
                        for (quint16 i = 0; i < numberOfLeaves; ++i) { // Foreach leaf in the node (index.parent())
                            eachItem = model->index(i, 0, index.parent());
                            // The entire property list has to be altered because model->setData cannot set individual properties
                            Model::PropertyList eachPropertyList = eachItem.data(Model::PropertiesRole).value<Model::PropertyList>();
                            eachPropertyList[clickedProperty].stateInStasis = eachPropertyList[clickedProperty].state.toBool();
                            eachPropertyList[clickedProperty].state = false;
                            eachPropertyList[clickedProperty].isInStasis = true;
                            model->setData(eachItem, QVariant::fromValue(eachPropertyList), Model::PropertiesRole);
                        }
                        /* Now set the current node's clickedProperty back to True, to save the user time
                        (obviously, if the user is clicking one item with ctrl+click, that item should
                        have a True property, value while the others are in stasis and set to False) */
                        // First refresh propertyList, otherwise old data will be saved back causing bugs
                        propertyList = index.data(Model::PropertiesRole).value<Model::PropertyList>();
                        propertyList[clickedProperty].state = true;
                        model->setData(index, QVariant::fromValue(propertyList), Model::PropertiesRole);
                    } else { // Leave
                        /* Make every leaf of this node go State = stateInStasis */
                        for (quint16 i = 0; i < numberOfLeaves; ++i) {
                            eachItem = model->index(i, 0, index.parent());
                            // The entire property list has to be altered because model->setData cannot set individual properties
                            Model::PropertyList eachPropertyList = eachItem.data(Model::PropertiesRole).value<Model::PropertyList>();
                            eachPropertyList[clickedProperty].state = eachPropertyList[clickedProperty].stateInStasis;
                            eachPropertyList[clickedProperty].isInStasis = false;
                            model->setData(eachItem, QVariant::fromValue(eachPropertyList), Model::PropertiesRole);
                        }
                    }
                } else {
                    propertyList[clickedProperty].state = !propertyList[clickedProperty].state.toBool();
                    model->setData(index, QVariant::fromValue(propertyList), Model::PropertiesRole);
                }
            }
            return true;
        }

        if (mouseEvent->button() == Qt::LeftButton && mouseEvent->modifiers() == Qt::AltModifier) {
            d->view->setCurrentIndex(index);
            model->setData(index, true, Model::AlternateActiveRole);
            return true;
        }

        if (mouseEvent->button() != Qt::LeftButton) {
            d->view->setCurrentIndex(index);
            return false;
        }
    } else if (event->type() == QEvent::ToolTip) {
        auto helpEvent = dynamic_cast<QHelpEvent *>(event);
        static bool canRepositionPopups = !qApp->platformName().startsWith(QLatin1String("wayland")) || QLibraryInfo::version() >= QVersionNumber(6, 8, 0);
        if (canRepositionPopups) {
            d->tip.showTip(d->view, helpEvent->globalPos(), d->view->visualRect(index), option, index);
        }
        event->setAccepted(true);
        return true;
    } else if (event->type() == QEvent::Leave) {
        d->tip.hide();
    }

    return false;
}

bool KoDocumentSectionDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    return QAbstractItemDelegate::helpEvent(event, view, option, index);
}

QWidget *KoDocumentSectionDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &, const QModelIndex &) const
{
    d->edit = new QLineEdit(parent);
    d->edit->installEventFilter(const_cast<KoDocumentSectionDelegate *>(this)); // hack?
    return d->edit;
}

void KoDocumentSectionDelegate::setEditorData(QWidget *widget, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(widget);
    Q_ASSERT(edit);

    edit->setText(index.data(Qt::DisplayRole).toString());
}

void KoDocumentSectionDelegate::setModelData(QWidget *widget, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *edit = qobject_cast<QLineEdit *>(widget);
    Q_ASSERT(edit);

    model->setData(index, edit->text(), Qt::DisplayRole);
}

void KoDocumentSectionDelegate::updateEditorGeometry(QWidget *widget, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    widget->setGeometry(textRect(option, index).translated(option.rect.topLeft()));
}

// PROTECTED

bool KoDocumentSectionDelegate::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        if (d->edit) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            if (!QRect(d->edit->mapToGlobal(QPoint()), d->edit->size()).contains(me->globalPos()))
                Q_EMIT closeEditor(d->edit);
        }
    } break;
    case QEvent::KeyPress: {
        QLineEdit *edit = qobject_cast<QLineEdit *>(object);
        if (edit && edit == d->edit) {
            QKeyEvent *ke = static_cast<QKeyEvent *>(event);
            switch (ke->key()) {
            case Qt::Key_Escape:
                Q_EMIT closeEditor(edit);
                return true;
            case Qt::Key_Tab:
                Q_EMIT commitData(edit);
                Q_EMIT closeEditor(edit, EditNextItem);
                return true;
            case Qt::Key_Backtab:
                Q_EMIT commitData(edit);
                Q_EMIT closeEditor(edit, EditPreviousItem);
                return true;
            case Qt::Key_Return:
            case Qt::Key_Enter:
                Q_EMIT commitData(edit);
                Q_EMIT closeEditor(edit);
                return true;
            default:
                break;
            }
        }
    } break;
    case QEvent::FocusOut: {
        QLineEdit *edit = qobject_cast<QLineEdit *>(object);
        if (edit && edit == d->edit) {
            Q_EMIT commitData(edit);
            Q_EMIT closeEditor(edit);
        }
    }
    default:
        break;
    }

    return QAbstractItemDelegate::eventFilter(object, event);
}

// PRIVATE

QStyleOptionViewItem KoDocumentSectionDelegate::getOptions(const QStyleOptionViewItem &o, const QModelIndex &index)
{
    QStyleOptionViewItem option = o;
    QVariant v = index.data(Qt::FontRole);
    if (v.isValid()) {
        option.font = v.value<QFont>();
        option.fontMetrics = QFontMetrics(option.font);
    }
    v = index.data(Qt::TextAlignmentRole);
    if (v.isValid())
        option.displayAlignment = QFlag(v.toInt());
    v = index.data(Qt::ForegroundRole);
    if (v.isValid())
        option.palette.setColor(QPalette::Text, v.value<QColor>());
    v = index.data(Qt::BackgroundRole);
    if (v.isValid())
        option.palette.setColor(QPalette::Window, v.value<QColor>());

    return option;
}

int KoDocumentSectionDelegate::thumbnailHeight(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QSize size = index.data(Qt::SizeHintRole).toSize();
    int width = option.rect.width();
    if (!option.rect.isValid())
        width = availableWidth();
    if (size.width() <= width)
        return size.height();
    else
        return int(width / (qreal(size.width()) / size.height()));
}

int KoDocumentSectionDelegate::availableWidth() const
{
    return d->view->width(); // not viewport()->width(), otherwise we get infinite scrollbar addition/removal!
}

int KoDocumentSectionDelegate::textBoxHeight(const QStyleOptionViewItem &option) const
{
    return qMax(option.fontMetrics.height(), option.decorationSize.height());
}

QRect KoDocumentSectionDelegate::textRect(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (d->view->displayMode() == View::ThumbnailMode) {
        const QRect r = decorationRect(option, index);
        const int left = r.right() + d->margin;
        return QRect(left, r.top(), option.rect.width() - left, textBoxHeight(option));
    } else {
        static QFont f;
        static int minbearing = 1337 + 666; // can be 0 or negative, 2003 is less likely
        if (minbearing == 2003 || f != option.font) {
            f = option.font; // getting your bearings can be expensive, so we cache them
            minbearing = option.fontMetrics.minLeftBearing() + option.fontMetrics.minRightBearing();
        }

        int indent = decorationRect(option, index).right() + d->margin;

        const int width =
            (d->view->displayMode() == View::DetailedMode ? option.rect.width() : iconsRect(option, index).left()) - indent - d->margin + minbearing;

        return QRect(indent, 0, width, textBoxHeight(option));
    }
}

QRect KoDocumentSectionDelegate::iconsRect(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (d->view->displayMode() == View::ThumbnailMode)
        return QRect();

    Model::PropertyList lp = index.data(Model::PropertiesRole).value<Model::PropertyList>();
    int propscount = 0;
    for (int i = 0, n = lp.count(); i < n; ++i)
        if (lp[i].isMutable)
            propscount++;

    const int iconswidth = propscount * option.decorationSize.width() + (propscount - 1) * d->margin;

    const int x = d->view->displayMode() == View::DetailedMode ? thumbnailRect(option, index).right() + d->margin : option.rect.width() - iconswidth;
    const int y = d->view->displayMode() == View::DetailedMode ? textBoxHeight(option) + d->margin : 0;

    return QRect(x, y, iconswidth, option.decorationSize.height());
}

QRect KoDocumentSectionDelegate::thumbnailRect(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (d->view->displayMode() == View::ThumbnailMode)
        return QRect(0, 0, option.rect.width(), thumbnailHeight(option, index));
    else
        return QRect(0, 0, option.rect.height(), option.rect.height());
}

QRect KoDocumentSectionDelegate::decorationRect(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int width = option.decorationSize.width();
    if (index.data(Qt::DecorationRole).value<QIcon>().isNull())
        width = 0;
    switch (d->view->displayMode()) {
    case View::ThumbnailMode: {
        QFont font = option.font;
        if (index.data(Model::ActiveRole).toBool())
            font.setBold(!font.bold());
        const QFontMetrics metrics(font);
        const int totalwidth = metrics.boundingRect(index.data(Qt::DisplayRole).toString()).width() + width + d->margin;
        int left;
        if (totalwidth < option.rect.width())
            left = (option.rect.width() - totalwidth) / 2;
        else
            left = 0;
        return QRect(left, thumbnailRect(option, index).bottom() + d->margin, width, textBoxHeight(option));
    }
    case View::DetailedMode:
    case View::MinimalMode: {
        const int left = thumbnailRect(option, index).right() + d->margin;
        return QRect(left, 0, width, textBoxHeight(option));
    }
    default:
        return QRect();
    }
}

QRect KoDocumentSectionDelegate::progressBarRect(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (d->view->displayMode() == View::ThumbnailMode)
        return QRect();
    QRect iconsRect_ = iconsRect(option, index);
    int width = d->view->width() / 4;
    if (d->view->displayMode() == View::DetailedMode) {
        // In detailed mode the progress bar take 50% width on the right of the icons
        return QRect(option.rect.width() - width - d->margin, iconsRect_.top(), width, iconsRect_.height());
    } else {
        // In minimal mode the progress bar take 50% width on the left of icons
        return QRect(iconsRect_.left() - width - d->margin, iconsRect_.top(), width, iconsRect_.height());
    }
}

void KoDocumentSectionDelegate::drawText(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QRect r = textRect(option, index).translated(option.rect.topLeft());

    p->save();
    {
        p->setClipRect(r);
        p->translate(r.left(), r.top());
        QPalette::ColorGroup cg = (option.state & QStyle::State_Enabled) ? QPalette::Active : QPalette::Disabled;
        QPalette::ColorRole cr = (option.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text;
        p->setPen(option.palette.color(cg, cr));

        if (index.data(Model::ActiveRole).toBool()) {
            QFont f = p->font();
            f.setBold(!f.bold());
            p->setFont(f);
        }

        const QString text = index.data(Qt::DisplayRole).toString();
        const QString elided = p->fontMetrics().elidedText(text, Qt::ElideRight, r.width());
        p->drawText(d->margin, 0, r.width(), r.height(), Qt::AlignLeft | Qt::AlignTop, elided);
    }
    p->restore();
}

void KoDocumentSectionDelegate::drawIcons(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QRect r = iconsRect(option, index).translated(option.rect.topLeft());

    p->save();
    {
        p->setClipRect(r);
        p->translate(r.left(), r.top());
        int x = 0;
        Model::PropertyList lp = index.data(Model::PropertiesRole).value<Model::PropertyList>();
        for (int i = 0, n = lp.count(); i < n; ++i) {
            if (lp[i].isMutable) {
                QIcon icon = lp[i].state.toBool() ? lp[i].onIcon : lp[i].offIcon;
                p->drawPixmap(x, 0, icon.pixmap(option.decorationSize, (option.state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled));
                x += option.decorationSize.width() + d->margin;
            }
        }
    }
    p->restore();
}

void KoDocumentSectionDelegate::drawThumbnail(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QRect r = thumbnailRect(option, index).translated(option.rect.topLeft());

    p->save();
    {
        p->setClipRect(r);
        const qreal myratio = qreal(r.width()) / r.height();
        const qreal thumbratio = index.data(Model::AspectRatioRole).toDouble();
        const int s = (myratio > thumbratio) ? r.height() : r.width();

        QImage img = index.data(int(Model::BeginThumbnailRole) + s).value<QImage>();
        if (!(option.state & QStyle::State_Enabled)) {
            // Make the image grayscale
            // TODO: if someone feel bored a more optimized version of this would be welcome
            for (int i = 0; i < img.width(); ++i) {
                for (int j = 0; j < img.width(); ++j) {
                    img.setPixel(i, j, qGray(img.pixel(i, j)));
                }
            }
        }
        QPoint offset;
        offset.setX(r.width() / 2 - img.width() / 2);
        offset.setY(r.height() / 2 - img.height() / 2);

        if (!img.isNull() && img.width() > 0 && img.height() > 0) {
            p->drawImage(r.topLeft() + offset, img);
        }
    }
    p->restore();
}

void KoDocumentSectionDelegate::drawDecoration(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const QRect r = decorationRect(option, index).translated(option.rect.topLeft());

    p->save();
    {
        p->setClipRect(r);
        p->translate(r.topLeft());
        if (!index.data(Qt::DecorationRole).value<QIcon>().isNull())
            p->drawPixmap(0,
                          0,
                          index.data(Qt::DecorationRole)
                              .value<QIcon>()
                              .pixmap(option.decorationSize, (option.state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled));
    }
    p->restore();
}

void KoDocumentSectionDelegate::drawProgressBar(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QVariant value = index.data(KoDocumentSectionModel::ProgressRole);
    if (!value.isNull() && (value.toInt() >= 0 && value.toInt() <= 100)) {
        const QRect r = progressBarRect(option, index).translated(option.rect.topLeft());
        p->save();
        {
            p->setClipRect(r);
            QStyle *style = QApplication::style();
            QStyleOptionProgressBar opt;

            opt.minimum = 0;
            opt.maximum = 100;
            opt.progress = value.toInt();
            opt.textVisible = true;
            opt.textAlignment = Qt::AlignHCenter;
            opt.text = i18n("%1 %", opt.progress);
            opt.rect = r;
            opt.state = QStyle::State_Horizontal;
            opt.state = option.state;
            style->drawControl(QStyle::CE_ProgressBar, &opt, p, nullptr);
        }
        p->restore();
    }
}
