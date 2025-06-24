#pragma once

#include "LayerManager.h"
#include <QVariant>
#include <QVector>

/**
 * @brief Simple layer implementation for testing
 */
class SimpleLayer : public Layer
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param name Layer name
     * @param parent Parent object
     */
    explicit SimpleLayer(const QString& name, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~SimpleLayer();

    // Layer interface implementation
    QVariant data() const override;
    void setData(const QVariant& data) override;
    QVector<float> bounds() const override;
    void render(void* context) override;

private:
    QVariant m_data;
    QVector<float> m_bounds;
};
