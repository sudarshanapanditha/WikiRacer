#include <fstream>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>
#include "wikiscraper.h"


using std::cout;            using std::endl;
using std::string;          using std::vector;
using std::priority_queue;  using std::unordered_map;
using std::unordered_set;


/*
 * This function is used to find a ladder wikipedia pages
 * from the given start page to the end page
*/
vector<string> findWikiLadder(const string& start_page, const string& end_page) {
    WikiScraper scraper; // to scrape links from wikipedia pages

    auto endPageLinks = scraper.getLinkSet(end_page); // calculate end page's links only once
    auto compFn = [&scraper, &endPageLinks](vector<string> & ladder1, vector<string> & ladder2){ // comparator function
        auto page1Links = scraper.getLinkSet(ladder1.back());
        auto page2Links = scraper.getLinkSet(ladder2.back());

        int num1 {0}; // number of common links in page 1
        int num2 {0}; // number of common links in page 2
        for (auto it = endPageLinks.begin(); it != endPageLinks.end(); ++it){ // calculating number of common links
            num1 += std::count(page1Links.begin(), page1Links.end(), *it);
            num2 += std::count(page2Links.begin(), page2Links.end(), *it);
        }

        return num1 < num2;
    };

    priority_queue<vector<string>, vector<vector<string>>, decltype (compFn)> ladderQueue(compFn); // queue of ladders

    unordered_set<string> visitedLinks{}; // set of visited links

    ladderQueue.push({start_page}); // initialize the queue of ladders with the start page

    while(!ladderQueue.empty()){
        auto ladder = ladderQueue.top();
        ladderQueue.pop(); // remove the highest priority ladder

        auto links = scraper.getLinkSet(ladder.back());
        visitedLinks.insert(ladder.back());
        if(links.count(end_page)){ // found end page in current ladder
            ladder.push_back(end_page);
            return ladder;
        }
        // end page not found, create new ladders from the current ladder
        for(auto it = links.begin(); it != links.end(); ++it){
            if(!visitedLinks.count(*it)){
                auto partialLadder {ladder};
                partialLadder.push_back(*it);
                ladderQueue.push(partialLadder);
            }
        }
    }
    return {}; // no ladder found
}



int main() {
    auto ladder = findWikiLadder("Milkshake", "Gene");
    cout << endl;

    if(ladder.empty()) {
        cout << "No ladder found!" << endl;
    } else {
        cout << "Ladder found:" << endl;
        cout << "\t";

        // Print the ladder here!
        for(auto it = ladder.begin(); it != ladder.end(); ++it){
            cout << *it << " ";
        }
        cout << endl;
    }

    return 0;
}




