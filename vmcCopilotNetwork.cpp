#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QUrl>
#include <QObject>
#include <QDebug>

#include "vmcCopilotNetwork.h"

vmcCopilotNetwork::vmcCopilotNetwork(QObject* parent) :
  QObject(parent)
{
  m_manager = std::make_unique<QNetworkAccessManager>(this);
  connect(m_manager.get(), &QNetworkAccessManager::finished, this, &vmcCopilotNetwork::slot_replyFinished);
}

void vmcCopilotNetwork::sendRequest(const QString& url, const QString& key, const QString& question)
{
  // Create request object.
  QNetworkRequest request;

  // Set request URL.
  request.setUrl(QUrl(url));

  // Set request headers.
  request.setRawHeader("Ocp-Apim-Subscription-Key", key.toUtf8());
  request.setRawHeader("Content-Type", "application/json");

  // Set request body.
  QByteArray postData;
  postData = QString("{\"question\": \"" + question + "\"}").toUtf8();

  // Send request.
  m_manager->post(request, postData);
}

void vmcCopilotNetwork::sendRequest(const QString& oaUrl, const QString& oakey, const QJsonObject& oaContext, const QString& question)
{
  // Create request object.
  QNetworkRequest request;

  // Set request URL.
  request.setUrl(QUrl(oaUrl));

  // Set request headers.
  request.setRawHeader("Content-Type", "application/json");
  request.setRawHeader("api-key", oakey.toUtf8());

  // Clone oaContext object.
  QJsonObject oaLocalContext = oaContext;

  // Get message from oaContext as array.
  QJsonArray messages = oaLocalContext["messages"].toArray();
  QJsonObject message;
  message["content"] = question;
  message["role"] = "user";
  messages.append(message);
  oaLocalContext["messages"] = messages;

  // Set request body.
  QByteArray postData;
  QJsonDocument jsonDoc(oaLocalContext);
  postData = jsonDoc.toJson();

  // Send request.
  m_manager->post(request, postData);
}

void vmcCopilotNetwork::slot_replyFinished(QNetworkReply* reply)
{
  QString response;

  if (reply->error()) {
    response = reply->errorString();
    reply->deleteLater();
  }
  else {
    response = reply->readAll();
    reply->deleteLater();
    emit signal_responseReceived(response);
  }
}
