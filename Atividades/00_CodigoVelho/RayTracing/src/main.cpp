#include "commons.hpp"
#include "global_vars.hpp"
#include "rt_headers.hpp"
#include "scene.hpp"
#include "menu.hpp"

void doRender()
{
	while (!die)
	{
		mut.lock();
		if (rerender)
		{
			rmut.lock();
			//if (sketch) doSketch(rW, rH, pixels);
			//else render(rW, rH, pixels);
			render(rW, rH, pixels);
			rerender = false;
			rmut.unlock();
		}
		mut.unlock();
	}
}

/* MAIN PROGRAM */
int main(int argc, char **argv)
{
	MAXTRACE = 3;

	if (argc > 1) { sscanf(argv[1], "%u", &W); sscanf(argv[2], "%u", &H); }
	if (argc > 3) { sscanf(argv[3], "%u", &MAXTRACE); }
	if (argc > 4) { toFile = true; sscanf(argv[4], "%s", outfile); }

	rW = W; rH = H;

	sf::RenderWindow window(sf::VideoMode(W, H), "MultiThreaded SFML RayTracing", sf::Style::Titlebar);
	window.setVerticalSyncEnabled(true);
	window.clear();
	window.display();

	image.create(W, H, sf::Color::Black);
	if (!texture.create(W, H))
		printf("Erro ao criar textura\n");

	pixels = new sf::Uint8[rW * rH * 4];

	texture.update(pixels);
	sprite.setTexture(texture);

	InitArealightVectors();
	InitDefaultScene();

	std::stringstream command;
	std::string line, op;

	float f_x, f_y, f_z, f_a, f_b;
	int i_a, i_b, i_iw, i_ih;
	bool redraw = true, sketch = false;

	int frametime;
	sf::Clock clock;
	sf::Event event;
	std::thread renderThread(doRender);

	std::thread consoleThread(consoleReader, &window);
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			// "close requested" event: we close the window
			if (event.type == sf::Event::Closed)
			{
				window.close();
				break;
			}
		}
	
		if (toFile)
		{
			image.create(rW, rH, pixels);
			image.saveToFile(outfile);
			toFile = false;
		}
		else
		{
			dmut.lock();
			window.clear();
			//window.display();
			if (resize) window.setSize(sf::Vector2u(W, H));
			// Atualizar textura
			texture.update(pixels);
			sprite.setTexture(texture, true);
			// Apresentar resultado
			window.draw(sprite);
			window.display();
			dmut.unlock();
		}

		if (die)
			break;
	}
	window.close();
	renderThread.join();
	consoleThread.join();

	return 0;
}
