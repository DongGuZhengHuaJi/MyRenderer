#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "window/RenderViewport.h"
#include "scene/SceneNode.h"

#include <QFileDialog>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
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

    setupInspector();

    // 信号/槽
    connect(m_viewport, &RenderViewport::sceneChanged,
            this, &MainWindow::onSceneChanged);
    connect(ui->sceneTree, &QTreeWidget::itemClicked,
            this, &MainWindow::onTreeItemClicked);
    connect(ui->sceneTree, &QTreeWidget::customContextMenuRequested,
            this, &MainWindow::onTreeContextMenuRequested);
    connect(ui->actionOpenModel, &QAction::triggered,
            this, &MainWindow::onOpenModel);
    connect(ui->actionWireFrame, &QAction::toggled,
            this, &MainWindow::onWireframe);
    connect(ui->actionResetCamera, &QAction::triggered,
            this, &MainWindow::onResetCamera);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// --- Inspector ---

void MainWindow::setupInspector()
{
    m_inspectorContent = ui->inspector->widget();
    if (!m_inspectorContent) {
        m_inspectorContent = new QWidget();
        ui->inspector->setWidget(m_inspectorContent);
    }

    m_inspectorLayout = new QFormLayout(m_inspectorContent);
    m_inspectorLayout->setLabelAlignment(Qt::AlignLeft);

    auto makeSpinBox = [](double min, double max, double step, double value) {
        auto* sb = new QDoubleSpinBox();
        sb->setRange(min, max);
        sb->setSingleStep(step);
        sb->setDecimals(3);
        sb->setValue(value);
        sb->setMinimumWidth(80);
        return sb;
    };

    auto* posGroup = new QGroupBox(tr("Position"));
    auto* posLayout = new QFormLayout(posGroup);
    m_spinPosX = makeSpinBox(-999, 999, 0.1, 0);
    m_spinPosY = makeSpinBox(-999, 999, 0.1, 0);
    m_spinPosZ = makeSpinBox(-999, 999, 0.1, 0);
    posLayout->addRow("X", m_spinPosX);
    posLayout->addRow("Y", m_spinPosY);
    posLayout->addRow("Z", m_spinPosZ);
    m_inspectorLayout->addRow(posGroup);

    auto* rotGroup = new QGroupBox(tr("Rotation"));
    auto* rotLayout = new QFormLayout(rotGroup);
    m_spinRotX = makeSpinBox(-360, 360, 1, 0);
    m_spinRotY = makeSpinBox(-360, 360, 1, 0);
    m_spinRotZ = makeSpinBox(-360, 360, 1, 0);
    rotLayout->addRow("X", m_spinRotX);
    rotLayout->addRow("Y", m_spinRotY);
    rotLayout->addRow("Z", m_spinRotZ);
    m_inspectorLayout->addRow(rotGroup);

    auto* sclGroup = new QGroupBox(tr("Scale"));
    auto* sclLayout = new QFormLayout(sclGroup);
    m_spinSclX = makeSpinBox(0.01, 999, 0.1, 1);
    m_spinSclY = makeSpinBox(0.01, 999, 0.1, 1);
    m_spinSclZ = makeSpinBox(0.01, 999, 0.1, 1);
    sclLayout->addRow("X", m_spinSclX);
    sclLayout->addRow("Y", m_spinSclY);
    sclLayout->addRow("Z", m_spinSclZ);
    m_inspectorLayout->addRow(sclGroup);

    m_inspectorLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    auto connectSpin = [this](QDoubleSpinBox* sb) {
        connect(sb, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                this, &MainWindow::onTransformEdited);
    };
    connectSpin(m_spinPosX); connectSpin(m_spinPosY); connectSpin(m_spinPosZ);
    connectSpin(m_spinRotX); connectSpin(m_spinRotY); connectSpin(m_spinRotZ);
    connectSpin(m_spinSclX); connectSpin(m_spinSclY); connectSpin(m_spinSclZ);

    m_inspectorContent->setEnabled(false);
}

// --- 场景树 ---

void MainWindow::rebuildSceneTree()
{
    ui->sceneTree->clear();
    m_selectedNode = nullptr;
    m_inspectorContent->setEnabled(false);

    auto rootNode = m_viewport->getScene().getRoot();
    ui->sceneTree->addTopLevelItem(new QTreeWidgetItem({QString::fromStdString(rootNode->m_name)}));
    ui->sceneTree->topLevelItem(0)->setData(0, Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(rootNode.get())));

    auto addChildren = [this](QTreeWidgetItem* parentItem, const std::shared_ptr<SceneNode>& node, auto& addChildrenRef) -> void {
        for (const auto& child : node->m_children) {
            QTreeWidgetItem* childItem = new QTreeWidgetItem({QString::fromStdString(child->m_name)});
            childItem->setData(0, Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(child.get())));
            parentItem->addChild(childItem);
            addChildrenRef(childItem, child, addChildrenRef);
        }
    };
    addChildren(ui->sceneTree->topLevelItem(0), rootNode, addChildren);
}

void MainWindow::onSceneChanged()
{
    rebuildSceneTree();
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int /*column*/)
{
    if (!item) return;

    auto ptr = item->data(0, Qt::UserRole).value<quintptr>();
    auto* node = reinterpret_cast<SceneNode*>(ptr);
    m_selectedNode = node;
    m_viewport->selectModel(node); // 统一管理选中状态

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
            ui->sceneTree->removeItemWidget(item, 0);
            m_viewport->removeSelectedModel(node);
            rebuildSceneTree();
            ui->logOutput->append(
                QString("Deleted node: %1").arg(QString::fromStdString(node->m_name)));
        }
    });
    contextMenu.exec(ui->sceneTree->viewport()->mapToGlobal(pos));
}

// --- Inspector ---

void MainWindow::updateInspectorFromNode(SceneNode* node)
{
    if (!node) return;
    m_updatingInspector = true;

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

    m_updatingInspector = false;
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

// --- Toolbar ---

void MainWindow::onOpenModel()
{
    QString path = QFileDialog::getOpenFileName(
        this, tr("Open Model"),
        QString(),
        tr("3D Models (*.obj *.fbx *.gltf *.glb);;All Files (*)"));

    if (path.isEmpty()) return;

    m_viewport->loadModel(path);
}

void MainWindow::onWireframe(bool checked)
{
    m_viewport->setWireframe(checked);
    ui->logOutput->append(
        QString("Wireframe: %1").arg(checked ? "ON" : "OFF"));
}

void MainWindow::onResetCamera()
{
    m_viewport->resetCamera();
    ui->logOutput->append("Camera reset.");
}
