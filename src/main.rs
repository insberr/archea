use std::f32::consts::PI;
use bevy::pbr::CascadeShadowConfigBuilder;
// use rand::prelude::*;
use bevy::prelude::*;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};

// Why
mod engine;
use engine::systems::sideways_movement::*;
use crate::engine::systems::update_pixel_color::update_pixel_color;

fn main() {
    App::new()
        .add_plugins(DefaultPlugins.set(ImagePlugin::default_nearest()))
        .add_plugins(PanOrbitCameraPlugin)

        .insert_resource(Time::<Fixed>::from_seconds(0.08))

        .add_systems(Startup, setup)

        // .add_systems(FixedUpdate, collision_system)
        // .add_systems(FixedUpdate, fix_y)
        .add_systems(FixedUpdate, sideways_movement)

        .add_systems(FixedPostUpdate, update_pixel_color)
        .add_systems(Update, check_y)

        .run();
}

fn check_y(
    mut commands: Commands,
    parents_query: Query<Entity, With<Pixel>>,
    mut transform_query: Query<&mut Transform, With<Pixel>>,
) {
    for parent in &parents_query {
        if let Ok(mut transform) = transform_query.get_mut(parent) {
            // transform.rotate_z(-PI / 2. * time.delta_seconds());
            if (transform.translation.y < 0.0) {
                commands.entity(parent).despawn();
            }
        }
    }
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

#[derive(Component)]
struct Pixel {
    dont_move: bool,
    pixel_type: PixelType,
}
#[derive(Component)]
struct PixelSand;
#[derive(Component)]
struct PixelWater;
#[derive(Component)]
struct PixelLava;
#[derive(Component)]
struct PixelUnmoveable;


// const GRID_SIZE: f32 = 1.0;

fn spawn_cube(
    commands: &mut Commands,
    shape: Handle<Mesh>,
    materials: &mut ResMut<Assets<StandardMaterial>>,
    x: f32,
    y: f32,
    z: f32,
) {
    // let mut rng = rand::thread_rng();

    commands
        .spawn(PbrBundle {
            mesh: shape,
            material: materials.add(Color::NONE),
            transform: Transform::from_xyz(
                x,//rng.gen_range(-50..=50) as f32,
                y,//10.0,
                z//rng.gen_range(-50..=50) as f32,
            ),
            ..default()
        })
        .insert(Pixel {
            dont_move: false,
            pixel_type: PixelType::Water,
        })
        .insert(PixelWater);
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    let shapes = [
        meshes.add(Cuboid {
            half_size: Vec3 {
                x: 0.5,
                y: 0.5,
                z: 0.5,
            }
        }),
    ];

    for shape in shapes.into_iter() {
        for a in 0..10 {
            for b in 20..30 {
                for c in 0..10 {
                    spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32);
                }
            }
        }
    }

    // Create the light
    // commands.spawn(PointLightBundle {
    //     point_light: PointLight {
    //         shadows_enabled: false,
    //         intensity: 300_000_000.,
    //         range: 100.0,
    //         color: Color::WHITE,
    //         ..default()
    //     },
    //     transform: Transform::from_xyz(0.0, 50.0, 0.0),
    //     ..default()
    // });

    // directional 'sun' light
    commands.spawn(DirectionalLightBundle {
        directional_light: DirectionalLight {
            illuminance: light_consts::lux::OVERCAST_DAY,
            shadows_enabled: true,
            ..default()
        },
        transform: Transform {
            translation: Vec3::new(0.0, 2.0, 0.0),
            rotation: Quat::from_rotation_x(-PI / 4.),
            ..default()
        },
        // The default cascade config is designed to handle large scenes.
        // As this example has a much smaller world, we can tighten the shadow
        // bounds for better visual quality.
        cascade_shadow_config: CascadeShadowConfigBuilder {
            first_cascade_far_bound: 4.0,
            maximum_distance: 10.0,
            ..default()
        }
            .into(),
        ..default()
    });


    // Create the ground plane
    commands.spawn(PbrBundle {
        mesh: meshes.add(Plane3d::default().mesh().size(100.0, 100.0)),
        material: materials.add(Color::WHITE),
        transform: Transform::from_xyz(0.0, -0.5, 0.0),
        ..default()
    });

    // Spawn Camera
    commands.spawn((
        Camera3dBundle {
            transform: Transform::from_translation(Vec3::new(60.0, 60.0, 60.0)),
            ..default()
        },
        PanOrbitCamera::default(),
    ));
}
