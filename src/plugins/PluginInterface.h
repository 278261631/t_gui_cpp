#pragma once

#include <QString>
#include <QObject>
#include <QWidget>
#include <QJsonObject>

class Application;

/**
 * @brief Plugin metadata structure
 */
struct PluginMetadata
{
    QString name;           ///< Plugin name
    QString version;        ///< Plugin version
    QString description;    ///< Plugin description
    QString author;         ///< Plugin author
    QString license;        ///< Plugin license
    QStringList dependencies; ///< Plugin dependencies
    
    /**
     * @brief Convert to JSON object
     * @return JSON representation
     */
    QJsonObject toJson() const;
    
    /**
     * @brief Create from JSON object
     * @param json JSON object
     * @return Plugin metadata
     */
    static PluginMetadata fromJson(const QJsonObject& json);
};

/**
 * @brief Base interface for all plugins
 * 
 * This interface defines the basic contract that all plugins must implement.
 * Plugins can extend this interface to provide specific functionality.
 */
class PluginInterface
{
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~PluginInterface() = default;

    /**
     * @brief Get plugin metadata
     * @return Plugin metadata
     */
    virtual PluginMetadata metadata() const = 0;

    /**
     * @brief Initialize the plugin
     * @param app Application instance
     * @return true if initialization successful
     */
    virtual bool initialize(Application* app) = 0;

    /**
     * @brief Shutdown the plugin
     */
    virtual void shutdown() = 0;

    /**
     * @brief Check if plugin is enabled
     * @return true if enabled
     */
    virtual bool isEnabled() const = 0;

    /**
     * @brief Enable or disable the plugin
     * @param enabled Enable state
     */
    virtual void setEnabled(bool enabled) = 0;

    /**
     * @brief Get plugin configuration widget
     * @return Configuration widget or nullptr
     */
    virtual QWidget* configurationWidget() { return nullptr; }

    /**
     * @brief Handle plugin configuration changes
     * @param config Configuration data
     */
    virtual void configure(const QJsonObject& config) { Q_UNUSED(config) }

    /**
     * @brief Get current plugin configuration
     * @return Configuration data
     */
    virtual QJsonObject configuration() const { return QJsonObject(); }
};

/**
 * @brief Interface for UI plugins
 * 
 * UI plugins can provide custom widgets, menu items, and toolbar actions.
 */
class UIPluginInterface : public PluginInterface
{
public:
    /**
     * @brief Get plugin widget
     * @return Plugin widget or nullptr
     */
    virtual QWidget* widget() { return nullptr; }

    /**
     * @brief Get menu actions
     * @return List of menu actions
     */
    virtual QList<QAction*> menuActions() { return QList<QAction*>(); }

    /**
     * @brief Get toolbar actions
     * @return List of toolbar actions
     */
    virtual QList<QAction*> toolbarActions() { return QList<QAction*>(); }

    /**
     * @brief Get dock widget
     * @return Dock widget or nullptr
     */
    virtual QWidget* dockWidget() { return nullptr; }

    /**
     * @brief Get dock widget title
     * @return Dock widget title
     */
    virtual QString dockWidgetTitle() const { return QString(); }

    /**
     * @brief Get preferred dock area
     * @return Dock area
     */
    virtual Qt::DockWidgetArea dockWidgetArea() const { return Qt::RightDockWidgetArea; }
};

/**
 * @brief Interface for data processing plugins
 * 
 * Data processing plugins can manipulate layer data and provide analysis tools.
 */
class DataPluginInterface : public PluginInterface
{
public:
    /**
     * @brief Get supported file formats
     * @return List of supported file extensions
     */
    virtual QStringList supportedFormats() const { return QStringList(); }

    /**
     * @brief Check if plugin can handle file
     * @param fileName File name
     * @return true if plugin can handle the file
     */
    virtual bool canHandle(const QString& fileName) const = 0;

    /**
     * @brief Load data from file
     * @param fileName File name
     * @return true if successful
     */
    virtual bool loadData(const QString& fileName) = 0;

    /**
     * @brief Save data to file
     * @param fileName File name
     * @return true if successful
     */
    virtual bool saveData(const QString& fileName) = 0;
};

// Qt plugin interface macros
Q_DECLARE_INTERFACE(PluginInterface, "org.t-gui.PluginInterface/1.0")
Q_DECLARE_INTERFACE(UIPluginInterface, "org.t-gui.UIPluginInterface/1.0")
Q_DECLARE_INTERFACE(DataPluginInterface, "org.t-gui.DataPluginInterface/1.0")
