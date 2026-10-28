# Prompts para generar las pruebas unitarias con IA

Se usaron dos prompts para comparar cómo cambia la calidad de las pruebas generadas según
la información que recibe la IA. Ambos se pueden pegar tal cual en ChatGPT, Claude u otro
asistente.

| Prompt | Estrategia | Respuesta | Código compilado |
| --- | --- | --- | --- |
| 1. Básico | Sólo los requerimientos de la kata, como lo pediría un estudiante | [`respuestas/prompt-1-basico.md`](respuestas/prompt-1-basico.md) | [`tests/ia/basico/`](../tests/ia/basico) |
| 2. Estructurado | Rol, contexto técnico, API real del dominio y reglas de calidad | [`respuestas/prompt-2-estructurado.md`](respuestas/prompt-2-estructurado.md) | [`tests/ia/estructurado/`](../tests/ia/estructurado) |

La IA usada para generar las respuestas fue **Claude (Anthropic)**, en una sola respuesta
por prompt y sin correcciones posteriores. Lo que ocurrió al compilar y ejecutar cada
respuesta está en [`resultados.md`](resultados.md).

---

## Prompt 1 — Básico

```text
Genera las pruebas unitarias en C++ usando Qt Test para la siguiente kata de TDD de
Tres en Raya:

Requerimiento 1: Una pieza puede estar colocada en un espacio de un tablero 3x3
Pruebas:
- Cuando una pieza está fuera del eje X, entonces se debe lanzar una excepción
- Cuando una pieza está fuera del eje Y, entonces se debe lanzar una excepción
- Cuando una pieza esté en un lugar ya ocupado, se debe lanzar una excepción

Requerimiento 2: Debe existir una forma de determinar qué jugador juega después
Pruebas:
- El primer turno siempre debe ser jugado por 'X'
- Si el último turno fue jugado por 'X', entonces el siguiente turno es para 'O'
- Si el último turno fue jugado por 'O', entonces el siguiente turno es para 'X'

Requerimiento 3: Un jugador gana si es el primero en conectar una línea de un lado a
otro, o de una esquina a otra
Pruebas:
- Si no se cumple la condición de victoria, entonces no hay ganador
- Un jugador gana cuando toda una línea horizontal es ocupada por sus piezas
- Un jugador gana cuando toda una línea vertical es ocupada por sus piezas
- Un jugador gana cuando toda una línea diagonal es ocupada por sus piezas
```

---

## Prompt 2 — Estructurado (rol + contexto + API + reglas)

````text
Actúa como un ingeniero de software especializado en pruebas unitarias para C++.

## Contexto
- Proyecto en C++20 con Qt 6.10+ y CMake. Las pruebas usan Qt Test (#include <QtTest>).
- Se compila con -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror.
- El dominio YA está implementado y NO debes modificarlo. Esta es su API pública
  (namespace tresenraya::tictactoe):

```cpp
// tresenraya/tictactoe/Position.h
inline constexpr int kBoardSize = 3;
inline constexpr int kMinCoordinate = 1;
inline constexpr int kMaxCoordinate = kBoardSize;
struct Position { int x = 1; int y = 1; /* operator== por defecto */ }; // x: columna, y: fila
enum class Axis : char { X = 'X', Y = 'Y' };

// tresenraya/tictactoe/Player.h
enum class Player : char { X = 'X', O = 'O' };
constexpr char symbolOf(Player player) noexcept;
constexpr Player opponentOf(Player player) noexcept;

// tresenraya/tictactoe/GameExceptions.h  (todas heredan de std::runtime_error)
class InvalidMoveException : public std::runtime_error {};
class OutOfBoardException : public InvalidMoveException {
public: Axis axis() const noexcept; int value() const noexcept; };
class OccupiedCellException : public InvalidMoveException {
public: Position position() const noexcept; Player occupant() const noexcept; };
class GameOverException : public InvalidMoveException {};

// tresenraya/tictactoe/Board.h
class Board {
public:
    void place(Position position, Player player);          // lanza OutOfBound/Occupied
    std::optional<Player> pieceAt(Position position) const; // lanza OutOfBoardException
    bool isFull() const noexcept;
    bool isEmpty() const noexcept;
    void clear() noexcept;
};

// tresenraya/tictactoe/TicTacToe.h
enum class GameStatus { InProgress, Won, Draw };
using Line = std::array<Position, 3>;
class TicTacToe {
public:
    static constexpr Player kFirstPlayer = Player::X;
    GameStatus play(Position position); // lanza OutOfBoard, OccupiedCell o GameOver
    Player nextPlayer() const noexcept;
    std::optional<Player> lastPlayer() const noexcept;
    std::optional<Player> winner() const noexcept;
    std::optional<Line> winningLine() const noexcept;
    GameStatus status() const noexcept;
    bool isOver() const noexcept;
    const Board &board() const noexcept;
    void reset() noexcept;
};
```

## Requerimientos de la kata (cada prueba listada debe tener al menos un caso)
Requerimiento 1: Una pieza puede estar colocada en un espacio de un tablero 3x3
- Cuando una pieza está fuera del eje X, se debe lanzar una excepción
- Cuando una pieza está fuera del eje Y, se debe lanzar una excepción
- Cuando una pieza esté en un lugar ya ocupado, se debe lanzar una excepción
Requerimiento 2: Debe existir una forma de determinar qué jugador juega después
- El primer turno siempre debe ser jugado por 'X'
- Si el último turno fue jugado por 'X', el siguiente turno es para 'O'
- Si el último turno fue jugado por 'O', el siguiente turno es para 'X'
Requerimiento 3: Un jugador gana si es el primero en conectar una línea de un lado a
otro, o de una esquina a otra
- Si no se cumple la condición de victoria, no hay ganador
- Gana cuando toda una línea horizontal es ocupada por sus piezas
- Gana cuando toda una línea vertical es ocupada por sus piezas
- Gana cuando toda una línea diagonal es ocupada por sus piezas

## Reglas para las pruebas
1. Un único archivo `tst_ia_estructurado.cpp`: una clase QObject con slots privados y
   QTEST_APPLESS_MAIN, terminando con #include "tst_ia_estructurado.moc".
2. Cada prueba debe ser independiente: usa init() para empezar una partida nueva.
3. Verifica el TIPO concreto de la excepción y sus datos (eje, valor, ocupante), no sólo
   que "se lance algo".
4. Usa pruebas guiadas por datos (_data + QFETCH) para cubrir las 3 filas, las 3 columnas
   y las 2 diagonales.
5. Incluye casos límite: coordenadas 0 y 4, valores negativos, empate, victoria en la
   última jugada, jugar después de terminar la partida y que una jugada inválida no
   cambie el turno.
6. Nombres descriptivos y un comentario que cite el requerimiento sobre cada prueba.
7. Devuelve sólo el código, listo para compilar sin advertencias.
````
