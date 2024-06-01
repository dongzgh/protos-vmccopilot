#pragma once

#include <memory>

#include <QDialog>
#include <QJsonArray>
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
  void signal_navigationRequested(QWebEngineNavigationRequest& request, QString content);

protected slots:
  void slot_responseReceived(const QString& response);
  void slot_navigationRequested(QWebEngineNavigationRequest& request);;

protected:
  QString getPrompt(QString url);
  QString getUi(QString url);
  QString getCode();

private:
  QString m_url;
  QString m_key;
  QString m_answer;
  std::unique_ptr<Ui::vmcCopilot> m_ui;
  std::unique_ptr<vmcCopilotNetwork> m_networkManager;
};
