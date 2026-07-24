#pragma once

#include <QMainWindow>
#include <QTreeWidget>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QScrollArea>
#include <QTextEdit>
#include <QLabel>
#include <QDialog>

#include "ui/AddNodeDialog.h"

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
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onTreeContextMenuRequested(const QPoint& pos);
    void onAddNode(QString name, NodeType type);
    void onWireframe(bool checked);
    void onResetCamera();
    void onSceneStructureChanged();
    void onInspectorChanged(SceneNode* node);

    // Inspector slots
    void onNameEdited();
    void onTransformEdited();
    void onVisibilityChanged(bool visible);
    void onCameraParamsEdited();
    void onCameraActiveToggled(bool active);

private:
    void setupInspector();
    void rebuildSceneTree();
    void updateInspectorFromNode(SceneNode* node);

    Ui::MainWindow* ui;
    RenderViewport* m_viewport = nullptr;

    // --- Inspector widgets ---
    QWidget* m_inspectorContent = nullptr;
    QVBoxLayout* m_inspectorLayout = nullptr;

    // Node info
    QLabel* m_nodeTypeLabel = nullptr;

    // Name
    QLineEdit* m_nameEdit = nullptr;

    // Transform
    QGroupBox* m_transformGroup = nullptr;
    QDoubleSpinBox* m_spinPosX = nullptr;
    QDoubleSpinBox* m_spinPosY = nullptr;
    QDoubleSpinBox* m_spinPosZ = nullptr;
    QDoubleSpinBox* m_spinRotX = nullptr;
    QDoubleSpinBox* m_spinRotY = nullptr;
    QDoubleSpinBox* m_spinRotZ = nullptr;
    QDoubleSpinBox* m_spinSclX = nullptr;
    QDoubleSpinBox* m_spinSclY = nullptr;
    QDoubleSpinBox* m_spinSclZ = nullptr;

    // Model
    QGroupBox* m_modelGroup = nullptr;
    QCheckBox* m_visibleCheck = nullptr;
    QLabel* m_meshCountLabel = nullptr;

    // Camera
    QGroupBox* m_cameraGroup = nullptr;
    QCheckBox* m_activeCheck = nullptr;
    QDoubleSpinBox* m_spinFov = nullptr;
    QDoubleSpinBox* m_spinNear = nullptr;
    QDoubleSpinBox* m_spinFar = nullptr;
    QLabel* m_aspectLabel = nullptr;
    QDoubleSpinBox* m_spinCamPosX = nullptr;
    QDoubleSpinBox* m_spinCamPosY = nullptr;
    QDoubleSpinBox* m_spinCamPosZ = nullptr;

    SceneNode* m_selectedNode = nullptr;
    bool m_updatingInspector = false;

private:
    AddNodeDialog* m_addNodeDialog;
};
