![image](https://github.com/user-attachments/assets/39fd0f50-88a5-4f9e-9342-9ea6bf911c9e)


# What is Zeytin ?

**Zeytin** is a lightweight and modular game engine written in **C++**, built on top of [Raylib](https://github.com/raysan5/raylib). It features a fully integrated editor built using [Dear ImGui](https://github.com/ocornut/imgui), offering a streamlined and efficient development workflow.

---

## ✨ Features

-  Lightweight, modular core engine
-  Editor developed as a separate C++ application
-  Component-based design (similar to Unity)
-  Advanced runtime type information with automatic code generation
-  Cross-platform build system powered by [Premake](https://premake.github.io/)

---

## ⚙️ Getting Started

### Prerequisites

Make sure the following tools and libraries are installed on your system:

-  A C++ compiler with **C++17** support (e.g., GCC, Clang, MSVC)
-  [Premake](https://premake.github.io/) for project file generation
-  [Raylib](https://github.com/raysan5/raylib) (already bundled with Zeytin)
-  [ZeroMQ](https://zeromq.org/) for messaging support

---

## 🧭 A Tour of Zeytin

Upon launching the editor, you'll find an intuitive interface designed for productivity:

- **Hierarchy Panel (Left):**  
  Displays all entities and their components in the current scene.
  
- **Console (Bottom):**  
  Outputs real-time log messages from both the engine and the editor.
  
- **Asset Browser (Right):**  
  Lets you navigate and manage project files and folders.

![Zeytin Editor UI](https://github.com/user-attachments/assets/67f2b4a7-10bb-40cd-bc7b-3be628679f8d)

---

## 🧪 Running the Engine

- You can design and edit levels directly in the editor—no need to start the engine immediately.
- To start the engine:
  - Click the **"Start Engine"** button at the top of the editor.
  - This will automatically **build** and **run** the engine.

---

## 🚀 Your First Game: Adding a Moving Cube

In Zeytin, the engine and editor are separate executables, but the game code is part of the engine project. This means:

- The **game code is compiled with the engine code**.
- The **parser expects all game-related code** to be placed under a specific directory: 

###  Project Structure

```plaintext
zeytin/
├── editor/                 
│   ├── include/            
│   ├── source/             
│   └── build.sh            
├── engine/                 
│   ├── include/
│   │   └── game/           ← Game header files
│   ├── source/
│   │   └── game/           ← Game implementation files
│   └── build.sh            
└── shared_resources/       
    ├── entities/           
    └── variants/           
```

## Creating a Variant

In the **Zeytin** ecosystem, components are referred to as **variants**. This naming convention originated from the internal implementation dependency on `rttr::variant`, and the term has persisted ever since.

### Step 1: Define a Simple `Position` Variant


```plaintext
zeytin/
├── editor/                 
│   ├── include/            
│   ├── source/             
│   └── build.sh            
├── engine/                 
│   ├── include/
│   │   └── game/
            └── position.h              ← Add here   
│   ├── source/
│   │   └── game/          
│   └── build.sh            
└── shared_resources/       
    ├── entities/           
    └── variants/           
```


#### Key Concepts

- **Variants** are a core abstraction in Zeytin used to define game object components.
- **Macros** like `VARIANT()` and `PROPERTY()` are essential for reflection and parsing:
  - `VARIANT(TypeName)`:
    - Generates boilerplate code necessary for the engine to recognize and manage the variant.
  - `PROPERTY()`:
    - An empty macro used by the editor/parser to identify fields that should be treated as editable properties.

#### Example: `position.h`

```cpp
#pragma once

#include "variant/variant_base.h"

class Position : public VariantBase {
    VARIANT(Position)  // Required macro for engine-side registration

public:
    Position(float x, float y) : x(x), y(y) {}

    // Public member variables with PROPERTY() macro
    float x = 0; PROPERTY()  // Marks this field as an editable property
    float y = 0; PROPERTY()
};
```

#### Example: `speed.h`

Now let's define a speed variant, under ```game/include```:

```cpp
#pragma once

#include "variant/variant_base.h"

class Speed : public VariantBase { 
    VARIANT(Speed);

public:
    float value; PROPERTY();
};

```


#### Example: `cube.h`

Finally, let's a cube variant where we will also have the logic, under ```game/include```:

```cpp
#pragma once

#include "variant/variant_base.h"
#include "game/position.h"

class Cube : public VariantBase {
    VARIANT(Cube);

public:
    float width = 50.0f; PROPERTY()
    float height = 50.0f; PROPERTY()
    Color color = RED; PROPERTY() // Nesting is also OK, Color is a Raylib struct.
    
    void on_init() override; // Lifetime method, called right after construction
    void on_update() override; // Lifetime method, called every frame
    void on_play_update() override; // Lifetime method, called every frame while in play mode

    /*  Not implemented ones:
        void on_post_init() override; // Called after on_init()
        void on_play_start() override; // Called on entering play mode
        void on_play_late_start() override // Called after on_play_start()
    */
    
private:
    void handle_input();
};
```

Now it's time to code some logic, let's add cube.cpp under ```source/game/```

```cpp
#include "game/cube.h"
#include "game/speed.h"
#include "core/query.h"
#include "core/raylib_wrapper.h"

#include "logger.h"

void Cube::on_init() {
    // Initialization if needed
    log_info() << "Cube is initialized" << std::endl;
}

void Cube::on_update() {
    // Get the position component
    auto& position = Query::get<Position>(this);
    
    // Draw the cube
    draw_rectangle(
        position.x - width / 2,
        position.y - height / 2,
        width,
        height,
        color);
}

void Cube::on_play_update() {
    // Handle user input during play mode
    handle_input();
}

void Cube::handle_input() {
    auto& position = Query::get<Position>(this);
    // Using Query::read for Speed as it's read-only
    const auto& speed = Query::read<Speed>(this);
    
    float delta_time = get_frame_time();
    float movement_speed = speed.value;
    
    // Handle keyboard input for movement
    if (is_key_down(KEY_LEFT) || is_key_down(KEY_A)) {
        position.x -= movement_speed * delta_time;
    }

    if (is_key_down(KEY_RIGHT) || is_key_down(KEY_D)) {
        position.x += movement_speed * delta_time;
    }

    if (is_key_down(KEY_UP) || is_key_down(KEY_W)) {
        position.y -= movement_speed * delta_time;
    }

    if (is_key_down(KEY_DOWN) || is_key_down(KEY_S)) {
        position.y += movement_speed * delta_time;
    }
}
```








