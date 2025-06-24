#pragma once

#include <QApplication>
#include <QDir>
#include <memory>

class MainWindow;
class PluginManager;
class LayerManager;
class EventSystem;
class Logger;
class Config;

/**
 * @brief Main application class for the GUI framework
 * 
 * This class manages the overall application lifecycle, including
 * initialization of core components, plugin loading, and cleanup.
 */
class Application : public QApplication
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param argc Command line argument count
     * @param argv Command line arguments
     */
    Application(int& argc, char** argv);
    
    /**
     * @brief Destructor
     */
    ~Application();

    /**
     * @brief Initialize the application
     * @return true if initialization successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Get the singleton instance
     * @return Application instance
     */
    static Application* instance();

    /**
     * @brief Get the main window
     * @return Pointer to main window
     */
    MainWindow* mainWindow() const { return m_mainWindow.get(); }

    /**
     * @brief Get the plugin manager
     * @return Pointer to plugin manager
     */
    PluginManager* pluginManager() const { return m_pluginManager.get(); }

    /**
     * @brief Get the layer manager
     * @return Pointer to layer manager
     */
    LayerManager* layerManager() const { return m_layerManager.get(); }

    /**
     * @brief Get the event system
     * @return Pointer to event system
     */
    EventSystem* eventSystem() const { return m_eventSystem.get(); }

    /**
     * @brief Get the logger
     * @return Pointer to logger
     */
    Logger* logger() const { return m_logger.get(); }

    /**
     * @brief Get the configuration manager
     * @return Pointer to config manager
     */
    Config* config() const { return m_config.get(); }

    /**
     * @brief Get application data directory
     * @return Path to application data directory
     */
    QString dataDirectory() const { return m_dataDir; }

    /**
     * @brief Get plugins directory
     * @return Path to plugins directory
     */
    QString pluginsDirectory() const { return m_pluginsDir; }

public slots:
    /**
     * @brief Show the main window
     */
    void showMainWindow();

    /**
     * @brief Quit the application gracefully
     */
    void quit();

private slots:
    /**
     * @brief Handle application about to quit
     */
    void onAboutToQuit();

private:
    /**
     * @brief Initialize core components
     * @return true if successful
     */
    bool initializeCore();

    /**
     * @brief Initialize directories
     * @return true if successful
     */
    bool initializeDirectories();

    /**
     * @brief Load plugins
     * @return true if successful
     */
    bool loadPlugins();

    /**
     * @brief Setup connections between components
     */
    void setupConnections();

private:
    static Application* s_instance;

    // Core components
    std::unique_ptr<MainWindow> m_mainWindow;
    std::unique_ptr<PluginManager> m_pluginManager;
    std::unique_ptr<LayerManager> m_layerManager;
    std::unique_ptr<EventSystem> m_eventSystem;
    std::unique_ptr<Logger> m_logger;
    std::unique_ptr<Config> m_config;

    // Directories
    QString m_dataDir;
    QString m_pluginsDir;
    QString m_configDir;

    // State
    bool m_initialized;
};
