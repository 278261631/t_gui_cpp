#include "Application.h"
#include "MainWindow.h"
#include "../plugins/PluginManager.h"
#include "LayerManager.h"
#include "EventSystem.h"
#include "../utils/Logger.h"
#include "../utils/Config.h"

#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

Application* Application::s_instance = nullptr;

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_initialized(false)
{
    s_instance = this;
    
    // Set application properties
    setApplicationName("T-GUI Framework");
    setApplicationVersion("1.0.0");
    setOrganizationName("T-GUI");
    setOrganizationDomain("t-gui.org");

    // Connect quit signal
    connect(this, &QApplication::aboutToQuit, this, &Application::onAboutToQuit);
}

Application::~Application()
{
    s_instance = nullptr;
}

bool Application::initialize()
{
    if (m_initialized) {
        return true;
    }

    qDebug() << "Initializing T-GUI Framework...";

    // Initialize directories first
    if (!initializeDirectories()) {
        qCritical() << "Failed to initialize directories";
        return false;
    }

    // Initialize core components
    if (!initializeCore()) {
        qCritical() << "Failed to initialize core components";
        return false;
    }

    // Setup connections between components
    setupConnections();

    // Load plugins
    if (!loadPlugins()) {
        qWarning() << "Some plugins failed to load";
        // Don't fail initialization for plugin loading issues
    }

    m_initialized = true;
    qDebug() << "T-GUI Framework initialized successfully";
    
    return true;
}

Application* Application::instance()
{
    return s_instance;
}

void Application::showMainWindow()
{
    if (m_logger) {
        m_logger->info("Showing main window...");
    }

    if (m_mainWindow) {
        m_mainWindow->show();
        m_mainWindow->raise();
        m_mainWindow->activateWindow();

        if (m_logger) {
            m_logger->info("Main window shown successfully");
        }
    } else {
        if (m_logger) {
            m_logger->error("Main window is null!");
        }
    }
}

void Application::quit()
{
    if (m_logger) {
        m_logger->info("Application shutting down...");
    }
    
    QApplication::quit();
}

void Application::onAboutToQuit()
{
    if (m_logger) {
        m_logger->info("Application about to quit");
    }

    // Save configuration
    if (m_config) {
        m_config->save();
    }

    // Cleanup plugins
    if (m_pluginManager) {
        m_pluginManager->unloadAllPlugins();
    }
}

bool Application::initializeCore()
{
    try {
        // Initialize logger first
        m_logger = std::make_unique<Logger>();
        m_logger->info("Logger initialized");

        // Initialize configuration
        m_config = std::make_unique<Config>(m_configDir);
        m_config->load();
        m_logger->info("Configuration loaded");

        // Initialize event system
        m_eventSystem = std::make_unique<EventSystem>();
        m_logger->info("Event system initialized");

        // Initialize layer manager
        m_layerManager = std::make_unique<LayerManager>();
        m_logger->info("Layer manager initialized");

        // Initialize plugin manager
        m_pluginManager = std::make_unique<PluginManager>(m_pluginsDir);
        m_logger->info("Plugin manager initialized");

        // Initialize main window
        m_logger->info("Creating main window...");
        m_mainWindow = std::make_unique<MainWindow>();
        m_logger->info("Main window initialized");

        return true;
    }
    catch (const std::exception& e) {
        qCritical() << "Exception during core initialization:" << e.what();
        return false;
    }
}

bool Application::initializeDirectories()
{
    // Get standard application data directory
    m_dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_configDir = QDir(m_dataDir).filePath("config");
    m_pluginsDir = QDir(m_dataDir).filePath("plugins");

    // Create directories if they don't exist
    QDir dataDir(m_dataDir);
    if (!dataDir.exists() && !dataDir.mkpath(".")) {
        qCritical() << "Failed to create data directory:" << m_dataDir;
        return false;
    }

    QDir configDir(m_configDir);
    if (!configDir.exists() && !configDir.mkpath(".")) {
        qCritical() << "Failed to create config directory:" << m_configDir;
        return false;
    }

    QDir pluginsDir(m_pluginsDir);
    if (!pluginsDir.exists() && !pluginsDir.mkpath(".")) {
        qCritical() << "Failed to create plugins directory:" << m_pluginsDir;
        return false;
    }

    qDebug() << "Data directory:" << m_dataDir;
    qDebug() << "Config directory:" << m_configDir;
    qDebug() << "Plugins directory:" << m_pluginsDir;

    return true;
}

bool Application::loadPlugins()
{
    if (!m_pluginManager) {
        qWarning() << "Plugin manager not initialized";
        return false;
    }

    // Load plugins from the plugins directory
    int loadedCount = m_pluginManager->loadPluginsFromDirectory(m_pluginsDir);
    
    // Also try to load from application directory
    QString appPluginsDir = QDir(applicationDirPath()).filePath("plugins");
    if (QDir(appPluginsDir).exists()) {
        loadedCount += m_pluginManager->loadPluginsFromDirectory(appPluginsDir);
    }

    m_logger->info(QString("Loaded %1 plugins").arg(loadedCount));
    return true;
}

void Application::setupConnections()
{
    // Connect layer manager to main window
    if (m_layerManager && m_mainWindow) {
        // These connections will be implemented when we create the specific classes
    }

    // Connect event system to components
    if (m_eventSystem) {
        // Event system connections will be set up here
    }
}
