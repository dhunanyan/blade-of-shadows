#include "game/app/qt/save_game_repository.h"

#include <algorithm>
#include <cmath>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

namespace
{
QJsonObject positionToJson(const Position& position)
{
  return QJsonObject{
      {QString::fromUtf8("x"), static_cast<int>(position.x())},
      {QString::fromUtf8("y"), static_cast<int>(position.y())}};
}

Position positionFromJson(const QJsonObject& object)
{
  return Position(
      std::max(0, object.value(QString::fromUtf8("x")).toInt()),
      std::max(0, object.value(QString::fromUtf8("y")).toInt()));
}

QString directionToString(Direction direction)
{
  return direction == Direction::LEFT
             ? QString::fromUtf8("left")
             : QString::fromUtf8("right");
}

Direction directionFromString(const QString& value)
{
  return value == QString::fromUtf8("left")
             ? Direction::LEFT
             : Direction::RIGHT;
}
} // namespace

QString SaveGameRepository::savePath() const
{
  const QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  return QDir(root).filePath(QString::fromUtf8("savegame.json"));
}

bool SaveGameRepository::exists() const
{
  return QFile::exists(savePath());
}

bool SaveGameRepository::save(const SaveGameData& data) const
{
  QJsonArray coinArray;
  for (const Position& coin : data.snapshot.remainingCoins)
  {
    coinArray.append(positionToJson(coin));
  }

  QJsonArray enemyArray;
  for (const Engine::Snapshot::EnemyState& enemy : data.snapshot.enemies)
  {
    QJsonObject object = positionToJson(enemy.position);
    object.insert(QString::fromUtf8("life"), enemy.life);
    object.insert(QString::fromUtf8("maxLife"), enemy.maxLife);
    enemyArray.append(object);
  }

  const QJsonObject playerObject{
      {QString::fromUtf8("pixelX"), data.snapshot.playerPixelX},
      {QString::fromUtf8("pixelY"), data.snapshot.playerPixelY},
      {QString::fromUtf8("velocityY"), data.snapshot.playerVelocityY},
      {QString::fromUtf8("direction"), directionToString(data.snapshot.playerDirection)},
      {QString::fromUtf8("remainingAirJumps"), data.snapshot.remainingAirJumps},
      {QString::fromUtf8("health"), data.snapshot.playerHealth},
      {QString::fromUtf8("maxHealth"), data.snapshot.playerMaxHealth},
      {QString::fromUtf8("coins"), data.snapshot.coins},
      {QString::fromUtf8("score"), data.snapshot.score}};

  const QJsonObject root{
      {QString::fromUtf8("version"), 2},
      {QString::fromUtf8("levelId"), data.levelId},
      {QString::fromUtf8("player"), playerObject},
      {QString::fromUtf8("remainingCoins"), coinArray},
      {QString::fromUtf8("enemies"), enemyArray},
      {QString::fromUtf8("levelComplete"), data.snapshot.levelComplete}};

  const QString path = savePath();
  QDir().mkpath(QFileInfo(path).absolutePath());
  QSaveFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    return false;
  }
  file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
  return file.commit();
}

std::optional<SaveGameData> SaveGameRepository::load() const
{
  QFile file(savePath());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return std::nullopt;
  }

  QJsonParseError error;
  const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
  if (error.error != QJsonParseError::NoError || !document.isObject())
  {
    return std::nullopt;
  }

  const QJsonObject root = document.object();
  const int version = root.value(QString::fromUtf8("version")).toInt();
  if (version != 1 && version != 2)
  {
    return std::nullopt;
  }

  SaveGameData data;
  data.levelId = root.value(QString::fromUtf8("levelId")).toString();
  const QJsonObject player = root.value(QString::fromUtf8("player")).toObject();
  data.snapshot.playerPixelX = static_cast<float>(player.value(QString::fromUtf8("pixelX")).toDouble());
  data.snapshot.playerPixelY = static_cast<float>(player.value(QString::fromUtf8("pixelY")).toDouble());
  if (!std::isfinite(data.snapshot.playerPixelX) ||
      !std::isfinite(data.snapshot.playerPixelY))
  {
    return std::nullopt;
  }
  if (version == 1)
  {
    // Version 1 stored a 24x24 collision cell. Preserve its center and feet
    // when migrating to the 16x32 player body.
    data.snapshot.playerPixelX += 4.0f;
    data.snapshot.playerPixelY -= 8.0f;
  }
  data.snapshot.playerVelocityY =
      static_cast<float>(player.value(QString::fromUtf8("velocityY")).toDouble(0.0));
  data.snapshot.playerDirection =
      directionFromString(player.value(QString::fromUtf8("direction")).toString());
  data.snapshot.remainingAirJumps =
      std::clamp(player.value(QString::fromUtf8("remainingAirJumps")).toInt(1), 0, 1);
  data.snapshot.playerHealth = player.value(QString::fromUtf8("health")).toInt(5);
  data.snapshot.playerMaxHealth = player.value(QString::fromUtf8("maxHealth")).toInt(5);
  data.snapshot.coins = player.value(QString::fromUtf8("coins")).toInt();
  data.snapshot.score = player.value(QString::fromUtf8("score")).toInt();
  data.snapshot.levelComplete = root.value(QString::fromUtf8("levelComplete")).toBool(false);

  for (const QJsonValue& value : root.value(QString::fromUtf8("remainingCoins")).toArray())
  {
    data.snapshot.remainingCoins.push_back(positionFromJson(value.toObject()));
  }
  for (const QJsonValue& value : root.value(QString::fromUtf8("enemies")).toArray())
  {
    const QJsonObject object = value.toObject();
    data.snapshot.enemies.push_back(Engine::Snapshot::EnemyState{
        positionFromJson(object),
        static_cast<float>(object.value(QString::fromUtf8("life")).toDouble(100.0)),
        static_cast<float>(object.value(QString::fromUtf8("maxLife")).toDouble(100.0))});
  }

  if (data.levelId.isEmpty())
  {
    return std::nullopt;
  }
  return data;
}
