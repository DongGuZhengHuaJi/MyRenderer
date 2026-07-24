#include "MainWindow.h"
#include "ui/AddNodeDialog.h"
#include "ui_MainWindow.h"
#include "window/RenderViewport.h"
#include "scene/SceneNode.h"
#include "scene/ModelNode.h"
#include "scene/CameraNode.h"
#include "scene/NodeFactory.h"

#include <QFileDialog>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_viewport = qobject_cast<RenderViewport*>(ui->openGLWidget);

    ui->sceneTree->setHeaderHidden(false);
    ui->sceneTree->setHeaderLabel(tr("Scene"));
    ui->sceneTree->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->sceneTree->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->logOutput->setReadOnly(true);

    ui->actionWireFrame->setCheckable(true);

    setupInspector();

    m_addNodeDialog = new AddNodeDialog(this);

    connect(m_viewport,
            &RenderViewport::sceneStructureChanged,
            this,
            &MainWindow::onSceneStructureChanged);

    connect(m_viewport,
            &RenderViewport::inspectorChanged,
            this,
            &MainWindow::onInspectorChanged);
    connect(ui->sceneTree, &QTreeWidget::itemClicked,
            this, &MainWindow::onTreeItemClicked);
    connect(ui->sceneTree, &QTreeWidget::customContextMenuRequested,
            this, &MainWindow::onTreeContextMenuRequested);
    connect(ui->actionAddNode, &QAction::triggered,
            this, [this]() { m_addNodeDialog->show(); });
    connect(ui->actionWireFrame, &QAction::toggled,
            this, &MainWindow::onWireframe);
    connect(ui->actionResetCamera, &QAction::triggered,
            this, &MainWindow::onResetCamera);

    connect(m_addNodeDialog, &AddNodeDialog::addNode, this, &MainWindow::onAddNode);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ========== Inspector 初始化 ==========

static QDoubleSpinBox* makeSpinBox(double min, double max, double step, double value) {
    auto* sb = new QDoubleSpinBox();
    sb->setRange(min, max);
    sb->setSingleStep(step);
    sb->setDecimals(3);
    sb->setValue(value);
    sb->setMinimumWidth(80);
    return sb;
}

void MainWindow::setupInspector()
{
    m_inspectorContent = ui->inspector->widget();
    if (!m_inspectorContent) {
        m_inspectorContent = new QWidget();
        ui->inspector->setWidget(m_inspectorContent);
    }

    // 清除 scrollArea 自带的 layout，用自定义 VBoxLayout
    delete m_inspectorContent->layout();
    m_inspectorLayout = new QVBoxLayout(m_inspectorContent);
    m_inspectorLayout->setAlignment(Qt::AlignTop);

    // ---- 节点类型标签 ----
    m_nodeTypeLabel = new QLabel();
    m_nodeTypeLabel->setStyleSheet("font-weight: bold; font-size: 13px; padding: 4px;");
    m_inspectorLayout->addWidget(m_nodeTypeLabel);

    // ---- 名称 ----
    auto* nameGroup = new QGroupBox(tr("Name"));
    auto* nameLayout = new QFormLayout(nameGroup);
    m_nameEdit = new QLineEdit();
    nameLayout->addRow(m_nameEdit);
    m_inspectorLayout->addWidget(nameGroup);

    // ---- Transform ----
    m_transformGroup = new QGroupBox(tr("Transform"));

    auto* posGroup = new QGroupBox(tr("Position"));
    auto* posLayout = new QFormLayout(posGroup);
    m_spinPosX = makeSpinBox(-999, 999, 0.1, 0);
    m_spinPosY = makeSpinBox(-999, 999, 0.1, 0);
    m_spinPosZ = makeSpinBox(-999, 999, 0.1, 0);
    posLayout->addRow("X", m_spinPosX);
    posLayout->addRow("Y", m_spinPosY);
    posLayout->addRow("Z", m_spinPosZ);

    auto* rotGroup = new QGroupBox(tr("Rotation"));
    auto* rotLayout = new QFormLayout(rotGroup);
    m_spinRotX = makeSpinBox(-360, 360, 1, 0);
    m_spinRotY = makeSpinBox(-360, 360, 1, 0);
    m_spinRotZ = makeSpinBox(-360, 360, 1, 0);
    rotLayout->addRow("X", m_spinRotX);
    rotLayout->addRow("Y", m_spinRotY);
    rotLayout->addRow("Z", m_spinRotZ);

    auto* sclGroup = new QGroupBox(tr("Scale"));
    auto* sclLayout = new QFormLayout(sclGroup);
    m_spinSclX = makeSpinBox(0.01, 999, 0.1, 1);
    m_spinSclY = makeSpinBox(0.01, 999, 0.1, 1);
    m_spinSclZ = makeSpinBox(0.01, 999, 0.1, 1);
    sclLayout->addRow("X", m_spinSclX);
    sclLayout->addRow("Y", m_spinSclY);
    sclLayout->addRow("Z", m_spinSclZ);

    auto* tfLayout = new QVBoxLayout(m_transformGroup);
    tfLayout->addWidget(posGroup);
    tfLayout->addWidget(rotGroup);
    tfLayout->addWidget(sclGroup);
    m_inspectorLayout->addWidget(m_transformGroup);

    // ---- Model 专属 ----
    m_modelGroup = new QGroupBox(tr("Model"));
    auto* modelLayout = new QFormLayout(m_modelGroup);
    m_visibleCheck = new QCheckBox(tr("Visible"));
    m_visibleCheck->setChecked(true);
    modelLayout->addRow(m_visibleCheck);
    m_meshCountLabel = new QLabel();
    modelLayout->addRow(tr("Meshes:"), m_meshCountLabel);
    m_inspectorLayout->addWidget(m_modelGroup);

    // ---- Camera 专属 ----
    m_cameraGroup = new QGroupBox(tr("Camera"));
    auto* camLayout = new QFormLayout(m_cameraGroup);
    m_activeCheck = new QCheckBox(tr("Active"));
    m_activeCheck->setChecked(true);
    camLayout->addRow(m_activeCheck);

    auto* projGroup = new QGroupBox(tr("Projection"));
    auto* projLayout = new QFormLayout(projGroup);
    m_spinFov = makeSpinBox(1, 179, 1, 45);
    m_spinNear = makeSpinBox(0.001, 100, 0.1, 0.1);
    m_spinFar = makeSpinBox(1, 9999, 10, 100);
    projLayout->addRow("FOV", m_spinFov);
    projLayout->addRow("Near", m_spinNear);
    projLayout->addRow("Far", m_spinFar);
    m_aspectLabel = new QLabel();
    projLayout->addRow("Aspect", m_aspectLabel);
    camLayout->addRow(projGroup);

    auto* camPosGroup = new QGroupBox(tr("Position"));
    auto* camPosLayout = new QFormLayout(camPosGroup);
    m_spinCamPosX = makeSpinBox(-999, 999, 0.1, 0);
    m_spinCamPosY = makeSpinBox(-999, 999, 0.1, 0);
    m_spinCamPosZ = makeSpinBox(-999, 999, 0.1, 3);
    camPosLayout->addRow("X", m_spinCamPosX);
    camPosLayout->addRow("Y", m_spinCamPosY);
    camPosLayout->addRow("Z", m_spinCamPosZ);
    camLayout->addRow(camPosGroup);
    m_inspectorLayout->addWidget(m_cameraGroup);

    // ---- stretch ----
    m_inspectorLayout->addStretch();

    // ---- 信号连接 ----
    connect(m_nameEdit, &QLineEdit::editingFinished, this, &MainWindow::onNameEdited);

    auto connectSpin = [this](QDoubleSpinBox* sb) {
        connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformEdited);
    };
    connectSpin(m_spinPosX); connectSpin(m_spinPosY); connectSpin(m_spinPosZ);
    connectSpin(m_spinRotX); connectSpin(m_spinRotY); connectSpin(m_spinRotZ);
    connectSpin(m_spinSclX); connectSpin(m_spinSclY); connectSpin(m_spinSclZ);

    connect(m_visibleCheck, &QCheckBox::toggled, this, &MainWindow::onVisibilityChanged);

    auto connectCamSpin = [this](QDoubleSpinBox* sb) {
        connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onCameraParamsEdited);
    };
    connectCamSpin(m_spinFov); connectCamSpin(m_spinNear); connectCamSpin(m_spinFar);
    connectCamSpin(m_spinCamPosX); connectCamSpin(m_spinCamPosY); connectCamSpin(m_spinCamPosZ);
    connect(m_activeCheck, &QCheckBox::toggled, this, &MainWindow::onCameraActiveToggled);

    m_inspectorContent->setEnabled(false);
}

// ========== 场景树 ==========

void MainWindow::rebuildSceneTree()
{
    ui->sceneTree->clear();
    m_selectedNode = nullptr;
    m_inspectorContent->setEnabled(false);

    auto rootNode = m_viewport->getScene().getRoot();
    auto* rootItem = new QTreeWidgetItem({QString::fromStdString(rootNode->m_name)});
    rootItem->setData(0, Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(rootNode.get())));
    ui->sceneTree->addTopLevelItem(rootItem);

    auto addChildren = [this](QTreeWidgetItem* parentItem, const std::shared_ptr<SceneNode>& node, auto& addChildrenRef) -> void {
        for (const auto& child : node->m_children) {
            QTreeWidgetItem* childItem = new QTreeWidgetItem({QString::fromStdString(child->m_name)});
            childItem->setData(0, Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(child.get())));
            parentItem->addChild(childItem);
            addChildrenRef(childItem, child, addChildrenRef);
        }
    };
    addChildren(rootItem, rootNode, addChildren);
    ui->sceneTree->expandAll();
}

void MainWindow::onSceneStructureChanged()
{
    rebuildSceneTree();
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int /*column*/)
{
    if (!item) return;

    auto ptr = item->data(0, Qt::UserRole).value<quintptr>();
    auto* node = reinterpret_cast<SceneNode*>(ptr);
    m_selectedNode = node;
    m_viewport->selectModel(node);

    if (node) {
        updateInspectorFromNode(node);
        m_inspectorContent->setEnabled(true);
    }
}

void MainWindow::onTreeContextMenuRequested(const QPoint& pos)
{
    auto* item = ui->sceneTree->itemAt(pos);
    if (!item) return;

    QMenu contextMenu(this);
    QAction* deleteAction = contextMenu.addAction(tr("Delete Node"));
    connect(deleteAction, &QAction::triggered, this, [this, item]() {
        auto ptr = item->data(0, Qt::UserRole).value<quintptr>();
        auto* node = reinterpret_cast<SceneNode*>(ptr);
        if (node) {
            m_viewport->removeSelectedModel(node);
            rebuildSceneTree();
            ui->logOutput->append(
                QString("Deleted node: %1").arg(QString::fromStdString(node->m_name)));
        }
    });
    contextMenu.exec(ui->sceneTree->viewport()->mapToGlobal(pos));
}

// ========== Inspector 更新 ==========

void MainWindow::onInspectorChanged(SceneNode* node)
{
    if (node == nullptr)
        return;

    if (node != m_selectedNode)
        return;

    updateInspectorFromNode(node);
}

void MainWindow::updateInspectorFromNode(SceneNode* node)
{
    if (!node) return;
    m_updatingInspector = true;

    // --- 类型标签 ---
    QString typeName;
    if (dynamic_cast<CameraNode*>(node))        typeName = "CameraNode";
    else if (dynamic_cast<ModelNode*>(node))     typeName = "ModelNode";
    else                                         typeName = "SceneNode";
    m_nodeTypeLabel->setText(typeName);

    // --- 名称 ---
    m_nameEdit->setText(QString::fromStdString(node->m_name));

    // --- Transform ---
    auto& t = node->m_transform;
    m_spinPosX->setValue(static_cast<double>(t.position.x));
    m_spinPosY->setValue(static_cast<double>(t.position.y));
    m_spinPosZ->setValue(static_cast<double>(t.position.z));
    m_spinRotX->setValue(static_cast<double>(t.rotation.x));
    m_spinRotY->setValue(static_cast<double>(t.rotation.y));
    m_spinRotZ->setValue(static_cast<double>(t.rotation.z));
    m_spinSclX->setValue(static_cast<double>(t.scale.x));
    m_spinSclY->setValue(static_cast<double>(t.scale.y));
    m_spinSclZ->setValue(static_cast<double>(t.scale.z));
    m_transformGroup->setVisible(true);

    // --- Model 专属 ---
    if (auto* mdl = dynamic_cast<ModelNode*>(node)) {
        m_modelGroup->setVisible(true);
        m_visibleCheck->setChecked(mdl->visible);
        int meshCount = mdl->m_model ? static_cast<int>(mdl->m_model->m_meshes.size()) : 0;
        m_meshCountLabel->setText(QString::number(meshCount));
    } else {
        m_modelGroup->setVisible(false);
    }

    // --- Camera 专属 ---
    if (auto* cam = dynamic_cast<CameraNode*>(node)) {
        m_cameraGroup->setVisible(true);
        m_activeCheck->setChecked(cam->isActive);
        m_spinFov->setValue(static_cast<double>(cam->m_camera->fov));
        m_spinNear->setValue(static_cast<double>(cam->m_camera->nearPlane));
        m_spinFar->setValue(static_cast<double>(cam->m_camera->farPlane));
        m_aspectLabel->setText(QString::number(static_cast<double>(cam->m_camera->aspect), 'f', 3));
        m_spinCamPosX->setValue(static_cast<double>(cam->position.x));
        m_spinCamPosY->setValue(static_cast<double>(cam->position.y));
        m_spinCamPosZ->setValue(static_cast<double>(cam->position.z));
    } else {
        m_cameraGroup->setVisible(false);
    }

    m_updatingInspector = false;
}

// ========== Inspector 编辑回调 ==========

void MainWindow::onNameEdited()
{
    if (!m_selectedNode) return;
    m_selectedNode->m_name = m_nameEdit->text().toStdString();
    rebuildSceneTree();
}

void MainWindow::onTransformEdited()
{
    if (m_updatingInspector || !m_selectedNode) return;
    m_viewport->setModelTransform(
        m_selectedNode,
        static_cast<float>(m_spinPosX->value()),
        static_cast<float>(m_spinPosY->value()),
        static_cast<float>(m_spinPosZ->value()),
        static_cast<float>(m_spinRotX->value()),
        static_cast<float>(m_spinRotY->value()),
        static_cast<float>(m_spinRotZ->value()),
        static_cast<float>(m_spinSclX->value()),
        static_cast<float>(m_spinSclY->value()),
        static_cast<float>(m_spinSclZ->value()));
}

void MainWindow::onVisibilityChanged(bool visible)
{
    if (m_updatingInspector) return;
    if (auto* mdl = dynamic_cast<ModelNode*>(m_selectedNode)) {
        mdl->visible = visible;
        m_viewport->update();
    }
}

void MainWindow::onCameraParamsEdited()
{
    if (m_updatingInspector) return;
    if (auto* cam = dynamic_cast<CameraNode*>(m_selectedNode)) {
        cam->m_camera->fov = static_cast<float>(m_spinFov->value());
        cam->m_camera->nearPlane = static_cast<float>(m_spinNear->value());
        cam->m_camera->farPlane = static_cast<float>(m_spinFar->value());
        cam->position.x = static_cast<float>(m_spinCamPosX->value());
        cam->position.y = static_cast<float>(m_spinCamPosY->value());
        cam->position.z = static_cast<float>(m_spinCamPosZ->value());
        cam->updateCameraVectors();
        m_viewport->update();
    }
}

void MainWindow::onCameraActiveToggled(bool active)
{
    if (m_updatingInspector) return;
    if (auto* cam = dynamic_cast<CameraNode*>(m_selectedNode)) {
        cam->isActive = active;
    }

    for (const auto& node : m_viewport->getScene().getRoot()->m_children) {
        if (auto* cam = dynamic_cast<CameraNode*>(node.get())) {
            if (cam != m_selectedNode) {
                cam->isActive = false;
            }
        }
    }
}

// ========== Toolbar ==========

void MainWindow::onAddNode(QString name, NodeType type)
{
    std::string nodeName = name.toStdString();

    if (type == NodeType::Model) {
        QString path = QFileDialog::getOpenFileName(
            this, tr("Open Model"),
            QString(),
            tr("3D Models (*.obj *.fbx *.gltf *.glb);;All Files (*)"));
        if (path.isEmpty()) return;
        nodeName = path.toStdString();
    }

    if (type == NodeType::Model) m_viewport->makeCurrent();
    auto node = NodeFactory::createNode(nodeName, type, m_viewport->gl());
    if (type == NodeType::Model) m_viewport->doneCurrent();

    if (node) {
        if (type != NodeType::Model) {
            node->m_name = name.toStdString();
        }
        m_viewport->getScene().getRoot()->addChild(node);
        m_viewport->update();
        emit m_viewport->sceneStructureChanged();
        ui->logOutput->append(
            QString("Added node: %1 (%2)").arg(name).arg(
                type == NodeType::Model ? "Model" :
                type == NodeType::Camera ? "Camera" :
                type == NodeType::Light ? "Light" : "Empty"));
    } else {
        ui->logOutput->append("Failed to create node.");
    }
}

void MainWindow::onWireframe(bool checked)
{
    // checked = true; // 强制开启线框模式
    std::cout << "Wireframe: " << (checked ? "ON" : "OFF") << std::endl;
    m_viewport->setWireframe(checked);
    ui->logOutput->append(
        QString("Wireframe: %1").arg(checked ? "ON" : "OFF"));
}

void MainWindow::onResetCamera()
{
    m_viewport->resetCamera();
    ui->logOutput->append("Camera reset.");
}
