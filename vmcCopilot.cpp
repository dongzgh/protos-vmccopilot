#include "vmcCopilot.h"
#include "ui_vmcCopilot.h"

#include "vmcCopilotNetwork.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>

vmcCopilot::vmcCopilot(QWidget *parent) :
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
  m_networkManager->sendRequest(m_url, m_key, question);
}

void vmcCopilot::slot_responseReceived(const QString& response)
{
  QJsonDocument jsonDoc = QJsonDocument::fromJson(response.toUtf8());
  QJsonObject jsonObj = jsonDoc.object();
  QJsonArray answersArray = jsonObj["answers"].toArray();
  QJsonObject answerObj = answersArray[0].toObject();
  QString answer = answerObj["answer"].toString();
  m_answer = answer;
  QTextDocument document;
  document.setMarkdown(answer);
  answer = document.toHtml();
  m_ui->wvOutput->setHtml(answer);
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
  } // Handle ui data requests.
  else if(request.url().toString().contains("data:ui"))
  {
    request.reject();
    QString ui = getUi(request.url().toString());
    emit signal_navigationRequested(request, ui);
    return;
  } // Handle code data requests.
  else if(request.url().toString().contains("data:code"))
  {
    request.reject();
    QString code = getCode();
    emit signal_navigationRequested(request, code);
    return;
  } // Reject all other requests.
  else
  {
    request.reject();
    return;
  }
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
  QRegularExpressionMatchIterator iterator = re.globalMatch(m_answer);
  QRegularExpressionMatch match = iterator.next();
  code = match.captured(1).trimmed();

  return code;
}