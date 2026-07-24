#pragma once

#include <QDialog>
#include "scene/NodeFactory.h"

class QLineEdit;
class QComboBox;
class QLabel;

class AddNodeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddNodeDialog(QWidget* parent = nullptr);

    QString nodeName() const;
    NodeType nodeType() const;

    void accept() override;


signals:
    void addNode(QString,NodeType type);
private:
    QLineEdit* m_nameEdit;
    QComboBox* m_typeCombo;
};