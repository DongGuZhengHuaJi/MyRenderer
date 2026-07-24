#include "AddNodeDialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <qdialog.h>
#include <QMessageBox>
AddNodeDialog::AddNodeDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Add Node"));
    setModal(true);
    resize(320, 180);

    m_nameEdit = new QLineEdit("New Node");

    m_typeCombo = new QComboBox;
    m_typeCombo->addItem("Empty");
    m_typeCombo->addItem("Model");
    m_typeCombo->addItem("Camera");
    m_typeCombo->addItem("Light");

    auto* form = new QFormLayout;
    form->addRow(tr("Name:"), m_nameEdit);
    form->addRow(tr("Type:"), m_typeCombo);

    auto* buttons =
        new QDialogButtonBox(
            QDialogButtonBox::Ok |
            QDialogButtonBox::Cancel);

    connect(buttons, &QDialogButtonBox::accepted,
            this, &QDialog::accept);

    connect(buttons, &QDialogButtonBox::rejected,
            this, &QDialog::reject);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addStretch();
    layout->addWidget(buttons);

    setLayout(layout);
}

QString AddNodeDialog::nodeName() const
{
    return m_nameEdit->text();
}

NodeType AddNodeDialog::nodeType() const
{
    switch (m_typeCombo->currentIndex())
    {
    case 0: return NodeType::Empty;
    case 1: return NodeType::Model;
    case 2: return NodeType::Camera;
    case 3: return NodeType::Light;
    default: return NodeType::Empty;
    }
}

void AddNodeDialog::accept() {
    QString name = m_nameEdit->text();

    if (name.isEmpty())
    {
        QMessageBox::warning(this,
                             "Warning",
                             "Node name cannot be empty.");
        return;
    }

    emit addNode(name,nodeType());

    QDialog::accept();
}
