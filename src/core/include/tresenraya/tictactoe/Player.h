#pragma once

namespace tresenraya::tictactoe {

/// Las dos piezas del juego. El valor subyacente es el símbolo que se muestra.
enum class Player : char {
    X = 'X',
    O = 'O',
};

[[nodiscard]] constexpr char symbolOf(Player player) noexcept
{
    return static_cast<char>(player);
}

[[nodiscard]] constexpr Player opponentOf(Player player) noexcept
{
    return player == Player::X ? Player::O : Player::X;
}

} // namespace tresenraya::tictactoe
