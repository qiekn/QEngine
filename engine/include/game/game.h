#pragma once

#include <functional>
#include <vector>
#include "game/brick.h"
#include "variant/variant_base.h"

using callback = std::function<void()>;

enum class GameState {
  None = 0,
  Idle,
  Running,
  End,
  Length,
  // START: IDLE -> RUNNING, RESTART: END->IDLE, END: RUNNING->END
};

class Game : public VariantBase {
  VARIANT(Game);

public:
  GameState get_game_state() const { return m_game_state; }

  void on_play_update() override;

  void start_game();
  void end_game();
  void on_break_destroyed(const Brick& brick);

  void register_on_game_start(callback cb);
  void register_on_game_end(callback cb);
  void register_on_brick_destoryed(std::function<void(const Brick& brick)> cb);

private:
  GameState m_game_state = GameState::Idle;

  std::vector<callback> m_game_start_callbacks;
  std::vector<callback> m_game_end_callbacks;
  std::vector<std::function<void(const Brick& brick)>> m_on_brick_destroyed;
};
