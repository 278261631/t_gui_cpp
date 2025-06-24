#pragma once

#include <QObject>
#include <QEvent>
#include <QVariant>
#include <QHash>
#include <QList>
#include <functional>

/**
 * @brief Custom event types
 */
enum class CustomEventType
{
    LayerAdded = QEvent::User + 1,
    LayerRemoved,
    LayerChanged,
    LayerSelectionChanged,
    ViewChanged,
    ZoomChanged,
    ToolChanged,
    PluginLoaded,
    PluginUnloaded,
    ConfigurationChanged,
    FileOpened,
    FileSaved,
    UserDefined = QEvent::User + 1000
};

/**
 * @brief Custom event class
 */
class CustomEvent : public QEvent
{
public:
    /**
     * @brief Constructor
     * @param type Event type
     * @param data Event data
     */
    CustomEvent(CustomEventType type, const QVariant& data = QVariant())
        : QEvent(static_cast<QEvent::Type>(type))
        , m_data(data)
    {
    }

    /**
     * @brief Get event data
     * @return Event data
     */
    QVariant data() const { return m_data; }

    /**
     * @brief Set event data
     * @param data Event data
     */
    void setData(const QVariant& data) { m_data = data; }

    /**
     * @brief Get custom event type
     * @return Custom event type
     */
    CustomEventType customType() const { return static_cast<CustomEventType>(type()); }

private:
    QVariant m_data;
};

/**
 * @brief Event handler function type
 */
using EventHandler = std::function<void(const CustomEvent&)>;

/**
 * @brief Event system class
 * 
 * Provides a centralized event system for communication between components.
 */
class EventSystem : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit EventSystem(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~EventSystem();

    /**
     * @brief Get singleton instance
     * @return EventSystem instance
     */
    static EventSystem* instance();

    /**
     * @brief Subscribe to event type
     * @param eventType Event type
     * @param handler Event handler function
     * @return Subscription ID
     */
    int subscribe(CustomEventType eventType, EventHandler handler);

    /**
     * @brief Subscribe to event type with object context
     * @param eventType Event type
     * @param receiver Receiver object
     * @param handler Event handler function
     * @return Subscription ID
     */
    int subscribe(CustomEventType eventType, QObject* receiver, EventHandler handler);

    /**
     * @brief Unsubscribe from event
     * @param subscriptionId Subscription ID
     */
    void unsubscribe(int subscriptionId);

    /**
     * @brief Unsubscribe all handlers for an object
     * @param receiver Receiver object
     */
    void unsubscribe(QObject* receiver);

    /**
     * @brief Publish event
     * @param eventType Event type
     * @param data Event data
     */
    void publish(CustomEventType eventType, const QVariant& data = QVariant());

    /**
     * @brief Publish event asynchronously
     * @param eventType Event type
     * @param data Event data
     */
    void publishAsync(CustomEventType eventType, const QVariant& data = QVariant());

    /**
     * @brief Check if there are subscribers for event type
     * @param eventType Event type
     * @return true if there are subscribers
     */
    bool hasSubscribers(CustomEventType eventType) const;

    /**
     * @brief Get number of subscribers for event type
     * @param eventType Event type
     * @return Number of subscribers
     */
    int subscriberCount(CustomEventType eventType) const;

    /**
     * @brief Clear all subscriptions
     */
    void clear();

    /**
     * @brief Enable/disable event logging
     * @param enabled Logging state
     */
    void setEventLogging(bool enabled) { m_eventLogging = enabled; }

    /**
     * @brief Check if event logging is enabled
     * @return true if enabled
     */
    bool eventLogging() const { return m_eventLogging; }

signals:
    /**
     * @brief Emitted when an event is published
     * @param eventType Event type
     * @param data Event data
     */
    void eventPublished(CustomEventType eventType, const QVariant& data);

protected:
    /**
     * @brief Handle custom events
     * @param event Event
     * @return true if handled
     */
    bool event(QEvent* event) override;

private slots:
    /**
     * @brief Handle object destruction
     * @param obj Destroyed object
     */
    void onObjectDestroyed(QObject* obj);

private:
    /**
     * @brief Subscription information
     */
    struct Subscription
    {
        int id;
        CustomEventType eventType;
        QObject* receiver;
        EventHandler handler;
        
        Subscription(int id, CustomEventType type, QObject* recv, EventHandler h)
            : id(id), eventType(type), receiver(recv), handler(h) {}
    };

    /**
     * @brief Process event synchronously
     * @param event Custom event
     */
    void processEvent(const CustomEvent& event);

    /**
     * @brief Log event
     * @param eventType Event type
     * @param data Event data
     */
    void logEvent(CustomEventType eventType, const QVariant& data);

    /**
     * @brief Get event type name
     * @param eventType Event type
     * @return Event type name
     */
    QString eventTypeName(CustomEventType eventType) const;

private:
    static EventSystem* s_instance;

    // Subscriptions
    QHash<CustomEventType, QList<Subscription>> m_subscriptions;
    QHash<QObject*, QList<int>> m_objectSubscriptions;
    int m_nextSubscriptionId;

    // Configuration
    bool m_eventLogging;
};

/**
 * @brief Hash function for CustomEventType
 */
inline uint qHash(CustomEventType eventType, uint seed = 0)
{
    return qHash(static_cast<int>(eventType), seed);
}
