#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>

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

// Пример использования
int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Slider Example");
    sf::Font font;
    if (!font.loadFromFile("../Dependencies/GUI/Silkscreen.ttf")) {
        return -1;
    }

    Slider slider(100, 300, 600, 50, font);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            slider.handleEvent(event);
        }

        // Получение значения слайдера
        int sliderValue = slider.getValue();

        // Вывод значения в консоль (для демонстрации)
        std::cout << "Slider Value: " << sliderValue << '\r';

        window.clear(sf::Color::White);
        slider.draw(window);
        window.display();
    }

    return 0;
}