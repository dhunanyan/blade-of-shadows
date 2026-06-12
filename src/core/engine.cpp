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
  const int middleX = static_cast<int>(stage_.width() / 2);
  const int middleY = static_cast<int>(stage_.height() / 2);
  player_.setPosition(Position{middleX, middleY});
  playerPixelX_ = static_cast<float>(middleX) * 24.0f;
  playerPixelY_ = static_cast<float>(middleY) * 24.0f;
  player_.setDirection(Direction::RIGHT);
  player_.setIsAlive(true);
}

void Engine::update()
{
  ++updateTick_;
  if (player_.invulnerabilityTicks() > 0)
  {
    player_.setInvulnerabilityTicks(player_.invulnerabilityTicks() - 1);
  }
  if (player_.doubleJumpFxTicks() > 0)
  {
    player_.setDoubleJumpFxTicks(player_.doubleJumpFxTicks() - 1);
  }
  updatePlayerAttackState(player_);
  handlePlayerJump(player_);
  updatePlayerDodgeState(player_);
  applyHorizontalMovement(player_);
  applyGravity(player_);
  updateEnemies();
  resolvePlayerAttack();
  resolveEnemyContact();
  resolveCollectibles();
  resolveLevelExit();
}

void Engine::updatePlayerDodgeState(Player& player)
{
  constexpr int dodgeDurationTicks = 12;
  constexpr int dodgeCooldownTicks = 28;

  if (player.dodgeCooldownTicks() > 0)
  {
    player.setDodgeCooldownTicks(player.dodgeCooldownTicks() - 1);
  }
  if (player.dodgeTicks() > 0)
  {
    player.setDodgeTicks(player.dodgeTicks() - 1);
  }
  if (player.dodgeRequested() && player.isGrounded() && player.dodgeCooldownTicks() <= 0)
  {
    player.setDodgeTicks(dodgeDurationTicks);
    player.setDodgeCooldownTicks(dodgeCooldownTicks);
  }
  player.setDodgeRequested(false);
}

void Engine::updatePlayerAttackState(Player& player)
{
  constexpr int attackDurationTicks = 18;

  if (!player.attackInProgress())
  {
    if (player.attackRequested())
    {
      player.setAttackInProgress(true);
      player.setAttackTicksLeft(attackDurationTicks);
      player.setAttackDamageApplied(false);
      player.setAttackRequested(false);
    }
    return;
  }

  int ticksLeft = player.attackTicksLeft();
  if (ticksLeft > 0)
  {
    --ticksLeft;
  }
  player.setAttackTicksLeft(ticksLeft);

  if (ticksLeft <= 0)
  {
    if (player.attackHeld())
    {
      player.setAttackTicksLeft(attackDurationTicks);
      player.setAttackDamageApplied(false);
    }
    else
    {
      player.setAttackInProgress(false);
    }
  }
}

void Engine::handlePlayerJump(Player& player)
{
  constexpr float groundJumpImpulse = -4.6f;
  constexpr float airJumpImpulse = -3.75f;
  constexpr float groundMaxHoldTime = 0.14f;
  constexpr float airMaxHoldTime = 0.10f;
  constexpr float groundHoldBoost = -0.075f;
  constexpr float airHoldBoost = -0.06f;
  constexpr int maxAirJumps = 1;
  constexpr int doubleJumpFxDurationTicks = 16;
  constexpr int jumpApexHangDurationTicks = 8; // brief apex hang
  constexpr float tileSizePx = 24.0f;

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

  // Keep grounded state synchronized at jump decision time
  player.setIsGrounded(isSolidBelowAtPixelY(playerPixelY_));

  if (player.isGrounded())
  {
    player.setRemainingAirJumps(maxAirJumps);
  }

  if (player.jumpRequested())
  {
    const bool canJumpFromGround = player.isGrounded();
    const bool canJumpInAir = !player.isGrounded() && player.remainingAirJumps() > 0;

    if (canJumpFromGround || canJumpInAir)
    {
      const bool isGroundJump = canJumpFromGround;
      if (canJumpInAir)
      {
        player.setRemainingAirJumps(player.remainingAirJumps() - 1);
        player.setDoubleJumpFxTicks(doubleJumpFxDurationTicks);
      }
      player.setVelocityY(isGroundJump ? groundJumpImpulse : airJumpImpulse);
      player.setIsGrounded(false);
      player.setJumpHoldTime(0.0f);
      player.setJumpApexHangTicks(jumpApexHangDurationTicks);
    }
  }
  player.setJumpRequested(false);

  const bool boostingGroundJump = player.remainingAirJumps() == maxAirJumps;
  const float maxHoldTime = boostingGroundJump ? groundMaxHoldTime : airMaxHoldTime;
  const float holdBoost = boostingGroundJump ? groundHoldBoost : airHoldBoost;

  if (
    !player.isGrounded() && 
    player.jumpHeld() && 
    player.velocityY() < 0.0f &&
    player.jumpHoldTime() < maxHoldTime
  )
  {
    player.setVelocityY(player.velocityY() + holdBoost);
    player.setJumpHoldTime(player.jumpHoldTime() + (1.0f / 60.0f));
  }

  if (!player.jumpHeld() && player.velocityY() < -1.2f) {
    player.setVelocityY(-1.2f);
  }
}

void Engine::applyGravity(Player& player)
{
  constexpr float tileSizePx = 24.0f;
  constexpr float gravityUp = 0.19f;
  constexpr float gravityDown = 0.42f;
  constexpr float maxFallSpeed = 3.10f;
  constexpr float apexVelocityThreshold = 0.22f;

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
  auto isSolidAboveAtPixelY = [&](float pixelY) -> bool
  {
    if (pixelY < 0.0f)
    {
      return true;
    }
    const int topCellY = static_cast<int>(std::floor(pixelY / tileSizePx));
    const int leftCellX = static_cast<int>(std::floor(playerPixelX_ / tileSizePx));
    const int rightCellX = static_cast<int>(std::floor((playerPixelX_ + tileSizePx - 1.0f) / tileSizePx));
    return isSolidAt(leftCellX, topCellY) || isSolidAt(rightCellX, topCellY);
  };

  player.setIsGrounded(isSolidBelowAtPixelY(playerPixelY_));

  if (player.isGrounded())
  {
    player.setVelocityY(0.0f);
    player.setJumpHoldTime(0.0f);
    player.setJumpApexHangTicks(0);
    playerPixelY_ = std::floor(playerPixelY_ / tileSizePx) * tileSizePx;
  }
  else
  {
    if (std::abs(player.velocityY()) <= apexVelocityThreshold && player.jumpApexHangTicks() > 0)
    {
      player.setVelocityY(0.0f);
      player.setJumpApexHangTicks(player.jumpApexHangTicks() - 1);
      syncPlayerGridPosition();
      return;
    }

    const float gravity = (player.velocityY() < 0.0f) ? gravityUp : gravityDown;
    float vy = player.velocityY() + gravity;
    if (vy > maxFallSpeed)
    {
      vy = maxFallSpeed;
    }
    if (std::abs(vy) < 0.05f)
    {
      vy = 0.0f;
    }
    player.setVelocityY(vy);

    float remaining = player.velocityY();
    if (remaining > 0.0f)
    {
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
    } else if (remaining < 0.0f)
    {
      while (remaining < 0.0f) {
        const float step = std::max(-1.0f, remaining);
        const float candidateY = playerPixelY_ + step;

        if (isSolidAboveAtPixelY(candidateY))
        {
          player.setVelocityY(0.0f);
          break;
        }

        playerPixelY_ = candidateY;
        remaining -= step; // step is negative
      }
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

  if (updateTick_ % 12 != 0)
  {
    return;
  }

  const Position playerPos = player_.position();
  for (const auto& enemy : enemies_)
  {
    if (!enemy)
    {
      continue;
    }

    const int dx = signum(static_cast<int>(playerPos.x()) - static_cast<int>(enemy->position().x()));
    Position candidate = enemy->position();
    if (dx > 0 && candidate.x() + 1 < stage_.width())
    {
      candidate = Position(candidate.x() + 1, candidate.y());
    }
    else if (dx < 0 && candidate.x() > 0)
    {
      candidate = Position(candidate.x() - 1, candidate.y());
    }

    if (canEnemyOccupy(candidate))
    {
      enemy->setPosition(candidate);
    }
  }
}

void Engine::resetSession(
    Position playerSpawn,
    const std::vector<Position>& enemySpawns,
    const std::vector<Position>& coinSpawns,
    std::optional<Position> levelExit)
{
  enemies_.clear();
  enemies_.reserve(enemySpawns.size());
  for (const Position& spawn : enemySpawns)
  {
    if (stage_.isInside(spawn))
    {
      enemies_.push_back(std::make_shared<Enemy>(spawn));
    }
  }

  coins_.clear();
  for (const Position& coin : coinSpawns)
  {
    if (stage_.isInside(coin))
    {
      coins_.push_back(coin);
    }
  }

  levelExit_ = levelExit;
  levelComplete_ = false;
  updateTick_ = 0;
  player_.resetProgress();
  player_.setPosition(playerSpawn);
  player_.setDirection(Direction::RIGHT);
  player_.setVelocityY(0.0f);
  player_.setIsGrounded(false);
  player_.setRemainingAirJumps(1);
  player_.setAttackInProgress(false);
  player_.setAttackHeld(false);
  player_.setAttackRequested(false);
  player_.setDodgeRequested(false);
  player_.setDodgeTicks(0);
  player_.setDodgeCooldownTicks(0);
  playerPixelX_ = static_cast<float>(playerSpawn.x()) * 24.0f;
  playerPixelY_ = static_cast<float>(playerSpawn.y()) * 24.0f;
  playerMoveIntentX_ = 0;
}

Engine::Snapshot Engine::snapshot() const
{
  Snapshot state;
  state.playerPixelX = playerPixelX_;
  state.playerPixelY = playerPixelY_;
  state.playerHealth = player_.health();
  state.playerMaxHealth = player_.maxHealth();
  state.coins = player_.coins();
  state.score = player_.score();
  state.remainingCoins = coins_;
  state.levelComplete = levelComplete_;
  state.enemies.reserve(enemies_.size());
  for (const auto& enemy : enemies_)
  {
    if (enemy && enemy->isAlive())
    {
      state.enemies.push_back(Snapshot::EnemyState{
          enemy->position(),
          enemy->life(),
          enemy->maxLife()});
    }
  }
  return state;
}

void Engine::restoreSnapshot(const Snapshot& state)
{
  player_.setMaxHealth(state.playerMaxHealth);
  player_.setHealth(state.playerHealth);
  player_.setCoins(state.coins);
  player_.setScore(state.score);
  coins_ = state.remainingCoins;
  enemies_.clear();
  enemies_.reserve(state.enemies.size());
  for (const Snapshot::EnemyState& enemyState : state.enemies)
  {
    auto enemy = std::make_shared<Enemy>(enemyState.position, enemyState.maxLife);
    enemy->decreaseLife(enemyState.maxLife - enemyState.life);
    enemies_.push_back(std::move(enemy));
  }
  levelComplete_ = state.levelComplete;
  setPlayerPixelX(state.playerPixelX);
  setPlayerPixelY(state.playerPixelY);
  player_.setVelocityY(0.0f);
  player_.setIsGrounded(false);
}

void Engine::resolvePlayerAttack()
{
  if (!player_.attackInProgress() || player_.attackDamageApplied())
  {
    return;
  }

  constexpr int attackHitTick = 12;
  constexpr float attackDamage = 50.0f;
  if (player_.attackTicksLeft() > attackHitTick)
  {
    return;
  }

  const int playerX = static_cast<int>(player_.position().x());
  const int playerY = static_cast<int>(player_.position().y());
  const int direction = player_.direction() == Direction::LEFT ? -1 : 1;

  for (const auto& enemy : enemies_)
  {
    if (!enemy || !enemy->isAlive())
    {
      continue;
    }
    const int enemyX = static_cast<int>(enemy->position().x());
    const int enemyY = static_cast<int>(enemy->position().y());
    const int forwardDistance = (enemyX - playerX) * direction;
    if (forwardDistance >= 0 && forwardDistance <= 2 && std::abs(enemyY - playerY) <= 1)
    {
      const bool wasAlive = enemy->isAlive();
      enemy->decreaseLife(attackDamage);
      if (wasAlive && !enemy->isAlive())
      {
        player_.addScore(100);
      }
    }
  }
  player_.setAttackDamageApplied(true);
}

void Engine::resolveEnemyContact()
{
  if (!player_.isAlive() || player_.invulnerabilityTicks() > 0 || player_.isDodging())
  {
    return;
  }

  for (const auto& enemy : enemies_)
  {
    if (!enemy || !enemy->isAlive())
    {
      continue;
    }
    const int dx = std::abs(static_cast<int>(enemy->position().x()) - static_cast<int>(player_.position().x()));
    const int dy = std::abs(static_cast<int>(enemy->position().y()) - static_cast<int>(player_.position().y()));
    if (dx <= 1 && dy <= 1)
    {
      player_.takeDamage(enemy->contactDamage());
      player_.setInvulnerabilityTicks(60);
      break;
    }
  }
}

void Engine::resolveCollectibles()
{
  const Position playerPosition = player_.position();
  const auto oldSize = coins_.size();
  coins_.erase(
      std::remove_if(
          coins_.begin(),
          coins_.end(),
          [&](const Position& coin)
          {
            return coin.x() == playerPosition.x() && coin.y() == playerPosition.y();
          }),
      coins_.end());

  const int collected = static_cast<int>(oldSize - coins_.size());
  if (collected > 0)
  {
    player_.addCoin(collected);
    player_.addScore(collected * 10);
  }
}

void Engine::resolveLevelExit()
{
  if (!levelExit_ || !coins_.empty())
  {
    return;
  }

  const Position playerPosition = player_.position();
  levelComplete_ =
      playerPosition.x() == levelExit_->x() &&
      playerPosition.y() == levelExit_->y();
}

bool Engine::canEnemyOccupy(const Position& position) const
{
  if (!stage_.isInside(position))
  {
    return false;
  }
  const int x = static_cast<int>(position.x());
  const int y = static_cast<int>(position.y());
  return !isSolidAt(x, y) &&
         (y + 1 >= static_cast<int>(stage_.height()) || isSolidAt(x, y + 1));
}

bool Engine::isSolidAt(int gridX, int gridY) const
{
  if (!solidQuery_) return false;
  return solidQuery_(gridX, gridY);
}

bool Engine::willPlayerTouchGroundSoon(float lookAheadPx) const
{
  constexpr float tileSizePx = 24.0f;
  const float safeLookAheadPx = std::max(0.0f, lookAheadPx);

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

  return isSolidBelowAtPixelY(playerPixelY_ + safeLookAheadPx);
}

void Engine::applyHorizontalMovement(Player& player)
{
  constexpr float tileSizePx = 24.0f;
  constexpr float playerMoveStepPx = 2.35f;
  constexpr float playerDodgeStepPx = 5.0f;

  int movementIntent = playerMoveIntentX_;
  if (player.isDodging() && movementIntent == 0)
  {
    movementIntent = player.direction() == Direction::LEFT ? -1 : 1;
  }

  if (movementIntent == 0)
  {
    syncPlayerGridPosition();
    return;
  }

  if (movementIntent < 0)
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

  const float moveStep = player.isDodging() ? playerDodgeStepPx : playerMoveStepPx;
  const float desiredMove = moveStep * static_cast<float>(movementIntent);
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

void Engine::setPlayerPixelX(float pixelX)
{
  const float maxX = std::max(0.0f, static_cast<float>(stage_.width() * 24) - 24.0f);
  playerPixelX_ = std::clamp(pixelX, 0.0f, maxX);
  syncPlayerGridPosition();
}

void Engine::setPlayerPixelY(float pixelY)
{
  const float maxY = std::max(0.0f, static_cast<float>(stage_.height() * 24) - 24.0f);
  playerPixelY_ = std::clamp(pixelY, 0.0f, maxY);
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

  player_.setPosition(Position(gridX, gridY));
}

void Engine::randEnemies(Position (*positionGenerator)(int,int))
{
  while (enemies_.size() < maxEnemies_)
  {
    const Position enemyPosition = positionGenerator(static_cast<int>(stageWidthCells()), static_cast<int>(stageHeightCells()));
    enemies_.push_back(std::make_shared<Enemy>(enemyPosition));
  }
}
