#ifndef FAKE_KIOPREVIEWJOB_H
#define FAKE_KIOPREVIEWJOB_H

#include <QDebug>
#include <kfileitem.h>
#include <kio/job.h>

class QPixmap;

namespace KIO {

    class PreviewJob : public KIO::Job
    {
        //Q_OBJECT
    public:
        enum ScaleType { Unscaled, Scaled, ScaledAndCached };
        PreviewJob(const KFileItemList &items, const QSize &size, const QStringList *enabledPlugins = 0) : KIO::Job() {}

#if 0
        /**
         * Sets the size of the MIME-type icon which overlays the preview. If zero
         * is passed no overlay will be shown at all. The setting has no effect if
         * the preview plugin that will be used does not use icon overlays. Per
         * default the size is set to 0.
         * @since 4.7
         */
        void setOverlayIconSize(int size);

        /**
         * @return The size of the MIME-type icon which overlays the preview.
         * @see PreviewJob::setOverlayIconSize()
         * @since 4.7
         */
        int overlayIconSize() const;

        /**
         * Sets the alpha-value for the MIME-type icon which overlays the preview.
         * The alpha-value may range from 0 (= fully transparent) to 255 (= opaque).
         * Per default the value is set to 70.
         * @see PreviewJob::setOverlayIconSize()
         * @since 4.7
         */
        void setOverlayIconAlpha(int alpha);

        /**
         * @return The alpha-value for the MIME-type icon which overlays the preview.
         *         Per default 70 is returned.
         * @see PreviewJob::setOverlayIconAlpha()
         * @since 4.7
         */
        int overlayIconAlpha() const;

        /**
         * Sets the scale type for the generated preview. Per default
         * PreviewJob::ScaledAndCached is set.
         * @see PreviewJob::ScaleType
         * @since 4.7
         */
        void setScaleType(ScaleType type);

        /**
         * @return The scale type for the generated preview.
         * @see PreviewJob::ScaleType
         * @since 4.7
         */
        ScaleType scaleType() const;

        /**
         * Removes an item from preview processing. Use this if you passed
         * an item to filePreview and want to delete it now.
         *
         * @param url the url of the item that should be removed from the preview queue
         */
        void removeItem( const KUrl& url );

        /**
         * If @p ignoreSize is true, then the preview is always
         * generated regardless of the settings
         **/
        void setIgnoreMaximumSize(bool ignoreSize = true);

        /**
         * Sets the sequence index given to the thumb creators.
         * Use the sequence index, it is possible to create alternative
         * icons for the same item. For example it may allow iterating through
         * the items of a directory, or the frames of a video.
         *
         * @since KDE 4.3
         **/
        void setSequenceIndex(int index);

        /**
         * Returns the currently set sequence index
         *
         * @since KDE 4.3
         **/
        int sequenceIndex() const;
#endif

        static QStringList availablePlugins() { return QStringList(); }
        static QStringList supportedMimeTypes() { return QStringList(); }

#if 0
    Q_SIGNALS:
        void gotPreview( const KFileItem& item, const QPixmap &preview );
        void failed( const KFileItem& item );
#endif
    };

    PreviewJob *filePreview(const KFileItemList &items, const QSize &size, const QStringList *enabledPlugins = 0)
    {
        return new PreviewJob(items, size, enabledPlugins);
    }

}

#endif
