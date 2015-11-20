
#include "teststauros2.h"
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QUrl>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxy>
#include <QtXml/QDomDocument>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtCore/QList>
#include <QtNetwork/QSslConfiguration>
//#include <E:/VIS-Sense/Code/qt-everywhere-opensource-src-4.7.4/src/network/ssl/qsslconfiguration.h>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslCertificate>
#include <QtCore/QCoreApplication>
#include <QtCore/QRegExp>
#include <QtNetwork/QSslSocket>
#include <QtCore/QPair>
#include <fstream>



void delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

teststauros2::teststauros2(string ticketurl,string ticket,string timeperiod,string dataset_url,string staticgraph,string dynamicgraph,QWidget *parent) :
    QMainWindow(parent)//,
    //ui(new Ui::teststauros2)
{
	
    //ui->setupUi(this);
	string dataurl;
    finishedexecuted=0;

    QByteArray postData;
    postData.append("username=certh&");
    postData.append("password=$vis-certh-sense$");
    QVBoxLayout *layout = new QVBoxLayout(this);
    bool isSSLSupported = QSslSocket::supportsSsl();
	
	m_label = new QLabel();
    m_label->setText("0/0 MB");
    m_progress = new QProgressBar();
    m_progress->setRange(0, 100);

    layout->addWidget(m_label);
    layout->addWidget(m_progress);
    QWidget* widget = new QWidget(this);
    widget->setLayout(layout);
    setCentralWidget(widget);

    m_network = new QNetworkAccessManager(this);
    QNetworkRequest request;

    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
	QUrl url=QUrl(ticketurl.c_str());
	QString urlstring=url.host();
    request.setUrl(url);


    try{
        QSslConfiguration config = QSslConfiguration::defaultConfiguration();
        config.setProtocol(QSsl::SslV3);
        request.setSslConfiguration(config);
        QString myticket=ticketretriever(request,postData);
        finishedexecuted=0;

        string url_session_id_retriever=ticketurl+string(myticket.toUtf8());
		for(int i=0;i<4;i++)
		{
			if(i==0)
				dataurl=timeperiod;
			else if(i==1)
				dataurl=dataset_url;
			else if(i==2)
			{
				dataurl=staticgraph;
				
			}
			else if(i==3)
			{
				dataurl=dynamicgraph;
			}

			QByteArray postData2;
			postData2.append((string("service=")+dataurl).c_str());
			request.setUrl(QUrl((url_session_id_retriever).c_str()));
			mysesion_id=session_id_retriever(request,postData2);
			finishedexecuted=0;
			dataurl+="\?ticket=";
            dataurl+=string(mysesion_id.toUtf8());

			QUrl urlget((dataurl).c_str());
			
			
	        
			request.setUrl(urlget);
			data_from_url=retrievedata(request);
			
			QFile datafrurl("datafromurl.txt");
			datafrurl.open(QIODevice::WriteOnly | QIODevice::Text);
			QTextStream csv(&datafrurl);
			csv<<data_from_url;
			datafrurl.close();
			
			convert("datafromurl.txt",i);
			datafrurl.close();
			if(i==0)
			{
				this->timeperiod=gettimeperiod(data_from_url);
				QString timeperiod_modified_for_folder= this->timeperiod;
				timeperiod_modified_for_folder.replace(" ","");
				timeperiod_modified_for_folder.replace("-","");
				timeperiod_modified_for_folder.replace(":","");
				timeperiod_modified_for_folder.replace(",","");
				timeperiod_modified_for_folder.replace("}","");
				timeperiod_modified_for_folder.replace("\"","");
				this->folder_with_timeperiod="folder"+timeperiod_modified_for_folder;
				
				QDir dir(this->folder_with_timeperiod);
				if (!dir.exists()) {
					dir.mkpath(".");
				}
				
			}

			downloadTime.start();
			QFile file("./toust.txt");
			if(file.open(QIODevice::WriteOnly))
			{
				QTextStream out(&file);
                out << mysesion_id.toUtf8()<<endl;//<<'\n';
                out << data_from_url.toUtf8();
                file.write((dataurl+"\/"+string(mysesion_id.toUtf8())).c_str());
				file.write((string("ticket ")+ticket).c_str());

				file.close();
	            

			}
		}

	 parseperfeature("datasetdata.csv");
	 


    }
    catch(...){
        fprintf(stderr,"exoume lathos");fflush(stderr);
        
    }
}

teststauros2::~teststauros2()
{
    //delete ui;
}
QString teststauros2::retrievedata(QNetworkRequest request)
{
    //request.setRawHeader("Accept","text/csv");
	QNetworkReply *reply = m_network->get(request);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(messageError(QNetworkReply::NetworkError)));
    QObject::connect(m_network, SIGNAL(finished(QNetworkReply *)),
                     SLOT(slotRequestFinished(QNetworkReply *)));

    connect(m_network, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslError(QNetworkReply*, const QList<QSslError> &)));
    while(finishedexecuted==0)
    {
        delay();
    }
	finishedexecuted=0;
    QVariant redirectUrl;
    QVariant possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(!possibleRedirectUrl.isNull() &&
           possibleRedirectUrl != request.url().toString())
            {
                redirectUrl = possibleRedirectUrl;
            }
	QNetworkRequest request2;
	request2.setUrl(QUrl(redirectUrl.toString()));
	//request2.setRawHeader("Content-Type","text/plain");
	//request2.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    reply = m_network->get(request2);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(messageError(QNetworkReply::NetworkError)));
    QObject::connect(m_network, SIGNAL(finished(QNetworkReply *)),
                     SLOT(slotRequestFinished(QNetworkReply *)));

    connect(m_network, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslError(QNetworkReply*, const QList<QSslError> &)));
    if(finishedexecuted==0)
    {
        delay();
    }
    //qDebug()<<data;
    return QString(data);
}
QString teststauros2::ticketretriever(QNetworkRequest request,QByteArray postData)
{
    QNetworkReply *reply = m_network->post(request,postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(messageError(QNetworkReply::NetworkError)));
    QObject::connect(m_network, SIGNAL(finished(QNetworkReply *)),
                     SLOT(slotRequestFinished(QNetworkReply *)));

    connect(m_network, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslError(QNetworkReply*, const QList<QSslError> &)));
    while(finishedexecuted==0)
    {
        delay();
    }

    QRegExp actionmatch("action=\"*org");
    actionmatch.setMinimal(true);
    actionmatch.setPatternSyntax(QRegExp::Wildcard);
    int pos = actionmatch.indexIn(data);
    QByteArray localdata=data;
    QString value2;
    if ( pos > -1 )
    {
        QString value = actionmatch.cap( 0 );
        //qDebug()<<value<<pos;
        QRegExp ticketmatch("tickets*org");
        ticketmatch.setMinimal(true);
        ticketmatch.setPatternSyntax(QRegExp::Wildcard);
        int pos = ticketmatch.indexIn(value);
        if ( pos > -1 )
        {
            value2 = ticketmatch.cap( 0 ).mid(8);
            //qDebug()<<value2<<pos;
        }
    }
    return value2;
}
QString teststauros2::session_id_retriever(QNetworkRequest request,QByteArray postData)
{
    QNetworkReply *reply = m_network->post(request,postData);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(messageError(QNetworkReply::NetworkError)));
    QObject::connect(m_network, SIGNAL(finished(QNetworkReply *)),
                     SLOT(slotRequestFinished(QNetworkReply *)));

    connect(m_network, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslError(QNetworkReply*, const QList<QSslError> &)));
    while(finishedexecuted==0)
    {
        delay();
    }

    //qDebug()<<data;
    return QString(data);
}
void teststauros2::messageError(QNetworkReply::NetworkError error)
{
    //qDebug() <<error;
	//fprintf(stderr,error.errorString().to);fflush(stderr);
    //qDebug() <<"arithmos error";
}
void teststauros2::sslError(QNetworkReply* reply, const QList<QSslError> &errors )
{
    std::string sslerrors;
    foreach( const QSslError &error, errors ) {
//        fprintf(stderr, "SSL Error: " + error.errorString().toUtf8());fflush(stderr);
        sslerrors.append(error.errorString().toUtf8());
    }
    //QUrl urlleme=reply->request().url();
    //qDebug() << urlleme.host();
    //qDebug() << urlleme.port();
    reply->ignoreSslErrors();
}
void teststauros2::slotRequestFinished(QNetworkReply *reply)
{
    m_progress->setValue(0);
    //QUrl urlleme=reply->request().url();
    
    if (reply->error() > 0) {
        m_label->setText("Error number = " + reply->errorString());
		QString label=m_label->text();
//		fprintf(stderr,reply->errorString().toUtf8());fflush(stderr);
        
    }
    else {
        QByteArray datalocal=reply->readAll();
        if(datalocal.size()>0)
            data = datalocal;
        else
            return;
        
        int size= data.size();
        QString unit;
        if (size < 1024) {
            unit = "bytes";
        } else if (size < 1024*1024) {
            size /= 1024;
            unit = "kB";
        } else {
            size /= 1024*1024;
            unit = "MB";
        }
        m_label->setText("recieved "+QVariant(size).toString()+unit);
        m_progress->setValue(100);

        QFile file("./test.txt");
        if(file.open(QIODevice::WriteOnly))
        {
            
            file.write(data);
            file.close();
            

        }
        else
        {
            //qDebug()<<"Error opening file";
        }
        finishedexecuted=1;



    }
}

void teststauros2::slotSetProgress(qint64 received, qint64 total)
{
    if (total > 0 && received > 0) {
        double speed = received * 1000.0 / downloadTime.elapsed();
        QString unit;
        if (speed < 1024) {
            unit = "Bytes/s";
        } else if (speed < 1024*1024) {
            speed /= 1024;
            unit = "kB/s";
        } else {
            speed /= 1024*1024;
            unit = "MB/s";
        }
        speed=((int)(speed));
        QString speedS=" speed = "+QVariant(speed).toString()+unit;
        m_progress->setValue((int)  ((float)received/ total *100) );
        float rec=received/1024./1024;
        rec=((int)(rec*10))/10.;
        float tot=total/1024./1024;
        tot=((int)(tot*10))/10.;
        QString S;
        if(rec==(int) rec)
            S= QVariant(rec).toString()+".0 MB / "+ QVariant(tot).toString()+" MB" +speedS;
        else
            S= QVariant(rec).toString()+" MB / "+ QVariant(tot).toString()+" MB"+speedS ;
        m_label->setText(S);
        ////qDebug()<<((float)received/ total *100);
    }
}
void teststauros2::convert(QString htmfile,int i) 
{
	std::ofstream ofs("test23.txt");
	//std::string exepath=GetExecutablePath();
	QString csvfile;
	if(i==0)
	{
		csvfile="timeperiod.csv";
		return;
	}
	else if(i==1)
		csvfile="datasetdata.csv";
	else if (i==2)
	{
		csvfile=this->folder_with_timeperiod+"/clusters.clu";
        ofs<<csvfile.toUtf8().data()<<" mpikame_sto_static";
	}
	else if (i==3)
	{
		csvfile=this->folder_with_timeperiod+"/Mat_P.txt";
        ofs<<csvfile.toUtf8().data()<<" mpikame_sto_dynamic";
	}
	QFile htmf(htmfile);
	htmf.open(QIODevice::ReadOnly | QIODevice::Text);
	QTextStream htm(&htmf);
	QFile csvft("temp.txt");
	csvft.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream csvt(&csvft);
	bool add_comma=true;
	ofs<<"1";
	while (!htm.atEnd())
	{
		add_comma=true;
		QString line=htm.readLine();
		if(line.contains("<td>"))
		{
			int to=line.indexOf("</td>") ;
			int from=4;
			QString value=line.midRef(from,to-from).toString();
			//           qDebug()<<"VALUE   "<<value;
			//           break;
			csvt<<value;
			if(value.size()==0)
				add_comma=false;
		}
		else if(line.contains("</tr>"))
		{
			csvt<<endl;
			add_comma=false;
		}
		else
		{
			add_comma=false;
		}
		if(add_comma)
		{
			csvt<<",";
		}
	}
	ofs<<"2";
	csvft.close();
	QFile f2("temp.txt");
	f2.open(QIODevice::ReadOnly | QIODevice::Text);
	
	QTextStream in(&f2);

	QFile fileforwrite(csvfile);
	//fileforwrite.close();
	//fileforwrite.remove();
	if(!fileforwrite.open(QIODevice::WriteOnly | QIODevice::Text))
	{
        ofs<<"it was not opened"<<fileforwrite.errorString().toUtf8().data();
	}
	else
	{
		ofs<<"opened";
	}
	QTextStream csv(&fileforwrite);
	while (!in.atEnd())
	{
		ofs<<"3";
		QString line=in.readLine();
		line=line.midRef(0,line.size()-1).toString();
		csv<<line<<endl;
	}
	ofs<<"4";
	f2.close();
	f2.remove();
	fileforwrite.close();
	ofs.close();
	
}
void teststauros2::parseperfeature(QString in_fileName)
{
	QFile file(in_fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<<"Error: Cannot open file";
		exit(0);
	}

	QTextStream in(&file);
	QString line = in.readLine();
	QStringList feature_list=line.split(',');

	//    long long number_of_lines=0;
	//    while (!in.atEnd())
	//    {
	//        in.readLine();
	//        number_of_lines++;
	//    }
	//    qDebug()<<"Number of lines = "<<number_of_lines;
	file.close();


	for(int i=1;i<feature_list.size();i++)
	{
		qDebug()<<"Step "<<i+1<<" of "<< feature_list.size()<<" parsing feature: "<<feature_list.at(i);
		parseperfeature_and_save(feature_list.at(i),i,true,in_fileName);
	}
}


void teststauros2::parseperfeature_and_save(QString feature_name,int featureIndex,bool importId,QString in_filename)
{

	
	QFile in_file(in_filename);
	if (!in_file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qDebug()<<"Error: Cannot open file";
		exit(0);
	}
	QTextStream in(&in_file);
	in.readLine();
	QString database_name=in_filename.midRef(0,in_filename.size()-4).toString();
	QDir dir(this->folder_with_timeperiod);
	if (!dir.exists()) {
		dir.mkpath(".");
	}
	QFile out_file(this->folder_with_timeperiod+"/"+database_name+"_"+feature_name+".csv");
	if (!out_file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		qDebug()<<"Error: Cannot open file";
		exit(0);
	}
	QTextStream out(&out_file);

	long long index=0;
	while (!in.atEnd())
	{
		QString line = in.readLine();
		//ofs<<index<<"\n";
		//qDebug()<<line;
		if(index==345)
		{
			int a;
			a=45;
		}
		QStringList string_list=line.split(',');
		if((string_list.size()-1)<featureIndex)
			continue;
		
		QString feature_value= string_list.at(featureIndex);
		QString Keyvalue=string_list.at(0);
		

		if(feature_value=="" || feature_value==" ")
		{
			index++;
			continue;
		}
		

		out<<Keyvalue<<","<<feature_value<<endl;

		index++;
		
		if(index%100000==0) qDebug()<<feature_name<<" line "<< index/100000<<" of "<<151;
		//if(index==100000) break;
	}
	

}
QString teststauros2::gettimeperiod(QString data_from_url)
{
	QString timeperiod;
	int from_first=data_from_url.indexOf("ts_start") ;
	int to_first=data_from_url.indexOf("ts_end");
	int from_second=data_from_url.indexOf("start_at") ;
	int to_second=data_from_url.indexOf("end_at");
	if(from_first>0 && to_first>0)
	{
		timeperiod=data_from_url.midRef(from_first+11,19).toString()+","+data_from_url.midRef(to_first+9,19).toString();

	}
	else if(from_second>0 && to_second>0)
	{
		timeperiod=data_from_url.midRef(from_second+13,19).toString()+data_from_url.midRef(to_second+11,19).toString();
	}
	else 
		timeperiod="false";
	//QString value=line.midRef(from,to-from).toString();
	
	return timeperiod;
}

#include <windows.h>
#include <sstream>
std::string teststauros2::GetExecutablePath() {
	std::vector<char> executablePath(MAX_PATH);

	// Try to get the executable path with a buffer of MAX_PATH characters.
	DWORD result = ::GetModuleFileNameA(
		NULL, &executablePath[0], static_cast<DWORD>(executablePath.size())
		);

	// As long the function returns the buffer size, it is indicating that the buffer
	// was too small. Keep enlarging the buffer by a factor of 2 until it fits.
	while(result == executablePath.size()) {
		executablePath.resize(executablePath.size() * 2);
		result = ::GetModuleFileNameA(
			NULL, &executablePath[0], static_cast<DWORD>(executablePath.size())
			);
	}

	// If the function returned 0, something went wrong
	if(result == 0) {
		throw std::runtime_error("GetModuleFileName() failed");
	}

	// We've got the path, construct a standard string from it
	return std::string(executablePath.begin(), executablePath.begin() + result);
}

std::string teststauros2::GetExecutablePathAndMatchItWithFilename(std::string ffname){
	std::string execPath = GetExecutablePath();
	size_t found=execPath.find_last_of("\\");
	execPath = execPath.substr(0,found);
	std::stringstream ss;
	ss << execPath << "\\" << ffname;
	return ss.str();
}


