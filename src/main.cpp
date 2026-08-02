#include <LightEngine.hpp>
#include <gui/DrawContext.hpp>
#include <input/Keyboard.hpp>
#include <util/Clock.hpp>
#include <util/Logger.hpp>
#include <util/PathUtil.hpp>
#include <util/Time.hpp>
#include <util/math/MathUtils.hpp>

#include <glm/ext/vector_float2.hpp>

#include <iostream>
#include <random>

namespace bl = blaze::lightEngine;

class Box {
public:
    glm::vec2 pos = glm::vec2(0.0f, 0.0f);
    float w;
    float h;
    int argb;
    bool isCircle;

    Box(float x, float y, float w, float h, int argb, bool isCircle = false) {
        this->pos = glm::vec2(x, y);
        this->w = w;
        this->h = h;
        this->argb = argb;
        this->isCircle = isCircle;
    }

    glm::vec2 getPos() {
        return pos;
    }

    void move(float x, float y) {
        this->pos += glm::vec2(x, y);
    }

    void setPos(float x, float y) {
        this->pos = glm::vec2(x, y);
    }

    float getX() {
        return this->pos.x;
    }

    float getY() {
        return this->pos.y;
    }

    void setX(float x) {
        this->pos.x = x;
    }

    void setY(float y) {
        this->pos.y = y;
    }

    void draw(bl::DrawContext* context) {
        if (this->isCircle) context->drawCircle(this->pos.x, this->pos.y, this->w, 200, this->argb, true);
        else context->drawQuad(this->pos.x - (this->w / 2), this->pos.y - (this->h / 2), this->w, this->h, this->argb);
    }
};

static bool isPlaying = false;
static float ballSpeed = 10;
static int ballRadius = 5;
static float ballAngle = 0.0f;
static Box* ball;
static Box* pad1;
static Box* pad2;
static std::string pauseMsg = "Press Space to start";
static float paddleSpeed = 40;
static float rightPaddleSpeed;
static bool showDebug;
static bl::Clock* AITimer;
static bl::Time AITime = bl::Time::seconds(0.1);

int randInt() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distrib(0, 1000);
    return distrib(gen);
}

void init(bl::Window& window) {
    int x = 200;
    int w = window.getWidth();
    int h = window.getHeight();
    float y = (float) h / 2 - 75;

    ball = new Box((float) w / 2 - 15, (float) h / 2 - 15, 10, 10, 0xFF4CAF50, true);
    pad1 = new Box(x, y, 30, 150, 0xFFFF6347);
    pad2 = new Box(w - x - 30, y, 30, 150, 0xFF009688);
    ballSpeed = 10;

    AITimer = new bl::Clock();

    do {
        ballAngle = (float) (randInt() * 2 * PI / 360);
    } while (std::abs(std::cos(ballAngle)) < 0.7f);
}


void update(bl::Window& window, float deltaTime) {
    float factor = ballSpeed * deltaTime;

    // Move the player's paddle
    if (bl::Keyboard::isPressed(bl::Keyboard::LeftShift) && (pad1->getY() - 150.f / 2 > 5.f)) {
        pad1->move(0.f, -paddleSpeed * deltaTime);
    }

    if (bl::Keyboard::isPressed(bl::Keyboard::LeftAlt) && (pad1->getY() + 150.f / 2 < window.getHeight() - 5.f)) {
        pad1->move(0.f, paddleSpeed * deltaTime);
    }

    // Move the computer's paddle
    if (((rightPaddleSpeed < 0.f) && (pad2->getY() - 150.f / 2 > 5.f)) || ((rightPaddleSpeed > 0.f) && (pad2->getY() + 150.f / 2 < window.getHeight() - 5.f))) {
        pad2->move(0.f, rightPaddleSpeed * deltaTime);
    }

    // Update the computer's paddle direction according to the ball position
    if (AITimer->getElapsedTime().asMicroseconds() > AITime.asMicroseconds()) {
        AITimer->restart();
        if (ball->getY() + ballRadius > pad2->getY() + 150.f / 2) rightPaddleSpeed = paddleSpeed * ballSpeed / 10;
        else if (ball->getY() - ballRadius < pad2->getY() - 150.f / 2) rightPaddleSpeed = -paddleSpeed * ballSpeed / 10;
        else rightPaddleSpeed = 0.f;
    }

    ball->move((float) (std::cos(ballAngle) * factor), (float) (std::sin(ballAngle) * factor));

    // Check collisions between the ball and the screen
    if (ball->getX() - ballRadius < 0.f) {
        isPlaying = false;
        pauseMsg = "You lost!\nPress space to restart or\nescape to exit";
    }

    if (ball->getX() + ballRadius > window.getWidth()) {
        isPlaying = false;
        pauseMsg = "You won!\nPress space to restart or\nescape to exit";
    }

    if (ball->getY() - ballRadius < 0.f) {
        // ballSound.play();
        ballSpeed *= 1.05;
        ballAngle = -ballAngle;
        ball->setPos(ball->getX(), ballRadius + 0.1f);
    }

    if (ball->getY() + ballRadius > window.getHeight()) {
        // ballSound.play();
        ballSpeed *= 1.05;
        ballAngle = -ballAngle;
        ball->setPos(ball->getX(), window.getHeight() - ballRadius - 0.1f);
    }

    // Check the collisions between the ball and the paddles
    // Left Paddle
    if (ball->getX() - ballRadius < pad1->getX() + (float) 30.f / 2 && ball->getX() - ballRadius > pad1->getX() && ball->getY() + ballRadius >= pad1->getY() - (float) 150.f / 2 && ball->getY() - ballRadius <= pad1->getY() + (float) 150.f / 2) {
        if (ball->getY() > pad1->getY()) ballAngle = (float) (PI - ballAngle + (randInt() % 20) * PI / 180);
        else ballAngle = (float) (PI - ballAngle - (randInt() % 20) * PI / 180);

        // ballSound.play();
        ballSpeed *= 1.05;
        ball->setPos(pad1->getX() + ballRadius + (float) 30.f / 2 + 0.1f, ball->getY());
    }

    // Right Paddle
    if (ball->getX() + ballRadius > pad2->getX() - (float) 30.f / 2 && ball->getX() + ballRadius < pad2->getX() && ball->getY() + ballRadius >= pad2->getY() - (float) 150.f / 2 && ball->getY() - ballRadius <= pad2->getY() + (float) 150.f / 2) {
        if (ball->getY() > pad2->getY()) ballAngle = (float) (PI - ballAngle + (randInt() % 20) * PI / 180);
        else ballAngle = (float) (PI - ballAngle - (randInt() % 20) * PI / 180);

        // ballSound.play();
        ballSpeed *= 1.05;
        ball->setPos(pad2->getX() - ballRadius - (float) 30.f / 2 - 0.1f, ball->getY());
    }
}

void render(bl::DrawContext& context, int mouseX, int mouseY, float delta) {
    bl::Window* window = context.getWindow();
    float w = window->getWidth();
    float h = window->getHeight();
    int x = 200;

    pad2->setX(w - x - 30);

    if (isPlaying) {
        ball->draw(&context);
        pad1->draw(&context);
        pad2->draw(&context);

        if (showDebug) {
            float qx = (float) (std::cos(ballAngle) * 200) + ball->getX();
            float qy = (float) (std::sin(ballAngle) * 200) + ball->getY();

            context.drawHorizontalLine(0, w, pad2->getY() - (pad2->h / 2), 0xFFFFFF00);
            context.drawHorizontalLine(0, w, pad2->getY() - (pad2->h / 2) + pad2->h, 0xFFFF00FF);

            context.drawLine(ball->getX(), qx, ball->getY(), qy, 0xFF7492FF);
        }

        update(*window, delta);
    } else {
        context.drawCenteredTextWithShadow((float) w / 2, (float) h / 2 - context.getTextHeight(pauseMsg) / 2, pauseMsg, 0xFFFFFFFF);
    }

    if (isPlaying && bl::Keyboard::isPressed(bl::Keyboard::LeftCtrl)) {
        showDebug = !showDebug;
    }

    if (!isPlaying && bl::Keyboard::isPressed(bl::Keyboard::LeftCtrl)) {
        init(*window);
        isPlaying = true;
    }

    if (bl::Keyboard::isPressed(bl::Keyboard::Esc)) {
        bl::LightEngine::getInstance().scheduleStop();
    }
}

int main() {
    bl::RunArgs runArgs(
        bl::WindowSettings{1200, 600, 1366, 768, false},
        bl::DirectorySet{"../", "../resources", "../resources", ""}
    );

    bl::Logger::getInstance().setOutputFile(bl::PathUtil::join(runArgs.directories.runDir, "latest.log"));
    bl::LightEngine engine(std::move(runArgs));
    engine.initialize();

    bl::Window window = engine.getWindow();

    init(window);

    engine.addRenderCallback([](bl::DrawContext& ctx, int x, int y, float d) {
        render(ctx, x, y, d * 100);
    });

    engine.run();

    if (!engine.isRunning()) {
        std::cout << "engine shutdown ok" << std::endl;
        return 0;
    }

    return 1;
}

