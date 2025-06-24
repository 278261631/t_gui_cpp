#pragma once

#include <QObject>
#include <QAbstractItemModel>
#include <QVariant>
#include <QModelIndex>
#include <memory>

class Layer;

/**
 * @brief Layer types enumeration
 */
enum class LayerType
{
    Image,      ///< Image layer
    Points,     ///< Points layer
    Shapes,     ///< Shapes layer
    Surface,    ///< Surface layer
    Volume,     ///< Volume layer
    Labels,     ///< Labels layer
    Tracks,     ///< Tracks layer
    Vectors     ///< Vectors layer
};

/**
 * @brief Base layer class
 */
class Layer : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param name Layer name
     * @param type Layer type
     * @param parent Parent object
     */
    explicit Layer(const QString& name, LayerType type, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    virtual ~Layer() = default;

    /**
     * @brief Get layer name
     * @return Layer name
     */
    QString name() const { return m_name; }

    /**
     * @brief Set layer name
     * @param name New name
     */
    void setName(const QString& name);

    /**
     * @brief Get layer type
     * @return Layer type
     */
    LayerType type() const { return m_type; }

    /**
     * @brief Check if layer is visible
     * @return true if visible
     */
    bool isVisible() const { return m_visible; }

    /**
     * @brief Set layer visibility
     * @param visible Visibility state
     */
    void setVisible(bool visible);

    /**
     * @brief Get layer opacity
     * @return Opacity value (0.0 - 1.0)
     */
    float opacity() const { return m_opacity; }

    /**
     * @brief Set layer opacity
     * @param opacity Opacity value (0.0 - 1.0)
     */
    void setOpacity(float opacity);

    /**
     * @brief Check if layer is selected
     * @return true if selected
     */
    bool isSelected() const { return m_selected; }

    /**
     * @brief Set layer selection state
     * @param selected Selection state
     */
    void setSelected(bool selected);

    /**
     * @brief Get layer data
     * @return Layer data as QVariant
     */
    virtual QVariant data() const = 0;

    /**
     * @brief Set layer data
     * @param data New data
     */
    virtual void setData(const QVariant& data) = 0;

    /**
     * @brief Get layer bounds
     * @return Bounding box as [xmin, ymin, xmax, ymax]
     */
    virtual QVector<float> bounds() const = 0;

    /**
     * @brief Render the layer
     * @param context Render context
     */
    virtual void render(void* context) = 0;

signals:
    /**
     * @brief Emitted when layer properties change
     */
    void changed();

    /**
     * @brief Emitted when layer name changes
     * @param name New name
     */
    void nameChanged(const QString& name);

    /**
     * @brief Emitted when layer visibility changes
     * @param visible New visibility state
     */
    void visibilityChanged(bool visible);

    /**
     * @brief Emitted when layer opacity changes
     * @param opacity New opacity
     */
    void opacityChanged(float opacity);

    /**
     * @brief Emitted when layer selection changes
     * @param selected New selection state
     */
    void selectionChanged(bool selected);

protected:
    QString m_name;
    LayerType m_type;
    bool m_visible;
    float m_opacity;
    bool m_selected;
};

/**
 * @brief Layer manager class
 * 
 * Manages a collection of layers and provides a model interface for UI components.
 */
class LayerManager : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit LayerManager(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~LayerManager();

    // QAbstractItemModel interface
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    /**
     * @brief Add a layer
     * @param layer Layer to add
     * @param index Insert position (-1 for end)
     */
    void addLayer(Layer* layer, int index = -1);

    /**
     * @brief Remove a layer
     * @param index Layer index
     * @return true if successful
     */
    bool removeLayer(int index);

    /**
     * @brief Remove a layer by name
     * @param name Layer name
     * @return true if successful
     */
    bool removeLayer(const QString& name);

    /**
     * @brief Get layer count
     * @return Number of layers
     */
    int layerCount() const { return m_layers.size(); }

    /**
     * @brief Get layer by index
     * @param index Layer index
     * @return Layer pointer or nullptr
     */
    Layer* layer(int index) const;

    /**
     * @brief Get layer by name
     * @param name Layer name
     * @return Layer pointer or nullptr
     */
    Layer* layer(const QString& name) const;

    /**
     * @brief Get all layers
     * @return List of layer pointers
     */
    QList<Layer*> layers() const;

    /**
     * @brief Get selected layers
     * @return List of selected layer pointers
     */
    QList<Layer*> selectedLayers() const;

    /**
     * @brief Clear all layers
     */
    void clear();

    /**
     * @brief Move layer
     * @param from Source index
     * @param to Destination index
     * @return true if successful
     */
    bool moveLayer(int from, int to);

    /**
     * @brief Get layer index
     * @param layer Layer pointer
     * @return Layer index or -1 if not found
     */
    int indexOf(Layer* layer) const;

signals:
    /**
     * @brief Emitted when a layer is added
     * @param layer Added layer
     * @param index Layer index
     */
    void layerAdded(Layer* layer, int index);

    /**
     * @brief Emitted when a layer is removed
     * @param index Removed layer index
     */
    void layerRemoved(int index);

    /**
     * @brief Emitted when layers are reordered
     */
    void layersReordered();

    /**
     * @brief Emitted when layer selection changes
     */
    void selectionChanged();

private slots:
    /**
     * @brief Handle layer changes
     */
    void onLayerChanged();

    /**
     * @brief Handle layer selection changes
     * @param selected Selection state
     */
    void onLayerSelectionChanged(bool selected);

private:
    QList<Layer*> m_layers;
};
