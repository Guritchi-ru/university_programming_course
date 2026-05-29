#include "paint_core.hpp"
#include <algorithm>   // std::min, std::max, std::transform, std::fill
#include <cmath>       // std::abs
#include <stack>       // std::stack
#include <iostream>

extern std::stack<Canvas> undoStack;

// ========== Вспомогательная функция сохранения (уже готова) ==========
void saveToUndo(const Canvas& canvas) {
    undoStack.push(canvas);
}

// ========== 1. Очистка холста (оценка 3) ==========
void clearCanvas(Canvas& canvas) {
    for (auto& row : canvas){
        std::fill(row.begin(), row.end(), sf::Color::White);
    }
    // TODO: заполнить весь холст белым цветом sf::Color::White
    // Использовать std::fill для каждой строки (или двойной цикл)
    // Подсказка: for (auto& row : canvas) std::fill(row.begin(), row.end(), sf::Color::White);
}

// ========== 2. Рисование пикселя (оценка 3) ==========
void drawPixel(Canvas& canvas, int x, int y, const sf::Color& color) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
        canvas[y][x] = color;
    }
    // TODO: проверить, что x и y в пределах [0, WIDTH-1] и [0, HEIGHT-1]
    // Если да, то присвоить canvas[y][x] = color
}

// ========== 3. Круглая кисть (оценка 3) ==========
void drawBrush(Canvas& canvas, int x, int y, const sf::Color& color, int radius) {
    for (int dy = -radius; dy <= radius; ++dy){
        for (int dx = -radius; dx <= radius; ++dx){
            if (dx*dx + dy*dy <= radius*radius) {
                drawPixel(canvas, x + dx, y +dy, color);
            }
        }
    }
    // TODO: закрасить круг с центром (x,y) радиусом radius
    // Перебрать все dx, dy от -radius до +radius, проверить dx*dx+dy*dy <= radius*radius
    // Вызвать drawPixel для (x+dx, y+dy)
}

// ========== 4. Прямоугольник (оценка 3) ==========
void drawRectangle(Canvas& canvas, int x1, int y1, int x2, int y2, const sf::Color& color) {
    saveToUndo(canvas);
    int left = std::min(x1, x2);
    int right = std::max(x1, x2);
    int top = std::min(y1, y2);
    int bottom = std::max(y1, y2);
    
    for (int y = top; y <= bottom; ++y){
        for (int x = left; x <= right; ++x){
            drawPixel(canvas, x, y, color);
        }
    }// уже есть, не удалять
    // TODO: вычислить left = min(x1,x2), right = max(x1,x2), top = min(y1,y2), bottom = max(y1,y2)
    // Двойной цикл for (int y = top; y <= bottom; ++y) for (int x = left; x <= right; ++x)
    // Вызвать drawPixel(canvas, x, y, color)
}

// ========== 5. Линия (Брезенхем) (оценка 4) ==========
void drawLine(Canvas& canvas, int x1, int y1, int x2, int y2, const sf::Color& color) {
    saveToUndo(canvas);
    int x = x1;
    int y = y1;
    int dx = std::abs(x2-x1);
    int dy = std::abs(y2-y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        drawPixel(canvas, x, y, color);
        if (x == x2 && y == y2) break;
        
        int e2 = 2*err;
        if(e2 > -dy){
            err -= dy;
            x += sx;
        }
        if (e2 < dx){
            err += dx;
            y += sy;
        }
    }
    // TODO: реализовать целочисленный алгоритм Брезенхема
    // Использовать std::abs, переменные dx, dy, sx, sy, err
    // В цикле рисовать пиксель и изменять x1,y1 пока не дойдём до (x2,y2)
}

// ========== 6. Undo (оценка 4) ==========
void undo(Canvas& canvas) {
    if (!undoStack.empty()){
        canvas = undoStack.top();
        undoStack.pop();
    }
    // TODO: если undoStack не пуст, то взять верхний элемент и присвоить canvas, затем удалить его
    // иначе ничего не делать
}

// ========== 7. Оттенки серого (оценка 5) ==========
void applyGrayscale(Canvas& canvas) {
    saveToUndo(canvas);

    for (auto& row : canvas) {
        std::transform(row.begin(), row.end(), row.begin(), [](const sf::Color& c){
            int gray = (c.r + c.g + c.b) / 3;
            return sf::Color(gray, gray, gray); 
        });
    }
    // TODO: применить к каждой строке
    // int gray = (c.r + c.g + c.b) / 3; return sf::Color(gray, gray, gray);

}

// ========== 8. Негатив (оценка 5) ==========
void applyNegative(Canvas& canvas) {
    saveToUndo(canvas);

    for (auto& row : canvas) {
        std::transform(row.begin(), row.end(), row.begin(),[](const sf::Color& c){
            return sf::Color(255 - c.r, 255 - c.g, 255 - c.b);
        });
    }
    // TODO: аналогично, но для каждого компонента: 255 - c.r и т.д.
}

// ========== 9. Размытие (оценка 5) ==========
void applyBlur(Canvas& canvas) {
    saveToUndo(canvas);

    Canvas original = canvas;
    for (int y = 1; y < HEIGHT - 1; ++y){
        for (int x = 1; x < WIDTH -1; ++x){
            int r = 0, g = 0, b = 0;

            for (int dy = -1; dy <= 1; ++dy){
                for (int dx = -1; dx <= 1; ++dx){
                    r += original[y + dy][x + dx].r;
                    g += original[y + dy][x + dx].g;
                    b += original[y + dy][x + dx].b;
                }
            }
            canvas[y][x] = sf::Color(r/9, g/9, b/9);
        }
    }
    // TODO: создать копию Canvas original = canvas;
    // Для всех y от 1 до HEIGHT-2, x от 1 до WIDTH-2:
    //   просуммировать r,g,b всех 9 соседей из original, поделить на 9
    //   присвоить canvas[y][x] новый цвет
    // Границы не трогать (оставить как есть)
}

// ========== Вспомогательная отрисовка (уже готова) ==========
void renderToWindow(sf::RenderWindow& window, const Canvas& canvas) {
    static sf::Image image;
    static sf::Texture texture;
    static sf::Sprite sprite;
    
    image.create(WIDTH, HEIGHT);
    for (int y = 0; y < HEIGHT; ++y)
        for (int x = 0; x < WIDTH; ++x)
            image.setPixel(x, y, canvas[y][x]);
    
    texture.loadFromImage(image);
    texture.setSmooth(false);
    sprite.setTexture(texture);
    sprite.setScale(PIXEL_SIZE, PIXEL_SIZE);
    window.draw(sprite);
}

void pickColour(Canvas& canvas, int x, int y, sf::Color& color){
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT){
        color = canvas[y][x];
    }
}
