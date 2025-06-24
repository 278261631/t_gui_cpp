#include "PluginManager.h"
#include "../core/Application.h"
#include "../utils/Logger.h"

#include <QDir>
#include <QPluginLoader>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

PluginManager::PluginManager(const QString& pluginsDir, QObject* parent)
    : QObject(parent)
    , m_pluginsDir(pluginsDir)
    , m_application(nullptr)
{
    // Supported plugin extensions
    m_supportedExtensions << "dll" << "so" << "dylib";
}

PluginManager::~PluginManager()
{
    unloadAllPlugins();
}

int PluginManager::loadPluginsFromDirectory(const QString& directory)
{
    QDir dir(directory);
    if (!dir.exists()) {
        qWarning() << "Plugin directory does not exist:" << directory;
        return 0;
    }

    int loadedCount = 0;
    QStringList filters;
    for (const QString& ext : m_supportedExtensions) {
        filters << QString("*.%1").arg(ext);
    }

    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    for (const QFileInfo& fileInfo : files) {
        if (loadPlugin(fileInfo.absoluteFilePath())) {
            loadedCount++;
        }
    }

    return loadedCount;
}

bool PluginManager::loadPlugin(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString pluginName = fileInfo.baseName();

    // Check if already loaded
    if (m_plugins.contains(pluginName)) {
        qWarning() << "Plugin already loaded:" << pluginName;
        return false;
    }

    // Create plugin info
    PluginInfo* info = new PluginInfo();
    info->fileName = fileInfo.fileName();
    info->filePath = filePath;
    info->loader = new QPluginLoader(filePath);

    // Load the plugin
    QObject* pluginObject = info->loader->instance();
    if (!pluginObject) {
        info->errorString = info->loader->errorString();
        qWarning() << "Failed to load plugin:" << pluginName << info->errorString;
        return false;
    }

    // Cast to plugin interface
    PluginInterface* plugin = qobject_cast<PluginInterface*>(pluginObject);
    if (!plugin) {
        info->errorString = "Plugin does not implement PluginInterface";
        qWarning() << info->errorString << pluginName;
        info->loader->unload();
        return false;
    }

    info->instance = plugin;
    info->metadata = plugin->metadata();
    info->loaded = true;

    // Initialize plugin
    if (!initializePlugin(*info)) {
        info->loaded = false;
        info->loader->unload();
        delete info;
        return false;
    }

    // Store plugin info
    m_plugins[pluginName] = info;

    emit pluginLoaded(pluginName);
    return true;
}

bool PluginManager::unloadPlugin(const QString& pluginName)
{
    auto it = m_plugins.find(pluginName);
    if (it == m_plugins.end()) {
        return false;
    }

    PluginInfo* info = it.value();
    shutdownPlugin(*info);

    if (info->loader && info->loader->isLoaded()) {
        info->loader->unload();
    }

    delete info;
    m_plugins.erase(it);
    emit pluginUnloaded(pluginName);
    return true;
}

void PluginManager::unloadAllPlugins()
{
    QStringList pluginNames = m_plugins.keys();
    for (const QString& name : pluginNames) {
        unloadPlugin(name);
    }
}

QStringList PluginManager::loadedPlugins() const
{
    return m_plugins.keys();
}

const PluginInfo* PluginManager::pluginInfo(const QString& pluginName) const
{
    auto it = m_plugins.find(pluginName);
    return (it != m_plugins.end()) ? it.value() : nullptr;
}

PluginInterface* PluginManager::plugin(const QString& pluginName) const
{
    const PluginInfo* info = pluginInfo(pluginName);
    return info ? info->instance : nullptr;
}

bool PluginManager::setPluginEnabled(const QString& pluginName, bool enabled)
{
    auto it = m_plugins.find(pluginName);
    if (it == m_plugins.end()) {
        return false;
    }

    PluginInfo* info = it.value();
    if (info->enabled == enabled) {
        return true;
    }

    if (info->instance) {
        info->instance->setEnabled(enabled);
        info->enabled = enabled;
        emit pluginEnabledChanged(pluginName, enabled);
        return true;
    }

    return false;
}

bool PluginManager::isPluginEnabled(const QString& pluginName) const
{
    const PluginInfo* info = pluginInfo(pluginName);
    return info ? info->enabled : false;
}

void PluginManager::refresh()
{
    loadPluginsFromDirectory(m_pluginsDir);
}

bool PluginManager::validatePlugin(PluginInterface* plugin) const
{
    if (!plugin) {
        return false;
    }

    PluginMetadata metadata = plugin->metadata();
    if (metadata.name.isEmpty()) {
        return false;
    }

    return true;
}

bool PluginManager::initializePlugin(PluginInfo& info)
{
    if (!info.instance || !m_application) {
        return false;
    }

    if (!validatePlugin(info.instance)) {
        info.errorString = "Plugin validation failed";
        return false;
    }

    if (!checkDependencies(info.metadata)) {
        info.errorString = "Plugin dependencies not satisfied";
        return false;
    }

    if (!info.instance->initialize(m_application)) {
        info.errorString = "Plugin initialization failed";
        return false;
    }

    info.enabled = true;
    return true;
}

void PluginManager::shutdownPlugin(PluginInfo& info)
{
    if (info.instance) {
        info.instance->shutdown();
        info.enabled = false;
    }
}

bool PluginManager::checkDependencies(const PluginMetadata& metadata) const
{
    for (const QString& dependency : metadata.dependencies) {
        if (!m_plugins.contains(dependency)) {
            return false;
        }
    }
    return true;
}

QJsonObject PluginManager::loadPluginConfig(const QString& pluginName) const
{
    // TODO: Implement plugin configuration loading
    Q_UNUSED(pluginName)
    return QJsonObject();
}

void PluginManager::savePluginConfig(const QString& pluginName, const QJsonObject& config)
{
    // TODO: Implement plugin configuration saving
    Q_UNUSED(pluginName)
    Q_UNUSED(config)
}

void PluginManager::onPluginDestroyed()
{
    // Handle plugin object destruction
    QObject* obj = sender();
    if (obj) {
        // Find and remove the plugin from our list
        for (auto it = m_plugins.begin(); it != m_plugins.end(); ++it) {
            PluginInfo* info = it.value();
            QObject* pluginObj = dynamic_cast<QObject*>(info->instance);
            if (pluginObj == obj) {
                info->instance = nullptr;
                break;
            }
        }
    }
}
