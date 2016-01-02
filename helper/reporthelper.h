#pragma once

#include <QString>

class ReportHelper {
public:

    QString getHTML() {return html;}
    void clear();
    void appendHeader(QString text, int level=3, bool center=true);
    void appendParagraph(QString text, QString align="left");
	void appendValueTable(QString header,QList<std::pair<QString,QString>> valueList);

private:
    
    QString html;
    
};
