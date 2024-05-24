#include "vmcCopilot.h"
#include "ui_vmcCopilot.h"

#include "vmcCopilotNetwork.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

vmcCopilot::vmcCopilot(QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_unique<Ui::vmcCopilot>())
{
  m_ui->setupUi(this);

  m_networkManager = std::make_unique<vmcCopilotNetwork>(this);

  connect(m_ui->leInput, &QLineEdit::returnPressed, this, &vmcCopilot::sendRequest);
  connect(m_networkManager.get(), &vmcCopilotNetwork::responseReceived, this, &vmcCopilot::slot_ResponseReceived);
  connect(m_ui->wvOutput->page(), &QWebEnginePage::navigationRequested, this, &vmcCopilot::slot_NavigationRequested);
}

vmcCopilot::~vmcCopilot()
{
}

void vmcCopilot::sendRequest()
{
  QString question = m_ui->leInput->text();
  m_networkManager->sendRequest(m_url, m_key, question);
}

void vmcCopilot::slot_ResponseReceived(const QString& response)
{
  QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
  QJsonObject jsonObj = jsonDoc.object();
  QJsonArray answersArray = jsonObj["answers"].toArray();
  QJsonObject answerObj = answersArray[0].toObject();
  QString answer = answerObj["answer"].toString();
  QTextDocument textDoc;
  textDoc.setMarkdown(answer);
  answer = textDoc.toHtml();
  m_ui->wvOutput->setHtml(answer);
}

void vmcCopilot::slot_NavigationRequested(QWebEngineNavigationRequest& request)
{
  qDebug() << request.url().toString();

  // Accept conventional navigation requests.
  if (request.url().toString().contains("data:text/html") ||
    request.url().toString().contains("http://") ||
    request.url().toString().contains("https://")) {
    request.accept();
    return;
  } // Accept data:ui and data:code navigation requests.
  else if(request.url().toString().contains("data:ui") ||
    request.url().toString().contains("data:code"))
  {
    request.reject();
    emit signal_navigationRequested(request);
    return;
  } // Reject all other requests.
  else
  {
    request.reject();
    return;
  }
}