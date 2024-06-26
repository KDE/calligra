/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Cyrille Berger <cberger@cberger.net>
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef _KO_FILTER_EFFECT_H_
#define _KO_FILTER_EFFECT_H_

class QImage;
class QString;
class QRectF;
class KoXmlWriter;
class KoFilterEffectRenderContext;
class KoFilterEffectLoadingContext;
class KoXmlElement;

#include "flake_export.h"
#include <QList>

/**
 * This is the base for filter effect (blur, invert...) that can be applied on a shape.
 * All sizes and coordinates of the filter effect are stored in object bounding box
 * coordinates, where (0,0) refers to the top-left corner of a shapes bounding rect
 * and (1,1) refers to the bottom-right corner.
 * When loading, a transformation matrix is given to convert from user space coordinates.
 * Another transformation matrix is given via the render context to convert back to
 * user space coordinates when applying the effect.
 * Using object bounding box coordinates internally makes it easy to share effects
 * between shapes or even between users via the filter effect resources.
 */
class FLAKE_EXPORT KoFilterEffect
{
public:
    KoFilterEffect(const QString &id, const QString &name);
    virtual ~KoFilterEffect();

    /// Returns the user visible name of the filter
    QString name() const;

    /// Returns the unique id of the filter
    QString id() const;

    /// Sets the region the filter is applied to in bounding box units
    void setFilterRect(const QRectF &filterRect);

    /// Returns the region this filter is applied to in bounding box units
    QRectF filterRect() const;

    /// Returns the region this filter is applied to for the given bounding rect
    QRectF filterRectForBoundingRect(const QRectF &boundingRect) const;

    /**
     * Sets the name of the output image
     *
     * The name is used so that other effects can reference
     * the output of this effect as one of their input images.
     *
     * @param output the output image name
     */
    void setOutput(const QString &output);

    /// Returns the name of the output image
    QString output() const;

    /**
     * Returns list of named input images of this filter effect.
     *
     * These names identify the input images for this filter effect.
     * These can be one of the keywords SourceGraphic, SourceAlpha,
     * BackgroundImage, BackgroundAlpha, FillPaint or StrokePaint,
     * as well as a named output of another filter effect in the stack.
     * An empty input list of the first effect in the stack default to
     * SourceGraphic, whereas on subsequent effects it defaults to the
     * result of the previous filter effect.
     */
    QList<QString> inputs() const;

    /// Adds a new input at the end of the input list
    void addInput(const QString &input);

    /// Inserts an input at the giben position in the input list
    void insertInput(int index, const QString &input);

    /// Sets an existing input to a new value
    void setInput(int index, const QString &input);

    /// Removes an input from the given position in the input list
    void removeInput(int index);

    /**
     * Return the required number of input images.
     * The default required number of input images is 1.
     * Derived classes should call setRequiredInputCount to set
     * a different number.
     */
    int requiredInputCount() const;

    /**
     * Returns the maximal number of input images.
     * The default maximal number of input images is 1.
     * Derived classes should call setMaximalInputCount to set
     * a different number.
     */
    int maximalInputCount() const;

    /**
     * Apply the effect on an image.
     * @param image the image the filter should be applied to
     * @param context the render context providing additional data
     */
    virtual QImage processImage(const QImage &image, const KoFilterEffectRenderContext &context) const;

    /**
     * Apply the effect on a list of images.
     * @param images the images the filter should be applied to
     * @param context the render context providing additional data
     */
    virtual QImage processImages(const QVector<QImage> &images, const KoFilterEffectRenderContext &context) const;

    /**
     * Loads data from given xml element.
     * @param element the xml element to load data from
     * @param context the loading context providing additional data
     * @return true if loading was successful, else false
     */
    virtual bool load(const KoXmlElement &element, const KoFilterEffectLoadingContext &context);

    /**
     * Writes custom data to given xml element.
     * @param writer the xml writer to write data to
     */
    virtual void save(KoXmlWriter &writer);

protected:
    /// Sets the required number of input images
    void setRequiredInputCount(int count);

    /// Sets the maximal number of input images
    void setMaximalInputCount(int count);

    /**
     * Saves common filter attributes
     *
     * Saves result, subregion and input attributes. The input attrinbute
     * is only saved if required, maximal and actual input count equals 1.
     * All other filters have to write inputs on their own.
     */
    void saveCommonAttributes(KoXmlWriter &writer);

private:
    class Private;
    Private *const d;
};

#endif // _KO_FILTER_EFFECT_H_
