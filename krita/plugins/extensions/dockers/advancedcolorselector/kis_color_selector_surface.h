/*
 *  Copyright (c) 2010 Adam Celarek <kdedev at xibo dot at>
 *  Copyright (c) 2012 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _KIS_COLOR_SELECTOR_SURFACE_H_
#define _KIS_COLOR_SELECTOR_SURFACE_H_

#include <qnamespace.h>
#include <QColor>

class KisColorSelectorInterface;
class QColor;
class QPainter;
class KisColorSelectorTriangle;
class KisColorSelectorRing;
class KisColorSelectorComponent;
class KisColorSelectorSimple;
class KisColorSelectorWheel;

class KisColorSelectorSurface
{
public:
    enum Type {Ring, Square, Wheel, Triangle, Slider};
    enum Parameters {H, hsvS, V, hslS, L, SL, SV, SV2, hsvSH, hslSH, VH, LH};

    struct Configuration {
        Type mainType;
        Type subType;
        Parameters mainTypeParameter;
        Parameters subTypeParameter;
        Configuration(Type mainT = Triangle,
                              Type subT = Ring,
                              Parameters mainTP = SL,
                              Parameters subTP = H)
                                  : mainType(mainT),
                                  subType(subT),
                                  mainTypeParameter(mainTP),
                                  subTypeParameter(subTP)
        {}
        Configuration(QString string)
        {
            readString(string);
        }

        QString toString() const
        {
            return QString("%1|%2|%3|%4").arg(mainType).arg(subType).arg(mainTypeParameter).arg(subTypeParameter);
        }
        void readString(QString string)
        {
            QStringList strili = string.split('|');
            if(strili.length()!=4) return;

            int imt=strili.at(0).toInt();
            int ist=strili.at(1).toInt();
            int imtp=strili.at(2).toInt();
            int istp=strili.at(3).toInt();

            if(imt>Slider || ist>Slider || imtp>LH || istp>LH)
                return;

            mainType = Type(imt);
            subType = Type(ist);
            mainTypeParameter = Parameters(imtp);
            subTypeParameter = Parameters(istp);
        }
        static Configuration fromString(QString string)
        {
            Configuration ret;
            ret.readString(string);
            return ret;
        }
    };
public:
    KisColorSelectorSurface(KisColorSelectorInterface* colorSelectorInterface);
    ~KisColorSelectorSurface();
    void setConfiguration(KisColorSelectorSurface::Configuration conf);
    const KisColorSelectorSurface::Configuration& configuration() const;
    
    KisColorSelectorComponent* mainComponent();
    KisColorSelectorComponent* subComponent();
    KisColorSelectorComponent* grabbingComponent();
    void paint(QPainter* p);
    void resize(int width, int height, int wheelSquareMargin);
    void processMousePress(int x, int y, Qt::MouseButtons _buttons);
    void processMouseMove(int x, int y, Qt::MouseButtons _buttons);
    void processMouseRelease(int x, int y, Qt::MouseButtons _buttons);
    const QColor& currentColor() const;
    void setColor(const QColor &color);
private:
    KisColorSelectorRing* m_ring;
    KisColorSelectorComponent* m_triangle;
    KisColorSelectorSimple* m_slider;
    KisColorSelectorSimple* m_square;
    KisColorSelectorWheel* m_wheel;
    KisColorSelectorComponent* m_mainComponent;
    KisColorSelectorComponent* m_subComponent;
    KisColorSelectorComponent* m_grabbingComponent;
    
    Configuration m_configuration;
    QColor m_currentColor;
};

#endif
