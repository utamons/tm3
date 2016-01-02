#include "reporthelper.h"
#include <QList>

void ReportHelper::clear() {
	html.clear();
}

void ReportHelper::appendHeader(QString text, int level, bool center) {
    QString h;
    QString htag= "h"+QString::number(level);
	if (center) {
        h = h + "<center>";
	}
    h = h + "<" + htag + ">" + text + "</" + htag + ">";

	if (center) {
        h = h + "</center>";
	}

	html.append(h);
}

void ReportHelper::appendParagraph(QString text, QString align) {
    html.append("<p align=\"" + align + "\">" + text + "</p>");
}

void ReportHelper::appendValueTable(QString header, QList<std::pair<QString,QString>> valueList) {
	html.append("<b>");
	appendParagraph(header,"center");
    html.append("</b><table border=1 width=100% cellspacing=0 cellpadding=5>");
	for (int i=0; i<valueList.size(); i=i+2) {
        html.append("<tr><td width=35%>" + valueList[i].first + "</td><td width=15%>" +
        valueList[i].second + "</td>");
		if ((i+1)<valueList.size()) {
            html.append("<td width=35%>" + valueList[i+1].first + "</td><td width=15%>" +
            valueList[i+1].second + "</td>");
		}
		html.append("</tr>");
	}
	html.append("</table>");
}
