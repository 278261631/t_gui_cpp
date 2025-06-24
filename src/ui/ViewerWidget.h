#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

class Layer;
class LayerManager;

/**
 * @brief Main viewer widget for displaying layers
 * 
 * This widget provides 2D/3D visualization capabilities using OpenGL.
 * It supports pan, zoom, and rotation interactions.
 */
class ViewerWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    /**
     * @brief View mode enumeration
     */
    enum class ViewMode
    {
        View2D,     ///< 2D view mode
        View3D      ///< 3D view mode
    };

    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit ViewerWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ViewerWidget();

    /**
     * @brief Get current view mode
     * @return View mode
     */
    ViewMode viewMode() const { return m_viewMode; }

    /**
     * @brief Set view mode
     * @param mode View mode
     */
    void setViewMode(ViewMode mode);

    /**
     * @brief Get current zoom level
     * @return Zoom level
     */
    float zoomLevel() const { return m_zoomLevel; }

    /**
     * @brief Set zoom level
     * @param zoom Zoom level
     */
    void setZoomLevel(float zoom);

    /**
     * @brief Get view center
     * @return View center coordinates
     */
    QVector3D viewCenter() const { return m_viewCenter; }

    /**
     * @brief Set view center
     * @param center View center coordinates
     */
    void setViewCenter(const QVector3D& center);

    /**
     * @brief Get layer manager
     * @return Layer manager pointer
     */
    LayerManager* layerManager() const { return m_layerManager; }

    /**
     * @brief Set layer manager
     * @param manager Layer manager
     */
    void setLayerManager(LayerManager* manager);

    /**
     * @brief Convert screen coordinates to world coordinates
     * @param screenPos Screen position
     * @return World coordinates
     */
    QVector3D screenToWorld(const QPoint& screenPos) const;

    /**
     * @brief Convert world coordinates to screen coordinates
     * @param worldPos World position
     * @return Screen coordinates
     */
    QPoint worldToScreen(const QVector3D& worldPos) const;

public slots:
    /**
     * @brief Reset view to fit all layers
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
     * @brief Toggle view mode
     */
    void toggleViewMode();

    /**
     * @brief Update display
     */
    void updateDisplay();

signals:
    /**
     * @brief Emitted when view changes
     */
    void viewChanged();

    /**
     * @brief Emitted when zoom changes
     * @param zoom New zoom level
     */
    void zoomChanged(float zoom);

    /**
     * @brief Emitted when mouse position changes
     * @param worldPos World coordinates
     * @param screenPos Screen coordinates
     */
    void mousePositionChanged(const QVector3D& worldPos, const QPoint& screenPos);

    /**
     * @brief Emitted when view mode changes
     * @param mode New view mode
     */
    void viewModeChanged(ViewMode mode);

protected:
    // QOpenGLWidget interface
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // Event handlers
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /**
     * @brief Handle layer changes
     */
    void onLayerChanged();

private:
    /**
     * @brief Setup OpenGL state
     */
    void setupGL();

    /**
     * @brief Update projection matrix
     */
    void updateProjectionMatrix();

    /**
     * @brief Update view matrix
     */
    void updateViewMatrix();

    /**
     * @brief Render all layers
     */
    void renderLayers();

    /**
     * @brief Render background
     */
    void renderBackground();

    /**
     * @brief Render grid
     */
    void renderGrid();

    /**
     * @brief Calculate view bounds
     * @return View bounds [xmin, ymin, xmax, ymax]
     */
    QVector<float> calculateViewBounds() const;

    /**
     * @brief Handle pan gesture
     * @param delta Pan delta
     */
    void handlePan(const QPoint& delta);

    /**
     * @brief Handle zoom gesture
     * @param delta Zoom delta
     * @param center Zoom center
     */
    void handleZoom(float delta, const QPoint& center);

    /**
     * @brief Handle rotation gesture
     * @param delta Rotation delta
     */
    void handleRotation(const QPoint& delta);

private:
    // View state
    ViewMode m_viewMode;
    float m_zoomLevel;
    QVector3D m_viewCenter;
    QVector3D m_rotation;
    
    // Matrices
    QMatrix4x4 m_projectionMatrix;
    QMatrix4x4 m_viewMatrix;
    QMatrix4x4 m_modelMatrix;
    
    // Interaction state
    bool m_isPanning;
    bool m_isRotating;
    QPoint m_lastMousePos;
    Qt::MouseButton m_activeButton;
    
    // Layer management
    LayerManager* m_layerManager;
    
    // Rendering settings
    QColor m_backgroundColor;
    bool m_showGrid;
    bool m_showAxes;
    
    // OpenGL state
    bool m_glInitialized;
};
