@echo off

REM ↓↓↓↓↓↓↓↓↓↓↓↓ Path to SFML ↓↓↓↓↓↓↓↓↓↓↓↓

SET sfmlPath="C:/Program Files/SFML-2.5.1"

REM ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑

SET includePath=%sfmlPath%/include
SET libPath=%sfmlPath%/lib

g++ ./src/implementation/*.cpp -o ./build/main.exe -O3 -O2 -O1 -DSFML_STATIC -I %includePath% -I "./src/headers" -L %libPath% -lsfml-graphics -lsfml-window -lsfml-system

echo Press any button..
pause > nul