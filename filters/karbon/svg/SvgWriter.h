  /* This file is part of the KDE project
    Copyright (C) 2002 Lars Siebold <khandha5@gmx.net>
    Copyright (C) 2002 Werner Trobin <trobin@kde.org>
    Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
    Copyright (C) 2002-2003,2005 Rob Buis <buis@kde.org>
    Copyright (C) 2005 Boudewijn Rempt <boud@valdyas.org>
    Copyright (C) 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
    Copyright (C) 2005 Thomas Zander <zander@kde.org>
    Copyright (C) 2005,2008 Jan Hambrecht <jaham@gmx.net>
    Copyright (C) 2006 Inge Wallin <inge@lysator.liu.se>
    Copyright (C) 2006 Laurent Montel <montel@kde.org>

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

  #ifndef SVGWRITER_H
  #define SVGWRITER_H

  #include <KoFilter.h>
  #include <QVariantList>
  #include <QtGui/QGradient>


  class KoShapeLayer;
  class KoShapeGroup;
  class KoShape;
  class KoPathShape;
  class KoShapeBorderModel;
  class ArtisticTextShape;
  class TextShape;
  class ArtisticTextRange;
  class EllipseShape;
  class RectangleShape;
  class Frame;
  class KoPatternBackground;
  class QTextStream;
  class QPixmap;
  class QImage;
  class QColor;
  class QBrush;


  /// Implements exporting shapes to SVG
  class SvgWriter
  {
  public:
      /// Creates svg writer to export specified layers
      SvgWriter(const QList<KoShapeLayer*> &layers, const QSizeF &pageSize);

      /// Creates svg writer to export specified shapes
      SvgWriter(const QList<KoShape*> &toplevelShapes, const QSizeF &pageSize);

      /// Destroys the svg writer
      virtual ~SvgWriter();

      /// Writes svg to specified output device
      bool save(QIODevice &outputDevice);

      /// Writes svg to the specified file
      bool save(const QString &filename, bool writeInlineImages);

  private:
    
      void saveLayer(KoShapeLayer * layer);
      void saveGroup(KoShapeGroup * group);
      void saveShape(KoShape * shape);
      void savePath(KoPathShape * path);
      void saveEllipse(EllipseShape * ellipse);
      void saveRectangle(RectangleShape * rectangle);
      void saveImage(KoShape *picture);
      void saveText(ArtisticTextShape * text);

      /*
      * Saves the properties associated with the shape used for SVg animation.
      */
      void saveFrame(Frame *frame);
      void savePlainText();
      void saveScript();
      void forTesting(KoShape *shape);
      
      void getStyle(KoShape * shape, QTextStream * stream);
      void getFill(KoShape * shape, QTextStream *stream);
      void getStroke(KoShape * shape, QTextStream *stream);
      void getEffects(KoShape *shape, QTextStream *stream);
      void getClipping(KoShape *shape, QTextStream *stream);
      void getColorStops(const QGradientStops & colorStops);
      void getGradient(const QGradient * gradient, const QTransform &gradientTransform);
      void getPattern(KoPatternBackground * pattern, KoShape * shape);
      QString getTransform(const QTransform &matrix, const QString &attributeName);
      void saveFont(const QFont &font, QTextStream *stream);
      void saveTextRange(const ArtisticTextRange &range, QTextStream *stream, bool saveFont, qreal baselineOffset);

      
      QString getID(const KoShape *obj);
      QString createID(const KoShape * obj);


      /// Checks if the matrix only has translation set
      bool isTranslation(const QTransform &);

      QTextStream* m_stream;
      QTextStream* m_defs;
      QTextStream* m_body;
      QTextStream* m_frames;
      
      unsigned int m_indent;
      unsigned int m_indent2;

      QMap<const KoShape*, QString> m_shapeIds;
      QList<KoShape*> m_toplevelShapes;
      QTransform m_userSpaceMatrix;
      QSizeF m_pageSize;
      bool m_writeInlineImages;
      QString m_filename;

      QString m_script;
  };

  #endif /*SVGWRITER_H */
