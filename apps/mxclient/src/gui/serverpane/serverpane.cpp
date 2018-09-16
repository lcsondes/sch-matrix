/*
 *  This file is part of SCH Matrix.
 *
 *  SCH Matrix is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SCH Matrix is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SCH Matrix. If not, see <https://www.gnu.org/licenses/>.
 */

#include "serverpane.h"
#include "libmnp4/init.h"
#include "libmnp4/packets/mptime.h"
#include "libmnp4/core/mceventdispatcher.h"
#include "libmnp4/core/mcconfig.h"

const int ServerPane::TIMEOUT;
const QString ServerPane::SERVER_IP_QUERY("mxclient.server_ip");

ServerPane::ServerPane(QWidget* parent/*=NULL*/)
    :QWidget(parent)
{
    layout_=new QGridLayout(this);
    ipLabel_=new QLabel(tr("IP:"),this);
    layout_->addWidget(ipLabel_,0,0,1,1);
    ip_=new QLineEdit(this);
    ip_->setInputMask("009.009.009.009;_");
    struct IPValidator : public QValidator
    {
        IPValidator(QObject* parent=NULL):QValidator(parent){}
        State validate(QString& input,int&) const
        {
            foreach(QString n,input.split('.'))
            {
                if(n.toInt()>255)return Invalid;
            }
            QHostAddress a;
            if(!a.setAddress(input))
            {
                return Intermediate;
            }
            return Acceptable;
        }
    };
    ip_->setValidator(new IPValidator(this));
    
    ip_->setText(MCConfig::query(SERVER_IP_QUERY).toString());
    
    layout_->addWidget(ip_,0,1,1,1);
    connect_=new QPushButton(tr("Connect"),this);
    layout_->addWidget(connect_,0,2,1,1);
    status_=new QLabel(tr("Not connected"),this);
    layout_->addWidget(status_,1,0,1,3);

    connect(connect_,SIGNAL(clicked()),this,SLOT(connectClicked()));

    refreshTick_=new QTimer(this);
    lastResponse_.start();

    connect(refreshTick_,SIGNAL(timeout()),this,SLOT(refreshStatus()));
    MCEventDispatcher::instance()->connect<MPTime>(this,SLOT(incomingTime()));
}

void ServerPane::connectClicked()
{
    QHostAddress a;
    if(a.setAddress(ip_->text()))
    {
        libmnp_start_client(a);
        ip_->setEnabled(false);
        connect_->setEnabled(false);
        refreshTick_->start(100);
        emit connected(a);
    }
}

void ServerPane::refreshStatus()
{
    status_->setText(lastResponse_.elapsed()>TIMEOUT?
        tr("Last keepalive was %n ms ago",NULL,lastResponse_.elapsed()):
        tr("Connection OK"));
}

void ServerPane::incomingTime()
{
    lastResponse_.start();
}
