#include "browser.h"
#include <list>
#include <string>

typedef std::list<std::string> strings;

extern "C" {

browser_handler img_browser_create() {
    return new strings();
}

void img_browser_free(browser_handler bh) {
    delete (strings *)bh;
}

browser_item img_iterator_begin(browser_handler bh) {
    return new strings::iterator(((strings *)bh)->begin());
}

void img_iterator_free(browser_item i) {
    delete (strings::iterator *)(i);
}

browser_item img_iterator_next(browser_item i) {
    ((strings::iterator *)(i))->operator ++();
    return i;
}

int img_iterator_is_end(browser_handler bh, browser_item i) {
    return ((strings *)bh)->end() == *((strings::iterator *)(i));
}
void img_append(browser_handler bh, const char *name) {
    ((strings *)bh)->push_back(name);
}

const char *img_browser_get_name(browser_handler bh, browser_item i) {
    return (*(strings::iterator *)(i))->c_str();
}

}

