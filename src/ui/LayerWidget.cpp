#include "LayerWidget.h"
#include "../core/LayerManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTreeView>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QDebug>

LayerWidget::LayerWidget(QWidget* parent)
    : QWidget(parent)
    , m_layerManager(nullptr)
{
    setupUI();
    createContextMenu();
    updateButtonStates();
}

LayerWidget::~LayerWidget()
{
}

void LayerWidget::setLayerManager(LayerManager* manager)
{
    if (m_layerManager) {
        disconnect(m_layerManager, nullptr, this, nullptr);
    }
    
    m_layerManager = manager;
    m_treeView->setModel(manager);
    
    if (m_layerManager) {
        connect(m_layerManager, &LayerManager::selectionChanged, this, &LayerWidget::onSelectionChanged);
        connect(m_layerManager, &LayerManager::dataChanged, this, &LayerWidget::onLayerModelChanged);
        connect(m_layerManager, &LayerManager::rowsInserted, this, &LayerWidget::onLayerModelChanged);
        connect(m_layerManager, &LayerManager::rowsRemoved, this, &LayerWidget::onLayerModelChanged);
    }
    
    updateButtonStates();
}

QList<Layer*> LayerWidget::selectedLayers() const
{
    QList<Layer*> layers;
    if (!m_layerManager) {
        return layers;
    }
    
    QModelIndexList selection = m_treeView->selectionModel()->selectedRows();
    for (const QModelIndex& index : selection) {
        Layer* layer = layerFromIndex(index);
        if (layer) {
            layers.append(layer);
        }
    }
    
    return layers;
}

void LayerWidget::addLayer()
{
    // This is a simplified implementation
    // In a real application, this would show a dialog to select layer type
    if (m_layerManager) {
        // Create a simple test layer
        // Note: This won't compile without a concrete Layer implementation
        // For now, we'll just emit a signal
        qDebug() << "Add layer requested";
    }
}

void LayerWidget::removeSelectedLayers()
{
    if (!m_layerManager) {
        return;
    }
    
    QList<Layer*> layers = selectedLayers();
    for (Layer* layer : layers) {
        int index = m_layerManager->indexOf(layer);
        if (index >= 0) {
            m_layerManager->removeLayer(index);
        }
    }
}

void LayerWidget::duplicateSelectedLayers()
{
    // TODO: Implement layer duplication
    qDebug() << "Duplicate layers requested";
}

void LayerWidget::moveLayersUp()
{
    if (!m_layerManager) {
        return;
    }
    
    QList<Layer*> layers = selectedLayers();
    for (Layer* layer : layers) {
        int index = m_layerManager->indexOf(layer);
        if (index > 0) {
            m_layerManager->moveLayer(index, index - 1);
        }
    }
}

void LayerWidget::moveLayersDown()
{
    if (!m_layerManager) {
        return;
    }
    
    QList<Layer*> layers = selectedLayers();
    for (Layer* layer : layers) {
        int index = m_layerManager->indexOf(layer);
        if (index < m_layerManager->layerCount() - 1) {
            m_layerManager->moveLayer(index, index + 1);
        }
    }
}

void LayerWidget::toggleLayerVisibility()
{
    QList<Layer*> layers = selectedLayers();
    for (Layer* layer : layers) {
        layer->setVisible(!layer->isVisible());
    }
}

void LayerWidget::showLayerProperties()
{
    // TODO: Implement layer properties dialog
    qDebug() << "Show layer properties requested";
}

void LayerWidget::refresh()
{
    if (m_treeView->model()) {
        m_treeView->model()->layoutChanged();
    }
    updateButtonStates();
}

void LayerWidget::contextMenuEvent(QContextMenuEvent* event)
{
    if (m_contextMenu) {
        m_contextMenu->exec(event->globalPos());
    }
}

void LayerWidget::onSelectionChanged()
{
    updateButtonStates();
    emit layerSelectionChanged(selectedLayers());
}

void LayerWidget::onItemDoubleClicked(const QModelIndex& index)
{
    Layer* layer = layerFromIndex(index);
    if (layer) {
        emit layerDoubleClicked(layer);
    }
}

void LayerWidget::onLayerModelChanged()
{
    updateButtonStates();
}

void LayerWidget::showContextMenu(const QPoint& position)
{
    Q_UNUSED(position)
    if (m_contextMenu) {
        m_contextMenu->exec(QCursor::pos());
    }
}

void LayerWidget::onAddLayerTriggered()
{
    addLayer();
}

void LayerWidget::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(4, 4, 4, 4);
    m_mainLayout->setSpacing(4);
    
    createToolbar();
    
    // Create tree view
    m_treeView = new QTreeView(this);
    m_treeView->setRootIsDecorated(false);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Configure header
    m_treeView->header()->setStretchLastSection(false);
    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_treeView->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    
    // Connect signals
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &LayerWidget::onSelectionChanged);
    connect(m_treeView, &QTreeView::doubleClicked, this, &LayerWidget::onItemDoubleClicked);
    connect(m_treeView, &QTreeView::customContextMenuRequested,
            this, &LayerWidget::showContextMenu);
    
    m_mainLayout->addWidget(m_treeView);
}

void LayerWidget::createToolbar()
{
    m_toolbarLayout = new QHBoxLayout();
    m_toolbarLayout->setContentsMargins(0, 0, 0, 0);
    m_toolbarLayout->setSpacing(2);
    
    // Add button with menu
    m_addButton = new QToolButton(this);
    m_addButton->setText("+");
    m_addButton->setToolTip("Add Layer");
    m_addButton->setPopupMode(QToolButton::InstantPopup);
    
    m_addLayerMenu = new QMenu(this);
    m_addImageLayerAction = m_addLayerMenu->addAction("Image Layer");
    m_addPointsLayerAction = m_addLayerMenu->addAction("Points Layer");
    m_addShapesLayerAction = m_addLayerMenu->addAction("Shapes Layer");
    m_addSurfaceLayerAction = m_addLayerMenu->addAction("Surface Layer");
    m_addVolumeLayerAction = m_addLayerMenu->addAction("Volume Layer");
    m_addLabelsLayerAction = m_addLayerMenu->addAction("Labels Layer");
    m_addTracksLayerAction = m_addLayerMenu->addAction("Tracks Layer");
    m_addVectorsLayerAction = m_addLayerMenu->addAction("Vectors Layer");
    
    m_addButton->setMenu(m_addLayerMenu);
    
    // Other buttons
    m_removeButton = new QToolButton(this);
    m_removeButton->setText("-");
    m_removeButton->setToolTip("Remove Selected Layers");
    
    m_duplicateButton = new QToolButton(this);
    m_duplicateButton->setText("⧉");
    m_duplicateButton->setToolTip("Duplicate Selected Layers");
    
    m_moveUpButton = new QToolButton(this);
    m_moveUpButton->setText("↑");
    m_moveUpButton->setToolTip("Move Layers Up");
    
    m_moveDownButton = new QToolButton(this);
    m_moveDownButton->setText("↓");
    m_moveDownButton->setToolTip("Move Layers Down");
    
    m_propertiesButton = new QToolButton(this);
    m_propertiesButton->setText("⚙");
    m_propertiesButton->setToolTip("Layer Properties");
    
    // Add buttons to layout
    m_toolbarLayout->addWidget(m_addButton);
    m_toolbarLayout->addWidget(m_removeButton);
    m_toolbarLayout->addWidget(m_duplicateButton);
    m_toolbarLayout->addWidget(m_moveUpButton);
    m_toolbarLayout->addWidget(m_moveDownButton);
    m_toolbarLayout->addStretch();
    m_toolbarLayout->addWidget(m_propertiesButton);
    
    // Connect button signals
    connect(m_removeButton, &QToolButton::clicked, this, &LayerWidget::removeSelectedLayers);
    connect(m_duplicateButton, &QToolButton::clicked, this, &LayerWidget::duplicateSelectedLayers);
    connect(m_moveUpButton, &QToolButton::clicked, this, &LayerWidget::moveLayersUp);
    connect(m_moveDownButton, &QToolButton::clicked, this, &LayerWidget::moveLayersDown);
    connect(m_propertiesButton, &QToolButton::clicked, this, &LayerWidget::showLayerProperties);
    
    // Connect menu actions
    connect(m_addImageLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addPointsLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addShapesLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addSurfaceLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addVolumeLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addLabelsLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addTracksLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    connect(m_addVectorsLayerAction, &QAction::triggered, this, &LayerWidget::onAddLayerTriggered);
    
    m_mainLayout->addLayout(m_toolbarLayout);
}

void LayerWidget::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    m_removeAction = m_contextMenu->addAction("Remove");
    m_duplicateAction = m_contextMenu->addAction("Duplicate");
    m_contextMenu->addSeparator();
    m_moveUpAction = m_contextMenu->addAction("Move Up");
    m_moveDownAction = m_contextMenu->addAction("Move Down");
    m_contextMenu->addSeparator();
    m_toggleVisibilityAction = m_contextMenu->addAction("Toggle Visibility");
    m_propertiesAction = m_contextMenu->addAction("Properties...");
    
    // Connect actions
    connect(m_removeAction, &QAction::triggered, this, &LayerWidget::removeSelectedLayers);
    connect(m_duplicateAction, &QAction::triggered, this, &LayerWidget::duplicateSelectedLayers);
    connect(m_moveUpAction, &QAction::triggered, this, &LayerWidget::moveLayersUp);
    connect(m_moveDownAction, &QAction::triggered, this, &LayerWidget::moveLayersDown);
    connect(m_toggleVisibilityAction, &QAction::triggered, this, &LayerWidget::toggleLayerVisibility);
    connect(m_propertiesAction, &QAction::triggered, this, &LayerWidget::showLayerProperties);
}

void LayerWidget::updateButtonStates()
{
    bool hasSelection = !selectedLayers().isEmpty();
    bool hasLayers = m_layerManager && m_layerManager->layerCount() > 0;
    
    m_removeButton->setEnabled(hasSelection);
    m_duplicateButton->setEnabled(hasSelection);
    m_moveUpButton->setEnabled(hasSelection);
    m_moveDownButton->setEnabled(hasSelection);
    m_propertiesButton->setEnabled(hasSelection);
    
    if (m_removeAction) m_removeAction->setEnabled(hasSelection);
    if (m_duplicateAction) m_duplicateAction->setEnabled(hasSelection);
    if (m_moveUpAction) m_moveUpAction->setEnabled(hasSelection);
    if (m_moveDownAction) m_moveDownAction->setEnabled(hasSelection);
    if (m_toggleVisibilityAction) m_toggleVisibilityAction->setEnabled(hasSelection);
    if (m_propertiesAction) m_propertiesAction->setEnabled(hasSelection);
}

Layer* LayerWidget::layerFromIndex(const QModelIndex& index) const
{
    if (!index.isValid() || !m_layerManager) {
        return nullptr;
    }
    
    return m_layerManager->layer(index.row());
}

QModelIndex LayerWidget::indexFromLayer(Layer* layer) const
{
    if (!layer || !m_layerManager) {
        return QModelIndex();
    }
    
    int index = m_layerManager->indexOf(layer);
    if (index >= 0) {
        return m_layerManager->index(index, 0);
    }
    
    return QModelIndex();
}
