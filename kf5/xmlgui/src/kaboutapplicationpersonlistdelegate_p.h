/* This file is part of the KDE libraries
   Copyright (C) 2010 Teo Mrnjavac <teo@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KABOUT_APPLICATION_PERSON_LIST_DELEGATE_H
#define KABOUT_APPLICATION_PERSON_LIST_DELEGATE_H

#include <kwidgetitemdelegate.h>

#include <QLabel>

namespace KDEPrivate
{
class KAboutApplicationPersonProfile;

class KAboutApplicationPersonListDelegate : public KWidgetItemDelegate
{
    Q_OBJECT
public:
    KAboutApplicationPersonListDelegate( QAbstractItemView *itemView,
                                         QObject *parent = 0 );

    ~KAboutApplicationPersonListDelegate() {}

    void paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    QSize sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const;

    QList< QWidget *> createItemWidgets() const;

    void updateItemWidgets( const QList<QWidget *> widgets,
                            const QStyleOptionViewItem &option,
                            const QPersistentModelIndex &index ) const;

private Q_SLOTS:
    void launchUrl( QAction *action ) const;

private:
    int heightForString( const QString &string, int lineWidth, const QStyleOptionViewItem &option ) const;
    QString buildTextForProfile( const KAboutApplicationPersonProfile &profile ) const;
    QRect widgetsRect( const QStyleOptionViewItem &option, const QPersistentModelIndex &index ) const;

    enum DelegateWidgets
    {
        TextLabel = 0,
        MainLinks,
        SocialLinks
    };

    enum MainLinkActions
    {
        EmailAction = 0,
        HomepageAction,
        VisitProfileAction
    };
};

} //namespace KDEPrivate

#endif // KABOUT_APPLICATION_PERSON_LIST_DELEGATE_H
