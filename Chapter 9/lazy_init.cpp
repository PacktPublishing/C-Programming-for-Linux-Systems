#include <iostream>
#include <chrono>
#include <optional>
#include <string_view>
#include <thread>

using namespace std;

struct Settings {
    Settings(string_view fileName) {
        cout << "Loading settings: " << fileName << endl;
    }
    ~Settings() {
        cout << "Removing settings" << endl;
    }

    void setSetting(string_view setting,
                    string_view value) {
        cout << "Set setting: " << setting 
             << " to: " << value << endl;
    }
};

struct Config {
    optional<Settings> settings{};
    Config() {
        cout << "Config loaded..." << endl;
    }

    void changeSetting(string_view setting, 
                       string_view value) {
        if (!settings)
            settings.emplace("settings.cfg");
        settings->setSetting(setting, value);
    }
};

int main() {
    Config cfg;
    cout << "Application startup..." << endl;
    this_thread::sleep_for(chrono::seconds(10));
    cfg.changeSetting("Drive mode", "Sport");
    cfg.changeSetting("Gear label", "PRNDL");

    return 0;
}