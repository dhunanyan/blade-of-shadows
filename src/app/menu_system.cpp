#include "game/app/menu_system.h"
#include <algorithm>
#include <utility>

void MenuSystem::registerMenu(MenuDefinition menu)
{
  if (menu.id.empty() || menu.items.empty())
  {
    return;
  }
  selectedIndices_.try_emplace(menu.id, 0);
  menus_[menu.id] = std::move(menu);
}

bool MenuSystem::openRoot(const std::string& menuId)
{
  if (!menus_.contains(menuId))
  {
    return false;
  }
  stack_.clear();
  stack_.push_back(menuId);
  return true;
}

bool MenuSystem::pushMenu(const std::string& menuId)
{
  if (!menus_.contains(menuId))
  {
    return false;
  }
  stack_.push_back(menuId);
  return true;
}

bool MenuSystem::popMenu()
{
  if (stack_.empty())
  {
    return false;
  }
  stack_.pop_back();
  return true;
}

void MenuSystem::close()
{
  stack_.clear();
}

void MenuSystem::moveSelection(int delta)
{
  const MenuDefinition* menu = currentMenu();
  if (menu == nullptr || menu->items.empty() || delta == 0)
  {
    return;
  }

  const int itemCount = static_cast<int>(menu->items.size());
  int next = selectedIndexForCurrentMenu() + delta;
  if (next < 0)
  {
    next = itemCount - 1;
  }
  else if (next >= itemCount)
  {
    next = 0;
  }
  selectedIndexForCurrentMenu() = next;
}

void MenuSystem::setSelection(int index)
{
  const MenuDefinition* menu = currentMenu();
  if (menu == nullptr || menu->items.empty())
  {
    return;
  }
  const int clampedIndex = std::clamp(index, 0, static_cast<int>(menu->items.size()) - 1);
  selectedIndexForCurrentMenu() = clampedIndex;
}

bool MenuSystem::activateSelected()
{
  const MenuDefinition* menu = currentMenu();
  if (menu == nullptr || menu->items.empty())
  {
    return false;
  }

  const int index = std::clamp(selectedIndexForCurrentMenu(), 0, static_cast<int>(menu->items.size()) - 1);
  const auto callback = menu->items[static_cast<std::size_t>(index)].onActivate;
  if (callback)
  {
    callback();
    return true;
  }
  return false;
}

bool MenuSystem::activateAt(int index)
{
  setSelection(index);
  return activateSelected();
}

const std::string& MenuSystem::currentMenuId() const
{
  if (stack_.empty())
  {
    return emptyId_;
  }
  return stack_.back();
}

MenuView MenuSystem::view() const
{
  MenuView out{};
  const MenuDefinition* menu = currentMenu();
  if (menu == nullptr)
  {
    return out;
  }

  out.visible = true;
  out.title = menu->title;
  out.selectedIndex = selectedIndexForCurrentMenu();
  out.items.reserve(menu->items.size());
  for (const auto& item : menu->items)
  {
    out.items.push_back(item.label);
  }

  return out;
}

const MenuDefinition* MenuSystem::currentMenu() const
{
  if (stack_.empty())
  {
    return nullptr;
  }
  const auto it = menus_.find(stack_.back());
  if (it == menus_.end())
  {
    return nullptr;
  }
  return &it->second;
}

int& MenuSystem::selectedIndexForCurrentMenu()
{
  const auto& menuId = currentMenuId();
  return selectedIndices_[menuId];
}

int MenuSystem::selectedIndexForCurrentMenu() const
{
  const auto& menuId = currentMenuId();
  const auto it = selectedIndices_.find(menuId);
  if (it == selectedIndices_.end())
  {
    return 0;
  }
  return it->second;
}
