#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QUrl>
#include <QObject>
#include <QDebug>

#include "vmcCopilotNetwork.h"

vmcCopilotNetwork::vmcCopilotNetwork(QObject* parent) :
  QObject(parent)
{
  m_manager = std::make_unique<QNetworkAccessManager>(this);
  connect(m_manager.get(), &QNetworkAccessManager::finished, this, &vmcCopilotNetwork::replyFinished);
}

void vmcCopilotNetwork::sendRequest(const QString& url, const QString& key, const QString& question) 
{
  // Create request object.
  QNetworkRequest request;

  // Set request URL.
  request.setUrl(QUrl(url));

  // Set request headers.
  // Convert key to QByteArray.
  request.setRawHeader("Ocp-Apim-Subscription-Key", key.toUtf8());
  request.setRawHeader("Content-Type", "application/json");

  // Set request body.
  QByteArray postData;
  postData = QString("{\"question\": \"" + question + "\"}").toUtf8();

  // Send request.
  m_manager->post(request, postData);
}

void vmcCopilotNetwork::replyFinished(QNetworkReply* reply) 
{
  QString response;

  if (reply->error()) {
    response = reply->errorString();
  }
  else {    
    response = reply->readAll();
  }

  reply->deleteLater();
  emit responseReceived(response);  
}
