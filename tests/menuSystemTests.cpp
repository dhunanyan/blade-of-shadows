#include <gtest/gtest.h>
#include "game/app/menu_system.h"

TEST(MenuSystemTester, selectionWrapsAndMouseActivationUsesRequestedItem)
{
  MenuSystem menus;
  int activated = 0;
  menus.registerMenu(MenuDefinition{
      "main",
      "Main",
      {
          MenuItem{"First", [&activated]() { activated = 1; }},
          MenuItem{"Second", [&activated]() { activated = 2; }},
          MenuItem{"Third", [&activated]() { activated = 3; }},
      }});

  ASSERT_TRUE(menus.openRoot("main"));
  menus.moveSelection(-1);
  EXPECT_EQ(menus.view().selectedIndex, 2);
  EXPECT_TRUE(menus.activateAt(1));
  EXPECT_EQ(activated, 2);
}

TEST(MenuSystemTester, submenuStackReturnsToItsParent)
{
  MenuSystem menus;
  menus.registerMenu(MenuDefinition{"main", "Main", {MenuItem{"Open", []() {}}}});
  menus.registerMenu(MenuDefinition{"options", "Options", {MenuItem{"Back", []() {}}}});

  ASSERT_TRUE(menus.openRoot("main"));
  ASSERT_TRUE(menus.pushMenu("options"));
  EXPECT_EQ(menus.currentMenuId(), "options");
  EXPECT_FALSE(menus.isRootMenuOpen());

  EXPECT_TRUE(menus.popMenu());
  EXPECT_EQ(menus.currentMenuId(), "main");
  EXPECT_TRUE(menus.isRootMenuOpen());
}

TEST(MenuSystemTester, activationMaySafelyReplaceTheCurrentMenu)
{
  MenuSystem menus;
  int callbackCount = 0;
  menus.registerMenu(MenuDefinition{
      "main",
      "Main",
      {MenuItem{
          "Replace",
          [&menus, &callbackCount]()
          {
            ++callbackCount;
            menus.registerMenu(
                MenuDefinition{"main", "Rebuilt", {MenuItem{"Continue", []() {}}}});
          }}}});

  ASSERT_TRUE(menus.openRoot("main"));
  EXPECT_TRUE(menus.activateSelected());
  EXPECT_EQ(callbackCount, 1);
  EXPECT_EQ(menus.view().title, "Rebuilt");
  ASSERT_EQ(menus.view().items.size(), 1U);
  EXPECT_EQ(menus.view().items.front(), "Continue");
}

TEST(MenuSystemTester, longMenusExposeAWindowAroundTheSelection)
{
  MenuSystem menus;
  std::vector<MenuItem> items;
  for (int index = 0; index < 12; ++index)
  {
    items.push_back(MenuItem{"Item " + std::to_string(index), []() {}});
  }
  menus.registerMenu(MenuDefinition{"levels", "Levels", std::move(items)});

  ASSERT_TRUE(menus.openRoot("levels"));
  for (int index = 0; index < 9; ++index)
  {
    menus.moveSelection(1);
  }

  const MenuView view = menus.view(5);
  EXPECT_EQ(view.selectedIndex, 9);
  EXPECT_EQ(view.firstVisibleIndex, 5);
  EXPECT_EQ(view.totalItemCount, 12);
  ASSERT_EQ(view.items.size(), 5U);
  EXPECT_EQ(view.items.front(), "Item 5");
  EXPECT_EQ(view.items.back(), "Item 9");
}
