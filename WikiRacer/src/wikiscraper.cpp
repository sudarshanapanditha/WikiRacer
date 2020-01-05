#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include "wikiscraper.h"
#include "error.h"

using std::cout;            using std::endl;
using std::cerr;            using std::string;
using std::unordered_map;   using std::unordered_set;




/* Part A Implementation Begins */

/* Constants */
const unordered_set<char> disallowedCharacters {'#', ':'}; // set of disallowed characters

/*
 * This function takes an html page as string returns an
 * unordered map of valid wikipedia links
*/
unordered_set<string> findWikiLinks(const string & pageHtml) {
    unordered_set<string> wikiLinks {};

    const string wikiLinkStart = "wiki/";
    const char wikiLinkEnd = '"';

    auto next = std::search(pageHtml.begin(), pageHtml.end(), wikiLinkStart.begin(), wikiLinkStart.end());
    while(next != pageHtml.end()) {
        auto start = next + wikiLinkStart.size(); // ignoring the "wiki/" string
        auto end = std::find(start, pageHtml.end(), wikiLinkEnd); // find the end of the link
        bool isValid = std::all_of(start, end, [](char c){return !disallowedCharacters.count(c);});
        if(isValid){ // valid link found
            wikiLinks.insert(string{start, end});
        }
        next = std::search(end + 1, pageHtml.end(), wikiLinkStart.begin(), wikiLinkStart.end());
    }
    return wikiLinks;
}

/* Part A Implementation Ends */


/*
 * ==================================================================================
 * |                           DON"T EDIT ANYTHING BELOW HERE                       |
 * ==================================================================================
 */
unordered_set<string> WikiScraper::getLinkSet(const string& page_name) {
    if(linkset_cache.find(page_name) == linkset_cache.end()) {
        auto links = findWikiLinks(getPageSource(page_name));
        linkset_cache[page_name] = links;
    }
    return linkset_cache[page_name];
}


WikiScraper::WikiScraper() {
    (void)getPageSource("Main_Page");
#ifdef _WIN32
    // define something for Windows
    system("cls");
#else
    // define it for a Unix machine
    system("clear");
#endif
}


string createPageUrl(const string& page_name) {
    return "https://en.wikipedia.org/wiki/" + page_name;
}

void notFoundError(const string& msg, const string& page_name, const string& url) {
    const string title = "    AN ERROR OCCURED DURING EXECUTION.    ";
    const string border(title.size() + 4, '*');
    cerr << endl;
    errorPrint(border);
    errorPrint("* " + title + " *");
    errorPrint(border);
    errorPrint();
    errorPrint("Reason: " + msg);
    errorPrint();
    errorPrint("Debug Information:");
    errorPrint();
    errorPrint("\t- Input parameter: " + page_name);
    errorPrint("\t- Attempted url: " + url);
    errorPrint();
}

string WikiScraper::getPageSource(const string &page_name) {
    const static string not_found = "Wikipedia does not have an article with this exact name.";
    if(page_cache.find(page_name) == page_cache.end()) {
        QUrl url(createPageUrl(page_name).c_str()); // need c string to convert to QString

        QNetworkRequest request(url);

        QNetworkReply *reply(manager.get(request));

        QEventLoop loop;
        QObject::connect(reply, SIGNAL(finished()) , &loop, SLOT(quit()));
        loop.exec();

        string ret = reply->readAll().toStdString();
        if(std::search(ret.begin(), ret.end(), not_found.begin(), not_found.end()) != ret.end()){
            notFoundError("Page does not exist!", page_name, url.toString().toStdString());
            return "";
        }
        size_t indx = ret.find("plainlinks hlist navbar mini");
        if(indx != string::npos) {
            return ret.substr(0, indx);
        }
        page_cache[page_name] = ret;
    }
    return page_cache[page_name];
}





