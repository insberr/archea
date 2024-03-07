use std::f32::consts::PI;
use bevy::pbr::CascadeShadowConfigBuilder;
// use rand::prelude::*;
use bevy::prelude::*;
use bevy_mod_picking::prelude::*;
use bevy_panorbit_camera::{PanOrbitCamera, PanOrbitCameraPlugin};

// Why
mod engine;
use engine::systems::sand_movement::*;
use engine::systems::water_movement::*;
use crate::engine::systems::update_pixel_color::update_pixel_color;

struct PixelTransform {
    transform: Transform,
    entity: Entity,
}

#[derive(Resource, Default)]
struct PixelPositions {
    positions: Vec<PixelTransform>,
}

fn main() {
    App::new()
        .add_plugins(DefaultPlugins.set(ImagePlugin::default_nearest()))
        .add_plugins(PanOrbitCameraPlugin)
        .add_plugins(DefaultPickingPlugins)
        .insert_resource(DebugPickingMode::Normal)

        .init_resource::<PixelPositions>()

        .insert_resource(Time::<Fixed>::from_seconds(0.08))

        .add_systems(Startup, setup)

        .add_systems(FixedPreUpdate, update_transforms_list)
        // .add_systems(FixedUpdate, collision_system)
        // .add_systems(FixedUpdate, fix_y)
        // .add_systems(FixedUpdate, sideways_movement)

        .add_systems(FixedUpdate, sand_movement)
        .add_systems(FixedUpdate, water_movement)

        .add_systems(FixedPostUpdate, update_pixel_color)
        .add_systems(Update, check_destroy)

        .run();
}


fn update_transforms_list(
    mut pixel_transforms: ResMut<PixelPositions>,
    pixels: Query<(&Transform, &Pixel)>
) {
    pixel_transforms.positions.clear();
    for (tr, pix) in pixels.iter() {
        pixel_transforms.positions.push(PixelTransform {
            transform: tr.clone(),
            entity: pix.id,
        });
    }
}

fn check_destroy(
    mut commands: Commands,
    parents_query: Query<Entity, With<Pixel>>,
    mut pixel_query: Query<&mut Pixel>,
) {
    // For each entity with the Pixel component
    for parent in &parents_query {
        // Pull out the Pixel component
        if let Ok(mut pixel) = pixel_query.get_mut(parent) {
            // transform.rotate_z(-PI / 2. * time.delta_seconds());
            // if (transform.translation.y < 0.0) {
            if pixel.destroy {
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
    destroy: bool,
    pixel_type: PixelType,
    id: Entity,
}
impl Pixel {
    pub fn destroy(&mut self) {
        self.destroy = false;
    }
}

#[derive(Component)]
struct PixelSand;
#[derive(Component)]
struct PixelWater;
#[derive(Component)]
struct PixelUnmovable;



// const GRID_SIZE: f32 = 1.0;

fn spawn_cube(
    commands: &mut Commands,
    shape: Handle<Mesh>,
    materials: &mut ResMut<Assets<StandardMaterial>>,
    x: f32,
    y: f32,
    z: f32,
    pixel_type: PixelType,
) -> Entity {
    // let mut rng = rand::thread_rng();

    let entity = commands
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
        // .insert(Pixel {
        //     dont_move: false,
        //     pixel_type: pixel_type,
        //     destroy: false,
        // })
        // .insert(PixelSand)
        // Despawn an entity when clicked:
        .insert(On::<Pointer<Click>>::target_commands_mut(|_click, target_commands| {
            target_commands.despawn();
        }))
        // Optional: adds selection, highlighting, and helper components.
        .insert(PickableBundle::default()).id();

    commands.entity(entity).insert(Pixel {
        dont_move: false,
        pixel_type: pixel_type,
        destroy: false,
        id: entity,
    });

    return entity;
}

fn setup(
    mut commands: Commands,
    mut meshes: ResMut<Assets<Mesh>>,
    mut materials: ResMut<Assets<StandardMaterial>>,
) {
    commands.insert_resource(PixelPositions {
        positions: Vec::new(),
    });

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
                    if a % 2 == 0 {
                        let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Sand);
                        commands.entity(entity).insert(PixelSand);
                    } else {
                        let entity = spawn_cube(&mut commands, shape.clone(), &mut materials, a as f32, b as f32, c as f32, PixelType::Water);
                        commands.entity(entity).insert(PixelWater);
                    }
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
