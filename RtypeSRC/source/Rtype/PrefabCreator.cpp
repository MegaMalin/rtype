//
// PrefabCreator.cpp for  in /home/gandoulf/epitech/cpp_indie_studio/indie-studio/source/IndieStudio
//
// Made by Gandoulf
// Login   <gandoulf@epitech.net>
//
// Started on  Wed May 25 14:52:49 2016 Gandoulf
// Last update Wed Dec 21 15:08:01 2016 Gandoulf
//

#include "Rtype/PrefabCreator.hpp"
#include <time.h>

namespace rtype
{
  PrefabCreator::PrefabCreator()
  {
    srand (time(NULL));
  }

  PrefabCreator::~PrefabCreator()
  {

  }

  GameObject *PrefabCreator::instantiate(std::string const &prefabFile,
					 Vector2F const & position,
					 Vector2F const & scale)
  {
    std::map<std::string, Prefab>::iterator it;
    std::string	_prefabFile;

#if defined(__GNUC__)
    _prefabFile = std::string("RtypeSRC/prefab/" + prefabFile);
#elif defined(_MSC_VER)
    _prefabFile = std::string("../prefab/" + prefabFile);
#endif
    try {
      loadFile(_prefabFile);
    } catch (PrefabLoadFileError e) {
      std::cout << e.what() << std::endl;
      return (NULL);
    }
    if ((it = _prefab.find(_prefabFile)) == _prefab.end())
      {
	std::cout << "can't instantiate gameobject" << std::endl;
	return (NULL);
      }
    if (it->second._usable)
      {
	GameObject *gameObject;

	gameObject = new GameObject();
	instantiateRenderObject(it->second._properties, gameObject, position, scale);
	if (gameObject == NULL)
	  {
	    return (gameObject);
	  }
	gameObject->setTag(it->second._properties.tag);
	if (!it->second._properties.skillFunction.empty())
	  {
	    std::cout << "push Dynamic gameObject" << std::endl;
	    for (int i = 0; i < it->second._properties.skillFunction.size(); ++i)
	      gameObject->setScript(it->second._properties.skillFunction[i](gameObject));
	    gameObject->_dynamic = true;
	    //indie::GameManager::getGameObjects()
	    //.addObject(gameObject, GameObjectManager::DYNAMIC);
	  }
	else
	  {
	    std::cout << "push Static gameObject" << std::endl;
	    gameObject->_dynamic = false;
	    //indie::GameManager::getGameObjects()
	    //.addObject(gameObject, GameObjectManager::STATIC);
	  }
	return (gameObject);
      }
  }

  //private methode

  void PrefabCreator::loadFile(std::string const &prefabFile)
  {
    std::map<std::string, Prefab>::iterator it;

    if ((it = _prefab.find(prefabFile)) != _prefab.end())
      return;

    if (!_parser.loadFile(prefabFile))
      {
	std::cout << "File not Found." << std::endl;
	return ;
      }
    else{
      _prefab[prefabFile] = Prefab();
      it = _prefab.find(prefabFile);
      getName(it->second._properties);
      getTag(it->second._properties);
      getScript(it->second._properties);
      //getCollider(it->second._properties);
      getPosition(it->second._properties);
      getScale(it->second._properties);
    }
    if (!it->second._properties.script.empty())
      {
	for (int i = 0; i < it->second._properties.script.size(); ++i)
	  {
	    std::map<std::string, Memory::LibraryLoader>::iterator itLib;
	    if ((itLib = _lib.find(it->second._properties.script[i])) == _lib.end())
	      {
		_lib[it->second._properties.script[i]] = Memory::LibraryLoader();
		itLib = _lib.find(it->second._properties.script[i]);
	      }
	    try {
	      loadLibrary(it->second._properties, itLib->second, it->second._usable, i);
	    } catch (PrefabLibraryLoaderError e) {
	      std::cout << e.what() << std::endl;
	      throw PrefabLoadFileError();
	    }
	    std:: cout << "lib found " << it->second._properties.script[i]
		       << ", loading script" << std::endl;
	  }
      }
  }

  void PrefabCreator::getName(Properties & _properties)
  {
    std::string	name(_parser.getValue("Name"));

    if (!name.empty())
      _properties.name = name;
  }

  void PrefabCreator::getTag(Properties & _properties)
  {
    std::string	tag(_parser.getValue("Tag"));

    if (!tag.empty())
      _properties.tag = tag;
  }

  void PrefabCreator::getScript(Properties & _properties)
  {
    std::vector<std::string>  script = _parser.stoa(_parser.getValue("Script"), ';');

    if (!script.empty())
      for (int i = 0; i < script.size(); ++i)
	{
#if defined(__GNUC__)
	  _properties.script.push_back(std::string("RtypeSRC/script/binaries/lib" + script[i]));
#elif defined(_MSC_VER)
	  _properties.script.push_back(std::string(script[i]));
#endif
	}
  }

  void PrefabCreator::loadLibrary(Properties & _properties, Memory::LibraryLoader & _library,
				  bool &_usable, int it)
  {
    try {
      _library.loadSharedLibrary(_properties.script[it]);
    } catch (Memory::LibraryLoadError e) {
      std ::cout << e.what() << std::endl;
      std::cout << std::string("load error message") + _library.logError() << std::endl;
      _usable = false;
      throw PrefabLibraryLoaderError();
    }
    try {
      _properties.skillFunction.push_back((skillScript)_library.getFunction("getScriptInstance"));
    } catch (Memory::LibraryFunctionLoadError e) {
      std::cout << e.what() << std::endl;
      _usable = false;
      throw PrefabLibraryLoaderError();
    }
  }

  /*void PrefabCreator::getCollider(Properties & _properties)
  {
    std::string collider(_parser.getValue("Collider"));

    if (!collider.empty())
      _properties.collider = _parser.stob(collider);
  }*/

  void PrefabCreator::getPosition(Properties & _properties)
  {
    std::vector<std::string>  position = _parser.stoa(_parser.getValue("Position"), ';');

    if (position.size() == 3)
      {
	_properties.position.x = _parser.stoi(position[0]);
	_properties.position.y = _parser.stoi(position[1]);
      }
    else
      _properties.position = Vector2F(0, 0);
  }

  void PrefabCreator::getScale(Properties & _properties)
  {
    std::vector<std::string>  scale = _parser.stoa(_parser.getValue("Scale"), ';');

    if (scale.size() == 3)
      {
	_properties.scale.x = _parser.stof(scale[0]);
	_properties.scale.y = _parser.stof(scale[1]);
      }
    else
      _properties.scale = Vector2F(1, 1);
  }

  //instantiate private methode

  void PrefabCreator::instantiateRenderObject(Properties & _properties,
					      GameObject *&gameObject,
					      Vector2F const & position,
					      Vector2F const & scale)
  {
    instantiateMeshObject(_properties, gameObject, position, scale);
  }

  void PrefabCreator::instantiateMeshObject(Properties & _properties,
					    GameObject *&gameObject,
					    Vector2F const & position,
					    Vector2F const & scale)
  {
    //gameObject->setCollider();
    if (gameObject->setPositionF(position == Vector2F(0,0) ?
				_properties.position : position) == false)
      {
	delete gameObject;
	gameObject = NULL;
	return ;
      }
    gameObject->setScale(scale == Vector2F(1, 1) ?
			 _properties.scale : scale);
    return ;
  }

};
