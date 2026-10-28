# Resultados de las pruebas generadas por IA

Cada respuesta de la IA se guardó tal cual en [`respuestas/`](respuestas) y se compiló sin
modificaciones dentro del proyecto, con las mismas advertencias estrictas (`-Werror`) que el
resto del código. Las métricas completas están en
[`docs/resultados/resumen.md`](../docs/resultados/resumen.md) y se regeneran con:

```bash
python3 tools/compare_suites.py   # cobertura + pruebas de mutación de las tres suites
ctest --preset ia                 # ejecuta sólo las suites generadas por IA
```

## Prompt 1: básico

**Qué devolvió:** 11 pruebas, una por cada prueba de la kata, al estilo *given/when/then*.
Como no conocía el código, **inventó su propia API**: `play(x, y)` devuelve un texto
(`"X es el ganador"`) y `nextPlayer()` devuelve un `char`.

**Al compilar:** no se puede compilar directamente contra el dominio real. Para ejecutarla
sin tocarla se escribió un **adaptador** de unas 30 líneas
([`tests/ia/basico/TicTacToe.h`](../tests/ia/basico/TicTacToe.h)) que implementa la API
supuesta delegando en el dominio. Con el adaptador compila sin advertencias y **las 11
pruebas pasan**.

**Debilidades encontradas:**
- Verifica las excepciones de forma genérica (`std::runtime_error`), así que no distingue
  "fuera del eje X" de "fuera del eje Y" ni de "casilla ocupada".
- Usa un único valor inválido (`5`), lejos del límite. No detecta si se acepta el `0` o
  el `4`.
- Prueba una sola fila, una sola columna y las dos diagonales con secuencias fijas. Si el
  juego no reconociera la tercera columna o la fila inferior, las pruebas seguirían pasando.
- No cubre el empate, la victoria en la última jugada, jugar después de terminar ni el
  reinicio.
- Usa `new`/`delete` manual en `init()`/`cleanup()` en lugar de un objeto por valor.
- Resultado: **64 % de cobertura de líneas** y sólo **5 de 16 mutantes** detectados.

## Prompt 2: estructurado

**Qué devolvió:** 23 casos con pruebas guiadas por datos, verificación del tipo concreto de
excepción y de sus datos (eje, valor, ocupante), y casos límite pedidos explícitamente.

**Al compilar:** compila a la primera, sin advertencias y contra la API real.

**Al ejecutar:** **1 prueba falla**: `winOnLastMoveIsNotDraw`. El error está en la prueba, no
en el juego:
- El comentario dibuja un tablero en el que X completa la fila 3 en la novena jugada.
- La secuencia de jugadas no corresponde a ese dibujo: con ella, **O completa la columna 2
  en la octava jugada**, así que la novena lanza `GameOverException`.
- Como la excepción no se captura dentro de la prueba, Qt Test **aborta todo el ejecutable**
  (código de salida 134). En CI el resultado se ve como un fallo de toda la suite, no de
  una sola prueba.

**Debilidades encontradas:**
- La prueba defectuosa era justamente la que debía detectar "un tablero lleno es empate
  aunque haya línea". Por eso ese error (mutante M13) **sobrevive**.
- `invalidMoveDoesNotChangeTurn` hace dos jugadas inválidas seguidas. Si cada jugada
  inválida consumiera un turno, los dos cambios se anularían y la prueba seguiría pasando
  (mutante M9 **sobrevive**).
- No prueba la clase `Board` por separado, ni valores extremos de `int`, ni el reinicio
  después de una victoria, ni la capa de presentación.
- Resultado: **96,8 % de cobertura de líneas**, pero **14 de 16 mutantes**. Una cobertura
  alta no garantiza pruebas correctas.

## Conclusión

| | Repositorio (TDD) | IA: prompt básico | IA: prompt estructurado |
| --- | --- | --- | --- |
| Compila contra el código real | Sí | No (requiere adaptador) | Sí |
| Pruebas que fallan sin errores en el código | 0 | 0 | 1 (prueba defectuosa) |
| Cobertura de líneas del dominio | 100 % | 64 % | 96,8 % |
| Mutantes detectados | 16 / 16 | 5 / 16 | 14 / 16 |

Darle a la IA contexto (la API real) y reglas de calidad mejora mucho el resultado (de 31 %
a 88 % de puntaje de mutación). Aun así, sus pruebas necesitan **revisión humana**: una de
ellas era incorrecta y otra estaba diseñada de forma que no podía detectar el error que
buscaba.
