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
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};

// Why
mod engine;
use engine::systems::gravity_system;
use engine::systems::collision_system;

fn main() {
    App::new()
        .add_plugins(DefaultPlugins.set(ImagePlugin::default_nearest()))
        .add_plugins(PanOrbitCameraPlugin)
        // .add_plugins(RapierPhysicsPlugin::<NoUserData>::default())
        // .add_plugins(RapierDebugRenderPlugin::default())
        .add_systems(Startup, setup)
        // .add_systems(Update, gravity)
        .add_systems(Update, (collision_system::collision_system, gravity_system::gravity_system, fix_y).chain())
        .run();
}
//needa clean up my code

/// A marker component for our shapes so we can query them separately from the ground plane
#[derive(Component)]
struct Pixel {
    dont_move: bool,
    transform: Transform,
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
    let mut rng = rand::thread_rng();

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
            material: materials.add(Color::ORANGE),
            transform: Transform::from_xyz(
                x,//rng.gen_range(-50..=50) as f32 * GRID_SIZE, //-X_EXTENT,// / 2. + 0 as f32 / (1 - 1) as f32 * X_EXTENT,
                y,//10.0,
                z//rng.gen_range(-50..=50) as f32 * GRID_SIZE,
            ),
            ..default()
        }, Pixel { dont_move: false, transform: Transform::from_xyz(
            x,//rng.gen_range(-50..=50) as f32 * GRID_SIZE, //-X_EXTENT,// / 2. + 0 as f32 / (1 - 1) as f32 * X_EXTENT,
            y,//10.0,
            z//rng.gen_range(-50..=50) as f32 * GRID_SIZE,
            ) }));
        // .insert(TransformBundle::from();
        // .insert(Velocity {
        //     linvel: Vec3::new(0.0, 0.0, 0.0),
        //     angvel: Vec3::new(0.0, 0.0, 0.0),
        // })
        // .insert(GravityScale(9.8))
        // .insert(Sleeping::default())
        // .insert(Ccd::disabled())
        // .insert(Collider::cuboid(0.4, 0.5, 0.4))
        // .insert(Friction::coefficient(10.0))
        // .insert(Restitution {
        //     coefficient: 0.0,
        //     combine_rule: CoefficientCombineRule::Min
        // })
        // .insert(ColliderMassProperties::Mass(1.0))
        // .insert(LockedAxes::TRANSLATION_LOCKED_X | LockedAxes::TRANSLATION_LOCKED_Z | LockedAxes::ROTATION_LOCKED_Y | LockedAxes::ROTATION_LOCKED_X | LockedAxes::ROTATION_LOCKED_Z);
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
            x: 0.4,
            y: 0.4,
            z: 0.4,
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
        for a in 0..10 {
            for b in 5..10 {
                for c in 0..5 {
                    spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32);
                    // println!("IDK: {a} {b} {c}");
                }
            }
        }
    }


    commands.spawn(PointLightBundle {
        point_light: PointLight {
            shadows_enabled: false,
            intensity: 90_000_000.,
            range: 100.0,
            color: Color::YELLOW_GREEN,
            ..default()
        },
        transform: Transform::from_xyz(8.0, 10.0, 8.0),
        ..default()
    });

    // The ground
    commands
        .spawn(Collider::cuboid(60.0, 1.0, 60.0))
        .insert(TransformBundle::from(Transform::from_xyz(0.0, -0.5, 0.0)));

    // ground plane
    commands.spawn(PbrBundle {
        mesh: meshes.add(Plane3d::default().mesh().size(100.0, 100.0)),
        material: materials.add(Color::SILVER),
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
