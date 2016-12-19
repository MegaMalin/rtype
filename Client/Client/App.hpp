#pragma once
#ifndef APP_HPP_
# define APP_HPP_

# include <SFML/Graphics.hpp>
# include "IState.hpp"

# define FPS (1000 / 60)

class App
{
protected:
	IState *currentState;
	sf::Clock gameClock;
	sf::Clock drawClock;
	bool active;

	void loop();
public:
	sf::RenderWindow win;

	App();

	/**
	* Launch the graphic client
	*/
	void run(void);
	
	/**
	* Change current state of the app
	*/
	void setState(IState *);

	/**
	* Close the window and terminates the program
	*/
	void quit();

	bool getActive(void) const;
};

#endif /* !APP_HPP_ */