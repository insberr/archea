//! This example demonstrates the built-in 3d shapes in Bevy.
//! The scene includes a patterned texture and a rotation for visualizing the normals and UVs.

use std::arch::x86_64::_mm_abs_epi32;
use std::f32::consts::PI;
use rand::prelude::*;
use bevy_rapier3d::prelude::*;

use bevy::{
    prelude::*,
    render::{
        render_asset::RenderAssetUsages,
        render_resource::{Extent3d, TextureDimension, TextureFormat},
    },
};
use bevy::ecs::bundle::DynamicBundle;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};

// Why
mod engine;
use engine::systems::gravity_system::*;
use engine::systems::collision_system::*;
use engine::systems::sideways_movement::*;
use engine::systems::timestep_system::*;
use crate::engine::systems::update_pixel_color::update_pixel_color;
use crate::PixelType::Sand;

fn main() {
    App::new()
        .add_plugins(DefaultPlugins.set(ImagePlugin::default_nearest()))
        .add_plugins(PanOrbitCameraPlugin)

        .insert_resource(Time::<Fixed>::from_seconds(0.3)) // runs 60 times a second

        .add_systems(Startup, setup)

        // .add_systems(FixedUpdate, collision_system)
        // .add_systems(FixedUpdate, fix_y)
        .add_systems(FixedUpdate, sideways_movement)

        .add_systems(FixedPostUpdate, update_pixel_color)

        .run();
}

enum PixelType {
    Unmovable = -2,
    Invalid = -1,
    Sand,
    Water,
    Lava,
    Steam,
    Rock,
}

/// A marker component for our shapes so we can query them separately from the ground plane
#[derive(Component)]
struct Pixel {
    dont_move: bool,
    pixel_type: PixelType,
}

const X_EXTENT: f32 = 12.0;
const GRID_SIZE: f32 = 1.0;

fn spawn_cube(
    commands: &mut Commands,
    mut shape: Handle<Mesh>,
    materials: &mut ResMut<Assets<StandardMaterial>>,
    x: f32,
    y: f32,
    z: f32,
) {
    // let mut rng = rand::thread_rng();

    // commands.spawn((
    //     PbrBundle {
    //         mesh: shape,
    //         // material: debug_material.clone(),
    //         transform: Transform::from_xyz(
    //             rng.gen_range(-10..=10) as f32 * GRID_SIZE, //-X_EXTENT,// / 2. + 0 as f32 / (1 - 1) as f32 * X_EXTENT,
    //             10.0,
    //             rng.gen_range(-10..=10) as f32 * GRID_SIZE,
    //         ),
    //         //.with_rotation(Quat::from_rotation_x(-PI / 4.)),
    //         ..default()
    //     },
    //     Shape,
    // ))
    //     .insert(GravityScale(0.1))
    //     .insert(Collider::cuboid(0.5, 0.5, 0.5));

    commands // .spawn(RigidBody::Fixed)
        .spawn((PbrBundle {
            mesh: shape,
            material: materials.add(Color::NONE),
            transform: Transform::from_xyz(
                x,//rng.gen_range(-50..=50) as f32 * GRID_SIZE, //-X_EXTENT,// / 2. + 0 as f32 / (1 - 1) as f32 * X_EXTENT,
                y,//10.0,
                z//rng.gen_range(-50..=50) as f32 * GRID_SIZE,
            ),
            ..default()
        }, Pixel {
            dont_move: false,
            pixel_type: PixelType::Water,
        }));
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    // mut images: ResMut<Assets<Image>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    // let debug_material = materials.add(StandardMaterial {
    //     base_color_texture: Some(images.add(uv_debug_texture())),
    //     ..default()
    // });

    let shapes = [
    meshes.add(Cuboid {
        half_size: Vec3 {
            x: 0.5,
            y: 0.5,
            z: 0.5,
        }
    }),
        // meshes.add(Capsule3d::default()),
        // meshes.add(Torus::default()),
        // meshes.add(Cylinder::default()),
        // meshes.add(Sphere::default().mesh().ico(5).unwrap()),
        // meshes.add(Sphere::default().mesh().uv(32, 18)),
    ];

    //let num_shapes = shapes.len();

    for (i, shape) in shapes.into_iter().enumerate() {
        for a in 0..20 {
            for b in 5..10 {
                for c in 0..20 {
                    spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32);
                    // println!("IDK: {a} {b} {c}");
                }
            }
        }
    }


    commands.spawn(PointLightBundle {
        point_light: PointLight {
            shadows_enabled: false,
            intensity: 300_000_000.,
            range: 100.0,
            color: Color::WHITE,
            ..default()
        },
        transform: Transform::from_xyz(0.0, 50.0, 0.0),
        ..default()
    });

    // ground plane
    commands.spawn(PbrBundle {
        mesh: meshes.add(Plane3d::default().mesh().size(100.0, 100.0)),
        material: materials.add(Color::WHITE),
        transform: Transform::from_xyz(0.0, -0.5, 0.0),
        ..default()
    });


    // commands.spawn(Collider::cuboid(0.5, 0.5, 0.5));

    // commands.spawn(Camera3dBundle {
    //     transform: Transform::from_xyz(0.0, 6., 12.0).looking_at(Vec3::new(0., 1., 0.), Vec3::Y),
    //     ..default()
    // });

    commands.spawn((
        Camera3dBundle {
            transform: Transform::from_translation(Vec3::new(60.0, 60.0, 60.0)),
            ..default()
        },
        PanOrbitCamera::default(),
    ));
}

fn fix_y(
    mut transforms: Query<&mut Transform, With<Pixel>>,
) {
    for mut transform in &mut transforms {
        // transform.translation.y = (transform.translation.y as i32) as f32;
        if transform.translation.y < 1.0 {
            transform.translation.y = 1.0;
        }

        if transform.translation.y > 50.0 {
            transform.translation.y = 50.0;
        }
    }
}

/// Creates a colorful test pattern
fn uv_debug_texture() -> Image {
    const TEXTURE_SIZE: usize = 8;

    let mut palette: [u8; 32] = [
        255, 102, 159, 255, 255, 159, 102, 255, 236, 255, 102, 255, 121, 255, 102, 255, 102, 255,
        198, 255, 102, 198, 255, 255, 121, 102, 255, 255, 236, 102, 255, 255,
    ];

    let mut texture_data = [0; TEXTURE_SIZE * TEXTURE_SIZE * 4];
    for y in 0..TEXTURE_SIZE {
        let offset = TEXTURE_SIZE * y * 4;
        texture_data[offset..(offset + TEXTURE_SIZE * 4)].copy_from_slice(&palette);
        palette.rotate_right(4);
    }

    Image::new_fill(
        Extent3d {
            width: TEXTURE_SIZE as u32,
            height: TEXTURE_SIZE as u32,
            depth_or_array_layers: 1,
        },
        TextureDimension::D2,
        &texture_data,
        TextureFormat::Rgba8UnormSrgb,
        RenderAssetUsages::RENDER_WORLD,
    )
}
