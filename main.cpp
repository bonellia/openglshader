#include "helper.h"

// Shaders
GLuint idProgramShader;
GLuint idVertexShader;
GLuint idFragmentShader;
GLuint idJpegTexture;
GLuint idHeightTexture;

int widthWindow = 1000;
int heightWindow = 1000;
constexpr auto PI = 3.141592653589793238462643383279502884L;

static constexpr auto DEFAULT_HEIGHT_FACTOR = 10.f;
static constexpr auto HEIGHT_FACTOR_UNIT = 0.5f;
static constexpr auto DEFAULT_TEXTURE_OFFSET = 0.0f;
static constexpr auto PAN_UNIT = 1.0f;
static constexpr auto DEFAULT_PITCH = 0.0f;
static constexpr auto PITCH_UNIT = 0.05f;
static constexpr auto PITCH_MAX = 90.0f;
static constexpr auto PITCH_MIN = -90.0f;
static constexpr auto DEFAULT_YAW = 90.0f;
static constexpr auto YAW_UNIT = 0.05f;
static constexpr auto YAW_MAX = 360.0f;
static constexpr auto YAW_MIN = 0.0f;
static constexpr auto DEFAULT_SPEED = 0.0f;
static constexpr auto SPEED_UNIT = 0.01f;
static constexpr auto LIGHT_UNIT = 5.0f;

// OpenGL parameters
int TextureWidth;
int TextureHeight;
GLint CameraPositionLocation;
GLint LightPositionLocation;
GLint MVPMatrixLocation;
GLint HeightFactorLocation;
GLint TextureHorizontalShiftLocation;

// Window parameters, cam related
double HeightFactor = DEFAULT_HEIGHT_FACTOR;
double TextureHorizontalShift = DEFAULT_TEXTURE_OFFSET;
float Pitch = DEFAULT_PITCH;
float Yaw = DEFAULT_YAW;
float Speed = DEFAULT_SPEED;

struct CameraType {
    glm::vec3 Position;
    glm::vec3 Gaze{0.0f, 0.0f, 1.0f};
    glm::vec3 Up{0.0f, 1.0f, 0.0f};
} Camera;

// Window parameters, size related
struct {
    struct {
        int X, Y;
    } Position;

    struct {
        int Width, Height;
    } Size;

    bool IsFullScreen = false;
    bool FullScreenKeyDown = false;
} Window;

CameraType DEFAULT_CAMERA;

struct {
    glm::mat4 Model{1.0f};
    glm::mat4 View;
    glm::mat4 Projection = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
    glm::mat4 Composite;
} Matrices;

glm::vec3 LightPosition;

void Initialize(const int &TextureWidth_, const int &TextureHeight_)
{
    // Initialize OpenGL variables
    TextureWidth = TextureWidth_;
    TextureHeight = TextureHeight_;
    CameraPositionLocation =
        glGetUniformLocation(idProgramShader, "CameraPosition");
    LightPositionLocation =
        glGetUniformLocation(idProgramShader, "LightPosition");
    MVPMatrixLocation = glGetUniformLocation(idProgramShader, "MVPMatrix");
    HeightFactorLocation =
        glGetUniformLocation(idProgramShader, "HeightFactor");
    TextureHorizontalShiftLocation =
        glGetUniformLocation(idProgramShader, "TextureHorizontalShift");

    // Initialize world data
    Camera.Position = {(float)TextureWidth / 2.0f, (float)TextureWidth / 10.0f,
                       (float)-TextureWidth / 4.0f};

    LightPosition = {(float)TextureWidth / 2.0f, 100.0f,
                     (float)TextureHeight / 2.0f};

    // Save this incarnation of the camera so that it can be restored later
    DEFAULT_CAMERA = Camera;
}

void Update()
{
    // Cam
    auto YawInRadians = glm::radians(Yaw);
    auto PitchInRadians = glm::radians(Pitch);

    Camera.Gaze = glm::normalize(
        glm::vec3(std::cos(YawInRadians) * std::cos(PitchInRadians),
                  std::sin(PitchInRadians),
                  std::sin(YawInRadians) * std::cos(PitchInRadians)));

    Camera.Position += Speed * Camera.Gaze;

    glUniform3fv(CameraPositionLocation, 1, glm::value_ptr(Camera.Position));

    // MVPMatrix
    Matrices.View =
        glm::lookAt(Camera.Position, Camera.Position + Camera.Gaze, Camera.Up);

    Matrices.Composite = Matrices.Projection * Matrices.View * Matrices.Model;

    glUniformMatrix4fv(MVPMatrixLocation, 1, GL_FALSE,
                       glm::value_ptr(Matrices.Composite));

    // Height factor
    glUniform1f(HeightFactorLocation, HeightFactor);

    // Light position
    glUniform3fv(LightPositionLocation, 1, glm::value_ptr(LightPosition));

    // Trucking.
    glUniform1f(TextureHorizontalShiftLocation, TextureHorizontalShift);
}

void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

void FramebufferSizeCallback(GLFWwindow *win, int Width, int Height)
{
    glViewport(0, 0, Width, Height);
    Matrices.Projection =
        glm::perspective(45.0f, (float)Width / Height, 0.1f, 1000.0f);
}

void KeyCallback(GLFWwindow *win, int Key, int ScanCode, int Action, int Mods)
{

    switch (Key) {

    case GLFW_KEY_LEFT: {
        LightPosition.x += LIGHT_UNIT;
        break;
    }
    case GLFW_KEY_RIGHT: {
        LightPosition.x -= LIGHT_UNIT;
        break;
    }
    case GLFW_KEY_UP: {
        LightPosition.z += LIGHT_UNIT;
        break;
    }
    case GLFW_KEY_DOWN: {
        LightPosition.z -= LIGHT_UNIT;
        break;
    }
    case GLFW_KEY_W: {
        Pitch += PITCH_UNIT;
        if (Pitch > PITCH_MAX)
            Pitch = PITCH_MAX;
        break;
    }
    case GLFW_KEY_A: {
        Yaw -= YAW_UNIT;
        if (Yaw < YAW_MIN)
            Yaw += YAW_MAX;
        break;
    }
    case GLFW_KEY_S: {
        Pitch -= PITCH_UNIT;
        if (Pitch < PITCH_MIN)
            Pitch = PITCH_MIN;
        break;
    }
    case GLFW_KEY_D: {
        Yaw += YAW_UNIT;
        if (Yaw > YAW_MAX)
            Yaw -= YAW_MAX;
        break;
    }
    case GLFW_KEY_R: {
        HeightFactor += HEIGHT_FACTOR_UNIT;
        break;
    }
    case GLFW_KEY_F: {
        HeightFactor -= HEIGHT_FACTOR_UNIT;
        break;
    }
    case GLFW_KEY_T: {
        LightPosition.y += LIGHT_UNIT;
        break;
    }
    case GLFW_KEY_G: {
        LightPosition.y -= LIGHT_UNIT;
        break;
    }
    case GLFW_KEY_Y: {
        Speed += SPEED_UNIT;
        break;
    }
    case GLFW_KEY_H: {
        Speed -= SPEED_UNIT;
        break;
    }
    case GLFW_KEY_Q: {
        TextureHorizontalShift -= PAN_UNIT;
        break;
    }
    case GLFW_KEY_E: {
        TextureHorizontalShift += PAN_UNIT;
        break;
    }
    case GLFW_KEY_X: {
        Speed = DEFAULT_SPEED;
        break;
    }
    case GLFW_KEY_I: {
        Speed = DEFAULT_SPEED;
        Pitch = DEFAULT_PITCH;
        Yaw = DEFAULT_YAW;
        Camera = DEFAULT_CAMERA;
        break;
    }
    default:
        break;
    }

    auto KeyState = glfwGetKey(win, GLFW_KEY_P);
    if (KeyState == GLFW_PRESS && !Window.FullScreenKeyDown) {
        Window.FullScreenKeyDown = true;

        if (!Window.IsFullScreen) {
            glfwGetWindowPos(win, &Window.Position.X, &Window.Position.Y);
            glfwGetWindowSize(win, &Window.Size.Width, &Window.Size.Height);

            auto Monitor = glfwGetPrimaryMonitor();
            auto VideoMode = glfwGetVideoMode(Monitor);
            auto Width = VideoMode->width;
            auto Height = VideoMode->height;

            glfwSetWindowMonitor(win, Monitor, 0, 0, Width, Height,
                                 GLFW_DONT_CARE);
            glViewport(0, 0, Width, Height);

            Window.IsFullScreen = true;
        } else {
            auto Width = Window.Size.Width;
            auto Height = Window.Size.Height;
            auto XOffset = Window.Position.X;
            auto YOffset = Window.Position.Y;
            glfwSetWindowMonitor(win, nullptr, XOffset, YOffset, Width, Height,
                                 GLFW_DONT_CARE);
            glViewport(0, 0, Width, Height);

            Window.IsFullScreen = false;
        }

    } else if (KeyState == GLFW_RELEASE && Window.FullScreenKeyDown) {
        Window.FullScreenKeyDown = false;
    }

    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, true);
    }
}

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TextureCoordinates;
};

std::string VertexShaderPath = "shader.vert";
std::string FragmentShaderPath = "shader.frag";
std::string HeightMapPath;
std::string TexturePath;

std::vector<Vertex> Vertices;
std::vector<int> Indices;

GLFWwindow *win;
GLuint VertexArrayObject;
GLuint VertexBufferObject;
GLuint ElementBufferObject;

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Please provide height and texture image files!\n");
        exit(-1);
    }

    HeightMapPath = argv[1];
    TexturePath = argv[2];
    if (!glfwInit()) {
        glfwTerminate();
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    win = glfwCreateWindow(widthWindow, heightWindow, "CENG477", nullptr,
                           nullptr);

    if (!win) {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(win);

    glewExperimental = GL_TRUE;
    glewInit();
    initShaders(idProgramShader, VertexShaderPath, FragmentShaderPath);
    glUseProgram(idProgramShader);
    glEnable(GL_DEPTH_TEST);

    initTexture(argv[1], argv[2], &TextureWidth, &TextureHeight);

    auto TextureWidthLocation =
        glGetUniformLocation(idProgramShader, "TextureWidth");
    auto TextureHeightLocation =
        glGetUniformLocation(idProgramShader, "TextureHeight");
    auto HeightMapLocation = glGetUniformLocation(idProgramShader, "HeightMap");
    auto TextureLocation = glGetUniformLocation(idProgramShader, "Texture");

    glUniform1i(TextureWidthLocation, TextureWidth);
    glUniform1i(TextureHeightLocation, TextureHeight);
    glUniform1i(HeightMapLocation, 0);
    glUniform1i(TextureLocation, 1);
    for (auto z = 0; z < TextureHeight; ++z) {
        for (auto x = 0; x < TextureWidth; ++x) {
            Vertices.push_back(
                {.Position = glm::vec3((float)x, 0.0f, (float)z),
                 .TextureCoordinates = glm::vec2((float)-x / TextureWidth,
                                                 (float)-z / TextureHeight)});
        }
    }
    for (auto i = 0; i < TextureHeight - 1; ++i) {
        for (auto j = 0; j < TextureWidth - 1; ++j) {
            auto Current = i * TextureWidth + j;
            auto Right = Current + 1;
            auto Bottom = Current + TextureWidth;
            auto BottomRight = Bottom + 1;

            Indices.push_back(Current);
            Indices.push_back(Right);
            Indices.push_back(Bottom);
            Indices.push_back(Right);
            Indices.push_back(BottomRight);
            Indices.push_back(Bottom);
        }
    }
    glGenVertexArrays(1, &VertexArrayObject);
    glBindVertexArray(VertexArrayObject);
    glGenBuffers(1, &ElementBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
    glGenBuffers(1, &VertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex),
                 Vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size() * sizeof(int),
                 Indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void *>(0));
    glVertexAttribPointer(
        1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void *>(offsetof(Vertex, TextureCoordinates)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glfwSetKeyCallback(win, KeyCallback);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(win, FramebufferSizeCallback);
    Initialize(TextureWidth, TextureHeight);

    // Program loop
    while (!glfwWindowShouldClose(win)) {
        constexpr auto BGColor = 0.0f;
        glClearColor(BGColor, BGColor, BGColor, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Update();
        glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
