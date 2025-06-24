#include "Logger.h"
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include <QMutexLocker>
#include <iostream>

Logger* Logger::s_instance = nullptr;

Logger::Logger(QObject* parent)
    : QObject(parent)
    , m_logLevel(LogLevel::Info)
    , m_consoleOutput(true)
    , m_fileOutput(false)
    , m_maxHistorySize(1000)
{
    s_instance = this;
    
    // Set default log file name
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_logFileName = QDir(dataDir).filePath("application.log");
}

Logger::~Logger()
{
    flush();
    s_instance = nullptr;
}

Logger* Logger::instance()
{
    return s_instance;
}

void Logger::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

void Logger::setConsoleOutput(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_consoleOutput = enabled;
}

void Logger::setFileOutput(bool enabled, const QString& fileName)
{
    QMutexLocker locker(&m_mutex);
    
    if (!fileName.isEmpty()) {
        m_logFileName = fileName;
    }
    
    if (enabled && !m_fileOutput) {
        initializeLogFile();
    } else if (!enabled && m_fileOutput) {
        m_fileStream.reset();
        m_logFile.reset();
    }
    
    m_fileOutput = enabled;
}

void Logger::addCategoryFilter(const QString& category)
{
    QMutexLocker locker(&m_mutex);
    if (!m_categoryFilters.contains(category)) {
        m_categoryFilters.append(category);
    }
}

void Logger::removeCategoryFilter(const QString& category)
{
    QMutexLocker locker(&m_mutex);
    m_categoryFilters.removeAll(category);
}

void Logger::clearCategoryFilters()
{
    QMutexLocker locker(&m_mutex);
    m_categoryFilters.clear();
}

void Logger::log(LogLevel level, const QString& category, const QString& message,
                 const QString& file, int line, const QString& function)
{
    QMutexLocker locker(&m_mutex);
    
    // Check log level
    if (level < m_logLevel) {
        return;
    }
    
    // Check category filter
    if (!shouldLogCategory(category)) {
        return;
    }
    
    // Create log entry
    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = level;
    entry.category = category;
    entry.message = message;
    entry.file = file;
    entry.line = line;
    entry.function = function;
    
    // Add to history
    m_logHistory.append(entry);
    if (m_logHistory.size() > m_maxHistorySize) {
        m_logHistory.removeFirst();
    }
    
    // Format message
    QString formattedMessage = formatLogEntry(entry);
    
    // Output to console
    if (m_consoleOutput) {
        if (level >= LogLevel::Error) {
            std::cerr << formattedMessage.toStdString() << std::endl;
        } else {
            std::cout << formattedMessage.toStdString() << std::endl;
        }
    }
    
    // Output to file
    if (m_fileOutput && m_fileStream) {
        *m_fileStream << formattedMessage << Qt::endl;
        m_fileStream->flush();
    }
    
    // Emit signal
    emit logEntryAdded(entry);
}

void Logger::debug(const QString& message, const QString& category)
{
    log(LogLevel::Debug, category, message);
}

void Logger::info(const QString& message, const QString& category)
{
    log(LogLevel::Info, category, message);
}

void Logger::warning(const QString& message, const QString& category)
{
    log(LogLevel::Warning, category, message);
}

void Logger::error(const QString& message, const QString& category)
{
    log(LogLevel::Error, category, message);
}

void Logger::critical(const QString& message, const QString& category)
{
    log(LogLevel::Critical, category, message);
}

QList<LogEntry> Logger::recentEntries(int count) const
{
    QMutexLocker locker(&m_mutex);
    
    if (count >= m_logHistory.size()) {
        return m_logHistory;
    }
    
    return m_logHistory.mid(m_logHistory.size() - count);
}

void Logger::clearHistory()
{
    QMutexLocker locker(&m_mutex);
    m_logHistory.clear();
}

void Logger::flush()
{
    QMutexLocker locker(&m_mutex);
    if (m_fileStream) {
        m_fileStream->flush();
    }
}

QString Logger::formatLogEntry(const LogEntry& entry) const
{
    QString timestamp = entry.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString level = logLevelString(entry.level);
    
    QString formatted = QString("[%1] [%2] [%3] %4")
                       .arg(timestamp)
                       .arg(level)
                       .arg(entry.category)
                       .arg(entry.message);
    
    if (!entry.file.isEmpty() && entry.line > 0) {
        QString fileName = QFileInfo(entry.file).fileName();
        formatted += QString(" (%1:%2)").arg(fileName).arg(entry.line);
    }
    
    return formatted;
}

QString Logger::logLevelString(LogLevel level) const
{
    switch (level) {
    case LogLevel::Debug:    return "DEBUG";
    case LogLevel::Info:     return "INFO ";
    case LogLevel::Warning:  return "WARN ";
    case LogLevel::Error:    return "ERROR";
    case LogLevel::Critical: return "CRIT ";
    default:                 return "UNKN ";
    }
}

bool Logger::shouldLogCategory(const QString& category) const
{
    // If no filters are set, log everything
    if (m_categoryFilters.isEmpty()) {
        return true;
    }
    
    // Check if category is in filter list
    return m_categoryFilters.contains(category);
}

void Logger::initializeLogFile()
{
    // Create directory if it doesn't exist
    QFileInfo fileInfo(m_logFileName);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    // Open log file
    m_logFile = std::make_unique<QFile>(m_logFileName);
    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append)) {
        m_fileStream = std::make_unique<QTextStream>(m_logFile.get());
        m_fileStream->setCodec("UTF-8");
        
        // Write session start marker
        *m_fileStream << QString("\n=== Log session started at %1 ===\n")
                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                      << Qt::endl;
    } else {
        qWarning() << "Failed to open log file:" << m_logFileName;
        m_logFile.reset();
    }
}
