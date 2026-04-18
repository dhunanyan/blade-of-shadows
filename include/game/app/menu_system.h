#ifndef GAME_APP_MENU_SYSTEM_H
#define GAME_APP_MENU_SYSTEM_H

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

struct MenuItem
{
  std::string label;
  std::function<void()> onActivate;
};

struct MenuDefinition
{
  std::string id;
  std::string title;
  std::vector<MenuItem> items;
};

struct MenuView
{
  bool visible = false;
  std::string title;
  std::vector<std::string> items;
  int selectedIndex = 0;
};

class MenuSystem
{
public:
  void registerMenu(MenuDefinition menu);
  bool openRoot(const std::string& menuId);
  bool pushMenu(const std::string& menuId);
  bool popMenu();

  void moveSelection(int delta);
  bool activateSelected();

  bool isOpen() const
  {
    return !stack_.empty();
  }
  bool isRootMenuOpen() const
  {
    return stack_.size() <= 1;
  }
  const std::string& currentMenuId() const;
  MenuView view() const;

private:
  const MenuDefinition* currentMenu() const;
  int& selectedIndexForCurrentMenu();
  int selectedIndexForCurrentMenu() const;

private:
  std::unordered_map<std::string, MenuDefinition> menus_;
  std::vector<std::string> stack_;
  std::unordered_map<std::string, int> selectedIndices_;
  std::string emptyId_;
};

#endif // GAME_APP_MENU_SYSTEM_H
