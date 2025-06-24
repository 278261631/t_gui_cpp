#include "EventSystem.h"
#include <QCoreApplication>
#include <QDebug>

EventSystem* EventSystem::s_instance = nullptr;

EventSystem::EventSystem(QObject* parent)
    : QObject(parent)
    , m_nextSubscriptionId(1)
    , m_eventLogging(false)
{
    s_instance = this;
}

EventSystem::~EventSystem()
{
    clear();
    s_instance = nullptr;
}

EventSystem* EventSystem::instance()
{
    return s_instance;
}

int EventSystem::subscribe(CustomEventType eventType, EventHandler handler)
{
    return subscribe(eventType, nullptr, handler);
}

int EventSystem::subscribe(CustomEventType eventType, QObject* receiver, EventHandler handler)
{
    int subscriptionId = m_nextSubscriptionId++;
    
    Subscription subscription(subscriptionId, eventType, receiver, handler);
    m_subscriptions[eventType].append(subscription);
    
    if (receiver) {
        m_objectSubscriptions[receiver].append(subscriptionId);
        connect(receiver, &QObject::destroyed, this, &EventSystem::onObjectDestroyed);
    }
    
    return subscriptionId;
}

void EventSystem::unsubscribe(int subscriptionId)
{
    for (auto& subscriptionList : m_subscriptions) {
        for (int i = 0; i < subscriptionList.size(); ++i) {
            if (subscriptionList[i].id == subscriptionId) {
                QObject* receiver = subscriptionList[i].receiver;
                subscriptionList.removeAt(i);
                
                // Remove from object subscriptions
                if (receiver && m_objectSubscriptions.contains(receiver)) {
                    m_objectSubscriptions[receiver].removeAll(subscriptionId);
                    if (m_objectSubscriptions[receiver].isEmpty()) {
                        m_objectSubscriptions.remove(receiver);
                    }
                }
                return;
            }
        }
    }
}

void EventSystem::unsubscribe(QObject* receiver)
{
    if (!receiver || !m_objectSubscriptions.contains(receiver)) {
        return;
    }
    
    QList<int> subscriptionIds = m_objectSubscriptions[receiver];
    for (int id : subscriptionIds) {
        unsubscribe(id);
    }
}

void EventSystem::publish(CustomEventType eventType, const QVariant& data)
{
    if (m_eventLogging) {
        logEvent(eventType, data);
    }
    
    CustomEvent event(eventType, data);
    processEvent(event);
    
    emit eventPublished(eventType, data);
}

void EventSystem::publishAsync(CustomEventType eventType, const QVariant& data)
{
    CustomEvent* event = new CustomEvent(eventType, data);
    QCoreApplication::postEvent(this, event);
}

bool EventSystem::hasSubscribers(CustomEventType eventType) const
{
    return m_subscriptions.contains(eventType) && !m_subscriptions[eventType].isEmpty();
}

int EventSystem::subscriberCount(CustomEventType eventType) const
{
    if (m_subscriptions.contains(eventType)) {
        return m_subscriptions[eventType].size();
    }
    return 0;
}

void EventSystem::clear()
{
    m_subscriptions.clear();
    m_objectSubscriptions.clear();
}

bool EventSystem::event(QEvent* event)
{
    if (event->type() >= static_cast<QEvent::Type>(CustomEventType::LayerAdded)) {
        CustomEvent* customEvent = static_cast<CustomEvent*>(event);
        processEvent(*customEvent);
        return true;
    }
    
    return QObject::event(event);
}

void EventSystem::onObjectDestroyed(QObject* obj)
{
    unsubscribe(obj);
}

void EventSystem::processEvent(const CustomEvent& event)
{
    CustomEventType eventType = event.customType();
    
    if (!m_subscriptions.contains(eventType)) {
        return;
    }
    
    const QList<Subscription>& subscriptions = m_subscriptions[eventType];
    for (const Subscription& subscription : subscriptions) {
        try {
            subscription.handler(event);
        } catch (const std::exception& e) {
            qWarning() << "Exception in event handler:" << e.what();
        } catch (...) {
            qWarning() << "Unknown exception in event handler";
        }
    }
}

void EventSystem::logEvent(CustomEventType eventType, const QVariant& data)
{
    QString eventName = eventTypeName(eventType);
    QString dataStr = data.toString();
    
    if (dataStr.length() > 100) {
        dataStr = dataStr.left(97) + "...";
    }
    
    qDebug() << "Event:" << eventName << "Data:" << dataStr;
}

QString EventSystem::eventTypeName(CustomEventType eventType) const
{
    switch (eventType) {
    case CustomEventType::LayerAdded:           return "LayerAdded";
    case CustomEventType::LayerRemoved:        return "LayerRemoved";
    case CustomEventType::LayerChanged:        return "LayerChanged";
    case CustomEventType::LayerSelectionChanged: return "LayerSelectionChanged";
    case CustomEventType::ViewChanged:         return "ViewChanged";
    case CustomEventType::ZoomChanged:         return "ZoomChanged";
    case CustomEventType::ToolChanged:         return "ToolChanged";
    case CustomEventType::PluginLoaded:        return "PluginLoaded";
    case CustomEventType::PluginUnloaded:      return "PluginUnloaded";
    case CustomEventType::ConfigurationChanged: return "ConfigurationChanged";
    case CustomEventType::FileOpened:          return "FileOpened";
    case CustomEventType::FileSaved:           return "FileSaved";
    default:                                    return "Unknown";
    }
}
