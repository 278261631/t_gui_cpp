#pragma once

#include "PluginInterface.h"
#include <QObject>
#include <QJsonObject>

class Application;

/**
 * @brief Base plugin class providing common functionality
 * 
 * This class provides a basic implementation of PluginInterface
 * that can be extended by concrete plugins.
 */
class BasePlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit BasePlugin(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~BasePlugin();

    // PluginInterface implementation
    bool initialize(Application* app) override;
    void shutdown() override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    void configure(const QJsonObject& config) override;
    QJsonObject configuration() const override;

protected:
    /**
     * @brief Called during plugin initialization
     * Override this method to perform plugin-specific initialization
     * @return true if successful
     */
    virtual bool onInitialize() { return true; }

    /**
     * @brief Called during plugin shutdown
     * Override this method to perform plugin-specific cleanup
     */
    virtual void onShutdown() {}

    /**
     * @brief Called when plugin is enabled/disabled
     * @param enabled New enabled state
     */
    virtual void onEnabledChanged(bool enabled) { Q_UNUSED(enabled) }

    /**
     * @brief Called when plugin configuration changes
     * @param config New configuration
     */
    virtual void onConfigurationChanged(const QJsonObject& config) { Q_UNUSED(config) }

    /**
     * @brief Get application instance
     * @return Application pointer
     */
    Application* application() const { return m_application; }

private:
    Application* m_application;
    bool m_enabled;
    bool m_initialized;
    QJsonObject m_configuration;
};
