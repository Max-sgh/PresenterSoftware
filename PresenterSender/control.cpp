#include "control.h"
#include <QFile>
#include <QtXml>

Control::Control(QObject *parent) : QObject(parent)
{

}

Control::~Control() {
    /*delete _net;
    delete _ph;
    delete _pw;
    delete _hw;
    delete _fm;*/
}

void Control::saveSettings(QString filename, QString serverName, QString serverIP) {
    QDomDocument dom;
    QDomElement root = dom.createElement("settings");
    dom.appendChild(root);

    //serverName
    QDomElement name = dom.createElement("SetItem");
    name.setAttribute("ID", 0);
    name.setAttribute("Name", "serverName");
    name.setAttribute("value", serverName);
    root.appendChild(name);

    //serverIP
    QDomElement ip = dom.createElement("SetItem");
    ip.setAttribute("ID", 1);
    ip.setAttribute("Name", "serverIP");
    ip.setAttribute("value", serverIP);
    root.appendChild(ip);

    QFile f(filename);
    if (f.open(QIODevice::WriteOnly)) {
        QTextStream s(&f);
        s << dom.toString();
        f.close();
    } else {
        std::cout << "Cannot open File! - " << f.errorString().toStdString() << std::endl;
    }
}


std::vector<std::string> Control::iterateArgs(std::string origin) {
    std::vector<std::string> ret;
    std::string copy = origin;
    while(copy.length() > 0) {
        std::string s = copy.substr(0, copy.find("$$", 0));
        ret.push_back(s);
        copy.erase(0, s.length() + 2);
    }
    return ret;
}
