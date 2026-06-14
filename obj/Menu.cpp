#include <iostream>
#include <conio.h>
#include <iomanip>
#include "Menu.h"

Menu::Menu(const string &description, vector<Option> options, int onLeftClick) {
    Menu::description = description;
    Menu::options = move(options);
    Menu::currentOption = 0;
    Menu::onLeftClick = onLeftClick;
    Menu::options[onLeftClick].forward = false;

    longestOptionName = 0;
    for (int i = 0; i < optionsCount(); i++) {
        if (Menu::options[i].name.length() > longestOptionName) {
            longestOptionName = Menu::options[i].name.length();
        }
    }
}

Menu::~Menu() {
    for (auto & option : options)
        delete option.onClickListener;
}

void Menu::navigate(Menu &firstMenu) {

    Menu *menu = &firstMenu;
    bool stillNavigating = true;

    menu->print();

    do {
        bool reloadMenu = true;

        switch (_getch()) {
            case 13: // enter
            case ' ':
            case 'd':
            case 'D':
            case '6':
                right(menu, stillNavigating, menu->currentOption);
                break;
            case '\b': // backspace
            case 27: // esc
            case 'a':
            case 'A':
            case '4':
                left(menu, stillNavigating, reloadMenu);
                break;
            case 'w':
            case 'W':
            case '8':
                up(menu, reloadMenu);
                break;
            case 's':
            case 'S':
            case '2':
                down(menu, reloadMenu);
                break;
            case 0:
            case 224:
                switch (_getch()) { // arrows
                    case 77:
                        right(menu, stillNavigating, menu->currentOption);
                        break;
                    case 75:
                        left(menu, stillNavigating, reloadMenu);
                        break;
                    case 72:
                        up(menu, reloadMenu);
                        break;
                    case 80:
                        down(menu, reloadMenu);
                        break;
                    case 73: // pg up
                        if (menu->currentOption) menu->currentOption = 0;
                        else reloadMenu = false;
                        break;
                    case 81: // pg dn
                        if (menu->currentOption != menu->optionsCount() - 1) {
                            menu-> currentOption = menu->optionsCount() - 1;
                            break;
                        }
                    default:
                        reloadMenu = false;
                }
                break;
            default:
                reloadMenu = false;
        }

        if (stillNavigating && reloadMenu) {
            system("cls");
            menu->print();
        }
    } while (stillNavigating);
}

// responsible for right clicks in Menu::navigate
void Menu::right(Menu *&menu, bool &nav, int currentOption) {
    Option *current = &menu->options[currentOption];

    if (current->isListenerSet())
        // dynamic cast returns nullptr if onClickListener is not in an instance of Exit
        nav = !static_cast<bool>(dynamic_cast<Exit *>(current->onClickListener));
    current->onClick(); // on click action runs if present

    if (current->nextMenu) {
        menu = current->nextMenu; // go to next menu if present
        if (current->forward) menu->currentOption = 0;
    }
}

// responsible for left clicks in Menu::navigate
void Menu::left(Menu *&menu, bool &nav, bool &reload) {
    // just right click but on onLeftClick Option
    if (menu->hasBackOption()) {
        right(menu, nav, menu->onLeftClick);
    } else reload = false;
}

// responsible for up clicks in Menu::navigate
void Menu::up(Menu *&menu, bool &reload) {
    if (menu->optionsCount() > 1) {
        if (--menu->currentOption < 0)
            menu->currentOption = menu->optionsCount() - 1;
    } else reload = false;
}

// responsible for down clicks in Menu::navigate
void Menu::down(Menu *&menu, bool &reload) {
    if (menu->optionsCount() > 1) {
        if (++menu->currentOption >= menu->optionsCount())
            menu->currentOption = 0;
    } else reload = false;
}

void Menu::addOption(const Option &option, int position, bool setAsCurrent) {
    if (position >= 0 && position <= optionsCount()) {
        options.insert(options.cbegin() + position, option);

        if (position <= onLeftClick) onLeftClick++;
        if (setAsCurrent) currentOption = position;
        if (option.name.length() > longestOptionName)
            longestOptionName = option.name.length();
    }
}

void Menu::removeOption(int optionNr) {
    if (optionNr >= 0 && optionNr < optionsCount()) {
        delete options[optionNr].onClickListener;
        options.erase(options.cbegin() + optionNr);

        longestOptionName = 0;
        for (int i = 0; i < optionsCount(); i++) {
            if (options[i].name.length() > longestOptionName) {
                longestOptionName = options[i].name.length();
            }
        }

        if (optionNr == currentOption) {
            currentOption = 0;
        } else if (optionNr < currentOption) {
            currentOption--;
        }

        if (optionNr == onLeftClick) {
            onLeftClick = -1;
        } else if (optionNr < onLeftClick) {
            onLeftClick--;
        }
    }
}

// sets withMenu as next menu of Option indexed with optionNr in this Menu
void Menu::link(int optionNr, Menu &withMenu, bool andViceVersa) {
    if (optionNr >= 0 && optionNr < optionsCount()) {
        this->options[optionNr].nextMenu = &withMenu;
        if (andViceVersa && withMenu.hasBackOption())
            withMenu.options[withMenu.onLeftClick].nextMenu = this;
    }
}

// prints menu
void Menu::print() {
    cout << endl << ' ' << description << endl;
    cout << setfill(static_cast<char>(-60)) << setw(longestOptionName + 6) << static_cast<char>(-65) << endl << setfill(' ');

    for (int i = 0; i < optionsCount(); i++) {
        bool selected = currentOption == i;

        cout << (selected ? '>' : ' ') << "  " << options[i].name;
        cout << setw(longestOptionName - static_cast<int>(options[i].name.length()) + 3);
        if (selected && options[i].hasDescription)
            cout << static_cast<char>(-61) << static_cast<char>(-60) << ' ' << options[i].description;
        else cout << static_cast<char>(-77);
        cout << endl;
    }
    cout << setfill(static_cast<char>(-60)) << setw(longestOptionName + 6) << static_cast<char>(-39) << endl;
    cout << setfill(' ');
}

bool Menu::hasBackOption() {
    return onLeftClick >= 0 && onLeftClick < optionsCount();
}

int Menu::optionsCount() {
    return options.size();
}

void Menu::setOnClickListener(int optionNr, Clickable::OnClickListener *listener) {
    if (optionNr >= 0 && optionNr < optionsCount())
        options[optionNr].setOnClickListener(listener);
}

void Menu::setDescription(const string &desc) {
    Menu::description = desc;
}

void Menu::setCurrentOptionIndex(int optionNr) {
    if (optionNr >= 0 && optionNr < optionsCount())
        currentOption = optionNr;
}

int Menu::getCurrentOptionIndex() const {
    return currentOption;
}


Option::Option(const string &name, const string &description, bool isBackOption) {
    Option::nextMenu = nullptr;
    Option::name = name;
    Option::description = description;
    Option::hasDescription = !description.empty();
    Option::forward = !isBackOption;
}

Menu *Option::getNextMenu() const {
    return nextMenu;
}

void Option::setNextMenu(Menu *menu) {
    Option::nextMenu = menu;
}

void Option::setDescription(const string &desc) {
    Option::description = desc;
}
