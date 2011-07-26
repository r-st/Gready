#include "./reader.h"

Reader::Reader():m_apiUrl("http://www.google.com/reader/api/0/") {
//connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(authenticated(QNetworkReply*)));

    getID();

}


void Reader::getID() {
    QUrl params;
    QByteArray data;

// Prepare authentication data
    params.addQueryItem("service", "reader");
    params.addEncodedQueryItem("Email", m_settings.value("username").toByteArray());
    params.addEncodedQueryItem("Passwd", m_settings.value("password").toByteArray());
    params.addEncodedQueryItem("source", QByteArray(m_settings.applicationName().toAscii()));
    params.addEncodedQueryItem("continue", "http://www.google.com/");

    data = params.encodedQuery();

    QNetworkRequest req(QUrl("https://www.google.com/accounts/ClientLogin"));

    QNetworkReply* reply =  m_manager.post(req, data);
    connect(reply, SIGNAL(finished()), SLOT(authenticated()));
    m_replies.append(reply);
}

void Reader::authenticated() {
    QNetworkReply* reply;

    for (uint i = 0; i < m_replies.count(); i++) {
        if (m_replies[i]->isFinished()) {
            reply =m_replies[i];
            break;
        }
    }

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Auth: " << reply->errorString();
    } else {
        // skip two lines
        reply->readLine();
        reply->readLine();

        // remove key and get value
        QString autVal = reply->readLine().remove(0,4);
        // delete newline
        autVal.chop(1);
        m_authKey = autVal;
    }

    m_replies.removeAll(reply);

    reply->deleteLater();
}

QNetworkRequest Reader::setAuthHeader(QNetworkRequest req) {
    req.setRawHeader(QByteArray("Authorization"), QByteArray("GoogleLogin auth").append(m_authKey));

    return req;
}

void Reader::getTags() {
    QString url = m_apiUrl;
    url.append("tag/list");
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

    QUrl request(url);
    request.addQueryItem(QString("output"),QString("xml"));
    request.addQueryItem(QString("ck"), QString::number(timestamp));
    request.addQueryItem(QString("client"), m_settings.applicationName());


    QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
    connect(reply, SIGNAL(finished()), SLOT(taglistFinished()));

    m_replies.append(reply);
}

void Reader::taglistFinished() {
    QNetworkReply* reply;

    for (uint i = 0; i < m_replies.count(); i++) {
        if (m_replies[i]->isFinished()) {
            reply =m_replies[i];
            break;
        }
    }

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Tag: " << reply->errorString();
    } else {


        QXmlStreamReader xml;
        xml.addData(reply->readAll());
        if (xml.error()) {
            qDebug() << xml.errorString();
        }
        while (!xml.atEnd()) {
            if (xml.error()) {
                qDebug() << xml.errorString();
            }
            xml.readNext();
            if (xml.error()) {
                qDebug() << xml.errorString();
            }
            if (xml.isStartElement()) {
                if (xml.name()=="string" && xml.attributes().value("name") == "id") {
                    QString elementText = xml.readElementText();
                    QString name;
                    Tag::type type;

                    // state element
                    if (elementText.contains("state/com.")) {
                        if (elementText.endsWith("starred")) {
                            type = Tag::Starred;
                            name = "Starred";
                        } else if (elementText.endsWith("broadcast")) {
                            type = Tag::Shared;
                            name = "Shared";
                        } else if (elementText.endsWith("blogger-following")) {
                            type = Tag::Following;
                            name = "Following";
                        } else {
                            type = Tag::StateUnknown;
                            QString indexStr = "/state/";
                            int index = elementText.indexOf(indexStr);
                            name = elementText.mid(index + indexStr.length());
                        }
                        // label element
                    } else {
                        type = Tag::Label;
                        QString indexStr = "/label/";
                        int index = elementText.indexOf(indexStr);
                        name = elementText.mid(index + indexStr.length());
                    }
                    // Create new tag
                    Tag* newTag = new Tag(name,elementText,type);

                    // Save it
                    m_tagList.insert(name, newTag);
                }
            }
            xml.readNext();
        }
    }
}


void Reader::getAllFeeds()
{
    QString url = m_apiUrl;
    url.append("subscription/list");
    qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

    QUrl request(url);
    request.addQueryItem(QString("output"),QString("xml"));
    request.addQueryItem(QString("ck"), QString::number(timestamp));
    request.addQueryItem(QString("client"), m_settings.applicationName());


    QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
    connect(reply, SIGNAL(finished()), SLOT(subscriptionFinished()));

    m_replies.append(reply);
}

void Reader::subscriptionFinished()
{
    QNetworkReply* reply;

    for (uint i = 0; i < m_replies.count(); i++) {
        if (m_replies[i]->isFinished()) {
            reply =m_replies[i];
            break;
        }
    }

    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Tag: " << reply->errorString();
    } else {
        qDebug() << reply->readAll();
    }
}


#include "reader.moc"

