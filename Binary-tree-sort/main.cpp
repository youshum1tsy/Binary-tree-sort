#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>


class Slider {
public:
    Slider(float x, float y, float width, float height, sf::Font& font)
        : minValue(0), maxValue(100000), currentValue(0), isDragging(false) {
        track.setSize(sf::Vector2f(width, height / 4));
        track.setPosition(x, y + height / 2 - track.getSize().y / 2);
        track.setFillColor(sf::Color(200, 200, 200));

        knob.setSize(sf::Vector2f(height, height));
        knob.setOrigin(knob.getSize().x / 2, knob.getSize().y / 2);
        knob.setPosition(x, y + height / 2);
        knob.setFillColor(sf::Color(100, 100, 100));

        valueText.setFont(font);
        valueText.setCharacterSize(24);
        valueText.setFillColor(sf::Color::Black);
        valueText.setPosition(x, y - 30);
        updateValueText();
    }

    void handleEvent(const sf::Event& event) {
        if (event.type == sf::Event::MouseButtonPressed) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            if (knob.getGlobalBounds().contains(mousePos)) {
                isDragging = true;
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased) {
            isDragging = false;
        }
        else if (event.type == sf::Event::MouseMoved) {
            if (isDragging) {
                float newX = static_cast<float>(event.mouseMove.x);
                newX = std::max(track.getPosition().x, std::min(newX, track.getPosition().x + track.getSize().x));
                knob.setPosition(newX, knob.getPosition().y);
                currentValue = static_cast<int>(minValue + (maxValue - minValue) * ((newX - track.getPosition().x) / track.getSize().x));
                updateValueText();
            }
        }
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(track);
        window.draw(knob);
        window.draw(valueText);
    }

    int getValue() const {
        return currentValue;
    }

    void setValue(int value) {
        currentValue = std::max(minValue, std::min(maxValue, value));
        float newX = track.getPosition().x + track.getSize().x * ((currentValue - minValue) / static_cast<float>(maxValue - minValue));
        knob.setPosition(newX, knob.getPosition().y);
        updateValueText();
    }

private:
    void updateValueText() {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << currentValue;
        valueText.setString(oss.str());
    }

    sf::RectangleShape track;
    sf::RectangleShape knob;
    sf::Text valueText;
    int minValue;
    int maxValue;
    int currentValue;
    bool isDragging;
};

void openFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (file.is_open()) {
        std::cout << "Contents of " << filePath << ":\n";
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        file.close();
    }
    else {
        std::cout << "Failed to open file: " << filePath << std::endl;
    }
}


int main() {

    sf::RenderWindow window(sf::VideoMode(1100, 700), "Menu");
    window.setFramerateLimit(60);
    sf::Sprite menubg;
    sf::Font font;
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(32);

    if (!font.loadFromFile("../Dependencies/GUI/Silkscreen.ttf")) {
        return -1;
    }

    sf::Text menu[4];
    std::string menuItems[4] = { "Random", "Open file 1", "Open file 2", "Exit" };
    sf::Color bgcolor(192, 192, 192);
    
    
    for (int i = 0; i < 4; ++i) {
        menu[i].setFont(font);
        menu[i].setString(menuItems[i]);
        menu[i].setPosition(100, 300 + i * 80);

    }
    bool isMainMenu = true;

    Slider slider(250, 100, 600, 50, font);
    sf::Text subMenu[4];
    std::string subMenuItems[4] = { "Sorted file", "Unsorted file", "Back to main menu", "Exit" };

    for (int i = 0; i < 4; ++i) {
        subMenu[i].setFont(font);
        subMenu[i].setString(subMenuItems[i]);
        subMenu[i].setPosition(100, 300 + i * 80);
    }
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (isMainMenu) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 4; ++i) {
                        if (menu[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (i == 0) {
                                isMainMenu = false;
                            }
                            else if (i == 1) {
                                openFile("file1.txt");
                            }
                            else if (i == 2) {
                                openFile("file2.txt");
                            }
                            else if (i == 3) {
                                window.close();
                            }
                        }
                    }
                }
            }
            else {
                std::string timeString = "Time:";
                text.setString(timeString);
                text.setPosition(600, 299);
                slider.handleEvent(event);
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 4; ++i) {

                        if (subMenu[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (i == 2) {
                                isMainMenu = true;
                            }
                            else if (i == 3) {
                                window.close();
                            }
                            else if (i == 0) {
                                openFile("sorted_file.txt");
                            }
                            else if (i == 1) {
                                openFile("unsorted_file.txt");
                            }
                        }
                    }
                }
            }
        }

        window.clear(bgcolor);
        if (isMainMenu) {
            for (int i = 0; i < 4; ++i) {
                window.draw(menu[i]);
            }
        }
        else {
            slider.draw(window);
            for (int i = 0; i < 4; ++i) {
                window.draw(text);
                window.draw(subMenu[i]);

            }
        }
        window.display();
    }

    return 0;
}

//
//class Slider {
//public:
//    Slider(float x, float y, float width, float height, sf::Font& font)
//        : minValue(0), maxValue(100000), currentValue(0), isDragging(false) {
//        track.setSize(sf::Vector2f(width, height / 4));
//        track.setPosition(x, y + height / 2 - track.getSize().y / 2);
//        track.setFillColor(sf::Color(200, 200, 200));
//
//        knob.setSize(sf::Vector2f(height, height));
//        knob.setOrigin(knob.getSize().x / 2, knob.getSize().y / 2);
//        knob.setPosition(x, y + height / 2);
//        knob.setFillColor(sf::Color(100, 100, 100));
//
//        valueText.setFont(font);
//        valueText.setCharacterSize(24);
//        valueText.setFillColor(sf::Color::Black);
//        valueText.setPosition(x, y - 30);
//        updateValueText();
//    }
//
//    void handleEvent(const sf::Event& event) {
//        if (event.type == sf::Event::MouseButtonPressed) {
//            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
//            if (knob.getGlobalBounds().contains(mousePos)) {
//                isDragging = true;
//            }
//        }
//        else if (event.type == sf::Event::MouseButtonReleased) {
//            isDragging = false;
//        }
//        else if (event.type == sf::Event::MouseMoved) {
//            if (isDragging) {
//                float newX = static_cast<float>(event.mouseMove.x);
//                newX = std::max(track.getPosition().x, std::min(newX, track.getPosition().x + track.getSize().x));
//                knob.setPosition(newX, knob.getPosition().y);
//                currentValue = static_cast<int>(minValue + (maxValue - minValue) * ((newX - track.getPosition().x) / track.getSize().x));
//                updateValueText();
//            }
//        }
//    }
//
//    void draw(sf::RenderWindow& window) const {
//        window.draw(track);
//        window.draw(knob);
//        window.draw(valueText);
//    }
//
//    int getValue() const {
//        return currentValue;
//    }
//
//    void setValue(int value) {
//        currentValue = std::max(minValue, std::min(maxValue, value));
//        float newX = track.getPosition().x + track.getSize().x * ((currentValue - minValue) / static_cast<float>(maxValue - minValue));
//        knob.setPosition(newX, knob.getPosition().y);
//        updateValueText();
//    }
//
//private:
//    void updateValueText() {
//        std::ostringstream oss;
//        oss << std::fixed << std::setprecision(0) << currentValue;
//        valueText.setString(oss.str());
//    }
//
//    sf::RectangleShape track;
//    sf::RectangleShape knob;
//    sf::Text valueText;
//    int minValue;
//    int maxValue;
//    int currentValue;
//    bool isDragging;
//};
//
//// Пример использования
//int main() {
//    sf::RenderWindow window(sf::VideoMode(800, 600), "Slider Example");
//    sf::Font font;
//    if (!font.loadFromFile("../Dependencies/GUI/Silkscreen.ttf")) {
//        return -1;
//    }
//
//    Slider slider(100, 300, 600, 50, font);
//
//    while (window.isOpen()) {
//        sf::Event event;
//        while (window.pollEvent(event)) {
//            if (event.type == sf::Event::Closed) {
//                window.close();
//            }
//            slider.handleEvent(event);
//        }
//
//        // Получение значения слайдера
//        int sliderValue = slider.getValue();
//
//        // Вывод значения в консоль (для демонстрации)
//        std::cout << "Slider Value: " << sliderValue << '\r';
//
//        window.clear(sf::Color::White);
//        slider.draw(window);
//        window.display();
//    }
//
//    return 0;
//}