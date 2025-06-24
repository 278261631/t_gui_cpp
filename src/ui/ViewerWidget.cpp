#include "ViewerWidget.h"
#include "../core/LayerManager.h"

#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QDebug>

ViewerWidget::ViewerWidget(QWidget* parent)
    : QOpenGLWidget(parent)
    , m_viewMode(ViewMode::View2D)
    , m_zoomLevel(1.0f)
    , m_viewCenter(0.0f, 0.0f, 0.0f)
    , m_rotation(0.0f, 0.0f, 0.0f)
    , m_isPanning(false)
    , m_isRotating(false)
    , m_activeButton(Qt::NoButton)
    , m_layerManager(nullptr)
    , m_backgroundColor(64, 64, 64)
    , m_showGrid(true)
    , m_showAxes(true)
    , m_glInitialized(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
}

ViewerWidget::~ViewerWidget()
{
}

void ViewerWidget::setViewMode(ViewMode mode)
{
    if (m_viewMode != mode) {
        m_viewMode = mode;
        updateProjectionMatrix();
        update();
        emit viewModeChanged(mode);
    }
}

void ViewerWidget::setZoomLevel(float zoom)
{
    zoom = qMax(0.1f, qMin(100.0f, zoom));
    if (qAbs(m_zoomLevel - zoom) > 0.001f) {
        m_zoomLevel = zoom;
        updateProjectionMatrix();
        update();
        emit zoomChanged(zoom);
    }
}

void ViewerWidget::setViewCenter(const QVector3D& center)
{
    if (m_viewCenter != center) {
        m_viewCenter = center;
        updateViewMatrix();
        update();
        emit viewChanged();
    }
}

void ViewerWidget::setLayerManager(LayerManager* manager)
{
    if (m_layerManager) {
        disconnect(m_layerManager, nullptr, this, nullptr);
    }
    
    m_layerManager = manager;
    
    if (m_layerManager) {
        connect(m_layerManager, &LayerManager::dataChanged, this, &ViewerWidget::onLayerChanged);
        connect(m_layerManager, &LayerManager::rowsInserted, this, &ViewerWidget::onLayerChanged);
        connect(m_layerManager, &LayerManager::rowsRemoved, this, &ViewerWidget::onLayerChanged);
    }
    
    update();
}

QVector3D ViewerWidget::screenToWorld(const QPoint& screenPos) const
{
    // Simple 2D screen to world conversion
    // This is a basic implementation - would need proper matrix math for 3D
    float x = (screenPos.x() - width() / 2.0f) / m_zoomLevel + m_viewCenter.x();
    float y = (height() / 2.0f - screenPos.y()) / m_zoomLevel + m_viewCenter.y();
    return QVector3D(x, y, 0.0f);
}

QPoint ViewerWidget::worldToScreen(const QVector3D& worldPos) const
{
    // Simple world to screen conversion
    int x = (worldPos.x() - m_viewCenter.x()) * m_zoomLevel + width() / 2.0f;
    int y = height() / 2.0f - (worldPos.y() - m_viewCenter.y()) * m_zoomLevel;
    return QPoint(x, y);
}

void ViewerWidget::resetView()
{
    m_zoomLevel = 1.0f;
    m_viewCenter = QVector3D(0.0f, 0.0f, 0.0f);
    m_rotation = QVector3D(0.0f, 0.0f, 0.0f);
    updateProjectionMatrix();
    updateViewMatrix();
    update();
    emit viewChanged();
    emit zoomChanged(m_zoomLevel);
}

void ViewerWidget::zoomIn()
{
    setZoomLevel(m_zoomLevel * 1.2f);
}

void ViewerWidget::zoomOut()
{
    setZoomLevel(m_zoomLevel / 1.2f);
}

void ViewerWidget::zoomToFit()
{
    if (!m_layerManager || m_layerManager->layerCount() == 0) {
        resetView();
        return;
    }

    QVector<float> bounds = calculateViewBounds();
    if (bounds.size() >= 4) {
        float width = bounds[2] - bounds[0];
        float height = bounds[3] - bounds[1];
        float centerX = (bounds[0] + bounds[2]) / 2.0f;
        float centerY = (bounds[1] + bounds[3]) / 2.0f;
        
        m_viewCenter = QVector3D(centerX, centerY, 0.0f);
        
        float scaleX = this->width() / width;
        float scaleY = this->height() / height;
        m_zoomLevel = qMin(scaleX, scaleY) * 0.9f; // 90% to add some margin
        
        updateProjectionMatrix();
        updateViewMatrix();
        update();
        emit viewChanged();
        emit zoomChanged(m_zoomLevel);
    }
}

void ViewerWidget::toggleViewMode()
{
    setViewMode(m_viewMode == ViewMode::View2D ? ViewMode::View3D : ViewMode::View2D);
}

void ViewerWidget::updateDisplay()
{
    update();
}

void ViewerWidget::initializeGL()
{
    initializeOpenGLFunctions();
    setupGL();
    m_glInitialized = true;
}

void ViewerWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    updateProjectionMatrix();
}

void ViewerWidget::paintGL()
{
    if (!m_glInitialized) {
        return;
    }

    // Clear the screen
    glClearColor(m_backgroundColor.redF(), m_backgroundColor.greenF(), 
                 m_backgroundColor.blueF(), 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render background elements
    renderBackground();
    
    if (m_showGrid) {
        renderGrid();
    }

    // Render layers
    renderLayers();
}

void ViewerWidget::mousePressEvent(QMouseEvent* event)
{
    m_lastMousePos = event->pos();
    m_activeButton = event->button();

    if (event->button() == Qt::LeftButton) {
        m_isPanning = true;
    } else if (event->button() == Qt::RightButton && m_viewMode == ViewMode::View3D) {
        m_isRotating = true;
    }

    QOpenGLWidget::mousePressEvent(event);
}

void ViewerWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPoint delta = event->pos() - m_lastMousePos;
    
    if (m_isPanning && (event->buttons() & Qt::LeftButton)) {
        handlePan(delta);
    } else if (m_isRotating && (event->buttons() & Qt::RightButton)) {
        handleRotation(delta);
    }

    // Emit mouse position
    QVector3D worldPos = screenToWorld(event->pos());
    emit mousePositionChanged(worldPos, event->pos());

    m_lastMousePos = event->pos();
    QOpenGLWidget::mouseMoveEvent(event);
}

void ViewerWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_isPanning = false;
    } else if (event->button() == Qt::RightButton) {
        m_isRotating = false;
    }

    m_activeButton = Qt::NoButton;
    QOpenGLWidget::mouseReleaseEvent(event);
}

void ViewerWidget::wheelEvent(QWheelEvent* event)
{
    float delta = event->angleDelta().y() / 120.0f;
    handleZoom(delta * 0.1f, event->position().toPoint());
    QOpenGLWidget::wheelEvent(event);
}

void ViewerWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
    case Qt::Key_R:
        resetView();
        break;
    case Qt::Key_F:
        zoomToFit();
        break;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
        toggleViewMode();
        break;
    default:
        QOpenGLWidget::keyPressEvent(event);
        break;
    }
}

void ViewerWidget::onLayerChanged()
{
    update();
}

void ViewerWidget::setupGL()
{
    // Enable depth testing for 3D
    glEnable(GL_DEPTH_TEST);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set up initial matrices
    updateProjectionMatrix();
    updateViewMatrix();
}

void ViewerWidget::updateProjectionMatrix()
{
    m_projectionMatrix.setToIdentity();
    
    float aspect = float(width()) / float(height());
    
    if (m_viewMode == ViewMode::View2D) {
        // Orthographic projection for 2D
        float halfWidth = width() / (2.0f * m_zoomLevel);
        float halfHeight = height() / (2.0f * m_zoomLevel);
        m_projectionMatrix.ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1000.0f, 1000.0f);
    } else {
        // Perspective projection for 3D
        m_projectionMatrix.perspective(45.0f, aspect, 0.1f, 1000.0f);
    }
}

void ViewerWidget::updateViewMatrix()
{
    m_viewMatrix.setToIdentity();
    
    if (m_viewMode == ViewMode::View2D) {
        m_viewMatrix.translate(-m_viewCenter.x(), -m_viewCenter.y(), 0.0f);
    } else {
        m_viewMatrix.translate(0.0f, 0.0f, -10.0f);
        m_viewMatrix.rotate(m_rotation.x(), 1.0f, 0.0f, 0.0f);
        m_viewMatrix.rotate(m_rotation.y(), 0.0f, 1.0f, 0.0f);
        m_viewMatrix.rotate(m_rotation.z(), 0.0f, 0.0f, 1.0f);
        m_viewMatrix.translate(-m_viewCenter);
    }
}

void ViewerWidget::renderLayers()
{
    if (!m_layerManager) {
        return;
    }

    // Render each layer
    for (int i = 0; i < m_layerManager->layerCount(); ++i) {
        Layer* layer = m_layerManager->layer(i);
        if (layer && layer->isVisible()) {
            // TODO: Implement actual layer rendering
            // layer->render(renderContext);
        }
    }
}

void ViewerWidget::renderBackground()
{
    // Background is already cleared in paintGL
}

void ViewerWidget::renderGrid()
{
    // TODO: Implement grid rendering
    // This would draw a grid overlay to help with navigation
}

QVector<float> ViewerWidget::calculateViewBounds() const
{
    QVector<float> bounds;
    
    if (!m_layerManager || m_layerManager->layerCount() == 0) {
        return bounds;
    }

    bool first = true;
    float minX = 0, minY = 0, maxX = 0, maxY = 0;

    for (int i = 0; i < m_layerManager->layerCount(); ++i) {
        Layer* layer = m_layerManager->layer(i);
        if (layer && layer->isVisible()) {
            QVector<float> layerBounds = layer->bounds();
            if (layerBounds.size() >= 4) {
                if (first) {
                    minX = layerBounds[0];
                    minY = layerBounds[1];
                    maxX = layerBounds[2];
                    maxY = layerBounds[3];
                    first = false;
                } else {
                    minX = qMin(minX, layerBounds[0]);
                    minY = qMin(minY, layerBounds[1]);
                    maxX = qMax(maxX, layerBounds[2]);
                    maxY = qMax(maxY, layerBounds[3]);
                }
            }
        }
    }

    if (!first) {
        bounds << minX << minY << maxX << maxY;
    }

    return bounds;
}

void ViewerWidget::handlePan(const QPoint& delta)
{
    QVector3D worldDelta = QVector3D(delta.x() / m_zoomLevel, -delta.y() / m_zoomLevel, 0.0f);
    setViewCenter(m_viewCenter - worldDelta);
}

void ViewerWidget::handleZoom(float delta, const QPoint& center)
{
    QVector3D worldCenter = screenToWorld(center);
    float newZoom = m_zoomLevel * (1.0f + delta);
    
    setZoomLevel(newZoom);
    
    // Adjust view center to zoom towards the mouse position
    QVector3D newWorldCenter = screenToWorld(center);
    QVector3D offset = worldCenter - newWorldCenter;
    setViewCenter(m_viewCenter + offset);
}

void ViewerWidget::handleRotation(const QPoint& delta)
{
    if (m_viewMode == ViewMode::View3D) {
        m_rotation.setX(m_rotation.x() + delta.y() * 0.5f);
        m_rotation.setY(m_rotation.y() + delta.x() * 0.5f);
        updateViewMatrix();
        update();
        emit viewChanged();
    }
}
