#pragma once

#include <QDialog>

#include "vmcCopilotExport.h"

namespace Ui {
class vmcCopilot;
}

class VMCCOPILOT_DLL_EXPORT vmcCopilot : public QDialog {
  Q_OBJECT

public:
  explicit vmcCopilot(QWidget *parent = nullptr);
  ~vmcCopilot();

private:
  Ui::vmcCopilot *ui;
};
