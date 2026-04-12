#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "game/core/engine.h"
#include "game/core/player.h"
#include "game/core/stage.h"
#include "game/core/enemy.h"

namespace
{
inline auto signum(int x)
{
  return (x > 0) - (x < 0);
}

std::size_t defaultLaneY(int height)
{
  return static_cast<std::size_t>(height / 2);
}
} // namespace


Position generateNewEnemyPosition(int width, int height)
{
  const auto yLane = defaultLaneY(height);
  const auto xEdge = (rand() % 2 == 0) ? 0 : static_cast<std::size_t>(width - 1);
  return Position(xEdge, yLane);
}


Engine::~Engine() = default;


Engine::Engine(std::size_t stageWidth, std::size_t stageHeight):
  stage_(stageWidth, stageHeight)
{
  const decltype(Position::x_) middleX = stage_.width() / 2;
  const decltype(Position::y_) middleY = stage_.height() / 2;
  player_.setPosition(Position{middleX, middleY});
  playerPixelX_ = static_cast<float>(middleX) * 24.0f;
  playerPixelY_ = static_cast<float>(middleY) * 24.0f;
  player_.setDirection(Direction::RIGHT);
  player_.setIsAlive(true);
}

void Engine::update()
{
  updatePlayerAttackState();
  applyHorizontalMovement(player_);
  applyGravity(player_);
  updateEnemies();
  randEnemies();
}

void Engine::updatePlayerAttackState()
{
  constexpr int attackDurationTicks = 18;

  if (!player_.attackInProgress())
  {
    if (player_.attackRequested())
    {
      player_.setAttackInProgress(true);
      player_.setAttackTicksLeft(attackDurationTicks);
      player_.setAttackRequested(false);
    }
    return;
  }

  int ticksLeft = player_.attackTicksLeft();
  if (ticksLeft > 0)
  {
    --ticksLeft;
  }
  player_.setAttackTicksLeft(ticksLeft);

  if (ticksLeft <= 0)
  {
    if (player_.attackHeld())
    {
      player_.setAttackTicksLeft(attackDurationTicks);
    }
    else
    {
      player_.setAttackInProgress(false);
    }
  }
}

void Engine::applyGravity(Player& player)
{
  constexpr float tileSizePx = 24.0f;
  constexpr float gravity = 0.144f;
  constexpr float maxFallSpeed = 1.28f;

  auto isSolidBelowAtPixelY = [&](float pixelY) -> bool
  {
    if (pixelY < 0.0f)
    {
      return false;
    }
    const int topCell = static_cast<int>(std::floor(pixelY / tileSizePx));
    const int belowCellY = topCell + 1;
    if (belowCellY >= static_cast<int>(stage_.height()))
    {
      return true;
    }

    const int leftCellX = static_cast<int>(std::floor(playerPixelX_ / tileSizePx));
    const int rightCellX = static_cast<int>(std::floor((playerPixelX_ + tileSizePx - 1.0f) / tileSizePx));
    return isSolidAt(leftCellX, belowCellY) || isSolidAt(rightCellX, belowCellY);
  };

  player.setIsGrounded(isSolidBelowAtPixelY(playerPixelY_));

  if (player.isGrounded())
  {
    player.setVelocityY(0.0f);
    playerPixelY_ = std::floor(playerPixelY_ / tileSizePx) * tileSizePx;
  }
  else
  {
    float vy = player.velocityY() + gravity;
    if (vy > maxFallSpeed)
    {
      vy = maxFallSpeed;
    }
    player.setVelocityY(vy);

    float remaining = player.velocityY();
    while (remaining > 0.0f)
    {
      const float step = std::min(1.0f, remaining);
      const float candidateY = playerPixelY_ + step;
      if (isSolidBelowAtPixelY(candidateY))
      {
        player.setIsGrounded(true);
        player.setVelocityY(0.0f);
        break;
      }

      playerPixelY_ = candidateY;
      player.setIsGrounded(false);
      remaining -= step;
    }
  }

  syncPlayerGridPosition();
}

void Engine::applyGravity(Enemy& enemy)
{
  if (!enemy.isGrounded())
  {
    const float gravity = 0.056f;
    const float maxFallSpeed = 0.48f;

    float vy = enemy.velocityY() + gravity;
    if (vy > maxFallSpeed) vy = maxFallSpeed;
    enemy.setVelocityY(vy);
  }
}

void Engine::updateEnemies()
{
  enemies_.erase(
      std::remove_if(
          enemies_.begin(), enemies_.end(),
          [](const std::shared_ptr<Enemy>& enemy)
          { 
            return !enemy || !enemy->isAlive();
          }
      ),
      enemies_.end());

  const Position playerPos = player_.position();
  for (const auto& enemy : enemies_)
  {
    if (!enemy || !enemy->shouldIMoveThisTime())
    {
      continue;
    }

    const int dx = signum(static_cast<int>(playerPos.x()) - static_cast<int>(enemy->position().x()));
    if (dx > 0)
    {
      enemy->moveRight();
    }
    else if (dx < 0)
    {
      enemy->moveLeft();
    }
  }
}

bool Engine::isSolidAt(int gridX, int gridY) const
{
  if (!solidQuery_) return false;
  return solidQuery_(gridX, gridY);
}

void Engine::applyHorizontalMovement(Player& player)
{
  constexpr float tileSizePx = 24.0f;
  constexpr float playerMoveStepPx = 2.0f;

  if (playerMoveIntentX_ == 0)
  {
    syncPlayerGridPosition();
    return;
  }

  if (playerMoveIntentX_ < 0)
  {
    player.setDirection(Direction::LEFT);
  }
  else
  {
    player.setDirection(Direction::RIGHT);
  }

  auto canOccupyAtPixel = [&](float leftX, float topY) -> bool
  {
    const float rightX = leftX + tileSizePx - 1.0f;
    const float bottomY = topY + tileSizePx - 1.0f;
    const float levelWidthPx = static_cast<float>(stage_.width()) * tileSizePx;
    const float levelHeightPx = static_cast<float>(stage_.height()) * tileSizePx;

    if (leftX < 0.0f || rightX >= levelWidthPx || topY < 0.0f || bottomY >= levelHeightPx)
    {
      return false;
    }

    const int leftCell = static_cast<int>(std::floor(leftX / tileSizePx));
    const int rightCell = static_cast<int>(std::floor(rightX / tileSizePx));
    const int topCell = static_cast<int>(std::floor(topY / tileSizePx));
    const int bottomCell = static_cast<int>(std::floor(bottomY / tileSizePx));

    return !isSolidAt(leftCell, topCell) &&
           !isSolidAt(rightCell, topCell) &&
           !isSolidAt(leftCell, bottomCell) &&
           !isSolidAt(rightCell, bottomCell);
  };

  const float desiredMove = playerMoveStepPx * static_cast<float>(playerMoveIntentX_);
  float remaining = std::abs(desiredMove);
  const float sign = (desiredMove < 0.0f) ? -1.0f : 1.0f;

  while (remaining > 0.0f)
  {
    const float step = std::min(1.0f, remaining) * sign;
    const float candidateX = playerPixelX_ + step;
    if (!canOccupyAtPixel(candidateX, playerPixelY_))
    {
      break;
    }

    playerPixelX_ = candidateX;
    remaining -= std::abs(step);
  }

  syncPlayerGridPosition();
}

void Engine::syncPlayerGridPosition()
{
  constexpr float tileSizePx = 24.0f;

  const int maxGridX = static_cast<int>(stage_.width()) - 1;
  const int maxGridY = static_cast<int>(stage_.height()) - 1;

  const float centerX = playerPixelX_ + tileSizePx * 0.5f;
  int gridX = static_cast<int>(std::floor(centerX / tileSizePx));
  int gridY = static_cast<int>(std::floor(playerPixelY_ / tileSizePx));

  if (gridX < 0) gridX = 0;
  if (gridX > maxGridX) gridX = maxGridX;
  if (gridY < 0) gridY = 0;
  if (gridY > maxGridY) gridY = maxGridY;

  player_.setPosition(Position(static_cast<std::size_t>(gridX), static_cast<std::size_t>(gridY)));
}

void Engine::randEnemies(Position (*positionGenerator)(int,int))
{
  while (enemies_.size() < maxEnemies_)
  {
    const Position enemyPosition = positionGenerator(static_cast<int>(stageWidthCells()), static_cast<int>(stageHeightCells()));
    enemies_.push_back(std::make_shared<Enemy>(enemyPosition));
  }
}
