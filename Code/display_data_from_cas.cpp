#include "display_data_from_cas.h"
#include <sstream>
#include <QtCore/QVariant>
#include <QtCore/QString>

display_data_from_cas::display_data_from_cas(string ticketurl,string ticket,string dataurl,string data_to_display,string session_id,QWidget *parent): QDialog(parent)
{
	//parent_dyn_s_d=parent;
	resize(800,600);
	QGridLayout* layout = new QGridLayout();
	setLayout(layout);
	QGroupBox* groupboxOptions = new QGroupBox(tr("CAS informations"), this);
	layout->addWidget(groupboxOptions, 0, 0, 3, 3);

	QGridLayout* layoutOptions = new QGridLayout(groupboxOptions);
	
	QLabel* ticketurl_label = new QLabel(QString("ticket url :")+ ticketurl.c_str(), groupboxOptions);
	ticketurl_label->setDisabled(false);
	layoutOptions->addWidget(ticketurl_label, 0,0,Qt::AlignCenter);
	//Split Data
	QLabel* ticket_label = new QLabel(QString("ticket :")+ ticket.c_str(), groupboxOptions);
	ticket_label->setDisabled(false);
	layoutOptions->addWidget(ticket_label, 1, 0,Qt::AlignCenter);
	
	QLabel* data_label = new QLabel(QString("data :")+ data_to_display.c_str(), groupboxOptions);
	data_label->setDisabled(false);
	layoutOptions->addWidget(data_label, 2, 0,Qt::AlignCenter);
	
	QLabel* data_url_label = new QLabel(QString("workflow url:")+ dataurl.c_str(), groupboxOptions);
	data_url_label->setDisabled(false);
	layoutOptions->addWidget(data_url_label, 3, 0,Qt::AlignCenter);
	
	QLabel* session_id_label = new QLabel(QString("session id:")+ session_id.c_str(), groupboxOptions);
	session_id_label->setDisabled(false);
	layoutOptions->addWidget(session_id_label, 4, 0,Qt::AlignCenter);
	
	//labelallfeatures = new QLabel(tr(("ticketurl "+ticketurl+" ticket "+ticket+" dataurl "+dataurl+" datatodisplay "+data_to_display+" session_id "+session_id+"You have choosed: ").c_str()), groupboxOptions);
	//labelallfeatures->setDisabled(false);
	//layoutOptions->addWidget(labelallfeatures, 1,2,Qt::AlignCenter);

	
	exec();
}


