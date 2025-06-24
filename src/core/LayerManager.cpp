#include "LayerManager.h"
#include <QDebug>

// Layer implementation
Layer::Layer(const QString& name, LayerType type, QObject* parent)
    : QObject(parent)
    , m_name(name)
    , m_type(type)
    , m_visible(true)
    , m_opacity(1.0f)
    , m_selected(false)
{
}

void Layer::setName(const QString& name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged(name);
        emit changed();
    }
}

void Layer::setVisible(bool visible)
{
    if (m_visible != visible) {
        m_visible = visible;
        emit visibilityChanged(visible);
        emit changed();
    }
}

void Layer::setOpacity(float opacity)
{
    opacity = qMax(0.0f, qMin(1.0f, opacity));
    if (qAbs(m_opacity - opacity) > 0.001f) {
        m_opacity = opacity;
        emit opacityChanged(opacity);
        emit changed();
    }
}

void Layer::setSelected(bool selected)
{
    if (m_selected != selected) {
        m_selected = selected;
        emit selectionChanged(selected);
        emit changed();
    }
}

// LayerManager implementation
LayerManager::LayerManager(QObject* parent)
    : QAbstractItemModel(parent)
{
}

LayerManager::~LayerManager()
{
    clear();
}

QModelIndex LayerManager::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    
    if (row < 0 || row >= m_layers.size() || column < 0 || column >= 3) {
        return QModelIndex();
    }
    
    return createIndex(row, column, m_layers[row]);
}

QModelIndex LayerManager::parent(const QModelIndex& child) const
{
    Q_UNUSED(child)
    return QModelIndex(); // Flat list, no parent
}

int LayerManager::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_layers.size();
}

int LayerManager::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return 3; // Name, Visible, Opacity
}

QVariant LayerManager::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_layers.size()) {
        return QVariant();
    }
    
    Layer* layer = m_layers[index.row()];
    
    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case 0: return layer->name();
        case 1: return layer->isVisible() ? "Visible" : "Hidden";
        case 2: return QString::number(layer->opacity(), 'f', 2);
        }
        break;
        
    case Qt::CheckStateRole:
        if (index.column() == 1) {
            return layer->isVisible() ? Qt::Checked : Qt::Unchecked;
        }
        break;
        
    case Qt::UserRole:
        return QVariant::fromValue(layer);
    }
    
    return QVariant();
}

QVariant LayerManager::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0: return "Name";
        case 1: return "Visible";
        case 2: return "Opacity";
        }
    }
    return QVariant();
}

Qt::ItemFlags LayerManager::flags(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }
    
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    if (index.column() == 0) {
        flags |= Qt::ItemIsEditable;
    } else if (index.column() == 1) {
        flags |= Qt::ItemIsUserCheckable;
    }
    
    return flags;
}

bool LayerManager::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || index.row() >= m_layers.size()) {
        return false;
    }
    
    Layer* layer = m_layers[index.row()];
    
    if (role == Qt::EditRole && index.column() == 0) {
        layer->setName(value.toString());
        emit dataChanged(index, index);
        return true;
    } else if (role == Qt::CheckStateRole && index.column() == 1) {
        layer->setVisible(value.toInt() == Qt::Checked);
        emit dataChanged(index, index);
        return true;
    }
    
    return false;
}

void LayerManager::addLayer(Layer* layer, int index)
{
    if (!layer) {
        return;
    }

    if (index < 0 || index > m_layers.size()) {
        index = m_layers.size();
    }

    beginInsertRows(QModelIndex(), index, index);

    // Connect layer signals
    connect(layer, &Layer::changed, this, &LayerManager::onLayerChanged);
    connect(layer, &Layer::selectionChanged, this, &LayerManager::onLayerSelectionChanged);

    // Set parent to manage memory
    layer->setParent(this);

    m_layers.insert(index, layer);
    endInsertRows();

    emit layerAdded(m_layers[index], index);
}

bool LayerManager::removeLayer(int index)
{
    if (index < 0 || index >= m_layers.size()) {
        return false;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    m_layers.removeAt(index);
    endRemoveRows();
    
    emit layerRemoved(index);
    return true;
}

bool LayerManager::removeLayer(const QString& name)
{
    for (int i = 0; i < m_layers.size(); ++i) {
        if (m_layers[i]->name() == name) {
            return removeLayer(i);
        }
    }
    return false;
}

Layer* LayerManager::layer(int index) const
{
    if (index >= 0 && index < m_layers.size()) {
        return m_layers[index];
    }
    return nullptr;
}

Layer* LayerManager::layer(const QString& name) const
{
    for (Layer* layer : m_layers) {
        if (layer->name() == name) {
            return layer;
        }
    }
    return nullptr;
}

QList<Layer*> LayerManager::layers() const
{
    return m_layers;
}

QList<Layer*> LayerManager::selectedLayers() const
{
    QList<Layer*> result;
    for (Layer* layer : m_layers) {
        if (layer->isSelected()) {
            result.append(layer);
        }
    }
    return result;
}

void LayerManager::clear()
{
    if (!m_layers.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, m_layers.size() - 1);
        m_layers.clear();
        endRemoveRows();
    }
}

bool LayerManager::moveLayer(int from, int to)
{
    if (from < 0 || from >= m_layers.size() || to < 0 || to >= m_layers.size() || from == to) {
        return false;
    }
    
    beginMoveRows(QModelIndex(), from, from, QModelIndex(), to > from ? to + 1 : to);
    m_layers.move(from, to);
    endMoveRows();
    
    emit layersReordered();
    return true;
}

int LayerManager::indexOf(Layer* layer) const
{
    return m_layers.indexOf(layer);
}

void LayerManager::onLayerChanged()
{
    Layer* layer = qobject_cast<Layer*>(sender());
    if (layer) {
        int index = indexOf(layer);
        if (index >= 0) {
            QModelIndex topLeft = createIndex(index, 0);
            QModelIndex bottomRight = createIndex(index, columnCount() - 1);
            emit dataChanged(topLeft, bottomRight);
        }
    }
}

void LayerManager::onLayerSelectionChanged(bool selected)
{
    Q_UNUSED(selected)
    emit selectionChanged();
}
