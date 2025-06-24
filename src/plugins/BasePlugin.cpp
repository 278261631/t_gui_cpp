#include "BasePlugin.h"
#include "../core/Application.h"

BasePlugin::BasePlugin(QObject* parent)
    : QObject(parent)
    , m_application(nullptr)
    , m_enabled(false)
    , m_initialized(false)
{
}

BasePlugin::~BasePlugin()
{
    if (m_initialized) {
        shutdown();
    }
}

bool BasePlugin::initialize(Application* app)
{
    if (m_initialized) {
        return true;
    }

    m_application = app;
    
    if (!onInitialize()) {
        m_application = nullptr;
        return false;
    }

    m_initialized = true;
    m_enabled = true;
    
    return true;
}

void BasePlugin::shutdown()
{
    if (!m_initialized) {
        return;
    }

    onShutdown();
    
    m_enabled = false;
    m_initialized = false;
    m_application = nullptr;
}

bool BasePlugin::isEnabled() const
{
    return m_enabled && m_initialized;
}

void BasePlugin::setEnabled(bool enabled)
{
    if (m_enabled == enabled || !m_initialized) {
        return;
    }

    m_enabled = enabled;
    onEnabledChanged(enabled);
}

void BasePlugin::configure(const QJsonObject& config)
{
    m_configuration = config;
    onConfigurationChanged(config);
}

QJsonObject BasePlugin::configuration() const
{
    return m_configuration;
}
