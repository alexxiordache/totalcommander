#include "utils.h"

const float WINDOW_W = 1200.0f;
const float WINDOW_H = 800.0f;
const float PADDING = 10.0f;
const float ITEM_HEIGHT = 30.0f;
const unsigned int FONT_SIZE = 20;


void DrawPane(sf::RenderWindow& window, const sf::Font& font, const char* path_display, float x, float y, data files[]) {
    const float PANE_W = (WINDOW_W - 3 * PADDING) / 2.0f;
    const float PANE_H = WINDOW_H - 2 * PADDING;

    sf::RectangleShape pane_bg;
    pane_bg.setSize(sf::Vector2f(PANE_W, PANE_H));
    pane_bg.setPosition(sf::Vector2f(x, y)); 
    pane_bg.setFillColor(sf::Color(40, 40, 40)); 
    pane_bg.setOutlineThickness(1.0f);
    pane_bg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(pane_bg);

    sf::Text path_text(font);
    path_text.setCharacterSize(FONT_SIZE);
    path_text.setString(path_display); // Aici setati continutul
    path_text.setPosition(sf::Vector2f(x + 5.0f, y + 5.0f));
    window.draw(path_text);

    for (int i = 0; i < n; ++i) { 
        sf::Text item_text(font, files[i].name, FONT_SIZE);        
        if (files[i].isDir) {
            item_text.setFillColor(sf::Color(150, 150, 255)); 
        } else {
            item_text.setFillColor(sf::Color::White); 
        }
        item_text.setPosition(sf::Vector2f(x + 5.0f, y + 5.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        window.draw(item_text);
    }
}

int main() {
    DIR *dir;
    struct dirent *ent;
    bool found = 0;
    char *path, documents_path[PATH_MAX_LEN];
    // Construiesc path-ul pana in folderul Documents
    const char *temp="\\Documents", *user_profile = getenv("USERPROFILE");
    if(!user_profile) {
        printf("Eroare: Nu am putut gasi user profile");
        return 0;
    }
    strcpy(documents_path, user_profile);
    strcat(documents_path, temp);
    path = get_executable_directory();
    _chdir(path);
    save_with_metadata(path, files, n);
    save_with_metadata(documents_path, files2, n2);


    sf::VideoMode video_mode;
    video_mode.size = sf::Vector2u((unsigned int)WINDOW_W, (unsigned int)WINDOW_H);
    sf::RenderWindow window(video_mode, "My Commander", sf::Style::Close);
    sf::Font font;
    if (!font.openFromFile("Segoe UI.ttf")) { 
        fprintf(stderr, "Error loading font 'Segoe UI.ttf'! Ensure it is in the project root.\n");
        return 0;
    }
    while(window.isOpen()) {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            
            if (const auto* keypressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keypressed->code == sf::Keyboard::Key::Escape) { 
                    window.close();
                }
                if (keypressed->code == sf::Keyboard::Key::Tab) { 
                    printf("TAB pressed - switch pane logic required.\n");
                }
            }

            // Curata ecranul
            window.clear(sf::Color(30, 30, 30)); 

            // Deseneaza Panoul Stâng (Pass C-string literals)
            DrawPane(window, font, path, PADDING, PADDING, files);

            // Deseneaza Panoul Drept 
            float left_pane_w = (WINDOW_W - 3 * PADDING) / 2.0f;
            DrawPane(window, font, documents_path, PADDING + left_pane_w + PADDING, PADDING, files2);

            window.display();
        }
    }
}

// TODO: Adauga .. pt a te intoarce in folder-ul parinte