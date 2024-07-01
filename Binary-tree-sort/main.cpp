#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <fstream>
#include <windows.h>

namespace MenuConstants {
    const int MainMenu = 0;
    const int RandomMenu = 1;
    const int File1 = 2;
    const int File2 = 3;
}

class Slider {
public:
    Slider(float x, float y, float width, float height, sf::Font& font) : minValue(0), maxValue(100000), currentValue(0), isDragging(false) {
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

struct Node {
    int value;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
};

Node* createNode(int val, Node* parent = nullptr) {
    Node* newNode = new Node;
    newNode->value = val;
    newNode->parent = parent;
    return newNode;
}

void insertNode(Node*& root, int val) {
    if (root == nullptr) {
        root = createNode(val);
    }
    else {
        Node* current = root;
        Node* parent = nullptr;
        while (current != nullptr) {
            parent = current;
            if (val < current->value) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }
        if (val < parent->value) {
            parent->left = createNode(val, parent);
        }
        else {
            parent->right = createNode(val, parent);
        }
    }
}

Node* buildBinarySortTree(const std::vector<int>& arr) {
    Node* root = nullptr;
    for (int val : arr) {
        insertNode(root, val);
    }
    return root;
}

void collectSortedValues(Node* root, std::vector<int>& sortedArray) {
    if (root != nullptr) {
        collectSortedValues(root->left, sortedArray);
        sortedArray.push_back(root->value);
        collectSortedValues(root->right, sortedArray);
    }
}

std::vector<int> binaryTreeSort(const std::vector<int>& arr) {
    Node* root = buildBinarySortTree(arr);
    std::vector<int> sortedArray;
    collectSortedValues(root, sortedArray);
    return sortedArray;
}

void parseNumbersFromFile(const std::string& path, std::vector<int>& numbers) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Could not open the file " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string item;
        while (std::getline(ss, item, ',')) {
            try {
                numbers.push_back(std::stoi(item));
            }
            catch (const std::invalid_argument& e) {
                std::cerr << "Invalid number: " << item << std::endl;
            }
            catch (const std::out_of_range& e) {
                std::cerr << "Number out of range: " << item << std::endl;
            }
        }
    }

    file.close();
}

void generateNumbersFile(const std::string& path, int numElements) {
    std::ofstream outFile(path);
    if (!outFile) {
        std::cerr << "Error opening file: " << path << std::endl;
        return;
    }

    srand(time(NULL));

    for (int i = 0; i < numElements; ++i) {
        int number = (rand() % 10000) * 10 + (rand() % 10);
        if (rand() % 2 == 0) {
            number *= -1;
        }
        outFile << number;
        if (i < numElements - 1) {
            outFile << ",";
        }
    }

    outFile.close();
}

void writeNumbersToFile(const std::string& path, const std::vector<int>& data) {
    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << path << std::endl;
        return;
    }

    // Записываем значения вектора в файл, разделяя запятыми
    for (size_t i = 0; i < data.size(); ++i) {
        outFile << data[i];
        if (i < data.size() - 1) {
            outFile << ",";
        }
    }

    outFile.close();
}

void openFile(const wchar_t* relativePath) {
    wchar_t absolutePath[MAX_PATH];
    GetFullPathName(relativePath, MAX_PATH, absolutePath, NULL);
    HINSTANCE result = ShellExecute(NULL, L"open", absolutePath, NULL, NULL, SW_SHOWNORMAL);
}


int main() {

    sf::RenderWindow window(sf::VideoMode(1100, 700), "Menu");
    sf::Color bgColor(192, 192, 192);
    window.setFramerateLimit(60);

    sf::Font font;
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(32);

    if (!font.loadFromFile("../Dependencies/GUI/Silkscreen.ttf")) {
        return -1;
    }

    sf::Text mainMenu[4];
    std::string mainMenuItems[4] = { "Random", "Open file 1", "Open file 2", "Exit" };

    for (int i = 0; i < 4; ++i) {
        mainMenu[i].setFont(font);
        mainMenu[i].setString(mainMenuItems[i]);
        mainMenu[i].setPosition(100, 300 + i * 80);

    }

    Slider slider(250, 100, 600, 50, font);

    sf::Text randomMenu[4];
    std::string randomMenuItems[4] = { "Start sort", "Sorted file", "Unsorted file", "Back to main menu" };

    sf::Text OtherMenu1[4];
    std::string OtherMenuItems[4] = { "Start sort", "Sorted file", "Unsorted file", "Back to main menu" };

    sf::Text OtherMenu2[4];
    std::string OtherMenuItems2[4] = { "Start sort", "Sorted file", "Unsorted file", "Back to main menu" };


    for (int i = 0; i < 4; ++i) {
        randomMenu[i].setFont(font);
        randomMenu[i].setString(randomMenuItems[i]);
        randomMenu[i].setPosition(100, 300 + i * 80);
    }

    for (int i = 0; i < 4; ++i) {
        OtherMenu1[i].setFont(font);
        OtherMenu1[i].setString(OtherMenuItems[i]);
        OtherMenu1[i].setPosition(100, 300 + i * 80);
    }

    for (int i = 0; i < 4; ++i) {
        OtherMenu2[i].setFont(font);
        OtherMenu2[i].setString(OtherMenuItems2[i]);
        OtherMenu2[i].setPosition(100, 300 + i * 80);
    }

    int currentMenu = MenuConstants::MainMenu;
    sf::Clock clock;
    sf::Time elapsed;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (currentMenu == MenuConstants::MainMenu) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 4; ++i) {
                        if (mainMenu[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (i == 0) {
                                currentMenu = MenuConstants::RandomMenu;
                            }
                            else if (i == 1) {
                                currentMenu = MenuConstants::File1;
                            }
                            else if (i == 2) {
                                currentMenu = MenuConstants::File2;
                            }
                            else if (i == 3) {
                                window.close();
                            }
                        }
                    }
                }
            }

            else if (currentMenu == MenuConstants::RandomMenu) {
                text.setPosition(600, 295);
                slider.handleEvent(event);
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 4; ++i) {
                        if (mainMenu[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (i == 0) {

                                generateNumbersFile("../Dependencies/FILES/randomUnsortedSet.txt", slider.getValue());

                                std::vector<int> data;
                                clock.restart();
                                parseNumbersFromFile("../Dependencies/FILES/randomUnsortedSet.txt", data);

                                std::vector<int> sortedData = binaryTreeSort(data);
                                elapsed = clock.getElapsedTime();

                                writeNumbersToFile("../Dependencies/FILES/randomSortedSet.txt", sortedData);
                                text.setString("Time: " + std::to_string(elapsed.asSeconds()) + " sec");
                            }
                            else if (i == 1) {
                                openFile(L"../Dependencies/FILES/randomSortedSet.txt");
                            }
                            else if (i == 2) {
                                openFile(L"../Dependencies/FILES/randomUnsortedSet.txt");
                            }
                            else if (i == 3) {
                                currentMenu = MenuConstants::MainMenu;
                            }
                        }
                    }
                }
            }

            else if (currentMenu == MenuConstants::File1) {
                text.setPosition(600, 295);
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 4; ++i) {
                        if (mainMenu[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (i == 0) {

                                std::vector<int> data;
                                clock.restart();
                                parseNumbersFromFile("../Dependencies/FILES/UnsortedSet1.txt", data);
                                std::vector<int> sortedData = binaryTreeSort(data);
                                elapsed = clock.getElapsedTime();

                                writeNumbersToFile("../Dependencies/FILES/SortedSet1.txt", sortedData);
                                text.setString("Time: " + std::to_string(elapsed.asSeconds()) + " sec");

                            }
                            else if (i == 1) {
                                openFile(L"../Dependencies/FILES/SortedSet1.txt");
                            }
                            else if (i == 2) {
                                openFile(L"../Dependencies/FILES/UnsortedSet1.txt");
                            }
                            else if (i == 3) {
                                currentMenu = MenuConstants::MainMenu;
                            }
                        }
                    }
                }

            }

            else if (currentMenu == MenuConstants::File2) {
                text.setPosition(600, 295);
                if (event.type == sf::Event::MouseButtonPressed) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    for (int i = 0; i < 4; ++i) {
                        if (mainMenu[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            if (i == 0) {

                                std::vector<int> data;
                                clock.restart();
                                parseNumbersFromFile("../Dependencies/FILES/UnsortedSet2.txt", data);
                                std::vector<int> sortedData = binaryTreeSort(data);
                                elapsed = clock.getElapsedTime();

                                writeNumbersToFile("../Dependencies/FILES/SortedSet2.txt", sortedData);
                                text.setString("Time: " + std::to_string(elapsed.asSeconds()) + " sec");

                            }
                            else if (i == 1) {
                                openFile(L"../Dependencies/FILES/SortedSet2.txt");
                            }
                            else if (i == 2) {
                                openFile(L"../Dependencies/FILES/UnsortedSet2.txt");
                            }
                            else if (i == 3) {
                                currentMenu = MenuConstants::MainMenu;
                            }
                        }
                    }
                }
            }

        }

        window.clear(bgColor);

        if (currentMenu == MenuConstants::MainMenu) {
            for (int i = 0; i < 4; ++i) {
                window.draw(mainMenu[i]);
            }
        }

        else if (currentMenu == MenuConstants::RandomMenu) {
            slider.draw(window);
            for (int i = 0; i < 4; ++i) {
                window.draw(text);
                window.draw(randomMenu[i]);
            }
        }

        else if (currentMenu == MenuConstants::File1) {
            for (int i = 0; i < 4; ++i) {
                window.draw(text);
                window.draw(OtherMenu1[i]);
            }
        }

        else if (currentMenu == MenuConstants::File2) {
            for (int i = 0; i < 4; ++i) {
                window.draw(text);
                window.draw(OtherMenu2[i]);
            }
        }

        window.display();
    }

    return 0;
}