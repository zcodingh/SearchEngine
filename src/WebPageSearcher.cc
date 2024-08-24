#include "../include/WebPageSearcher.h"
#include "../include/WebPageQuery.h"
#include "../include/Configuration.h"
#include "../include/SplitTool.h"
#include <fstream>
#include <sstream>

WebPageSearcher::WebPageSearcher(const string& query, const TcpConnectionPtr& con)
: _query(query)
, _con(con)
{}

void WebPageSearcher::doQuery() {
    WebPageQuery::getInstance().cut(_query);
    WebPageQuery::getInstance().doQuery(_query, _con);
}


string WebPageSearcher::getQuery() {
    return _query;
}

