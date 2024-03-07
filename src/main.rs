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

        .insert_resource(Time::<Fixed>::from_seconds(0.07))

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

#[derive(Component)]
struct Pixel {
    dont_move: bool,
    pixel_type: PixelType,
}

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
        .spawn((PbrBundle {
            mesh: shape,
            material: materials.add(Color::NONE),
            transform: Transform::from_xyz(
                x,//rng.gen_range(-50..=50) as f32,
                y,//10.0,
                z//rng.gen_range(-50..=50) as f32,
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
        for a in 0..20 {
            for b in 5..10 {
                for c in 0..20 {
                    spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32);
                }
            }
        }
    }

    // Create the light
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
