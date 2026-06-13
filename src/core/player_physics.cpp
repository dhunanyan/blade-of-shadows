#include "game/core/player_physics.h"

#include <algorithm>
#include <cmath>

namespace
{
constexpr float groundJumpImpulse = -4.6f;
constexpr float airJumpImpulse = -3.75f;
constexpr float groundMaxHoldTime = 0.14f;
constexpr float airMaxHoldTime = 0.10f;
constexpr float groundHoldBoost = -0.075f;
constexpr float airHoldBoost = -0.06f;
constexpr float gravityUp = 0.19f;
constexpr float gravityDown = 0.42f;
constexpr float maxFallSpeed = 3.10f;
constexpr float apexVelocityThreshold = 0.22f;
constexpr float playerMoveStep = 2.35f;
constexpr float playerDodgeStep = 5.0f;
constexpr int maxAirJumps = 1;
constexpr int doubleJumpFxDurationTicks = 16;
constexpr int jumpApexHangDurationTicks = 8;
} // namespace

void PlayerPhysics::spawn(Player& player, Position requestedSpawn) const
{
  const Position spawn =
      level_.resolveStandingSpawn(requestedSpawn, player.body().size());
  player.setWorldPosition(
      level_.worldPositionForStandingCell(spawn, player.body().size()));
  player.body().setVelocity({});
  player.setPosition(spawn);
  player.setIsGrounded(isGrounded(player.body()));
}

void PlayerPhysics::update(
    Player& player,
    int movementIntent,
    std::vector<GameEvent>& events) const
{
  handleJump(player, events);
  moveHorizontally(player, movementIntent);
  applyGravity(player);
  synchronizeGridPosition(player);
}

void PlayerPhysics::handleJump(
    Player& player,
    std::vector<GameEvent>& events) const
{
  player.setIsGrounded(isGrounded(player.body()));
  if (player.isGrounded())
  {
    player.setRemainingAirJumps(maxAirJumps);
  }

  if (player.jumpRequested())
  {
    const bool groundJump = player.isGrounded();
    const bool airJump = !groundJump && player.remainingAirJumps() > 0;
    if (groundJump || airJump)
    {
      if (airJump)
      {
        player.setRemainingAirJumps(player.remainingAirJumps() - 1);
        player.setDoubleJumpFxTicks(doubleJumpFxDurationTicks);
        events.push_back(GameEvent::DoubleJumpStarted);
      }
      else
      {
        events.push_back(GameEvent::JumpStarted);
      }
      player.setVelocityY(groundJump ? groundJumpImpulse : airJumpImpulse);
      player.setIsGrounded(false);
      player.setJumpHoldTime(0.0f);
      player.setJumpApexHangTicks(jumpApexHangDurationTicks);
    }
  }
  player.setJumpRequested(false);

  const bool groundJumpProfile = player.remainingAirJumps() == maxAirJumps;
  const float maxHoldTime =
      groundJumpProfile ? groundMaxHoldTime : airMaxHoldTime;
  const float holdBoost =
      groundJumpProfile ? groundHoldBoost : airHoldBoost;
  if (!player.isGrounded() &&
      player.jumpHeld() &&
      player.velocityY() < 0.0f &&
      player.jumpHoldTime() < maxHoldTime)
  {
    player.setVelocityY(player.velocityY() + holdBoost);
    player.setJumpHoldTime(player.jumpHoldTime() + 1.0f / 60.0f);
  }
  if (!player.jumpHeld() && player.velocityY() < -1.2f)
  {
    player.setVelocityY(-1.2f);
  }
}

void PlayerPhysics::moveHorizontally(Player& player, int movementIntent) const
{
  int intent = std::clamp(movementIntent, -1, 1);
  if (player.isDodging() && intent == 0)
  {
    intent = player.direction() == Direction::LEFT ? -1 : 1;
  }
  if (intent == 0)
  {
    return;
  }

  player.setDirection(intent < 0 ? Direction::LEFT : Direction::RIGHT);
  const float speed = player.isDodging() ? playerDodgeStep : playerMoveStep;
  moveAlongAxis(player, speed * static_cast<float>(intent), true);
}

void PlayerPhysics::applyGravity(Player& player) const
{
  player.setIsGrounded(isGrounded(player.body()));
  if (player.isGrounded() && player.velocityY() >= 0.0f)
  {
    player.setVelocityY(0.0f);
    player.setJumpHoldTime(0.0f);
    player.setJumpApexHangTicks(0);
    return;
  }

  if (std::abs(player.velocityY()) <= apexVelocityThreshold &&
      player.jumpApexHangTicks() > 0)
  {
    player.setVelocityY(0.0f);
    player.setJumpApexHangTicks(player.jumpApexHangTicks() - 1);
    return;
  }

  const float gravity = player.velocityY() < 0.0f ? gravityUp : gravityDown;
  player.setVelocityY(std::min(maxFallSpeed, player.velocityY() + gravity));
  moveAlongAxis(player, player.velocityY(), false);
  player.setIsGrounded(isGrounded(player.body()));
  if (player.isGrounded() && player.velocityY() > 0.0f)
  {
    player.setVelocityY(0.0f);
  }
}

void PlayerPhysics::moveAlongAxis(
    Player& player,
    float amount,
    bool horizontal) const
{
  float remaining = std::abs(amount);
  const float direction = amount < 0.0f ? -1.0f : 1.0f;
  while (remaining > 0.0f)
  {
    const float step = std::min(1.0f, remaining) * direction;
    WorldPoint candidate = player.worldPosition();
    if (horizontal)
    {
      candidate.x += step;
    }
    else
    {
      candidate.y += step;
    }

    if (!canOccupy(WorldRect{candidate, player.body().size()}))
    {
      const float tile = static_cast<float>(level_.tileSize());
      const WorldSize size = player.body().size();
      constexpr float epsilon = 0.001f;

      if (horizontal)
      {
        if (direction > 0.0f)
        {
          const int column =
              static_cast<int>(std::floor((candidate.x + size.width) / tile));
          candidate.x = column * tile - size.width;
        }
        else
        {
          const int column =
              static_cast<int>(std::floor((candidate.x - epsilon) / tile));
          candidate.x = (column + 1) * tile;
        }
      }
      else
      {
        if (direction > 0.0f)
        {
          const int row =
              static_cast<int>(std::floor((candidate.y + size.height) / tile));
          candidate.y = row * tile - size.height;
        }
        else
        {
          const int row =
              static_cast<int>(std::floor((candidate.y - epsilon) / tile));
          candidate.y = (row + 1) * tile;
        }
      }

      if (canOccupy(WorldRect{candidate, size}))
      {
        player.setWorldPosition(candidate);
      }
      if (!horizontal)
      {
        player.setVelocityY(0.0f);
      }
      break;
    }
    player.setWorldPosition(candidate);
    remaining -= std::abs(step);
  }
}

bool PlayerPhysics::canOccupy(const WorldRect& bounds) const
{
  const float tile = static_cast<float>(level_.tileSize());
  constexpr float epsilon = 0.001f;
  if (bounds.left() < 0.0f ||
      bounds.top() < 0.0f ||
      bounds.right() > level_.width() * tile ||
      bounds.bottom() > level_.height() * tile)
  {
    return false;
  }

  const int left = static_cast<int>(std::floor(bounds.left() / tile));
  const int right = static_cast<int>(
      std::floor((bounds.right() - epsilon) / tile));
  const int top = static_cast<int>(std::floor(bounds.top() / tile));
  const int bottom = static_cast<int>(
      std::floor((bounds.bottom() - epsilon) / tile));
  for (int y = top; y <= bottom; ++y)
  {
    for (int x = left; x <= right; ++x)
    {
      if (level_.isSolid(x, y))
      {
        return false;
      }
    }
  }
  return true;
}

bool PlayerPhysics::isGrounded(
    const PhysicsBody& body,
    float lookAhead) const
{
  WorldRect probe = body.bounds();
  probe.origin.y += std::max(0.0f, lookAhead);
  return !canOccupy(probe);
}

bool PlayerPhysics::willTouchGroundSoon(
    const Player& player,
    float lookAhead) const
{
  return isGrounded(player.body(), lookAhead);
}

bool PlayerPhysics::setWorldPosition(
    Player& player,
    WorldPoint position) const
{
  const WorldRect bounds{position, player.body().size()};
  if (!canOccupy(bounds))
  {
    return false;
  }
  player.setWorldPosition(position);
  synchronizeGridPosition(player);
  player.setIsGrounded(isGrounded(player.body()));
  return true;
}

void PlayerPhysics::synchronizeGridPosition(Player& player) const
{
  player.setPosition(player.body().occupiedCell(level_.tileSize()));
}
