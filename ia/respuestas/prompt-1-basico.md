# Respuesta de la IA al prompt 1 (básico)

> Generada por Claude (Anthropic) en una sola respuesta, sin correcciones. Se copia tal
> cual para poder compararla.

---

¡Claro! A continuación tienes las pruebas unitarias para la kata de Tres en Raya usando
**Qt Test**. Como en TDD las pruebas se escriben antes que el código, asumo la siguiente
interfaz para la clase `TicTacToe` (puedes adaptar los nombres a tu implementación):

```cpp
// TicTacToe.h (interfaz esperada)
#pragma once
#include <QString>

class TicTacToe
{
public:
    // Coloca la pieza del jugador en turno en la columna x y la fila y (de 1 a 3).
    // Lanza std::runtime_error si la posición es inválida o está ocupada.
    // Devuelve "No hay ganador" o "<jugador> es el ganador".
    QString play(int x, int y);

    // Devuelve 'X' u 'O' según a quién le toca jugar.
    char nextPlayer() const;
};
```

## Pruebas: `tst_tictactoe.cpp`

```cpp
#include <QtTest>
#include <stdexcept>
#include "TicTacToe.h"

class TicTacToeTest : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // Requerimiento 1
    void whenXOutsideBoardThenException();
    void whenYOutsideBoardThenException();
    void whenOccupiedThenException();

    // Requerimiento 2
    void givenFirstTurnWhenNextPlayerThenX();
    void givenLastTurnWasXWhenNextPlayerThenO();
    void givenLastTurnWasOWhenNextPlayerThenX();

    // Requerimiento 3
    void whenPlayThenNoWinner();
    void whenPlayAndWholeHorizontalLineThenWinner();
    void whenPlayAndWholeVerticalLineThenWinner();
    void whenPlayAndTopBottomDiagonalLineThenWinner();
    void whenPlayAndBottomTopDiagonalLineThenWinner();

private:
    TicTacToe *ticTacToe = nullptr;
};

void TicTacToeTest::init()
{
    ticTacToe = new TicTacToe();
}

void TicTacToeTest::cleanup()
{
    delete ticTacToe;
}

// ---------- Requerimiento 1 ----------

void TicTacToeTest::whenXOutsideBoardThenException()
{
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, ticTacToe->play(5, 2));
}

void TicTacToeTest::whenYOutsideBoardThenException()
{
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, ticTacToe->play(2, 5));
}

void TicTacToeTest::whenOccupiedThenException()
{
    ticTacToe->play(2, 1);
    QVERIFY_THROWS_EXCEPTION(std::runtime_error, ticTacToe->play(2, 1));
}

// ---------- Requerimiento 2 ----------

void TicTacToeTest::givenFirstTurnWhenNextPlayerThenX()
{
    QCOMPARE(ticTacToe->nextPlayer(), 'X');
}

void TicTacToeTest::givenLastTurnWasXWhenNextPlayerThenO()
{
    ticTacToe->play(1, 1);
    QCOMPARE(ticTacToe->nextPlayer(), 'O');
}

void TicTacToeTest::givenLastTurnWasOWhenNextPlayerThenX()
{
    ticTacToe->play(1, 1);
    ticTacToe->play(2, 2);
    QCOMPARE(ticTacToe->nextPlayer(), 'X');
}

// ---------- Requerimiento 3 ----------

void TicTacToeTest::whenPlayThenNoWinner()
{
    QString actual = ticTacToe->play(1, 1);
    QCOMPARE(actual, QString("No hay ganador"));
}

void TicTacToeTest::whenPlayAndWholeHorizontalLineThenWinner()
{
    ticTacToe->play(1, 1); // X
    ticTacToe->play(1, 2); // O
    ticTacToe->play(2, 1); // X
    ticTacToe->play(2, 2); // O
    QString actual = ticTacToe->play(3, 1); // X
    QCOMPARE(actual, QString("X es el ganador"));
}

void TicTacToeTest::whenPlayAndWholeVerticalLineThenWinner()
{
    ticTacToe->play(2, 1); // X
    ticTacToe->play(1, 1); // O
    ticTacToe->play(3, 1); // X
    ticTacToe->play(1, 2); // O
    ticTacToe->play(2, 2); // X
    QString actual = ticTacToe->play(1, 3); // O
    QCOMPARE(actual, QString("O es el ganador"));
}

void TicTacToeTest::whenPlayAndTopBottomDiagonalLineThenWinner()
{
    ticTacToe->play(1, 1); // X
    ticTacToe->play(1, 2); // O
    ticTacToe->play(2, 2); // X
    ticTacToe->play(1, 3); // O
    QString actual = ticTacToe->play(3, 3); // X
    QCOMPARE(actual, QString("X es el ganador"));
}

void TicTacToeTest::whenPlayAndBottomTopDiagonalLineThenWinner()
{
    ticTacToe->play(1, 3); // X
    ticTacToe->play(1, 1); // O
    ticTacToe->play(2, 2); // X
    ticTacToe->play(1, 2); // O
    QString actual = ticTacToe->play(3, 1); // X
    QCOMPARE(actual, QString("X es el ganador"));
}

QTEST_APPLESS_MAIN(TicTacToeTest)
#include "tst_tictactoe.moc"
```

## Cómo compilarlas con CMake

```cmake
find_package(Qt6 REQUIRED COMPONENTS Core Test)
qt_standard_project_setup()

qt_add_executable(tst_tictactoe tst_tictactoe.cpp TicTacToe.cpp)
target_link_libraries(tst_tictactoe PRIVATE Qt6::Core Qt6::Test)

enable_testing()
add_test(NAME tst_tictactoe COMMAND tst_tictactoe)
```

## Notas

- Cada prueba corresponde a uno de los casos de la kata, con nombres al estilo
  *given/when/then*.
- `init()` y `cleanup()` crean una partida nueva para cada prueba, así no dependen entre sí.
- Siguiendo TDD, primero ejecuta las pruebas (fallarán) y luego implementa `TicTacToe`
  hasta que todas pasen.
