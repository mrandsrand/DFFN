#include <vector>
#include "Clickable.h"

using namespace std;

class Option;

// just a container for Options
class Menu {

private:
    static void right(Menu *&menu, bool &nav, int currentOption);
    static void left(Menu *&menu, bool &nav, bool &reload);
    static void up(Menu *&menu, bool &reload);
    static void down(Menu *&menu, bool &reload);

public:
    static void navigate(Menu &firstMenu);

    class Exit: public Clickable::OnClickListener {
        Exit() = default;
    public:
        static Exit *create() { return new Exit; }
        void onClick(Clickable &clickable) override {
            cout << "Exiting..." << endl;
            delete this;
        }
    };

private:
    string description;
    vector<Option> options;
    int currentOption;
    int onLeftClick;
    int longestOptionName;

    void print();

public:
    Menu(const string &description, vector<Option> options, int onLeftClick = -1);
    ~Menu();

    bool hasBackOption();
    int optionsCount();
    void addOption(const Option& option, int position, bool setAsCurrent = false);
    void removeOption(int optionNr);
    void link(int optionNr, Menu &withMenu, bool andViceVersa = true);

    void setOnClickListener(int optionNr, Clickable::OnClickListener *listener);
    void setDescription(const string &desc);

    int getCurrentOptionIndex() const;
    void setCurrentOptionIndex(int optionNr);
};

// Menus consist of Options
class Option : public Clickable {
friend class Menu;

private:
    bool forward;
    Menu *nextMenu;
    string name;
    string description;
    bool hasDescription;

public:
    explicit Option(const string &name, const string &description = {}, bool isBackOption = false);

    Menu *getNextMenu() const;

    void setNextMenu(Menu *menu);

    void setDescription(const string &desc);
};