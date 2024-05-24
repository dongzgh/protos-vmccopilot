#pragma once

#include <memory>

#include <QDialog>
#include <QWebEngineNavigationRequest>

#include "vmcCopilotNetwork.h"
#include "vmcCopilotExport.h"

namespace Ui {
class vmcCopilot;
}

class VMCCOPILOT_DLL_EXPORT vmcCopilot : public QDialog {
  Q_OBJECT

public:
  explicit vmcCopilot(QWidget *parent = nullptr);
  ~vmcCopilot();
  void setUrl(const QString& url) { m_url = url; };
  void setKey(const QString& key) { m_key = key; };
  void sendRequest();

signals:
  void signal_navigationRequested(QWebEngineNavigationRequest& request);

protected slots:
  void slot_ResponseReceived(const QString& response);
  void slot_NavigationRequested(QWebEngineNavigationRequest& request);;

private:
  QString m_url;
  QString m_key;
  std::unique_ptr<Ui::vmcCopilot> m_ui;
  std::unique_ptr<vmcCopilotNetwork> m_networkManager;
};
