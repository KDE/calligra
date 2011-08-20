#include <cfloat>
#include <boost/tuple/tuple.hpp>
#include <kis_iterator_ng.h>
#include <kis_paint_device.h>
#include <kis_types.h>
#include <KoColorSpaceMaths.h>
#include "kisColorSignature.h"
#include "kisSioxSegmentator.h"


using boost::tuples::get;


namespace {


/**
  Returns the squared euclidian distance between two Lab color pixels.
 */
inline float getLabColorDiffSquared(/*const quint16* c0, const quint16* c1*/);

/**
  Blurs confidence matrix with a given symmetrically weighted kernel. In the
  standard case confidence matrix entries are between 0...1 and the weight
  factors sum up to 1.
 */
void smoothCondidenceMatrix(float matrix[], int xres, int yres, float weight1,
    float weight2, float weight3);

/**
  Normalizes the matrix to values to [0..1].
 */
void normalizeConfidenceMatrix(float confidenceMatrix[], int length);


}

KisSioxSegmentator::KisSioxSegmentator(const KisPaintDeviceSP& pimage,
    const KisPaintDeviceSP& puserConfidenceMatrix, float plimitL, float plimitA,
    float plimitB)
    : image(pimage),
    labImage(new KisPaintDevice(*pimage)),
    userConfidenceMatrix(puserConfidenceMatrix),
    confidenceMatrix(new KisPaintDevice(*puserConfidenceMatrix)),
    labelField(pimage->exactBounds().width() * pimage->exactBounds().height(), -1),
    limitL(plimitL),
    limitA(plimitA),
    limitB(plimitB)
{
    const KoColorSpace* labCS = KoColorSpaceRegistry::instance()->lab16();
    KUndo2Command* cmd = labImage->convertTo(labCS);
    delete cmd;
}

void KisSioxSegmentator::fillColorRegions(float confidenceMatrix[])
{
    QVector< quint64 > pixelsToVisit;
    QRect labImageBounds = labImage->exactBounds();
    int imgWidth = labImageBounds.width();
    int imgHeight = labImageBounds.height();

    // Iteration over confidenceMatrix
    KisRectConstIteratorSP labImageIterator = labImage->createRectConstIteratorNG(
        labImageBounds);
    quint64 i = 0;
    do {
        // Already visited or background.
        if (labelField[i] != -1 ||
            confidenceMatrix[i] < UNKNOWN_REGION_CONFIDENCE) {
            continue;
        }

        //Q_ASSERT(i < imageSize);

        //const quint16* origColor = reinterpret_cast<const quint16*>(
        //    labImageIterator->oldRawData());
        const quint64 curLabel = i + 1;

        labelField[i] = curLabel;
        confidenceMatrix[i] = CERTAIN_FOREGROUND_CONFIDENCE;
        pixelsToVisit.push_back(i);

        // Depth first search to fill region.
        while (!pixelsToVisit.isEmpty()) {
            const int pos(pixelsToVisit.last());
            pixelsToVisit.pop_back();
            const int x = pos % imgWidth;
            const int y = pos / imgWidth;

            // Check all four neighbours.
            const int left = pos - 1;

            // The 'false' is for not fill up util implement getLabColorDiffSquared.
            if (x - 1 >= 0 && labelField[left] == -1 && false
                /*getLabColorDiffSquared(image[left], origColor) < 1.0*/) {

                labelField[left] = curLabel;
                confidenceMatrix[left] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(left);
            }

            const int right = pos + 1;

            if (x + 1 < imgWidth && labelField[right] == -1 && false
                /*getLabColorDiffSquared(image[right], origColor) < 1.0*/) {

                labelField[right] = curLabel;
                confidenceMatrix[right] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(right);
            }

            const int top = pos - imgWidth;

            if (y - 1 >= 0 && labelField[top] == -1 && false
                /*getLabColorDiffSquared(image[top], origColor) < 1.0*/) {

                labelField[top] = curLabel;
                confidenceMatrix[top] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(top);
            }

            const int bottom = pos + imgWidth;

            if (y + 1 < imgHeight && labelField[bottom] == -1 && false
                /*getLabColorDiffSquared(image[bottom], origColor) < 1.0*/) {

                labelField[bottom] = curLabel;
                confidenceMatrix[bottom] = CERTAIN_FOREGROUND_CONFIDENCE;
                pixelsToVisit.push_back(bottom);
            }
        }

        i++;
    } while (labImageIterator->nextPixel());
}

bool KisSioxSegmentator::segmentate(int /*smoothness*/, double /*sizeFactorToKeep*/)
{
    nearestPixels.clear();

    QVector< const quint16* > knownBg, knownFg;

    { // Collect known background and foreground.
        KisRectConstIteratorSP labImageIter =
            labImage->createRectConstIteratorNG(labImage->exactBounds());
        KisRectConstIteratorSP confidenceMatrixIter =
            confidenceMatrix->createRectConstIteratorNG( confidenceMatrix->exactBounds());
        do {
            const quint16* data = reinterpret_cast< const quint16* >(
                labImageIter->oldRawData());
            const quint8* confidenceMatrixData = reinterpret_cast< const quint8* >(
                confidenceMatrixIter->oldRawData());

            if (*confidenceMatrixData <= BACKGROUND_CONFIDENCE * ALPHA8_RANGE)
                knownBg.push_back(data);
            else if (*confidenceMatrixData >= FOREGROUND_CONFIDENCE * ALPHA8_RANGE)
                knownFg.push_back(data);

        }  while (labImageIter->nextPixel() & confidenceMatrixIter->nextPixel());
    }

    // Create color signatures

    // TODO
    // - possibly, the signatures would be better manipulated in floats
    ColorSignature< quint16 > bgColorSign, fgColorSign;

    QVector< QVector< quint16 > > bgSignature, fgSignature;
    bgColorSign.createSignature(bgSignature, knownBg, limitL, limitA, limitB,
        BACKGROUND_CONFIDENCE);
    fgColorSign.createSignature(fgSignature, knownFg, limitL, limitA, limitB,
        BACKGROUND_CONFIDENCE);

    // It is not possible to segmentate de image in this case.
    // TODO - throw a specific exception
    if (bgSignature.size() < 1) {
        return false;
    }

    { // Classify using color signatures.
        KisRectConstIteratorSP labImageIter = labImage->createRectConstIteratorNG(
            labImage->exactBounds());
        KisRectConstIteratorSP imageIter = image->createRectConstIteratorNG(
            image->exactBounds());
        KisRectIteratorSP confidenceMatrixIter = confidenceMatrix->createRectIteratorNG(
            confidenceMatrix->exactBounds());
        do {
            quint8* confidenceMatrixData = reinterpret_cast< quint8* >(
                confidenceMatrixIter->rawData());

            if (*confidenceMatrixData >= FOREGROUND_CONFIDENCE * ALPHA8_RANGE) {
                confidenceMatrixData[0] = CERTAIN_FOREGROUND_CONFIDENCE * ALPHA8_RANGE;
                continue;
            }

            if (*confidenceMatrixData <= BACKGROUND_CONFIDENCE * ALPHA8_RANGE) {
                confidenceMatrixData[0] = CERTAIN_BACKGROUND_CONFIDENCE * ALPHA8_RANGE;
            } else {
                const quint16* data = reinterpret_cast< const quint16* >(
                    imageIter->oldRawData());

                NearestPixelsMap::iterator nearestIterator = nearestPixels.find(data);

                bool isBackground = true;

                if (nearestIterator != nearestPixels.end()) {
                    ClusterDistance& tuple = *nearestIterator;
                    isBackground = get< MIN_BG_DIST >(tuple) <= get< MIN_FG_DIST >(tuple);

                } else {
                    ClusterDistance& tuple =
                        (nearestPixels[data] = ClusterDistance(0, 0, 0, 0));

                    const quint16* labData = reinterpret_cast< const quint16* >(
                        labImageIter->oldRawData());

                    // Convert Lab data to float.
                    float realLabData[SOURCE_COLOR_DIMENSIONS];
                    for (int i = 0; i < SOURCE_COLOR_DIMENSIONS; i++) {
                        realLabData[i] = KoColorSpaceMaths<quint16, float>::scaleToA(
                            labData[i]);
                    }

                    float minBg;// = getLabColorDiffSquared(labData, bgSignature[0]);
                    int minIndex = 0;

                    for (int j = 1; j < bgSignature.size(); j++) {
                        float distace;// = getLabColorDiffSquared(labData, bgSignature[j]);

                        if (distace < minBg) {
                            minBg = distace;
                            minIndex = j;
                        }
                    }

                    get< MIN_BG_DIST >(tuple) = minBg;
                    get< MIN_BG_INDX >(tuple) = minIndex;
                    float minFg = FLT_MAX;
                    minIndex = -1;

                    for (int j = 0; j < fgSignature.size(); j++) {
                        float distace;// = getLabColorDiffSquared(labData, fgSignature[j]);

                        if (distace < minFg) {
                            minFg = distace;
                            minIndex = j;
                        }
                    }

                    get< MIN_FG_DIST >(tuple) = minFg;
                    get< MIN_FG_INDX >(tuple) = minIndex;

                    if (fgSignature.size() == 0) {
                        isBackground;// = (minBg <= clusterSize);
                        // Impossible to segmentate.
                        //throw IllegalStateException: "Foreground signature does not exists.;
                    } else {
                        isBackground = minBg < minFg;
                    }
                }

                if (isBackground) {
                    confidenceMatrixData[0] = CERTAIN_BACKGROUND_CONFIDENCE * ALPHA8_RANGE;
                } else {
                    confidenceMatrixData[0] = CERTAIN_FOREGROUND_CONFIDENCE * ALPHA8_RANGE;
                }
            }
        }  while (labImageIter->nextPixel() & imageIter->nextPixel() &
            confidenceMatrixIter->nextPixel());
    }

    // TODO
    // postprocessing
    // - Smooth confidence matrix (at least one time).
    // - Normalize matrix (at least one time).
    // - Erode.
    // - Remove small components.
    // - Fill color regions.
    // - Dilate.

    return true;
}


namespace {


inline float getLabColorDiffSquared(/*const quint16* c0, const quint16* c1*/) {
    float euclid = 0;

    // TODO Convert c0 and c1 to float
//    for (int k = 0; k < 3; k++) {
//        euclid += (c0[k] - c1[k]) * (c0[k] - c1[k]);
//    }

    return euclid;
}

void smoothCondidenceMatrix(float matrix[], int xres, int yres, float weight1,
    float weight2, float weight3) {

    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres - 2; x++) {
            int idx = (y * xres) + x;
            matrix[idx] = weight1 * matrix[idx] + weight2 * matrix[idx + 1] +
                weight3 * matrix[idx + 2];
        }
    }

    for (int y = 0; y < yres; y++) {
        for (int x = xres - 1; x >= 2; x--) {
            int idx = (y * xres) + x;
            matrix[idx] = weight3 * matrix[idx - 2] + weight2 * matrix[idx - 1] +
                weight1 * matrix[idx];
        }
    }

    for (int y = 0; y < yres - 2; y++) {
        for (int x = 0; x < xres; x++) {
            int idx = (y * xres) + x;
            matrix[idx] = weight1 * matrix[idx] + weight2 * matrix[((y + 1) * xres) + x] +
                weight3 * matrix[((y + 2) * xres) + x];
        }
    }

    for (int y = yres - 1; y >= 2; y--) {
        for (int x = 0; x < xres; x++) {
            int idx = (y * xres) + x;
            matrix[idx] = weight3 * matrix[((y - 2) * xres) + x] + weight2 *
                matrix[((y - 1) * xres) + x] + weight1 * matrix[idx];
        }
    }
}

void normalizeConfidenceMatrix(float confidenceMatrix[], int length) {
    float max = 0.0f;
    for (int i = 0; i < length; i++) {
        if (max < confidenceMatrix[i])
            max = confidenceMatrix[i];
    }

    if (max <= 0.0 || max == 1.00)
        return;

    float alpha = 1.00f / max;

    for (int i = 0; i < length; i++) {
        confidenceMatrix[i] = alpha * confidenceMatrix[i];
    }
}


}
