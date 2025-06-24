#pragma once

#include "../../src/plugins/PluginInterface.h"
#include <QObject>
#include <QWidget>
#include <QAction>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

class Application;

/**
 * @brief Example plugin widget
 */
class ExamplePluginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExamplePluginWidget(QWidget* parent = nullptr);

private slots:
    void onButtonClicked();

private:
    QVBoxLayout* m_layout;
    QLabel* m_label;
    QPushButton* m_button;
    int m_clickCount;
};

/**
 * @brief Example plugin implementation
 * 
 * Demonstrates how to create a simple UI plugin with menu actions
 * and a dock widget.
 */
class ExamplePlugin : public QObject, public UIPluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.t-gui.UIPluginInterface/1.0" FILE "plugin.json")
    Q_INTERFACES(UIPluginInterface)

public:
    /**
     * @brief Constructor
     * @param parent Parent object
     */
    explicit ExamplePlugin(QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~ExamplePlugin();

    // PluginInterface implementation
    PluginMetadata metadata() const override;
    bool initialize(Application* app) override;
    void shutdown() override;
    bool isEnabled() const override;
    void setEnabled(bool enabled) override;
    QWidget* configurationWidget() override;
    void configure(const QJsonObject& config) override;
    QJsonObject configuration() const override;

    // UIPluginInterface implementation
    QWidget* widget() override;
    QList<QAction*> menuActions() override;
    QList<QAction*> toolbarActions() override;
    QWidget* dockWidget() override;
    QString dockWidgetTitle() const override;
    Qt::DockWidgetArea dockWidgetArea() const override;

private slots:
    /**
     * @brief Handle menu action
     */
    void onMenuActionTriggered();

    /**
     * @brief Handle toolbar action
     */
    void onToolbarActionTriggered();

    /**
     * @brief Show about dialog
     */
    void showAbout();

private:
    /**
     * @brief Create actions
     */
    void createActions();

    /**
     * @brief Create widget
     */
    void createWidget();

private:
    // Plugin state
    Application* m_application;
    bool m_enabled;
    bool m_initialized;

    // UI components
    ExamplePluginWidget* m_widget;
    QList<QAction*> m_menuActions;
    QList<QAction*> m_toolbarActions;

    // Actions
    QAction* m_exampleAction;
    QAction* m_aboutAction;

    // Configuration
    QJsonObject m_config;
};
