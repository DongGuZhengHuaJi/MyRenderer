#pragma once

#include <QMainWindow>
#include <QTreeWidget>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QTextEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class RenderViewport;
class SceneNode;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSceneChanged();
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onTreeContextMenuRequested(const QPoint& pos);
    void onOpenModel();
    void onWireframe(bool checked);
    void onResetCamera();
    void onTransformEdited();

private:
    void setupInspector();
    void rebuildSceneTree();
    void updateInspectorFromNode(SceneNode* node);

    Ui::MainWindow* ui;
    RenderViewport* m_viewport = nullptr;
    QWidget* m_inspectorContent = nullptr;
    QFormLayout* m_inspectorLayout = nullptr;
    QDoubleSpinBox* m_spinPosX = nullptr;
    QDoubleSpinBox* m_spinPosY = nullptr;
    QDoubleSpinBox* m_spinPosZ = nullptr;
    QDoubleSpinBox* m_spinRotX = nullptr;
    QDoubleSpinBox* m_spinRotY = nullptr;
    QDoubleSpinBox* m_spinRotZ = nullptr;
    QDoubleSpinBox* m_spinSclX = nullptr;
    QDoubleSpinBox* m_spinSclY = nullptr;
    QDoubleSpinBox* m_spinSclZ = nullptr;

    SceneNode* m_selectedNode = nullptr;
    bool m_updatingInspector = false;
};
