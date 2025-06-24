#pragma once

#include <QMainWindow>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QDockWidget>
#include <memory>

class ViewerWidget;
class LayerWidget;
class ToolBar;
class QLabel;

/**
 * @brief Main window class for the GUI framework
 * 
 * This class provides the main user interface, including the central viewer,
 * layer panel, toolbar, and menu system.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit MainWindow(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MainWindow();

    /**
     * @brief Get the central viewer widget
     * @return Pointer to viewer widget
     */
    ViewerWidget* viewerWidget() const { return m_viewerWidget.get(); }

    /**
     * @brief Get the layer widget
     * @return Pointer to layer widget
     */
    LayerWidget* layerWidget() const { return m_layerWidget.get(); }

    /**
     * @brief Get the toolbar
     * @return Pointer to toolbar
     */
    ToolBar* toolBar() const { return m_toolBar.get(); }

public slots:
    /**
     * @brief Show about dialog
     */
    void showAbout();

    /**
     * @brief Open file dialog
     */
    void openFile();

    /**
     * @brief Save current work
     */
    void save();

    /**
     * @brief Save as dialog
     */
    void saveAs();

    /**
     * @brief Exit application
     */
    void exit();

    /**
     * @brief Show preferences dialog
     */
    void showPreferences();

    /**
     * @brief Show plugin manager dialog
     */
    void showPluginManager();

    /**
     * @brief Toggle layer panel visibility
     */
    void toggleLayerPanel();

    /**
     * @brief Toggle toolbar visibility
     */
    void toggleToolBar();

    /**
     * @brief Update status bar message
     * @param message Status message
     */
    void updateStatusMessage(const QString& message);

protected:
    /**
     * @brief Handle close event
     * @param event Close event
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief Handle resize event
     * @param event Resize event
     */
    void resizeEvent(QResizeEvent* event) override;

private slots:
    /**
     * @brief Handle layer selection change
     */
    void onLayerSelectionChanged();

    /**
     * @brief Handle viewer state change
     */
    void onViewerStateChanged();

private:
    /**
     * @brief Setup the user interface
     */
    void setupUI();

    /**
     * @brief Create menu bar
     */
    void createMenuBar();

    /**
     * @brief Create toolbar
     */
    void createToolBar();

    /**
     * @brief Create status bar
     */
    void createStatusBar();

    /**
     * @brief Create dock widgets
     */
    void createDockWidgets();

    /**
     * @brief Setup central widget
     */
    void setupCentralWidget();

    /**
     * @brief Connect signals and slots
     */
    void connectSignals();

    /**
     * @brief Load window settings
     */
    void loadSettings();

    /**
     * @brief Save window settings
     */
    void saveSettings();

private:
    // Central widgets
    std::unique_ptr<ViewerWidget> m_viewerWidget;
    std::unique_ptr<QSplitter> m_centralSplitter;

    // Dock widgets
    std::unique_ptr<LayerWidget> m_layerWidget;
    QDockWidget* m_layerDock;

    // Toolbar
    std::unique_ptr<ToolBar> m_toolBar;

    // Menu actions
    QAction* m_openAction;
    QAction* m_saveAction;
    QAction* m_saveAsAction;
    QAction* m_exitAction;
    QAction* m_aboutAction;
    QAction* m_preferencesAction;
    QAction* m_pluginManagerAction;
    QAction* m_toggleLayerPanelAction;
    QAction* m_toggleToolBarAction;

    // Status bar
    QLabel* m_statusLabel;
    QLabel* m_coordinatesLabel;
    QLabel* m_zoomLabel;

    // State
    bool m_isModified;
    QString m_currentFile;
};
