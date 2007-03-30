/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "KarbonLineStyleSelector.h"

#include <QPen>
#include <QPainter>
#include <QAbstractListModel>
#include <QAbstractItemDelegate>

#include <kdebug.h>


/// The line style model managing the style data
class KarbonLineStyleModel : public QAbstractListModel
{
public:
    KarbonLineStyleModel( QObject * parent = 0 );
    virtual ~KarbonLineStyleModel() {}
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /// adds the given style to the model
    bool addCustomStyle( const QVector<qreal> &style );
    /// selects the given style
    int setLineStyle( Qt::PenStyle style, const QVector<qreal> &dashes );
private:
    QList< QVector<qreal> > m_styles; ///< the added styles
    QVector<qreal> m_tempStyle; ///< a temporary added style
    bool m_hasTempStyle;        ///< state of the temporary style
};


/// The line style item delegate for rendering the styles
class KarbonLineStyleItemDelegate : public QAbstractItemDelegate
{
public:
    KarbonLineStyleItemDelegate( QObject * parent = 0 );
    ~KarbonLineStyleItemDelegate() {}
    void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;
};

class KarbonLineStyleSelector::Private
{
public:
    Private( QWidget * parent )
    : model( new KarbonLineStyleModel( parent ) )
    {
    }

    KarbonLineStyleModel * model;
};


KarbonLineStyleSelector::KarbonLineStyleSelector( QWidget * parent )
    : QComboBox( parent ), d( new Private( this ) )
{
    setModel( d->model );
    setItemDelegate( new KarbonLineStyleItemDelegate( this ) );
}

void KarbonLineStyleSelector::paintEvent( QPaintEvent *pe )
{
    QComboBox::paintEvent( pe );

    QStyleOptionComboBox option;
    option.initFrom( this );
    QRect r = style()->subControlRect( QStyle::CC_ComboBox, &option, QStyle::SC_ComboBoxEditField, this );

    QPen pen = itemData( currentIndex(), Qt::DecorationRole ).value<QPen>();

    QPainter painter( this );
    painter.setPen( pen );
    painter.drawLine( r.left(), r.center().y(), r.right(), r.center().y() );
}

bool KarbonLineStyleSelector::addCustomStyle( const QVector<qreal> &style )
{
    return d->model->addCustomStyle( style );
}

void KarbonLineStyleSelector::setLineStyle( Qt::PenStyle style, const QVector<qreal> &dashes )
{
    int index = d->model->setLineStyle( style, dashes );
    if( index >= 0 )
        setCurrentIndex( index );
}

Qt::PenStyle KarbonLineStyleSelector::lineStyle() const
{
    QPen pen = itemData( currentIndex(), Qt::DecorationRole ).value<QPen>();
    return pen.style();
}

QVector<qreal> KarbonLineStyleSelector::lineDashes() const
{
    QPen pen = itemData( currentIndex(), Qt::DecorationRole ).value<QPen>();
    return pen.dashPattern();
}

//################## The Model #############################################

KarbonLineStyleModel::KarbonLineStyleModel( QObject * parent )
    : QAbstractListModel( parent ), m_hasTempStyle( false )
{
    // add standard dash patterns
    for( int i = Qt::NoPen; i < Qt::CustomDashLine; i++ )
    {
        QPen pen( static_cast<Qt::PenStyle>(i) );
        m_styles << pen.dashPattern();
    }
}

int KarbonLineStyleModel::rowCount( const QModelIndex &/*parent*/ ) const
{
    return m_styles.count() + ( m_hasTempStyle ? 1 : 0 );
}

QVariant KarbonLineStyleModel::data( const QModelIndex &index, int role ) const
{
    if( ! index.isValid() )
         return QVariant();

    switch( role )
    {
        case Qt::DecorationRole:
        {
            QPen pen( Qt::black );
            pen.setWidth( 3 );
            if( index.row() < Qt::CustomDashLine )
                pen.setStyle( static_cast<Qt::PenStyle>( index.row() ) );
            else if( index.row() < m_styles.count() )
                pen.setDashPattern( m_styles[index.row()] );
            else if( m_hasTempStyle )
                pen.setDashPattern( m_tempStyle );
            else
                pen.setStyle( Qt::NoPen );

            return QVariant( pen );
        }
        case Qt::SizeHintRole:
            return QSize( 100, 15 );
        default:
            return QVariant();
    }
}

bool KarbonLineStyleModel::addCustomStyle( const QVector<qreal> &style )
{
    if( m_styles.contains( style ) )
        return false;

    m_styles.append( style );
    return true;
}

int KarbonLineStyleModel::setLineStyle( Qt::PenStyle style, const QVector<qreal> &dashes )
{
    // check if we select a standard or custom style
    if( style < Qt::CustomDashLine )
    {
        // a standard style
        m_hasTempStyle = false;
        reset();
        return style;
    }
    else if( style == Qt::CustomDashLine )
    {
        // a custom style -> check if already added
        int index = m_styles.indexOf( dashes, Qt::CustomDashLine );
        if( index < 0 )
        {
            // not already added -> add temporarly
            m_tempStyle = dashes;
            m_hasTempStyle = true;
            reset();
            return m_styles.count();
        }
        else
        {
            // already added -> return index
            m_hasTempStyle = false;
            reset();
            return index;
        }
    }
    else
        return -1;
}

//################## The Delegate ##########################################

KarbonLineStyleItemDelegate::KarbonLineStyleItemDelegate( QObject * parent )
    : QAbstractItemDelegate( parent )
{
}

void KarbonLineStyleItemDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    painter->save();

    if( option.state & QStyle::State_Selected )
        painter->fillRect( option.rect, option.palette.highlight() );

    QPen pen = index.data( Qt::DecorationRole ).value<QPen>();

    painter->setPen( pen );
    painter->drawLine( option.rect.left(), option.rect.center().y(), option.rect.right(), option.rect.center().y() );

    painter->restore();
}

QSize KarbonLineStyleItemDelegate::sizeHint( const QStyleOptionViewItem &, const QModelIndex & ) const
{
    return QSize( 100, 15 );
}

#include "KarbonLineStyleSelector.moc"
