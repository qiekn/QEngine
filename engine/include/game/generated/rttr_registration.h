#include "game/ball.h"
#include "game/brick.h"
#include "game/brick_manager.h"
#include "game/camera2d.h"
#include "game/collider.h"
#include "game/cube.h"
#include "game/game.h"
#include "game/paddle.h"
#include "game/position.h"
#include "game/scale.h"
#include "game/score.h"
#include "game/speed.h"
#include "game/sprite.h"
#include "game/tag.h"
#include "game/velocity.h"
#include "raylib.h"
#include "rttr/registration.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<VariantCreateInfo>("VariantCreateInfo")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("entity_id", &VariantCreateInfo::entity_id);
    
    rttr::registration::class_<VariantBase>("VariantBase")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("entity_id", &VariantBase::entity_id)(rttr::metadata("NO_SERIALIZE", true));

    rttr::registration::class_<Vector2>("Vector2")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector2::x)
        .property("y", &Vector2::y)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Vector3>("Vector3")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector3::x)
        .property("y", &Vector3::y)
        .property("z", &Vector3::z)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Rectangle>("Rectangle")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Rectangle::x)
        .property("y", &Rectangle::y)
        .property("width", &Rectangle::width)
        .property("height", &Rectangle::height)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Color>("Color")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("r", &Color::r)
        .property("g", &Color::g)
        .property("b", &Color::b)
        .property("a", &Color::a)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Camera2D>("Camera2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("offset", &Camera2D::offset)
        .property("target", &Camera2D::target)
        .property("rotation", &Camera2D::rotation)
        .property("zoom", &Camera2D::zoom)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Texture2D>("Texture2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("id", &Texture2D::id)
        .property("width", &Texture2D::width)
        .property("height", &Texture2D::height)
        .property("mipmaps", &Texture2D::mipmaps)
        .property("format", &Texture2D::format)
        (rttr::metadata("NO_VARIANT", true));

    rttr::registration::class_<Collider>("Collider")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("m_collider_type", &Collider::m_collider_type)
        .property("m_is_trigger", &Collider::m_is_trigger)
        .property("m_width", &Collider::m_width)
        .property("m_height", &Collider::m_height)
        .property("m_radius", &Collider::m_radius)
        .property("m_static", &Collider::m_static)
        .property("m_draw_debug", &Collider::m_draw_debug);

    rttr::registration::class_<Scale>("Scale")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("x", &Scale::x)
        .property("y", &Scale::y);

    rttr::registration::class_<Speed>("Speed")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("value", &Speed::value);

    rttr::registration::class_<BrickManager>("BrickManager")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("rows", &BrickManager::rows)
        .property("columns", &BrickManager::columns)
        .property("brick_width", &BrickManager::brick_width)
        .property("brick_height", &BrickManager::brick_height)
        .property("padding_x", &BrickManager::padding_x)
        .property("padding_y", &BrickManager::padding_y)
        .property("start_x", &BrickManager::start_x)
        .property("start_y", &BrickManager::start_y);

    rttr::registration::class_<Tag>("Tag")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("value", &Tag::value);

    rttr::registration::class_<Score>("Score")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("value", &Score::value)
        .property("point_base", &Score::point_base)
        .property("font_size", &Score::font_size)
        .property("x", &Score::x)
        .property("y", &Score::y);

    rttr::registration::class_<Ball>("Ball")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object);

    rttr::registration::class_<Camera2DSystem>("Camera2DSystem")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("zoom", &Camera2DSystem::zoom)
        .property("enable_drag", &Camera2DSystem::enable_drag)
        .property("enable_zoom", &Camera2DSystem::enable_zoom)
        .property("zoom_increment", &Camera2DSystem::zoom_increment)
        .property("min_zoom", &Camera2DSystem::min_zoom)
        .property("max_zoom", &Camera2DSystem::max_zoom)
        .property("drag_speed", &Camera2DSystem::drag_speed)
        .property("m_target", &Camera2DSystem::m_target);

    rttr::registration::class_<Cube>("Cube")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("width", &Cube::width)
        .property("height", &Cube::height)
        .property("color", &Cube::color);

    rttr::registration::class_<Game>("Game")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object);

    rttr::registration::class_<Sprite>("Sprite")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("path_to_sprite", &Sprite::path_to_sprite)(rttr::metadata("SET_CALLBACK", "handle_new_path"))

        .method("handle_new_path", &Sprite::handle_new_path);

    rttr::registration::class_<Brick>("Brick")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object);

    rttr::registration::class_<Paddle>("Paddle")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("width", &Paddle::width)
        .property("height", &Paddle::height)
        .property("speed", &Paddle::speed);

    rttr::registration::class_<Position>("Position")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("x", &Position::x)
        .property("y", &Position::y);

    rttr::registration::class_<Velocity>("Velocity")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("x", &Velocity::x)
        .property("y", &Velocity::y);

}
