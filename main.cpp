#include "utils.h"

const float WINDOW_W = 1350.0f;
const float WINDOW_H = 900.0f;
const float PADDING = 10.0f;
const float ITEM_HEIGHT = 30.0f;
const int VISIBLE_ITEMS = 26;
const unsigned int FONT_SIZE = 20;
const unsigned int NAME_MAX_LEN = 18, SIZE_MAX_LEN = 11;
char icon_path[PATH_MAX_LEN];
bool index_side = 0; // 0 - stanga, 1 - dreapta
sf::Font font;
std::set<int> idx;
bool isDragging;
float start_x, start_y, end_x, end_y;
bool found;
int active_search;
int scroll_left, scroll_right;
bool right_click;
int right_click_x, right_click_y;
float name_offset = 5.0f, date_offset = 250.0f, type_offset = 450.0f, size_offset = 525.0f; 


struct button_data{
    sf::RectangleShape shape;
    std::string name;
    bool isPressed;
} button[10], right_click_button[10], left_pane_headers[5], right_pane_headers[5];

void SetupButton(sf::RenderWindow& window, const sf::Font& font, float x, float y, button_data &button, int i) {
    float btnSize = WINDOW_W / 7;
    button.shape.setSize(sf::Vector2f(btnSize, 50));
    button.shape.setPosition(sf::Vector2f(x+btnSize*(i-1), y)); 
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool isHovering = button.shape.getGlobalBounds().contains(mousePos);
    if(isHovering) 
        button.shape.setFillColor(sf::Color(224, 224, 224)); 
    else 
        button.shape.setFillColor(sf::Color(40, 40, 40)); 
    button.shape.setOutlineThickness(1.0f);
    button.shape.setOutlineColor(sf::Color(100, 100, 100));

    sf::Text button_text(font, button.name, FONT_SIZE);
    std::string f_shortcut;
    f_shortcut += "F";
    f_shortcut += static_cast<char>('0'+i);
    sf::Text shortcut(font, f_shortcut, 15);
    shortcut.setPosition(sf::Vector2f(x+btnSize*(i-1)+3, y+2));
    if(isHovering){
        button_text.setFillColor(sf::Color::Black);
        shortcut.setFillColor(sf::Color::Black);
    }
    else {
        button_text.setFillColor(sf::Color::White);
        shortcut.setFillColor(sf::Color::White);
    }
    sf::FloatRect textBounds = button_text.getLocalBounds();
    sf::Vector2f button_pos = button.shape.getPosition();
    float text_x = button_pos.x + (btnSize/2.0f) - (textBounds.size.x / 2.0f);
    float text_y = button_pos.y + (50 / 2.0f) - (textBounds.size.y / 2.0f);
    button_text.setPosition(sf::Vector2f(text_x-textBounds.position.x, text_y-textBounds.position.y));
    window.draw(button.shape);
    window.draw(button_text);
    window.draw(shortcut);
}

void RightClickButton(sf::RenderWindow& window, const sf::Font& font, float x, float y, button_data &button, int i) {
    button.shape.setSize(sf::Vector2f(100, 30));
    button.shape.setPosition(sf::Vector2f(x, y+30*(i-1))); 
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool isHovering = button.shape.getGlobalBounds().contains(mousePos);
    if(isHovering) 
        button.shape.setFillColor(sf::Color(224, 224, 224)); 
    else 
        button.shape.setFillColor(sf::Color(40, 40, 40)); 
    button.shape.setOutlineThickness(1.0f);
    button.shape.setOutlineColor(sf::Color(100, 100, 100));

    sf::Text button_text(font, button.name, 15);
    if(isHovering)
        button_text.setFillColor(sf::Color::Black);
    else button_text.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = button_text.getLocalBounds();
    button_text.setPosition(sf::Vector2f(x + 10.0f, y + 30 * (i-1) + 5.0f));
    window.draw(button.shape);
    window.draw(button_text);
}

void PaneHeader(sf::RenderWindow& window, float x, float y, float size_x, float size_y, button_data &button) {
    button.shape.setSize(sf::Vector2f(size_x - PADDING, size_y));
    button.shape.setPosition(sf::Vector2f(x, y)); 
    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    bool isHovering = button.shape.getGlobalBounds().contains(mousePos);
    if(isHovering) 
        button.shape.setFillColor(sf::Color(37, 150, 190, 100)); 
    else 
        button.shape.setFillColor(sf::Color::White); 
    sf::Text button_text(font, button.name, FONT_SIZE);
    button_text.setFillColor(sf::Color::Black);
    sf::FloatRect textBounds = button_text.getLocalBounds();
    button_text.setPosition(sf::Vector2f(x + PADDING, y));
    window.draw(button.shape);
    window.draw(button_text);
}

bool UpdateButton(const sf::Vector2f& mousePos, button_data &button) {
    bool actionTriggered = false; 
    if (button.shape.getGlobalBounds().contains(mousePos)) {
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


void DrawPane(sf::RenderWindow& window, const char* path_display, float x, float y, data files[], int size, bool cur_side, int scroll_offset) {
    const float PANE_W = (WINDOW_W - 3 * PADDING) / 2.0f;
    const float PANE_H = WINDOW_H - 2 * PADDING - 50;

    sf::RectangleShape pane_bg;
    pane_bg.setSize(sf::Vector2f(PANE_W, PANE_H));
    pane_bg.setPosition(sf::Vector2f(x, y)); 
    pane_bg.setFillColor(sf::Color::White); 
    pane_bg.setOutlineThickness(1.0f);
    pane_bg.setOutlineColor(sf::Color(100, 100, 100));
    window.draw(pane_bg);

    sf::Text path_text(font, path_display, FONT_SIZE);
    path_text.setPosition(sf::Vector2f(x + 5.0f, y + 5.0f));
    path_text.setFillColor(sf::Color::Black); 
    window.draw(path_text);


    const float LIST_START_Y = y + 30.0f + ITEM_HEIGHT + PADDING; 
    const float LIST_LEFT_X = x + 2.0f;
    const float LIST_ITEM_W = PANE_W - 4.0f;
    char ext_path[PATH_MAX_LEN];

    for (int i = 0; i < VISIBLE_ITEMS; ++i) { 
        int file_idx = i + scroll_offset;
        if (file_idx >= size) break;
        float item_y = LIST_START_Y + i * ITEM_HEIGHT;
        char *ext = get_extension(files[file_idx]);
        if(files[file_idx].isDir)
            ext = strdup("directory");
        else if(!ext)
            ext = strdup("default");
        strcpy(ext_path, icon_path);
        strcat(ext, ".png");
        strcat(ext_path, ext);
        struct stat file_info;
        if(stat(ext_path, &file_info)) { // file does not exist
            strcpy(ext_path, icon_path);
            strcat(ext_path, "default.png");
        }
        char item_date[50];
        std::tm *time_info = std::localtime(&files[file_idx].date);
        std::strftime(item_date, sizeof(item_date), "%Y-%m-%d %H:%M", time_info);
        char item_name[PATH_MAX_LEN];
        strncpy(item_name, files[file_idx].name, NAME_MAX_LEN);
        item_name[NAME_MAX_LEN] = 0;
        char* item_size = convert_size(files[file_idx].size);
        char item_size_arr[32];
        strncpy(item_size_arr, item_size, SIZE_MAX_LEN);
        // printf("item size %s", item_size);

        const sf::Texture texture(ext_path);
        sf::RectangleShape icon_rect;
        icon_rect.setSize(sf::Vector2f(16.0f, 16.0f));
        sf::Text item_name_text(font, item_name, FONT_SIZE), item_date_text(font, item_date, FONT_SIZE), 
        item_type_text(font, get_extension(files[file_idx]), FONT_SIZE), item_size_text(font, item_size_arr, FONT_SIZE);        
        

        
        
        if (index_side == cur_side && idx.count(file_idx)) {
            sf::RectangleShape highlight_bg;
            highlight_bg.setSize(sf::Vector2f(LIST_ITEM_W, ITEM_HEIGHT - PADDING));
            highlight_bg.setPosition(sf::Vector2f(LIST_LEFT_X, item_y - 5.0f));
            highlight_bg.setFillColor(sf::Color(37, 150, 190, 100));
            highlight_bg.setOutlineThickness(1.0f);
            highlight_bg.setOutlineColor(sf::Color(0, 0, 0));
            window.draw(highlight_bg);
        }
        icon_rect.setPosition(sf::Vector2f(x + 5.0f, y + 35.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        icon_rect.setTexture(&texture);
        item_name_text.setPosition(sf::Vector2f(x + name_offset + 20.0f, y + 30.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        item_name_text.setFillColor(sf::Color::Black); 
        item_date_text.setPosition(sf::Vector2f(x + date_offset, y + 30.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        item_date_text.setFillColor(sf::Color::Black); 
        item_type_text.setPosition(sf::Vector2f(x + type_offset, y + 30.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        item_type_text.setFillColor(sf::Color::Black); 
        item_size_text.setPosition(sf::Vector2f(x + size_offset, y + 30.0f + ITEM_HEIGHT + (i * ITEM_HEIGHT)));
        item_size_text.setFillColor(sf::Color::Black); 
        window.draw(icon_rect);
        window.draw(item_name_text);
        window.draw(item_date_text);
        window.draw(item_type_text);
        window.draw(item_size_text);
    }
    if (size > VISIBLE_ITEMS) {
        float sb_width = 10.0f;
        float track_h = PANE_H - (LIST_START_Y - y-6)*2;
        float track_x = x + PANE_W - sb_width;
        float track_y = LIST_START_Y-6.0f;

        sf::RectangleShape track(sf::Vector2f(sb_width, track_h));
        track.setPosition(sf::Vector2f(track_x, track_y));
        track.setFillColor(sf::Color(220, 220, 220));
        window.draw(track);

        float view_ratio = (float)VISIBLE_ITEMS / size;
        float thumb_h = track_h * view_ratio;
        if (thumb_h < 20.0f) thumb_h = 20.0f;

        float scroll_ratio = (float)scroll_offset / (size - VISIBLE_ITEMS);
        float thumb_y = track_y + (track_h - thumb_h) * scroll_ratio;

        sf::RectangleShape thumb(sf::Vector2f(sb_width, thumb_h));
        thumb.setPosition(sf::Vector2f(track_x, thumb_y));
        thumb.setFillColor(sf::Color(120, 120, 120));
        window.draw(thumb);
    }
}

int GetClickedIndex(float mouse_y, float pane_y, float pane_h, int size) {
    const float LIST_START_Y = pane_y + 5.0f + ITEM_HEIGHT + PADDING; 
    if (mouse_y < LIST_START_Y) return -1;
    float offset_y = mouse_y - LIST_START_Y;
    int index = (int)(offset_y / ITEM_HEIGHT);
    if (index >= 0 && index < size && mouse_y < pane_y + pane_h)
        return index;
    return -2;
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
    //window.setFramerateLimit(60);
    if (!font.openFromFile("Segoe UI.ttf")) { 
        printf("Eroare: Nu am gasit 'Segoe UI.ttf'!.\n");
        return 0;
    }
    button[1].name = "Copy";
    button[2].name = "Move";
    button[3].name = "Delete";
    button[4].name = "Rename";
    button[5].name = "New Folder";
    button[6].name = "New File";
    button[7].name = "Search";
    right_click_button[1].name = "Copy";
    right_click_button[2].name = "Move";
    right_click_button[3].name = "Delete";
    right_click_button[4].name = "Rename";
    right_click_button[5].name = "New Folder";
    right_click_button[6].name = "New File";
    right_click_button[7].name = "Search";
    left_pane_headers[1].name = "Nume";
    left_pane_headers[2].name = "Data";
    left_pane_headers[3].name = "Tip";
    left_pane_headers[4].name = "Dimensiune";
    right_pane_headers[1].name = "Nume";
    right_pane_headers[2].name = "Data";
    right_pane_headers[3].name = "Tip";
    right_pane_headers[4].name = "Dimensiune";

    // Definirea parametrilor panourilor pentru logica de click
    const float PANE_W = (WINDOW_W - 3 * PADDING) / 2.0f;
    const float LEFT_PANE_X = PADDING;
    const float LEFT_PANE_Y = PADDING;
    const float RIGHT_PANE_X = PADDING + PANE_W + PADDING;
    const float RIGHT_PANE_Y = PADDING;
    const float PANE_H = WINDOW_H - 2 * PADDING;

    std::string input = "";      
    bool input_active = false;        
    int active_action = 0;
    char new_name[105];
    int current_it;

    sf::RectangleShape input_bar;
    input_bar.setSize(sf::Vector2f(300.0f, 40.0f));
    input_bar.setPosition(sf::Vector2f(0, WINDOW_H-90)); 
    input_bar.setFillColor(sf::Color(220, 220, 220));
    input_bar.setOutlineThickness(2.0f);
    input_bar.setOutlineColor(sf::Color(40, 40, 40));

    while(window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            
            if (const auto* keypressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keypressed->code == sf::Keyboard::Key::Escape) { 
                    if(input_active)
                        input_active = false;
                    else if (active_search == 1) {
                        std::swap(files_left, search_result);
                        std::swap(size_left, size_search);
                        active_search = 0;
                    }
                    else if(active_search == 2) {
                        std::swap(files_right, search_result);
                        std::swap(size_right, size_search);
                        active_search = 0;
                    }
                    // else window.close();
                }
                if(keypressed->code == sf::Keyboard::Key::Enter && idx.size() == 1 && !input_active) {
                    int clicked_index = *(idx.begin());
                    if(!index_side) {
                        if (files_left[clicked_index+scroll_left].isDir) {
                            if (left_top < MAX_HISTORY) {
                                strcpy(left_history[left_top], path); 
                                left_top++;
                            } 
                            if (active_search == 1) {
                                        char search_path[PATH_MAX_LEN];
                                        strcpy(search_path, files_left[clicked_index+scroll_left].name);
                                        navigate(search_path, files_left[clicked_index+scroll_left].name, files_left, size_left);
                                        
                                    }
                                    else {
                                        navigate(path, files_left[clicked_index+scroll_left].name, files_left, size_left);
                                    }
                            scroll_left = 0;
                            idx.clear();
                        }
                        else open_file(path, files_left[clicked_index+scroll_left].name);
                    }
                    else {
                        if (files_right[clicked_index+scroll_right].isDir) {
                            if (left_top < MAX_HISTORY) {
                                strcpy(left_history[left_top], path); 
                                left_top++;
                            } 
                            if (active_search == 2) {
                                char search_path[PATH_MAX_LEN];
                                strcpy(search_path, files_right[clicked_index+scroll_right].name);
                                 navigate(search_path, files_right[clicked_index+scroll_right].name, files_right, size_right);        
                            }
                            else {
                                navigate(documents_path, files_right[clicked_index+scroll_right].name, files_right, size_right);
                            }
                            scroll_right = 0;
                            idx.clear();
                        }
                        else open_file(documents_path, files_right[clicked_index+scroll_right].name);
                    }
                }
                if (keypressed->code == sf::Keyboard::Key::Tab) { 
                    std::swap(files_left, files_right);
                    std::swap(size_left, size_right);
                    index_side ^= 1;
                    char aux[PATH_MAX_LEN];
                    strcpy(aux, path);
                    strcpy(path, documents_path);
                    strcpy(documents_path, aux);
                }
                if (!input_active) {
                    if (keypressed->code == sf::Keyboard::Key::F5) {
                        input_active = true; 
                        input = "";
                        active_action = 5;
                        continue;
                    }
                    if (keypressed->code == sf::Keyboard::Key::F6) {
                        input_active = true; 
                        input = "";
                        active_action = 6;
                        continue;
                    }
                    else if (keypressed->code == sf::Keyboard::Key::F7) {
                        input_active = true; 
                        input = "";
                        active_action = 7;
                        continue;
                    }
                    if (index_side == 0) {
                        std::set<int>::iterator it;
                        int file_offset = 0;
                        for (it = idx.begin(); it != idx.end(); it++) {
                            if (keypressed->code == sf::Keyboard::Key::F1) {
                                copy(path, files_left[*it-file_offset].name, documents_path, files_right, size_right);
                            }
                            else if (keypressed->code == sf::Keyboard::Key::F2) {
                                move(path, files_left[*it-file_offset].name, documents_path, files_left, size_left, files_right, size_right);
                                save_with_metadata(path, files_left, size_left);
                                save_with_metadata(documents_path, files_right, size_right);
                                file_offset++;
                            }
                            else if (keypressed->code == sf::Keyboard::Key::F3) {
                                file_delete(path,files_left[*it-file_offset].name, files_left, size_left);
                                save_with_metadata(path, files_left, size_left);
                                file_offset++;
                            }
                            else if (keypressed->code == sf::Keyboard::Key::F4){
                                if(idx.size() > 1)
                                    printf("Nu puteti redenumi mai mult de un fisier.");
                                else {
                                    input_active = true; 
                                    input.assign(files_left[*it].name);
                                    active_action = 4; 
                                    current_it = *it;
                                }
                            }
                        }
                    }
                    else {
                        std::set<int>::iterator it;
                        int file_offset = 0;
                        for (it = idx.begin(); it != idx.end(); it++) {
                            if (keypressed->code == sf::Keyboard::Key::F1) copy(documents_path, files_right[*it-file_offset].name, path, files_left, size_left);
                            else if (keypressed->code == sf::Keyboard::Key::F2) {
                                move(documents_path, files_right[*it-file_offset].name, path, files_right, size_right, files_left, size_left);
                                save_with_metadata(path, files_left, size_left);
                                save_with_metadata(documents_path, files_right, size_right);
                                file_offset++;
                            }
                            else if (keypressed->code == sf::Keyboard::Key::F3) {
                                file_delete(documents_path, files_right[*it-file_offset].name, files_right, size_right);
                                save_with_metadata(documents_path, files_right, size_right);
                                file_offset++;
                            }
                            else if (keypressed->code == sf::Keyboard::Key::F4) {
                                input_active = true; 
                                input.assign(files_right[*it].name);
                                active_action = 4; 
                                current_it = *it;
                            }
                        }
                    }
                }
                if (keypressed->code == sf::Keyboard::Key::Backspace && !input_active) {
                    if (index_side == 0 && left_top > 0) {
                        left_top--;
                        scroll_left = 0;
                        strcpy(path, left_history[left_top]);
                        save_with_metadata(path, files_left, size_left);
                        idx.clear();
                    }
                    else if (index_side == 0 && left_top == 0) {
                        navigate(path, "..", files_left, size_left);
                        scroll_left = 0;
                        idx.clear();
                    }
                    else if (index_side == 1 && right_top > 0) {
                        right_top--;
                        scroll_right = 0;
                        strcpy(documents_path, right_history[right_top]);
                        save_with_metadata(documents_path, files_right, size_right);
                        idx.clear();
                    }
                    else if (index_side == 1 && right_top == 0) {
                        navigate(documents_path, "..", files_right, size_right);
                        scroll_right = 0;
                        idx.clear();
                    }
                }
            }

            if (const auto *mouse_event = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse_event->button == sf::Mouse::Button::Left) {
                    right_click = false;
                    float mouse_x = (float)mouse_event->position.x, mouse_y = (float)mouse_event->position.y;
                    if(!isDragging)
                    {
                        isDragging = true;
                        start_x = mouse_x;
                        start_y = mouse_y;
                    }
                    end_x = mouse_x;
                    end_y = mouse_y;
                    
                    int clicked_index = -1;
                    bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                    // VERIFICARE PANOUL STANG
                    if (mouse_x >= LEFT_PANE_X && mouse_x < LEFT_PANE_X + PANE_W &&
                        mouse_y >= LEFT_PANE_Y && mouse_y < LEFT_PANE_Y + PANE_H-40) {
                        if (index_side != 0) {
                            index_side = 0;
                            isDragging = false;
                            idx.clear();
                        }
                        clicked_index = GetClickedIndex(mouse_y, LEFT_PANE_Y, PANE_H, size_left);
                        if (clicked_index >= 0) {
                            if (index_side == 0 && idx.count(clicked_index+scroll_left)) {
                                if(ctrlPressed) 
                                    idx.erase(clicked_index+scroll_left);
                                else if (files_left[clicked_index+scroll_left].isDir) {
                                    if (left_top < MAX_HISTORY) {
                                        strcpy(left_history[left_top], path); 
                                        left_top++;
                                    } 
                                    if (active_search == 1) {
                                        char search_path[PATH_MAX_LEN];
                                        strcpy(search_path, files_left[clicked_index+scroll_left].name);
                                        navigate(search_path, files_left[clicked_index+scroll_left].name, files_left, size_left);
                                        
                                    }
                                    else {
                                        navigate(path, files_left[clicked_index+scroll_left].name, files_left, size_left);
                                    }
                                    scroll_left = 0;
                                    idx.clear();
                                }
                                else {
                                    open_file(path, files_left[clicked_index+scroll_left].name);
                                }
                            }
                            else {
                                if(index_side || !ctrlPressed) {
                                    idx.clear();
                                    index_side = 0;
                                }
                                if(!idx.count(clicked_index+scroll_left))
                                    idx.insert(clicked_index+scroll_left);
                                else idx.erase(clicked_index+scroll_left);
                            }
                        }
                        else {
                            idx.clear();
                            index_side = 0;
                        }
                    }
                    
                    // VERIFICARE PANOUL DREPT
                    else if (mouse_x >= RIGHT_PANE_X && mouse_x < RIGHT_PANE_X + PANE_W &&
                            mouse_y >= RIGHT_PANE_Y && mouse_y < RIGHT_PANE_Y + PANE_H-40) {
                        if (index_side != 1) {
                            index_side = 1;
                            isDragging = false;
                            idx.clear();
                        }
                        clicked_index = GetClickedIndex(mouse_y, RIGHT_PANE_Y, PANE_H, size_right);
                        if (clicked_index >= 0) {
                            if (index_side == 1 && idx.count(clicked_index+scroll_right)) {
                                if(ctrlPressed) 
                                    idx.erase(clicked_index+scroll_right);
                                else if (files_right[clicked_index+scroll_right].isDir) {
                                    if (right_top < MAX_HISTORY) {
                                        strcpy(right_history[right_top], documents_path); 
                                        right_top++;
                                    } 
                                    if (active_search == 2) {
                                        char search_path[PATH_MAX_LEN];
                                        strcpy(search_path, files_right[clicked_index+scroll_right].name);
                                        navigate(search_path, files_right[clicked_index+scroll_right].name, files_right, size_right);
                                        
                                    }
                                    else {
                                        navigate(documents_path, files_right[clicked_index+scroll_right].name, files_right, size_right);
                                    }
                                    scroll_right = 0;
                                    idx.clear();
                                }
                                else {
                                    open_file(documents_path, files_right[clicked_index+scroll_right].name);
                                }
                            }
                            else {
                                if(!index_side || !ctrlPressed) {
                                    idx.clear();
                                    index_side = 1;
                                }
                                if(!idx.count(clicked_index+scroll_right))
                                    idx.insert(clicked_index+scroll_right);
                                else idx.erase(clicked_index+scroll_right);
                            }
                        }
                        else {
                            idx.clear();
                            index_side = 1;
                        }
                    }
                }
                else if (mouse_event->button == sf::Mouse::Button::Right) {
                    bool ctrlPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl);
                    right_click_x = mouse_event->position.x;
                    right_click_y = mouse_event->position.y;
                    right_click = true;
                    if (right_click_x >= LEFT_PANE_X && right_click_x < LEFT_PANE_X + PANE_W) {
                        if(index_side) {
                            idx.clear();
                            index_side = 0;
                        }
                    }
                    else 
                        if(!index_side) {
                            idx.clear();
                            index_side = 1;
                        }

                    int index;
                    if(!index_side) index = GetClickedIndex(right_click_y, LEFT_PANE_Y, PANE_H, size_left);
                    else index = GetClickedIndex(right_click_y, RIGHT_PANE_Y, PANE_H, size_right);
                    if(!ctrlPressed) 
                        idx.clear();   
                    if(index_side >= 0)
                        if (!index_side) 
                            idx.insert(index + scroll_left);
                        else idx.insert(index + scroll_right);
                    else {
                        if(idx.empty())
                        {
                            if(index == -1)
                                idx.insert(0);
                            else idx.insert(!index_side ? size_left - 1 : size_right - 1);
                        }
                    }
                }
            }
            if (const auto *move_event = event->getIf<sf::Event::MouseMoved>())
                end_x = (float)move_event->position.x, end_y = (float)move_event->position.y;
            if (const auto *release_event = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (isDragging && release_event->button == sf::Mouse::Button::Left) {
                    isDragging = false;
                    end_x = (float)release_event->position.x, end_y = (float)release_event->position.y;
                    // PANOUL STANG
                    if (start_x >= LEFT_PANE_X && start_x < LEFT_PANE_X + PANE_W && start_y >= LEFT_PANE_Y && start_y < LEFT_PANE_Y + PANE_H) {
                        int first_index = GetClickedIndex(start_y, LEFT_PANE_Y, PANE_H, size_left);
                        if(first_index == -1)
                            first_index = 0;
                        else if(first_index == -2)
                            first_index = size_left - 1;
                        int last_index = GetClickedIndex(end_y, LEFT_PANE_Y, PANE_H, size_left);
                        if(last_index == -1) 
                            last_index = 0;
                        else if(last_index == -2)
                            last_index = size_left - 1;
                        if(last_index < first_index)
                            std::swap(last_index, first_index);
                        if(first_index != last_index) {
                            for(int i = first_index; i <= last_index; i++)
                                idx.insert(i+scroll_left);
                            }
                    }
                    // PANOUL DREPT
                    else if (start_x >= RIGHT_PANE_X && start_x < RIGHT_PANE_X + PANE_W &&
                            start_y >= RIGHT_PANE_Y && start_y < RIGHT_PANE_Y + PANE_H-40) {
                        int first_index = GetClickedIndex(start_y, RIGHT_PANE_Y, PANE_H, size_right);
                        if(first_index == -1)
                            first_index = 0;
                        else if(first_index == -2)
                        first_index = size_right - 1;
                        int last_index = GetClickedIndex(end_y, RIGHT_PANE_Y, PANE_H, size_right);
                        if(last_index == -1) 
                            last_index = 0;
                        else if(last_index == -2)
                            last_index = size_right - 1;
                        if(last_index < first_index)
                            std::swap(last_index, first_index);
                        if(first_index != last_index) {
                            for(int i = first_index; i <= last_index; i++)
                                idx.insert(i+scroll_right);
                        }
                    }
                }

            }
                
            if (input_active) {
                if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                    // backspace
                    if (textEvent->unicode == 8) { 
                        if (!input.empty()) input.pop_back();
                    }
                    // enter
                    else if (textEvent->unicode == 13) {
                        input_active = false;
                        strcpy(new_name,input.c_str());
                        if (index_side) {
                            if (active_action == 4) 
                                file_rename(documents_path, files_right[current_it].name, new_name, files_right, size_right);
                            else if (active_action == 5) {
                                create_folder(documents_path, new_name, files_right, size_right);
                                idx.clear();
                                idx.insert(size_right - 1);
                            }
                            else if (active_action == 6) {
                                create_file(documents_path, new_name, files_right, size_right);
                                save_with_metadata(documents_path, files_right, size_right);
                                idx.clear();
                                idx.insert(size_right - 1);
                            }
                            else if (active_action == 7) {
                                idx.clear();
                                found = 0;
                                active_search = 2;
                                size_search = 0;
                                search(new_name, documents_path, found);
                                std::swap(search_result, files_right);
                                std::swap(size_right, size_search);
                            }
                        }
                        else {
                            if (active_action == 4) 
                                file_rename(path, files_left[current_it].name, new_name, files_left, size_left);
                            else if (active_action == 5) {
                                create_folder(path, new_name, files_left, size_left);
                                idx.clear();
                                idx.insert(size_left - 1);
                            }
                            else if (active_action == 6) {
                                create_file(path, new_name, files_left, size_left);
                                save_with_metadata(path, files_left, size_left);
                                idx.clear();
                                idx.insert(size_left - 1);
                            }
                            else if (active_action == 7) {
                                idx.clear();
                                found = 0;
                                active_search = 1;
                                size_search = 0;
                                search(new_name, path, found);
                                std::swap(search_result, files_left);
                                std::swap(size_left, size_search);
                            }
                        }
                        input = "";
                    }
                        // restul caracterelor
                    else if (textEvent->unicode > 31 && textEvent->unicode < 128) {
                        input += static_cast<char>(textEvent->unicode);
                    }
                }
            }

            if (const auto *scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (scroll->wheel == sf::Mouse::Wheel::Vertical) {
                    int *current_scroll;
                    int current_size;
                    if (!index_side) {
                        current_scroll = &scroll_left;
                        current_size = size_left;
                    }
                    else {
                        current_scroll = &scroll_right;
                        current_size = size_right;
                    }
                    if (scroll->delta > 0) {
                        if (*current_scroll > 0) (*current_scroll)--;
                    }
                    else if (scroll->delta < 0) {
                        if (*current_scroll + VISIBLE_ITEMS < current_size) (*current_scroll)++;
                    }
                }
            }    
        }
        window.clear(sf::Color(30, 30, 30)); 
        
        char res[50];
        strcpy(res, "Search results:");
        if (found == 0) strcpy(res, "No items match your search.");
        strcat(res, " Press ESC to go back.");
        // Deseneaza panoul stang
        if (active_search == 1) DrawPane(window, res, PADDING, PADDING, files_left, size_left, 0, scroll_left);
        else DrawPane(window, path, PADDING, PADDING, files_left, size_left, 0, scroll_left);
        // Deseneaza panoul drept 
        if (active_search == 2) DrawPane(window, res, PADDING + PANE_W + PADDING, PADDING, files_right, size_right, 1, scroll_right);
        else DrawPane(window, documents_path, PADDING + PANE_W + PADDING, PADDING, files_right, size_right, 1, scroll_right);

        if (input_active) {
            window.draw(input_bar);
            sf::Text displayInput(font, input, FONT_SIZE);
            displayInput.setPosition(sf::Vector2f(input_bar.getPosition().x + 5.0f, input_bar.getPosition().y + 5.0f));
            displayInput.setFillColor(sf::Color::Black);

            window.draw(displayInput);
        }

        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        PaneHeader(window, name_offset + PADDING, PADDING + 30, date_offset - name_offset, 25, left_pane_headers[1]);
        PaneHeader(window, date_offset, PADDING + 30, type_offset - date_offset, 25, left_pane_headers[2]);
        PaneHeader(window, type_offset, PADDING + 30, size_offset - type_offset, 25, left_pane_headers[3]);
        PaneHeader(window, size_offset, PADDING + 30, PANE_W - size_offset, 25, left_pane_headers[4]);
        
        PaneHeader(window, RIGHT_PANE_X + name_offset, PADDING + 30, date_offset - name_offset, 25, right_pane_headers[1]);
        PaneHeader(window, RIGHT_PANE_X + date_offset, PADDING + 30, type_offset - date_offset, 25, right_pane_headers[2]);
        PaneHeader(window, RIGHT_PANE_X + type_offset, PADDING + 30, size_offset - type_offset, 25, right_pane_headers[3]);
        PaneHeader(window, RIGHT_PANE_X + size_offset, PADDING + 30, PANE_W - size_offset, 25, right_pane_headers[4]);


        for (int i = 1;i <= 7;++i) {
            SetupButton(window, font, 0,  WINDOW_H-50, button[i], i);
            if(right_click)
                RightClickButton(window, font, right_click_x, right_click_y, right_click_button[i], i);
                if (UpdateButton(mousePos, button[i]) || (right_click && UpdateButton(mousePos, right_click_button[i]))) {
                // pentru cazul in care este selectat un fisier din panoul stang
                if (!index_side) {
                    if (i == 5) {
                        input_active = true; 
                        input = "";
                        active_action = 5;
                        continue;
                    }
                    else if (i == 6) {
                        input_active = true; 
                        input = "";
                        active_action = 6;
                        continue;
                    }
                    else if (i == 7) {
                        input_active = true; 
                        input = "";
                        active_action = 7;
                        continue;
                    }
                    std::set<int>::iterator it;
                    int file_offset = 0;
                    for (it = idx.begin(); it != idx.end(); it++) {
                        if (i == 1) {
                            copy(path, files_left[*it-file_offset].name, documents_path, files_right, size_right);
                        }
                        else if (i == 2) {
                            move(path, files_left[*it-file_offset].name, documents_path, files_left, size_left, files_right, size_right);
                            save_with_metadata(path, files_left, size_left);
                            save_with_metadata(documents_path, files_right, size_right);
                            file_offset++;
                        }
                        else if (i == 3) {
                            file_delete(path,files_left[*it-file_offset].name, files_left, size_left);
                            save_with_metadata(path, files_left, size_left);
                            file_offset++;
                        }
                        else if (i == 4){
                            if(idx.size() > 1)
                                printf("Nu puteti redenumi mai mult de un fisier.");
                            else {
                                input_active = true; 
                                input.assign(files_left[*it].name);
                                active_action = 4; 
                                current_it = *it;
                            }
                        }
                    }
                }
                // pentru cazul in care este selectat un fisier din panoul drept
                else if (index_side) {
                    if (i == 5) {
                        input_active = true; 
                        input = "";
                        active_action = 5;
                        continue;
                    }
                    if (i == 6) {
                        input_active = true; 
                        input = "";
                        active_action = 6;
                        continue;
                    }
                    else if (i == 7) {
                        input_active = true; 
                        input = "";
                        active_action = 7;
                        continue;
                    }
                    std::set<int>::iterator it;
                    int file_offset = 0;
                    for (it = idx.begin(); it != idx.end(); it++) {
                        if (i == 1) copy(documents_path, files_right[*it-file_offset].name, path, files_left, size_left);
                        else if (i == 2) {
                            move(documents_path, files_right[*it-file_offset].name, path, files_right, size_right, files_left, size_left);
                            save_with_metadata(path, files_left, size_left);
                            save_with_metadata(documents_path, files_right, size_right);
                            file_offset++;
                        }
                        else if (i == 3) {
                            file_delete(documents_path, files_right[*it-file_offset].name, files_right, size_right);
                            save_with_metadata(documents_path, files_right, size_right);
                            file_offset++;
                        }
                        else if (i == 4) {
                            input_active = true; 
                            input.assign(files_right[*it].name);
                            active_action = 4; 
                            current_it = *it;
                        }
                    }
                }
            }
            sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if(i < 5 && UpdateButton(mousePos, left_pane_headers[i])) {
                bool sort_order_option = 0;
                char option_arr[20];
                int string_size = left_pane_headers[i].name.size();
                for(int j = 0; j < string_size; j++) 
                    option_arr[j] = left_pane_headers[i].name[j];
                option_arr[string_size] = 0;
                if(!strcmp(option_arr, last_sort_option))
                    sort_order_option = last_sort_order^1;
                sort_files(option_arr, sort_order_option, files_left, size_left);
            }
            else if(i < 5 && UpdateButton(mousePos, right_pane_headers[i])) {
                bool sort_order_option = 0;
                char option_arr[20];
                int string_size = right_pane_headers[i].name.size();
                for(int j = 0; j < string_size; j++) 
                    option_arr[j] = right_pane_headers[i].name[j];
                option_arr[string_size] = 0;
                if(!strcmp(option_arr, last_sort_option))
                    sort_order_option = last_sort_order^1;
                sort_files(option_arr, sort_order_option, files_right, size_right);
            }
        }
        if(isDragging) {
            float x1 = std::min(start_x, end_x), x2 = std::max(start_x, end_x), y1 = std::min(start_y, end_y), y2 = std::max(start_y, end_y);
            sf::RectangleShape marquee;
            marquee.setSize(sf::Vector2f(x2-x1, y2-y1));
            marquee.setFillColor(sf::Color(37, 150, 190, 100));
            marquee.setOutlineThickness(1.0f);
            marquee.setOutlineColor(sf::Color(0, 0, 0));
            marquee.setPosition(sf::Vector2f(x1, y1)); 
            window.draw(marquee);
        }
        window.display();
    }
}

