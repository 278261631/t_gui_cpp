#pragma once

#include <QWidget>
#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

class LayerManager;
class Layer;
class QStandardItemModel;

/**
 * @brief Layer control widget
 * 
 * Provides a tree view of layers with controls for visibility, opacity,
 * and layer management operations.
 */
class LayerWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit LayerWidget(QWidget* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~LayerWidget();

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
     * @brief Get selected layers
     * @return List of selected layer pointers
     */
    QList<Layer*> selectedLayers() const;

public slots:
    /**
     * @brief Add new layer
     */
    void addLayer();

    /**
     * @brief Remove selected layers
     */
    void removeSelectedLayers();

    /**
     * @brief Duplicate selected layers
     */
    void duplicateSelectedLayers();

    /**
     * @brief Move selected layers up
     */
    void moveLayersUp();

    /**
     * @brief Move selected layers down
     */
    void moveLayersDown();

    /**
     * @brief Toggle visibility of selected layers
     */
    void toggleLayerVisibility();

    /**
     * @brief Show layer properties dialog
     */
    void showLayerProperties();

    /**
     * @brief Refresh layer list
     */
    void refresh();

signals:
    /**
     * @brief Emitted when layer selection changes
     * @param layers Selected layers
     */
    void layerSelectionChanged(const QList<Layer*>& layers);

    /**
     * @brief Emitted when a layer is double-clicked
     * @param layer Double-clicked layer
     */
    void layerDoubleClicked(Layer* layer);

    /**
     * @brief Emitted when layer visibility is toggled
     * @param layer Layer
     * @param visible New visibility state
     */
    void layerVisibilityToggled(Layer* layer, bool visible);

protected:
    /**
     * @brief Handle context menu event
     * @param event Context menu event
     */
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    /**
     * @brief Handle tree view selection changes
     */
    void onSelectionChanged();

    /**
     * @brief Handle tree view double-click
     * @param index Model index
     */
    void onItemDoubleClicked(const QModelIndex& index);

    /**
     * @brief Handle layer model changes
     */
    void onLayerModelChanged();

    /**
     * @brief Show context menu
     * @param position Menu position
     */
    void showContextMenu(const QPoint& position);

    /**
     * @brief Handle add layer menu
     */
    void onAddLayerTriggered();

private:
    /**
     * @brief Setup user interface
     */
    void setupUI();

    /**
     * @brief Create toolbar
     */
    void createToolbar();

    /**
     * @brief Create context menu
     */
    void createContextMenu();

    /**
     * @brief Update button states
     */
    void updateButtonStates();

    /**
     * @brief Get layer from model index
     * @param index Model index
     * @return Layer pointer or nullptr
     */
    Layer* layerFromIndex(const QModelIndex& index) const;

    /**
     * @brief Get model index from layer
     * @param layer Layer pointer
     * @return Model index
     */
    QModelIndex indexFromLayer(Layer* layer) const;

private:
    // UI components
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_toolbarLayout;
    QTreeView* m_treeView;
    
    // Toolbar buttons
    QToolButton* m_addButton;
    QToolButton* m_removeButton;
    QToolButton* m_duplicateButton;
    QToolButton* m_moveUpButton;
    QToolButton* m_moveDownButton;
    QToolButton* m_propertiesButton;
    
    // Menus
    QMenu* m_addLayerMenu;
    QMenu* m_contextMenu;
    
    // Actions
    QAction* m_addImageLayerAction;
    QAction* m_addPointsLayerAction;
    QAction* m_addShapesLayerAction;
    QAction* m_addSurfaceLayerAction;
    QAction* m_addVolumeLayerAction;
    QAction* m_addLabelsLayerAction;
    QAction* m_addTracksLayerAction;
    QAction* m_addVectorsLayerAction;
    
    QAction* m_removeAction;
    QAction* m_duplicateAction;
    QAction* m_moveUpAction;
    QAction* m_moveDownAction;
    QAction* m_toggleVisibilityAction;
    QAction* m_propertiesAction;
    
    // Data
    LayerManager* m_layerManager;
};
