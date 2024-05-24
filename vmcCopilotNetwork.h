#pragma once

#include <QNetworkAccessManager>
#include <QObject>

class vmcCopilotNetwork : public QObject {
  Q_OBJECT

public:
  explicit vmcCopilotNetwork(QObject* parent = nullptr);
  void sendRequest(const QString& url, const QString& key, const QString& question);

signals:
  void responseReceived(const QString& response);

private slots:
  void replyFinished(QNetworkReply* reply);

private:
  std::unique_ptr<QNetworkAccessManager> m_manager;
};