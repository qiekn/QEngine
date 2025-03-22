#include "game/collision.h"
#include "game/framework/camera_system.h"
#include "game/player.h"
#include "game/position.h"
#include "game/scale.h"
#include "game/speed.h"
#include "game/sprite.h"
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
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Vector3>("Vector3")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Vector3::x)
        .property("y", &Vector3::y)
        .property("z", &Vector3::z)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Rectangle>("Rectangle")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("x", &Rectangle::x)
        .property("y", &Rectangle::y)
        .property("width", &Rectangle::width)
        .property("height", &Rectangle::height)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Color>("Color")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("r", &Color::r)
        .property("g", &Color::g)
        .property("b", &Color::b)
        .property("a", &Color::a)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Camera2D>("Camera2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("offset", &Camera2D::offset)
        .property("target", &Camera2D::target)
        .property("rotation", &Camera2D::rotation)
        .property("zoom", &Camera2D::zoom)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Texture2D>("Texture2D")
        .constructor<>()(rttr::policy::ctor::as_object)
        .property("id", &Texture2D::id)
        .property("width", &Texture2D::width)
        .property("height", &Texture2D::height)
        .property("mipmaps", &Texture2D::mipmaps)
        .property("format", &Texture2D::format)
        (rttr::metadata("RAYLIB", true));

    rttr::registration::class_<Sprite>("Sprite")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("path_to_sprite", &Sprite::path_to_sprite)(rttr::metadata("SET_CALLBACK", "on_path_to_sprite_set"))

        .method("on_path_to_sprite_set", &Sprite::on_path_to_sprite_set);

    rttr::registration::class_<Speed>("Speed")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("value", &Speed::value);

    rttr::registration::class_<Scale>("Scale")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("x", &Scale::x)
        .property("y", &Scale::y);

    rttr::registration::class_<Player>("Player")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("name", &Player::name);

    rttr::registration::class_<Collision>("Collision")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object);

    rttr::registration::class_<Position>("Position")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("x", &Position::x)
        .property("y", &Position::y);

    rttr::registration::class_<Camera2DSystem>("Camera2DSystem")
        .constructor<>()(rttr::policy::ctor::as_object)
        .constructor<VariantCreateInfo>()(rttr::policy::ctor::as_object)
        .property("zoom", &Camera2DSystem::zoom)
        .property("enable_drag", &Camera2DSystem::enable_drag)
        .property("enable_zoom", &Camera2DSystem::enable_zoom)
        .property("zoom_increment", &Camera2DSystem::zoom_increment)
        .property("min_zoom", &Camera2DSystem::min_zoom)
        .property("max_zoom", &Camera2DSystem::max_zoom)
        .property("drag_speed", &Camera2DSystem::drag_speed);

}
