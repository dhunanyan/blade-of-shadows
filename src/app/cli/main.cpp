#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include "game/core/engine.h"

namespace {
std::size_t parsePositiveArg(const char* value, std::size_t fallback)
{
  try
  {
    const std::size_t parsed = std::stoull(value);
    return parsed == 0 ? fallback : parsed;
  }
  catch (const std::exception&)
  {
    return fallback;
  }
}
}  // namespace

int main(int argc, char** argv)
{
  if (argc > 1 && std::string(argv[1]) == "--help")
  {
    std::cout << "Headless gameplay simulation runner\n"
              << "Usage: " << argv[0] << " [ticks] [width] [height]\n"
              << "All arguments are optional positive integers.\n";
    return EXIT_SUCCESS;
  }

  std::size_t ticks = 40;
  std::size_t width = 38;
  std::size_t height = 25;

  if (argc > 1)
  {
    ticks = parsePositiveArg(argv[1], ticks);
  }
  if (argc > 2)
  {
    width = parsePositiveArg(argv[2], width);
  }
  if (argc > 3)
  {
    height = parsePositiveArg(argv[3], height);
  }

  Engine engine(width, height);

  for (std::size_t tick = 0; tick < ticks; ++tick)
  {
    switch (tick % 2)
    {
      case 0:
        engine.movePlayerRight();
        break;
      default:
        engine.movePlayerLeft();
        break;
    }

    engine.update();
  }

  const Position playerPosition = engine.playerPosition();
  std::cout << "Simulation finished\n"
            << "ticks: " << ticks << "\n"
            << "stage: " << engine.stageWidthCells() << "x" << engine.stageHeightCells() << "\n"
            << "player: (" << playerPosition.x() << ", " << playerPosition.y() << ")\n"
            << "player_alive: " << (engine.isPlayerAlive() ? "true" : "false") << "\n"
            << "enemies: " << engine.enemies().size() << "\n";

  return EXIT_SUCCESS;
}
