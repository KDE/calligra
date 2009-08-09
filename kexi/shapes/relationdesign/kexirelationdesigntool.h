/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIRELATIONDESIGNTOOL_H
#define KEXIRELATIONDESIGNTOOL_H

#include <KoTool.h>
#include <core/kexi.h>

class KexiRelationDesignShape;
class KexiStartupDialog;
class KComboBox;

class KexiRelationDesignTool : public KoTool {
    Q_OBJECT
    public:
        
        KexiRelationDesignTool ( KoCanvasBase* canvas );
        
        /// reimplemented from KoTool
        virtual void mouseReleaseEvent ( KoPointerEvent* event );
        /// reimplemented from KoTool
        virtual void mouseMoveEvent ( KoPointerEvent* event );
        /// reimplemented from KoTool
        virtual void mousePressEvent ( KoPointerEvent* event );
        /// reimplemented from KoTool
        virtual void paint ( QPainter& painter, const KoViewConverter& converter );
        /// reimplemented from KoTool
        virtual void activate (bool temporary=false);
        /// reimplemented from KoTool
        virtual void deactivate();
        
    protected:
        /// reimplemented from KoTool
        virtual QWidget* createOptionWidget();

    private:
        KexiRelationDesignShape *m_relationDesign;
        KexiStartupDialog *m_dbDialog;
        KUrl m_url;
        QLabel *m_fileLabel;
        KComboBox* m_relationCombo;

        void updateCombo();
        QStringList queryList();
        
    private slots:
        void changeUrlPressed();
        void relationSelected(const QString&);
};

#endif // KEXIRELATIONDESIGNTOOL_H
