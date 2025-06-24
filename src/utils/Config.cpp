#include "Config.h"
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDebug>

Config::Config(const QString& configDir, QObject* parent)
    : QObject(parent)
    , m_loaded(false)
    , m_modified(false)
{
    if (configDir.isEmpty()) {
        m_configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    } else {
        m_configDir = configDir;
    }
    
    m_configFilePath = QDir(m_configDir).filePath("config.json");
    
    // Initialize QSettings
    m_settings = std::make_unique<QSettings>(
        QDir(m_configDir).filePath("settings.ini"),
        QSettings::IniFormat
    );
    
    initializeDefaults();
}

Config::~Config()
{
    if (m_modified) {
        save();
    }
}

bool Config::load()
{
    if (m_loaded) {
        return true;
    }
    
    // Create config directory if it doesn't exist
    if (!createConfigDirectory()) {
        return false;
    }
    
    // Load from JSON file if it exists
    QFile file(m_configFilePath);
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        
        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            fromJson(doc.object());
        } else {
            qWarning() << "Failed to parse config file:" << error.errorString();
        }
    }
    
    // Migrate old configuration if needed
    migrateConfiguration();
    
    m_loaded = true;
    emit configurationLoaded();
    return true;
}

bool Config::save()
{
    if (!createConfigDirectory()) {
        return false;
    }
    
    // Save to JSON file
    QJsonObject config = toJson();
    QJsonDocument doc(config);
    
    QFile file(m_configFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << m_configFilePath;
        return false;
    }
    
    file.write(doc.toJson());
    file.close();
    
    // Sync QSettings
    m_settings->sync();
    
    m_modified = false;
    emit configurationSaved();
    return true;
}

QVariant Config::value(const QString& key, const QVariant& defaultValue) const
{
    QString fullKeyPath = fullKey(key);
    return m_settings->value(fullKeyPath, defaultValue);
}

void Config::setValue(const QString& key, const QVariant& value)
{
    QString fullKeyPath = fullKey(key);
    QVariant oldValue = m_settings->value(fullKeyPath);
    
    if (oldValue != value) {
        m_settings->setValue(fullKeyPath, value);
        m_modified = true;
        emit configurationChanged(key, value);
    }
}

bool Config::contains(const QString& key) const
{
    QString fullKeyPath = fullKey(key);
    return m_settings->contains(fullKeyPath);
}

void Config::remove(const QString& key)
{
    QString fullKeyPath = fullKey(key);
    if (m_settings->contains(fullKeyPath)) {
        m_settings->remove(fullKeyPath);
        m_modified = true;
        emit configurationChanged(key, QVariant());
    }
}

QStringList Config::allKeys() const
{
    return m_settings->allKeys();
}

void Config::clear()
{
    m_settings->clear();
    m_modified = true;
}

QJsonObject Config::group(const QString& group) const
{
    QJsonObject result;
    m_settings->beginGroup(group);
    QStringList keys = m_settings->allKeys();
    for (const QString& key : keys) {
        QVariant value = m_settings->value(key);
        result[key] = QJsonValue::fromVariant(value);
    }
    m_settings->endGroup();
    return result;
}

void Config::setGroup(const QString& group, const QJsonObject& values)
{
    m_settings->beginGroup(group);
    m_settings->remove(""); // Clear existing values in group
    
    for (auto it = values.begin(); it != values.end(); ++it) {
        m_settings->setValue(it.key(), it.value().toVariant());
    }
    
    m_settings->endGroup();
    m_modified = true;
}

void Config::beginGroup(const QString& group)
{
    m_groupStack.append(group);
    m_settings->beginGroup(group);
}

void Config::endGroup()
{
    if (!m_groupStack.isEmpty()) {
        m_groupStack.removeLast();
        m_settings->endGroup();
    }
}

QString Config::currentGroup() const
{
    return m_groupStack.join("/");
}

void Config::setConfigFilePath(const QString& filePath)
{
    m_configFilePath = filePath;
    QFileInfo fileInfo(filePath);
    m_configDir = fileInfo.dir().absolutePath();
}

bool Config::importFromJson(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }
    
    fromJson(doc.object());
    return true;
}

bool Config::exportToJson(const QString& filePath) const
{
    QJsonObject config = toJson();
    QJsonDocument doc(config);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    file.write(doc.toJson());
    return true;
}

QJsonObject Config::toJson() const
{
    QJsonObject result;
    QStringList keys = m_settings->allKeys();

    for (const QString& key : keys) {
        QVariant value = m_settings->value(key);
        result[key] = QJsonValue::fromVariant(value);
    }

    return result;
}

void Config::fromJson(const QJsonObject& json)
{
    // Flatten JSON object to QSettings keys
    std::function<void(const QJsonObject&, const QString&)> flatten;
    flatten = [&](const QJsonObject& obj, const QString& prefix) {
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            QString key = prefix.isEmpty() ? it.key() : prefix + "/" + it.key();
            
            if (it.value().isObject()) {
                flatten(it.value().toObject(), key);
            } else {
                m_settings->setValue(key, it.value().toVariant());
            }
        }
    };
    
    flatten(json, QString());
    m_modified = true;
}

void Config::resetToDefaults()
{
    clear();
    fromJson(m_defaults);
}

QJsonObject Config::defaultConfiguration()
{
    QJsonObject defaults;
    
    // Application settings
    QJsonObject app;
    app["language"] = "en";
    app["theme"] = "dark";
    app["autoSave"] = true;
    app["autoSaveInterval"] = 300; // 5 minutes
    defaults["application"] = app;
    
    // UI settings
    QJsonObject ui;
    ui["showGrid"] = true;
    ui["showAxes"] = true;
    ui["backgroundColor"] = "#404040";
    ui["gridColor"] = "#808080";
    ui["axesColor"] = "#FFFFFF";
    defaults["ui"] = ui;
    
    // Viewer settings
    QJsonObject viewer;
    viewer["defaultZoom"] = 1.0;
    viewer["zoomStep"] = 0.1;
    viewer["panSensitivity"] = 1.0;
    viewer["rotationSensitivity"] = 1.0;
    defaults["viewer"] = viewer;
    
    // Plugin settings
    QJsonObject plugins;
    plugins["autoLoad"] = true;
    plugins["enabledPlugins"] = QJsonArray();
    defaults["plugins"] = plugins;
    
    return defaults;
}

void Config::onSettingsChanged(const QString& key)
{
    emit configurationChanged(key, m_settings->value(key));
}

void Config::initializeDefaults()
{
    m_defaults = defaultConfiguration();
}

bool Config::createConfigDirectory()
{
    QDir dir(m_configDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Failed to create config directory:" << m_configDir;
        return false;
    }
    return true;
}

bool Config::validateConfiguration() const
{
    // Basic validation - check if critical settings exist
    return contains("application/language");
}

bool Config::migrateConfiguration()
{
    // TODO: Implement configuration migration for version updates
    return true;
}

QString Config::fullKey(const QString& key) const
{
    if (m_groupStack.isEmpty()) {
        return key;
    }
    return m_groupStack.join("/") + "/" + key;
}
