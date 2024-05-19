#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <chrono>
#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>

// #include <condition_variable>
// condition_variable cv;

sem_t reader;
sem_t writer;
int readercount = 0;
sem_t empty_slots, full_slots, mutex1;
bool palletflags[5] = {1, 1, 1, 1, 0};
bool stop = 0;
#include "header.h"

bool exit_window = false;
int score = 0;

const int total = 4;
bool delay_flag = false;
sem_t keys;
sem_t permits;
mutex lock1;
bool running = 1;
using namespace std;
using namespace sf;
std::vector<std::vector<int>> coordinates = {
	{14, 15},
	{13, 9},
	{5, 8},
	{15, 21},
	{13, 15}};
struct arguments
{
	Pacman *pacman;
	Dots *Dot;
	PowerPellets *powerpallet;
	Ghost *ghost;
	arguments(Pacman *pac, Dots *dot, PowerPellets *f, Ghost *ghost) : pacman(pac), Dot(dot), powerpallet(f), ghost(ghost)
	{
	}
};
bool makeGhostsBlue = false;
sf::Clock clock3;
sf::Time lastTime = clock3.getElapsedTime();
void *userInterfaceThread(void *args)
{
	arguments *arg = *(arguments **)args;
	// Score score;

	// cout<<arg->pacman->x<<" "<<arg->pacman->y<<endl;
	while (true)
	{
		arg->Dot->collisionDots(arg->pacman);

		lock1.lock();
		for (int i = 0; i < 4; i++)
		{
			bool flag = false;
			for (int i = 0; i < 4; i++)
			{
				if (arg->ghost[i].lefthouse == true)
				{
					flag = true;
					break;
				}
			}
			if (arg->powerpallet[i].Eat(arg->pacman))
			{ // Change state every 5 seconds
				cout << "power pellet eaten" << endl;
				// cout << elapsedTime.asSeconds() << endl;
				makeGhostsBlue = true;

				for (int i = 0; i < 4; i++)
				{
					arg->ghost[i].changeSprite("blue");
				}
				clock3.restart();
			}
		}
		lock1.unlock();

		lock1.lock();
		if (clock3.getElapsedTime().asSeconds() >= 5 && makeGhostsBlue)
		{
			for (int i = 0; i < 4; i++)
			{
				arg->ghost[i].changeSprite("revert");
			}
			makeGhostsBlue = false;
			arg->pacman->eaten = false;
		}
		lock1.unlock();
	}
	return nullptr;
}
// DINING PHILOSOPHER TECHNIQUE FOR 2 GHOST APPEARANCE AT A TIME

struct ghost_arg
{
	Ghost *ghost;
	Pacman *pacman;
	SpeedBooster *speedbooster;

	ghost_arg(Ghost *ghost, Pacman *pacman, SpeedBooster *s) : ghost(ghost), pacman(pacman), speedbooster(s)
	{
	}
};
Clock clock1;
float delay;

bool first = true;
int lefthouse = 0;
// Dining philosopher function
void DP(ghost_arg *arg)
{
	while (running)
	{
		// clock1.getElapsedTime().asSeconds() >= 10
		if (first)
		{
			//  Try to acquire both key and permit
			// cout << "Clock:" << clock1.getElapsedTime().asSeconds() << endl;
			arg->ghost->lefthouse = true;
			lefthouse++;
			if (lefthouse < 2)
				first = true;
			else
				first = false;
			sem_wait(&keys);
			sem_wait(&permits);
			lock1.lock();
			cout << "left house: " << lefthouse << endl;

			lock1.unlock();
			// Release both key and permit
			sem_post(&keys);
			sem_post(&permits);
			clock1.restart();

			lock1.lock();
			arg->ghost->hasKey = true;
			arg->ghost->hasPermit = true;
			cout << "Ghost " << arg->ghost->ghostnum << " has acquired key and permit, starts eating\n";
			lock1.unlock();
		}
		else
		{
			sleep(5);
			first = true;
			lefthouse = 0;
		}
		// cout << "before blue : " << arg->ghost->isBlue << endl;
		while (arg->pacman->BlueGhostCollision(arg->ghost) && arg->ghost->lefthouse == true)
		{
			if (arg->ghost->isBlue == false)
				arg->ghost->moveGhost(arg->pacman->x, arg->pacman->y);
		}
		lock1.lock();
		cout << "Ghost " << arg->ghost->ghostnum << " has finished eating\n";
		lock1.unlock();

		sleep(5); // Wait for a while before attempting to eat again
	}
}

void *producerThreadFunc(void *args)
{
	ghost_arg *arg = (ghost_arg *)args;

	arg->speedbooster->producer();

	return nullptr;
}

void *consumerThreadFunc(void *args)
{
	ghost_arg *arg = (ghost_arg *)args;

	cout << (arg->ghost->ghostnum) << endl;
	arg->speedbooster->consumer(arg->ghost); // arg->ghost->ghostnum);
	return nullptr;
}
struct speeddboost
{
	Ghost *ghost;
	Pacman *pacman;
	SpeedBooster *speedbooster;
};
void *producerconsumerthread(void *args)
{
	ghost_arg *arg = (ghost_arg *)args;
	while (true)
	{
		pthread_t producerThread, consumerThread1;
		pthread_create(&producerThread, nullptr, producerThreadFunc, args);
		pthread_create(&consumerThread1, nullptr, consumerThreadFunc, args);
		pthread_join(producerThread, nullptr);
		pthread_join(consumerThread1, nullptr);
	}
}
void *ghostControllerThread(void *args)
{
	ghost_arg *arg = (ghost_arg *)args;

	pthread_t t;
	pthread_create(&t, NULL, producerconsumerthread, args);
	while (true)
	{

		DP(arg);
	}
	pthread_join(t, nullptr);

	return nullptr;
}

void *gameEngineThread(void *args)
{
	pthread_t t[4];
	Score score;
	Lives lives(5);
	Pacman pacman(lives);
	SpeedBooster speedbooster;
	RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, (HEIGHT + bottomSpace + topSpace) * CELL_SIZE), "Pac-Man Maze");
	Ghost ghosts[4] = {
		Ghost("pacman-art/ghosts/blinky.png", 0.0f, 1, 0.3, 1), // No delay
		Ghost("pacman-art/ghosts/clyde.png", 0.5f, 2, 0.4, 1),	// Delay of 0.5 seconds
		Ghost("pacman-art/ghosts/inky.png", 1.0f, 3, 0.5, 0),	// Delay of 1 second
		Ghost("pacman-art/ghosts/pinky.png", 1.5f, 4, 0.6, 0)	// Delay of 1.5 seconds
	};
	Food food(score);
	const int numPellets = 4;

	PowerPellets pellets[numPellets] = {
		PowerPellets(14, 15),
		PowerPellets(13, 9),
		PowerPellets(5, 8),
		PowerPellets(15, 21)};

	Dots Dot(pacman, ghosts, score);
	// Main loop
	Event event;
	bool pressed = false;
	// cout<<pacman.x<<" "<<pacman.y<<endl;
	// PowerPellets Pellets[4] = {&pellets[0], &pellets[1], &pellets[2], &pellets[3]};
	arguments *args1 = new arguments(&pacman, &Dot, pellets, ghosts);

	pthread_create(&t[0], nullptr, userInterfaceThread, &args1);
	int n[total];
	for (int i = 0; i < 4; ++i)
	{
		ghost_arg *arg = new ghost_arg(&ghosts[i], &pacman, &speedbooster);
		pthread_create(&t[i], nullptr, ghostControllerThread, arg);
	}

	while (window.isOpen())
	{
		// sem_wait(&full_slots);
		// sem_wait(&mutex1);
		lock1.lock();

		lock1.unlock();

		// sem_post(&mutex1);
		// sem_post(&empty_slots);

		// Handle events

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				// pthread_mutex_lock(&lock1);
				running = false;
				window.close();
				exit(0);
				// pthread_mutex_unlock(&lock1);
			}
			pacman.handleEvent(event);
		}

		pacman.move_in_same_Direction();

		// for (int i = 0; i < 4; i++)
		// {
		// 	if (ghosts[i].hasKey && ghosts[i].hasPermit)
		// 		ghosts[i].moveGhost(pacman.x,pacman.y);
		// }
		for (int i = 0; i < 4; i++)
		{
			cout << "before ghost " << ghosts[i].isBlue << endl;
			pacman.GhostCollision(&ghosts[i]);
		}

		food.Eat(&pacman);

		window.clear();
		lock1.lock();
		if (lives.val <= 0)
		{
			cout << "Game over!" << endl;
			exit(0);
		}
		lock1.unlock();
		// Draw the maze
		drawMaze(window);

		// Draw Ghost
		for (int i = 0; i < 4; i++)
		{
			ghosts[i].draw(window);
		}

		// Check for power pellets

		// Draw Dots
		Dot.draw(window);

		// Draw Scores
		score.draw(window);

		// Draw food
		food.draw(window);
		lives.draw(window);
		// Draw Pacman
		pacman.draw(window);
		speedbooster.draw(window);
		lock1.lock();
		for (int i = 0; i < 4; i++)
		{

			if (pellets[i].respawn == true)
			{
				// pallet[i]=0;
			//	cout << "respawn true";
				for (int j = 0; j < 5; j++)
				{
					if (palletflags[j] == 0)
					{
						//cout << " icaaa" << i << " " << j << " " << coordinates[j][0] << " " << coordinates[j][1] << endl;

						pellets[i].x = coordinates[j][0];
						pellets[i].y = coordinates[j][1];
						palletflags[j] = 1;
						palletflags[i] = 0;
						pellets[i].updatePosition();
						pellets[i].respawn = false;
						break;
					}
				}
			}

			pellets[i].draw(window);
		}
		lock1.unlock();

		// Display the window
		window.display();
	}

	for (int i = 0; i < 4; i++)
	{
		pthread_join(t[i], nullptr);
	}
	return nullptr;
}

void *menuThreadFunc(void *args)
{
	sf::RenderWindow window(sf::VideoMode(WIDTH * CELL_SIZE, (HEIGHT + bottomSpace + topSpace) * CELL_SIZE), "Menu");
	window.setFramerateLimit(60);
	sf::Font font;
	if (!font.loadFromFile("Winter Memories.ttf"))
	{
		std::cerr << "Failed to load font!" << std::endl;
		return nullptr;
	}

	sf::Text message("Press Enter to Start the Game", font, 40);
	message.setPosition(6 * CELL_SIZE, 11 * CELL_SIZE);
	message.setFillColor(sf::Color::Red);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
			{
				std::cout << "Starting the game..." << std::endl;
				window.close();
			}
		}
		window.clear(sf::Color::Black);
		window.draw(message);
		window.display();
	}
	return nullptr;
}

int main()
{
	pthread_t menu;
	pthread_create(&menu, nullptr, menuThreadFunc, nullptr);
	pthread_join(menu, nullptr);
	pthread_t t;
	pthread_create(&t, nullptr, gameEngineThread, nullptr);
	sem_init(&keys, 0, 2);	  // Initialize the keys semaphore with 2 keys
	sem_init(&permits, 0, 2); // Initialize the permits semaphore with 2 permits
	sem_init(&reader, 0, 1);  // Initialize the keys semaphore with 2 keys
	sem_init(&writer, 0, 1);  // Initialize the permits
	sem_init(&empty_slots, 0, 1);
	sem_init(&full_slots, 0, 0);
	sem_init(&mutex1, 0, 1);

//	pthread_join(t, nullptr);

	sem_destroy(&keys);
	sem_destroy(&permits);
	sem_destroy(&empty_slots);
	sem_destroy(&full_slots);
	sem_destroy(&mutex1);
	pthread_exit(NULL);
	return 0;
}