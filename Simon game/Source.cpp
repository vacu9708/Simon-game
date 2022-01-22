#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <windows.h> // for Sleep();
#include <random>
#include <list>
#include <thread>
#include <string>

using namespace std;
using namespace sf;

int random_integer(int min, int max) {
	random_device seed; // Generate a random seed
	mt19937_64 mersenne(seed());
	uniform_int_distribution<> range(min, max);
	return range(mersenne);
}

RenderWindow* window;
short n_of_buttons = 0;

class Start_choice {
	Text choice_text;
public:
	CircleShape choice;
	Start_choice(short number, int x, int y, Font& font) {
		// Draw circles
		choice.setRadius(55);
		choice.setFillColor(Color::Green);
		choice.setPosition(x, y);
		window->draw(choice);
		//-----
		// Write a number in the circles
		choice_text.setFont(font);
		choice_text.setFillColor(Color::Yellow);
		choice_text.setCharacterSize(44);
		choice_text.setPosition(x + 44, y + 22);
		choice_text.setString(to_string(number));
		window->draw(choice_text);
		//-----
	}

	bool check_click() {
		auto mouse_pos = Mouse::getPosition(*window); // Mouse position relative to the window
		auto translated_pos = window->mapPixelToCoords(mouse_pos);
		return choice.getGlobalBounds().contains(translated_pos);
	}
};

void start() {
	//unique_ptr<RenderWindow> window(new RenderWindow(VideoMode(800, 600), "Simon game")); // Smart pointer doesn't work
	delete window;
	window = new RenderWindow(VideoMode(800, 600), "Simon game");
	window->setFramerateLimit(60);
	window->clear(Color::Black);

	// Start text
	Font font;
	font.loadFromFile("resources/AGENCYR.ttf");
	Text start;
	start.setFont(font);
	start.setFillColor(Color::Yellow);
	start.setCharacterSize(44);
	start.setPosition(11, 11);
	start.setString("Choose the number of buttons you want");
	window->draw(start);
	//-----

	vector<Start_choice> start_choices;
	short x = 11, y = 133; // Show the choices to decide the number of buttons
	for (int i = 0; i < 7; i++) {
		start_choices.push_back(Start_choice(i + 4, x, y, font));
		x += 120;
		if (x > 650) {
			y += 120;
			x = 11;
		}
	}
	window->display();

	Event event;
	while (n_of_buttons == 0) // Decide the number of buttons
		if (window->pollEvent(event))
			if (Mouse::isButtonPressed(Mouse::Left) == true) // Click check
				for (int i = 0; i < 7; i++)
					if (start_choices[i].check_click() == true) { // Which button was clicked?
						n_of_buttons = i + 4;
						window->clear(Color::White);
						window->display();
						break;
					}
}

class Button {
	CircleShape button;
	Color color;

	Sound sound;
public:
	Button(Color color, int x, int y) {
		this->color = color;
		button.setRadius(55);
		button.setFillColor(color);
		button.setPosition(x, y);
	}

	bool click_checker() {
		auto mouse_pos = Mouse::getPosition(*window); // Mouse position relative to the window
		auto translated_pos = window->mapPixelToCoords(mouse_pos);
		return button.getGlobalBounds().contains(translated_pos);
	}

	void operation(SoundBuffer& soundbuffer, int stage) {
		button.setFillColor(Color::Transparent); // Turning on the button
		sound.setBuffer(soundbuffer);
		sound.play();
		if (stage >= 8) // To limit the speed
			stage = 8;
		Sleep(1000 - stage * 100);
		sound.stop();
		button.setFillColor(color); // Get back the original color
	}

	void wrong_click(SoundBuffer& soundbuffer) {
		button.setFillColor(Color::Black);
		sound.setBuffer(soundbuffer);
		sound.play();
		while(sound.getStatus() == Sound::Playing); // Wait until the sound ends
	}

	void draw_button() {
		window->draw(button);
	}
};

bool is_gameover = false;
int stage = 1;
Text *stage_displayer;
class Button_set {
	Font font;
	short n_of_buttons;
	vector<Button> buttons;
	SoundBuffer soundbuffer[11];
	Color colors[11] =
	{ Color::Blue, Color::Cyan, Color::Green, Color::Magenta, Color::Red, Color::Yellow, 
		Color(0,102,102), Color(102,0,204), Color(153,255,51), Color(255,102,150),
		Color::Black };
public:
	Button_set(short n_of_buttons) {
		font.loadFromFile("resources/AGENCYR.ttf");
		delete stage_displayer;
		stage_displayer = new Text();
		stage_displayer->setFont(font);
		stage_displayer->setFillColor(Color::Black);
		stage_displayer->setCharacterSize(33);
		stage_displayer->setPosition(11, 11);

		this->n_of_buttons = n_of_buttons;
		short x = 11, y = 111;
		for (int i = 0; i < n_of_buttons; i++) {
			buttons.push_back(Button(colors[i], x, y));
			x += 120;
			if (x > 650) {
				y += 120;
				x = 11;
			}
		}

		soundbuffer[0].loadFromFile("resources/A3-49-96.wav"); 
		soundbuffer[1].loadFromFile("resources/B3-49-96.wav");
		soundbuffer[2].loadFromFile("resources/A4.wav"); 
		soundbuffer[3].loadFromFile("resources/B4.wav");
		soundbuffer[4].loadFromFile("resources/C4.wav"); 
		soundbuffer[5].loadFromFile("resources/D4.wav");
		soundbuffer[6].loadFromFile("resources/E4.wav"); 
		soundbuffer[7].loadFromFile("resources/F4.wav");
		soundbuffer[8].loadFromFile("resources/G4.wav"); 
		soundbuffer[9].loadFromFile("resources/C5.wav");
		soundbuffer[10].loadFromFile("resources/wrong answer.wav");
	}

	void stages() {
		stage = 1;
		while (true) {
			list<int> answers;
			int random_int;
			Sleep(555);
			for (int i = 0; i < stage; i++) { // Showing buttons to remember
				random_int = random_integer(0, n_of_buttons - 1);
				buttons[random_int].operation(soundbuffer[random_int], stage);
				answers.push_back(random_int);
			}

			list<int>::iterator answer_crawler = answers.begin(); // Guessing the answer
			for (int i = 0; i < stage; i++) {
				// Click detection
				char clicked_button = -1;
				while (clicked_button < 0) // Wait until click
					if (Mouse::isButtonPressed(Mouse::Left) == true)
						for (int j = 0; j < n_of_buttons; j++)
							if (buttons[j].click_checker() == true) {
								clicked_button = j;
								break;
							}
				//-----
				if (*answer_crawler == clicked_button) { // Correct click?
					buttons[clicked_button].operation(soundbuffer[clicked_button], stage);
					answer_crawler++;
				}
				else { // Wrong clic
					buttons[clicked_button].wrong_click(soundbuffer[10]);
					is_gameover = true;
					return;
				}
			}

			stage++; // To the next stage
		}
	}

	void draw_buttons() {
		for (Button& i : buttons)
			i.draw_button();
	}
};

void draw_stage() {
	stage_displayer->setString("Stage : " + to_string(stage));
	window->draw(*stage_displayer);
}

void game_over(Button_set *button_set) {
	window->clear(Color::Black);

	Font font; // Game over
	font.loadFromFile("resources/AGENCYR.ttf");
	Text gameover;
	gameover.setFont(font);
	gameover.setFillColor(Color::Red);
	gameover.setCharacterSize(88);
	gameover.setPosition(0, 200);
	gameover.setString("             GAME OVER\n     Right click to restart");
	window->draw(gameover);

	window->display();

	while (true) // Right click to restart
		if (Mouse::isButtonPressed(Mouse::Right) == true) {
			n_of_buttons = 0;
			start();
			is_gameover = false;
			delete button_set;
			button_set = new Button_set(n_of_buttons);
			thread thread1(&Button_set::stages, button_set);
			thread1.detach();
			break;
		}
}

int main() {
	start();

	Button_set* button_set = new Button_set(n_of_buttons);
	thread thread1(&Button_set::stages, button_set);
	thread1.detach();

	while (window->isOpen()) {
		window->clear(Color::White);

		button_set->draw_buttons();
		draw_stage();

		window->display();

		if (is_gameover == true)
			game_over(button_set);
	}
}
