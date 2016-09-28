/*
	Ricardo Tejada 2016
	Alt Ctrl Game Jam 2016
	Green Thumb - Help these flowers grow by watering them and controlling the weather!
*/

#include <iostream>
#include <vector>
#include <cstdlib>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Log.h"
#include "Serial.h"

struct WaterDrop : public sf::Sprite
{
	WaterDrop(sf::Texture& texture, unsigned char power) : sf::Sprite(texture)
	{
		m_life = 30;
		m_splashed = false;

		move(186, 168);
		setOrigin(8, 8);

		power = power / 32;
		
		// A whole bunch of hacky math for effects
		m_speed = sf::Vector2f(-(rand() % 6 + power), 6 + rand() % 4 + (5 / (1 + power)));
		m_spin = rand() % 8;
		setScale(0.25 + rand() % 2 * 0.25 * power, 0.25 + 0.25 * power);
	}

	int m_life;
	bool m_splashed;
	sf::Vector2f m_speed;
	float m_spin;
};

int main(int argc, char* argvp[])
{
	sf::RenderWindow window(sf::VideoMode(480, 640), "Green Thumb - Alt Ctrl Jam 2016", sf::Style::Close);
	sf::View camera(sf::FloatRect(0, 0, 240, 320));
	window.setView(camera);

	sf::Clock clock;
	float frame_limit = 1.0f / 60.0f;
	float frame_sum = 0;
	int frame_count = 10;

	srand(clock.getElapsedTime().asSeconds());

	Serial serial("COM3");

	if(serial.isConnected())
		moony::logDebug() << "Serial connection established.";

	char input_data = 0;
	unsigned char button = 0;
	unsigned char water_power = 0;
	unsigned char water_anim = 0;
	unsigned char solar_power = 0;
	unsigned char spawn_delay = 0;

	unsigned char color_red = 0;
	unsigned char color_green = 0;
	unsigned char color_blue = 0;

	int need_water = 0;
	int need_sun = 0;

	sf::Texture tex_pipe_head;
	if(!tex_pipe_head.loadFromFile("resources/pipe_head.png"))
	{
		moony::logError() << "Failed to load pipe_head.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_pipe_knob;
	tex_pipe_knob.setRepeated(true);
	if(!tex_pipe_knob.loadFromFile("resources/pipe_knob.png"))
	{
		moony::logError() << "Failed to load pipe_knob.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_pipe_body;
	tex_pipe_body.setRepeated(true);
	if(!tex_pipe_body.loadFromFile("resources/pipe_body.png"))
	{
		moony::logError() << "Failed to load pipe_body.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_grass;
	tex_grass.setRepeated(true);
	if(!tex_grass.loadFromFile("resources/grass_tile.png"))
	{
		moony::logError() << "Failed to load grass_tile.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_water;
	if(!tex_water.loadFromFile("resources/water.png"))
	{
		moony::logError() << "Failed to load water.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_splash;
	if(!tex_splash.loadFromFile("resources/splash.png"))
	{
		moony::logError() << "Failed to load splash.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_pot;
	if(!tex_pot.loadFromFile("resources/pot.png"))
	{
		moony::logError() << "Failed to load pot.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_sun;
	if(!tex_sun.loadFromFile("resources/sun.png"))
	{
		moony::logError() << "Failed to load sun.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_cloud;
	if(!tex_cloud.loadFromFile("resources/cloud.png"))
	{
		moony::logError() << "Failed to load cloud.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_flower;
	if(!tex_flower.loadFromFile("resources/flower.png"))
	{
		moony::logError() << "Failed to load flower.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_faces;
	if(!tex_faces.loadFromFile("resources/faces.png"))
	{
		moony::logError() << "Failed to load faces.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_stem;
	if(!tex_stem.loadFromFile("resources/stem.png"))
	{
		moony::logError() << "Failed to load stem.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_needs;
	if(!tex_needs.loadFromFile("resources/needs.png"))
	{
		moony::logError() << "Failed to load needs.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Texture tex_bg;
	if(!tex_bg.loadFromFile("resources/background.png"))
	{
		moony::logError() << "Failed to load background.png";
		moony::logSaveToFile();
		return -1;
	}

	sf::Sprite spr_bg(tex_bg);

	sf::Sprite spr_sun(tex_sun);
	spr_sun.move(16, 16);

	sf::Sprite spr_cloud_a(tex_cloud);
	spr_cloud_a.setColor(sf::Color(255, 255, 255, 225));

	sf::Sprite spr_cloud_b(tex_cloud);
	spr_cloud_b.setColor(sf::Color(255, 255, 255, 225));

	sf::Sprite spr_pipe_head(tex_pipe_head);
	spr_pipe_head.move(176, 160);
	sf::Sprite spr_pipe_knob(tex_pipe_knob, sf::IntRect(0, 0, 32, 16));
	spr_pipe_knob.move(192, 144);
	sf::Sprite spr_pipe_body(tex_pipe_body, sf::IntRect(0, 0, 16, 200));
	spr_pipe_body.move(200, 192);


	sf::Sprite spr_flower(tex_flower);
	spr_flower.move(88, 160);

	sf::Sprite spr_face(tex_faces, sf::IntRect(0, 0, 32, 32));
	spr_face.move(104, 176);

	sf::Sprite spr_stem(tex_stem);
	spr_stem.move(104, 224);

	sf::Sprite spr_pot(tex_pot);
	spr_pot.move(88, 240);

	sf::RectangleShape spr_pot_bbox(sf::Vector2f(40, 68));
	spr_pot_bbox.setOutlineColor(sf::Color::Red);
	spr_pot_bbox.setOutlineThickness(1);
	spr_pot_bbox.move(100, 254);

	sf::Sprite spr_grass(tex_grass, sf::IntRect(0, 0, 240, 64));
	spr_grass.move(0, 288);

	sf::RectangleShape spr_shadow(sf::Vector2f(240, 320));
	spr_shadow.setFillColor(sf::Color::Black);

	std::vector<WaterDrop> spr_water;

	while(window.isOpen() && serial.isConnected())
	{
		frame_sum += clock.restart().asSeconds();
		frame_count = 0;


		serial.read(&input_data, 1);

		if(input_data == 'A')		// Check for start of input_data
		{
			serial.read(&input_data, 1);
			water_power = input_data;

			serial.read(&input_data, 1);
			water_anim = input_data;

			serial.read(&input_data, 1);
			solar_power = input_data;

			serial.read(&input_data, 1);
			button  = input_data;
		}
		else
			serial.write("A", 1);	// Tell Arduino we are ready for input_data

		Sleep(20);


		while(frame_sum > frame_limit && frame_count < 10)
		{
			sf::Event event;

			while(window.pollEvent(event))
			{
				if(event.type == sf::Event::Closed)
					window.close();
			}

			// Update code
			if(spawn_delay < 1 && water_power > 0)
			{
				spr_water.push_back(WaterDrop(tex_water, water_power));
				spawn_delay = 50 / (1 + water_power);
			}

			for(int i = 0; i < spr_water.size(); i++)
			{
				if(spr_water[i].m_life < 0) 
				{
					std::swap(spr_water[i], spr_water.back());
					spr_water.pop_back();
					i--;
				}
				else
				{
					if(spr_water[i].getGlobalBounds().intersects(spr_pot_bbox.getGlobalBounds()) && spr_water[i].m_splashed == false)
					{
						spr_water[i].m_life = 5;
						spr_water[i].setTexture(tex_splash);
						spr_water[i].m_speed = sf::Vector2f(0, 0);
						spr_water[i].m_spin = 0;

						spr_water[i].m_splashed = true;

						spr_flower.move(0, -0.25);
						spr_face.move(0, -0.25);
						spr_stem.move(0, -0.25);

						sf::IntRect tex_rect = spr_stem.getTextureRect();
						tex_rect.height++;
						spr_stem.setTextureRect(tex_rect);
					}
					
					spr_water[i].move(spr_water[i].m_speed);
					spr_water[i].rotate(spr_water[i].m_spin);
				}
			}

			frame_sum -= frame_limit;
			frame_count++;
		}

		std::cout << "\rnumber of water drops: " << spr_water.size();

		if(spawn_delay > 0)
			spawn_delay--;

		window.clear(sf::Color::Cyan);

		window.draw(spr_bg);
		window.draw(spr_sun);
		spr_cloud_a.setPosition(100 - (solar_power / 2), 32);
		window.draw(spr_cloud_a);
		spr_cloud_b.setPosition(-100 + (solar_power / 4), 0);

		window.draw(spr_cloud_b);

		window.draw(spr_pipe_head);
		spr_pipe_knob.setTextureRect(sf::IntRect(0, (water_anim) * 16, 32, 16));
		window.draw(spr_pipe_knob);
		window.draw(spr_pipe_body);

		window.draw(spr_stem);
		window.draw(spr_flower);
		window.draw(spr_face);
		window.draw(spr_pot);

		for(int i = 0; i < spr_water.size(); i++)
		{
			window.draw(spr_water[i]);
			spr_water[i].m_life--;
		}

		window.draw(spr_grass);

		spr_shadow.setFillColor(sf::Color(0, 0, 0, solar_power));
		window.draw(spr_shadow);
 
		window.display();
	}

	moony::logSaveToFile();
}