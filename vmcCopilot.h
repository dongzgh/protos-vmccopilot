#pragma once

#include <memory>

#include <QDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QWebEngineNavigationRequest>

#include "vmcCopilotNetwork.h"
#include "vmcCopilotExport.h"

namespace Ui {
  class vmcCopilot;
}

class VMCCOPILOT_DLL_EXPORT vmcCopilot : public QDialog {
  Q_OBJECT

public: enum ENGINE {
    CS = 0,
    OA = 1
  };

public:
  explicit vmcCopilot(QWidget* parent = nullptr);
  ~vmcCopilot();
  void setCsUrl(const QString& csUrl) { m_csUrl = csUrl; };
  void setCsKey(const QString& csKey) { m_csKey = csKey; };
  void setOaUrl(const QString& oaUrl) { m_oaUrl = oaUrl; };
  void setOaKey(const QString& oaKey) { m_oaKey = oaKey; };
  void setOaContext(const QJsonObject& oaContext) { m_oaContext = oaContext; };
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
  int m_model{ ENGINE::CS };
  QString m_csUrl;
  QString m_csKey;
  QString m_oaUrl;
  QString m_oaKey;
  QJsonObject m_oaContext;
  QString m_message;
  std::unique_ptr<Ui::vmcCopilot> m_ui;
  std::unique_ptr<vmcCopilotNetwork> m_networkManager;
};
