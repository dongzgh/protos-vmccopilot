#pragma once

#include <QNetworkAccessManager>
#include <QObject>

class vmcCopilotNetwork : public QObject {
  Q_OBJECT

public:
  explicit vmcCopilotNetwork(QObject* parent = nullptr);
  void sendRequest(const QString& url, const QString& key, const QString& question);
  void sendRequest(const QString& oaUrl, const QString& oaKey, const QJsonObject& oaContext, const QString& question);

signals:
  void signal_responseReceived(const QString& response);

private slots:
  void slot_replyFinished(QNetworkReply* reply);

private:
  std::unique_ptr<QNetworkAccessManager> m_manager;
};