/*
    This file is part of the Kasten Framework, made within the KDE community.

    Copyright 2008-2009 Friedrich W. H. Kossebau <kossebau@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ZOOMSLIDER_H
#define ZOOMSLIDER_H


// Qt
#include <QtGui/QWidget>

class QSlider;
class QToolButton;


namespace Kasten2
{

class AbstractModel;
namespace If {
class Zoomable;
}


class ZoomSlider : public QWidget
{
  Q_OBJECT

  public:
    explicit ZoomSlider( QWidget* parent );
    virtual ~ZoomSlider();

  public:
    void setTargetModel( AbstractModel* model );

  private:
    void updateToolTip( int sliderValue );

  private Q_SLOTS: // action slots
    void zoomIn();
    void zoomOut();
    void onSliderValueChanged( int sliderValue );
    void onSliderMoved( int sliderValue );

  private Q_SLOTS:
    void onZoomLevelChange( double level );

  private:
    AbstractModel* mModel;
    If::Zoomable* mZoomControl;

    double mZoomLevel;

    QSlider* mSlider;
    QToolButton* mZoomInButton;
    QToolButton* mZoomOutButton;
};

}

#endif
