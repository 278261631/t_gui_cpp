#include "SimpleLayer.h"

SimpleLayer::SimpleLayer(const QString& name, QObject* parent)
    : Layer(name, LayerType::Image, parent)
    , m_bounds({-100.0f, -100.0f, 100.0f, 100.0f})
{
}

SimpleLayer::~SimpleLayer()
{
}

QVariant SimpleLayer::data() const
{
    return m_data;
}

void SimpleLayer::setData(const QVariant& data)
{
    if (m_data != data) {
        m_data = data;
        emit changed();
    }
}

QVector<float> SimpleLayer::bounds() const
{
    return m_bounds;
}

void SimpleLayer::render(void* context)
{
    Q_UNUSED(context)
    // Simple render implementation - just a placeholder
}
