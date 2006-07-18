/* This file is part of the KDE project
   Copyright (C) 2001,2002,2003,2004 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "kchartFontConfigPage.h"

#include "kchartFontConfigPage.moc"

#include <kapplication.h>
#include <klocale.h>
#include <kcolorbutton.h>
#include <kdebug.h>

#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <q3listbox.h>
#include <QPushButton>
#include <qpainter.h>

//Added by qt3to4:
#include <Q3GridLayout>

#include <kfontdialog.h>

// For IRIX
namespace std {}

using namespace std;

#include "kchart_params.h"


class KChartFontListBoxItem : public Q3ListBoxText
{
public:
    KChartFontListBoxItem( Q3ListBox* lb,  const QString& text = QString::null ) :
        Q3ListBoxText( lb,  text )  {}
    KChartFontListBoxItem( const QString& text = QString::null ) :
        Q3ListBoxText( text )  {}

    void setFont( const QFont& font )  {
        _font = font;
        listBox()->repaint();
    }
    QFont font() const {
        return _font;
    }

protected:
    void paint( QPainter* painter )  
    {
        painter->save();
        painter->setFont( _font );
        Q3ListBoxText::paint( painter );
        painter->restore();
    }

private:
    QFont _font;
};


// ----------------------------------------------------------------


namespace KChart
{

KChartFontConfigPage::KChartFontConfigPage( KChartParams* params,
                                            QWidget* parent, 
					    KDChartTableData *dat) :
    QWidget( parent ), m_params( params ), data(dat)
{
    Q3GridLayout *grid = new Q3GridLayout(this,4,3,KDialog::marginHint(), KDialog::spacingHint());

    // The listbox
    m_list = new Q3ListBox(this);
    m_list->resize( m_list->sizeHint() );
    grid->addWidget(m_list, 0, 0); // Row 0-0, col 0-1

    // The font button.
    m_fontButton = new QPushButton( this);
    m_fontButton->setText(i18n("Font..."));
    m_fontButton->setWhatsThis( i18n("Select an item in the list above and click on this button to display the KDE font dialog in order to choose a new font for this item."));
    m_fontButton->resize( m_fontButton->sizeHint() );
    grid->addWidget( m_fontButton, 1, 0);

#if 0
    // FIXME: Integrate the font chooser instead?  Well, maybe later.
    KFontChooser  *fontChooser = new KFontChooser(this, "fontChooser");
    grid->addMultiCellWidget(fontChooser, 0, 2, 1, 1);
#endif

    grid->setColumnStretch(2, 1);
    grid->setRowStretch(3, 1);

    connect( m_fontButton, SIGNAL(clicked()),
	     this,         SLOT(changeLabelFont()));
    connect( m_list,       SIGNAL(doubleClicked ( Q3ListBoxItem * )),
	     this,         SLOT(changeLabelFont()));

    // Enter the items into the list.
    initList();
}


void KChartFontConfigPage::initList()
{
    if ( m_params->chartType() != KChartParams::Pie
	 && m_params->chartType() != KChartParams::Ring ) {
        m_list->insertItem(new KChartFontListBoxItem( i18n("X-Title")));
        m_list->insertItem(new KChartFontListBoxItem( i18n("Y-Title")));
        m_list->insertItem(new KChartFontListBoxItem( i18n("X-Axis")));
        m_list->insertItem(new KChartFontListBoxItem( i18n("Y-Axis")));
        m_list->insertItem(new KChartFontListBoxItem( i18n("All Axes")));
    }

    m_list->insertItem(i18n("Label"));
    m_list->setCurrentItem(0);
}



void KChartFontConfigPage::changeLabelFont()
{
    QFont                 *font = 0;
	Qt::CheckState  *state = 0;
    bool                   diffAxes = false;

    if (m_list->currentText()==i18n("X-Title")) {
        font  = &xTitle;
        state = &xTitleIsRelative;
    } else if(m_list->currentText()==i18n("Y-Title")) {
        font  = &yTitle;
        state = &yTitleIsRelative;
    } else if(m_list->currentText()==i18n("X-Axis")) {
        font  = &xAxis;
        state = &xAxisIsRelative;
    } else if(m_list->currentText()==i18n("Y-Axis")) {
        font  = &yAxis;
        state = &yAxisIsRelative;
    } else if(m_list->currentText()==i18n("All Axes")) {
        diffAxes = true;
    } else if(m_list->currentText()==i18n("Label")) {
        font  = &label;
        state = &labelIsRelative;
    }
    else
        kDebug( 35001 ) << "Pb in listBox" << endl;

    if ( diffAxes ) {
        QFont newFont;
        int flags = 0;
		Qt::CheckState newState
            = (xAxisIsRelative == yAxisIsRelative)
            ? (xAxisIsRelative ? Qt::Checked : Qt::Unchecked )
            : Qt::PartiallyChecked;
        if (KFontDialog::getFontDiff( newFont,
                                      flags,
                                      false,
                                      this,
                                      true,
                                      &newState ) != QDialog::Rejected) {
            if ( KFontChooser::FamilyList & flags ) {
                xAxis.setFamily( newFont.family() );
                yAxis.setFamily( newFont.family() );
            }

            if ( KFontChooser::StyleList & flags ) {
                xAxis.setWeight( newFont.weight() );
                xAxis.setItalic( newFont.italic() );
                xAxis.setUnderline( newFont.underline() );
                xAxis.setStrikeOut( newFont.strikeOut() );

                yAxis.setWeight( newFont.weight() );
                yAxis.setItalic( newFont.italic() );
                yAxis.setUnderline( newFont.underline() );
                yAxis.setStrikeOut( newFont.strikeOut() );
            }

            if ( KFontChooser::SizeList & flags ) {
                xAxis.setPointSize( newFont.pointSize() );
                yAxis.setPointSize( newFont.pointSize() );
            }

            // CharSet settings are ignored since we are not Qt 2.x compatible
            // if( KFontChooser::CharsetList & flags ) {
            // }

            if ( Qt::PartiallyChecked != newState ) {
                xAxisIsRelative = newState;
                yAxisIsRelative = newState;
            }
        }
    }
    else if ( font && state ) {
        QFont newFont( *font );
		Qt::CheckState newState = *state;
        if (KFontDialog::getFont( newFont,
                                  false,
                                  this,
                                  true,
                                  &newState ) != QDialog::Rejected) {
            *font = newFont;
            if ( Qt::PartiallyChecked!= newState )
                *state = newState;
        }
    }
}


void KChartFontConfigPage::init()
{
    KDChartAxisParams  leftparms;
    leftparms   = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams  rightparms;
    rightparms  = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams  bottomparms;
    bottomparms = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

    xAxis           = bottomparms.axisLabelsFont();
    xAxisIsRelative = bottomparms.axisLabelsFontUseRelSize() 
	? Qt::Checked : Qt::Unchecked;

    if ( Qt::Checked == xAxisIsRelative )
        xAxis.setPointSize( bottomparms.axisLabelsFontRelSize() );

    yAxis = leftparms.axisLabelsFont();
    yAxisIsRelative = leftparms.axisLabelsFontUseRelSize()
	? Qt::Checked : Qt::Unchecked;

    if ( Qt::Checked == yAxisIsRelative )
        yAxis.setPointSize( leftparms.axisLabelsFontRelSize() );
    // PENDING(khz) Add support for the other 6 possible axes

    // PENDING(khz) Add support for the other 16 possible hd/ft areas


   xTitle = m_params->axisTitleFont( KDChartAxisParams::AxisPosBottom );
   yTitle = m_params->axisTitleFont( KDChartAxisParams::AxisPosLeft );
   xTitle.setPointSize( m_params->axisTitleFontRelSize( KDChartAxisParams::AxisPosBottom ) );
   yTitle.setPointSize( m_params->axisTitleFontRelSize( KDChartAxisParams::AxisPosLeft ) );
//   label = _params->labelFont();

    // PENDING(kalle) Adapt
//   for(int i=0;i<12;i++)
//     extColor.setColor(i,_params->ExtColor.color(i));
//   index = 0;
//   colorButton->setColor(extColor.color(index));
}


void KChartFontConfigPage::apply()
{
    // PENDING(kalle) Adapt
    KDChartAxisParams  leftparms;
    leftparms   = m_params->axisParams( KDChartAxisParams::AxisPosLeft );
    KDChartAxisParams  rightparms;
    rightparms  = m_params->axisParams( KDChartAxisParams::AxisPosRight );
    KDChartAxisParams  bottomparms;
    bottomparms = m_params->axisParams( KDChartAxisParams::AxisPosBottom );

    leftparms.setAxisLabelsFont( yAxis, Qt::Unchecked == yAxisIsRelative );
    if ( Qt::Checked == yAxisIsRelative )
        leftparms.setAxisLabelsFontRelSize( yAxis.pointSize() );

    // PENDING(khz) change right axis handling
    // use left axis settings for the right axis as well
    //   (this must be changed, khz 14.12.2001)
    rightparms.setAxisLabelsFont( yAxis, Qt::Unchecked == yAxisIsRelative );
    if ( Qt::Checked == yAxisIsRelative )
        rightparms.setAxisLabelsFontRelSize( yAxis.pointSize() );

    bottomparms.setAxisLabelsFont( xAxis, Qt::Unchecked == xAxisIsRelative );
    if ( Qt::Checked == xAxisIsRelative )
        bottomparms.setAxisLabelsFontRelSize( xAxis.pointSize() );
    // PENDING(khz) Add support for the other 6 possible axes

    m_params->setAxisParams( KDChartAxisParams::AxisPosLeft, leftparms );
    m_params->setAxisParams( KDChartAxisParams::AxisPosRight, rightparms );
    m_params->setAxisParams( KDChartAxisParams::AxisPosBottom, bottomparms );
    // PENDING(khz) change hd2 and ft handling
    // use header settings for header 2 and footer as well
    //   (this must be changed, khz 14.12.2001)
    // PENDING(khz) Add support for the other 16 possible hd/ft areas

    m_params->setAxisTitleFont( KDChartAxisParams::AxisPosLeft, yTitle );
    m_params->setAxisTitleFont( KDChartAxisParams::AxisPosBottom, xTitle );
    m_params->setAxisTitleFontRelSize( KDChartAxisParams::AxisPosLeft, yTitle.pointSize() );
    m_params->setAxisTitleFontRelSize( KDChartAxisParams::AxisPosBottom, xTitle.pointSize() );
}

}  //KChart namespace
