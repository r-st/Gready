#include "./reader.h"

Reader::Reader():m_apiUrl("http://www.google.com/reader/api/0/"), m_atomUrl("http://www.google.com/reader/atom/") {
//connect(&m_manager, SIGNAL(finished(QNetworkReply*)), SLOT(authenticated(QNetworkReply*)));
    m_signalMapper = new QSignalMapper(this);
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
    emit authenticationDone();
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
            // TODO: Process error
            qDebug() << xml.errorString();
        }
        while (!xml.atEnd()) {
            xml.readNext();
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
      emit tagsFetchingDone();
    }

    m_replies.removeAll(reply);
    reply->deleteLater();
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
    connect(reply, SIGNAL(finished()), SLOT(feedsFetched()));

    m_replies.append(reply);
}

void Reader::feedsFetched()
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
        QXmlStreamReader xml;
        xml.addData(reply->readAll());
        if (xml.error()) {
            // TODO: Process error
            qDebug() << xml.errorString();
        }

        QString id;
        QString title;
        bool category = false;
        Feed* newFeed ;

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                // feed address
                if (xml.name() == "string" && xml.attributes().value("name") == "id" && !category) {
                    id = xml.readElementText();
                    // feed title
                } else if (xml.name() == "string" && xml.attributes().value("name") == "title" && !category) {
                    title = xml.readElementText();
                    newFeed = new Feed(title, id, this);
                    m_feedList.insert(title, newFeed);
                    // category name
                } else if (xml.name() == "string" && xml.attributes().value("name") == "id" && category) {
                    newFeed->setHasCategory(true);
                    QString indexStr = "/label/";
                    QString category;
                    QString elementText = xml.readElementText();
                    int index = elementText.indexOf(indexStr);
                    category= elementText.mid(index + indexStr.length());

                    m_tagList[category]->addFeed(newFeed);
                    // start of the list of categories
                } else if (xml.name() == "list" && xml.attributes().value("name") == "categories") {
                    category = true;
                }
                // end of categories list
            } else if (xml.isEndElement() && xml.name() == "list") {
                category = false;
            }
        }
        
      emit feedsFetchingDone();
    
    }
    m_replies.removeAll(reply);
    reply->deleteLater();
    
}


void Reader::getArticlesFromFeed(QString feedName)
{
    QString url = m_atomUrl;
    if (!m_feedList.contains(feedName)) {
        // TODO: error handling
        qDebug() << "Error: bad feed name";
    } else {
        url.append(m_feedList[feedName]->getID());
        qint64 timestamp = QDateTime::currentMSecsSinceEpoch()/1000;

        QUrl request(url);
        request.addQueryItem(QString("ck"), QString::number(timestamp));
        request.addQueryItem(QString("client"), m_settings.applicationName());

        if (!m_feedList[feedName]->getContinuation().isEmpty()) {
            request.addQueryItem(QString("c"), m_feedList[feedName]->getContinuation());
        }


        QNetworkReply* reply = m_manager.get(setAuthHeader(QNetworkRequest(request)));
        //connect(reply, SIGNAL(finished()), SLOT(articlesFromFeedFinished()));
        connect(reply, SIGNAL(finished()), m_signalMapper, SLOT(map()));

        m_signalMapper->setMapping(reply, feedName);
        connect(m_signalMapper, SIGNAL(mapped(QString)), this, SLOT(articlesFromFeedFinished(QString)));

        m_replies.append(reply);
    }
}
void Reader::articlesFromFeedFinished(QString feedName)
{

    QNetworkReply* reply;

    for (uint i = 0; i < m_replies.count(); i++) {
        if (m_replies[i]->isFinished()) {
            reply =m_replies[i];
            break;
        }
    }

    //qDebug() << feedName;
    if (reply->error()) {
        // TODO: Process error
        qDebug() << "Tag: " << reply->errorString();
    } else {
        Feed* feed = m_feedList[feedName];
        QXmlStreamReader xml;
        xml.addData(reply->readAll());

        QDateTime published;
        QUrl articleUrl;
        QString author;
        bool inEntry = false;
        QMimeData articleContent;
        bool isRead = false;
        bool isStarred = false;
        bool isShared = false;
        QString articleId;
        QString title;

        while (!xml.atEnd()) {
            xml.readNext();
            if (xml.isStartElement()) {
                // set continuation string
                if (xml.name() == "continuation") {
                    feed->setContinuation(xml.readElementText());
                }

                // start of article entry
                else if (xml.name() == "entry") {
                    inEntry = true;
                }
                // process article entry
                else if (inEntry) {
                    if (xml.name() == "link" ) {
                        articleUrl = xml.attributes().value("href").toString();    // url of original article
                    }
                    else if (xml.name() == "source") {
                        xml.skipCurrentElement();    // skip <source> ... </source>, nothing important here
                    }
                    // google id of article
                    else if (xml.name() == "id") {
                        int index;
                        articleId = xml.readElementText();
                        index = articleId.lastIndexOf(":");
                        articleId = articleId.mid(index+1);
                    } else if (xml.name() == "title") {
                        title = xml.readElementText();
                    }
                    else if (xml.name() == "published") {
                        published = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
                    }
                    else if (xml.name() == "name") {
                        author = xml.readElementText();
                    }
                    else if (xml.name() == "summary") {
                        articleContent.setHtml(xml.readElementText());
                    }
                    else if (xml.name() == "category") {
                        if (xml.attributes().value("label") == "read") {
                            isRead = true;
                        }
                        else if (xml.attributes().value("label") == "starred") {
                            isStarred = true;
                        }
                        else if (xml.attributes().value("label") == "broadcast") {
                            isShared = true;
                        }
                    }
                }
                // end of article entry
            } else if (xml.isEndElement() && xml.name() == "entry") {

                Article* newArticle = new Article(title, author, articleId, articleUrl, articleContent, published, isRead, isStarred, isShared);
                m_feedList[feedName]->addArticle(articleId, newArticle);
                inEntry = false;
                title = author = articleId = "";
                articleUrl.clear();
                articleContent.clear();
                isRead = isShared = isStarred = false;
            }
        }
        
      emit articlesFetchingDone(m_feedList[feedName]);
      
    }
    m_replies.removeAll(reply);
    reply->deleteLater();
    
}


#include "reader.moc"

