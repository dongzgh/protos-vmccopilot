#include "vmcCopilot.h"
#include "ui_vmcCopilot.h"

#include "vmcCopilotNetwork.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QFile>
#include <QDebug>

vmcCopilot::vmcCopilot(QWidget* parent) :
  QDialog(parent),
  m_ui(std::make_unique<Ui::vmcCopilot>())
{
  m_ui->setupUi(this);

  m_networkManager = std::make_unique<vmcCopilotNetwork>(this);

  connect(m_ui->leInput, &QLineEdit::returnPressed, this, &vmcCopilot::sendRequest);
  connect(m_networkManager.get(), &vmcCopilotNetwork::signal_responseReceived, this, &vmcCopilot::slot_responseReceived);
  connect(m_ui->wvOutput->page(), &QWebEnginePage::navigationRequested, this, &vmcCopilot::slot_navigationRequested);
}

vmcCopilot::~vmcCopilot()
{
}

void vmcCopilot::sendRequest()
{
  QString question = m_ui->leInput->text();
  m_message.clear();
  if (question.startsWith("@oa"))
  {
    m_model = ENGINE::OA;
    m_networkManager->sendRequest(m_oaUrl, m_oaKey, m_oaContext, question);
  }    
  else
  {
    m_model = ENGINE::CS;
    m_networkManager->sendRequest(m_csUrl, m_csKey, question);
  }
}

void vmcCopilot::slot_responseReceived(const QString& response)
{
  QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
  QJsonObject json = jsonDoc.object();
  if (m_model == ENGINE::OA)
  {
    QJsonArray choices = json["choices"].toArray();
    QJsonObject choice = choices[0].toObject();
    QJsonObject message = choice["message"].toObject();
    QString content = message["content"].toString();
    m_message = content;
  }
  else if (m_model == ENGINE::CS)
  {
    QJsonArray answers = json["answers"].toArray();
    QJsonObject answer = answers[0].toObject();
    QString message = answer["answer"].toString();
    QJsonObject dialog = answer["dialog"].toObject();
    QJsonArray prompts = dialog["prompts"].toArray();
    if (prompts.size() > 0)
    {
      for (auto prompt : prompts)
      {
        QJsonObject value = prompt.toObject();
        QString displayText = value["displayText"].toString();
        QString encodedDisplayText = QUrl::toPercentEncoding(displayText);
        message.append("\n\n[" + displayText + "](data:prompt;displayText=" + encodedDisplayText + ")");
      }
    }
    m_message = message;
  }
  
  QTextDocument textDoc;
  textDoc.setMarkdown(m_message);
  QString message = textDoc.toHtml();
  QTextDocument htmlDoc;
  QFile file(":/styles/default.css");
  file.open(QIODevice::ReadOnly);
  QString styleSheet = file.readAll();
  file.close();
  htmlDoc.setDefaultStyleSheet(styleSheet);
  htmlDoc.setHtml(message);
  QString htmlAnaswer = htmlDoc.toHtml();
  m_ui->wvOutput->setHtml(htmlAnaswer);
}

void vmcCopilot::slot_navigationRequested(QWebEngineNavigationRequest& request)
{
  qDebug() << request.url().toString();

  // Accept conventional navigation requests.
  if (request.url().toString().contains("data:text/html") ||
    request.url().toString().contains("http://") ||
    request.url().toString().contains("https://")) {
    request.accept();
    return;
  }
  // Handle prompt data requests.
  else if (request.url().toString().contains("data:prompt"))
  {
    request.reject();
    QString prompt = getPrompt(request.url().toString());
    m_ui->leInput->setText(prompt);
    sendRequest();
    return;
  }
  // Handle ui data requests.
  else if (request.url().toString().contains("data:ui"))
  {
    request.reject();
    QString ui = getUi(request.url().toString());
    emit signal_navigationRequested(request, ui);
    return;
  }
  // Handle code data requests.
  else if (request.url().toString().contains("data:code"))
  {
    request.reject();
    QString code = getCode();
    emit signal_navigationRequested(request, code);
    return;
  }
  // Reject all other requests.
  else
  {
    request.reject();
    return;
  }
}

QString vmcCopilot::getPrompt(QString url)
{
  // Initialize the ui array.
  QString prompt;

  // Get ui parts.
  QStringList parts = url.split(";");
  parts.removeFirst();
  prompt = parts[0].split("=")[1];
  prompt = QUrl::fromPercentEncoding(prompt.toUtf8());

  return prompt;
}

QString vmcCopilot::getUi(QString url)
{
  // Initialize the ui array.
  QString ui;

  // Get ui parts.
  QStringList parts = url.split(";");
  parts.removeFirst();
  ui = parts.join(";");

  return ui;
}

QString vmcCopilot::getCode()
{
  // Initialize the code string.
  QString code;

  // Define a regular expression to find code blocks.
  QRegularExpression re("`{3}python(.*?)`{3}", QRegularExpression::DotMatchesEverythingOption);
  QRegularExpressionMatchIterator iterator = re.globalMatch(m_message);
  QRegularExpressionMatch match = iterator.next();
  code = match.captured(1).trimmed();

  return code;
}