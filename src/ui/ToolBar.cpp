#include "ToolBar.h"
#include "ViewerWidget.h"
#include <QAction>
#include <QActionGroup>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QWidget>

ToolBar::ToolBar(QWidget* parent)
    : QToolBar(parent)
    , m_currentTool(ToolType::Select)
    , m_viewerWidget(nullptr)
{
    setObjectName("MainToolBar");
    setWindowTitle("Tools");
    setupToolbar();
}

ToolBar::~ToolBar()
{
}

void ToolBar::setCurrentTool(ToolType tool)
{
    if (m_currentTool != tool) {
        m_currentTool = tool;
        updateToolStates();
        emit toolChanged(tool);
    }
}

void ToolBar::setViewerWidget(ViewerWidget* viewer)
{
    if (m_viewerWidget) {
        disconnect(m_viewerWidget, nullptr, this, nullptr);
    }
    
    m_viewerWidget = viewer;
    
    if (m_viewerWidget) {
        connect(m_viewerWidget, &ViewerWidget::zoomChanged, this, &ToolBar::updateZoomDisplay);
        connect(m_viewerWidget, &ViewerWidget::viewModeChanged, this, [this](ViewerWidget::ViewMode mode) {
            m_viewModeCombo->setCurrentIndex(static_cast<int>(mode));
        });
    }
}

void ToolBar::resetView()
{
    if (m_viewerWidget) {
        m_viewerWidget->resetView();
    }
}

void ToolBar::zoomIn()
{
    if (m_viewerWidget) {
        m_viewerWidget->zoomIn();
    }
}

void ToolBar::zoomOut()
{
    if (m_viewerWidget) {
        m_viewerWidget->zoomOut();
    }
}

void ToolBar::zoomToFit()
{
    if (m_viewerWidget) {
        m_viewerWidget->zoomToFit();
    }
}

void ToolBar::toggleViewMode()
{
    if (m_viewerWidget) {
        m_viewerWidget->toggleViewMode();
    }
    emit viewModeToggled();
}

void ToolBar::toggleGrid()
{
    // TODO: Implement grid toggle
    emit gridToggled(true); // Placeholder
}

void ToolBar::toggleAxes()
{
    // TODO: Implement axes toggle
    emit axesToggled(true); // Placeholder
}

void ToolBar::takeScreenshot()
{
    // TODO: Implement screenshot functionality
    qDebug() << "Screenshot requested";
}

void ToolBar::onToolTriggered(QAction* action)
{
    if (action == m_selectAction) {
        setCurrentTool(ToolType::Select);
    } else if (action == m_panAction) {
        setCurrentTool(ToolType::Pan);
    } else if (action == m_zoomAction) {
        setCurrentTool(ToolType::Zoom);
    } else if (action == m_measureAction) {
        setCurrentTool(ToolType::Measure);
    } else if (action == m_annotateAction) {
        setCurrentTool(ToolType::Annotate);
    }
}

void ToolBar::onZoomLevelChanged(int value)
{
    if (m_viewerWidget) {
        float zoom = value / 100.0f;
        m_viewerWidget->setZoomLevel(zoom);
    }
}

void ToolBar::onViewModeChanged(int index)
{
    if (m_viewerWidget) {
        ViewerWidget::ViewMode mode = static_cast<ViewerWidget::ViewMode>(index);
        m_viewerWidget->setViewMode(mode);
    }
}

void ToolBar::setupToolbar()
{
    createToolActions();
    createViewActions();
    createZoomControls();
    
    // Add separators between sections
    addSeparator();
    
    // Add view mode combo
    addWidget(new QLabel("View:"));
    addWidget(m_viewModeCombo);
    
    addSeparator();
    
    // Add zoom controls
    addWidget(new QLabel("Zoom:"));
    addWidget(m_zoomSlider);
    addWidget(m_zoomLabel);
}

void ToolBar::createToolActions()
{
    m_toolGroup = new QActionGroup(this);
    
    m_selectAction = new QAction("Select", this);
    m_selectAction->setCheckable(true);
    m_selectAction->setChecked(true);
    m_selectAction->setToolTip("Selection tool");
    m_selectAction->setShortcut(QKeySequence("S"));
    m_toolGroup->addAction(m_selectAction);
    addAction(m_selectAction);
    
    m_panAction = new QAction("Pan", this);
    m_panAction->setCheckable(true);
    m_panAction->setToolTip("Pan tool");
    m_panAction->setShortcut(QKeySequence("P"));
    m_toolGroup->addAction(m_panAction);
    addAction(m_panAction);
    
    m_zoomAction = new QAction("Zoom", this);
    m_zoomAction->setCheckable(true);
    m_zoomAction->setToolTip("Zoom tool");
    m_zoomAction->setShortcut(QKeySequence("Z"));
    m_toolGroup->addAction(m_zoomAction);
    addAction(m_zoomAction);
    
    m_measureAction = new QAction("Measure", this);
    m_measureAction->setCheckable(true);
    m_measureAction->setToolTip("Measurement tool");
    m_measureAction->setShortcut(QKeySequence("M"));
    m_toolGroup->addAction(m_measureAction);
    addAction(m_measureAction);
    
    m_annotateAction = new QAction("Annotate", this);
    m_annotateAction->setCheckable(true);
    m_annotateAction->setToolTip("Annotation tool");
    m_annotateAction->setShortcut(QKeySequence("A"));
    m_toolGroup->addAction(m_annotateAction);
    addAction(m_annotateAction);
    
    connect(m_toolGroup, &QActionGroup::triggered, this, &ToolBar::onToolTriggered);
}

void ToolBar::createViewActions()
{
    m_resetViewAction = new QAction("Reset", this);
    m_resetViewAction->setToolTip("Reset view");
    m_resetViewAction->setShortcut(QKeySequence("R"));
    addAction(m_resetViewAction);
    connect(m_resetViewAction, &QAction::triggered, this, &ToolBar::resetView);
    
    m_zoomInAction = new QAction("Zoom In", this);
    m_zoomInAction->setToolTip("Zoom in");
    m_zoomInAction->setShortcut(QKeySequence::ZoomIn);
    addAction(m_zoomInAction);
    connect(m_zoomInAction, &QAction::triggered, this, &ToolBar::zoomIn);
    
    m_zoomOutAction = new QAction("Zoom Out", this);
    m_zoomOutAction->setToolTip("Zoom out");
    m_zoomOutAction->setShortcut(QKeySequence::ZoomOut);
    addAction(m_zoomOutAction);
    connect(m_zoomOutAction, &QAction::triggered, this, &ToolBar::zoomOut);
    
    m_zoomToFitAction = new QAction("Fit", this);
    m_zoomToFitAction->setToolTip("Zoom to fit");
    m_zoomToFitAction->setShortcut(QKeySequence("F"));
    addAction(m_zoomToFitAction);
    connect(m_zoomToFitAction, &QAction::triggered, this, &ToolBar::zoomToFit);
    
    m_toggleViewModeAction = new QAction("2D/3D", this);
    m_toggleViewModeAction->setToolTip("Toggle 2D/3D view");
    m_toggleViewModeAction->setShortcut(QKeySequence("Space"));
    addAction(m_toggleViewModeAction);
    connect(m_toggleViewModeAction, &QAction::triggered, this, &ToolBar::toggleViewMode);
    
    m_toggleGridAction = new QAction("Grid", this);
    m_toggleGridAction->setCheckable(true);
    m_toggleGridAction->setChecked(true);
    m_toggleGridAction->setToolTip("Toggle grid");
    m_toggleGridAction->setShortcut(QKeySequence("G"));
    addAction(m_toggleGridAction);
    connect(m_toggleGridAction, &QAction::triggered, this, &ToolBar::toggleGrid);
    
    m_toggleAxesAction = new QAction("Axes", this);
    m_toggleAxesAction->setCheckable(true);
    m_toggleAxesAction->setChecked(true);
    m_toggleAxesAction->setToolTip("Toggle axes");
    addAction(m_toggleAxesAction);
    connect(m_toggleAxesAction, &QAction::triggered, this, &ToolBar::toggleAxes);
    
    m_screenshotAction = new QAction("Screenshot", this);
    m_screenshotAction->setToolTip("Take screenshot");
    addAction(m_screenshotAction);
    connect(m_screenshotAction, &QAction::triggered, this, &ToolBar::takeScreenshot);
}

void ToolBar::createZoomControls()
{
    // View mode combo
    m_viewModeCombo = new QComboBox(this);
    m_viewModeCombo->addItem("2D");
    m_viewModeCombo->addItem("3D");
    m_viewModeCombo->setCurrentIndex(0);
    connect(m_viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ToolBar::onViewModeChanged);
    
    // Zoom slider
    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setRange(10, 1000); // 10% to 1000%
    m_zoomSlider->setValue(100); // 100%
    m_zoomSlider->setFixedWidth(100);
    m_zoomSlider->setToolTip("Zoom level");
    connect(m_zoomSlider, &QSlider::valueChanged, this, &ToolBar::onZoomLevelChanged);
    
    // Zoom label
    m_zoomLabel = new QLabel("100%", this);
    m_zoomLabel->setMinimumWidth(40);
    m_zoomLabel->setAlignment(Qt::AlignCenter);
}

void ToolBar::updateToolStates()
{
    switch (m_currentTool) {
    case ToolType::Select:
        m_selectAction->setChecked(true);
        break;
    case ToolType::Pan:
        m_panAction->setChecked(true);
        break;
    case ToolType::Zoom:
        m_zoomAction->setChecked(true);
        break;
    case ToolType::Measure:
        m_measureAction->setChecked(true);
        break;
    case ToolType::Annotate:
        m_annotateAction->setChecked(true);
        break;
    }
}

void ToolBar::updateZoomDisplay(float zoom)
{
    int zoomPercent = qRound(zoom * 100);
    m_zoomSlider->setValue(zoomPercent);
    m_zoomLabel->setText(QString("%1%").arg(zoomPercent));
}
