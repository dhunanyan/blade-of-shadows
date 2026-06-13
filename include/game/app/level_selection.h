#ifndef GAME_APP_LEVEL_SELECTION_H
#define GAME_APP_LEVEL_SELECTION_H

#include <string>

struct LevelSelectionEntry
{
  std::string id;
  std::string label;
  bool unlocked = true;
  bool custom = false;
};

#endif // GAME_APP_LEVEL_SELECTION_H
