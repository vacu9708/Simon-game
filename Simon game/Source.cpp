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

	bool click_checker(RenderWindow& window) {
		auto mouse_pos = Mouse::getPosition(window); // Mouse position relative to the window
		auto translated_pos = window.mapPixelToCoords(mouse_pos);
		return button.getGlobalBounds().contains(translated_pos);
	}

	void operation(SoundBuffer& soundbuffer, int stage) {
		button.setFillColor(Color::Transparent); // Turning on the button
		sound.setBuffer(soundbuffer);
		sound.play();
		if (stage >= 8)
			stage = 8;
		Sleep(1000 - stage * 100);
		sound.stop();
		button.setFillColor(color); // Get back the original color
	}

	void wrong_click(SoundBuffer& soundbuffer, int stage) {
		button.setFillColor(Color::Black);
		sound.setBuffer(soundbuffer);
		sound.play();
		while(sound.getStatus() == Sound::Playing);
		sound.stop();
	}

	void draw_button(RenderWindow& window) {
		window.draw(button);
	}
};

bool is_gameover = false;
class Button_set {
	int stage = 1;
	Font font;
	Text stage_displayer;
	int number_of_buttons;
	vector<Button> buttons;
	SoundBuffer soundbuffer[11];
	Color colors[11] =
	{ Color::Blue, Color::Cyan, Color::Green, Color::Magenta, Color::Red, Color::Yellow, 
		Color(0,102,102), Color(102,0,204), Color(153,255,51), Color(255,102,150),
		Color::Black };
public:
	Button_set(int number_of_buttons) {
		font.loadFromFile("resources/AGENCYR.ttf");
		stage_displayer.setFont(font);
		stage_displayer.setFillColor(Color::Black);
		stage_displayer.setCharacterSize(33);
		stage_displayer.setPosition(11, 11);

		this->number_of_buttons = number_of_buttons;
		int x = 11, y = 111;
		for (int i = 0; i < number_of_buttons; i++) {
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

	void stages(RenderWindow& window) {
		stage = 1;
		while (true) {
			list<int> answers;
			int random_int;
			Sleep(555);
			for (int i = 0; i < stage; i++) { // Show the answer
				random_int = random_integer(0, number_of_buttons - 1);
				buttons[random_int].operation(soundbuffer[random_int], stage);
				answers.push_back(random_int);
			}

			list<int>::iterator answer_crawler = answers.begin(); // Guessing the answer
			for (int i = 0; i < stage; i++) {
				char clicked_button = -1;
				while (clicked_button < 0) // Wait until click
					if (Mouse::isButtonPressed(Mouse::Left) == true)
						for (int j = 0; j < number_of_buttons; j++)
							if (buttons[j].click_checker(window) == true) {
								clicked_button = j;
								break;
							}

				if (*answer_crawler == clicked_button) { // Correct click
					buttons[clicked_button].operation(soundbuffer[clicked_button], stage);
					answer_crawler++;
				}
				else { // Wrong click		
					buttons[clicked_button].wrong_click(soundbuffer[10], stage);
					is_gameover = true;
					return;
				}
			}
			stage++; // To the next stage
		}
	}

	void draw_buttons(RenderWindow& window) {
		for (Button& i : buttons)
			i.draw_button(window);
	}

	void draw_stage(RenderWindow& window) {
		stage_displayer.setString("Stage : " + to_string(stage));
		window.draw(stage_displayer);
	}
};

class Start_choice {
	Text choice_text;
public:
	CircleShape choice;
	Start_choice(int number, int x, int y, Font& font, RenderWindow& window) {
		choice.setRadius(55);
		choice.setFillColor(Color::Green);
		choice.setPosition(x, y);
		window.draw(choice);

		choice_text.setFont(font);
		choice_text.setFillColor(Color::Yellow);
		choice_text.setCharacterSize(44);
		choice_text.setPosition(x + 44, y + 22);
		choice_text.setString(to_string(number));
		window.draw(choice_text);
	}

	bool check_click(RenderWindow& window) {
		auto mouse_pos = Mouse::getPosition(window); // Mouse position relative to the window
		auto translated_pos = window.mapPixelToCoords(mouse_pos);
		return choice.getGlobalBounds().contains(translated_pos);
	}
};

void start(int& number_of_buttons, RenderWindow& window) {
	window.clear(Color::Black);

	Font font; // Start text
	font.loadFromFile("resources/AGENCYR.ttf");
	Text start;
	start.setFont(font);
	start.setFillColor(Color::Yellow);
	start.setCharacterSize(44);
	start.setPosition(11, 11);
	start.setString("Choose the number of buttons you want");
	window.draw(start);

	vector<Start_choice> start_choices;
	int x = 11, y = 133; // Show the choices to decide the number of buttons
	for (int i = 4; i <= 10; i++) {
		start_choices.push_back(Start_choice(i, x, y, font, window));
		x += 120;
		if (x > 650) {
			y += 120;
			x = 11;
		}
	}

	window.display();

	Event event;
	while (number_of_buttons == 0) // Decide the number of buttons
		if (window.pollEvent(event))
			if (Mouse::isButtonPressed(Mouse::Left) == true)
					for (int i = 0; i < 7; i++) {
						if (start_choices[i].check_click(window) == true) {
							number_of_buttons = i + 4;
							window.clear(Color::White);
							window.display();
							break;
						}
					}
}

int main() {
	RenderWindow window(VideoMode(800, 600), "Simon game"); // Window
	window.setFramerateLimit(60);

	int number_of_buttons = 0;
	start(number_of_buttons, window);

	Button_set* button_set = new Button_set(number_of_buttons);
	thread thread1(&Button_set::stages, button_set, ref(window));
	thread1.detach();

	while (window.isOpen()) {
		window.clear(Color::White);

		button_set->draw_buttons(window);
		button_set->draw_stage(window);

		window.display();

		if (is_gameover == true) {
			window.clear(Color::Black);

			Font font; // Game over
			font.loadFromFile("resources/AGENCYR.ttf");
			Text gameover;
			gameover.setFont(font);
			gameover.setFillColor(Color::Red);
			gameover.setCharacterSize(88);
			gameover.setPosition(0, 200);
			gameover.setString("             GAME OVER\n     Right click to restart");
			window.draw(gameover);

			window.display();

			while (true) // Right click to restart
				if (Mouse::isButtonPressed(Mouse::Right) == true) {
					number_of_buttons = 0;
					start(number_of_buttons, window);
					is_gameover = false;

					delete button_set;
					button_set = new Button_set(number_of_buttons);
					thread thread1(&Button_set::stages, button_set, ref(window));
					thread1.detach();
					break;
				}
		}
	}
}