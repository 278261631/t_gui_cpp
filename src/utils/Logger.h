#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QTextStream>
#include <QFile>
#include <QMutex>
#include <memory>

/**
 * @brief Log level enumeration
 */
enum class LogLevel
{
    Debug = 0,      ///< Debug messages
    Info = 1,       ///< Informational messages
    Warning = 2,    ///< Warning messages
    Error = 3,      ///< Error messages
    Critical = 4    ///< Critical error messages
};

/**
 * @brief Log entry structure
 */
struct LogEntry
{
    QDateTime timestamp;    ///< Entry timestamp
    LogLevel level;         ///< Log level
    QString category;       ///< Log category
    QString message;        ///< Log message
    QString file;           ///< Source file
    int line;              ///< Source line number
    QString function;       ///< Source function
};

/**
 * @brief Logger class for application logging
 * 
 * Provides thread-safe logging with multiple output targets and filtering.
 */
class Logger : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit Logger(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~Logger();

    /**
     * @brief Get singleton instance
     * @return Logger instance
     */
    static Logger* instance();

    /**
     * @brief Set minimum log level
     * @param level Minimum level
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief Get current log level
     * @return Current log level
     */
    LogLevel logLevel() const { return m_logLevel; }

    /**
     * @brief Enable/disable console output
     * @param enabled Console output state
     */
    void setConsoleOutput(bool enabled);

    /**
     * @brief Check if console output is enabled
     * @return true if enabled
     */
    bool consoleOutput() const { return m_consoleOutput; }

    /**
     * @brief Enable/disable file output
     * @param enabled File output state
     * @param fileName Log file name (optional)
     */
    void setFileOutput(bool enabled, const QString& fileName = QString());

    /**
     * @brief Check if file output is enabled
     * @return true if enabled
     */
    bool fileOutput() const { return m_fileOutput; }

    /**
     * @brief Get log file name
     * @return Log file name
     */
    QString logFileName() const { return m_logFileName; }

    /**
     * @brief Add log category filter
     * @param category Category to filter
     */
    void addCategoryFilter(const QString& category);

    /**
     * @brief Remove log category filter
     * @param category Category to remove
     */
    void removeCategoryFilter(const QString& category);

    /**
     * @brief Clear all category filters
     */
    void clearCategoryFilters();

    /**
     * @brief Log a message
     * @param level Log level
     * @param category Log category
     * @param message Log message
     * @param file Source file
     * @param line Source line
     * @param function Source function
     */
    void log(LogLevel level, const QString& category, const QString& message,
             const QString& file = QString(), int line = 0, const QString& function = QString());

    /**
     * @brief Log debug message
     * @param message Debug message
     * @param category Log category
     */
    void debug(const QString& message, const QString& category = "General");

    /**
     * @brief Log info message
     * @param message Info message
     * @param category Log category
     */
    void info(const QString& message, const QString& category = "General");

    /**
     * @brief Log warning message
     * @param message Warning message
     * @param category Log category
     */
    void warning(const QString& message, const QString& category = "General");

    /**
     * @brief Log error message
     * @param message Error message
     * @param category Log category
     */
    void error(const QString& message, const QString& category = "General");

    /**
     * @brief Log critical message
     * @param message Critical message
     * @param category Log category
     */
    void critical(const QString& message, const QString& category = "General");

    /**
     * @brief Get recent log entries
     * @param count Maximum number of entries
     * @return List of log entries
     */
    QList<LogEntry> recentEntries(int count = 100) const;

    /**
     * @brief Clear log history
     */
    void clearHistory();

    /**
     * @brief Flush log outputs
     */
    void flush();

signals:
    /**
     * @brief Emitted when a new log entry is added
     * @param entry Log entry
     */
    void logEntryAdded(const LogEntry& entry);

private:
    /**
     * @brief Format log entry for output
     * @param entry Log entry
     * @return Formatted string
     */
    QString formatLogEntry(const LogEntry& entry) const;

    /**
     * @brief Get log level string
     * @param level Log level
     * @return Level string
     */
    QString logLevelString(LogLevel level) const;

    /**
     * @brief Check if category should be logged
     * @param category Category name
     * @return true if should be logged
     */
    bool shouldLogCategory(const QString& category) const;

    /**
     * @brief Initialize log file
     */
    void initializeLogFile();

private:
    static Logger* s_instance;

    // Configuration
    LogLevel m_logLevel;
    bool m_consoleOutput;
    bool m_fileOutput;
    QString m_logFileName;
    QStringList m_categoryFilters;

    // Output streams
    std::unique_ptr<QFile> m_logFile;
    std::unique_ptr<QTextStream> m_fileStream;

    // Log history
    QList<LogEntry> m_logHistory;
    int m_maxHistorySize;

    // Thread safety
    mutable QMutex m_mutex;
};

// Convenience macros for logging with file/line information
#define LOG_DEBUG(message, category) \
    Logger::instance()->log(LogLevel::Debug, category, message, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_INFO(message, category) \
    Logger::instance()->log(LogLevel::Info, category, message, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_WARNING(message, category) \
    Logger::instance()->log(LogLevel::Warning, category, message, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_ERROR(message, category) \
    Logger::instance()->log(LogLevel::Error, category, message, __FILE__, __LINE__, Q_FUNC_INFO)

#define LOG_CRITICAL(message, category) \
    Logger::instance()->log(LogLevel::Critical, category, message, __FILE__, __LINE__, Q_FUNC_INFO)
