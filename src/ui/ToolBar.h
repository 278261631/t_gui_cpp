#pragma once

#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>
#include <QLabel>

class ViewerWidget;

/**
 * @brief Main toolbar for the application
 * 
 * Provides quick access to common tools and view controls.
 */
class ToolBar : public QToolBar
{
    Q_OBJECT

public:
    /**
     * @brief Tool types enumeration
     */
    enum class ToolType
    {
        Select,     ///< Selection tool
        Pan,        ///< Pan tool
        Zoom,       ///< Zoom tool
        Measure,    ///< Measurement tool
        Annotate    ///< Annotation tool
    };

    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit ToolBar(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ToolBar();

    /**
     * @brief Get current tool
     * @return Current tool type
     */
    ToolType currentTool() const { return m_currentTool; }

    /**
     * @brief Set current tool
     * @param tool Tool type
     */
    void setCurrentTool(ToolType tool);

    /**
     * @brief Get viewer widget
     * @return Viewer widget pointer
     */
    ViewerWidget* viewerWidget() const { return m_viewerWidget; }

    /**
     * @brief Set viewer widget
     * @param viewer Viewer widget
     */
    void setViewerWidget(ViewerWidget* viewer);

public slots:
    /**
     * @brief Reset view
     */
    void resetView();

    /**
     * @brief Zoom in
     */
    void zoomIn();

    /**
     * @brief Zoom out
     */
    void zoomOut();

    /**
     * @brief Zoom to fit
     */
    void zoomToFit();

    /**
     * @brief Toggle 2D/3D view
     */
    void toggleViewMode();

    /**
     * @brief Show/hide grid
     */
    void toggleGrid();

    /**
     * @brief Show/hide axes
     */
    void toggleAxes();

    /**
     * @brief Take screenshot
     */
    void takeScreenshot();

signals:
    /**
     * @brief Emitted when tool changes
     * @param tool New tool type
     */
    void toolChanged(ToolType tool);

    /**
     * @brief Emitted when view mode is toggled
     */
    void viewModeToggled();

    /**
     * @brief Emitted when grid visibility is toggled
     * @param visible Grid visibility
     */
    void gridToggled(bool visible);

    /**
     * @brief Emitted when axes visibility is toggled
     * @param visible Axes visibility
     */
    void axesToggled(bool visible);

private slots:
    /**
     * @brief Handle tool selection
     * @param action Triggered action
     */
    void onToolTriggered(QAction* action);

    /**
     * @brief Handle zoom level change
     * @param value Zoom value
     */
    void onZoomLevelChanged(int value);

    /**
     * @brief Handle view mode combo change
     * @param index Selected index
     */
    void onViewModeChanged(int index);

private:
    /**
     * @brief Setup toolbar
     */
    void setupToolbar();

    /**
     * @brief Create tool actions
     */
    void createToolActions();

    /**
     * @brief Create view actions
     */
    void createViewActions();

    /**
     * @brief Create zoom controls
     */
    void createZoomControls();

    /**
     * @brief Update tool states
     */
    void updateToolStates();

    /**
     * @brief Update zoom display
     * @param zoom Zoom level
     */
    void updateZoomDisplay(float zoom);

private:
    // Tool actions
    QActionGroup* m_toolGroup;
    QAction* m_selectAction;
    QAction* m_panAction;
    QAction* m_zoomAction;
    QAction* m_measureAction;
    QAction* m_annotateAction;

    // View actions
    QAction* m_resetViewAction;
    QAction* m_zoomInAction;
    QAction* m_zoomOutAction;
    QAction* m_zoomToFitAction;
    QAction* m_toggleViewModeAction;
    QAction* m_toggleGridAction;
    QAction* m_toggleAxesAction;
    QAction* m_screenshotAction;

    // Controls
    QComboBox* m_viewModeCombo;
    QSlider* m_zoomSlider;
    QLabel* m_zoomLabel;

    // State
    ToolType m_currentTool;
    ViewerWidget* m_viewerWidget;
};
