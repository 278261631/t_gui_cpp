#pragma once

#include "PluginInterface.h"
#include <QObject>
#include <QPluginLoader>
#include <QDir>
#include <QJsonObject>
#include <QMap>
#include <memory>

class Application;

/**
 * @brief Plugin information structure
 */
struct PluginInfo
{
    QString fileName;                           ///< Plugin file name
    QString filePath;                          ///< Full file path
    PluginMetadata metadata;                   ///< Plugin metadata
    QPluginLoader* loader;                     ///< Plugin loader
    PluginInterface* instance;                 ///< Plugin instance
    bool loaded;                               ///< Load status
    bool enabled;                              ///< Enable status
    QString errorString;                       ///< Error message if any

    PluginInfo() : loader(nullptr), instance(nullptr), loaded(false), enabled(false) {}
    ~PluginInfo() { delete loader; }
};

/**
 * @brief Plugin manager class
 * 
 * Manages loading, unloading, and lifecycle of plugins.
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param pluginsDir Plugins directory path
     * @param parent Parent object
     */
    explicit PluginManager(const QString& pluginsDir, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~PluginManager();

    /**
     * @brief Load plugins from directory
     * @param directory Directory path
     * @return Number of plugins loaded
     */
    int loadPluginsFromDirectory(const QString& directory);

    /**
     * @brief Load a specific plugin
     * @param filePath Plugin file path
     * @return true if successful
     */
    bool loadPlugin(const QString& filePath);

    /**
     * @brief Unload a plugin
     * @param pluginName Plugin name
     * @return true if successful
     */
    bool unloadPlugin(const QString& pluginName);

    /**
     * @brief Unload all plugins
     */
    void unloadAllPlugins();

    /**
     * @brief Get loaded plugins
     * @return List of plugin names
     */
    QStringList loadedPlugins() const;

    /**
     * @brief Get plugin info
     * @param pluginName Plugin name
     * @return Plugin info or nullptr
     */
    const PluginInfo* pluginInfo(const QString& pluginName) const;

    /**
     * @brief Get plugin instance
     * @param pluginName Plugin name
     * @return Plugin instance or nullptr
     */
    PluginInterface* plugin(const QString& pluginName) const;

    /**
     * @brief Enable or disable a plugin
     * @param pluginName Plugin name
     * @param enabled Enable state
     * @return true if successful
     */
    bool setPluginEnabled(const QString& pluginName, bool enabled);

    /**
     * @brief Check if plugin is enabled
     * @param pluginName Plugin name
     * @return true if enabled
     */
    bool isPluginEnabled(const QString& pluginName) const;

    /**
     * @brief Get plugins by interface type
     * @tparam T Interface type
     * @return List of plugins implementing the interface
     */
    template<typename T>
    QList<T*> pluginsByInterface() const;

    /**
     * @brief Refresh plugin list
     */
    void refresh();

    /**
     * @brief Get plugins directory
     * @return Plugins directory path
     */
    QString pluginsDirectory() const { return m_pluginsDir; }

    /**
     * @brief Set application instance
     * @param app Application instance
     */
    void setApplication(Application* app) { m_application = app; }

signals:
    /**
     * @brief Emitted when a plugin is loaded
     * @param pluginName Plugin name
     */
    void pluginLoaded(const QString& pluginName);

    /**
     * @brief Emitted when a plugin is unloaded
     * @param pluginName Plugin name
     */
    void pluginUnloaded(const QString& pluginName);

    /**
     * @brief Emitted when a plugin is enabled/disabled
     * @param pluginName Plugin name
     * @param enabled Enable state
     */
    void pluginEnabledChanged(const QString& pluginName, bool enabled);

    /**
     * @brief Emitted when plugin loading fails
     * @param filePath Plugin file path
     * @param error Error message
     */
    void pluginLoadFailed(const QString& filePath, const QString& error);

private slots:
    /**
     * @brief Handle plugin destruction
     */
    void onPluginDestroyed();

private:
    /**
     * @brief Validate plugin
     * @param plugin Plugin instance
     * @return true if valid
     */
    bool validatePlugin(PluginInterface* plugin) const;

    /**
     * @brief Initialize plugin
     * @param info Plugin info
     * @return true if successful
     */
    bool initializePlugin(PluginInfo& info);

    /**
     * @brief Shutdown plugin
     * @param info Plugin info
     */
    void shutdownPlugin(PluginInfo& info);

    /**
     * @brief Check plugin dependencies
     * @param metadata Plugin metadata
     * @return true if dependencies are satisfied
     */
    bool checkDependencies(const PluginMetadata& metadata) const;

    /**
     * @brief Load plugin configuration
     * @param pluginName Plugin name
     * @return Configuration object
     */
    QJsonObject loadPluginConfig(const QString& pluginName) const;

    /**
     * @brief Save plugin configuration
     * @param pluginName Plugin name
     * @param config Configuration object
     */
    void savePluginConfig(const QString& pluginName, const QJsonObject& config);

private:
    QString m_pluginsDir;
    Application* m_application;
    QMap<QString, PluginInfo*> m_plugins;
    QStringList m_supportedExtensions;
};

template<typename T>
QList<T*> PluginManager::pluginsByInterface() const
{
    QList<T*> result;
    for (PluginInfo* info : m_plugins) {
        if (info->loaded && info->enabled && info->instance) {
            T* interface = qobject_cast<T*>(info->instance);
            if (interface) {
                result.append(interface);
            }
        }
    }
    return result;
}
