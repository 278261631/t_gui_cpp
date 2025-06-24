#include "core/Application.h"
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>

/**
 * @brief Setup application environment
 */
void setupEnvironment()
{
    // Set application properties
    QCoreApplication::setApplicationName("T-GUI Framework");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("T-GUI");
    QCoreApplication::setOrganizationDomain("t-gui.org");

    // Enable high DPI support
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    // Set default style
    QApplication::setStyle("Fusion");
}

/**
 * @brief Show error message and exit
 * @param message Error message
 * @param exitCode Exit code
 */
void showErrorAndExit(const QString& message, int exitCode = 1)
{
    qCritical() << message;
    
    // Try to show message box if possible
    if (QApplication::instance()) {
        QMessageBox::critical(nullptr, "T-GUI Framework Error", message);
    }
    
    std::exit(exitCode);
}

/**
 * @brief Check system requirements
 * @return true if requirements are met
 */
bool checkSystemRequirements()
{
    // Check Qt version
    QString qtVersion = qVersion();
    qDebug() << "Qt version:" << qtVersion;
    
    // Check if we can create application data directory
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataDir);
    if (!dir.exists() && !dir.mkpath(".")) {
        qWarning() << "Cannot create application data directory:" << dataDir;
        return false;
    }
    
    return true;
}

/**
 * @brief Print application information
 */
void printApplicationInfo()
{
    qDebug() << "=== T-GUI Framework ===";
    qDebug() << "Version:" << QCoreApplication::applicationVersion();
    qDebug() << "Qt Version:" << qVersion();
    qDebug() << "Build Date:" << __DATE__ << __TIME__;
    qDebug() << "Data Directory:" << QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    qDebug() << "========================";
}

/**
 * @brief Main application entry point
 * @param argc Command line argument count
 * @param argv Command line arguments
 * @return Exit code
 */
int main(int argc, char* argv[])
{
    // Setup environment before creating QApplication
    setupEnvironment();
    
    // Create application instance
    Application app(argc, argv);
    
    // Print application information
    printApplicationInfo();
    
    // Check system requirements
    if (!checkSystemRequirements()) {
        showErrorAndExit("System requirements not met. Please check your Qt installation and permissions.");
        return 1;
    }
    
    // Initialize application
    if (!app.initialize()) {
        showErrorAndExit("Failed to initialize application. Check logs for details.");
        return 1;
    }
    
    // Show main window
    app.showMainWindow();
    
    qDebug() << "Application started successfully";
    
    // Run event loop
    int result = app.exec();
    
    qDebug() << "Application exiting with code:" << result;
    return result;
}
