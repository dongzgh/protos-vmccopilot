#include "vmcCopilot.h"
#include "ui_vmcCopilot.h"

#include "vmcCopilotNetwork.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

vmcCopilot::vmcCopilot(QWidget *parent) :
    QDialog(parent),
    m_ui(std::make_unique<Ui::vmcCopilot>())
{
  m_ui->setupUi(this);

  m_networkManager = std::make_unique<vmcCopilotNetwork>(this);

  connect(m_ui->leInput, &QLineEdit::returnPressed, this, &vmcCopilot::sendRequest);
  connect(m_networkManager.get(), &vmcCopilotNetwork::responseReceived, this, &vmcCopilot::responseReceived);
}

vmcCopilot::~vmcCopilot()
{
}

void vmcCopilot::sendRequest()
{
  QString question = m_ui->leInput->text();
  m_networkManager->sendRequest(m_url, m_key, question);
}

void vmcCopilot::responseReceived(const QString& response)
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