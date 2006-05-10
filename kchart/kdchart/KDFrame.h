/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDFRAME_H__
#define __KDFRAME_H__

#include <qapplication.h>
#include <QFont>
#include <QColor>
#include <qpixmap.h>
#include <qpen.h>
#include <QMap>
#include <qobject.h>
#include <qtextstream.h>
#include <qdom.h>

#include <KDFrameProfileSection.h>


/** \file KDFrame.h
  \brief Header of the KDFrame class providing highly configurable rectangular frame drawing.

  \ifnot v200
  This class is work in progress, at present only single line frame borders
  and (scaled/streched) background pictures are available.
  See KDChartParams::setSimpleFrame() to learn how to use them.
  \else
  A frame may consist of an (optional) border and/or an (optional) background.
  The border may consist of up to four edges and/or up to four border corners.

  \note Each of the edges and each of the corners may use their own user-definable profile.

  The frame corners ( see types defined in \c CornerStyle ) are set up via setCorner()

  Some commonly used frame profiles (e.g. raised box, sunken panel) are predefined in \c SimpleFrame, see setSimpleFrame().
  To learn how to specify your frame profiles have a look at example code given with setProfile()
  \endif
  */


/**
  \brief The main class of KDFrame.

  KDFrame is made for highly configurable rectangular frame drawing.

  A frame may consist of an (optional) border and/or an (optional) background.
  \ifndef v200
  This class is work in progress, at present only single line frame borders
  and (scaled/streched) background pictures are available.
  See KDChartParams::setSimpleFrame() to learn how to use them.
  \else
  The border may consist of up to four edges and/or up to four border corners.

  \note Each of the edges and each of the corners may use their own user-definable profile.

  The frame corners ( see types defined in CornerStyle ) are set up via setCorner()

  Some commonly used frame profiles (e.g. raised box, sunken panel) are predefined in SimpleFrame, see setSimpleFrame().
  To learn how to specify your frame profiles have a look at example code given with setProfile()
  \endif
  */
class KDCHART_EXPORT KDFrame : public QObject
{
    Q_OBJECT
    Q_ENUMS( BackPixmapMode )
    Q_ENUMS( SimpleFrame )
    Q_ENUMS( ProfileName )
    Q_ENUMS( CornerName )
    Q_ENUMS( CornerStyle )
    Q_ENUMS( KDFramePaintSteps )

public:
    //// GENERAL


    /**
    These are ways how to display a pixmap that might
    be painted into the inner area.

    \Note To have a 'tiled' background image do not use a background
    pixmap but use a background <b>brush</b> holding this pixmap.
    */
    enum BackPixmapMode { PixCentered, PixScaled, PixStretched };

public slots:

    /**
    Converts the specified background pixmap mode enum to a string
    representation.

    \param type the background pixmap mode to convert
    \return the string representation of the background pixmap mode enum
    */
    static QString backPixmapModeToString( BackPixmapMode type ) {
        switch( type ) {
            case PixCentered:
                return "Centered";
            case PixScaled:
                return "Scaled";
            case PixStretched:
                return "Stretched";
        }

        return "Stretched";
    }


    /**
    Converts the specified string to a background pixmap mode enum value.

    \param string the string to convert
    \return the background pixmap mode enum value
    */
    static BackPixmapMode stringToBackPixmapMode( const QString& string ) {
        if( string == "Centered" )
            return PixCentered;
        else if( string == "Scaled" )
            return PixScaled;
        else if( string == "Stretched" )
            return PixStretched;

        return PixStretched;
    }


public:


    /**
    These simple frames are pre-defined for your convenience

    \li \c FrameFlat a flat rectangular frame
    \li \c FrameElegance a flat frame consisting of three lines
    \li \c FrameBoxRaized a raised box
    \li \c FrameBoxSunken a sunken box
    \li \c FramePanelRaized a raised panel
    \li \c FramePanelSunken a sunken panel
    \li \c FrameSemicircular a raised box with round edges

    \ifnot v200
    All pre-defined frames have normal corners.
    \else
    All pre-defined frames have normal corners but
    of course you may specify differently looking corners by
    calling setCorners() (or setCorner(), resp.) after having
    called setSimpleFrame()

    \sa setSimpleFrame
    \endif
    */
    enum SimpleFrame { FrameFlat,        FrameElegance,
        FrameBoxRaized,   FrameBoxSunken,
        FramePanelRaized, FramePanelSunken,
        FrameSemicircular };

public slots:

    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Select a pre-defined frame.
    This methode is provided for convenience, the same results can be obtained by calling
    addProfileSection multiple times specifying the appropriate parameters and finally
    setting the corners to \c CornerNormal

    \note When using FrameElegance the midLineWidth value will be ignored since
    in this special case the lineValue will indicate the <b>total width</b> of the frame.
    FrameElegance frames look best when lineWidth is 16 or the multiple of 16.

    \Note To have a 'tiled' background image do not use a background
    pixmap but use a background <b>brush</b> holding this pixmap.

    See setProfile for a short example using this method.

    \param frame one of the values defined for enum \c SimpleFrame
    \param lineWidth for FrameFlat and FrameElegance: the complete frame width;
        for frames with 2-line edges (FramePanelRaized, FramePanelSunken): the width of each of the lines;
        for frames with 3-line edges (FrameBoxRaized, FrameBoxSunken, FrameSemicircular): the width of the outer and the
        width of the inner line.
    \param midLineWidth only used for frames with 3-line edges: the width
        of the middle line.
    \param pen the basic pen to be used for this frame's lines
    \param background the brush to be used for the frame's background or QBrush() if the background is not to be filled by a brush
    \param backPixmap if not zero points to a pixmap to be used for the background
    \param backPixmapMode determines how the size pixmap is adjusted to the frame size,
        see \c BackPixmapMode for the possible values.

    \sa SimpleFrame, BackPixmapMode, addProfileSection, setProfile, profile, setCorners, setCorner, cornerStyle, cornerWidth
    \endif
    */
    void setSimpleFrame( SimpleFrame    frame,
            int            lineWidth,
            int            midLineWidth,
            QPen           pen,
            QBrush         background     = QBrush(),
            const QPixmap* backPixmap     = 0,
            BackPixmapMode backPixmapMode = PixStretched );

public:
    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Names of the four frame edges

    \li \c ProfileTop
    \li \c ProfileRight
    \li \c ProfileBottom
    \li \c ProfileLeft

    \sa setSimpleFrame, clearProfile, addProfileSection, setProfile, profile
    \endif
    */
    enum ProfileName { ProfileTop,    ProfileRight,
        ProfileBottom, ProfileLeft };

public slots:
    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Add another section to one of the frames profiles.

    \note The sections will be drawn in the order in which they were added to the profile
    beginning at the outside and ending with the inner section of the frame

    See setProfile for a short example using this method.

    \sa ProfileName, setSimpleFrame, addProfileSection, setProfile, profile
    \endif
    */
    void clearProfile( ProfileName name );

    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Add another section to one of the frames profiles.

    \note The sections will be drawn in the order in which they were added to the profile
    beginning at the outside and ending with the inner section of the frame.

    Adding a gap between two sections can be done by specifying a <b>QPen( Qt::NoPen )</b>.

    See setProfile for a short example using this method.

    \sa ProfileName, setSimpleFrame, clearProfile, setProfile, profile
    \endif
    */
    void addProfileSection( ProfileName      name,
            int              wid,
            QPen             pen,
            KDFrameProfileSection::Direction dir  = KDFrameProfileSection::DirPlain, // PENDING(blackie) possible enum problem
            KDFrameProfileSection::Curvature curv = KDFrameProfileSection::CvtPlain );

    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Specify one of the frames profiles by copying another KDFrameProfile.
    Use this function if a profile shall look the same as another one.

    <b>Example:</b>

    \verbatim
    // instantiate a frame around an inner rectangle 50/20, 250/20
    KDFrame myFrame( 50,20, 250,120 );

    // select a very simple rectangular frame with normal corners, black border, white background
    myFrame.setSimpleFrame( KDFrame::FrameFlat, 1, 0, QPen( Qt::Black ), QBrush( Qt::White ) );

    // make the top profile look more nice
    myFrame.clearProfile(      KDFrame::ProfileTop );
    myFrame.addProfileSection( KDFrame::ProfileTop, 2, QPen( Qt::Black ),
    KDFrameProfileSection::DirPlain,
    KDFrameProfileSection::CvtPlain );
    myFrame.addProfileSection( KDFrame::ProfileTop, 5, QPen( Qt::NoPen ),
    KDFrameProfileSection::DirPlain,
    KDFrameProfileSection::CvtPlain );
    myFrame.addProfileSection( KDFrame::ProfileTop, 1, QPen( Qt::Black ),
    KDFrameProfileSection::DirPlain,
    KDFrameProfileSection::CvtPlain );

    // copy the top profile settings into the bottom profile
    myFrame.setProfile( KDFrame::ProfileBottom,
    myFrame.profile( KDFrame::ProfileTop ) );
    \endverbatim


    \sa ProfileName, setSimpleFrame, clearProfile, addProfileSection, profile
    \endif
    */
    void setProfile( ProfileName name, const KDFrameProfile& profile );

    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Return one of the frames profiles.

    \sa ProfileName, setSimpleFrame, clearProfile, addProfileSection, setProfile
    \endif
    */
    const KDFrameProfile& profile( ProfileName name ) const;

public:

    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Names of the frame corners:

    \li \c CornerTopLeft
    \li \c CornerTopRight
    \li \c CornerBottomLeft
    \li \c CornerBottomRight

    \sa setCorners, setCorner, setSunPos, cornerStyle, cornerWidth, sunPos
    \endif
    */
    enum CornerName { CornerTopLeft,
        CornerTopRight,
        CornerBottomLeft,
        CornerBottomRight,
        CornerUNKNOWN };

public slots:
    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Converts the specified corner name enum to a string representation.

    \param type the corner name to convert
    \return the string representation of the corner name enum
    \endif
    */
    static QString cornerNameToString( CornerName type ) {
        switch( type ) {
            case CornerTopLeft:
                return "TopLeft";
            case CornerTopRight:
                return "TopRight";
            case CornerBottomLeft:
                return "BottomLeft";
            case CornerBottomRight:
                return "BottomRight";
            case CornerUNKNOWN:
                return "UNKNOWN";
        }

        return "UNKNOWN";
    }


    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Converts the specified string to a corner name enum value.

    \param string the string to convert
    \return the corner name enum value
    \endif
    */
    static CornerName stringToCornerName( const QString& string ) {
        if( string == "TopLeft" )
            return CornerTopLeft;
        else if( string == "TopRight" )
            return CornerTopRight;
        else if( string == "BottomLeft" )
            return CornerBottomLeft;
        else if( string == "BottomRight" )
            return CornerBottomRight;
        else if( string == "UNKNOWN" )
            return CornerUNKNOWN;

        return CornerUNKNOWN;
    }

public:


    /**
    \ifnot v200
    \deprecated Feature scheduled for future release, at present not implemented.
    \else
    Look of the frame corners:

    \li \c CornerNormal  a square angle corner.
    \li \c CornerRound   a quarter of a circle.
    \li \c CornerOblique corner cut-off by 45 degrees.

    \sa setCorners, setCorner, cornerStyle, cornerWidth
    \endif
    */
    enum CornerStyle { CornerNormal,
        CornerRound,
        CornerOblique };

public slots:
        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Converts the specified corner style enum to a string representation.

        \param type the corner style enum to convert
        \return the string representation of the corner style enum
        \endif
        */
        static QString cornerStyleToString( CornerStyle type ) {
            switch( type ) {
                case CornerNormal:
                    return "Normal";
                case CornerRound:
                    return "Round";
                case CornerOblique:
                    return "Oblique";
            }

            return "Normal";
        }


        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Converts the specified string to a corner stylye enum value.

        \param string the string to convert
        \return the corner style enum value
        \endif
        */
        static CornerStyle stringToCornerStyle( const QString& string ) {
            if( string == "Normal" )
                return CornerNormal;
            else if( string == "Round" )
                return CornerRound;
            else if( string == "Oblique" )
                return CornerOblique;

            return CornerNormal;
        }


public:

#ifndef Q_MOC_RUN
        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Helper class storing settings for one corner of the frame.

        To set/retrieve information stored in this class please use
        the appropriate methods of the KDFrame class.

        \sa setCorners, setCorner, cornerStyle, cornerWidth, cornerProfile, CornerName, CornerStyle
        \endif
        */
    class KDCHART_EXPORT KDFrameCorner
        {
            friend class KDFrame;
            public:
            KDFrameCorner( CornerStyle           style   = CornerNormal,
                    int                   width   = 0,
                    KDFrameProfile* const profile = 0 )
                : _style( style ),
            _width( width )
            {
                if( profile )
                    _profile = *profile;
                else
                    _profile.clear();
            }
            /**
            Destructor. Only defined to have it virtual.
            */
            virtual ~KDFrameCorner();

            /**
            \ifnot v200
            \deprecated Feature scheduled for future release, at present not implemented.
            \else
            Creates a DOM element node that represents a frame corner for use
            in a DOM document.

            \param document the DOM document to which the node will belong
            \param parent the parent node to which the new node will be appended
            \param elementName the name of the new node
            \param corner the corner to be represented
            \endif
            */
            static void createFrameCornerNode( QDomDocument& document,
                    QDomNode& parent,
                    const QString& elementName,
                    const KDFrameCorner& corner );

            /**
            \ifnot v200
            \deprecated Feature scheduled for future release, at present not implemented.
            \else
            Reads data from a DOM element node that represents a frame
            corner and fills a KDFrameCorner object with the data.

            \param element the DOM element to read from
            \param corner the frame corner object to read the data into
            \endif
            */
            static bool readFrameCornerNode( const QDomElement& element,
                    KDFrameCorner& corner );

            protected:
            void setAll( CornerStyle           style,
                    int                   width,
                    KDFrameProfile* const profile = 0 )
            {
                _style = style;
                _width = width;
                if( profile )
                    _profile = *profile;
                else
                    _profile.clear();
            }
            CornerStyle style() const
            {
                return _style;
            }
            int width() const
            {
                return _width;
            }
            const KDFrameProfile& profile() const
            {
                return _profile;
            }
            private:
            CornerStyle  _style;
            int          _width;
            KDFrameProfile _profile;
        };

#endif

public slots:
        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Specify the look and the width and (optionally) the profile
        of one of the frame corners.

        \sa setCorners, cornerStyle, cornerWidth, cornerWidth, CornerName, CornerStyle
        \endif
        */
        void setCorner( CornerName name,
                CornerStyle style,
                int width,
                KDFrameProfile* const profile = 0 )
        {
            switch( name ) {
                case CornerTopLeft:     _cornerTL.setAll( style, width, profile );
                                        break;
                case CornerTopRight:    _cornerTR.setAll( style, width, profile );
                                        break;
                case CornerBottomLeft:  _cornerBL.setAll( style, width, profile );
                                        break;
                case CornerBottomRight: _cornerBR.setAll( style, width, profile );
                                        break;
                case CornerUNKNOWN:
                                        break;
            }
        }

        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Specify the look and the width and (optionally) the profile
        of all of the frame corners.

        \sa setCorner, cornerStyle, cornerWidth, CornerName, CornerStyle
        \endif
        */
        void setCorners( CornerStyle style,
                int width,
                KDFrameProfile* const profile = 0 )
        {
            _cornerTL.setAll( style, width, profile );
            _cornerTR.setAll( style, width, profile );
            _cornerBL.setAll( style, width, profile );
            _cornerBR.setAll( style, width, profile );
        }


        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Returns the look of one of the frame corners.

        \sa setCorners, cornerWidth, CornerName, CornerStyle
        \endif
        */
        CornerStyle cornerStyle( CornerName name ) const
        {
            switch( name ) {
                case CornerTopLeft:     return _cornerTL.style();
                case CornerTopRight:    return _cornerTR.style();
                case CornerBottomLeft:  return _cornerBL.style();
                case CornerBottomRight: return _cornerBR.style();
                case CornerUNKNOWN:
                default:                return CornerNormal;
            }
        }

        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Returns the width of one of the frame corners.

        \sa setCorners, cornerStyle, CornerName, CornerStyle
        \endif
        */
        int cornerWidth( CornerName name ) const
        {
            switch( name ) {
                case CornerTopLeft:     return _cornerTL.width();
                case CornerTopRight:    return _cornerTR.width();
                case CornerBottomLeft:  return _cornerBL.width();
                case CornerBottomRight: return _cornerBR.width();
                case CornerUNKNOWN:
                default:                return 0;
            }
        }

        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Returns the profile of one of the frame corners.

        \sa setCorners, cornerStyle, CornerName, CornerStyle
        \endif
        */
        const KDFrameProfile& cornerProfile( CornerName name ) const
        {
            switch( name ) {
                case CornerUNKNOWN:
                case CornerTopLeft:     return _cornerTL.profile();
                case CornerTopRight:    return _cornerTR.profile();
                case CornerBottomLeft:  return _cornerBL.profile();
                case CornerBottomRight: return _cornerBR.profile();
                default: return _cornerTL.profile();
            }
        }


        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Specifies the position of the sun, normally this is the upper left corner.

        \sa sunPos
        \endif
        */
        void setSunPos( CornerName sunPos )
        {
            _sunPos = sunPos;
        }

        /**
        \ifnot v200
        \deprecated Feature scheduled for future release, at present not implemented.
        \else
        Returns the position of the sun.

        \sa setSunPos
        \endif
        */
        CornerName sunPos() const
        {
            return _sunPos;
        }


        /**
        Specifies the brush to be used to fill the inner area of this frame,
        calling this methode without passing in a parameter re-sets the background brush
        to \c QBrush( \cQt::NoBrush \c).

        \Note To have a 'tiled' background image just use a brush
        holding this pixmap - for other ways to show background
        images please use setBackPixmap.

        \sa setBackPixmap, background
        */
        void setBackground( QBrush background = QBrush( Qt::NoBrush ) )
        {
            _background = background;
        }

        /**
        Specifies a pixmap to be used to fill the inner area of this frame,
        calling this methode without passing in a parameter removes the background pixmap.

        \Note To have a 'tiled' background image do not use setBackPixmap
        but use setBackground specifying a brush holding the pixmap.

        \sa setBackground, background
        */
        void setBackPixmap( const QPixmap* backPixmap     = 0,
                BackPixmapMode backPixmapMode = PixStretched )
        {
            _backPixmap     = backPixmap ? *backPixmap : QPixmap();
            _backPixmapMode = backPixmapMode;
        }

        /**
        Returns the brush that is used to fill the inner area of this frame,
        or a QBrush( \cNoBrush \c) if no background is to be drawn.

        \param backPixmap receives the pixmap used for drawing the background or
        a null pixmap, test this by calling backPixmap.isNull()

        \Note If a 'tiled' background image is shown the respective pixmap
        may be found by calling the <b>brush's</b> pixmap() function.

        \sa setBackground, setBackPixmap
        */
        const QBrush& background( const QPixmap*& backPixmap,
                BackPixmapMode& backPixmapMode ) const
        {
            backPixmap     = &_backPixmap;
            backPixmapMode =  _backPixmapMode;
            return _background;
        }


        /**
        Specifies the position and the size of area that is surrounded by the frame.

        \note The rectangle applies to the <b>inner</b> area of the frame.
        The Frame is drawn around this area: touching it but not covering it.
        The outer size of the frame and the position of its outer left corner
        depends from the frame profile width(s).

        \sa innerRect, setCorners, setSimpleFrame, setProfile
        */
        void setInnerRect( QRect innerRect )
        {
            _innerRect = innerRect;
        }

        /**
        Returns the position and the size of the frame.

        \note The rectangle returns to the <b>inner</b> area of the frame.
        The Frame is drawn around this area: touching it but not covering it.
        The outer size of the frame and the position of its outer left corner
        depends from the frame profile width.

        \sa setInnerRect, setProfile
        */
        QRect innerRect() const
        {
            return _innerRect;
        }


        /**
        Paint methode drawing the background (if any) of the frame.

        \note Under normal circumstances you will <b>never</b> have to
        call this methode since it is called internally by paint() if you
        start it with  \c PaintBackground (or with \c PaintAll, resp.) for the \c steps parameter.
        */
        virtual void paintBackground( QPainter& painter, const QRect& innerRect ) const;
        /**
        Paint methode drawing the edges (if any) of the frame.

        \note Under normal circumstances you will <b>never</b> have to
        call this methode since it is called internally by paint() if you
        start it with \c PaintEdges (or with \c PaintAll, \c PaintBorder, resp.) for the \c steps parameter.
        */
        virtual void paintEdges( QPainter& painter, const QRect& innerRect ) const;
        /**
        Paint methode drawing the corners (if any) of the frame.

        \note Under normal circumstances you will <b>never</b> have to
        call this methode since it is called internally by paint() if you
        start it with \c PaintCorners (or with \c PaintAll, \c PaintBorder, resp.) for the \c steps parameter.
        */
        virtual void paintCorners( QPainter& painter, const QRect& innerRect ) const;


public:
        /**
        The different steps taken to paint the frame:
        first paint the background then the edges then the corners.
        By choosing \c PaintAll all the parts of the frame will be drawn.

        \li \c PaintBackground -- first paints the brush, then paints the pixmap if any
        \li \c PaintEdges
        \li \c PaintCorners
        \li \c PaintBorder -- paint the edges and the corners but not the background
        \li \c PaintAll

        \sa paint
        */
        enum KDFramePaintSteps { PaintBackground, PaintEdges, PaintCorners, PaintBorder, PaintAll };

public slots:

        /**
        Paint methode actually drawing the frame.

        This method must be called from inside the \c paint() methode of your widget.

        In order not to override the inner contents of the frame you normally would
        first call <b>paint( painter, PaintBackground );</b> then do all the inside
        drawing and finally call <b>paint( painter, PaintBorder );</b>. In case the
        inner contents <b>are</b> allowed to (partially) override the frame border
        you could do the following: First call <b>paint( painter, PaintBackground );</b>
        immediately followed by <b>paint( painter, PaintEdges );</b> then do all your
        inside work and finally draw <b>paint( painter, PaintCorners );</b>.

        \param painter The QPainter to be used for drawing.
        \param steps The part of the frame to be drawn, use KDFrame::PaintAll to draw the entire frame and the background,
        use KDFrame::PaintBackground to draw only the background, use KDFrame::PaintEdges to draw just the edges,
        use KDFrame::PaintCorners to draw only the corners.
        \param innerRect The area inside the frame. Use this parameter to temporarily override the \c innerRect set by
        the constructor of KDFrame or by setInnerRect(). This approach can be usefull if you want to draw several
        frames that differ only regarding to their position and size but share the same edges/corners/background settings.
        In this case you could decide to instantiate only one KDFrame set up the desired settings and just call
        its paint() methode several time - giving it the appropriate innerRect for each frame. This would result in
        less memory usage since you could use that single KDFrame object as kind of a shared ressource.
        */
        virtual void paint( QPainter* painter,
                KDFramePaintSteps steps = PaintAll,
                QRect innerRect         = QRect(0,0,0,0) ) const;


        /**
        Remove all settings and specify no border, no edges, no background.
        */
        void clearAll()
        {
            _background     = QBrush();
            _backPixmap     = QPixmap();
            _backPixmapMode = PixStretched;
            _shadowWidth    = 0;
            _sunPos         = CornerTopLeft;
            _innerRect      = QRect( 0,0, 0,0 );
            _topProfile.clear();
            _rightProfile.clear();
            _bottomProfile.clear();
            _leftProfile.clear();
            _cornerTL.setAll( CornerNormal, 0, 0 );
            _cornerTR.setAll( CornerNormal, 0, 0 );
            _cornerBL.setAll( CornerNormal, 0, 0 );
            _cornerBR.setAll( CornerNormal, 0, 0 );
        }

public:

    /**
    Default Constructor. Defines default values.

    The constructor does *not* have a \c parent parameter since drawing
    of the frame is not done transparently but by (one or more) explicit
    calls of the frames paint() methode.  See explanation given there
    to learn about the why and how of this...

    \note The rectangle applies to the <b>inner</b> area of the frame.
    The Frame is drawn around this area: touching it but not covering it.
    The outer size of the frame and the position of its outer left corner
    depends from the frame profile width.

    \Note To have a 'tiled' background image do not specify a backPixmap
    but use a background <b>brush</b> holding this pixmap.

    \sa rect, setInnerRect, setProfile
    */
    KDFrame( QRect          innerRect      = QRect(0,0,0,0),
            SimpleFrame    frame          = FrameFlat,
            int            lineWidth      = 1,
            int            midLineWidth   = 0,
            QPen           pen            = QPen(),   // solid black line with 0 width
            QBrush         background     = QBrush(), // no brush
            const QPixmap* backPixmap     = 0,        // no pixmap
            BackPixmapMode backPixmapMode = PixStretched,
            int            shadowWidth    = 0,
            CornerName     sunPos         = CornerTopLeft )
    {
        _profileSections.setAutoDelete( true );
        _innerRect = innerRect;
        setSimpleFrame( frame,
                        lineWidth,
                        midLineWidth,
                        pen,
                        background,
                        backPixmap,
                        backPixmapMode );
        _shadowWidth = shadowWidth;
        _sunPos      = sunPos;
    }


/*
    Constructor. Set up a frame by copying settings of another frame.

    The constructor does *not* have a \c parent parameter since drawing
    of the frame is not done transparently but by (one or more) explicit
    calls of the frames paint() methode.  See explanation given there
    to learn about the why and how of this...

    \note The rectangle applies to the <b>inner</b> area of the frame.
    The Frame is drawn around this area: touching it but not covering it.
    The outer size of the frame and the position of its outer left corner
    depends from the frame profile width.

    \sa rect, setInnerRect, setProfile

    KDFrame( QRect innerRect,
            const KDFrame& R,
            CornerName sunPos = CornerUNKNOWN )
    {
        deepCopy( *this, R );
        if( innerRect.isValid() )
            _innerRect = innerRect;
        if( CornerUNKNOWN != sunPos )
            _sunPos = sunPos;

        _profileSections.setAutoDelete( true );
    }
*/
private:
    KDFrame( const KDFrame& ) : QObject(0) {}
    KDFrame& operator=( const KDFrame& ){return *this;}



public:
    /**
    Destructor. Only defined to have it virtual.
    */
    virtual ~KDFrame();

    /*
    Kopierroutine, aufgerufen im Copy-C'tor und im Zuweisungs-Operator
    */
    static void deepCopy( KDFrame& D, const KDFrame& R )
    {
        D._shadowWidth    = R._shadowWidth;
        D._sunPos         = R._sunPos;
        D._background     = R._background;
        D._backPixmap     = R._backPixmap;
        D._backPixmapMode = R._backPixmapMode;
        D._innerRect      = R._innerRect;
        D._topProfile     = R._topProfile;
        D._rightProfile   = R._rightProfile;
        D._bottomProfile  = R._bottomProfile;
        D._leftProfile    = R._leftProfile;
        D._cornerTL       = R._cornerTL;
        D._cornerTR       = R._cornerTR;
        D._cornerBL       = R._cornerBL;
        D._cornerBR       = R._cornerBR;
        D._profileSections= R._profileSections;
        D._profileSections.setAutoDelete( true );
        R.setProfileSectionsAutoDelete( false );
    }

    friend QTextStream& operator<<( QTextStream& s, const KDFrame& p );
    friend QTextStream& operator>>( QTextStream& s, KDFrame& p );

    /**
    Creates a DOM element node that represents a frame for use
    in a DOM document.

    \param document the DOM document to which the node will belong
    \param parent the parent node to which the new node will be appended
    \param elementName the name of the new node
    \param frame the frame to be represented
    */
    static void createFrameNode( QDomDocument& document, QDomNode& parent,
            const QString& elementName,
            const KDFrame& frame );

    /**
    Creates a DOM element node that represents a frame profile for use
    in a DOM document.

    \param document the DOM document to which the node will belong
    \param parent the parent node to which the new node will be appended
    \param elementName the name of the new node
    \param profile the profile to be represented
    */
    static void createFrameProfileNode( QDomDocument& document,
            QDomNode& parent,
            const QString& elementName,
            KDFrameProfile profile );

    /**
    Reads data from a DOM element node that represents a frame
    object and fills a KDFrame object with the data.

    \param element the DOM element to read from
    \param frame the frame object to read the data into
    */
    static bool readFrameNode( const QDomElement& element,
            KDFrame& frame );

    /**
    Reads data from a DOM element node that represents a frame
    profile and fills a KDFrameProfile object with the data.

    \param element the DOM element to read from
    \param profile the frame profile object to read the data into
    */
    static bool readFrameProfileNode( const QDomElement& element,
            KDFrameProfile& profile );

signals:
    /**
    This signal is emitted when any of the frame parameters has changed.
    */
    void changed();

private:
    void setProfileSectionsAutoDelete( bool on ) const
    {
        ((KDFrame*)this)->_profileSections.setAutoDelete( on );
    }

    /**
    Stores the width of the shadow.
    */
    int _shadowWidth;

    /**
    Stores the position of the sun.
    */
    CornerName _sunPos;

    /**
    Stores the brush to be used to fill the inner area.
    */
    QBrush _background;

    /**
    Stores the pixmap to be painted into the inner area.
    */
    QPixmap _backPixmap;

    /**
    Stores the way how to display the pixmap that is
    to be painted into the inner area.
    */
    BackPixmapMode _backPixmapMode;

    /**
    Stores the position and size of the frame.
    */
    QRect _innerRect;

    /**
    Stores all currently used profile settings for a controlled deletion.

    \note The other pointer lists (like _topProfile or _rightProfile)
    do NOT delete the objects that belong to their pointers,
    but all sections will be deleted via this extra _profileSections list:
    this allows for using the same KDFrameProfileSection* to be used
    by several lists - typically done for simple frames where all 4 sides
    are composed the same way.
    */
    KDFrameProfile _profileSections;

    /**
    Stores the profile settings for the top side of the frame.
    */
    KDFrameProfile _topProfile;
    /**
    Stores the profile settings for the right side of the frame.
    */
    KDFrameProfile _rightProfile;
    /**
    Stores the profile settings for the bottom side of the frame.
    */
    KDFrameProfile _bottomProfile;
    /**
    Stores the profile settings for the left side of the frame.
    */
    KDFrameProfile _leftProfile;

    /**
    Stores the settings for the top left corner of the frame.
    */
    KDFrameCorner _cornerTL;
    /**
    Stores the settings for the top right corner of the frame.
    */
    KDFrameCorner _cornerTR;
    /**
    Stores the settings for the bottom left corner of the frame.
    */
    KDFrameCorner _cornerBL;
    /**
    Stores the settings for the bottom right corner of the frame.
    */
    KDFrameCorner _cornerBR;
};


/**
  Writes the KDFrame object p as an XML document to the text stream s.

  \param s the text stream to write to
  \param p the KDFrame object to write
  \return the text stream after the write operation
  */
QTextStream& operator<<( QTextStream& s, const KDFrame& p );


/**
  Reads the an XML document from the text stream s into the
  KDFrame object p

  \param s the text stream to read from
  \param p the KDFrame object to read into
  \return the text stream after the read operation
  */
QTextStream& operator>>( QTextStream& s, KDFrame& p );


#endif
