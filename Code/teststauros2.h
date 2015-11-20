#ifndef TESTSTAUROS2_H
#define TESTSTAUROS2_H

#include <QMainWindow>
#include <QtCore/QTime>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <string>
#include "QFileDialog"
#include "QtCore/QFile"
#include "QtCore/QTextStream"
#include "QtCore/QDebug"
#include "QtCore/QStringList"
#include "QtCore/QDateTime"
using namespace std;
class QLabel;
class QProgressBar;
class QNetworkAccessManager;
class QNetworkReply;
class QTime;
class QProcess;

class teststauros2 : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit teststauros2(string ticketurl,string ticket,string timeperiod,string dataset_url,string staticgraph,string dynamicgraph,QWidget *parent = 0);
    ~teststauros2();
    std::string GetExecutablePath();
	std::string GetExecutablePathAndMatchItWithFilename(std::string ffname);
    QByteArray data;
	QString data_from_url;
	QString mysesion_id;
	QString timeperiod;
	QString folder_with_timeperiod;
private slots:
        void slotRequestFinished(QNetworkReply *);
        void slotSetProgress(qint64, qint64);
        void sslError(QNetworkReply* reply, const QList<QSslError> &errors );
        void messageError(QNetworkReply::NetworkError error);
private:
        QString retrievedata(QNetworkRequest request);
        QString ticketretriever(QNetworkRequest request,QByteArray postData);
        QString session_id_retriever(QNetworkRequest request,QByteArray postData);
		QString gettimeperiod(QString data_from_url);
		void convert(QString htmfile,int i);
		void parseperfeature_and_save(QString feature_name,int featureIndex,bool importId,QString in_filename);
		void parseperfeature(QString in_fileName);
        QProcess *myProcess;
        QLabel *m_label;
        QProgressBar *m_progress;
        QNetworkAccessManager *m_network;
        QTime downloadTime;
        int finishedexecuted;
		
	
};

#endif // teststauros2_H
