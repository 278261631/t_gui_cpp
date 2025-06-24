#pragma once

#include <QObject>
#include <QSettings>
#include <QVariant>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QStringList>
#include <memory>

/**
 * @brief Configuration manager class
 * 
 * Manages application settings and configuration with support for
 * both QSettings and JSON configuration files.
 */
class Config : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param configDir Configuration directory
     * @param parent Parent object
     */
    explicit Config(const QString& configDir = QString(), QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Config();

    /**
     * @brief Load configuration
     * @return true if successful
     */
    bool load();

    /**
     * @brief Save configuration
     * @return true if successful
     */
    bool save();

    /**
     * @brief Get configuration value
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return Configuration value
     */
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    /**
     * @brief Set configuration value
     * @param key Configuration key
     * @param value Configuration value
     */
    void setValue(const QString& key, const QVariant& value);

    /**
     * @brief Check if key exists
     * @param key Configuration key
     * @return true if key exists
     */
    bool contains(const QString& key) const;

    /**
     * @brief Remove configuration key
     * @param key Configuration key
     */
    void remove(const QString& key);

    /**
     * @brief Get all keys
     * @return List of all keys
     */
    QStringList allKeys() const;

    /**
     * @brief Clear all configuration
     */
    void clear();

    /**
     * @brief Get configuration group
     * @param group Group name
     * @return Configuration values in group
     */
    QJsonObject group(const QString& group) const;

    /**
     * @brief Set configuration group
     * @param group Group name
     * @param values Group values
     */
    void setGroup(const QString& group, const QJsonObject& values);

    /**
     * @brief Begin configuration group
     * @param group Group name
     */
    void beginGroup(const QString& group);

    /**
     * @brief End configuration group
     */
    void endGroup();

    /**
     * @brief Get current group
     * @return Current group name
     */
    QString currentGroup() const;

    /**
     * @brief Get configuration file path
     * @return Configuration file path
     */
    QString configFilePath() const { return m_configFilePath; }

    /**
     * @brief Set configuration file path
     * @param filePath Configuration file path
     */
    void setConfigFilePath(const QString& filePath);

    /**
     * @brief Import configuration from JSON file
     * @param filePath JSON file path
     * @return true if successful
     */
    bool importFromJson(const QString& filePath);

    /**
     * @brief Export configuration to JSON file
     * @param filePath JSON file path
     * @return true if successful
     */
    bool exportToJson(const QString& filePath) const;

    /**
     * @brief Get configuration as JSON object
     * @return JSON representation
     */
    QJsonObject toJson() const;

    /**
     * @brief Set configuration from JSON object
     * @param json JSON object
     */
    void fromJson(const QJsonObject& json);

    /**
     * @brief Reset to default configuration
     */
    void resetToDefaults();

    /**
     * @brief Get default configuration
     * @return Default configuration values
     */
    static QJsonObject defaultConfiguration();

signals:
    /**
     * @brief Emitted when configuration changes
     * @param key Changed key
     * @param value New value
     */
    void configurationChanged(const QString& key, const QVariant& value);

    /**
     * @brief Emitted when configuration is loaded
     */
    void configurationLoaded();

    /**
     * @brief Emitted when configuration is saved
     */
    void configurationSaved();

private slots:
    /**
     * @brief Handle settings changes
     * @param key Changed key
     */
    void onSettingsChanged(const QString& key);

private:
    /**
     * @brief Initialize default settings
     */
    void initializeDefaults();

    /**
     * @brief Create configuration directory
     * @return true if successful
     */
    bool createConfigDirectory();

    /**
     * @brief Validate configuration
     * @return true if valid
     */
    bool validateConfiguration() const;

    /**
     * @brief Migrate old configuration
     * @return true if successful
     */
    bool migrateConfiguration();

    /**
     * @brief Get full key path
     * @param key Relative key
     * @return Full key path
     */
    QString fullKey(const QString& key) const;

private:
    // Configuration storage
    std::unique_ptr<QSettings> m_settings;
    QString m_configDir;
    QString m_configFilePath;
    
    // Group management
    QStringList m_groupStack;
    
    // State
    bool m_loaded;
    bool m_modified;
    
    // Default values
    QJsonObject m_defaults;
};
