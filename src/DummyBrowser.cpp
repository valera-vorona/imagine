#include "DummyBrowser.h"

    int DummyBrowser::can_do(std::string path) const {
        return 1;
    }

    void DummyBrowser::update_path(std::string path, bool force_update) {

    }

    void DummyBrowser::update_file(std::string file, bool force_update) {

    }

    bool DummyBrowser::prev() {
        return false;
    }

    bool DummyBrowser::next() {
        return false;
    }

    bool DummyBrowser::enter() {
        return false;
    }

    std::string DummyBrowser::get_path() const {
        return "";
    }

    std::string DummyBrowser::get_full_path() const {
        return "";
    }

    bool DummyBrowser::is_dir() const {
        return false;
    }

