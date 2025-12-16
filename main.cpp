#include "utils.h"

const float WINDOW_W = 1200.0f;
const float WINDOW_H = 800.0f;
const float PADDING = 10.0f;
const float ITEM_HEIGHT = 30.0f;
const unsigned int FONT_SIZE = 20;
int selected_index_left, selected_index_right;
sf::Font font;

struct button_data{
    sf::RectangleShape shape;
    std::string name;
    bool isPressed;
} button[10];

void SetupButton(sf::RenderWindow& window, const sf::Font& font, float x, float y, button_data &button, int i) {
    button.shape.setSize(sf::Vector2f(200, 50));
    button.shape.setPosition(sf::Vector2f(x+200*(i-1), y)); 
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool isHovering = button.shape.getGlobalBounds().contains(mousePos);
    if(isHovering) 
        button.shape.setFillColor(sf::Color(224, 224, 224)); 
    else 
        button.shape.setFillColor(sf::Color(40, 40, 40)); 
    button.shape.setOutlineThickness(1.0f);
    button.shape.setOutlineColor(sf::Color(100, 100, 100));

    sf::Text button_text(font);
    button_text.setFont(font); 
    button_text.setCharacterSize(FONT_SIZE);
    button_text.setString(button.name);
    if(isHovering)
        button_text.setFillColor(sf::Color::Black);
    else button_text.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = button_text.getLocalBounds();
    button_text.setPosition(sf::Vector2f(x + 200*(i-1) + 75.0f, y + 15.0f));
    window.draw(button.shape);
    window.draw(button_text);
}

bool UpdateButton(const sf::Vector2f& mousePos, button_data &button) {
    bool actionTriggered = false; 
    if (button.shape.getGlobalBounds().contains(mousePos)) {
        button.shape.setFillColor(sf::Color::Red);
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            if (!button.isPressed) {
                actionTriggered = true; 
                button.isPressed = true; 
            }
        }
        else {
            button.isPressed = false;
        }
    }
    else {
        button.isPressed = false; 
    }
    return actionTriggered; 
}


void DrawPane(sf::RenderWindow& window, const char* path_display, float x, float y, data files[], int size, int selected_index) {
    const float PANE_W = (WINDOW_W - 3 * PADDING) / 2.0f;
    const float PANE_H = WINDOW_H - 2 * PADDING;

    sf::RectangleShape pane_bg;
    pane_bg.setSize(sf::Vector2f(PANE_W, PANE_H));
    pane_bg.setPosition(sf::Vector2f(x, y)); 
    pane_bg.setFillColor(sf::Color::White); 
    pane_bg.setOutlineThickness(1.0f);
    pane_bg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(pane_bg);

    sf::Text path_text(font);
    path_text.setCharacterSize(FONT_SIZE);
    path_text.setString(path_display); // Aici setati continutul
    path_text.setPosition(sf::Vector2f(x + 5.0f, y + 5.0f));
    path_text.setFillColor(sf::Color::Black); 
    window.draw(path_text);

    const float LIST_START_Y = y + 5.0f + ITEM_HEIGHT + PADDING; 
    const float LIST_LEFT_X = x + 2.0f;
    const float LIST_ITEM_W = PANE_W - 4.0f;

    for (int i = 0; i < size; ++i) { 
        sf::Text item_text(font, files[i].name, FONT_SIZE);        
        if (files[i].isDir) {
            item_text.setFillColor(sf::Color(150, 150, 255)); 
        } else {
            item_text.setFillColor(sf::Color::White); 
        }
        if (i == selected_index) {
            sf::RectangleShape highlight_bg;
            highlight_bg.setSize(sf::Vector2f(LIST_ITEM_W, ITEM_HEIGHT - PADDING));
            highlight_bg.setPosition(sf::Vector2f(LIST_LEFT_X, LIST_START_Y + i * ITEM_HEIGHT - 5.0f));
            highlight_bg.setFillColor(sf::Color(37, 150, 190, 100)); // Portocaliu transparent
            highlight_bg.setOutlineThickness(1.0f);
            highlight_bg.setOutlineColor(sf::Color(255, 128, 0, 255));
            window.draw(highlight_bg);
        }
        item_text.setPosition(sf::Vector2f(x + 5.0f, y + 5.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        item_text.setFillColor(sf::Color::Black); 

        window.draw(item_text);
    }
}

int GetClickedIndex(float mouse_y, float pane_y, float pane_h, int size) {
    const float LIST_START_Y = pane_y + 5.0f + ITEM_HEIGHT + PADDING; 
    if (mouse_y < LIST_START_Y) return -1;
    float offset_y = mouse_y - LIST_START_Y;
    int index = (int)(offset_y / ITEM_HEIGHT);
    if (index >= 0 && index < size && mouse_y < pane_y + pane_h)
        return index;
    return -1;
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
    save_with_metadata(path, files_left, size_left);
    save_with_metadata(documents_path, files_right, size_right);


    sf::VideoMode video_mode;
    video_mode.size = sf::Vector2u((unsigned int)WINDOW_W, (unsigned int)WINDOW_H);
    sf::RenderWindow window(video_mode, "My Commander", sf::Style::Close);
    if (!font.openFromFile("Segoe UI.ttf")) { 
        fprintf(stderr, "Error loading font 'Segoe UI.ttf'! Ensure it is in the project root.\n");
        return 0;
    }
    button[1].name = "Copy";
    button[2].name = "Move";
    button[3].name = "NewFolder";
    button[4].name = "Delete";
    button[5].name = "Rename";
    // Definirea parametrilor panourilor pentru logica de click
    const float PANE_W = (WINDOW_W - 3 * PADDING) / 2.0f;
    const float LEFT_PANE_X = PADDING;
    const float LEFT_PANE_Y = PADDING;
    const float RIGHT_PANE_X = PADDING + PANE_W + PADDING;
    const float RIGHT_PANE_Y = PADDING;
    const float PANE_H = WINDOW_H - 2 * PADDING;
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

            if (const auto *mouse_event = event->getIf<sf::Event::MouseButtonPressed>()) {
                // const auto& mouse_event = event.get<sf::Event::MouseButtonPressed>();
                if (mouse_event->button == sf::Mouse::Button::Left) {
                    float mouse_x = (float)mouse_event->position.x, mouse_y = (float)mouse_event->position.y;
                    
                    int clicked_index = -1;

                    // VERIFICARE PANOUL STANG
                    if (mouse_x >= LEFT_PANE_X && mouse_x < LEFT_PANE_X + PANE_W &&
                        mouse_y >= LEFT_PANE_Y && mouse_y < LEFT_PANE_Y + PANE_H) {
                        
                        clicked_index = GetClickedIndex(mouse_y, LEFT_PANE_Y, PANE_H, size_left);
                        
                        if (clicked_index != -1) {
                            selected_index_left = clicked_index;
                            selected_index_right = -1; 
                            //printf("Selected Left: %s\n", files_left[selected_index_left].name);
                        }
                    }
                    
                    // VERIFICARE PANOUL DREPT
                    else if (mouse_x >= RIGHT_PANE_X && mouse_x < RIGHT_PANE_X + PANE_W &&
                            mouse_y >= RIGHT_PANE_Y && mouse_y < RIGHT_PANE_Y + PANE_H) {

                        clicked_index = GetClickedIndex(mouse_y, RIGHT_PANE_Y, PANE_H, size_right);
                        
                        if (clicked_index != -1) {
                            selected_index_right = clicked_index;
                            selected_index_left = -1;
                            //printf("Selected Right: %s\n", files_right[selected_index_right].name);
                        }
                    }
                }
                else if (mouse_event->button == sf::Mouse::Button::Right) {
                    selected_index_left = selected_index_left;
                }
            }
            window.clear(sf::Color(30, 30, 30)); 
            
            // Deseneaza panoul stang
            DrawPane(window, path, PADDING, PADDING, files_left, size_left, selected_index_left);

            // Deseneaza panoul drept 
            DrawPane(window, documents_path, PADDING + PANE_W + PADDING, PADDING, files_right, size_right, selected_index_right);

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (int i = 1;i <= 5;++i) {
                SetupButton(window, font, 0,  WINDOW_H-50, button[i], i);
                 if (UpdateButton( mousePos, button[i])) {

                    // pentru cazul in care este selectat un fisier din panoul stang
                    if (selected_index_left != -1 && selected_index_right == -1) {
                        if (i == 1) copy(path, files_left[selected_index_left].name, documents_path, files_right, size_right);
                        else if (i == 2) move(path, files_left[selected_index_left].name, documents_path, files_left, size_left, files_right, size_right);
                        else if (i == 3) {
                            char new_name[105];
                            printf("Name: ");
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strcspn(new_name, "\n")] = 0;
                            create_folder(path, new_name, files_left, size_left);
                        }
                        else if (i == 4) file_delete(path, files_left[selected_index_left].name, files_left, size_left);
                        else {
                            char new_name[105];
                            printf("Name: ");
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strcspn(new_name, "\n")] = 0;
                            file_rename(path, files_left[selected_index_left].name, new_name, files_left, size_left);
                        }
                    }
                    
                    // pentru cazul in care este selectat un fisier din panoul drept
                    else if (selected_index_left == -1 && selected_index_right != -1){
                        if (i == 1) copy(documents_path, files_right[selected_index_right].name, path, files_left, size_left);
                        else if (i == 2) move(documents_path, files_right[selected_index_right].name, path, files_right, size_right, files_left, size_left);
                        else if (i == 3) {
                            char new_name[105];
                            printf("Name: ");
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strcspn(new_name, "\n")] = 0;
                            create_folder(documents_path, new_name, files_right, size_right);
                        }
                        else if (i == 4) file_delete(documents_path, files_right[selected_index_right].name, files_right, size_right);
                        else {
                            char new_name[105];
                            printf("Name: ");
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strcspn(new_name, "\n")] = 0;
                            file_rename(documents_path, files_right[selected_index_right].name, new_name, files_right, size_right); 
                        }
                    }
                 }
            }
            window.display();
        }
    }
}

// TODO: Adauga .. pt a te intoarce in folder-ul parinte