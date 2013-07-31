#ifndef FAKE_KMESSAGE_H
#define FAKE_KMESSAGE_H

namespace KMessage
{
    enum MessageType
    {
        /**
         * Error message.
         * Display critical information that affect the behavior of the application.
         */
        Error,
        /**
         * Information message.
         * Display useful information to the user.
         */
        Information,
        /**
         * Warning message.
         * Display a message that could affect the behavior of the application.
         */
        Warning,
        /**
         * Sorry message.
         * Display a message explaining that a task couldn't be accomplished.
         */
        Sorry,
        /**
         * Fatal message.
         * Display a message before the application fail and close itself.
         */
        Fatal
    };

    /**
     * @brief Display a long message of a certain type.
     * A long message span on multiple lines and can have a caption.
     *
     * @param messageType Currrent type of message. See MessageType enum.
     * @param text Long message to be displayed.
     * @param caption Caption to be used. This is optional.
     */
    void message(KMessage::MessageType messageType, const QString &text, const QString &caption = QString()) {}


}
#endif
