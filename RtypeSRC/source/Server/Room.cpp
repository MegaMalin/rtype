//
// Room.cpp for  in /home/gandoulf/epitech/rtype/RtypeSRC/source/Server
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Wed Nov 30 11:04:49 2016 Gandoulf
// Last update Fri Dec 16 22:52:29 2016 Gandoulf
//

#include "Server/Room.hpp"
#include <cstring>

namespace rtype
{
  Room::Room(std::string const & name, unsigned short const & port,
	     unsigned int const & maxPlayers)
    : _name(name), _port(port), _maxPlayers(maxPlayers)
  {
    _gameServer = std::unique_ptr<GameServer>(new GameServer(port, "udp", _maxPlayers));
  }

  Room::~Room()
  {

  }

  bool Room::join(std::string &message, std::string const & nickName)
  {
    if (_players.size() >= _maxPlayers)
      {
	message = joinError();
	return (false);
      }
    for (int i = 0; i < _maxPlayers; ++i)
      if (_players.find(i) == _players.end()) {
	_players.insert(std::make_pair(i, Client(nickName)));
	break;
      }
    message = joinOk();
    return (true);
  }

  void Room::disconnect(std::string &message, std::string const & nickName)
  {
    message = "";
    for (auto player = _players.begin(); player != _players.end(); player = _players.begin())
      if (!player->second.getName().compare(nickName)){
	_players.erase(player);
	message = "player disconnect";
      }
  }

  int Room::getPlayerNbr() const
  {
    return (_players.size());
  }

  void Room::setReady(std::string &message, std::string const & nickName, bool const & isReady)
  {
    message = "";
    for (auto player = _players.begin(); player != _players.end(); ++player)
      if (!player->second.getName().compare(nickName)) {
	player->second.setReady(isReady);
	message = "player Ready";
      }
    bool launch = true;
    for (auto player = _players.begin(); player != _players.end(); ++player)
      if (!player->second.getReady())
	launch = false;
    if (launch)
      _gameServer->run();
  }

  //private

  std::string Room::joinError() const
  {
    return std::string(_name + ",KO");
  }

  std::string Room::joinOk() const
  {
    return std::string(_name + ",OK");
  }

  std::vector<std::string>    Room::getRoomPlayers()
  {
    std::vector<std::string> players;
    return (players);
  }
}