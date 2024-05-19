#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <queue>
using namespace sf;
using namespace std;

const int WIDTH = 23;
const int HEIGHT = 23;
const int CELL_SIZE = 35; // Size of each cell in pixels
const int topSpace = 2;
const int bottomSpace = 2;
const int BUFFER_SIZE=2;

mutex lock2;

int mazeLayout[HEIGHT][WIDTH] = {
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
	{0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
	{0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
	{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
	{0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
	{1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
	{0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0}, //
	{0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0},
	{0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 0},
	{0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0},
	{0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0},
	{0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0},
	{0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

void drawMaze(RenderWindow &window)
{
	// Create shapes for cells
	sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));

	// Draw maze
	for (int i = 0; i < HEIGHT; ++i)
	{
		for (int j = 0; j < WIDTH; ++j)
		{
			cell.setPosition(j * CELL_SIZE, (i + topSpace) * CELL_SIZE);
			if (mazeLayout[i][j] != 0)
			{
				cell.setFillColor(sf::Color::Black); // Wall
				window.draw(cell);
				if (j > 0 && mazeLayout[i][j - 1] == 0)
				{
					// Draw left outline
					sf::Vertex leftOutline[] = {
						sf::Vertex(sf::Vector2f(j * CELL_SIZE, (i + topSpace) * CELL_SIZE), sf::Color::Blue),
						sf::Vertex(sf::Vector2f(j * CELL_SIZE, ((i + topSpace) + 1) * CELL_SIZE), sf::Color::Blue)};
					window.draw(leftOutline, 2, sf::Lines);
				}
				if (j < (WIDTH - 1) && mazeLayout[i][j + 1] == 0)
				{
					// Draw right outline
					sf::Vertex rightOutline[] = {
						sf::Vertex(sf::Vector2f((j + 1) * CELL_SIZE, (i + topSpace) * CELL_SIZE), sf::Color::Blue),
						sf::Vertex(sf::Vector2f((j + 1) * CELL_SIZE, ((i + topSpace) + 1) * CELL_SIZE), sf::Color::Blue)};
					window.draw(rightOutline, 2, sf::Lines);
				}
				if (i > 0 && mazeLayout[i - 1][j] == 0)
				{
					// Draw top outline
					sf::Vertex topOutline[] = {
						sf::Vertex(sf::Vector2f(j * CELL_SIZE, (i + topSpace) * CELL_SIZE), sf::Color::Blue),
						sf::Vertex(sf::Vector2f((j + 1) * CELL_SIZE, (i + topSpace) * CELL_SIZE), sf::Color::Blue)};
					window.draw(topOutline, 2, sf::Lines);
				}
				if (i < (HEIGHT - 1) && mazeLayout[i + 1][j] == 0)
				{
					// Draw bottom outline
					sf::Vertex bottomOutline[] = {
						sf::Vertex(sf::Vector2f(j * CELL_SIZE, ((i + topSpace) + 1) * CELL_SIZE), sf::Color::Blue),
						sf::Vertex(sf::Vector2f((j + 1) * CELL_SIZE, ((i + topSpace) + 1) * CELL_SIZE), sf::Color::Blue)};
					window.draw(bottomOutline, 2, sf::Lines);
				}
			}
			else
			{
				cell.setFillColor(sf::Color{16, 16, 16}); // Empty space
				window.draw(cell);
			}
		}
	}
	// cout<<mazeLayout[8][21];
}

class Score
{
public:
	long long int val;
	string type;
	sf::Text text;
	sf::Font font;
	Score()
	{
		val = 0;
		if (!font.loadFromFile("arial.ttf"))
		{ // Load a font file for displaying text
			std::cerr << "Failed to load font!" << std::endl;
			return;
		}
		text.setFont(font);					 // Set the font for the text
		text.setCharacterSize(24);			 // Set the character size
		text.setFillColor(sf::Color::White); // Set the text color
		text.setPosition(10, 10);			 // Set the position of the text
		updateText();						 // Update the text initially
	}
	void update(int v)
	{
		val += v;
		updateText(); // Update the text whenever the score changes
	}

	void draw(sf::RenderWindow &window)
	{
		window.draw(text);
	}

private:
	void updateText()
	{
		text.setString("Score: " + std::to_string(val)); // Set the text content
	}
};

class Ghost
{
public:
	int x;
	int y;
	int current_path;
	Texture spriteTexture;
	Texture blueTexture;
	Sprite sprite;
	Clock clock;
	float moveInterval;
	float delay; 
	bool left = true;
	bool hasKey;
	bool hasPermit;
	int ghostnum;
	bool dead;
	bool lefthouse;
	string OrigSprite;
	bool isBlue;
	float speed = 0.3;
	bool stop_moving = 0;
	Color originalColor;
	bool fast;
	Ghost(string path, float delay, int ghostnum, float s,bool f) : delay(delay), ghostnum(ghostnum), speed(s),fast(f)
	{ 

		OrigSprite = path;
		lefthouse=false;
		hasKey = hasPermit = isBlue = false;
		dead = false;
		if (!spriteTexture.loadFromFile(path))
		{
			std::cerr << "Failed to load sprite texture!" << std::endl;
			return;
		}
		if (!blueTexture.loadFromFile("pacman-art/ghosts/blue_ghost.png"))
		{
			std::cerr << "Failed to load sprite texture!" << std::endl;
			return;
		}
		sprite.setTexture(spriteTexture);
		if (ghostnum == 0)
		{
			originalColor = sf::Color(255, 0, 0, 128); // Red=0, Green=0, Blue=255, Alpha=128
			sprite.setColor(originalColor);
			x = 9;
		}
		else if (ghostnum == 1)
		{
			// yellow
			originalColor = sf::Color(250, 250, 210, 128); // Red=0, Green=0, Blue=255, Alpha=128
			sprite.setColor(originalColor);
			x = 10;
		}
		else if (ghostnum == 2)
		{
			x = 11;
			originalColor = Color(0, 255, 0, 128); // Red=0, Green=0, Blue=255, Alpha=128
			sprite.setColor(originalColor);
		}
		else
		{
			x = 12;
			originalColor = Color(255, 192, 203, 128); // Red=0, Green=0, Blue=255, Alpha=128
			sprite.setColor(originalColor);
		}
		y = 11;
		sprite.setScale(2.0f, 2.0f);
		updatePosition();
		current_path = 0;
		moveInterval = 0.2f;
	}
	void defaultPos()
	{
		if (ghostnum == 0)
		{
			x = 9;
		}
		else if (ghostnum == 1)
		{
			x = 10;
		}
		else if (ghostnum = 2)
		{
			x = 11;
		}
		else
			x = 12;
		y = 11;
		updatePosition();
	}
	void changeSprite(string val)
	{
		lock2.lock();
		if (val == "blue")
		{
			if (lefthouse)
			{
				sf::Color customColor(0, 0, 255, 128); // Red=0, Green=0, Blue=255, Alpha=128
				sprite.setColor(customColor);
			}
			isBlue = true;
		}
		else
		{
			if (lefthouse)
			{
				cout<<"revert\n";
				sprite.setColor(originalColor);
			}
			isBlue = false;
		}
		lock2.unlock();
	}
	void updatePosition()
	{
		float xPos = CELL_SIZE * x + CELL_SIZE / 2 - sprite.getGlobalBounds().width / 2;
		float yPos = CELL_SIZE * (y + topSpace) + CELL_SIZE / 2 - sprite.getGlobalBounds().height / 2;
		sprite.setPosition(xPos, yPos);
	}

	void draw(RenderWindow &window)
	{
		window.draw(sprite);
	}


	vector<vector<int>> bfs(int PacmanX, int PacmanY)
	{
		vector<vector<int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
		vector<vector<bool>> visited(HEIGHT, vector<bool>(WIDTH, false));
		vector<vector<vector<int>>> parent(HEIGHT, vector<vector<int>>(WIDTH, vector<int>(2, -1)));
		queue<vector<int>> q;
		q.push({x, y});
		visited[y][x] = true;

		while (!q.empty())
		{
			vector<int> current = q.front();
			int curX = current[0];
			int curY = current[1];
			q.pop();
			if (curX == PacmanX && curY == PacmanY)
			{
				vector<vector<int>> path;
				while (curX != x || curY != y)
				{
					path.push_back({curX, curY});
					current = parent[curY][curX];
					curX = current[0];
					curY = current[1];
				}
				path.push_back({x, y});
				reverse(path.begin(), path.end());

				return path;
			}
			for (int i = 0; i < directions.size(); ++i)
			{
				int newX = curX + directions[i][0];
				int newY = curY + directions[i][1];

				if (newX >= 0 && newX < WIDTH && newY >= 0 && newY < HEIGHT && !visited[newY][newX])
				{
					sem_wait(&reader);
					readercount++;
					if (readercount == 1)
					sem_wait(&writer);
					sem_post(&reader);
					if (mazeLayout[newY][newX] != 0)
					{
						q.push({newX, newY});
						visited[newY][newX] = true;
						parent[newY][newX] = {curX, curY};
					}
					sem_wait(&reader);
					readercount--;
					if (readercount == 0)
					{
						sem_post(&writer);
					}
					sem_post(&reader);
				}
			}
		}
		return {};
	}

	void moveGhost(int PacmanX, int PacmanY)
	{
		vector<vector<int>> path;
		if (x != PacmanX || y != PacmanY)
			path = bfs(PacmanX, PacmanY);
		if (path.empty())
		{
			return;
		}

		int nextX = path[1][0];
		int nextY = path[1][1];
		if (clock.getElapsedTime().asSeconds() > speed)
		{
			x = nextX;
			y = nextY;
			updatePosition();
			clock.restart();
		}
	}

};

class SpeedBooster {
public:
    sem_t emptyBuffers;
    sem_t fullBuffers; 
    sem_t mutex1; 
    int buffer[BUFFER_SIZE];
	//sprites/food.png 
	sf::Sprite boosterSprites[BUFFER_SIZE]; 
    sf::Texture boosterTextures[BUFFER_SIZE]; 
	int boosterX[BUFFER_SIZE]; 
    int boosterY[BUFFER_SIZE];
	int collide;
	// int upboosterX[BUFFER_SIZE]; 
    // int upboosterY[BUFFER_SIZE];
	bool respawn=0;
public:
    SpeedBooster() {
        sem_init(&emptyBuffers, 0, BUFFER_SIZE); 
        sem_init(&fullBuffers, 0, 0);
        sem_init(&mutex1, 0, 1);
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            buffer[i] = 0;
    	}
		 for (int i = 0; i < BUFFER_SIZE; ++i) {
            if (!boosterTextures[i].loadFromFile("sprites/apple.png")) {
                std::cerr << "Failed to load booster texture " << i << "!" << std::endl;
                exit(EXIT_FAILURE);
            }
            boosterSprites[i].setTexture(boosterTextures[i]);
			boosterSprites[i].setScale(2.0f, 2.0f);
        }
		boosterSprites[0].setPosition(11*CELL_SIZE , 11*CELL_SIZE);
		boosterSprites[1].setPosition(2*CELL_SIZE , 3*CELL_SIZE); 
		boosterX[0] = 11; 
        boosterY[0] = 11;
        boosterX[1] = 2 ;
		boosterY[1] = 3 ;
}
    ~SpeedBooster() {
        sem_destroy(&emptyBuffers);
        sem_destroy(&fullBuffers);
        sem_destroy(&mutex1);
    }

    void producer() {
        while (true) {
           sem_wait(&emptyBuffers); // Wait for an empty buffer
            sem_wait(&mutex1);
            std::cout << "Produced a speed booster!" << std::endl;
            sem_post(&mutex1);
            sem_post(&fullBuffers);
        }
    }

    void consumer(Ghost * ghost) {
       while (true) {
	
	if(ghost->fast&&collideWithSpeedBooster(ghost)){

		//cout<<" ghost is "<<ghost->ghostnum<<endl;
           sem_wait(&fullBuffers); 
             sem_wait(&mutex1);
             static int index = 0;
            buffer[index] = 0; 
            index = (index + 1) % BUFFER_SIZE;
            std::cout << "Ghost" << ghost->ghostnum<< " acquired the speed booster!" << std::endl;
			float spd=ghost->speed;
            ghost->speed=0.1;
			sleep(1);
            std::cout << "Ghost" << ghost->ghostnum << " left the speed booster." << std::endl;
			ghost->speed=spd;
			updatePosition();
            sem_post(&mutex1);
        	sem_post(&emptyBuffers);
        }
		}
    }
	bool collideWithSpeedBooster(Ghost * ghost) {
	
    for (int i = 0; i < BUFFER_SIZE; ++i) {
        if (ghost->x == boosterX[i] && ghost->y == boosterY[i]) {
			boosterSprites[i].setPosition(-500,-500);
			collide=i;
            return true;
        }
    }
    return false; 
}
	void draw(sf::RenderWindow& window) {
        for (int i = 0; i < BUFFER_SIZE; ++i) {
            window.draw(boosterSprites[i]);
        }
    }
	void updatePosition()
	{	int i=collide;
		float xPos = CELL_SIZE * boosterX[i] + CELL_SIZE / 2 - boosterSprites[i].getGlobalBounds().width / 2;
		float yPos = CELL_SIZE * (boosterY[i]+ topSpace) + CELL_SIZE / 2 - boosterSprites[i].getGlobalBounds().height / 2;
		boosterSprites[i].setPosition(xPos, yPos);
		
	}
};

class Lives
{
public:
	int val;
	Sprite sprite;
	Texture texture;
	sf::Text text;
	sf::Font font;
	Lives(int val)
	{
		this->val = val;
		if (!font.loadFromFile("arial.ttf"))
		{ // Load a font file for displaying text
			std::cerr << "Failed to load font!" << std::endl;
			return;
		}
		text.setFont(font);					 // Set the font for the text
		text.setCharacterSize(24);			 // Set the character size
		text.setFillColor(sf::Color::White); // Set the text color
		text.setPosition(700, 10);	
	}
	void updateLives(){
		
		updateText();
	}
	void draw(sf::RenderWindow &window)
	{
		window.draw(text);
	}

private:
	void updateText()
	{
		text.setString("Lives: " + std::to_string(val));
	}
};


class Pacman
{
public:
	int x;
	int y;
	sf::Sprite sprite;
	sf::Texture spriteTextureUp;
	sf::Texture spriteTextureDown;
	sf::Texture spriteTextureLeft;
	sf::Texture spriteTextureRight;
	bool eaten;
	string active;

	sf::Keyboard::Key direction; // Direction of movement
	bool isTransitioning;
	sf::Clock transitionClock;
	Clock clock;
	float moveInterval;
	int TRANSITION_DURATION;
	Lives& lives;
	Pacman(Lives& lives):lives(lives)
	{
		eaten=false;

		if (!spriteTextureUp.loadFromFile("pacman-art/pacman-up/up.png") ||
			!spriteTextureDown.loadFromFile("pacman-art/pacman-down/down.png") ||
			!spriteTextureLeft.loadFromFile("pacman-art/pacman-left/left.png") ||
			!spriteTextureRight.loadFromFile("pacman-art/pacman-right/right.png"))
		{
			std::cerr << "Failed to load sprite texture!" << std::endl;
			active = "";
			return;
		}
		

			sprite.setTexture(spriteTextureRight);
		x = 1;
		y = 1;
		sprite.setScale(2.0f, 2.0f);

		TRANSITION_DURATION = 3;
		moveInterval = 0.1f;
		updatePosition();
	}

	bool handleEvent(sf::Event &event)
	{
		// cout<<"in handle event"<<endl;
		bool pressed = false;
		if (event.type == sf::Event::KeyPressed)
		{
			switch (event.key.code)
			{
			case sf::Keyboard::Up:
				sprite.setTexture(spriteTextureUp);
				direction = sf::Keyboard::Up;
				break;
			case sf::Keyboard::Down:
				sprite.setTexture(spriteTextureDown);
				direction = sf::Keyboard::Down;
				break;
			case sf::Keyboard::Left:
				sprite.setTexture(spriteTextureLeft);
				direction = sf::Keyboard::Left;
				break;
			case sf::Keyboard::Right:
				sprite.setTexture(spriteTextureRight);
				direction = sf::Keyboard::Right;
				// cout<<"right"<<endl;
				break;
			default:
				break;
			}
		}

		if (pressed)
			return true;
		else
			return false;
	}
	//
	void move_in_same_Direction()
	{
		if (clock.getElapsedTime().asSeconds() >= moveInterval)
		{

			if (direction == sf::Keyboard::Up && mazeLayout[y - 1][x] != 0)
			{
				y -= 1;
			}
			else if (direction == sf::Keyboard::Down && mazeLayout[y + 1][x] != 0)
			{
				y += 1;
			}
			else if (direction == sf::Keyboard::Left)
			{
				if (x == 0 && y == 11)
				{
					x = 22; // teleportation
				}
				else if (mazeLayout[y][x - 1] != 0)
				{
					x -= 1;
				}
			}
			else if (direction == sf::Keyboard::Right)
			{
				if (y == 11 && x == 22)
				{
					x = 0; // teleportation
				}
				else if (mazeLayout[y][x + 1] != 0)
				{
					x += 1;
				}
			}
			updatePosition();
			clock.restart();
		}
	}

	void updatePosition()
	{

		float xPos = CELL_SIZE * x + CELL_SIZE / 2 - sprite.getGlobalBounds().width / 2;
		float yPos = CELL_SIZE * (y + topSpace) + CELL_SIZE / 2 - sprite.getGlobalBounds().height / 2;
		sprite.setPosition(xPos, yPos);
	}
	bool GhostCollision(Ghost *ghosts)
	{
		//cout<<"in ghost "<<ghosts->isBlue<<endl;
		if (x == ghosts->x && y == ghosts->y && ghosts->isBlue == false)
		{

			x = 1;
			y = 3;
			updatePosition();
			lock2.lock();
			lives.val--;
		
			cout<<"Lives = "<<lives.val<<"\n";
			lives.updateLives();
			ghosts->lefthouse=false;
			//cout<<"ghost collision\n";
			lock2.unlock();
			return true;
		}

		return false;
	}
	bool BlueGhostCollision(Ghost *ghost)
	{
		//cout<<"in blue ghost "<<ghost->isBlue<<endl;
		if (x == ghost->x && y == ghost->y && ghost->isBlue)
		{


			ghost->isBlue = false;
			ghost->changeSprite("revert");
			ghost->hasKey = false;
			ghost->hasPermit = false;
			ghost->defaultPos();
			ghost->updatePosition();
			ghost->lefthouse=false;
			//cout<<"blue ghost collision\n";

			return false;
		}
		return true;
	}

	void draw(sf::RenderWindow &window)
	{
		// Get the bounding box of the sprite
		sf::FloatRect spriteBounds = sprite.getGlobalBounds();

		// Create a rectangle shape to cover the area of the sprite
		sf::RectangleShape eraser(sf::Vector2f(spriteBounds.width, spriteBounds.height));
		eraser.setPosition(sprite.getPosition());
		eraser.setFillColor(sf::Color::Black); // Change to your background color

		// Erase the area where the sprite was previously drawn
		window.draw(eraser);

		// Draw the sprite
		window.draw(sprite);
	}
};

class Food
{
public:
	vector<Sprite> sprites;
	Texture texture;
	int x;
	int y;
	int total;
	int **coord;
	Score &score;
	Food(Score &score) : score(score)
	{
		total = 4;
		coord = new int *[total];
		for (int i = 0; i < total; i++)
		{
			coord[i] = new int[2];
		}
		if (!texture.loadFromFile("sprites/cherry.png"))
		{
			std::cerr << "Failed to load sprite texture!" << std::endl;
			return;
		}
		Sprite sprite;
		sprite.setTexture(texture);
		sprite.setScale(0.0625, 0.0625);
		// sprite.setScale(0.03125, 0.03125);
		int i = 0;
		while (i < total)
		{
			bool flag = false;
			x = rand() % 21 + 1;
			y = rand() % 21 + 1;
			if (y == 11)
			{
				if (x >= 9 && x <= 13)
				{
					flag = true;
				}
			}
			if (y == 10 && x == 11)
				flag = true;
			//cout << "x = " << x << " , y= " << y << endl;
			cout << mazeLayout[x][y] << endl;
			if (mazeLayout[y][x] == 1 && flag == false)
			{

				float xPos = CELL_SIZE * x + CELL_SIZE / 2 - sprite.getGlobalBounds().width / 2;
				float yPos = CELL_SIZE * (y + topSpace) + CELL_SIZE / 2 - sprite.getGlobalBounds().height / 2;
				sprite.setPosition(xPos, yPos);
				coord[i][0] = x;
				coord[i][1] = y;
				sprites.push_back(sprite);
				i++;
			}
		}
		cout << "\narray" << endl;
		for (int i = 0; i < total; i++)
		{
			cout << coord[i][0] << " " << coord[i][1] << endl;
		}
	}

	void draw(sf::RenderWindow &window)
	{
		for (int i = 0; i < sprites.size(); i++)
		{
			window.draw(sprites[i]);
		}
	}
	void Eat(Pacman *pacman)
	{
		for (int i = 0; i < 4; i++)
		{

			if (pacman->x == coord[i][0] && pacman->y == coord[i][1])
			{
				score.update(10);
				sprites[i].setPosition(-500, -500);
				coord[i][0] = -500;
				coord[i][1] = -500;
			}
		}
	}
};

class PowerPellets
{
public:
	sf::CircleShape circles;
	int x;
	int y;
	int total;
	//int *coord;
	bool respawn = false;

	PowerPellets(int x1, int y1)
	{
		total = 4;
		x = x1;
		y = y1;

		//coord = new int[2];
		float radius = 15.0f;
		circles.setRadius(radius);
		circles.setFillColor(sf::Color(255, 165, 0));
		float xPos = CELL_SIZE * x + CELL_SIZE / 2 - circles.getGlobalBounds().width / 2;
		float yPos = CELL_SIZE * (y + topSpace) + CELL_SIZE / 2 - circles.getGlobalBounds().height / 2;
		circles.setPosition(xPos, yPos);
		//coord[0] = x;
		//coord[1] = y;
	}

	bool Eat(Pacman *pacman)
	{
		//for (int i = 0; i < 4; i++)
		//{

			
			if (pacman->x == x && pacman->y == y && pacman->eaten==false)
			{
				//cout << "eat true" << endl;
				pacman->eaten=true;

				circles.setPosition(-500, -500);
				//coord[0] = -500;
				//coord[1] = -500;
				respawn = true;
				return true;
			}
			
		//}
		return false;
	}

	void updatePosition()
	{
		float xPos = CELL_SIZE * x + CELL_SIZE / 2 - circles.getGlobalBounds().width / 2;
		float yPos = CELL_SIZE * (y + topSpace) + CELL_SIZE / 2 - circles.getGlobalBounds().height / 2;
		circles.setPosition(xPos, yPos);

	}

	void draw(sf::RenderWindow &window)
	{ // cout<<x<<" "<<y<<endl;
		window.draw(circles);
	}
};


class Dots
{
public:
	int **dots;
	vector<Sprite> sprites;
	Texture texture;
	Score &score;
	Pacman &pacman;
	int x;
	int y;
	Dots(Pacman &pacman, Ghost ghosts[], Score &score) : score(score), pacman(pacman)
	{
		if (!texture.loadFromFile("sprites/food.png"))
		{
			std::cerr << "Failed to load sprite texture!" << std::endl;
			return;
		}

		dots = new int *[WIDTH];
		int count = 0;
		for (int i = 0; i < WIDTH; i++)
		{
			dots[i] = new int[HEIGHT];
			for (int j = 0; j < HEIGHT; j++)
			{
				if ((i == pacman.y && j == pacman.x) || mazeLayout[i][j] == 0)
				{
					dots[i][j] = 0;
					continue;
				}
				bool ghost_check = false;
				for (int k = 0; k < 4; k++)
				{
					if (ghosts[k].y == i && ghosts[k].x == j)
						ghost_check = true;
				}
				if (ghost_check)
					continue;

				dots[i][j] = 1;
				Sprite sprite;
				sprite.setTexture(texture);
				sprite.setScale(0.03125, 0.03125);

				float xPos = CELL_SIZE * j + CELL_SIZE / 2 - sprite.getGlobalBounds().width / 2;
				float yPos = CELL_SIZE * (i + topSpace) + CELL_SIZE / 2 - sprite.getGlobalBounds().height / 2;
				sprite.setPosition(xPos, yPos);

				sprites.push_back(sprite);
				x = 0;
				y = 11;
				count++;
			}
		}
	}
	bool collisionDots(Pacman *pacman)
	{
		for (int i = 0; i < WIDTH; i++)
		{
			for (int j = 0; j < HEIGHT; j++)
			{
				if (pacman->x == j && pacman->y == i)
				{
					if (dots[i][j] == 1)
					{
						score.update(1);
						dots[i][j] = 0;

						sem_wait(&writer);
						mazeLayout[i][j] = 2;

						sem_post(&writer);

						// Find and erase the corresponding sprite
						for (auto it = sprites.begin(); it != sprites.end(); ++it)
						{
							sf::Vector2f spritePos = it->getPosition();
							int spriteX = spritePos.x / CELL_SIZE;	   // Assuming CELL_SIZE is the size of each cell
							int spriteY = ((spritePos.y) / CELL_SIZE); // row number

							if (spriteX == j && spriteY == i + topSpace)
							{
								it = sprites.erase(it);
								pacman->isTransitioning = true;
								pacman->transitionClock.restart();
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}

	void draw(sf::RenderWindow &window)
	{
		for (int i = 0; i < sprites.size(); i++)
			window.draw(sprites[i]);
	}
};