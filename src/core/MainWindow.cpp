#include "MainWindow.h"
#include "../ui/ViewerWidget.h"
#include "../ui/LayerWidget.h"
#include "../ui/ToolBar.h"
#include "Application.h"

#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSettings>
#include <QDebug>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_layerDock(nullptr)
    , m_isModified(false)
{
    setWindowTitle("T-GUI Framework");
    setMinimumSize(800, 600);
    resize(1200, 800);

    try {
        setupUI();
        connectSignals();
        loadSettings();

        // Update status
        updateStatusMessage("Ready");
    } catch (const std::exception& e) {
        qCritical() << "Exception in MainWindow constructor:" << e.what();
    } catch (...) {
        qCritical() << "Unknown exception in MainWindow constructor";
    }
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About T-GUI Framework",
        "<h2>T-GUI Framework</h2>"
        "<p>Version 1.0.0</p>"
        "<p>A napari-like GUI framework built with C++ and Qt.</p>"
        "<p>Features:</p>"
        "<ul>"
        "<li>Layer-based visualization</li>"
        "<li>Plugin system</li>"
        "<li>Extensible architecture</li>"
        "</ul>");
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Open File", QString(),
        "All Files (*.*);;Images (*.png *.jpg *.jpeg *.bmp *.tiff)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement file loading through layer manager
        m_currentFile = fileName;
        updateStatusMessage(QString("Opened: %1").arg(fileName));
        
        // Update window title
        setWindowTitle(QString("T-GUI Framework - %1").arg(QFileInfo(fileName).fileName()));
    }
}

void MainWindow::save()
{
    if (m_currentFile.isEmpty()) {
        saveAs();
    } else {
        // TODO: Implement saving
        updateStatusMessage(QString("Saved: %1").arg(m_currentFile));
        m_isModified = false;
    }
}

void MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        "Save File", QString(),
        "All Files (*.*)");
    
    if (!fileName.isEmpty()) {
        m_currentFile = fileName;
        save();
    }
}

void MainWindow::exit()
{
    close();
}

void MainWindow::showPreferences()
{
    // TODO: Implement preferences dialog
    QMessageBox::information(this, "Preferences", "Preferences dialog not yet implemented.");
}

void MainWindow::showPluginManager()
{
    // TODO: Implement plugin manager dialog
    QMessageBox::information(this, "Plugin Manager", "Plugin manager dialog not yet implemented.");
}

void MainWindow::toggleLayerPanel()
{
    if (m_layerDock) {
        m_layerDock->setVisible(!m_layerDock->isVisible());
    }
}

void MainWindow::toggleToolBar()
{
    if (m_toolBar) {
        m_toolBar->setVisible(!m_toolBar->isVisible());
    }
}

void MainWindow::updateStatusMessage(const QString& message)
{
    if (m_statusLabel) {
        m_statusLabel->setText(message);
    }
    statusBar()->showMessage(message, 2000);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (m_isModified) {
        QMessageBox::StandardButton ret = QMessageBox::warning(this,
            "T-GUI Framework",
            "The document has been modified.\n"
            "Do you want to save your changes?",
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (ret == QMessageBox::Save) {
            save();
            event->accept();
        } else if (ret == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
    }
    
    saveSettings();
    event->accept();
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // Handle any resize-specific logic here
}

void MainWindow::onLayerSelectionChanged()
{
    // TODO: Handle layer selection changes
    updateStatusMessage("Layer selection changed");
}

void MainWindow::onViewerStateChanged()
{
    // TODO: Handle viewer state changes
    updateStatusMessage("Viewer state changed");
}

void MainWindow::setupUI()
{
    qDebug() << "Setting up UI...";

    createMenuBar();
    qDebug() << "Menu bar created";

    // createToolBar();
    qDebug() << "Toolbar creation skipped";

    createStatusBar();
    qDebug() << "Status bar created";

    setupCentralWidget();
    qDebug() << "Central widget setup";

    // createDockWidgets();
    qDebug() << "Dock widgets creation skipped";

    qDebug() << "UI setup completed";
}

void MainWindow::createMenuBar()
{
    // File menu
    QMenu* fileMenu = menuBar()->addMenu("&File");
    
    m_openAction = new QAction("&Open...", this);
    m_openAction->setShortcut(QKeySequence::Open);
    m_openAction->setStatusTip("Open a file");
    fileMenu->addAction(m_openAction);
    
    fileMenu->addSeparator();
    
    m_saveAction = new QAction("&Save", this);
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAction->setStatusTip("Save the current file");
    fileMenu->addAction(m_saveAction);
    
    m_saveAsAction = new QAction("Save &As...", this);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs);
    m_saveAsAction->setStatusTip("Save the file with a new name");
    fileMenu->addAction(m_saveAsAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    m_exitAction->setStatusTip("Exit the application");
    fileMenu->addAction(m_exitAction);
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu("&View");
    
    m_toggleLayerPanelAction = new QAction("&Layer Panel", this);
    m_toggleLayerPanelAction->setCheckable(true);
    m_toggleLayerPanelAction->setChecked(true);
    viewMenu->addAction(m_toggleLayerPanelAction);
    
    m_toggleToolBarAction = new QAction("&Toolbar", this);
    m_toggleToolBarAction->setCheckable(true);
    m_toggleToolBarAction->setChecked(true);
    viewMenu->addAction(m_toggleToolBarAction);
    
    // Tools menu
    QMenu* toolsMenu = menuBar()->addMenu("&Tools");
    
    m_preferencesAction = new QAction("&Preferences...", this);
    m_preferencesAction->setStatusTip("Open preferences dialog");
    toolsMenu->addAction(m_preferencesAction);
    
    m_pluginManagerAction = new QAction("&Plugin Manager...", this);
    m_pluginManagerAction->setStatusTip("Open plugin manager");
    toolsMenu->addAction(m_pluginManagerAction);
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu("&Help");
    
    m_aboutAction = new QAction("&About", this);
    m_aboutAction->setStatusTip("Show information about the application");
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::createToolBar()
{
    m_toolBar = std::make_unique<ToolBar>(this);
    addToolBar(m_toolBar.get());
}

void MainWindow::createStatusBar()
{
    m_statusLabel = new QLabel("Ready");
    statusBar()->addWidget(m_statusLabel);
    
    m_coordinatesLabel = new QLabel("(0, 0)");
    statusBar()->addPermanentWidget(m_coordinatesLabel);
    
    m_zoomLabel = new QLabel("100%");
    statusBar()->addPermanentWidget(m_zoomLabel);
}

void MainWindow::setupCentralWidget()
{
    qDebug() << "Creating central widget...";

    // Create a simple widget instead of ViewerWidget for now
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: #404040;");
    setCentralWidget(centralWidget);

    qDebug() << "Central widget created successfully";
}

void MainWindow::createDockWidgets()
{
    // Layer panel dock
    m_layerDock = new QDockWidget("Layers", this);
    m_layerWidget = std::make_unique<LayerWidget>(this);
    m_layerDock->setWidget(m_layerWidget.get());
    m_layerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_layerDock);
}

void MainWindow::connectSignals()
{
    // Menu actions
    connect(m_openAction, &QAction::triggered, this, &MainWindow::openFile);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::save);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::saveAs);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::exit);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    connect(m_preferencesAction, &QAction::triggered, this, &MainWindow::showPreferences);
    connect(m_pluginManagerAction, &QAction::triggered, this, &MainWindow::showPluginManager);
    connect(m_toggleLayerPanelAction, &QAction::triggered, this, &MainWindow::toggleLayerPanel);
    connect(m_toggleToolBarAction, &QAction::triggered, this, &MainWindow::toggleToolBar);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
}
