# Resultados de la comparación

Generado por `tools/compare_suites.py`. Cobertura medida sobre `src/core`.

| Métrica | Repositorio (TDD) | IA: prompt básico | IA: prompt estructurado |
| --- | --- | --- | --- |
| Casos ejecutados | 85 | 11 | 23 |
| Fallan con el código correcto | 0 | 0 | 1 |
| Aserciones | 165 | 11 | 40 |
| Líneas de código de prueba | 699 | 103 | 194 |
| Cobertura de líneas (dominio) | 100,0 % | 64,0 % | 96,8 % |
| Cobertura de ramas (dominio) | 100,0 % | 83,3 % | 100,0 % |
| Mutantes detectados | 16 de 16 | 5 de 16 | 14 de 16 |
| Puntaje de mutación | 100 % | 31 % | 88 % |

## Mutantes

| Mutante | Req. | Error introducido | Repositorio (TDD) | IA: prompt básico | IA: prompt estructurado |
| --- | --- | --- | --- | --- | --- |
| M1 | R1 | Rechaza la coordenada 3 (límite superior excluido) | detectado | detectado | detectado |
| M2 | R1 | Acepta la coordenada 0 | detectado | **sobrevive** | detectado |
| M3 | R1 | Acepta la coordenada 4 | detectado | **sobrevive** | detectado |
| M4 | R1 | Informa el eje equivocado al salir del eje X | detectado | **sobrevive** | detectado |
| M5 | R1 | Permite jugar sobre una casilla ocupada | detectado | detectado | detectado |
| M6 | R1 | El tablero nunca se considera lleno | detectado | **sobrevive** | detectado |
| M7 | R2 | El primer turno es de O | detectado | detectado | detectado |
| M8 | R2 | Los turnos no alternan | detectado | detectado | detectado |
| M9 | R2 | Una jugada inválida consume el turno | detectado | **sobrevive** | **sobrevive** |
| M10 | R3 | No reconoce la diagonal secundaria | detectado | detectado | detectado |
| M11 | R3 | No reconoce la tercera columna | detectado | **sobrevive** | detectado |
| M12 | R3 | No reconoce la fila inferior | detectado | **sobrevive** | detectado |
| M13 | R3 | Un tablero lleno es empate aunque haya línea | detectado | **sobrevive** | **sobrevive** |
| M14 | R3 | Se puede seguir jugando después de ganar | detectado | **sobrevive** | detectado |
| M15 | R3 | winner() informa un ganador cuando nadie ganó | detectado | **sobrevive** | detectado |
| M16 | R3 | reset() no reinicia el estado de la partida | detectado | **sobrevive** | detectado |

## Pruebas que fallan sin mutantes

- IA: prompt estructurado: `tst_ia_estructurado::winOnLastMoveIsNotDraw()`
