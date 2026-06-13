#include "game/core/engine.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "game/core/enemy.h"

namespace
{
constexpr int maxAirJumps = 1;

int signum(int value)
{
  return (value > 0) - (value < 0);
}
} // namespace

Position generateNewEnemyPosition(int width, int height)
{
  const int yLane = height / 2;
  const int xEdge = (rand() % 2 == 0) ? 0 : width - 1;
  return Position(xEdge, yLane);
}

Engine::Engine(std::size_t stageWidth, std::size_t stageHeight)
    : level_(
          static_cast<int>(std::max<std::size_t>(1, stageWidth)),
          static_cast<int>(std::max<std::size_t>(1, stageHeight)),
          24),
      playerPhysics_(level_)
{
  const Position initialSpawn(
      static_cast<int>(stageWidthCells() / 2),
      static_cast<int>(stageHeightCells() / 2));
  level_.setPlayerSpawn(initialSpawn);
  for (int y = initialSpawn.y() + 1; y < level_.height(); ++y)
  {
    for (int x = 0; x < level_.width(); ++x)
    {
      level_.setSolid(Position(x, y), true);
    }
  }
  resetSession();
}

Engine::~Engine() = default;

void Engine::loadLevel(LevelDefinition level)
{
  level_ = std::move(level);
  resetSession();
}

void Engine::resetSession()
{
  enemies_.clear();
  enemies_.reserve(level_.enemySpawns().size());
  for (const Position& spawn : level_.enemySpawns())
  {
    if (level_.isInside(spawn) && !level_.isSolid(spawn))
    {
      enemies_.push_back(std::make_shared<Enemy>(spawn));
    }
  }

  coins_.clear();
  for (const Position& coin : level_.coinSpawns())
  {
    if (level_.isInside(coin) && !level_.isSolid(coin))
    {
      coins_.push_back(coin);
    }
  }

  levelExit_ =
      level_.exit() && level_.isInside(*level_.exit()) && !level_.isSolid(*level_.exit())
          ? level_.exit()
          : std::nullopt;
  levelComplete_ = false;
  updateTick_ = 0;
  playerMoveIntentX_ = 0;
  events_.clear();

  player_.resetProgress();
  player_.setDirection(Direction::RIGHT);
  player_.body().setVelocity({});
  player_.setRemainingAirJumps(maxAirJumps);
  player_.setAttackInProgress(false);
  player_.setAttackHeld(false);
  player_.setAttackRequested(false);
  player_.setDodgeRequested(false);
  player_.setDodgeTicks(0);
  player_.setDodgeCooldownTicks(0);
  player_.setJumpRequested(false);
  player_.setJumpHeld(false);
  player_.setJumpHoldTime(0.0f);
  player_.setJumpApexHangTicks(0);

  playerPhysics_.spawn(player_, level_.playerSpawn());
}

void Engine::resetSession(
    Position playerSpawn,
    const std::vector<Position>& enemySpawns,
    const std::vector<Position>& coinSpawns,
    std::optional<Position> levelExit)
{
  level_.setPlayerSpawn(playerSpawn);
  level_.setEnemySpawns(enemySpawns);
  level_.setCoinSpawns(coinSpawns);
  level_.setExit(levelExit);
  resetSession();
}

void Engine::update()
{
  events_.clear();
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
  updatePlayerDodgeState(player_);
  playerPhysics_.update(player_, playerMoveIntentX_, events_);
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
      events_.push_back(GameEvent::AttackStarted);
    }
    return;
  }

  const int ticksLeft = std::max(0, player.attackTicksLeft() - 1);
  player.setAttackTicksLeft(ticksLeft);
  if (ticksLeft > 0)
  {
    return;
  }

  if (player.attackHeld())
  {
    player.setAttackTicksLeft(attackDurationTicks);
    player.setAttackDamageApplied(false);
    events_.push_back(GameEvent::AttackStarted);
  }
  else
  {
    player.setAttackInProgress(false);
  }
}

void Engine::updateEnemies()
{
  enemies_.erase(
      std::remove_if(
          enemies_.begin(),
          enemies_.end(),
          [](const std::shared_ptr<Enemy>& enemy)
          {
            return !enemy || !enemy->isAlive();
          }),
      enemies_.end());

  if (updateTick_ % 12 != 0)
  {
    return;
  }

  const Position playerCell = playerPosition();
  for (const auto& enemy : enemies_)
  {
    const int dx = signum(playerCell.x() - enemy->position().x());
    const Position candidate(enemy->position().x() + dx, enemy->position().y());
    if (dx != 0 && canEnemyOccupy(candidate))
    {
      enemy->setPosition(candidate);
    }
  }
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

  const Position playerCell = playerPosition();
  const int direction = player_.direction() == Direction::LEFT ? -1 : 1;
  for (const auto& enemy : enemies_)
  {
    if (!enemy || !enemy->isAlive())
    {
      continue;
    }
    const int forwardDistance = (enemy->position().x() - playerCell.x()) * direction;
    if (forwardDistance >= 0 &&
        forwardDistance <= 2 &&
        std::abs(enemy->position().y() - playerCell.y()) <= 1)
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

  const WorldRect playerBounds = player_.worldBounds();
  for (const auto& enemy : enemies_)
  {
    if (!enemy || !enemy->isAlive())
    {
      continue;
    }
    if (playerBounds.intersects(level_.cellBounds(enemy->position())))
    {
      const int healthBeforeDamage = player_.health();
      player_.takeDamage(enemy->contactDamage());
      player_.setInvulnerabilityTicks(60);
      if (player_.health() < healthBeforeDamage)
      {
        events_.push_back(GameEvent::PlayerDamaged);
        if (!player_.isAlive())
        {
          events_.push_back(GameEvent::PlayerDefeated);
        }
      }
      break;
    }
  }
}

void Engine::resolveCollectibles()
{
  const Position playerCell = playerPosition();
  const auto oldSize = coins_.size();
  coins_.erase(
      std::remove(coins_.begin(), coins_.end(), playerCell),
      coins_.end());

  const int collected = static_cast<int>(oldSize - coins_.size());
  if (collected > 0)
  {
    player_.addCoin(collected);
    player_.addScore(collected * 10);
    events_.push_back(GameEvent::CoinCollected);
  }
}

void Engine::resolveLevelExit()
{
  const bool completed =
      levelExit_ &&
      coins_.empty() &&
      playerPosition() == *levelExit_;
  if (completed && !levelComplete_)
  {
    events_.push_back(GameEvent::LevelCompleted);
  }
  levelComplete_ = completed;
}

bool Engine::canEnemyOccupy(const Position& position) const
{
  return level_.isInside(position) &&
         !level_.isSolid(position) &&
         (position.y() + 1 >= level_.height() ||
          level_.isSolid(position.x(), position.y() + 1));
}

bool Engine::isSolidAt(int gridX, int gridY) const
{
  return level_.isSolid(gridX, gridY);
}

bool Engine::willPlayerTouchGroundSoon(float lookAheadPx) const
{
  return playerPhysics_.willTouchGroundSoon(player_, lookAheadPx);
}

void Engine::setPlayerPixelX(float pixelX)
{
  WorldPoint position = player_.worldPosition();
  const float maxX = std::max(
      0.0f,
      level_.width() * static_cast<float>(level_.tileSize()) - player_.body().size().width);
  position.x = std::clamp(pixelX, 0.0f, maxX);
  playerPhysics_.setWorldPosition(player_, position);
}

void Engine::setPlayerPixelY(float pixelY)
{
  WorldPoint position = player_.worldPosition();
  const float maxY = std::max(
      0.0f,
      level_.height() * static_cast<float>(level_.tileSize()) - player_.body().size().height);
  position.y = std::clamp(pixelY, 0.0f, maxY);
  playerPhysics_.setWorldPosition(player_, position);
}

Engine::Snapshot Engine::snapshot() const
{
  Snapshot state;
  state.playerPixelX = player_.worldPosition().x;
  state.playerPixelY = player_.worldPosition().y;
  state.playerVelocityY = player_.velocityY();
  state.playerDirection = player_.direction();
  state.remainingAirJumps = player_.remainingAirJumps();
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
  coins_.clear();
  for (const Position& coin : state.remainingCoins)
  {
    if (level_.isInside(coin) && !level_.isSolid(coin))
    {
      coins_.push_back(coin);
    }
  }

  enemies_.clear();
  for (const Snapshot::EnemyState& enemyState : state.enemies)
  {
    if (!level_.isInside(enemyState.position) || level_.isSolid(enemyState.position))
    {
      continue;
    }
    auto enemy = std::make_shared<Enemy>(enemyState.position, enemyState.maxLife);
    enemy->decreaseLife(enemyState.maxLife - enemyState.life);
    enemies_.push_back(std::move(enemy));
  }

  WorldPoint restored{state.playerPixelX, state.playerPixelY};
  const WorldRect restoredBounds{restored, player_.body().size()};
  if (!playerPhysics_.canOccupy(restoredBounds))
  {
    playerPhysics_.spawn(player_, level_.playerSpawn());
  }
  else
  {
    playerPhysics_.setWorldPosition(player_, restored);
  }
  player_.setDirection(state.playerDirection);
  player_.setVelocityY(state.playerVelocityY);
  player_.setRemainingAirJumps(std::clamp(state.remainingAirJumps, 0, maxAirJumps));
  player_.setIsGrounded(playerPhysics_.isGrounded(player_.body()));
  levelComplete_ = state.levelComplete && coins_.empty();
}

void Engine::randEnemies(Position (*positionGenerator)(int, int))
{
  while (enemies_.size() < maxEnemies_)
  {
    const Position position =
        positionGenerator(level_.width(), level_.height());
    if (level_.isInside(position))
    {
      enemies_.push_back(std::make_shared<Enemy>(position));
    }
  }
}
