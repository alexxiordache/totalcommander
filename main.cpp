#include "utils.h"

const float WINDOW_W = 1200.0f;
const float WINDOW_H = 800.0f;
const float PADDING = 10.0f;
const float ITEM_HEIGHT = 30.0f;
const unsigned int FONT_SIZE = 20;
char icon_path[PATH_MAX_LEN];
bool index_side = 0; // 0 - stanga, 1 - dreapta
int selected_index[1000], selected_cnt;
sf::Font font;
std::set<int> idx;

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


void DrawPane(sf::RenderWindow& window, const char* path_display, float x, float y, data files[], int size, bool cur_side) {
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
    char ext_path[PATH_MAX_LEN];

    for (int i = 0; i < size; ++i) { 
        float item_y = LIST_START_Y + i * ITEM_HEIGHT;
        char *ext = get_extension(files[i]);
        if(!ext)
            ext = strdup("directory");
        strcpy(ext_path, icon_path);
        strcat(ext, ".png");
        strcat(ext_path, ext);
        struct stat file_info;
        if(stat(ext_path, &file_info)) { // file does not exist
            strcpy(ext_path, icon_path);
            strcat(ext_path, "default.png");
        }

        const sf::Texture texture(ext_path);
        sf::RectangleShape icon_rect;
        icon_rect.setSize(sf::Vector2f(16.0f, 16.0f));
        sf::Text item_text(font, files[i].name, FONT_SIZE);        
        if (files[i].isDir) {
            item_text.setFillColor(sf::Color(150, 150, 255)); 
        } else {
            item_text.setFillColor(sf::Color::White); 
        }
        if (index_side == cur_side && idx.count(i)) {
            sf::RectangleShape highlight_bg;
            highlight_bg.setSize(sf::Vector2f(LIST_ITEM_W, ITEM_HEIGHT - PADDING));
            highlight_bg.setPosition(sf::Vector2f(LIST_LEFT_X, item_y - 5.0f));
            highlight_bg.setFillColor(sf::Color(37, 150, 190, 100)); // Portocaliu transparent
            highlight_bg.setOutlineThickness(1.0f);
            highlight_bg.setOutlineColor(sf::Color(255, 128, 0, 255));
            window.draw(highlight_bg);
        }
        icon_rect.setPosition(sf::Vector2f(x + 5.0f, y + 10.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        icon_rect.setTexture(&texture);
        item_text.setPosition(sf::Vector2f(x + 25.0f, y + 5.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        item_text.setFillColor(sf::Color::Black); 
        window.draw(icon_rect);
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
    strcpy(icon_path, path);
    strcat(icon_path, "/icons/");
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
                if (mouse_event->button == sf::Mouse::Button::Left) {
                    float mouse_x = (float)mouse_event->position.x, mouse_y = (float)mouse_event->position.y;
                    
                    int clicked_index = -1;
                    bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                    // VERIFICARE PANOUL STANG
                    if (mouse_x >= LEFT_PANE_X && mouse_x < LEFT_PANE_X + PANE_W &&
                        mouse_y >= LEFT_PANE_Y && mouse_y < LEFT_PANE_Y + PANE_H) {
                        
                        clicked_index = GetClickedIndex(mouse_y, LEFT_PANE_Y, PANE_H, size_left);
                        printf("%d\n", ctrlPressed);
                        if (clicked_index != -1) {
                            if(index_side || !ctrlPressed) {
                                idx.clear();
                                index_side = 0;
                            }
                            if(!idx.count(clicked_index))
                                idx.insert(clicked_index);
                            else idx.erase(clicked_index);
                        }
                    }
                    
                    // VERIFICARE PANOUL DREPT
                    else if (mouse_x >= RIGHT_PANE_X && mouse_x < RIGHT_PANE_X + PANE_W &&
                            mouse_y >= RIGHT_PANE_Y && mouse_y < RIGHT_PANE_Y + PANE_H) {

                        clicked_index = GetClickedIndex(mouse_y, RIGHT_PANE_Y, PANE_H, size_right);
                        if (clicked_index != -1) {
                            if(!index_side) {
                                idx.clear();
                                index_side = 1;
                            }
                            if(!idx.count(clicked_index))
                                idx.insert(clicked_index);
                            else idx.erase(clicked_index);
                        }
                    }
                }
                // else if (mouse_event->button == sf::Mouse::Button::Right) {
                    
                // }
            }
            window.clear(sf::Color(30, 30, 30)); 
            
            // Deseneaza panoul stang
            DrawPane(window, path, PADDING, PADDING, files_left, size_left, 0);

            // Deseneaza panoul drept 
            DrawPane(window, documents_path, PADDING + PANE_W + PADDING, PADDING, files_right, size_right, 1);

            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            int cnt = 0;
            // int selected_index[1000];
            // int temp_size = size_left;
            // if(index_side == 1) 
            //     temp_size = size_right;
            // for(int i = 0; i < temp_size; i++)
            //     if(idx[i])
            //         selected_index[cnt++] = i;
            for (int i = 1;i <= 5;++i) {
                SetupButton(window, font, 0,  WINDOW_H-50, button[i], i);
                 if (UpdateButton(mousePos, button[i])) {
                    // pentru cazul in care este selectat un fisier din panoul stang
                    if (!index_side) {
                        if (i == 3) {
                            char new_name[105];
                            printf("Name: ");
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strcspn(new_name, "\n")] = 0;
                            create_folder(path, new_name, files_left, size_left);
                            idx.clear();
                            idx.insert(size_left - 1);
                            continue;
                        }
                        std::set<int>::iterator it;
                        for (it = idx.begin(); it != idx.end(); it++) 
                        for(int i = 0; i < cnt; i++)
                            if (i == 1) copy(path, files_left[*it].name, documents_path, files_right, size_right);
                            else if (i == 2) move(path, files_left[*it].name, documents_path, files_left, size_left, files_right, size_right);
                            else if (i == 4) file_delete(path,files_left[*it].name, files_left, size_left);
                            else {
                                if(cnt > 1)
                                    printf("Cannot rename more than one file.");
                                else {
                                    char new_name[105];
                                    printf("Name: ");
                                    fgets(new_name, sizeof(new_name), stdin);
                                    new_name[strcspn(new_name, "\n")] = 0;
                                    file_rename(path, files_right[*it].name, new_name, files_left, size_left);
                                }
                            }
                        }
                    // pentru cazul in care este selectat un fisier din panoul drept
                    else if (index_side) {
                        if (i == 3) {
                            char new_name[105];
                            printf("Name: ");
                            fgets(new_name, sizeof(new_name), stdin);
                            new_name[strcspn(new_name, "\n")] = 0;
                            create_folder(documents_path, new_name, files_right, size_right);
                            idx.clear();
                            idx.insert(size_right - 1);
                            continue;
                        }
                        std::set<int>::iterator it;
                        for (it = idx.begin(); it != idx.end(); it++) {
                            if (i == 1) copy(documents_path, files_right[*it].name, path, files_left, size_left);
                            else if (i == 2) move(documents_path, files_right[*it].name, path, files_right, size_right, files_left, size_left);
                            else if (i == 4) file_delete(documents_path, files_right[*it].name, files_right, size_right);
                            else {
                                char new_name[105];
                                printf("Name: ");
                                fgets(new_name, sizeof(new_name), stdin);
                                new_name[strcspn(new_name, "\n")] = 0;
                                file_rename(documents_path, files_right[*it].name, new_name, files_right, size_right); 
                            }
                        }
                    }
                 }
            }
        window.display();
        }
    }
}

// TODO: Adauga .. pt a te intoarce in folder-ul parinte
// select cu mouse, tab, sageata sus, jos